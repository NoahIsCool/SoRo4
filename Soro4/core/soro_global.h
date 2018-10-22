#ifndef SORO_GLOBAL_H
#define SORO_GLOBAL_H

namespace videoStreamer{

const int CONTROL_PORT          = 6969;
const int CONTROL_CLIENT_PORT	= 6970;
const int HEARTBEAT_PORT        = 6971;
const int HEARTBEAT_CLIENT_PORT = 6972;
const int CAMERA_PORT_BASE      = 7000;

const int PARAMETRIC_CAMERA     = 0;
const int AERIAL_CAMERA         = 1;
const int CLAW_CAMERA           = 2;
const int ELBOW_CAMERA          = 3;

};

#endif // SORO_GLOBAL_H
