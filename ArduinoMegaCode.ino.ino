#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define FORCE_SENSOR_PIN A0 // the FSR and 10K pulldown are connected to A0
#define FORCE_SENSOR_PIN1 A2 // the FSR and 10K pulldown are connected to A0
//global variables for measuring starting values of velostat for comparison
double initVal;
double initVal1;
//mpu6050 variable
Adafruit_MPU6050 mpu;

void setup() {
  //initialize serial monitor on baud 115200
    //Serial.begin(115200);
    Serial2.begin(115200);
    
    Wire.begin();

    while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens


  // Initialize and connect to MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  //change config of MPU6050 accelerometer and gyrometer range
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  //initialize starting values from velostat
  initVal = analogRead(FORCE_SENSOR_PIN);
  initVal1 = analogRead(FORCE_SENSOR_PIN1);
}

void loop() {
  //read from the velostat strips
  int analogReading = analogRead(FORCE_SENSOR_PIN);
  int analogReading1 = analogRead(FORCE_SENSOR_PIN1);
  
  //set up variables to interact with gyrometer
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //measure the rotation speed of the gyrometer
  double deltaRotation = abs(g.gyro.x + g.gyro.y + g.gyro.z);
  //measure the temperature inside the box.
  double tempCelsius = temp.temperature;
  //measure the pressure on the velostat
  double damageCalc = abs(analogReading - initVal);
  double damageCalc1 = abs(analogReading1 - initVal1);
  //if any of these values exceed these parameters, a signal is sent through the serial monitor
  if(damageCalc1 > 60 || damageCalc > 60 || deltaRotation > 2 || tempCelsius > 30){
    Serial2.println("send");
  }
  //otherwise, do nothing

  //mandated delay for serial communication
  delay(1500);
  
}

