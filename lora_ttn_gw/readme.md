## Code to Send Data with LoRa to The Things Network

**Original Code From:** https://github.com/tftelkamp/single_chan_pkt_fwd

### Build
```
cd single_chan_pkt_fwd
make clean
make
```

### Run
The script `./start_lora_ttn_gw.sh` starts the gateway program and restarts it if it should crash.

```
./start_lora_ttn_gw.sh
```


### Add as Autostart:

The LoRa TTN gateway program can be set up to automatically start on boot.

- Edit the file */etc/rc.local*: `sudo vi /etc/rc.local`
- Add the following line (before `exit 0`):<br>
`/path/to/lora_ttn_gw/start_lora_ttn_gw.sh &`<br>
notice the **&** at the end!
