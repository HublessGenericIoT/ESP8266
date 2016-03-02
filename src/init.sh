#!/bin/bash

export PATH=$PATH:/Volumes/case-sensitive/esp-open-sdk/xtensa-lx106-elf/bin/
make clean
make SDK_BASE="/Volumes/case-sensitive/esp-open-sdk/esp_iot_sdk_v1.5.2" FLAVOR="release" all
