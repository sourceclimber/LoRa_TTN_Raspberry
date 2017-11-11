/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello, world!", that
 * will be processed by The Things Network server.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1,
*  0.1% in g2).
 *
 * Change DEVADDR to a unique address!
 * See http://thethingsnetwork.org/wiki/AddressSpace
 *
 * Do not forget to define the radio type correctly in config.h, default is:
 *   #define CFG_sx1272_radio 1
 * for SX1272 and RFM92, but change to:
 *   #define CFG_sx1276_radio 1
 * for SX1276 and RFM95.
 *
 *******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <lmic.h>
#include <hal.h>
#include <local_hal.h>

// LoRaWAN NwkSKey, network session key
// Use this key for The Things Network
static const u1_t DEVKEY[16] = { 0xA1, 0x23, 0x52, 0x7B, 0x70, 0x07, 0x4C, 0x57, 0x75, 0x2D, 0xD4, 0x40, 0x92, 0xDC, 0xAA, 0x48 };

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
static const u1_t ARTKEY[16] = { 0xDD, 0xCD, 0x98, 0x4F, 0x9B, 0x5F, 0xE9, 0x23, 0x55, 0x36, 0x8B, 0xFA, 0x31, 0xC3, 0x5F, 0x2E };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t DEVADDR = 0x26011B3C ; // <-- Change this address for every node!

//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, DEVKEY, 16);
}

#define MAX_RUNTIME 50

#define MAX_DATA_LENGTH 100
u1_t dataToSend[MAX_DATA_LENGTH];
int dataToSend_length;

static osjob_t sendjob;

int running = 1;

// Pin mapping
lmic_pinmap pins = {
  .nss = 6,
  .rxtx = UNUSED_PIN, // Not connected on RFM92/RFM95
  .rst = 0,  // Needed on RFM92/RFM95
  .dio = {7,4,5}
};

void onEvent (ev_t ev) {
    //debug_event(ev);

    switch(ev) {
      // scheduled data sent (optionally data received)
      // note: this includes the receive window!
      case EV_TXCOMPLETE:
          // use this event to keep track of actual transmissions
          fprintf(stdout, "Event EV_TXCOMPLETE, time: %d\n", millis() / 1000);

          running = 0;
          break;
       default:
          break;
    }


}

static void do_exit(osjob_t* j){
running = 0;
}

static void do_send_once(osjob_t* j){
      time_t t=time(NULL);
      fprintf(stdout, "[%x] (%ld) %s try to send... \n", hal_ticks(), t, ctime(&t));
      // Show TX channel (channel numbers are local to LMIC)
      // Check if there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
      fprintf(stdout, "OP_TXRXPEND, not sending\n");

      os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(5), do_send_once);
    } else {

      if(dataToSend_length > MAX_DATA_LENGTH)
        dataToSend_length = MAX_DATA_LENGTH;

      LMIC_setTxData2(1, dataToSend, dataToSend_length, 0);

      fprintf(stdout, "waiting some time before exiting ...\n");

      os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(MAX_RUNTIME), do_exit);
    }
}


void setup() {
  // LMIC init
  wiringPiSetup();

  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
  LMIC_setSession (0x1, DEVADDR, (u1_t*)DEVKEY, (u1_t*)ARTKEY);
  // Disable data rate adaptation
  LMIC_setAdrMode(0);
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // Disable beacon tracking
  LMIC_disableTracking ();
  // Stop listening for downstream data (periodical reception)
  LMIC_stopPingable();
  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7,14);
  //
}


int main(int argc, char** argv) {

  if(argc != 2)
  {
    fprintf(stdout, "usage: %s <dataToSend>\n", argv[0]);
    return 1;
  }

  int i=0;
  while(argv[1][i]) {
    dataToSend[i]=argv[1][i];
    i++;
  }

  dataToSend[i]='\0';
  dataToSend_length = i;

  setup();

  os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(5), do_send_once);

  int maxRunTime_os = os_getTime()+sec2osticks(MAX_RUNTIME);

  while (running) {
    os_runloop_once();

    if(os_getTime() > maxRunTime_os)
    {
      fprintf(stdout, "max runtime reached!\n");
      break;
    }
  }
  return 0;
}
