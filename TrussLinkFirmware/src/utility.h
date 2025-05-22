#pragma once


// Dictionary to hold DEVICE_IDs
String ids[34] = {
    "22003a001047363336383437", // P0
    "340047000747363339343638", // P1
    "3b0064000f51373331333230", // P2
    "1e0024000147373334323233", // P3
    "400022001447363336383438", // P4
    "410023000f51373331333230", // P5
    "380033000f47363336383437", // P6
    "2f0030000c47313037363132", // P7
    "2f003c001847393035313138", // P8
    "330039000547393033373334", // P9
    "2d0022001847393035313137", // P10
    "22001b001747393035313137", // P11
    "3e002a001547393035313136", // P12
    "29001d001447393035313136", // P13
    "280034001847393035313137", // P14
    "410032000547393035313138", // P15
    "3a003e001947313037363132", // P16
    "31002b000d47303338353831", // P17
    "3c001b000d47303338353831", // P18
    "390025000447393035313138", // P19
    "2c003f001547313036303933", // P20
    "180038000d47313037363132", // P21
    "3d0037000c47393334363636", // P22
    "3a0037000947313037363132", // P23
    "2b001f000b47313037363132", // P24
    "39001a000d47303338353831", // P25
    "3b0032001547313036303933", // P26
    "1c0028000b47313037363132", // P27
    "2e0019001547313036303933", // P28
    "320037000847313037363132", // P29
    "2d0031000a47313037363132", // P30
    "1d0025000e47313037363132", // P31 
    "33003b001547313036303933", // P32 
    "200021001447313036303933" // P33 
};



time_t SERVER_EPOCH                     = 0;
int    SERVER_EPOCH_MILLIS              = 0;

// SECONDS TIME USING EPOCH
int SERVER_TIME(){
    return Time.now() - SERVER_EPOCH;
}

// MILLISECONDS TIME USING EPOCH
int SERVER_TIME_MILLIS(){
    return millis() - SERVER_EPOCH_MILLIS;
}

// Stores robot status
struct status_t{
        char device_status;
    unsigned char srv0_pos; // position in mm [0 - STROKE_LENGTH, STROKE_LENGTH - 100mm]
    unsigned char srv1_pos;
    unsigned char srv0_raw;
    unsigned char srv1_raw;
    unsigned char bat_stat;
    unsigned char srv0_vel; // 0 - MAX_VEL*10[mm/s]
    unsigned char srv1_vel; // 0 - MAX_VEL*10[mm/s]
} status;


// Checks device_status from status struct
bool deviceStatusIs(unsigned char cur_status){
    return (status.device_status == cur_status);
}


// Gives time for user to start screen
void wait_visual(int ms){
    Serial.printf("Wait %i ms\n", ms);
    for(int i = 0; i < ms/500;i++){
        Serial.print(".");
        delay(500);
    }
    Serial.print("\n");
}


// Converts id to corresponding number
int get_device_id(){
    String deviceID = System.deviceID();
    for(int i = 0; i < 256; i++){
        if(deviceID.compareTo(ids[i]) == 0)
            return i; // used to indexed at 1
    }
    return -1;
}


// Math absolute
int getAbsoluteValue(int x){
    if(x < 0)
        return -x;
    else
        return x;
}


// Math sign
int getSign(int x){
    if(x < 0)
        return -1;
    if(x > 0)
        return 1;
    return 0;
}

// Returns which value is closer to target
int getCloserTo(int target, int x, int y){
    if(getAbsoluteValue(target - x) < getAbsoluteValue(target  - y))
        return x;
    return y;
}

int getMax(int x, int y){
    return x < y ? y : x;
}

int getMin(int x, int y){
    return x < y ? x : y;
}