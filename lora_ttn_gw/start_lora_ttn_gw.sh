#!/bin/bash
#
# this script starts the lora thethingsnetwork gateway
# and restarts it if it crashes
#
# autostart this script on boot:
# sudo vi /etc/rc.local
# add line: /path/to/lora_ttn_gw/start_lora_ttn_gw.sh &
#
##########

# get directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

until $DIR/single_chan_pkt_fwd/single_chan_pkt_fwd; do
    echo "'single_chan_pkt_fwd' crashed with exit code $?.  Respawning.." >&2
    sleep 1
done
