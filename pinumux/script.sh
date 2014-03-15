dtc -O dtb -o BB-GPIO-DATA-00A0.dtbo -b 0 -@ BB-GPIO-DATA-00A0.dts
cp ./BB-GPIO-DATA-00A0.dtbo /lib/firmware
echo BB-GPIO-DATA > /sys/devices/bone_capemgr.9/slots
