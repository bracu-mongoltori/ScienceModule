#include "HX711.h"

#define SIGNAL A0 // moisture sensor

#define CALIBRATION_SAMPLES 50

#define gas A1 // gas sensor

// S0, S1, S2, S3, SIG
int color_sensors[3][5] = {{2, 3, 4, 5, 7}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};

int calib[3][6];

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN_1 = 2;
const int LOADCELL_SCK_PIN_1 = 3;

const int LOADCELL_DOUT_PIN_2 = 4;
const int LOADCELL_SCK_PIN_2 = 5;

const int LOADCELL_DOUT_PIN_3 = 6;
const int LOADCELL_SCK_PIN_3 = 7;

void setup() {
  
  Serial.println("Initializing the scale");

  scale1.begin(LOADCELL_DOUT_PIN_1, LOADCELL_SCK_PIN_1);
  scale1.set_scale(390.909091);

  scale2.begin(LOADCELL_DOUT_PIN_2, LOADCELL_SCK_PIN_2);
  scale2.set_scale(1839.09091);

  scale3.begin(LOADCELL_DOUT_PIN_3, LOADCELL_SCK_PIN_3); 
  scale3.set_scale(317);
  
  for (int i = 0; i++; i < 3){
    for (int j = 0; j++; j < 4){
      pinMode(color_sensors[i][j], OUTPUT);
    }
    pinMode(color_sensors[i][4], INPUT);
  }
  
  for (int i = 0; i++; i < 3){
    digitalWrite(color_sensors[i][0], HIGH);
    digitalWrite(color_sensors[i][1], LOW);
  }

  pinMode(SIGNAL, INPUT); // moisture

  Serial.begin(9600);

  getCalibration(color_sensors[0], calib[0]);
  getCalibration(color_sensors[1], calib[1]);
  getCalibration(color_sensors[2], calib[2]);

  delay(1500);
}

void loop() {
  int r1 = getR(color_sensors[0], calib[0]), g1 = getG(color_sensors[0], calib[0]), b1 = getB(color_sensors[0], calib[0]);
  int r2 = getR(color_sensors[1], calib[1]), g2 = getG(color_sensors[1], calib[1]), b2 = getB(color_sensors[1], calib[1]);
  int r3 = getR(color_sensors[2], calib[2]), g3 = getG(color_sensors[2], calib[2]), b3 = getB(color_sensors[2], calib[2]);

  char buf[30];
  sprintf(buf, "C1 Red: %d, Green: %d, Blue: %d, Hue: %d", r1, g1, b1);
  sprintf(buf, "C2 Red: %d, Green: %d, Blue: %d, Hue: %d", r2, g2, b2);
  sprintf(buf, "C3 Red: %d, Green: %d, Blue: %d, Hue: %d", r3, g3, b3);
  Serial.println(buf);

  int MOIST = get_moisture();
  Serial.print("Moisture Level");
  Serial.println(MOIST);

  Serial.print("(1st scale) one reading:\t");
  Serial.print(scale1.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale1.get_units(10), 5);  
  
  Serial.print("(2nd scale) one reading:\t");
  Serial.print(scale2.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale2.get_units(10), 5);  
  
  Serial.print("(3rd scale) one reading:\t");
  Serial.print(scale3.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale3.get_units(10), 5);

  int gas_level = get_gas_sensor();
  
  Serial.print("gas sensor level ");
  Serial.println(gas_level);

  delay(500);

}

void getCalibration(int * sensor_pins, int * range) {
  Serial.println("\n======================");
  Serial.println(" White Surface Expose ");
  Serial.println("======================");

  delay(1500);
  Serial.println("\nStarting...\n");
  delay(500);

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    range[0] += getRawR(sensor_pins);
    range[1] += getRawG(sensor_pins);
    range[2] += getRawB(sensor_pins);
    delay(15);
  }

  for (int i = 0; i < 3; i++) range[i] = range[i] / CALIBRATION_SAMPLES;

  Serial.println("======================");
  Serial.println(" Black Surface Expose ");
  Serial.println("======================");

  delay(1500);
  Serial.println("\nStarting...\n");
  delay(500);

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    range[3] += getRawR(sensor_pins);
    range[4] += getRawG(sensor_pins);
    range[5] += getRawB(sensor_pins);
    delay(15);
  }

  for (int i = 3; i < 6; i++) range[i] = range[i] / CALIBRATION_SAMPLES;
}

int getR(int * sensor_pins, int * calib) {
  return constrain(map(getRawR(sensor_pins), calib[0], calib[3], 255, 0), 0, 255);
}

int getG(int * sensor_pins, int * calib) {
  return constrain(map(getRawG(sensor_pins), calib[1], calib[4], 255, 0), 0, 255);
}

int getB(int * sensor_pins, int * calib) {
  return constrain(map(getRawB(sensor_pins), calib[2], calib[5], 255, 0), 0, 255);
}

int getRawR(int * sensor_pins) {
  setFilter(sensor_pins, LOW, LOW);
  return getPulse(sensor_pins);
}

int getRawG(int * sensor_pins) {
  setFilter(sensor_pins, HIGH, HIGH);
  return getPulse(sensor_pins);
}

int getRawB(int * sensor_pins) {
  setFilter(sensor_pins, LOW, HIGH);
  return getPulse(sensor_pins);
}

int getPulse(int * sensor_pins) {
  return (int)pulseIn(sensor_pins[5], HIGH);
}

int get_moisture() {
  int water_level = analogRead(SIGNAL);
  //Serial.println(water_level);
  return water_level;
}

void setFilter(int * sensor_pins, int x, int y) {
  digitalWrite(sensor_pins[2], x);
  digitalWrite(sensor_pins[3], y);
}

int get_gas_sensor(){
  return analogRead(gas);
}
