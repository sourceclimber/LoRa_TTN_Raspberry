CFLAGS=-I./lmic
LDFLAGS=-lwiringPi

lora_ttn_sender: lora_ttn_sender.cpp
	cd lmic && $(MAKE)
	$(CC) $(CFLAGS) -o lora_ttn_sender lora_ttn_sender.cpp lmic/*.o $(LDFLAGS)
	chmod +x lora_ttn_sender

all: lora_ttn_sender

.PHONY: clean

clean:
	rm -f *.o lora_ttn_sender
	rm -f ./lmic*.o
