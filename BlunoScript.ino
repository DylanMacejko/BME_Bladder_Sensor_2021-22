#include <Wire.h>
#include "AD5933.h"
#define START_FREQ (30000)//changed from 80k    //originally 30000
#define FREQ_INCR (1000)                            //originally 1000
#define NUM_INCR (40)                           //originally 40

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

int gaintest;
int magtest;
double midgain;


union {
  byte array[8];
  double bigNum;
} myUnion;

void setup(void)
{
  
  // Begin I2C
  Serial.begin(115200);
  long REF_RESIST=(long)2220;
  Wire.begin();
  
  // Perform initial configuration. Fail if any one of these fail.
  /*
  if(!AD5933::reset()){
    while(1)
      Serial.write("Reset fails");
  }

  while(1){
    Serial.write("Made it to the end of reset checks\n");
  }
  if(!AD5933::setInternalClock(true)){
    while(1)
      Serial.write("Internal Clock fails");
  }
  if(!AD5933::setStartFrequency(START_FREQ)){
    while(1)
      Serial.write("Start Frequency fails");
  }
  if(!AD5933::setIncrementFrequency(FREQ_INCR)){
    while(1)
      Serial.write("Increment Frequency fails");
  }
  if(!AD5933::setNumberIncrements(NUM_INCR)){
    while(1)
      Serial.write("Number Increments fails");
  }

  if(!AD5933::setPGAGain(PGA_GAIN_X1)){
    while(1)
      Serial.write("PGA Gain fails");
  }

  while(1){
    Serial.write("Made it to the end of six checks\n");
  }

  */
  /*
  while(1){
    Serial.write("Capable of talking\n");
  }
  */
  if (!(AD5933::reset() &&
  AD5933::setInternalClock(true) &&
  AD5933::setStartFrequency(START_FREQ) &&
  AD5933::setIncrementFrequency(FREQ_INCR) &&
  AD5933::setNumberIncrements(NUM_INCR) &&
  AD5933::setPGAGain(PGA_GAIN_X1)))
  {
  while (true){
     Serial.write("Failed attempt");
  }
  
  }
  
/*
  while(1){
    Serial.write("Made it to the end of setup\n");
    }
    */
// Perform calibration sweep
    if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1)){
    }
    else;
   midgain=0;
   int slope = (gain[0]+gain[2])/2000;
   midgain = gain[0] + slope*1000;
}

    
void loop(void)
{
  //Serial.write("Ran frequency sweep/n");
  frequencySweepRaw();
  
  // Delay
  delay(200); //safe to decrease?
}

void frequencySweepRaw() {
  // Create variables to hold the impedance data and track frequency
  //while(1){
  //  Serial.write("Got to one");
  //}
  //Serial.write("Hello/n");
  
  int real, imag, i = 0, cfreq = START_FREQ/1000;
  double impedance;
  // Initialize the frequency sweep
  if (!(AD5933::setPowerMode(POWER_STANDBY) && // place in standby
  AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
  AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
  {
  }
  //Serial.write("Got to two");
  // Perform the actual sweep
while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) !=
STATUS_SWEEP_DONE) {

  // Get the frequency data for this frequency point
  if (!AD5933::getComplexData(&real, &imag)) {
  }

  bool calib = true;
  double magtesting;
  double gaintesting;
  while(calib){
    double magtesting = sqrt(pow(real, 2) + pow(imag, 2));
    double gaintesting = 1/(1000*magtesting);
    calib = false;
  }
  
  
  // Compute impedance
  double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
  if(i==0){
    
    //Serial.print("Magnitude: ");
    //Serial.print(magnitude);
    //Serial.print("\n");
    //Serial.print("Gain Factor: ");
    //Serial.print(gain[i],16);
    //Serial.print("\n");
    Serial.print("Impedance: ");
    Serial.print(1/(magnitude*gain[i]));
    Serial.print("\n");
    
    /*
    Serial.print("Impedance with our gain: ");
    Serial.print(1/(magtesting*gaintesting));
    Serial.print("\n");
    //delay();
    */
  }
  
  impedance = 1/(magnitude*gain[i]);
  // Increment the frequency
  i++;
  cfreq += FREQ_INCR/1000;
  AD5933::setControlMode(CTRL_INCREMENT_FREQ);
  

}

//Serial.write("Got to three");
/*
myUnion.bigNum = impedance;
for (int i = 0; i < sizeof(double); i++) {
  Serial.write(myUnion.array[i]); // Send to wherever
  if(i!=3){
    Serial.write(".");
  }
}
if (!AD5933::setPowerMode(POWER_STANDBY))
;
*/

}
