#define DEBUG
// the digits mean Mmmmrrr (M=Major,m=minor,r=revision)
#define SKETCH_VERSION 3000001

#include <EEPROM.h>
#include "src/Joystick.h"
#include "config.h"

// -------------------------
// Force mode / no Force mode
// -------------------------

bool ffbsupport_mode = false;

// -------------------------
// Various global variables
// -------------------------
unsigned long lastEffectsUpdate;
unsigned long nextJoystickMillis;
unsigned long nextEffectsMillis;

// --------------------------
// Joystick related variables
// --------------------------
#define minX -32768
#define maxX 32767
#define minY -32768
#define maxY 32767

bool is_connected = false;
bool forces_requested = false;
bool pos_updated = false;

int16_t  pos[2] = {0, 0};
int lastX;
int lastY;
int lastVelX;
int lastVelY;
int lastAccelX;
int lastAccelY;

EffectParams effects[2];
int32_t forces[2] = {0, 0};
int32_t before_forces[2] = {0, 0};
int32_t before_vibration[2] = {0, 0};


double  vibration1 = 0;
double  vibration2 = 0;

double  vibration1_m = 0;
double  vibration2_m = 0;

double  vib_data1 = 0;
double  vib_data2 = 0;

double  before_vib_data1 = 0;
double  before_vib_data2 = 0;
bool    vibrationMillis_check = false;


int16_t lastTime = millis();

// 설정관련
int setmode = 0;
int multiply = 1;
int min_vibration = 1;
int max_vibration = 3000;

// 스위치 관련
int button;
int button1;
int clicks = 0;
unsigned long current = 0;
unsigned long limits = 0;

// NON FORCE FEEDBACK MODE
int32_t client_vibration = 0;
int  wait_delay = 80;
int  pwr_delay = 20;



// 조이스틱 설정
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
  19, 2, // Button Count, Hat Switch Count
  true, true, false, // X, Y, Z
  false, false, false, // Rx, Ry, Rz
  false, false); // rudder, throttle

void setup() {
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  
  setupJoystick();

  // setup communication
  Serial.begin(SERIAL_BAUD);
  // setup timing and run them as soon as possible
  lastEffectsUpdate = 0;
  nextJoystickMillis = 0;
  nextEffectsMillis = 0;
}

void loop() {
         
  set_switch_calibration();
  get_messages_from_serial();

  if ( ffbsupport_mode == false ){
    unsigned long currentMillis;
    currentMillis = millis();
    // do not run more frequently than these many milliseconds
    if (currentMillis >= nextJoystickMillis) {
      if (pos_updated == true) {
          updateJoystickPos(); 
      }    
      
      Force_vibration();
    
      nextJoystickMillis = currentMillis + 2;

      // we calculate condition forces every 100ms or more frequently if we get position updates
      if (currentMillis >= nextEffectsMillis || pos_updated) {
        updateEffects(true);
        nextEffectsMillis = currentMillis + 100;
        pos_updated = false;
      } else {
        // calculate forces without recalculating condition forces
        // this helps having smoother spring/damper/friction
        // if our update rate matches our input device
        updateEffects(false);
      }
    }    
  } else {
       Vibration_mode();
       updateEffects(false);
       delay(wait_delay);
  }
}
