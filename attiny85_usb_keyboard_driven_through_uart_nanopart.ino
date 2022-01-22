// see https://github.com/vnp85/attiny85_usb_keyboard_driven_through_uart

#include <SoftwareSerial.h>
#define PIN_MYSERIAL_RX 2
#define PIN_MYSERIAL_TX A0

SoftwareSerial mySerial(PIN_MYSERIAL_RX, PIN_MYSERIAL_TX);

uint32_t lastReceivedButtonAtMillis;
int lastReceivedButton;
int lastKnownPauseState;

enum TPausedState {
  PAUSED_STATE___ROLLING,
  PAUSED_STATE___PAUSED
};

enum TButtonId{
  BUTTON_ID__NONE,

  BUTTON_ID__SCENE_1, 
  BUTTON_ID__SCENE_2,
  BUTTON_ID__SCENE_3,
  BUTTON_ID__SCENE_4,
  BUTTON_ID__SCENE_5,
  BUTTON_ID__START_RECORDING,
  BUTTON_ID__STOP_RECORDING,
  BUTTON_ID__RESERVED_FOR_NOW_AS_PHYSICALLY_NOT_IMPLEMENTED,
  BUTTON_ID__PAUSE,
  BUTTON_ID__UNPAUSE___VIRTUAL_BUTTON,
  
  BUTTON_ID__FIRST_INVALID_ITEM
};


#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

void setup(){
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  //pullup soldered in
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
}


void receiveButton(int b){
  if ((millis() - lastReceivedButtonAtMillis > 1000) || (b!=lastReceivedButton)){
     lastReceivedButton = b;
     char msg[16];
     if (BUTTON_ID__START_RECORDING == b){
       lastKnownPauseState = PAUSED_STATE___ROLLING;
     }
     if (BUTTON_ID__STOP_RECORDING == b){
       lastKnownPauseState = PAUSED_STATE___ROLLING;
     }
     if (BUTTON_ID__PAUSE == b){
       if (PAUSED_STATE___PAUSED == lastKnownPauseState){
         lastKnownPauseState = PAUSED_STATE___ROLLING;
         b = BUTTON_ID__UNPAUSE___VIRTUAL_BUTTON;
       }else{
         lastKnownPauseState = PAUSED_STATE___PAUSED;
       }
     }
     byte key = KEY_F12 - (b - 1);     
     strcpy(msg, "#s");
     if (key < 16){
      strcat(msg, "0");
     }
     itoa(key, msg+strlen(msg), 16);
     byte modifier=MOD_CONTROL_LEFT | MOD_SHIFT_LEFT | MOD_ALT_LEFT;
     if (modifier < 16){
      strcat(msg, "0");
     }
     itoa(modifier, msg+strlen(msg), 16);
     mySerial.println(msg);
     Serial.println(msg);
     lastReceivedButtonAtMillis = millis();
  }
}



void loop(){
  while (Serial.available()){
    char c[2];
    c[0] = Serial.read();
    c[1] = 0;
    //echo
    if ((c[0] != 13)&&(c[0] != 10)){
      mySerial.print("#e");
      mySerial.println(c);
    }
  }
  if (LOW == digitalRead(A1)){
    receiveButton(BUTTON_ID__SCENE_1);
  };
  if (LOW == digitalRead(A2)){
    receiveButton(BUTTON_ID__SCENE_2);
  };
  if (LOW == digitalRead(A3)){
    receiveButton(BUTTON_ID__SCENE_3);
  };
  if (analogRead(A6) < 64){
    receiveButton(BUTTON_ID__SCENE_4);
  }
  if (analogRead(A7) < 64){
    receiveButton(BUTTON_ID__SCENE_5);
  }

  if (LOW == digitalRead(4)){
    receiveButton(BUTTON_ID__START_RECORDING);
  }
  if (LOW == digitalRead(5)){
    receiveButton(BUTTON_ID__STOP_RECORDING);
  }
  if (LOW == digitalRead(6)){
    receiveButton(BUTTON_ID__RESERVED_FOR_NOW_AS_PHYSICALLY_NOT_IMPLEMENTED);
  }
  if (LOW == digitalRead(7)){
    // physically missing
    receiveButton(BUTTON_ID__PAUSE);
  }

}
