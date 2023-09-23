/*
 * ANDROID GAMEPAD
 * {A=1, B=2, C=3, X=4, Y=5, Z=6, L1=7, R1=8, L2=9, R2=10,
 * Select=11, Start=12, PS=13, L3=14 , R3=15} 
 * 
 * PS GAMEPAD MODE
 * {SQUARE=1, X=2, CIRCLE=3, TRIANGLE=4, L1=5, R1=6, L2=7, R2=8,
 * Select=9, Start=10, L3=11, R3=12, PS=13}
 * 
 */

#include <Arduino.h>
#include <BleGamepad.h>


//ABXY BUTTONS
#define X_BUTTON 23 // A
#define CIRCLE_BUTTON 19    // B
#define TRIANGLE_BUTTON 21  // Y
#define SQUARE_BUTTON 22   // X

//TRIGGERS
#define R1_BUTTON 0
#define R2_BUTTON 18
#define L1_BUTTON 0
#define L2_BUTTON 0

//MENU BUTTONS
#define START_BUTTON 0
#define SELECT_BUTTON 0
#define PS_BUTTON 0

//JOYSTICKS BUTTONS
#define R3_BUTTON 0
#define L3_BUTTON 2

//JOYSTICKS
#define LEFT_VRX_JOYSTICK 15
#define LEFT_VRY_JOYSTICK 4
#define RIGHT_VRX_JOYSTICK 0
#define RIGHT_VRY_JOYSTICK 0

#define NUM_BUTTONS 13

//The order of these three arrays matters a lot, be carefully when changing them
int buttonsPins[NUM_BUTTONS] = {X_BUTTON, CIRCLE_BUTTON, TRIANGLE_BUTTON, SQUARE_BUTTON,
                          R1_BUTTON, R2_BUTTON, L1_BUTTON, L2_BUTTON,
                          START_BUTTON, SELECT_BUTTON, PS_BUTTON,
                          R3_BUTTON, L3_BUTTON};

//There is not buttons for Y and Z
int androidGamepadButtons[NUM_BUTTONS] = {1, 2, 3, 4, 8, 10, 7, 9, 12, 11, 13, 15, 14};
int PS1GamepadButtons[NUM_BUTTONS] = {2, 3, 4, 1, 6, 8, 5, 7, 10, 9, 13, 12, 11};
int PCGamepadButtons[NUM_BUTTONS] = {1, 2, 4, 3, 6, 8, 5, 7, 10, 9, 0, 12, 11};


uint16_t leftVrxJoystickLecture = 0;
uint16_t leftVryJoystickLecture = 0;
uint16_t rightVrxJoystickLecture = 0;
uint16_t rightVryJoystickLecture = 0;

uint16_t leftVrxJoystickValue = 0;
uint16_t leftVryJoystickValue = 0;
uint16_t rightVrxJoystickValue = 0;
uint16_t rightVryJoystickValue = 0;



typedef enum{ANDROID, PS1, PC} GamepadModes;
GamepadModes gamepadMode = PC;


BleGamepad bleGamepad("Paminks --gamepad", "Maker101 Home");
BleGamepadConfiguration bleGamepadConfig;  


void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);

  for(int i=0; i<NUM_BUTTONS; i++){
    pinMode(buttonsPins[i], INPUT_PULLUP);
  }

  bleGamepadConfig.setAutoReport(false);
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  bleGamepadConfig.setVid(0xe502);
  bleGamepadConfig.setPid(0xabcd);
  bleGamepadConfig.setHatSwitchCount(4);
  bleGamepad.begin(&bleGamepadConfig);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bleGamepad.isConnected()){
    //Joysticks lecture
    leftVrxJoystickLecture = analogRead(LEFT_VRX_JOYSTICK);
    leftVryJoystickLecture = analogRead(LEFT_VRY_JOYSTICK);
    rightVrxJoystickLecture = analogRead(RIGHT_VRX_JOYSTICK);
    rightVryJoystickLecture = analogRead(RIGHT_VRY_JOYSTICK);
    Serial.print("\n");
    Serial.print(leftVrxJoystickLecture);
    Serial.print("\n");
    Serial.print(leftVryJoystickLecture);
    Serial.print("\n");
    Serial.print(rightVrxJoystickLecture);
    Serial.print("\n");
    Serial.print(rightVryJoystickLecture);

    //Compute joysticks value
    leftVrxJoystickValue = map(leftVrxJoystickLecture, 4095, 0, 0, 32737);
    leftVryJoystickValue = map(leftVryJoystickLecture, 0, 4095, 0, 32737);
    rightVrxJoystickValue = map(rightVrxJoystickLecture, 4095, 0, 0, 32737);
    rightVryJoystickValue = map(rightVryJoystickLecture, 0, 4095, 0, 32737);

    
    switch(gamepadMode){
      case ANDROID:
        for(int i=0; i<NUM_BUTTONS; i++){
          if(!digitalRead(buttonsPins[i])){
              bleGamepad.press(androidGamepadButtons[i]);   
          }
          else{
              bleGamepad.release(androidGamepadButtons[i]);     
          }
          joysticksHandlerForMobile(leftVrxJoystickValue, leftVryJoystickValue, rightVrxJoystickValue, rightVryJoystickValue);
        } 
        break;

      case PS1:
        for(int i=0; i<NUM_BUTTONS; i++){
          if(!digitalRead(buttonsPins[i])){
            bleGamepad.press(PS1GamepadButtons[i]);     
          }
          else{
            bleGamepad.release(PS1GamepadButtons[i]);      
          }
          joysticksHandlerForMobile(leftVrxJoystickValue, leftVryJoystickValue, rightVrxJoystickValue, rightVryJoystickValue);
        } 
        break;

        case PC:
          for(int i=0; i<NUM_BUTTONS; i++){
            if(!digitalRead(buttonsPins[i])){
              bleGamepad.press(PCGamepadButtons[i]);
            }
            else{
              bleGamepad.release(PCGamepadButtons[i]);
            }
            joysticksHandlerForPC(leftVrxJoystickValue, leftVryJoystickValue, rightVrxJoystickValue, rightVryJoystickValue);
          }
          break;
    }

    bleGamepad.sendReport();
  }
}

void joysticksHandlerForMobile(uint16_t leftVrx, uint16_t leftVry, uint16_t rightVrx, uint16_t rightVry){
  bleGamepad.setLeftThumb(leftVrx, leftVryJoystickValue);
  bleGamepad.setRightThumb(rightVrxJoystickValue, rightVryJoystickValue);  
}

void joysticksHandlerForPC(uint16_t leftVrx, uint16_t leftVry, uint16_t rightVrx, uint16_t rightVry){
  bleGamepad.setX(leftVrxJoystickValue);
  bleGamepad.setY(leftVryJoystickValue);
  bleGamepad.setZ(rightVrxJoystickValue);
  bleGamepad.setRX(rightVryJoystickValue);
}