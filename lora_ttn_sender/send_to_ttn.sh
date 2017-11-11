#!/bin/bash
#
# The first argument to this script is a string with data that should be sent
#
# This script starts the lora_ttn_sender in the background
# and ensures that only one lora_ttn_sender process is running.
# If a lora_ttn_sender process is running, it is killed before
# a new one is started.

# get directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

LORA_TTN_SENDER="$DIR/lora_ttn_sender"

PID_FILE="/tmp/lora_ttn_sender.pid"

if ! [ $# -eq 1 ]; then
    echo "missing argument!"
    exit
fi

if ! [ -f "$LORA_TTN_SENDER" ]; then
    echo "$LORA_TTN_SENDER not found!"
    exit

fi

if [ -f "$PID_FILE" ] && kill -0 $(cat "$PID_FILE") > /dev/null 2>&1; then
    echo "another lora_ttn_sender process is already running!"

    kill -15 $(cat "$PID_FILE")

    sleep 1    #wait some time before checking if the process is really stopped

    if kill -0 $(cat "$PID_FILE") > /dev/null 2>&1; then
      echo "Failed to stop already running lora_ttn_sender process!"
      echo "lora_ttn_sender not started!"
      exit
    fi

    rm -rf "$PID_FILE" > /dev/null 2> /dev/null
fi

#start lora ttn sender in background and save PID of new process
$LORA_TTN_SENDER $1 > /dev/null 2> /dev/null & echo $! > "$PID_FILE"
