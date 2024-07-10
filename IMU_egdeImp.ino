#include "LSM6DS3.h"
#include "Wire.h"
#include "SPI.h"

uint16_t errors = 0;

LSM6DS3Core sensor(I2C_MODE, 0x6A);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);


  if(sensor.beginCore() != 0){
    Serial.print("\nDevice error\n");
  } else {
    Serial.print("\nDevice setup complete\n");
  }

  uint8_t settings = 0;
  //bandwith of the anti-aliasing filter 00- 400hz 01- 200hz 10-100hz 11- 50hz
  settings |= 2; //100hz
  //acelerometer range 00: -2g to 2g 01: -16g to 16g 10: -4g to 4g 11: -8g to 8g 
  settings |= (3 << 2); // -8 a 8
  //output rate (tabela no manual). normal para movimento humano é cerca de 100hz - 0100
  settings |= (4 << 4); //104hz

  errors += sensor.writeRegister(0x10, settings); //0x10 = address CTRL1_XL



  settings = 0;
  //full scale selection 00:250dps 01: 500dps, 10: 1000dps, 11:2000dps
  settings |= (1<<2); //300 to 1200 -> 500dps
  //output rate (tabela no manual). normal para movimento humano é cerca de 100hz - 0100
  settings |= (4 << 4);//104hz

  errors += sensor.writeRegister(0x11, settings); //0x11 = address CTRL2_G

}

void loop() {
  // put your main code here, to run repeatedly:
  int16_t temp;
    //Get all parameters
    //Serial.print("\nAccelerometer Counts:\n");

    //Acelerometer axis X
    if (sensor.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTX_L_XL) != 0) {
        errors++;
    }
    //Serial.print(" acellX = ");
    Serial.print(temp);
    Serial.print(",");


    //Acelerometer axis Y
    if (sensor.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTY_L_XL) != 0) {
        errors++;
    }
    //Serial.print(" acellY = ");
    Serial.print(temp);
    Serial.print(",");


    //Acelerometer axis Z
    if (sensor.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTZ_L_XL) != 0) {
        errors++;
    }
    //Serial.print(" acellZ = ");
    Serial.print(temp);
    Serial.print(",");




    if (sensor.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTX_L_G) != 0) {
        errors++;
    }
    //Serial.print(" gyroX = ");
    Serial.print(temp);
    Serial.print(",");


    //Gyroscope axis Y
    if (sensor.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTY_L_G) != 0) {
        errors++;
    }
    //Serial.print(" gyroY = ");
    Serial.print(temp);
    Serial.print(",");


    //Gyroscope axis Z
    if (sensor.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTZ_L_G) != 0) {
        errors++;
    }
    //Serial.print(" gyroZ = ");
    Serial.print(temp);
    Serial.print("\n");


    /*Serial.println();
    Serial.print("Total reported Errors and Warnings: ");
    Serial.println(errors);
    */

    delay(1000);

}
