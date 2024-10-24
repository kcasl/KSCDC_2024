#include <MPU6050_tockn.h>

#include <Wire.h>
#include "MPU6050.h"
// Using the GY-521 breakout board, I set ADO to 0 by grounding through a 4k7 resistor
// Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1

// Pin definitions (핀과 변수 정의입니다)
int intPin = 1;  // This can be changed, 2 and 3 are the Arduinos ext int pins
int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
float ax, ay, az;                // Stores the real accel value in g's
int16_t gyroCount[3];            // Stores the 16-bit signed gyro sensor output
float gx, gy, gz;                // Stores the real gyro value in degrees per seconds
float gyroBias[3], accelBias[3]; // Bias corrections for gyro and accelerometer
int16_t tempCount;               // Stores the internal chip temperature sensor output 
float temperature;               // Scaled temperature in degrees Celsius
float SelfTest[6];               // Gyro and accelerometer self-test sensor output
uint32_t count = 0;
float aRes, gRes; // scale resolutions per LSB for the sensors
MPU6050lib mpu;

void setup()    // setup부분입니다.
{
  Wire.begin();
  Serial.begin(9600);
  
  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(intPin, INPUT);    // 인터럽트핀 설정이라는데 인터럽트는 솔직히 잘 모르겠습니다
  digitalWrite(intPin, LOW); // 나중에 각잡고 공부좀 해야겠습니다.
  
  Serial.println("MPU6050");
  Serial.println("6-DOF 16-bit");
  Serial.println("motion sensor");
  Serial.println("60 ug LSB");
 
   // Read the WHO_AM_I register, this is a good test of communication
  // 주소를 읽고 16진수 68인지 확인합니다
  uint8_t c = mpu.readByte(MPU6050_ADDRESS, WHO_AM_I_MPU6050);  // Read WHO_AM_I register for MPU-6050
  Serial.print("I AM ");
  Serial.print(c, HEX);  
  Serial.print(" I Should Be ");
  Serial.println(0x68, HEX); 

  // 주소가 맞으면 초기화합니다
  if (c == 0x68) // WHO_AM_I should always be 0x68
  {  
    Serial.println("MPU6050 is online...");
    
    mpu.MPU6050SelfTest(SelfTest); // Start by performing self test and reporting values
    Serial.print("x-axis self test: acceleration trim within : "); Serial.print(SelfTest[0],1); Serial.println("% of factory value");
    Serial.print("y-axis self test: acceleration trim within : "); Serial.print(SelfTest[1],1); Serial.println("% of factory value");
    Serial.print("z-axis self test: acceleration trim within : "); Serial.print(SelfTest[2],1); Serial.println("% of factory value");
    Serial.print("x-axis self test: gyration trim within : "); Serial.print(SelfTest[3],1); Serial.println("% of factory value");
    Serial.print("y-axis self test: gyration trim within : "); Serial.print(SelfTest[4],1); Serial.println("% of factory value");
    Serial.print("z-axis self test: gyration trim within : "); Serial.print(SelfTest[5],1); Serial.println("% of factory value");

    if(SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
    Serial.println("Pass Selftest!");  
      
    mpu.calibrateMPU6050(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers  
    mpu.initMPU6050(); Serial.println("MPU6050 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature

   }
   else
   {
    Serial.print("Could not connect to MPU6050: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
   }

  }
}

void loop()
{  
  // If data ready bit set, all data registers have new data
  if(mpu.readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt
   // 가속도값을 읽고
    mpu.readAccelData(accelCount);  // Read the x/y/z adc values
    aRes=mpu.getAres(); 
    // 실제 값으로 환산합니다
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes - accelBias[1];   
    az = (float)accelCount[2]*aRes - accelBias[2];  
   // 자이로값을 읽고
    mpu.readGyroData(gyroCount);  // Read the x/y/z adc values
    gRes=mpu.getGres();
    // 실제 값으로 환산합니다
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes - gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes - gyroBias[1];  
    gz = (float)gyroCount[2]*gRes - gyroBias[2];   

    // 온도값을 읽고 섭씨온도로 환산합니다
    tempCount = mpu.readTempData();  // Read the x/y/z adc values
    temperature = ((float) tempCount) / 340. + 36.53; // Temperature in degrees Centigrade
   }  
   
    uint32_t deltat = millis() - count;
    if(deltat > 500) {  // 0.5초 마다 한번씩 출력합니다 (2Hz)
 
    // Print acceleration values in milligs!
    Serial.print("X-acceleration: "); Serial.print(1000*ax); Serial.print(" mg "); 
    Serial.print("Y-acceleration: "); Serial.print(1000*ay); Serial.print(" mg "); 
    Serial.print("Z-acceleration: "); Serial.print(1000*az); Serial.println(" mg"); 
 
    // Print gyro values in degree/sec
    Serial.print("X-gyro rate: "); Serial.print(gx, 1); Serial.print(" degrees/sec "); 
    Serial.print("Y-gyro rate: "); Serial.print(gy, 1); Serial.print(" degrees/sec "); 
    Serial.print("Z-gyro rate: "); Serial.print(gz, 1); Serial.println(" degrees/sec"); 
    
   // Print temperature in degrees Centigrade      
    Serial.print("Temperature is ");  Serial.print(temperature, 2);  Serial.println(" degrees C"); // Print T values to tenths of s degree C
    Serial.println("");
        
    count = millis();
    }

}