TEMPLATE = subdirs

SUBDIRS += \
    core \
    autonomous \
    driveSystem \
    RoverMain \
    slaveArm \
    driveController \
    masterArm \
    #videoServer \
    Rover_MissionControl \
    #videoStreamer
    sciencePackage

CONFIG += ordered

RoverMain.depends = autonomous slaveArm driveSystem core #videoServer
RoverMain.file = RoverMain/RoverMain.pro
autonomous.file = autonomous/autonomous.pro
slaveArm.file = slaveArm/slaveArm.pro
driveSystem.file = driveSystem/driveSystem.pro

Rover_MissionControl.depends = driveController masterArm core
masterArm.file = masterArm/masterArm.pro
driveController.file = driveController/driveController.pro
Rover_MissionControl.file = Rover_MissionControl/Rover_MissionControl.pro
