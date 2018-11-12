#ifndef BALLTRACKER_H
#define BALLTRACKER_H

#include <thread>

class BallTracker
{
public:
    BallTracker();

private:
    void ballTracking();
    bool hasFound();

    bool found;
    std::thread trackingThread;
};

#endif // BALLTRACKER_H
