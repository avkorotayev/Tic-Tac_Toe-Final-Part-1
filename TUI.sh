#!/bin/bash

PORT="/dev/ttyUSB0"
BAUD=115200

echo "Connecting to ESP32 on $PORT..."
screen $PORT $BAUD
