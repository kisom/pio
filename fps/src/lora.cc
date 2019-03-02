// LoRaWAN-specific functions.
#include <Arduino.h>
#include <Streaming.h>
#include <config.h>
#include <string.h>

#include <SPI.h>
#include <lmic.h>
#include <hal/hal.h>

#include <lora.h>
#include <gps.h>
#include <graphics.h>


struct Position	pos;
char		statusBuf[21];


const char	STATUS_INIT = '?';
const char	STATUS_JOINING = '.';
const char	STATUS_JOINED = '*';
const char	STATUS_JOINFAIL = '!';
const char	STATUS_TXACK = '+';
const char	STATUS_TXNOACK = '-';
const char	STATUS_TXDONE = '@';


static void
printStatus(char *status)
{
	Serial << status << endl;
	snprintf(statusBuf+1, 19, "%d:%s", os_getTime(), status);
	OLED::print(1, statusBuf);
}


////////////////////////
// LMIC configuration //
////////////////////////

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};

// OTAA setup for the Feather Positioning System.
static const u1_t	PROGMEM APPEUI[8] = { 
	0xE2, 0x81, 0x01, 0xD0, 0x7E, 0xD5, 0xB3, 0x70
};

void
os_getArtEui(u1_t* buf)
{
	memcpy_P(buf, APPEUI, 8);
}

static const u1_t	PROGMEM DEVEUI[8] = {
	0xC6, 0x08, 0x11, 0x00, 0x00, 0xB6, 0x76, 0x98
};
void
os_getDevEui(u1_t* buf)
{
	memcpy_P(buf, DEVEUI, 8);
}


static const u1_t PROGMEM APPKEY[16] = {
	0x23, 0xEE, 0x87, 0xCF, 0xF2, 0x8E, 0x3D, 0x16,
	0x62, 0x48, 0x9A, 0x61, 0x59, 0xC9, 0x8B, 0xFF
};
void
os_getDevKey(u1_t* buf)
{
	memcpy_P(buf, APPKEY, 16);
}


static osjob_t sendjob;

void
do_send(osjob_t* j)
{
	// Check if there is not a current TX/RX job running
	if (!(LMIC.opmode & OP_TXRXPEND)) {
		// Prepare upstream data transmission at the next possible time.
		if (!getPosition(pos)) {
			printStatus("NO GPS");
			return;
		}

		// Next TX is scheduled after TX_COMPLETE event.
		LMIC_setTxData2(1, (uint8_t *)(&pos), sizeof(pos), 1);
		printStatus("PKT QUEUED");
	}
	else {
		Serial << "PENDING TX" << endl;
	}
}


void
scheduled_job(osjob_t *j)
{
	LMIC_clrTxData();
	Serial << "scheduled job now running" << endl;
	do_send(j);
}


void
onEvent(ev_t ev)
{
	Serial << os_getTime()  << ":EVENT: ";
	switch(ev) {
	case EV_SCAN_TIMEOUT:
		printStatus("SCAN_TIMEOUT");
		break;
	case EV_BEACON_FOUND:
		printStatus("BEACON_FOUND");
		break;
	case EV_BEACON_MISSED:
		printStatus("BEACON_MISSED");
		break;
	case EV_BEACON_TRACKED:
		printStatus("BEACON_TRACKED");
		break;
	case EV_JOINING:
		statusBuf[0] = STATUS_JOINING;
		printStatus("JOINING");
		break;
	case EV_JOINED:
		statusBuf[0] = STATUS_JOINED;
		printStatus("JOINED");
		{
			u4_t netid = 0;
			devaddr_t devaddr = 0;
			u1_t nwkKey[16];
			u1_t artKey[16];
			LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
		}
		// Disable link check validation (automatically enabled
		// during join, but because slow data rates change max TX
		// size, we don't use it in this example.
		// LMIC_setLinkCheckMode(1);
		break;
        // This event is defined but not used in the code. No
        // point in wasting codespace on it.
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        
	case EV_JOIN_FAILED:
		statusBuf[0] = STATUS_JOINFAIL;
		printStatus("JOIN_FAILED");
		break;
	case EV_REJOIN_FAILED:
		statusBuf[0] = STATUS_JOINFAIL;
		printStatus("REJOIN_FAILED");
		break;
	case EV_TXCOMPLETE:
		if (LMIC.txrxFlags & TXRX_ACK) {
			statusBuf[0] = STATUS_TXACK;
			printStatus("TXCOMPLETE+ACK");
		}
		else {
			statusBuf[0] = STATUS_TXNOACK;
			printStatus("TXCOMPLETE");
		}
		// Schedule next transmission
		os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), scheduled_job);
		break;
	case EV_LOST_TSYNC:
		printStatus("LOST_TSYNC");
		break;
	case EV_RESET:
		printStatus("RESET");
		break;
	case EV_RXCOMPLETE:
		// data received in ping slot
		printStatus("RXCOMPLETE");
		break;
	case EV_LINK_DEAD:
		printStatus("LINK_DEAD");
		break;
	case EV_LINK_ALIVE:
		printStatus("LINK_ALIVE");
		break;
	// This event is defined but not used in the code. No
	// point in wasting codespace on it.
	case EV_SCAN_FOUND:
	   Serial.println(F("EV_SCAN_FOUND"));
	   break;
	case EV_TXSTART:
		printStatus("TXSTART");
		os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), scheduled_job);
		break;
	default:
		char	buf[21];
		sprintf(buf, "UNK EVT: %d", ev);
		OLED::print(1, buf);
		break;
	}
}


void
lmic_init()
{
	statusBuf[0] = STATUS_INIT;
	statusBuf[1] = 0;
	// LMIC init
	os_init();
	// Reset the MAC state. Session and pending data transfers will be discarded.
	LMIC_reset();

	LMIC_setLinkCheckMode(0);
	LMIC_setDrTxpow(DR_SF7,14);
	LMIC_selectSubBand(1);
}


const unsigned long	transmit_delay = 15000;
static int		nmsg = 0;


static bool
radioReady()
{
	return (!(LMIC.opmode & OP_TXRXPEND));
}


void
lmic_transmit()
{
	char			buf[21];
	static unsigned long	nextUpdate = 0;
	unsigned long		now = millis();

	if (nextUpdate > now) {
		return;
	}

	if (checkFix() && radioReady()) {
		sprintf(buf, "TX POS - %05d", ++nmsg);
		printStatus(buf);
		do_send(&sendjob);
		nextUpdate = now + transmit_delay;
	}
}
