/*
 * 
 * Hall Effect Sensor Toy
 * a1302 ratiometric B field
 *
*/
#include <Arduino.h>

// Macro used for determining a mag field polarity
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // https://forum.arduino.cc/index.php?topic=627179.0

// Macro used for determining a mag field polarity
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // https://forum.arduino.cc/index.php?topic=627179.0
#define a1302_DATA 0
#define a1302_quiescentVOLTAGE 2.48 // Manually read from sensor and placed here
float a1302_Voltage = 0;             // init
float a1302_deltaVoltage = 0;        // from quiescent voltage

// Determined by specifications provided by Allegro Microsystems (likely not accurate for all a1302 sensors).
// REFERENCE: http://robotics.itee.uq.edu.au/~elec3004/laboratories/LeviLab/Hall%20Effect%20Sensor%20A1301-2-Datasheet.pdf
#define a1302_allegroSENSITIVITY (1.3/1000)*0.2  // Volts per Gauss (p.4) with a 0.2 eyeball correction for this version using Velleman vma313 sensor (30G trigger)
int a1302_allegroGauss = 0;                      // Calculated Below
int a1302_allegroPolarity = 0;                   // init determined below

// Determined by Arduino documentation.
// Voltage Scale: +5V
// MAX:1023 units of resolution
// REFERENCE: https://www.arduino.cc/en/Tutorial/AnalogInputPins
#define analogVoltageScale 5
#define analogVoltageResolution 1023
String a1302_allegroPolarityString = "";         // init determined below

void setup() {
}
void loop() {
  //
  //   _______
  //  |       |============
  //  | a1302 |============
  //  |_______|============
  //
  //
  //
  //
  // a1302: Read Voltage from the a1302 sensor
  //
  //
  a1302_Voltage = (float)( analogRead(a1302_DATA) ); // float casted analog pin voltage

  //
  //
  // a1302: Voltage value processing
  //
  //
  a1302_Voltage = a1302_Voltage * analogVoltageScale / analogVoltageResolution;
  a1302_deltaVoltage = (a1302_Voltage-a1302_quiescentVOLTAGE);
  a1302_deltaVoltage = ((int)(a1302_deltaVoltage * 100)/100.0); // truncate float after 2 decimal places (https://forum.arduino.cc/index.php?topic=488560.0)

  //
  //
  // a1302: Magnetic Field quantities
  //
  //
  // Determines North/South Polarity of the magnetic field
  a1302_allegroPolarity = sign(a1302_deltaVoltage);
  
  // Determines the magnitude of the magnetic field based on Allegro Microsystems datasheet (see above)
  a1302_allegroGauss = (a1302_allegroPolarity)*(int)( (abs(a1302_deltaVoltage)) / a1302_allegroSENSITIVITY );

  int f = 400 + (a1302_allegroGauss);
  tone(7,f);

  delay(7);
}
