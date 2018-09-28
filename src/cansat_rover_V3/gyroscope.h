#include "quaternionFilters.h"
#include "MPU9250.h"

// AHRS = Attitude Heading Reference System

#define AHRS true         // Set to false for basic data read
#define SER_DEBUG  NO  // Set to true to get Serial output for debugging

// Pin definitions
//int intPin = 14;  // These can be changed, 2 and 3 are the Arduinos ext int pins

MPU9250 myIMU;

char Orientation[30], MPUtemperature[14];

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high

void mpu_init()
{
  //pinMode(intPin, INPUT_PULLUP);
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  if (c == 0x71)
     {
      Serial.println(F("Gyroscope OK."));
      bitSet(hw_status, GYROSCOPE);
      //Αρχικοποίηση IMU
      myIMU.initMPU9250();
      byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
      Serial.print(F("AK8963 address: "));
      Serial.println(d, HEX);
      // Get magnetometer calibration from AK8963 ROM
      myIMU.initAK8963(myIMU.factoryMagCalibration);
      // Initialize device for active mode read of magnetometer
      Serial.println("AK8963 initialized for active data mode....");
      #if SER_DEBUG
          //  Serial.println("Calibration values: ");
          Serial.print("X-Axis factory sensitivity adjustment value ");
          Serial.println(myIMU.factoryMagCalibration[0], 2);
          Serial.print("Y-Axis factory sensitivity adjustment value ");
          Serial.println(myIMU.factoryMagCalibration[1], 2);
          Serial.print("Z-Axis factory sensitivity adjustment value ");
          Serial.println(myIMU.factoryMagCalibration[2], 2);
      #endif
      // Get sensor resolutions, only need to do this once
      myIMU.getAres();
      myIMU.getGres();
      myIMU.getMres();

      // The next call delays for 4 seconds, and then records about 15 seconds of
      // data to calculate bias and scale.
      /*myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);
      Serial.println("AK8963 mag biases (mG)");
      Serial.println(myIMU.magBias[0]);
      Serial.println(myIMU.magBias[1]);
      Serial.println(myIMU.magBias[2]);
      tx_response("AK mag biases");
      tx_response_r(myIMU.magBias[0]);
      tx_response_r(myIMU.magBias[1]);
      tx_response_r(myIMU.magBias[2]);

      Serial.println("AK8963 mag scale (mG)");
      Serial.println(myIMU.magScale[0]);
      Serial.println(myIMU.magScale[1]);
      Serial.println(myIMU.magScale[2]);
      tx_response("AK mag scale");
      tx_response_r(myIMU.magScale[0]);
      tx_response_r(myIMU.magScale[1]);
      tx_response_r(myIMU.magScale[2]);   
      delay(2000); // Add delay to see results before serial spew of data*/

      //Μόνιμη βαθμονόμηση για κάποιο μέρος
      myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
      myIMU.getMres();
      myIMU.magBias[0] = -1000.;  // User environmental x-axis correction in milliGauss, should be automatically calculated
      myIMU.magBias[1] = -530.;  // User environmental x-axis correction in milliGauss    
      myIMU.magBias[2] = -66.;  // User environmental x-axis correction in milliGauss
      // Calculate the magnetometer values in milliGauss
      // Include factory calibration per data sheet and user environmental corrections
      //myIMU.mx=(float)myIMU.magCount[0] * myIMU.mRes * myIMU.magCalibration[0] – myIMU.magBias[0];
      myIMU.mx=(float)myIMU.magCount[0] * myIMU.mRes * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
      myIMU.mx=(float)myIMU.magCount[1] * myIMU.mRes * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
      myIMU.mx=(float)myIMU.magCount[2] * myIMU.mRes * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];


      #if SER_DEBUG
          Serial.println("Magnetometer:");
          Serial.print("X-Axis sensitivity adjustment value ");
          Serial.println(myIMU.factoryMagCalibration[0], 2);
          Serial.print("Y-Axis sensitivity adjustment value ");
          Serial.println(myIMU.factoryMagCalibration[1], 2);
          Serial.print("Z-Axis sensitivity adjustment value ");
          Serial.println(myIMU.factoryMagCalibration[2], 2);
      #endif
     }
  else
     Serial.print(F("Gyroscope Initialization failed"));
}

void get_orientation()
{
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
     {
      myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
      // Now we'll calculate the accleration value into actual g's
      // This depends on scale being set
      myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes; // - myIMU.accelBias[0];
      myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes; // - myIMU.accelBias[1];
      myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes; // - myIMU.accelBias[2];
      myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
      // Calculate the gyro value into actual degrees per second
      // This depends on scale being set
      myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
      myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
      myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;
      myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
      // Calculate the magnetometer values in milliGauss
      // Include factory calibration per data sheet and user environmental
      // corrections
      // Get actual magnetometer value, this depends on scale being set
      myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
      myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
      myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];
     } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  myIMU.updateTime();
  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);

  /*MadgwickQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);*/

  // With these settings the filter is updating at a ~145 Hz rate using the
  // Madgwick scheme and >200 Hz using the Mahony scheme even though the
  // display refreshes at only 2 Hz. The filter update rate is determined
  // mostly by the mathematical steps in the respective algorithms, the
  // processor speed (8 MHz for the 3.3V Pro Mini), and the magnetometer ODR:
  // an ODR of 10 Hz for the magnetometer produce the above rates, maximum
  // magnetometer ODR of 100 Hz produces filter update rates of 36 - 145 and
  // ~38 Hz for the Madgwick and Mahony schemes, respectively. This is
  // presumably because the magnetometer read takes longer than the gyro or
  // accelerometer reads. This filter update rate should be fast enough to
  // maintain accurate platform orientation for stabilization control of a
  // fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
  // produced by the on-board Digital Motion Processor of Invensense's MPU6050
  // 6 DoF and MPU9150 9DoF sensors. The 3.3 V 8 MHz Pro Mini is doing pretty
  // well!

  if (!AHRS)  //Αν είναι να εμφανίσει μόνο τα βασικά
     {
      myIMU.delt_t = millis() - myIMU.count;
      if (myIMU.delt_t > 500)
         {
          #if SER_DEBUG
             // Print acceleration values in milligs!
             Serial.print("X-acceleration: "); Serial.print(1000 * myIMU.ax);
             Serial.print(" mg ");
             Serial.print("Y-acceleration: "); Serial.print(1000 * myIMU.ay);
             Serial.print(" mg ");
             Serial.print("Z-acceleration: "); Serial.print(1000 * myIMU.az);
             Serial.println(" mg ");

             // Print gyro values in degree/sec
             Serial.print("X-gyro rate: "); Serial.print(myIMU.gx, 3);
             Serial.print(" degrees/sec ");
             Serial.print("Y-gyro rate: "); Serial.print(myIMU.gy, 3);
             Serial.print(" degrees/sec ");
             Serial.print("Z-gyro rate: "); Serial.print(myIMU.gz, 3);
             Serial.println(" degrees/sec");

             // Print mag values in degree/sec
             Serial.print("X-mag field: "); Serial.print(myIMU.mx);
             Serial.print(" mG ");
             Serial.print("Y-mag field: "); Serial.print(myIMU.my);
             Serial.print(" mG ");
             Serial.print("Z-mag field: "); Serial.print(myIMU.mz);
             Serial.println(" mG");

             myIMU.tempCount = myIMU.readTempData();  // Read the adc values
             // Temperature in degrees Centigrade
             myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
             // Print temperature in degrees Centigrade
             Serial.print("Temperature is ");  Serial.print(myIMU.temperature, 1);
             Serial.println(" degrees C");
          #endif
         myIMU.count = millis();
         }
     }
  else //Αλλιώς εμφανίζει σύνθετα όπως θέση Yaw-Pitch-Roll
     {
      // Serial print and/or display at 0.5 s rate independent of data rates
      myIMU.delt_t = millis() - myIMU.count;

      // update LCD once per half-second independent of read rate
     if (myIMU.delt_t > 500)
         {
          #if SER_DEBUG
             Serial.print("ax = ");  Serial.print((int)1000 * myIMU.ax);
             Serial.print(" ay = "); Serial.print((int)1000 * myIMU.ay);
             Serial.print(" az = "); Serial.print((int)1000 * myIMU.az);
             Serial.println(" mg");

             Serial.print("gx = ");  Serial.print(myIMU.gx, 2);
             Serial.print(" gy = "); Serial.print(myIMU.gy, 2);
             Serial.print(" gz = "); Serial.print(myIMU.gz, 2);
             Serial.println(" deg/s");

             Serial.print("mx = ");  Serial.print((int)myIMU.mx);
             Serial.print(" my = "); Serial.print((int)myIMU.my);
             Serial.print(" mz = "); Serial.print((int)myIMU.mz);
             Serial.println(" mG");

             Serial.print("q0 = ");  Serial.print(*getQ());
             Serial.print(" qx = "); Serial.print(*(getQ() + 1));
             Serial.print(" qy = "); Serial.print(*(getQ() + 2));
             Serial.print(" qz = "); Serial.println(*(getQ() + 3));
           #endif

          myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ()
                              * *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1)
                              * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) - *(getQ()+3)
                              * *(getQ()+3));
          myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ()
                              * *(getQ()+2)));
          myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2)
                              * *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1)
                              * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) + *(getQ()+3)
                              * *(getQ()+3));
          myIMU.pitch *= RAD_TO_DEG;
          myIMU.yaw   *= RAD_TO_DEG;

          // Απόκλιση στην Πρέβεζα (38.952N 20.743E) is
          //   4° 19' W  ± 0.32 
          // - http://www.ngdc.noaa.gov/geomag-web/#declination
          myIMU.yaw  += 4.2;
          myIMU.roll *= RAD_TO_DEG;
          #if SER_DEBUG
              Serial.print("Yaw, Pitch, Roll: ");
              Serial.print(myIMU.yaw, 2);
              Serial.print(", ");
              Serial.print(myIMU.roll, 2);
              Serial.print(", ");
              Serial.println(myIMU.pitch, 2);

              Serial.print("rate = ");
              Serial.print((float)myIMU.sumCount / myIMU.sum, 2);
              Serial.println(" Hz");
          #endif
             
          char yaw_buffer[8];
          char pitch_buffer[8];
          char roll_buffer[8];
          char temper_buffer[8];
          myIMU.tempCount = myIMU.readTempData();  // Read the adc values
          // Temperature in degrees Centigrade
          myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;       
          dtostrf(myIMU.yaw, 6, 1, yaw_buffer); //7,2 - Μέγιστο μήκος μαζί με '-' και '.' δηλ. -12.5 και μετά 2 δεκαδικά ψηφία
          dtostrf(myIMU.pitch, 6, 1, pitch_buffer); 
          dtostrf(myIMU.roll, 6, 1, roll_buffer);
          float mag1=sqrt(pow(myIMU.mx, 2)+pow(myIMU.my,2)+pow(myIMU.mz,2))/10;
          //dtostrf(myIMU.temperature, 5, 1, temper_buffer); //6,2
          dtostrf(mag1, 5, 1, temper_buffer); //6,2
          sprintf(Orientation, "%s;%s;%s;%s", roll_buffer, pitch_buffer, yaw_buffer, temper_buffer);  //Ένωσε τα strings
          //Serial.println(Orientation); //debug

          
          //Serial.print("X-mag field: "); Serial.print(mag1);Serial.println(" uT");

          
          myIMU.count = millis();
          myIMU.sumCount = 0;
          myIMU.sum = 0;
        }
    }
}

void Calibration()
{
  // The next call delays for 4 seconds, and then records about 15 seconds of
      // data to calculate bias and scale.
      tx_response(PSTR("- MPU Calibration -"));
      myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);
      #if SER_DEBUG
         Serial.println("AK8963 mag biases (mG)");
         Serial.println(myIMU.magBias[0]);
         Serial.println(myIMU.magBias[1]);
         Serial.println(myIMU.magBias[2]);
      #endif
      tx_response(PSTR("AK mag biases"));
      tx_response_r(myIMU.magBias[0]);
      tx_response_r(myIMU.magBias[1]);
      tx_response_r(myIMU.magBias[2]);
      #if SER_DEBUG
         Serial.println("AK8963 mag scale (mG)");
         Serial.println(myIMU.magScale[0]);
         Serial.println(myIMU.magScale[1]);
         Serial.println(myIMU.magScale[2]);
      #endif
      tx_response(PSTR("AK mag scale"));
      tx_response_r(myIMU.magScale[0]);
      tx_response_r(myIMU.magScale[1]);
      tx_response_r(myIMU.magScale[2]);   
      delay(2000); // Add delay to see results before serial spew of data
}

