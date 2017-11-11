## Code for a The Things Network (LoRa) Gateway

**Original Code From:** https://github.com/ernstdevreede/lmic_pi

### Build
```
make clean
make

```

### Run
Use the bash script to start the lora ttn sender: `./send_to_ttn.sh <data>` the *<data>* string is sent to the The Things Network.
```
./send_to_ttn.sh <data>
```

### Modifications to the original code
- The *thethingsnetwork-send-v1* example code was used as base
- The code was modified to allow sending of one 'data item' from a python script

## lora sender setup
- delete the `ASSERT` call on line 630 in 'radio.c' because this assert sometimes fails.
- duplicate the function `os_runloop()` in 'oslmic.c', name it `os_runloop_once()` and remove the endless loop to get controll over the program run time.
- use `os_runloop_once()` inside a loop in the main method and end the loop when the data was sent

### Problems
- The sent data does not reach the gateway every time.
