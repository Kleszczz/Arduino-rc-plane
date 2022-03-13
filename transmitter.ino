// 4 channels transmmiter - https://github.com/Kleszcz1337
// throttle, pitch, roll, yaw
// If you would like to have more channel(no more then 8, cause by number of GPIO pins and Analog inputs)
// Add it in section: Signal, ResetData and in Loop.

#include <SPI.h>
#include <RF24.h> 

const uint64_t pipeOut = 0xE9E8F0F0E1LL; //Copy it into your reciver code, if you have changed it. If not, don't do anything.
RF24 radio(7, 8); //Pins 7D & D8 will be use to send data from transmitter.

struct Signal { //Simple data structure(something like class), we can use type byte because we will by using 0/12 - 1020 interval only.
byte throttle;
byte pitch;
byte roll;
byte yaw;
};

Signal data; //Creating instance of structure 

void ResetData() 
{
data.throttle = 127; //motor should stop at 520~ but 127 works only(copy from internet)
data.pitch = 127;
data.roll = 127;
data.yaw = 127;
}

void setup()
{
radio.begin();
radio.openWritingPipe(pipeOut);
radio.stopListening(); //after this execute we have successfully connected to our reciver
ResetData();
}

//copied from internet(likely it increase precision due to reducing the interval)
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper); //constrain makes our val not to be below 'lower and greater then 'upper'
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}


void loop()
{
  data.throttle = mapJoystickValues( analogRead(A1), 523, 524, 1020, false ); //if your motor spins after powering, change false for true, also 1020 to 1015(idk why)
  data.roll = mapJoystickValues( analogRead(A0), 0, 524, 1020, true );       // for reverse the servo just change true to false
  data.pitch = mapJoystickValues( analogRead(A2), 0, 524, 1020, true );    
  data.yaw = mapJoystickValues( analogRead(A3), 0, 524, 1020, true );      

  radio.write(&data, sizeof(Signal)); //sending data to transmitter
}
