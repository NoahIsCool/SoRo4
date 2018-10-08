/**********************************************************************
 *
 * Filename:    Socket.cpp
 *
 * Description: Implementations of Socket class
 *
 * Notes:
 *
 *
 * Copyright (c) 2018 by Kelly Klein.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#include "Socket.h"

using namespace useful;
using namespace logger;

Socket::Socket(int port){
    crcInit();
    sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sock == -1){
        std::cout << "Could not create socket" << std::endl;
        exit(1);
    }

    server.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        std::cout << "Connection error" << std::endl;
        exit(1);
    }
    messageThread = std::thread([this](){
        while(running){
            checkForMessage();
            //std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    });
}

int Socket::available(){
    return pendingMessages.size() > 0 ? true : false;
}

bool Socket::getMessage(Message *message,std::string &sender,int &port){
    if(!available()){
        return false;
    }
    readingMutex.lock();
    StoredPacket packet = pendingMessages.front();
    memcpy(message,&packet.message,sizeof(Message));
    sender = packet.sender;
    port = packet.port;
    pendingMessages.pop_front();

    readingMutex.unlock();
    return checkCRC(message);
}

void Socket::checkForMessage(){
    struct sockaddr_in client;
    int length = sizeof(client);
    uint8_t *buffer = new uint8_t[sizeof(Message)];
    //this method is blocking
    int bytes_read = recvfrom(sock,buffer,sizeof(Message),0,(struct sockaddr *)&client, (socklen_t *)&length);
    if(bytes_read > 0){
        Message message;
        std::string sender;
        int port;

        //memcpy(&message,buffer,bytes_read);
        sender = inet_ntoa(client.sin_addr);
        port = ntohs(client.sin_port);
        StoredPacket packet;
        memcpy(&packet.message,buffer,bytes_read);
        packet.sender = sender;
        packet.port = port;
        packet.timestamp = std::chrono::high_resolution_clock::now();
        if(packet.message.type == LAG){
            if(packet.message.data[0] == PING){
                int lagPort = (packet.message.data[1] << 8) + packet.message.data[2];
                packet.message.data[0] = PONG;
                write(&packet.message,packet.sender,lagPort);
            }else{
                lagTestingMessages.push(packet);
            }
        }else{
            pendingMessages.emplace_back(packet);
        }
    }else if(bytes_read == 0){
        //std::cout << "read 0 bytes" << std::endl;
        //socket closed...
    }else if(bytes_read == -1){
        std::cout << "got an error? " << std::endl;
        //some error...probably need to ask for a resend
    }
}

void Socket::write(Message *message,std::string address,int port){
    writingMutex.lock();

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(address.c_str());
    dest.sin_port = htons(port);

    generateCRC(message);

    int total = 0;
    int bytesLeft = sizeof(Message);//sizeof(message);
    int n;

    while(total < bytesLeft){
        n = sendto(sock,message,sizeof(Message),0,(struct sockaddr *)&dest,sizeof(dest));
        if(n == -1){break;}
        total += n;
        bytesLeft -= n;
    }

    writingMutex.unlock();
}

bool Socket::checkCRC(Message *message){
    int size = sizeof(Message) - sizeof(uint32_t);
    uint32_t crc = crcFast(reinterpret_cast<const unsigned char*>(message), size);
    if(crc == message->crc){
        return true;
    }else{
        return false;
    }
}

void Socket::generateCRC(Message *message){
    int size = sizeof(Message) - sizeof(uint32_t);
    uint32_t crc = crcFast(reinterpret_cast<const unsigned char*>(message), size);
    message->crc = crc;
}

double Socket::checkLag(int sampleSize,std::string address,int port,uint16_t receivePort){
    double sum = 0;
    for(int i = 0; i < sampleSize; i++){
        Message test;
        test.type = LAG;
        test.length = 3;
        test.data[0] = PING;
        test.data[1] = receivePort >> 8;
        test.data[2] = (uint8_t)receivePort;
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        write(&test,address,port);
        while(lagTestingMessages.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        StoredPacket message = lagTestingMessages.front();
        lagTestingMessages.pop();

        std::chrono::duration<double, std::milli> span = message.timestamp - t1;
        //lag time is just time it takes to get from the sender to the reciever
        //about 1/2 the time...
        sum += (span.count() / 2);
    }

    return sum / sampleSize;
}

Socket::~Socket(){
    shutdown(sock,SHUT_RDWR);
    close(sock);
    running = false;
    messageThread.join();
}
