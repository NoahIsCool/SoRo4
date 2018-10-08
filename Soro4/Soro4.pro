TEMPLATE = subdirs

SUBDIRS += \
    autonomous \
    driveSystem \
    slaveArm \
    driveController \
    masterArm \
    RoverMain \
    Rover_MissionControl \
    core

CONFIG += ordered

RoverMain.depends = autonomous slaveArm driveSystem
RoverMain.file = RoverMain/RoverMain.pro
autonomous.file = autonomous/autonomous.pro
slaveArm.file = slaveArm/slaveArm.pro
driveSystem.file = driveSystem/driveSystem.pro

Rover_MissionControl.depends = driveController masterArm
masterArm.file = masterArm/masterArm.pro
driveController.file = driveController/driveController.pro
Rover_MissionControl.file = Rover_MissionControl/Rover_MissionControl.pro
