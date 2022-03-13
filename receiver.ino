//  4 Channel Receiver
// We will be sending PWM signal, PWM is using for sending HIGH&LOW singal to servos and ESC

#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

int servo_angles[5] {0}; //tabel of servos angels filled with 0

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

struct Signal { //the same structure that was in our transmitter code
byte throttle;      
byte pitch;
byte roll;
byte yaw;
};

Signal data; //initial Signal structure

const uint64_t pipeIn = 0xE9E8F0F0E1LL; //the same code from transmitter
RF24 radio(9, 10);  // Pins D9 & D10 will be using to recive data


void ResetData()
{
  data.roll = 127;   //127 is a middle value for servos and potentiometers
  data.pitch = 127;  
  data.throttle = 127;
  data.yaw = 127;
}

void setup()
{
  //Setting up pins to send PWN signal 
  servo1.attach(2);
  servo2.attach(3);
  servo3.attach(4);
  servo4.attach(5);
  servo5.attach(6);

  //Starting RF module - NRF
  ResetData();
  radio.begin();
  radio.openReadingPipe(1,pipeIn);
  radio.startListening(); //start the radio comunication

  pinMode(LED_BUILTIN, OUTPUT);
}



unsigned long lastRecTime = 0;

void recvData(){
  while ( radio.available() ) {
    radio.read(&data, sizeof(Signal)); //reading the data from transmitter
    lastRecTime = millis();
  }
}

void loop(){
  recvData();
  unsigned long now = millis();
  
  //in case of signal lose all of the servos and motor will be center(motor will stop)
  if (now - lastRecTime > 1000) //I recommend you to use 2000ms for long range missions 
    ResetData();
  
  servo_angles[0] = map(data.throttle, 0, 255, 1000, 2000);     // pin D2 (PWM signal)
  servo_angles[1] = map(data.roll,    0, 255, 1000, 2000);     // pin D3 (PWM signal)
  servo_angles[2] = map(data.roll,     0, 255, 1000, 2000);     // pin D2 (PWM signal)
  servo_angles[3] = map(data.pitch,      0, 255, 1000, 2000);     // pin D5 (PWM signal)
  servo_angles[4] = map(data.yaw,     0, 255, 1000, 2000);     // pin d6 yaw
  
  //writing pwm signal by 'writeMicroseconds' 
  servo1.writeMicroseconds(servo_angles[0]);  //throttle
  servo2.writeMicroseconds(servo_angles[1]);  //roll
  servo3.writeMicroseconds(servo_angles[2]);  //roll - I know, I could use servo_angles[1] but I need some extra movement with this code.
  servo4.writeMicroseconds(servo_angles[3]);  //pitch
  servo5.writeMicroseconds(servo_angles[4]);  //yaw
}
