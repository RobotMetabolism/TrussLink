#include "utility.h"
#include "hardware.h"

//////////////////////////////////////// Setup //////////////////////////////////////////////

SYSTEM_THREAD(ENABLED);
int status_led = D7;
TCPClient server;

unsigned char DEVICE_ID                 = 0;
time_t LAST_UPDATE_TIME                 = 0;
int UPDATE_FREQUENCY                    = 1;
unsigned short CURRENT_COMMAND_CHECKSUM = 0;

/////////////////// List ///////////////////

int LIST_LOCATION                       = 4;
int LIST_CUMULATIVE_TIME                = 0;

boolean RUNNING_LIST = false;

////////////// Initialization //////////////

void setup(){

    //////// Hardware Setup ////////

    Serial.begin(9600);
    DEVICE_ID = get_device_id();
    status.device_status = 'B';

    pinMode(status_led, OUTPUT);
    device.srv0.attach(SERVO_0_D);
    device.srv1.attach(SERVO_1_D);
    Serial.println("Attached Servos");

    updateStatus();

    /// Contract Link on start up ///

    status.srv0_pos = 20;
    status.srv1_pos = 20;
    int timer = millis();
    while (millis() - timer < 10000)
        actuateMotors(2);
}

///////////////////////////////////// Main Code /////////////////////////////////////////////

char prev_device_status;
int bytes_read_header;
int bytes_read_body;
int bytes_read_footer;

// This loop runs constantly and without interruptions
void loop(){

    // Store status at the start to see if it was changed before the end
    prev_device_status = status.device_status;
    updateStatus();

    if(!server.connected()){

        // Indicate via LED: server NOT connected
        digitalWrite(status_led, LOW); 
        status.device_status = 'N';

        // Try to connect to Server
        Serial.println("Connecting to server...");
        server.connect(SERVER_IP, SERVER_PORT);
        waitFor(server.connected, 100);

        // Stop any list commands, reset list_location
        RUNNING_LIST = false;
        LIST_LOCATION = 4;

    } else {

        digitalWrite(status_led, HIGH);

        if(deviceStatusIs('N')){

            // Set status to Hello
            Serial.print("Connected to server! Sending Hello... ");
            status.device_status = 'H';

            // Prepare Hello package
            msg_hdr.get = {MSG_HELLO_LENGTH, 'H'};
            msg_hello.get = {DEVICE_ID, 65};
            msg_ftr.checksum_bytes = crc_15_footer(&msg_hdr, &msg_hello, MSG_HELLO_LENGTH);

            // Write package
            server.write(msg_hdr.bytes,   MSG_HDR_LENGTH);
            server.write(msg_hello.bytes, MSG_HELLO_LENGTH);
            server.write(msg_ftr.bytes,   MSG_FTR_LENGTH);

            // Set status to Ready
            Serial.println("Sent Hello!");
            status.device_status = 'R';

        } else if((bytes_read_header = server.read(msg_hdr.bytes, sizeof(msg_hdr))) != -1){
            
            // Try to recieve command package from server and execute it
            Serial.printf("Got Package of Type %c\n", msg_hdr.get.type);
            
            // List must be interrupted now
            RUNNING_LIST = false;

            switch (msg_hdr.get.type) {
                    case 'T':

                        // Read Time body and checksum
                        bytes_read_body = server.read(msg_epoch.bytes, sizeof(msg_epoch));
                        bytes_read_footer = server.read(msg_ftr.bytes, sizeof(msg_ftr));

                        // Verify checksum
                        if(msg_ftr.checksum_bytes == crc_15_footer(&msg_hdr, &msg_epoch, MSG_EPOCH_LENGTH)){
                            
                            // Checksum correct
                            CURRENT_COMMAND_CHECKSUM = msg_ftr.checksum_bytes;
                            
                            // Sync with second AND millisecond precision (into EPOCHs)
                            SERVER_EPOCH = msg_epoch.get.server_start;
                            int server_time_temp = SERVER_TIME();
                            while(server_time_temp == SERVER_TIME())
                                continue;
                            SERVER_EPOCH_MILLIS = millis() - (SERVER_TIME()*1000);
                        }
                        break;
                    case 'C':

                        // Read Calibration body and checksum
                        bytes_read_body = 0;
                        bytes_read_footer = server.read(msg_ftr.bytes   , sizeof(msg_ftr));

                        // Verify checksum
                        if(msg_ftr.checksum_bytes == crc_15(&msg_hdr, MSG_HDR_LENGTH)){

                            // Checksum correct
                            CURRENT_COMMAND_CHECKSUM = msg_ftr.checksum_bytes;

                            // Set status to trigger calibration
                            status.device_status = 'C';
                        }
                        break;
                    case 'P':

                        // Read PosVel body and checksum
                        bytes_read_body = server.read(msg_position.bytes, sizeof(msg_position));
                        bytes_read_footer = server.read(msg_ftr.bytes   , sizeof(msg_ftr));

                        // Verify checksum
                        if(msg_ftr.checksum_bytes == crc_15_footer(&msg_hdr, &msg_position, MSG_POSITION_LENGTH)){

                            // Checksum correct
                            CURRENT_COMMAND_CHECKSUM = msg_ftr.checksum_bytes;

                            // Set status to trigger PosVel
                            status.device_status = 'P';

                            // Initial time for velocity control
                            initPosVel();
                        }
                        break;
                    case 'W':

                        // Read Walk body and checksum
                        bytes_read_body = server.read(msg_walk.bytes, sizeof(msg_walk));
                        bytes_read_footer = server.read(msg_ftr.bytes   , sizeof(msg_ftr));

                        // Verify checksum
                        if(msg_ftr.checksum_bytes == crc_15_footer(&msg_hdr, &msg_walk, MSG_WALK_LENGTH)){

                            // Checksum correct
                            CURRENT_COMMAND_CHECKSUM = msg_ftr.checksum_bytes;

                            // Set status to trigger Walk
                            status.device_status = 'W';
                        }
                        break;
                    case 'S':

                        // Read Sinusoidal body and checksum
                        bytes_read_body = server.read(msg_sin.bytes, sizeof(msg_sin));
                        bytes_read_footer = server.read(msg_ftr.bytes, sizeof(msg_ftr));

                        // Verify checksum
                        if(msg_ftr.checksum_bytes == crc_15_footer(&msg_hdr, &msg_sin, MSG_SIN_LENGTH)){

                            // Checksum correct
                            CURRENT_COMMAND_CHECKSUM = msg_ftr.checksum_bytes;

                            // Set status to trigger Sinusoidal
                            status.device_status = 'S';
                        }
                        break;
                    case 'L':

                        // Read List body and checksum
                        bytes_read_body = server.read(msg_list_actual.bytes, msg_hdr.get.length);
                        bytes_read_footer = server.read(msg_ftr.bytes   , sizeof(msg_ftr));

                        // Verify checksum
                        if(msg_ftr.checksum_bytes == crc_15_footer(&msg_hdr, &msg_list_actual, msg_hdr.get.length)){

                            // Checksum correct
                            CURRENT_COMMAND_CHECKSUM = msg_ftr.checksum_bytes;

                            // Initialize List parameters
                            RUNNING_LIST    = true;
                            LIST_LOCATION   = 4;  // first 2 bytes are nr_repeat, next 2 bytes are start_time (short);
                            if(msg_list_actual.get.start_time == 0)    // 0 means start immediately
                                msg_list_actual.get.start_time = Time.now();
                            LIST_CUMULATIVE_TIME = 0;   // 0 seconds have passed cumulatively
                        }
                        break;
            }

            // Force-send update for acknowledgement
            LAST_UPDATE_TIME = -1;
        }
    }

    // This copies the appropriate command from the List bytes into memory and triggers it
    if(RUNNING_LIST){

        // Wait till it is time to read next command from List bytes (*)
        if(msg_list_actual.get.start_time + LIST_CUMULATIVE_TIME < SERVER_TIME()){

            // Copy command header: type and duration
            memcpy(msg_list_subheader.bytes, msg_list_actual.bytes + LIST_LOCATION, 2);
            LIST_LOCATION += 2;
            
            // To write time to next command: for line (*)
            LIST_CUMULATIVE_TIME += msg_list_subheader.get.duration;

            switch (msg_list_subheader.get.type){
                case 'P':
                    // Copy into other struct and update LIST_LOCATION
                    memcpy(msg_position.bytes, msg_list_actual.bytes+LIST_LOCATION, MSG_POSITION_LENGTH);
                    LIST_LOCATION += MSG_POSITION_LENGTH;

                    // Set status to trigger PosVel
                    status.device_status = 'P';
                    initPosVel();
                    break;
                case 'S':
                    // Copy into other struct and update LIST_LOCATION
                    memcpy(msg_sin.bytes, msg_list_actual.bytes+LIST_LOCATION, MSG_SIN_LENGTH);
                    LIST_LOCATION += MSG_SIN_LENGTH;

                    // Set status to trigger PosVel
                    status.device_status = 'S';
                    break;
                case 'E':
                    Serial.printf("Reached E: %d\n", msg_list_actual.get.nr_repeat);

                    if(msg_list_actual.get.nr_repeat == 1){
                        // If this was the last time, stop
                        RUNNING_LIST = false;
                        status.device_status = 'R';
                        Serial.println("Finished List and Loop!!!!");
                    } else if(msg_list_actual.get.nr_repeat > 0){
                        // Decrement counter and restart loop
                        Serial.printf("Looping %d\n", msg_list_actual.get.nr_repeat);
                        msg_list_actual.get.nr_repeat--;
                    }
                    LIST_LOCATION = 4;
                    break;
            }
        }
    }

    // Actuate correct Command according to status
    switch (status.device_status){
        case 'C':
            doCalibrateCommand();
            break;
        case 'P':
            doPosVelCommand();
            break;
        case 'W':
            doWalkCommand();
            break;
        case 'S':
            doSinCommand();
            break;
    }

    // List is waiting till next command starts
    if(RUNNING_LIST && status.device_status == 'R')
        status.device_status = '!';

    // Send update package to server
    if(server.connected() && !deviceStatusIs('N'))
        sendUpdatePackage();
}


void sendUpdatePackage(){
    if(LAST_UPDATE_TIME == -1 || Time.now() - LAST_UPDATE_TIME > UPDATE_FREQUENCY || (prev_device_status != status.device_status)){

        Serial.print("Time since Epoch: ");
        Serial.println(SERVER_TIME());
        Serial.println("Sending update... ");
        Serial.printf("Filled update structs %hi ...\n", CURRENT_COMMAND_CHECKSUM);

        // Prepare Update package
        msg_hdr.get = {MSG_UPDATE_LENGTH,'U'};
        msg_update.get = {
            status.device_status,
            status.srv0_pos,
            status.srv1_pos,
            status.srv0_raw,
            status.srv1_raw,
            status.bat_stat,
            status.srv0_vel,
            status.srv1_vel,
            CURRENT_COMMAND_CHECKSUM
        };
        msg_ftr.checksum_bytes = crc_15_footer(&msg_hdr, &msg_update, MSG_UPDATE_LENGTH);

        // Write package
        server.write(msg_hdr.bytes,     MSG_HDR_LENGTH);
        server.write(msg_update.bytes,  MSG_UPDATE_LENGTH);
        server.write(msg_ftr.bytes,     MSG_FTR_LENGTH);

        // Last update time is now
        LAST_UPDATE_TIME = Time.now();
        Serial.println("Sent update!");
    }
}