/**********************************************************************
 *
 * Filename:    Socket.h
 *
 * Description: A header file describing the various Socket methods
 *
 * Notes:
 *
 *
 * Copyright (c) 2018 by Kelly Klein.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#ifndef Socket_h
#define Socket_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <mutex>
#include <useful.h>
#include <deque>
#include <queue>
#include "crc.h"

//not proud of this but I cant get it to use dynamic packets just yet so if you know your packets
//are going to be larger than 512 bytes, then just increase this variable to something that will work for you
#define MAX_SIZE 512
#define LOG_TAG "SOCKET"

/*
    The socket class is just a simple c++ wrapper for c sockets. The basic functionallity of it is:
    Create the socket
    Send the message
    listen for message
    copy message
    I thought the best way to handle this is to allow the system to check for messages at its leasure.
    The Socket has a queue that holds the messages it recieves until the system is ready to view them.

    I also found for my projects that it is easier to test lag within this class. So there is a method
    to test the lag between 2 sockets. This only works with 2 of my sockets.

    Also, this class is thread safe. It uses mutexes to ensure that only one message is sent at a time and
    prevents multiple threads from trying to send a message at the same time.

    Something important, I did not have time to do the crc myself. Maybe one day I will make one but for
    now, I just coppied some code I found from Michael Barr. So thanks man, your code works great.
*/

//here are some basic message types for you
//to add more, just make sure that the message type
//you make starts at 6 or ERROR+1
enum MessageType : uint8_t{
    INIT = 0,
    HEARTBEAT,
    DATA,
    ACK,
    NACK,
    LAG,
    ERROR,
};

enum LagMessageTypes : uint8_t{
    PING = 0,
    PONG,
};

struct Message{
    MessageType type;
    uint16_t length;
    uint8_t data[MAX_SIZE];
    uint32_t crc;
};

struct StoredPacket{
    Message message;
    std::string sender;
    int port;
    std::chrono::high_resolution_clock::time_point timestamp;
};

class Socket{
public:
    Socket(int port);
    ~Socket();
    //returns the number of messages available for reading
    int available();
    //returns if the crc checks out. It also copies the message, sender and port
    bool getMessage(Message *message, std::string &sender, int &port);
    //writes a new message to the given address and the given port
    void write(Message *message, std::string address,int port);
    //sends 10 messages back and forth and mesures the time it takes for the messages to get accross.
    //be warned. Do not try to send messages when this is going on. Weird shit will happen...
    double checkLag(int sampleSize,std::string address,int port,uint16_t receivePort);

private:
    void generateCRC(Message *message);
    bool checkCRC(Message *message);
    void checkForMessage();

    std::thread messageThread;
    bool running = true;
    std::deque<StoredPacket> pendingMessages;
    struct sockaddr_in server;
    std::mutex readingMutex;
    std::mutex writingMutex;
    int socket_info;
    int sock;
    bool testPending = false;
    std::queue<StoredPacket> lagTestingMessages;
};

#endif
