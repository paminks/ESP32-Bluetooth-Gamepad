#include <Arduino.h>
#include <BleGamepad.h>


//ABXY BUTTONS
#define X_BUTTON 23 // A
#define CIRCLE_BUTTON 19    // B
#define TRIANGLE_BUTTON 21  // Y
#define SQUARE_BUTTON 22   // X

//change every 0 to your desired pin
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

//The order of these three arrays matters a lot, be carefully when changing them. You dont even need to change them
int buttonsPins[NUM_BUTTONS] = {X_BUTTON, CIRCLE_BUTTON, TRIANGLE_BUTTON, SQUARE_BUTTON,
                          R1_BUTTON, R2_BUTTON, L1_BUTTON, L2_BUTTON,
                          START_BUTTON, SELECT_BUTTON, PS_BUTTON,
                          R3_BUTTON, L3_BUTTON};

//There is not buttons for Y and Z
int androidGamepadButtons[NUM_BUTTONS] = {1, 2, 3, 4, 8, 10, 7, 9, 12, 11, 13, 15, 14};
int PS1GamepadButtons[NUM_BUTTONS] = {2, 3, 4, 1, 6, 8, 5, 7, 10, 9, 13, 12, 11};
int PCGamepadButtons[NUM_BUTTONS] = {1, 2, 4, 3, 6, 8, 5, 7, 10, 9, 0, 12, 11};


//integer values for the Axis

uint16_t leftVrxJoystickLecture = 0;
uint16_t leftVryJoystickLecture = 0;
uint16_t rightVrxJoystickLecture = 0;
uint16_t rightVryJoystickLecture = 0;

uint16_t leftVrxJoystickValue = 0;
uint16_t leftVryJoystickValue = 0;
uint16_t rightVrxJoystickValue = 0;
uint16_t rightVryJoystickValue = 0;


//chanege gamepadMode to your desired platform
typedef enum{ANDROID, PS1, PC} GamepadModes;
GamepadModes gamepadMode = PC;
                //change   /|\ this

BleGamepad bleGamepad("Paminks --gamepad", "Maker101 Home");
BleGamepadConfiguration bleGamepadConfig;  


void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);

  for(int i=0; i<NUM_BUTTONS; i++){
    pinMode(buttonsPins[i], INPUT_PULLUP);
  }
//initializes basics of the BLE GAMEPAD library
  bleGamepadConfig.setAutoReport(false);
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  bleGamepadConfig.setVid(0xe502);
  bleGamepadConfig.setPid(0xabcd);
  bleGamepadConfig.setHatSwitchCount(4);
  bleGamepad.begin(&bleGamepadConfig);
}

void loop() {
  // put your main code here, to run repeatedly:
  //reads analog values of the joysticks 
  if(bleGamepad.isConnected()){
    //Joysticks lecture
    leftVrxJoystickLecture = analogRead(LEFT_VRX_JOYSTICK);
    leftVryJoystickLecture = analogRead(LEFT_VRY_JOYSTICK);
    rightVrxJoystickLecture = analogRead(RIGHT_VRX_JOYSTICK);
    rightVryJoystickLecture = analogRead(RIGHT_VRY_JOYSTICK);
    //serial.print's are for debugging you can remove them if you want to
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

    //do not touch this code it already works no need to change
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
//this code is inspired by Maker 101 yotube channel, but they didn't documented the code very well, I'm doing it for them 
