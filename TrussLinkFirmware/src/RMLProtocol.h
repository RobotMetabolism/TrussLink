#pragma once

#include "utility.h"

// Global Variables
// Hardcoded protocol information (maybe spin off into a separate file)

#define MSG_HDR_LENGTH 2        // [message_length (1 byte) + msg_type (1 byte)]
#define MSG_FTR_LENGTH 2        // CRC-15 (unsigned short)
#define MSG_HELLO_LENGTH 2
#define MSG_POSITION_LENGTH 4
#define MSG_UPDATE_LENGTH 10
#define MSG_CALIBRATION_LENGTH 24
#define MSG_SIN_LENGTH 14
#define MSG_WALK_LENGTH 1
#define MSG_EPOCH_LENGTH 8

#define MSG_LIST_MAX_LENGTH 255

#define SERVER_IP {192,168,0,15}
#define SERVER_PORT 54657
#define TIMEOUT_INITIAL_RESPONSE 20000
#define TIMEOUT_RESPONSE_READ 5000
#define RECONNECT_DELAY 3000


// use std::memcpy(&my_struct_instance, buffer, sizeof(my_struct_instance));
// to copy data from the data buffer into your struct instance
// (use ntohl to fix endian byte ordering)


union MSG_hdr{
    struct HDR_DATA{
        unsigned char length; // length of body only
        char type;
    }get;
    uint8_t bytes[MSG_HDR_LENGTH];
} msg_hdr;


union MSG_ftr{
    unsigned short checksum_bytes;
    uint8_t bytes[MSG_FTR_LENGTH];
} msg_ftr;


union MSG_hello{
    struct MSG_HELLO_DATA{
        unsigned char device_id;
        unsigned char MAX_VEL; // MAX_VEL * 10
    } get;
    uint8_t bytes[MSG_HELLO_LENGTH];
} msg_hello;


union MSG_position{
    struct MSG_POSITION_DATA{
        unsigned char srv0_pos;
        unsigned char srv1_pos;
        unsigned char srv0_vel; // 0 - MAX_VEL*10[mm/s]
        unsigned char srv1_vel; // 0 - MAX_VEL*10[mm/s]
    }get;
    uint8_t bytes[MSG_POSITION_LENGTH];
} msg_position;


union MSG_update{
    struct MSG_UPDATE_DATA{
        char device_status;
        unsigned char srv0_pos; // position in mm [0 - STROKE_LENGTH, STROKE_LENGTH - 100mm]
        unsigned char srv1_pos;
        unsigned char srv0_raw;
        unsigned char srv1_raw;
        unsigned char bat_status;
        unsigned char srv0_vel; // 0 - MAX_VEL*10[mm/s]
        unsigned char srv1_vel; // 0 - MAX_VEL*10[mm/s]
        unsigned short current_command_checksum;
    } get;
    uint8_t bytes[MSG_UPDATE_LENGTH];
} msg_update;


union MSG_calibration_conf {
    struct SRV_CALIBRATE_DATA {
        uint8_t version;
        unsigned short srv0_min_ms, srv0_max_ms; // PWM value min/max that results in servo position change
        unsigned short srv1_min_ms, srv1_max_ms;  // same as above for servo 1
        unsigned short srv0_raw_min, srv0_raw_max; // raw readout position feedback value at fully contracted/expanded location
        unsigned short srv1_raw_min, srv1_raw_max; // same as above for servo 1
    } get;
    uint8_t bytes[MSG_CALIBRATION_LENGTH];
} msg_calibration_conf;


union MSG_sin{
    struct MSG_SIN_DATA{

        unsigned short start_time;  // 2 bytes - seconds relative to SERVER_EPOCH

        unsigned char a0;           // Amplitude (scales the function)
        unsigned char x0;           // base-position
        short phase_shift_0;        // milliseconds
        unsigned short period_0;    // milliseconds for complete

        unsigned char a1;
        unsigned char x1;
        short phase_shift_1;
        unsigned short period_1;

    } get;
    uint8_t bytes[MSG_SIN_LENGTH];
} msg_sin;

union MSG_walk{
    struct MSG_WALK_DATA{
        char nr_steps; // -128 (backwards) to 127 (forwards)
    }get;
    uint8_t bytes[MSG_WALK_LENGTH];
} msg_walk;

union MSG_epoch{
    struct MSG_TIME_DATA{
        time_t server_start; // server start time
    } get;
    uint8_t bytes[MSG_EPOCH_LENGTH];
} msg_epoch;

// List structs

union MSG_list_subheader{
    struct MSG_LIST_SUBHEADER{
        unsigned char duration;    // seconds
        char type;
    } get;
    uint8_t bytes[2];
} msg_list_subheader;

union MSG_list_actual{
    struct MSG_LIST_ACTUAL{
        short nr_repeat;
        unsigned short start_time;
    } get;
    uint8_t bytes[MSG_LIST_MAX_LENGTH];
} msg_list_actual;

////////////////////////////////////////// CRC 15 Checksum //////////////////////////////////////////

// Credit to https://github.com/hiharin/snappro_xboot/blob/master/board/dm3730logic/prod-id/crc-15.c

void crc_15_step(unsigned short *crc, unsigned char byte)
{
	int i;
	unsigned short crcnext;

	for (i=0; i<7; ++i) {
		crcnext = (byte & 1) ^ (*crc>>14);
		*crc = (*crc << 1) & 0x7fff;
		if (crcnext)
			*crc ^= 0x4599;
		byte >>= 1;
	}
}

unsigned short crc_15(void *buf, int len)
{
	unsigned char *p = (unsigned char *) buf;
	unsigned short xsum = 0;
	int i;

	for (i=0; i<len; ++i) {
		crc_15_step(&xsum, p[i]);
	}
	return xsum;
}

unsigned short crc_15_footer(void *header, void *message, int len){
	unsigned char *p = (unsigned char *) header;
	unsigned short xsum = 0;
	int i;
	for (i = 0; i < MSG_HDR_LENGTH; ++i)
		crc_15_step(&xsum, p[i]);

	unsigned char *q = (unsigned char *) message;

	for (i = 0; i < len; ++i)
		crc_15_step(&xsum, q[i]);

	return xsum;
}