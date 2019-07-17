#include "mavlink2\common\mavlink.h"

//Reflects connection with the drone
static const String HEARTBEATING = "HEARTBEATING";
//Set of the drone's state commands
static const String ARMED = "ARMED";
static const String DISARMED = "DISARMED";
//Set of the flight mode commands
static const String SET_FLIGHT_MODE_STABILIZE = "SET_FLIGHT_MODE_STABILIZE";
static const String SET_FLIGHT_MODE_ALTHOLD = "SET_FLIGHT_MODE_ALTHOLD";
static const String SET_FLIGHT_MODE_LOITER = "SET_FLIGHT_MODE_LOITER";
static const String SET_FLIGHT_MODE_AUTO = "SET_FLIGHT_MODE_AUTO";
static const String SET_FLIGHT_MODE_CIRCLE = "SET_FLIGHT_MODE_CIRCLE";

//Available flight modes
static const String STABILIZE = "STABILIZE";
static const String ALTHOLD = "ALTHOLD";
static const String LOITER = "LOITER";
static const String AUTO = "AUTO";
static const String CIRCLE = "CIRCLE";

//Set of the rotors value commands
static const String SET_ROLL = "SET_ROLL_";//+ int
static const String SET_PITCH = "SET_PITCH_";//+ int
static const String SET_THROTTLE = "SET_THROTTLE_";//+ int
static const String SET_YAW = "SET_YAW_";//+ int

//Sets elementary actions
static const String SET_ARM = "SET_ARM";
static const String SET_DISARM = "SET_DISARM";

//####################################
//#Initial Configuration of the drone#
//####################################

boolean current_arm = false;
String current_mode = STABILIZE;
int current_roll = 0;
int current_pitch = 0;
int current_throttle = 0; //Min value is 1150 to run motors
int current_yaw = 0;

HardwareSerial SerialMAV(2); //default pins for 16RX, 17TX

//#######################
//#Mavlink configuration#
//#######################

int sysid = 255;//GCS                   ///< ID 20 for this airplane. 1 PX, 255 ground station
int compid = 190;//Mission Planner                ///< The component sending the message
int type = MAV_TYPE_QUADROTOR;   ///< This system is an airplane / fixed wing

// Define the system type, in this case an airplane -> on-board controller
uint8_t system_type = MAV_TYPE_GENERIC;
uint8_t autopilot_type = MAV_AUTOPILOT_GENERIC;

// Hardware definitions
uint8_t system_mode = MAV_MODE_TEST_ARMED; /// /* UNDEFINED mode. This solely depends on the autopilot - use with caution, intended for developers only. | */
uint32_t custom_mode = MAV_MODE_FLAG_SAFETY_ARMED; ///< Custom mode, can be defined by user/adopter
uint8_t system_state = MAV_STATE_STANDBY; ///< System ready for flight

// Mavlink variables
unsigned long previousMillisMAVLink = 0;     // will store last time MAVLink was transmitted and listened
unsigned long next_interval_MAVLink = 1000;  // next interval to count
const int num_hbs = 60;                      // # of heartbeats to wait before activating STREAMS from APM. 60 = one minute.
int num_hbs_past = num_hbs;

 
void MavlinkSetup(){  

  //Start drone's serial connection
  //16, 17 corresponds to the ESP32 serial pins
  SerialMAV.begin(57600, SERIAL_8N1, 16, 17);
  
}


void MavlinkLoop(){
  // Initialize the required buffers
  mavlink_rc_channels_override_t sp;
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  //We have to send the heartbeats to indicate side by side connection
  mav_heartbeat_pack();

  mav_set_mode(current_mode);

  mav_arm_pack(current_arm);

  // ROLL, PITCH, THROTTLE, YAW
  mav_override_rc(current_roll, current_pitch, current_throttle, current_yaw);

  // Send the message with the standard UART send function
  // uart0_send might be named differently depending on
  // the individual microcontroller / library in use.
  unsigned long currentMillisMAVLink = millis();
  if (currentMillisMAVLink - previousMillisMAVLink >= next_interval_MAVLink) {
    // Timing variables
    previousMillisMAVLink = currentMillisMAVLink;

    SerialMAV.write(buf, len);

    //Mav_Request_Data();
    num_hbs_past++;
    if(num_hbs_past>=num_hbs) {
      // Request streams from APM
      Mav_Request_Data();
      num_hbs_past=0;
    }
  }

  // Check reception buffer
  comm_receive();
  
}

void comm_receive() {
  mavlink_message_t msg;
  mavlink_status_t status;
 //Checks if drone is connected
  while(SerialMAV.available()) {
    uint8_t c = SerialMAV.read();
    //Indicates data receive frequency
    digitalWrite(LED_BUILTIN, HIGH);
        
    // Try to get a new message
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      //Indicates data flow
      Serial.println("+");
      // Handle message
      switch(msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:  // #0: Heartbeat
          {
            // E.g. read GCS heartbeat and go into
            // comm lost mode if timer times out
            //Serial.println("MAVLINK_MSG_ID_HEARTBEAT");
            mavlink_heartbeat_t hb;
            mavlink_msg_heartbeat_decode(&msg, &hb);
            Serial.print("State: "); Serial.println(hb.base_mode == 209 ? "Armed" : "Disarmed");
            Serial.print("Mode: ");
            switch(hb.custom_mode) {
              case 0:
                Serial.println("Stabilize");
              break;
              case 2:
                Serial.println("AltHold");
              break;
              case 3:
                Serial.println("Auto");
              break;
              case 5:
                Serial.println("Loiter");
              break;
              case 7:
                Serial.println("Circle");
              break;
              default:
                Serial.println("Mode not known");
              break;
            }
            //Stablize = 0
            //AltHold = 2
            //Auto = 3
            //Loiter = 5
            //Circle = 7
          }
          break;
        case MAVLINK_MSG_ID_SYS_STATUS:  // #1: SYS_STATUS
          {
            /* Message decoding: PRIMITIVE
             *    mavlink_msg_sys_status_decode(const mavlink_message_t* msg, mavlink_sys_status_t* sys_status)
             */
            mavlink_sys_status_t sys_status;
            mavlink_msg_sys_status_decode(&msg, &sys_status);
            //Serial.println("MAVLINK_MSG_ID_SYS_STATUS");
            //Serial.println("Battery (V): ");
            //Serial.println(sys_status.voltage_battery);
          }
          break;
        case MAVLINK_MSG_ID_PARAM_VALUE:  // #22: PARAM_VALUE
          {
            /* Message decoding: PRIMITIVE
             *    mavlink_msg_param_value_decode(const mavlink_message_t* msg, mavlink_param_value_t* param_value)
             */
            //mavlink_message_t* msg;
            mavlink_param_value_t param_value;
            mavlink_msg_param_value_decode(&msg, &param_value);
            //Serial.println("MAVLINK_MSG_ID_PARAM_VALUE");
          }
          break;
        case MAVLINK_MSG_ID_RAW_IMU:  // #27: RAW_IMU
          {
            /* Message decoding: PRIMITIVE
             *    static inline void mavlink_msg_raw_imu_decode(const mavlink_message_t* msg, mavlink_raw_imu_t* raw_imu)
             */
            mavlink_raw_imu_t raw_imu;
            mavlink_msg_raw_imu_decode(&msg, &raw_imu);
            //Serial.println("MAVLINK_MSG_ID_RAW_IMU");
          }
          break;
        case MAVLINK_MSG_ID_ATTITUDE:  // #30
          {
            /* Message decoding: PRIMITIVE
             *    mavlink_msg_attitude_decode(const mavlink_message_t* msg, mavlink_attitude_t* attitude)
             */
            mavlink_attitude_t attitude;
            mavlink_msg_attitude_decode(&msg, &attitude);
            //Serial.println("MAVLINK_MSG_ID_ATTITUDE");
            //Serial.println("ROLL: ");
            //Serial.println(attitude.roll);
          }
          break;
        case MAVLINK_MSG_ID_SET_MODE: // #11
          {
            mavlink_set_mode_t set_mode;
            mavlink_msg_set_mode_decode(&msg, &set_mode);
            /*
            Serial.println("CUSTOM_MODE: ");
            Serial.println(set_mode.custom_mode);
            Serial.println("TARGET_SYSTEM: ");
            Serial.println(set_mode.target_system);
            Serial.println("BASE_MODE: ");
            Serial.println(set_mode.base_mode);
            */
          }
          break;
          //Not overriden channels
          case MAVLINK_MSG_ID_RC_CHANNELS_RAW: // #35
          {
           /* 
           *  RC (Radio controll) channels are the inputs and outputs for controlling all 
           *  actions called from joystick / mission planner. E.g. arm, throttle, pitch.
           */ 
            mavlink_rc_channels_raw_t chs;
            mavlink_msg_rc_channels_raw_decode(&msg, &chs);

            Serial.print("Roll: ");  Serial.print(chs.chan1_raw);
            Serial.println();
            Serial.print("Pitch: ");  Serial.print(chs.chan2_raw + '\n');
            Serial.println();
            Serial.print("Throttle: ");  Serial.print(chs.chan3_raw + '\n');
            Serial.println();
          }
          break;
          //Overriden channels for radio values
          case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE: // #70
          {
            mavlink_rc_channels_override_t ov_chs;
            mavlink_msg_rc_channels_override_decode(&msg, &ov_chs);
  
            Serial.print("Overr. Roll: ");  Serial.print(ov_chs.chan1_raw);
            Serial.println();
            Serial.print("Overr. Pitch: ");  Serial.print(ov_chs.chan2_raw + '\n');
            Serial.println();
            Serial.print("Overr. Throttle: ");  Serial.print(ov_chs.chan3_raw + '\n');
            Serial.println();
          }
          break;
      }
    }
  }
}


void Mav_Request_Data(){
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // STREAMS that can be requested
  /*
   * Definitions are in common.h: enum MAV_DATA_STREAM
   *   
   * MAV_DATA_STREAM_ALL=0, // Enable all data streams
   * MAV_DATA_STREAM_RAW_SENSORS=1, /* Enable IMU_RAW, GPS_RAW, GPS_STATUS packets.
   * MAV_DATA_STREAM_EXTENDED_STATUS=2, /* Enable GPS_STATUS, CONTROL_STATUS, AUX_STATUS
   * MAV_DATA_STREAM_RC_CHANNELS=3, /* Enable RC_CHANNELS_SCALED, RC_CHANNELS_RAW, SERVO_OUTPUT_RAW
   * MAV_DATA_STREAM_RAW_CONTROLLER=4, /* Enable ATTITUDE_CONTROLLER_OUTPUT, POSITION_CONTROLLER_OUTPUT, NAV_CONTROLLER_OUTPUT.
   * MAV_DATA_STREAM_POSITION=6, /* Enable LOCAL_POSITION, GLOBAL_POSITION/GLOBAL_POSITION_INT messages.
   * MAV_DATA_STREAM_EXTRA1=10, /* Dependent on the autopilot
   * MAV_DATA_STREAM_EXTRA2=11, /* Dependent on the autopilot
   * MAV_DATA_STREAM_EXTRA3=12, /* Dependent on the autopilot
   * MAV_DATA_STREAM_ENUM_END=13,
   * 
   * Data in PixHawk available in:
   *  - Battery, amperage and voltage (SYS_STATUS) in MAV_DATA_STREAM_EXTENDED_STATUS
   *  - Gyro info (IMU_SCALED) in MAV_DATA_STREAM_EXTRA1
   */

  // To be setup according to the needed information to be requested from the Pixhawk
  const int  maxStreams = 1;
  const uint8_t MAVStreams[maxStreams] = {MAV_DATA_STREAM_ALL};
  const uint16_t MAVRates[maxStreams] = {0x02};
    
  for (int i=0; i < maxStreams; i++) {
    /*
     * mavlink_msg_request_data_stream_pack(system_id, component_id, 
     *    &msg, 
     *    target_system, target_component, 
     *    MAV_DATA_STREAM_POSITION, 10000000, 1);
     *    
     * mavlink_msg_request_data_stream_pack(uint8_t system_id, uint8_t component_id, 
     *    mavlink_message_t* msg,
     *    uint8_t target_system, uint8_t target_component, uint8_t req_stream_id, 
     *    uint16_t req_message_rate, uint8_t start_stop)
     * 
     */
     
    mavlink_msg_request_data_stream_pack(2, 200, &msg, 1, 0, MAVStreams[i], MAVRates[i], 1);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);    
    SerialMAV.write(buf, len);
  }
}

void mav_heartbeat_pack() {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  
  // Pack the message
  mavlink_msg_heartbeat_pack(255,0, &msg, type, autopilot_type, system_mode, custom_mode, system_state);
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  SerialMAV.write(buf, len);
}

void mav_arm_pack(boolean state) {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  //Arm the drone
  //400 stands for MAV_CMD_COMPONENT_ARM_DISARM
  // 1 an 8'th argument is for ARM (0 for DISARM)
  if(state) {
    //ARM
    mavlink_msg_command_long_pack(0xFF, 0xBE, &msg, 1, 1, 400, 1,1.0,0,0,0,0,0,0);
  }else {
    //DISARM
    mavlink_msg_command_long_pack(0xFF, 0xBE, &msg, 1, 1, 400, 1,0.0,0,0,0,0,0,0);
  }
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  SerialMAV.write(buf, len);
}

void mav_set_mode(String value) {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  value.trim();

  //SET_MODE
  //Works with 1 at 4'th parameter
  if (value == STABILIZE){
    mavlink_msg_set_mode_pack(0xFF, 0xBE, &msg, 1, 209, 0);
  }

  if (value == ALTHOLD){
    mavlink_msg_set_mode_pack(0xFF, 0xBE, &msg, 1, 209, 2);
  }

  if (value == LOITER){
    mavlink_msg_set_mode_pack(0xFF, 0xBE, &msg, 1, 209, 5);
  }

  if (value == AUTO){
    mavlink_msg_set_mode_pack(0xFF, 0xBE, &msg, 1, 209, 3);
  }

  if (value == CIRCLE){
    mavlink_msg_set_mode_pack(0xFF, 0xBE, &msg, 1, 209, 7);
  }
  
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  SerialMAV.write(buf, len);  
}

void mav_override_rc(int roll, int pitch, int throttle, int yaw) {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  mavlink_msg_rc_channels_override_pack(0xFF, 0xBE, &msg, 1, 1, roll, pitch, throttle, yaw, 0, 0, 0, 0);
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  SerialMAV.write(buf, len);
}
