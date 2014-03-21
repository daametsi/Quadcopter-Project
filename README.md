The following projects will be comprised into a single project
which controls and interfaces with systems incorporated on
a quadcopter.

 * AccelProj(uses MPU6050)
 * 
INTERFACE TECNOLOGIES: I2C
Demonstrates the ability to interface with the accelerometer
module by initializing and pulling values from the registers.

 * CamProj(uses OV7670 w/ FIFO)
 * 
INTERFACE TECHNOLOGIES: I2C
Demonstrates the ability to interface with the camera module by
initializing camera modes and constructing a picture from the data
pins.

AccelProj
Directions:

1. Make and run program


CamProj
Directions:

1. Disable HDMI on the capemanager
 For this project HDMI is uneeded and currently used as a partial header for the OV7670 camera for debugging convience etc.
  I. locate uEnv.txt
    enter 'vim /boot/uEnv.txt'

  II. modify contents
    search for 'optargs' and insert/add the following:
      optargs=quiet drm.debug=7 capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN
      
  III. verify
    enter 'cat /sys/devices/bone_capemgr.*/slots'
    all disabled devices should be missing an L in the following sequence "P-O-L" and should appear something like this "P-O--"

2. Make and run program
