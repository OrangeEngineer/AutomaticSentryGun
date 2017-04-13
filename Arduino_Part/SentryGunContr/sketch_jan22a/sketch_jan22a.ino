/* Simplified Logitech Extreme 3D Pro Joystick Report Parser */
#include <pt.h> 
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser.h"

#define PT_DELAY(pt, ms,ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));
struct pt pt_ReadPiSerialState;
struct pt pt_JoystickState;
struct pt pt_PanMotorDriverState;
struct pt pt_TiltMotorDriverState;
struct pt pt_ShotState;

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

int joyX = 512;       // min: 0 | max: 1023 | center: 512
int joyY = 512;       // min: 0 | max: 1023 | center: 512
int joyHat = 8;       // up: 0 | right-up: 1 | right: 2 | right-down: 3 | down: 4 | left-down: 5 | left: 6 | left-up: 7 | center: 8
int joyTwist = 128;   // min: 0 | max: 255 | center: 128
int joySlider = 0;    // min: 0 | max: 255
int joyButtonsA = 0;
int joyButtonsB = 0;
int TestSerial = 0;
int incomingByte = 0; 

USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents;
JoystickReportParser                            Joy(&JoyEvents);

PT_THREAD(ReadPiSerialState(struct pt* pt))
{
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1)
  {
    if (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = Serial.read();
  
      // say what you got:
      Serial.print("I received: ");
      Serial.println(incomingByte, DEC);
      TestSerial = incomingByte;
    }
    PT_DELAY(pt, 200, ts);
  }
  PT_END(pt);
}

PT_THREAD(JoystickState(struct pt* pt))
{
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1)
  {
    joyX = JoystickEvents::mostRecentEvent.x;
    joyY = JoystickEvents::mostRecentEvent.y;
    joyButtonsA = JoystickEvents::mostRecentEvent.buttons_a;
//    Serial.print("X: ");
//    Serial.print(joyX);
//    Serial.print(" Y: ");
//    Serial.print(joyY);
//    Serial.print(" Button_A: ");
//    Serial.print(joyButtonsA);
//    Serial.println("");
    PT_DELAY(pt, 200, ts);
  }
  PT_END(pt);
}

PT_THREAD(PanMotorDriverState(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
    while (1)
    {
      if(joyX < 450 || TestSerial == 97){
        digitalWrite(7,HIGH);      //IN1 = HIGH  
        digitalWrite(8,LOW);       //IN2 = LOW -> MotorA get forward.
        analogWrite(12,255);       //PWM Control of MotorA
      } else if(joyX > 550|| TestSerial == 100) {
        digitalWrite(7,LOW);        //IN1 = LOW
        digitalWrite(8,HIGH);       //IN2 = HIGH -> MotorA get backward.
        analogWrite(12,255);         //PWM Control of MotorB
      } else {
        digitalWrite(7,HIGH);       //IN1 = HIG
        digitalWrite(8,HIGH);       //IN2 = HIGH -> MotorB stop
        analogWrite(12,0);          //PWM = 0 
      }
      PT_DELAY(pt, 50, ts);
    }
    PT_END(pt);
}

PT_THREAD(TiltMotorDriverState(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
    while (1)
    {
      if(joyY < 450 || TestSerial == 119){
        digitalWrite(4,HIGH);      //IN1 = HIGH 
        digitalWrite(5,LOW);       //IN2 = LOW -> MotorB get forward.
        analogWrite(6,255);       //PWM Control of MotorB
      } else if(joyY > 550 || TestSerial == 115) {
        digitalWrite(4,LOW);        //IN1 = LOW
        digitalWrite(5,HIGH);       //IN2 = HIGH -> MotorB get backward.
        analogWrite(6,255);         //PWM Control of MotorB
      } else {
        digitalWrite(4,HIGH);       //IN1 = HIGH
        digitalWrite(5,HIGH);       //IN2 = HIGH -> MotorB stop 
        analogWrite(6,0);            //PWM = 0 
      }
      PT_DELAY(pt, 50, ts);
    }
    PT_END(pt);
}

PT_THREAD(ShotState(struct pt* pt))
{
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1)
  {
    if(joyButtonsA == 1 || TestSerial == 102){
      digitalWrite(2,HIGH);
    } else {
      digitalWrite(2,LOW);
    }
    PT_DELAY(pt, 200, ts);
  }
  PT_END(pt);
}

void setup()
{
  TCCR1B = TCCR1B & B11111000 | B00000101;
  pinMode(4,OUTPUT);   //IN1 MotorA
  pinMode(5,OUTPUT);   //IN2 MotorA
  pinMode(6,OUTPUT);   //ENA MotorA

  pinMode(8,OUTPUT);   //IN1 MotorB
  pinMode(7,OUTPUT);   //IN2 MotorB
  pinMode(12,OUTPUT);   //ENA MotorB

  pinMode(2,OUTPUT); //Shooting Pin
  
  Serial.begin( 115200 );
  #if !defined(__MIPSEL__)
      while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    #endif
      Serial.println("Start");

      if (Usb.Init() == -1)
          Serial.println("OSC did not start.");

      if (!Hid.SetReportParser(0, &Joy))
          ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1  );
  
  PT_INIT(&pt_ReadPiSerialState);
  PT_INIT(&pt_JoystickState);
  PT_INIT(&pt_PanMotorDriverState);
  PT_INIT(&pt_TiltMotorDriverState);
  PT_INIT(&pt_ShotState);

}

void loop()
{
    Usb.Task();
    ReadPiSerialState(&pt_ReadPiSerialState);
    JoystickState(&pt_JoystickState);
    PanMotorDriverState(&pt_PanMotorDriverState);
    TiltMotorDriverState(&pt_TiltMotorDriverState);
    ShotState(&pt_ShotState);
}

