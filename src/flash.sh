tools/esptool/esptool.py -p /dev/tty.SLAB_USBtoUART write_flash 0x00000 firmware/0x00000.bin 0x40000 firmware/0x40000.bin
screen /dev/tty.SLAB_USBtoUART 115200
