#pragma once

#include "utility.h"
#include "RMLProtocol.h"
#include <math.h>

#define SERVO_0_A A0
#define SERVO_0_D D0
#define SERVO_1_A A1
#define SERVO_1_D D1

#define POS_LOWERBOUND 0
#define POS_UPPERBOUND 100
#define SRV_UNRESPONSIVE_CUTOFF 7
#define MOVING_AVERAGE_PERIOD 16
#define EEPROM_CALIBRATION_ADDR 0
#define CALIBRATION_CHANGE_THRESHOLD 5
#define FORCE_CALIBRATION 0
#define MAX_SPEED 65                   // mm/s (ToDo: Calibrate from empirical data)
#define DETATCH_STROKE_FRACTION 0.25    // fraction of total stroke used by detach mechanism

#define APPROX_CALIBRATION_1000 4096
#define APPROX_CALIBRATION_2000 0


////////////////////////////////////// Primary Functions ////////////////////////////////////////

// initialize robot hardware struct 
struct device_t{
    String device_id_string = System.deviceID();
    unsigned char device_id = 255;
    Servo srv0;
    Servo srv1;
}device;

// returns servo position in mm
int getServoPosition(int servo_number){

    int result = -1;

    if(servo_number == 0)
        result = map(analogRead(SERVO_0_A), 0, 4096, 100, 0);

    if(servo_number == 1)
        result = map(analogRead(SERVO_1_A), 0, 4096, 100, 0);

    // Serial.printf("Read servo %d as %d\n", servo_number, result);

    return result;
}

// set servo positions (takes parameters in mm)
void setServoPosition(int servo_number, int srv_pos){
    if(servo_number == 0)
        device.srv0.writeMicroseconds(map(srv_pos, POS_LOWERBOUND, POS_UPPERBOUND, APPROX_CALIBRATION_1000, APPROX_CALIBRATION_2000));

    if(servo_number == 1)
        device.srv1.writeMicroseconds(map(srv_pos, POS_LOWERBOUND, POS_UPPERBOUND, APPROX_CALIBRATION_1000, APPROX_CALIBRATION_2000));

    // Serial.printf("Moved servo %d to %d\n", servo_number, srv_pos);
}


// Updates the status struct
void updateStatus(){
    status.srv0_raw = getServoPosition(0);
    status.srv1_raw = getServoPosition(1);

    status.bat_stat = (analogRead(A2))/100;
}

void actuateMotors(int tolerance){

    if(getAbsoluteValue(status.srv0_pos - status.srv0_raw) > tolerance){
        if(status.srv0_raw < status.srv0_pos)
            device.srv0.writeMicroseconds(2000);
        else
            device.srv0.writeMicroseconds(1000);
    }

    if(getAbsoluteValue(status.srv1_pos - status.srv1_raw) > tolerance){
        if(status.srv1_raw < status.srv1_pos)
            device.srv1.writeMicroseconds(2000);
        else
            device.srv1.writeMicroseconds(1000);
    }

    delay(5);

    updateStatus();

    device.srv0.writeMicroseconds(map(status.srv0_raw, 0, 100, 1000, 2000));
    device.srv1.writeMicroseconds(map(status.srv1_raw, 0, 100, 1000, 2000));
}

////////////////////////////////////////// Calibration //////////////////////////////////////////

// initialize robot calibration struct
struct srv_calibration_t {
    uint8_t version;
    int srv0_min_ms, srv0_max_ms; 
    int srv1_min_ms, srv1_max_ms;
    int srv0_raw_min, srv0_raw_max;
    int srv1_raw_min, srv1_raw_max;
} srv_calibration;

void runCalibration(){
    int addr = EEPROM_CALIBRATION_ADDR;
    // initialize calibration
    srv_calibration = { 0,
                        1000, 2000, 
                        1000, 2000, 
                        0, 4096, 
                        0, 4096 
                      };

    // set servos to 0
    Serial.println("Set Servo to 1000 and wait for it to get there");
    device.srv0.writeMicroseconds(1000);
    device.srv1.writeMicroseconds(1000);
    status.srv0_pos = 0;
    status.srv1_pos = 0;
    wait_visual(20000);

    // calibration relevant variables
    int srv0_raw_new, srv1_raw_new;
    bool srv0_is_responding = false;
    bool srv1_is_responding = false;
    int srv0_buffer[MOVING_AVERAGE_PERIOD];
    int srv0_buffer_idx = 0;
    int srv0_sum = status.srv0_raw * MOVING_AVERAGE_PERIOD;
    int srv0_avg = status.srv0_raw;
    int srv0_prev_avg = srv0_avg;
    int srv1_buffer[MOVING_AVERAGE_PERIOD];
    int srv1_buffer_idx = 0;
    int srv1_sum = status.srv1_raw * MOVING_AVERAGE_PERIOD;
    int srv1_avg = status.srv1_raw;
    int srv1_prev_avg = srv1_avg;
    int srv0_OOR_counter = 0;
    int srv1_OOR_counter = 0;

    // initialize buffers with initial readings
    status.srv0_raw = analogRead(A0);
    status.srv1_raw = analogRead(A1);
    for (int i = 0; i < MOVING_AVERAGE_PERIOD; i++){
        srv0_buffer[i] = status.srv0_raw;
        srv1_buffer[i] = status.srv1_raw;
    }
    Serial.println("Entering Calibration Loop:\n");
    Serial.printf("Input,\tSRV0_new,\tSRV0,\tSRV1_new,\tSRV1\n"); 
    for (int i = 1000; i < 2001; i+=1){
        device.srv0.writeMicroseconds(i);
        device.srv1.writeMicroseconds(i);
        status.srv0_pos = i;
        status.srv1_pos = i;
        // wait for Servo to go to position
        delay(125);

        // remove previous value
        srv0_sum = srv0_sum - srv0_buffer[srv0_buffer_idx];
        srv1_sum = srv1_sum - srv1_buffer[srv1_buffer_idx];

        // read current position
        srv0_raw_new = analogRead(A0);
        srv1_raw_new = analogRead(A1);

        // add new raw value to buffer
        srv0_buffer[srv0_buffer_idx] = srv0_raw_new;
        srv1_buffer[srv1_buffer_idx] = srv1_raw_new;

        // update sum
        srv0_sum += srv0_raw_new;
        srv1_sum += srv1_raw_new;

        // save prev average
        srv0_prev_avg = srv0_avg;
        srv1_prev_avg = srv1_avg;

        // moving average
        srv0_avg = srv0_sum / MOVING_AVERAGE_PERIOD;
        srv1_avg = srv1_sum / MOVING_AVERAGE_PERIOD;
 
        Serial.printf("%i,\t%i,\t%i,\t%i,\t%i\n", 
            i, srv0_raw_new, status.srv0_raw, srv1_raw_new, status.srv1_raw);
        // Servo 0
        if (abs(srv0_prev_avg - srv0_avg) > CALIBRATION_CHANGE_THRESHOLD && !srv0_is_responding) {
            if ( srv0_OOR_counter == 0 ) {
                srv_calibration.srv0_min_ms = i;
                srv_calibration.srv0_raw_min = srv0_raw_new;
            }
            srv0_OOR_counter++;
            if (srv0_OOR_counter > SRV_UNRESPONSIVE_CUTOFF){
                Serial.printf("Set srv0_min_ms to %i\n", i);
                Serial.printf("Set srv0_raw_min to %i\n", srv0_raw_new);
                srv0_is_responding = true;
            }
        } else if (abs(srv0_prev_avg - srv0_avg) == 0 && srv0_is_responding) {
            if ( srv0_OOR_counter == 0 ) {
                srv_calibration.srv0_max_ms = i;
                srv_calibration.srv0_raw_max = srv0_raw_new;
            }
            srv0_OOR_counter++; // increment counter
            if ( srv0_OOR_counter > SRV_UNRESPONSIVE_CUTOFF ) {
                // after multiple commands result in no respond mark servo as unresponsive
                Serial.printf("Set srv0_max_ms to %i\n", i);
                Serial.printf("Set srv0_raw_max to %i\n", srv0_raw_new);
                srv0_is_responding = 0;
            }
        } else {
            srv0_OOR_counter = 0;
        }
        // Servo 1
        if (abs( srv1_prev_avg - srv1_avg) > CALIBRATION_CHANGE_THRESHOLD && !srv1_is_responding){
            if ( srv1_OOR_counter == 0 ){
                srv_calibration.srv1_min_ms = i;
                srv_calibration.srv1_raw_min = srv1_raw_new;
            }
            srv1_OOR_counter++; // increment counter
            if ( srv1_OOR_counter > SRV_UNRESPONSIVE_CUTOFF ) {
                Serial.printf("Set srv1_min_ms to %i\n", i);
                Serial.printf("Set srv1_raw_min to %i\n", srv1_raw_new);
                srv1_is_responding = true;
            }
        } else if (abs(srv1_prev_avg - srv1_avg) == 0 && srv1_is_responding){
            if ( srv1_OOR_counter == 0 ) {
                srv_calibration.srv1_max_ms = i;
                srv_calibration.srv1_raw_max = srv1_raw_new;
            }
            srv1_OOR_counter++; // increment counter
            if ( srv1_OOR_counter > SRV_UNRESPONSIVE_CUTOFF ) {
                Serial.printf("Set srv1_max_ms to %i\n", i);
                Serial.printf("Set srv1_raw_max to %i\n", srv1_raw_new);
                // after multiple commands result in no respond mark servo as unresponsive
                srv1_is_responding = 0;
            }
        } else {
            srv1_OOR_counter = 0;
        }
        // reset out of range counters
        status.srv0_raw = srv1_avg;
        status.srv1_raw = srv1_avg;

        // Compute index
        srv0_buffer_idx = (srv0_buffer_idx + 1) % MOVING_AVERAGE_PERIOD;
        srv1_buffer_idx = (srv1_buffer_idx + 1) % MOVING_AVERAGE_PERIOD;
    }

    // compute calibration parameters and store them in
    // srv_calibration and EEPROM
    EEPROM.put(addr, srv_calibration);
}

void calibrateServoRange(){
    int addr = EEPROM_CALIBRATION_ADDR;
    EEPROM.get(addr, srv_calibration);
    if(srv_calibration.version != 0 || FORCE_CALIBRATION) {
        // EEPROM was empty -> initialize value
        Serial.println("No Calibration found: running calibration.\n");
        runCalibration();
    } else {
        // Existing calibration found and loaded
        Serial.println("Existing Calibration found & Loaded.\n");
    }
    // re-attach servos with calibrated range
    device.srv0.detach();
    device.srv1.detach();
    device.srv0.attach(D0, srv_calibration.srv0_min_ms, srv_calibration.srv0_max_ms);
    device.srv1.attach(D1, srv_calibration.srv1_min_ms, srv_calibration.srv1_max_ms);
}


////////////////////////////////// Repeatedly called functions /////////////////////////////////

int POSVEL_START_TIME   = 0;
int POSVEL_START_0      = 0;
int POSVEL_START_1      = 0;

void initPosVel(){
    POSVEL_START_TIME = SERVER_TIME_MILLIS();
    POSVEL_START_0 = getServoPosition(0);
    POSVEL_START_1 = getServoPosition(1);
}

void doPosVelCommand(){

    // Get velocities
    status.srv0_vel = msg_position.get.srv0_vel;
    status.srv1_vel = msg_position.get.srv1_vel;

    // Time since initPosVel was called
    int time_passed = SERVER_TIME_MILLIS() - POSVEL_START_TIME;

    // 1mm/10s = 1mm/10000ms

    int dir0 = getSign(msg_position.get.srv0_pos - POSVEL_START_0);
    int dir1 = getSign(msg_position.get.srv1_pos - POSVEL_START_1);

    int net_delta0 = (1/10000.0) * status.srv0_vel * time_passed;
    int net_delta1 = (1/10000.0) * status.srv1_vel * time_passed;

    int delta0 = dir0 * net_delta0;
    int delta1 = dir1 * net_delta1;

    if(dir0 < 0)
        status.srv0_pos = getMax(msg_position.get.srv0_pos, POSVEL_START_0 + delta0);
    else
        status.srv0_pos = getMin(msg_position.get.srv0_pos, POSVEL_START_0 + delta0);

    if(dir1 < 0)
        status.srv1_pos = getMax(msg_position.get.srv1_pos, POSVEL_START_1 + delta1);
    else
        status.srv1_pos = getMin(msg_position.get.srv1_pos, POSVEL_START_1 + delta1);
    
    // Use bang-bang control to write positions
    actuateMotors(0);

    // Tolerance for this command is 1 - ends with status restore
    if(getAbsoluteValue(msg_position.get.srv0_pos - status.srv0_raw) < 1 && getAbsoluteValue(msg_position.get.srv1_pos - status.srv1_raw) < 1)
        status.device_status = 'R';
}

void doCalibrateCommand(){
    Serial.printf("Calibrating...\n");
    runCalibration();
}

void doSinCommand(){

    unsigned short start_time = msg_sin.get.start_time;

    unsigned char a0 = msg_sin.get.a0;
    unsigned char x0 = msg_sin.get.x0;
    short ps0 = msg_sin.get.phase_shift_0;
    short p0 = msg_sin.get.period_0;
    
    unsigned char a1 = msg_sin.get.a1;
    unsigned char x1 = msg_sin.get.x1;
    short ps1 = msg_sin.get.phase_shift_1;
    short p1 = msg_sin.get.period_1;

    int t = SERVER_TIME_MILLIS();

    status.srv0_pos = (a0*sin((t*(6.26318/p0)+start_time+(ps0/1000.0))))+x0;
    status.srv1_pos = (a1*sin((t*(6.26318/p1)+start_time+(ps1/1000.0))))+x1;

    if(SERVER_TIME() >= start_time)
        actuateMotors(0);
}

void doWalkCommand(){
    Serial.printf("Walking %d steps\n", msg_walk.get.nr_steps);
}
