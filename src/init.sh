#!/bin/bash

export PATH=$PATH:../xtensa-lx106-elf/bin/
make clean
make SDK_BASE="../esp_iot_sdk_v1.5.2" FLAVOR="release" all
