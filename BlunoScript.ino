#include <Wire.h>
#include "AD5933.h"
#define START_FREQ (30000)  //changed from 80k, 30k deemed optimal target impedance range
#define FREQ_INCR (1000)    //increases used primarily for two point calibration              
#define NUM_INCR (40)

double gain[NUM_INCR+1];  //stores the calibrated gain across all frequencies
int phase[NUM_INCR+1];

//used by previous team member for data translation
//not used by 2021-2022 team member
union {
  byte array[8];
  double bigNum;
} myUnion;

void setup(void)
{
  
  // Begin I2C, sets appropriate bluetooth signal
  Serial.begin(115200);
  long REF_RESIST=(long)2220;   //IMPORTANT: CALIBRATION RESISTANCE SET HERE
  Wire.begin();               //Establish connection with impedance analyzer
  
  // Perform initial configuration. Fail if any one of these fail.
  
  if (!(AD5933::reset() &&
  AD5933::setInternalClock(true) &&
  AD5933::setStartFrequency(START_FREQ) &&
  AD5933::setIncrementFrequency(FREQ_INCR) &&
  AD5933::setNumberIncrements(NUM_INCR) &&
  AD5933::setPGAGain(PGA_GAIN_X1)))
  {
  while (true){
     Serial.print("Failed startup. Unpower and repower arduino.");
  }
  
  }
  
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
  frequencySweepRaw();
  delay(200); //can be decreased or increased as necessary
              //too fast and the results will report to the screen and become illegible
              //too slow, recording data points would take forever
              //value up for deliberation
}

void frequencySweepRaw() {
  // Create variables to hold the impedance data and track frequency
  
  int real, imag, i = 0, cfreq = START_FREQ/1000;
  double impedance;
  
  // Initialize the frequency sweep
  if (!(AD5933::setPowerMode(POWER_STANDBY) && // place in standby
  AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
  AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
  {
  }

  // Perform the actual sweep
  while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) !=
  STATUS_SWEEP_DONE) {

  // Get the frequency data for this frequency point
    if (!AD5933::getComplexData(&real, &imag)) {
    }
  }
  
  // Compute impedance
  double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
  if(i==0){   //only focusing on one point calibration here
              //analyze more than i==0 for more frequencies
    
    //Serial.print("Magnitude: ");      <--- commented out lines useful for debugging
    //Serial.print(magnitude);          <--- easy to detect null magnitudes or extreme gain
    //Serial.print("\n");
    //Serial.print("Gain Factor: ");
    //Serial.print(gain[i],16);
    //Serial.print("\n");
    Serial.print("Impedance: ");
    Serial.print(1/(magnitude*gain[i]));
    Serial.print("\n");

    //Serial.print is helpful function to relay to onSerialReceived in Android Studio
  }
  
  impedance = 1/(magnitude*gain[i]);
  // Increment the frequency
  i++;  //irrelevant when doing one frequency point
        //more tests needed to determine if two point is better
  cfreq += FREQ_INCR/1000;
  AD5933::setControlMode(CTRL_INCREMENT_FREQ);
  

}

//previous team member attempt at sending impedance data
//determined best to send as a string for manual analysis
//computational analysis for further improvement may require integer type

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
