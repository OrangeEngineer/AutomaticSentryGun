/* Simplified Logitech Extreme 3D Pro Joystick Report Parser */
#include <pt.h> 
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser.h"

#define PT_DELAY(pt, ms,ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));

struct pt pt_JoystickState;
struct pt pt_PanMotorDriverState;
struct pt pt_TiltMotorDriverState;


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

USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents;
JoystickReportParser                            Joy(&JoyEvents);

PT_THREAD(JoystickState(struct pt* pt))
{
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1)
  {
    joyX = JoystickEvents::mostRecentEvent.x;
    joyY = JoystickEvents::mostRecentEvent.y;
    Serial.print("X: ");
    Serial.print(joyX);
    Serial.print(" Y: ");
    Serial.print(joyY);
    Serial.println("");
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
      if(joyX < 490){
        digitalWrite(7,HIGH);      //ตามตารางข้างบน ต้องกำหนด IN1 = HIGH 
        digitalWrite(8,LOW);       //และ IN2 = LOW มอเตอร์ A จึงจะทำงานหมุนไปด้านหน้า
        analogWrite(11,(512 - joyX/2));       //และสั่งหมุนที่ความเร็วสูงสุด 255 ผ่านทาง ENA (ขา 10) ที่เป็น PWM
      } else if(joyX > 530) {
        digitalWrite(7,LOW);        //ตามตารางข้างบน ต้องกำหนด IN1 = LOW
        digitalWrite(8,HIGH);       //และ IN2 = HIGH มอเตอร์ A จึงจะทำงานหมุนถอยหลัง
        analogWrite(11,(joyX - 512/2));         //และสั่งหมุนที่ความเร็วสูงสุด 255 ผ่านทาง ENA (ขา 10) ที่เป็น PWM
      } else {
        digitalWrite(7,HIGH);       //ตามตารางข้างบน ต้องกำหนด IN1 = HIGH
        digitalWrite(8,HIGH);       //และ IN2 = HIGH มอเตอร์ A จึงจะหยุดการทำงานทันที (เบรค)
        analogWrite(11,0);            //และสั่งความเร็วมอเตอร์ให้เป็น 0 ผ่านทาง ENA (ขา 10) ที่เป็น PWM
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
      if(joyY < 490){
        digitalWrite(4,HIGH);      //ตามตารางข้างบน ต้องกำหนด IN1 = HIGH 
        digitalWrite(5,LOW);       //และ IN2 = LOW มอเตอร์ A จึงจะทำงานหมุนไปด้านหน้า
        analogWrite(6,(512 - joyY/2));       //และสั่งหมุนที่ความเร็วสูงสุด 255 ผ่านทาง ENA (ขา 10) ที่เป็น PWM
      } else if(joyY > 530) {
        digitalWrite(4,LOW);        //ตามตารางข้างบน ต้องกำหนด IN1 = LOW
        digitalWrite(5,HIGH);       //และ IN2 = HIGH มอเตอร์ A จึงจะทำงานหมุนถอยหลัง
        analogWrite(6,(joyY - 512/2));         //และสั่งหมุนที่ความเร็วสูงสุด 255 ผ่านทาง ENA (ขา 10) ที่เป็น PWM
      } else {
        digitalWrite(4,HIGH);       //ตามตารางข้างบน ต้องกำหนด IN1 = HIGH
        digitalWrite(5,HIGH);       //และ IN2 = HIGH มอเตอร์ A จึงจะหยุดการทำงานทันที (เบรค)
        analogWrite(6,0);            //และสั่งความเร็วมอเตอร์ให้เป็น 0 ผ่านทาง ENA (ขา 10) ที่เป็น PWM
      }
      PT_DELAY(pt, 50, ts);
    }
    PT_END(pt);
}

void setup()
{
  TCCR1B = TCCR1B & B11111000 | B00000101;
  pinMode(4,OUTPUT);   //กำหนด ขา 8 (ต่ออยู่กับ IN2 ให้เป็นแบบ OUTPUT)
  pinMode(5,OUTPUT);   //กำหนด ขา 9 (ต่ออยู่กับ IN1 ให้เป็นแบบ OUTPUT)
  pinMode(6,OUTPUT);   //กำหนด ขา 10 (ต่ออยู่กับ ENA ให้เป็นแบบ OUTPUT)

  pinMode(8,OUTPUT);   //กำหนด ขา 8 (ต่ออยู่กับ IN2 ให้เป็นแบบ OUTPUT)
  pinMode(7,OUTPUT);   //กำหนด ขา 9 (ต่ออยู่กับ IN1 ให้เป็นแบบ OUTPUT)
  pinMode(11,OUTPUT);   //กำหนด ขา 10 (ต่ออยู่กับ ENA ให้เป็นแบบ OUTPUT)
  Serial.begin( 115200 );
  #if !defined(__MIPSEL__)
      while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    #endif
      Serial.println("Start");

      if (Usb.Init() == -1)
          Serial.println("OSC did not start.");

      if (!Hid.SetReportParser(0, &Joy))
          ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1  );
  PT_INIT(&pt_JoystickState);
  PT_INIT(&pt_PanMotorDriverState);
  PT_INIT(&pt_TiltMotorDriverState);

}

void loop()
{
    Usb.Task();
    JoystickState(&pt_JoystickState);
    PanMotorDriverState(&pt_PanMotorDriverState);
    TiltMotorDriverState(&pt_TiltMotorDriverState);
}

