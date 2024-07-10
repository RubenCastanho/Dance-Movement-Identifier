/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>
#include "LSM6DS3.h"
#include "Wire.h"

#define KNNK 3 //k do knn, deve ser impar
#define MAX_PRPH_CONNECTION   2
uint8_t connection_count = 0;
#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 100
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))
static unsigned long last_interval_ms = 0;

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);  //I2C device address 0x6A

typedef struct imusample{
  uint32_t timestamp;
  float accX;
  float accY;
  float accZ;
  float gyrX;
  float gyrY;
  float gyrZ;

} IMUSample;

typedef struct knndistance{ //estrutura que permite guardar as distancias com a classe associada
  float distance;
  int knnclass;
}KNNDistance;

typedef struct knnsample{   //estrututa de uma amostra que entra no knn, com a amostra analisada em cada eixo e em cada quartil da amostra, com media e variancia
  float accxmean1;
  float accxvar1;
  float accxmean2;
  float accxvar2;
  float accxmean3;
  float accxvar3;
  float accxmean4;
  float accxvar4;
  float accymean1;
  float accyvar1;
  float accymean2;
  float accyvar2;
  float accymean3;
  float accyvar3;
  float accymean4;
  float accyvar4;
  float acczmean1;
  float acczvar1;
  float acczmean2;
  float acczvar2;
  float acczmean3;
  float acczvar3;
  float acczmean4;
  float acczvar4;
  float gyrxmean1;
  float gyrxvar1;
  float gyrxmean2;
  float gyrxvar2;
  float gyrxmean3;
  float gyrxvar3;
  float gyrxmean4;
  float gyrxvar4;
  float gyrymean1;
  float gyryvar1;
  float gyrymean2;
  float gyryvar2;
  float gyrymean3;
  float gyryvar3;
  float gyrymean4;
  float gyryvar4;
  float gyrzmean1;
  float gyrzvar1;
  float gyrzmean2;
  float gyrzvar2;
  float gyrzmean3;
  float gyrzvar3;
  float gyrzmean4;
  float gyrzvar4;
  int knnclass;
}KNNSample;


KNNSample knnTraining[25];    //25 amostras recolhidas previamente






// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }

  Serial.println("Bluefruit52 BLEUART Example");
  Serial.println("---------------------------\n");

  //preenchimento das amostras utilizadas no treino
  int i = 0;
  //sample 1
  knnTraining[i].accxmean1=6.62;
  knnTraining[i].accxvar1=26.8;
  knnTraining[i].accxmean2=1.73;
  knnTraining[i].accxvar2=1.94;
  knnTraining[i].accxmean3=5.59;
  knnTraining[i].accxvar3=11.3;
  knnTraining[i].accxmean4=8.23;
  knnTraining[i].accxvar4=32;
  knnTraining[i].accymean1=8.98;
  knnTraining[i].accyvar1=2.34;
  knnTraining[i].accymean2=7.26;
  knnTraining[i].accyvar2=4.02;
  knnTraining[i].accymean3=0.49;
  knnTraining[i].accyvar3=2.76;
  knnTraining[i].accymean4=10.9;
  knnTraining[i].accyvar4=17.1;
  knnTraining[i].acczmean1=7.4;
  knnTraining[i].acczvar1=3.77;
  knnTraining[i].acczmean2=3.83;
  knnTraining[i].acczvar2=1.63;
  knnTraining[i].acczmean3=6.1;
  knnTraining[i].acczvar3=9.39;
  knnTraining[i].acczmean4=5.39;
  knnTraining[i].acczvar4=26.9;
  knnTraining[i].gyrxmean1=-49.2;
  knnTraining[i].gyrxvar1=7178;
  knnTraining[i].gyrxmean2=123;
  knnTraining[i].gyrxvar2=1535;
  knnTraining[i].gyrxmean3=22.2;
  knnTraining[i].gyrxvar3=6153;
  knnTraining[i].gyrxmean4=-57.6;
  knnTraining[i].gyrxvar4=3544;
  knnTraining[i].gyrymean1=-63.1;
  knnTraining[i].gyryvar1=4024;
  knnTraining[i].gyrymean2=46.8;
  knnTraining[i].gyryvar2=4504;
  knnTraining[i].gyrymean3=89.7;
  knnTraining[i].gyryvar3=471;
  knnTraining[i].gyrymean4=-98.2;
  knnTraining[i].gyryvar4=4776;
  knnTraining[i].gyrzmean1=-8.88;
  knnTraining[i].gyrzvar1=5227;
  knnTraining[i].gyrzmean2=26.9;
  knnTraining[i].gyrzvar2=825;
  knnTraining[i].gyrzmean3=-10.6;
  knnTraining[i].gyrzvar3=1443;
  knnTraining[i].gyrzmean4=-76.4;
  knnTraining[i].gyrzvar4=5036;
  knnTraining[i].knnclass=1;
  i++;
  //sample 2
  knnTraining[i].accxmean1=-2.8;
  knnTraining[i].accxvar1=6.41;
  knnTraining[i].accxmean2=-3.92;
  knnTraining[i].accxvar2=2.68;
  knnTraining[i].accxmean3=3.08;
  knnTraining[i].accxvar3=9.94;
  knnTraining[i].accxmean4=2.6;
  knnTraining[i].accxvar4=10.9;
  knnTraining[i].accymean1=4.05;
  knnTraining[i].accyvar1=2.27;
  knnTraining[i].accymean2=3.71;
  knnTraining[i].accyvar2=2.13;
  knnTraining[i].accymean3=-3.62;
  knnTraining[i].accyvar3=5.85;
  knnTraining[i].accymean4=6.85;
  knnTraining[i].accyvar4=9.45;
  knnTraining[i].acczmean1=13.3;
  knnTraining[i].acczvar1=27.7;
  knnTraining[i].acczmean2=6.74;
  knnTraining[i].acczvar2=7.87;
  knnTraining[i].acczmean3=8.57;
  knnTraining[i].acczvar3=19.8;
  knnTraining[i].acczmean4=11.7;
  knnTraining[i].acczvar4=7.67;
  knnTraining[i].gyrxmean1=52.4;
  knnTraining[i].gyrxvar1=8905;
  knnTraining[i].gyrxmean2=135;
  knnTraining[i].gyrxvar2=2421;
  knnTraining[i].gyrxmean3=-34.9;
  knnTraining[i].gyrxvar3=5202;
  knnTraining[i].gyrxmean4=22.8;
  knnTraining[i].gyrxvar4=5257;
  knnTraining[i].gyrymean1=-39.4;
  knnTraining[i].gyryvar1=10813;
  knnTraining[i].gyrymean2=-6.41;
  knnTraining[i].gyryvar2=5247;
  knnTraining[i].gyrymean3=93;
  knnTraining[i].gyryvar3=1262;
  knnTraining[i].gyrymean4=-28.2;
  knnTraining[i].gyryvar4=1729;
  knnTraining[i].gyrzmean1=-94.6;
  knnTraining[i].gyrzvar1=11206;
  knnTraining[i].gyrzmean2=114;
  knnTraining[i].gyrzvar2=2321;
  knnTraining[i].gyrzmean3=24.2;
  knnTraining[i].gyrzvar3=6521;
  knnTraining[i].gyrzmean4=-118;
  knnTraining[i].gyrzvar4=3862;
  knnTraining[i].knnclass=1;
  i++;
  //sample 3
  knnTraining[i].accxmean1=5.27;
  knnTraining[i].accxvar1=15.5;
  knnTraining[i].accxmean2=1.02;
  knnTraining[i].accxvar2=2.46;
  knnTraining[i].accxmean3=3.82;
  knnTraining[i].accxvar3=17.8;
  knnTraining[i].accxmean4=5.77;
  knnTraining[i].accxvar4=22.6;
  knnTraining[i].accymean1=8.63;
  knnTraining[i].accyvar1=1.14;
  knnTraining[i].accymean2=7.29;
  knnTraining[i].accyvar2=1.23;
  knnTraining[i].accymean3=0.264;
  knnTraining[i].accyvar3=17.4;
  knnTraining[i].accymean4=11.3;
  knnTraining[i].accyvar4=9.94;
  knnTraining[i].acczmean1=8.92;
  knnTraining[i].acczvar1=2.06;
  knnTraining[i].acczmean2=5.96;
  knnTraining[i].acczvar2=4.43;
  knnTraining[i].acczmean3=7.02;
  knnTraining[i].acczvar3=15.1;
  knnTraining[i].acczmean4=7.13;
  knnTraining[i].acczvar4=13.5;
  knnTraining[i].gyrxmean1=-57;
  knnTraining[i].gyrxvar1=7870;
  knnTraining[i].gyrxmean2=137;
  knnTraining[i].gyrxvar2=1130;
  knnTraining[i].gyrxmean3=32.9;
  knnTraining[i].gyrxvar3=8270;
  knnTraining[i].gyrxmean4=-42.4;
  knnTraining[i].gyrxvar4=2620;
  knnTraining[i].gyrymean1=-72.2;
  knnTraining[i].gyryvar1=6300;
  knnTraining[i].gyrymean2=29;
  knnTraining[i].gyryvar2=1300;
  knnTraining[i].gyrymean3=84.8;
  knnTraining[i].gyryvar3=966;
  knnTraining[i].gyrymean4=-91.3;
  knnTraining[i].gyryvar4=2900;
  knnTraining[i].gyrzmean1=-21.8;
  knnTraining[i].gyrzvar1=5710;
  knnTraining[i].gyrzmean2=44.5;
  knnTraining[i].gyrzvar2=1230;
  knnTraining[i].gyrzmean3=-19.5;
  knnTraining[i].gyrzvar3=8420;
  knnTraining[i].gyrzmean4=-92.1;
  knnTraining[i].gyrzvar4=1930;
  knnTraining[i].knnclass=1;
  i++;
  //sample 4
  knnTraining[i].accxmean1=3.08;
  knnTraining[i].accxvar1=13.8;
  knnTraining[i].accxmean2=-0.49;
  knnTraining[i].accxvar2=2.83;
  knnTraining[i].accxmean3=4.06;
  knnTraining[i].accxvar3=4.38;
  knnTraining[i].accxmean4=3.65;
  knnTraining[i].accxvar4=28.2;
  knnTraining[i].accymean1=9.06;
  knnTraining[i].accyvar1=17.5;
  knnTraining[i].accymean2=6.01;
  knnTraining[i].accyvar2=4.42;
  knnTraining[i].accymean3=-0.0718;
  knnTraining[i].accyvar3=5.86;
  knnTraining[i].accymean4=8.5;
  knnTraining[i].accyvar4=37.8;
  knnTraining[i].acczmean1=9.6;
  knnTraining[i].acczvar1=4.1;
  knnTraining[i].acczmean2=5.51;
  knnTraining[i].acczvar2=6;
  knnTraining[i].acczmean3=7.37;
  knnTraining[i].acczvar3=19.8;
  knnTraining[i].acczmean4=9.73;
  knnTraining[i].acczvar4=20.7;
  knnTraining[i].gyrxmean1=14.2;
  knnTraining[i].gyrxvar1=12900;
  knnTraining[i].gyrxmean2=97.5;
  knnTraining[i].gyrxvar2=3850;
  knnTraining[i].gyrxmean3=31.7;
  knnTraining[i].gyrxvar3=2180;
  knnTraining[i].gyrxmean4=-30.7;
  knnTraining[i].gyrxvar4=25800;
  knnTraining[i].gyrymean1=-76;
  knnTraining[i].gyryvar1=5470;
  knnTraining[i].gyrymean2=4.36;
  knnTraining[i].gyryvar2=4120;
  knnTraining[i].gyrymean3=111;
  knnTraining[i].gyryvar3=1090;
  knnTraining[i].gyrymean4=-49.8;
  knnTraining[i].gyryvar4=3690;
  knnTraining[i].gyrzmean1=-42.8;
  knnTraining[i].gyrzvar1=7140;
  knnTraining[i].gyrzmean2=55.6;
  knnTraining[i].gyrzvar2=1230;
  knnTraining[i].gyrzmean3=22.7;
  knnTraining[i].gyrzvar3=3400;
  knnTraining[i].gyrzmean4=-136;
  knnTraining[i].gyrzvar4=17000;
  knnTraining[i].knnclass=1;
  i++;
  //sample 5
  knnTraining[i].accxmean1=7.98;
  knnTraining[i].accxvar1=3.27;
  knnTraining[i].accxmean2=4.65;
  knnTraining[i].accxvar2=2.47;
  knnTraining[i].accxmean3=6.5;
  knnTraining[i].accxvar3=8.13;
  knnTraining[i].accxmean4=7.86;
  knnTraining[i].accxvar4=9.44;
  knnTraining[i].accymean1=5.35;
  knnTraining[i].accyvar1=7.66;
  knnTraining[i].accymean2=6.96;
  knnTraining[i].accyvar2=0.935;
  knnTraining[i].accymean3=3;
  knnTraining[i].accyvar3=5.67;
  knnTraining[i].accymean4=7.57;
  knnTraining[i].accyvar4=6.92;
  knnTraining[i].acczmean1=7.08;
  knnTraining[i].acczvar1=7.27;
  knnTraining[i].acczmean2=4.13;
  knnTraining[i].acczvar2=1.74;
  knnTraining[i].acczmean3=2.53;
  knnTraining[i].acczvar3=3.07;
  knnTraining[i].acczmean4=5.85;
  knnTraining[i].acczvar4=5.65;
  knnTraining[i].gyrxmean1=-56.2;
  knnTraining[i].gyrxvar1=5950;
  knnTraining[i].gyrxmean2=52;
  knnTraining[i].gyrxvar2=2000;
  knnTraining[i].gyrxmean3=0.4;
  knnTraining[i].gyrxvar3=3440;
  knnTraining[i].gyrxmean4=-31.2;
  knnTraining[i].gyrxvar4=2400;
  knnTraining[i].gyrymean1=-44.6;
  knnTraining[i].gyryvar1=5300;
  knnTraining[i].gyrymean2=-3.76;
  knnTraining[i].gyryvar2=1290;
  knnTraining[i].gyrymean3=30.6;
  knnTraining[i].gyryvar3=1740;
  knnTraining[i].gyrymean4=7.35;
  knnTraining[i].gyryvar4=1800;
  knnTraining[i].gyrzmean1=-39.5;
  knnTraining[i].gyrzvar1=2060;
  knnTraining[i].gyrzmean2=-8.07;
  knnTraining[i].gyrzvar2=2350;
  knnTraining[i].gyrzmean3=-7.15;
  knnTraining[i].gyrzvar3=1910;
  knnTraining[i].gyrzmean4=5.18;
  knnTraining[i].gyrzvar4=829;
  knnTraining[i].knnclass=1;
  i++;
  //sample 6
  knnTraining[i].accxmean1=7.54;
  knnTraining[i].accxvar1=35;
  knnTraining[i].accxmean2=8.07;
  knnTraining[i].accxvar2=84.9;
  knnTraining[i].accxmean3=4.3;
  knnTraining[i].accxvar3=84.2;
  knnTraining[i].accxmean4=4.2;
  knnTraining[i].accxvar4=13.2;
  knnTraining[i].accymean1=-3.81;
  knnTraining[i].accyvar1=8.46;
  knnTraining[i].accymean2=-5.53;
  knnTraining[i].accyvar2=67.2;
  knnTraining[i].accymean3=1.88;
  knnTraining[i].accyvar3=71.5;
  knnTraining[i].accymean4=-4.96;
  knnTraining[i].accyvar4=27.8;
  knnTraining[i].acczmean1=-4.65;
  knnTraining[i].acczvar1=7.19;
  knnTraining[i].acczmean2=-6.04;
  knnTraining[i].acczvar2=30.2;
  knnTraining[i].acczmean3=-9.76;
  knnTraining[i].acczvar3=68.6;
  knnTraining[i].acczmean4=-8.91;
  knnTraining[i].acczvar4=44.8;
  knnTraining[i].gyrxmean1=74.7;
  knnTraining[i].gyrxvar1=3120;
  knnTraining[i].gyrxmean2=-83;
  knnTraining[i].gyrxvar2=6560;
  knnTraining[i].gyrxmean3=76.9;
  knnTraining[i].gyrxvar3=29500;
  knnTraining[i].gyrxmean4=-64.5;
  knnTraining[i].gyrxvar4=13900;
  knnTraining[i].gyrymean1=-40;
  knnTraining[i].gyryvar1=13500;
  knnTraining[i].gyrymean2=48.4;
  knnTraining[i].gyryvar2=13100;
  knnTraining[i].gyrymean3=-34.4;
  knnTraining[i].gyryvar3=6500;
  knnTraining[i].gyrymean4=52.5;
  knnTraining[i].gyryvar4=9220;
  knnTraining[i].gyrzmean1=-10.9;
  knnTraining[i].gyrzvar1=725;
  knnTraining[i].gyrzmean2=47.3;
  knnTraining[i].gyrzvar2=5830;
  knnTraining[i].gyrzmean3=-95.9;
  knnTraining[i].gyrzvar3=12800;
  knnTraining[i].gyrzmean4=66.4;
  knnTraining[i].gyrzvar4=10300;
  knnTraining[i].knnclass=2;
  i++;
  //sample 7
  knnTraining[i].accxmean1=8.38;
  knnTraining[i].accxvar1=-16;
  knnTraining[i].accxmean2=8.97;
  knnTraining[i].accxvar2=72.9;
  knnTraining[i].accxmean3=2.07;
  knnTraining[i].accxvar3=42.6;
  knnTraining[i].accxmean4=5.11;
  knnTraining[i].accxvar4=14;
  knnTraining[i].accymean1=-1.83;
  knnTraining[i].accyvar1=13.8;
  knnTraining[i].accymean2=-3.98;
  knnTraining[i].accyvar2=27.6;
  knnTraining[i].accymean3=2.06;
  knnTraining[i].accyvar3=17.3;
  knnTraining[i].accymean4=-2.37;
  knnTraining[i].accyvar4=45.3;
  knnTraining[i].acczmean1=-3.98;
  knnTraining[i].acczvar1=7.14;
  knnTraining[i].acczmean2=-6.03;
  knnTraining[i].acczvar2=22.3;
  knnTraining[i].acczmean3=-9.79; 
  knnTraining[i].acczvar3=39.9;
  knnTraining[i].acczmean4=-8.4;
  knnTraining[i].acczvar4=22.1;
  knnTraining[i].gyrxmean1=96.5;
  knnTraining[i].gyrxvar1=4080;
  knnTraining[i].gyrxmean2=-98.2;
  knnTraining[i].gyrxvar2=8140;
  knnTraining[i].gyrxmean3=85.2;
  knnTraining[i].gyrxvar3=17000;
  knnTraining[i].gyrxmean4=-89.4;
  knnTraining[i].gyrxvar4=6610;
  knnTraining[i].gyrymean1=-22.2;
  knnTraining[i].gyryvar1=14500;
  knnTraining[i].gyrymean2=11.3;
  knnTraining[i].gyryvar2=17700;
  knnTraining[i].gyrymean3=-18.2;
  knnTraining[i].gyryvar3=2630;
  knnTraining[i].gyrymean4=43.2;
  knnTraining[i].gyryvar4=5740;
  knnTraining[i].gyrzmean1=-0.38;
  knnTraining[i].gyrzvar1=2540;
  knnTraining[i].gyrzmean2=45.1;
  knnTraining[i].gyrzvar2=5970;
  knnTraining[i].gyrzmean3=-129;
  knnTraining[i].gyrzvar3=15600;
  knnTraining[i].gyrzmean4=97.5;
  knnTraining[i].gyrzvar4=9490;
  knnTraining[i].knnclass=2;
  i++;
  //sample 8
  knnTraining[i].accxmean1=4.01;
  knnTraining[i].accxvar1=12;
  knnTraining[i].accxmean2=5.54;
  knnTraining[i].accxvar2=31.3;
  knnTraining[i].accxmean3=-0.238;
  knnTraining[i].accxvar3=28.4;
  knnTraining[i].accxmean4=6.69;
  knnTraining[i].accxvar4=9.83;
  knnTraining[i].accymean1=-3.51;
  knnTraining[i].accyvar1=10;
  knnTraining[i].accymean2=-6.08;
  knnTraining[i].accyvar2=44;
  knnTraining[i].accymean3=0.779;
  knnTraining[i].accyvar3=15.2;
  knnTraining[i].accymean4=-1.18;
  knnTraining[i].accyvar4=25.1;
  knnTraining[i].acczmean1=-7.11;
  knnTraining[i].acczvar1=5.54;
  knnTraining[i].acczmean2=-9.66;
  knnTraining[i].acczvar2=17.5;
  knnTraining[i].acczmean3=-9.99;
  knnTraining[i].acczvar3=30.7;
  knnTraining[i].acczmean4=-6.24;
  knnTraining[i].acczvar4=41.8;
  knnTraining[i].gyrxmean1=4.57;
  knnTraining[i].gyrxvar1=1590;
  knnTraining[i].gyrxmean2=-50.1;
  knnTraining[i].gyrxvar2=6310;
  knnTraining[i].gyrxmean3=13.6;
  knnTraining[i].gyrxvar3=19600;
  knnTraining[i].gyrxmean4=-42.1;
  knnTraining[i].gyrxvar4=6610;
  knnTraining[i].gyrymean1=-29;
  knnTraining[i].gyryvar1=4760;
  knnTraining[i].gyrymean2=45.2;
  knnTraining[i].gyryvar2=16000;
  knnTraining[i].gyrymean3=0.192;
  knnTraining[i].gyryvar3=8300;
  knnTraining[i].gyrymean4=33.8;
  knnTraining[i].gyryvar4=6740;
  knnTraining[i].gyrzmean1=-7.65;
  knnTraining[i].gyrzvar1=2330;
  knnTraining[i].gyrzmean2=2.4;
  knnTraining[i].gyrzvar2=12000;
  knnTraining[i].gyrzmean3=-71.1;
  knnTraining[i].gyrzvar3=21900;
  knnTraining[i].gyrzmean4=42.6;
  knnTraining[i].gyrzvar4=6660;
  knnTraining[i].knnclass=2;
  i++;
  //sample 9
  knnTraining[i].accxmean1=8.71;
  knnTraining[i].accxvar1=2.01;
  knnTraining[i].accxmean2=5.91;
  knnTraining[i].accxvar2=9.35;
  knnTraining[i].accxmean3=4.35;
  knnTraining[i].accxvar3=15.7;
  knnTraining[i].accxmean4=9.08;
  knnTraining[i].accxvar4=12.1;
  knnTraining[i].accymean1=-2.75;
  knnTraining[i].accyvar1=14.2;
  knnTraining[i].accymean2=8.09;
  knnTraining[i].accyvar2=137;
  knnTraining[i].accymean3=-3.57;
  knnTraining[i].accyvar3=63;
  knnTraining[i].accymean4=-0.368;
  knnTraining[i].accyvar4=77.3;
  knnTraining[i].acczmean1=-1.89;
  knnTraining[i].acczvar1=18.8;
  knnTraining[i].acczmean2=1.46;
  knnTraining[i].acczvar2=90.1;
  knnTraining[i].acczmean3=-9.79;
  knnTraining[i].acczvar3=51.7;
  knnTraining[i].acczmean4=-2.11;
  knnTraining[i].acczvar4=20.8;
  knnTraining[i].gyrxmean1=97.6;
  knnTraining[i].gyrxvar1=8600;
  knnTraining[i].gyrxmean2=-85.3;
  knnTraining[i].gyrxvar2=25100;
  knnTraining[i].gyrxmean3=64.6;
  knnTraining[i].gyrxvar3=23200;
  knnTraining[i].gyrxmean4=-78.1;
  knnTraining[i].gyrxvar4=9850;
  knnTraining[i].gyrymean1=44.1;
  knnTraining[i].gyryvar1=1070;
  knnTraining[i].gyrymean2=-48;
  knnTraining[i].gyryvar2=7130;
  knnTraining[i].gyrymean3=-56.5;
  knnTraining[i].gyryvar3=1980;
  knnTraining[i].gyrymean4=50.6;
  knnTraining[i].gyryvar4=2030;
  knnTraining[i].gyrzmean1=-38.4;
  knnTraining[i].gyrzvar1=1550;
  knnTraining[i].gyrzmean2=51.4;
  knnTraining[i].gyrzvar2=9930;
  knnTraining[i].gyrzmean3=-45.1;
  knnTraining[i].gyrzvar3=13000;
  knnTraining[i].gyrzmean4=1.28;
  knnTraining[i].gyrzvar4=3340;
  knnTraining[i].knnclass=2;
  i++;
  //sample 10
  knnTraining[i].accxmean1=8.58;
  knnTraining[i].accxvar1=2.96;
  knnTraining[i].accxmean2=9.71;
  knnTraining[i].accxvar2=18.2;
  knnTraining[i].accxmean3=5.49;
  knnTraining[i].accxvar3=4.98;
  knnTraining[i].accxmean4=8.62;
  knnTraining[i].accxvar4=10.3;
  knnTraining[i].accymean1=-1.72;
  knnTraining[i].accyvar1=11.5;
  knnTraining[i].accymean2=4.1;
  knnTraining[i].accyvar2=91.7;
  knnTraining[i].accymean3=-2.7;
  knnTraining[i].accyvar3=33.3;
  knnTraining[i].accymean4=-3.57;
  knnTraining[i].accyvar4=30.7;
  knnTraining[i].acczmean1=-0.643;
  knnTraining[i].acczvar1=14.1;
  knnTraining[i].acczmean2=-2.71;
  knnTraining[i].acczvar2=27.7;
  knnTraining[i].acczmean3=-7.43;
  knnTraining[i].acczvar3=36.6;
  knnTraining[i].acczmean4=-3.34;
  knnTraining[i].acczvar4=20.2;
  knnTraining[i].gyrxmean1=68.8;
  knnTraining[i].gyrxvar1=4990;
  knnTraining[i].gyrxmean2=-81.4;
  knnTraining[i].gyrxvar2=25500;
  knnTraining[i].gyrxmean3=56.8;
  knnTraining[i].gyrxvar3=14500;
  knnTraining[i].gyrxmean4=-66.9;
  knnTraining[i].gyrxvar4=5860;
  knnTraining[i].gyrymean1=38.4;  
  knnTraining[i].gyryvar1=4280;
  knnTraining[i].gyrymean2=-20;
  knnTraining[i].gyryvar2=8880;
  knnTraining[i].gyrymean3=-28.7;
  knnTraining[i].gyryvar3=3420;
  knnTraining[i].gyrymean4=21.9;
  knnTraining[i].gyryvar4=2490;
  knnTraining[i].gyrzmean1=-32;
  knnTraining[i].gyrzvar1=1180;
  knnTraining[i].gyrzmean2=50.5;
  knnTraining[i].gyrzvar2=7930;
  knnTraining[i].gyrzmean3=-35.8;
  knnTraining[i].gyrzvar3=5770;
  knnTraining[i].gyrzmean4=6.26;
  knnTraining[i].gyrzvar4=1470;
  knnTraining[i].knnclass=2;
  i++;
  //sample 11
  knnTraining[i].accxmean1=8.15;
  knnTraining[i].accxvar1=16.5;
  knnTraining[i].accxmean2=9.11;
  knnTraining[i].accxvar2=19.6;
  knnTraining[i].accxmean3=7.39;
  knnTraining[i].accxvar3=20.7;
  knnTraining[i].accxmean4=8.32;
  knnTraining[i].accxvar4=4.24;
  knnTraining[i].accymean1=3.43;
  knnTraining[i].accyvar1=14.2;
  knnTraining[i].accymean2=2.68;
  knnTraining[i].accyvar2=27.8;
  knnTraining[i].accymean3=-1.28;
  knnTraining[i].accyvar3=15.4;
  knnTraining[i].accymean4=0.114;
  knnTraining[i].accyvar4=18.1;
  knnTraining[i].acczmean1=1.87;
  knnTraining[i].acczvar1=20.9;
  knnTraining[i].acczmean2=2.43;
  knnTraining[i].acczvar2=12.8;
  knnTraining[i].acczmean3=4.58;
  knnTraining[i].acczvar3=20;
  knnTraining[i].acczmean4=3.86;
  knnTraining[i].acczvar4=18.2;
  knnTraining[i].gyrxmean1=86;
  knnTraining[i].gyrxvar1=5090;
  knnTraining[i].gyrxmean2=-81;
  knnTraining[i].gyrxvar2=5060;
  knnTraining[i].gyrxmean3=-6.4;
  knnTraining[i].gyrxvar3=691;
  knnTraining[i].gyrxmean4=15.7;
  knnTraining[i].gyrxvar4=2540;
  knnTraining[i].gyrymean1=89.7;
  knnTraining[i].gyryvar1=2870;
  knnTraining[i].gyrymean2=-25.8;
  knnTraining[i].gyryvar2=1890;
  knnTraining[i].gyrymean3=81.6;
  knnTraining[i].gyryvar3=4250;
  knnTraining[i].gyrymean4=-89.7;
  knnTraining[i].gyryvar4=1750;
  knnTraining[i].gyrzmean1=-78.4;
  knnTraining[i].gyrzvar1=2640;
  knnTraining[i].gyrzmean2=110;
  knnTraining[i].gyrzvar2=4210;
  knnTraining[i].gyrzmean3=-77.7;
  knnTraining[i].gyrzvar3=5630;
  knnTraining[i].gyrzmean4=76.9;
  knnTraining[i].gyrzvar4=3390;
  knnTraining[i].knnclass=3;
  i++;
  //sample 12
  knnTraining[i].accxmean1=7.53;
  knnTraining[i].accxvar1=20.1;
  knnTraining[i].accxmean2=10.6;
  knnTraining[i].accxvar2=29.3;
  knnTraining[i].accxmean3=3.7;
  knnTraining[i].accxvar3=15.2;
  knnTraining[i].accxmean4=6.2;
  knnTraining[i].accxvar4=2.1;
  knnTraining[i].accymean1=2.36;
  knnTraining[i].accyvar1=31.4;
  knnTraining[i].accymean2=4.47;
  knnTraining[i].accyvar2=21.8;
  knnTraining[i].accymean3=1.77;
  knnTraining[i].accyvar3=28.8;
  knnTraining[i].accymean4=2.77;  
  knnTraining[i].accyvar4=31.6;
  knnTraining[i].acczmean1=-1.26;
  knnTraining[i].acczvar1=12.5;
  knnTraining[i].acczmean2=2.5;
  knnTraining[i].acczvar2=23.2;
  knnTraining[i].acczmean3=6.98;
  knnTraining[i].acczvar3=50.5;
  knnTraining[i].acczmean4=6.04;
  knnTraining[i].acczvar4=22.7;
  knnTraining[i].gyrxmean1=81.4;
  knnTraining[i].gyrxvar1=8040;
  knnTraining[i].gyrxmean2=-82.2;
  knnTraining[i].gyrxvar2=20000;
  knnTraining[i].gyrxmean3=33;
  knnTraining[i].gyrxvar3=5630;
  knnTraining[i].gyrxmean4=7.22;
  knnTraining[i].gyrxvar4=1560;
  knnTraining[i].gyrymean1=66.7;
  knnTraining[i].gyryvar1=3510;
  knnTraining[i].gyrymean2=-18.5;
  knnTraining[i].gyryvar2=4520;
  knnTraining[i].gyrymean3=83.8;
  knnTraining[i].gyryvar3=14500;
  knnTraining[i].gyrymean4=-95.5;
  knnTraining[i].gyryvar4=5210;
  knnTraining[i].gyrzmean1=-54.1;
  knnTraining[i].gyrzvar1=10800;
  knnTraining[i].gyrzmean2=62.1;
  knnTraining[i].gyrzvar2=15200;
  knnTraining[i].gyrzmean3=-100;
  knnTraining[i].gyrzvar3=20800;
  knnTraining[i].gyrzmean4=108;
  knnTraining[i].gyrzvar4=4710;
  knnTraining[i].knnclass=3;
  i++;
  //sample 13
  knnTraining[i].accxmean1=7.11;
  knnTraining[i].accxvar1=29.1;
  knnTraining[i].accxmean2=7.63;
  knnTraining[i].accxvar2=11.3;
  knnTraining[i].accxmean3=6.47;
  knnTraining[i].accxvar3=12.9;
  knnTraining[i].accxmean4=6.62;
  knnTraining[i].accxvar4=17.1;
  knnTraining[i].accymean1=4.92;
  knnTraining[i].accyvar1=50.5;
  knnTraining[i].accymean2=5.34;
  knnTraining[i].accyvar2=37.9;
  knnTraining[i].accymean3=6.32;
  knnTraining[i].accyvar3=28.4;
  knnTraining[i].accymean4=5.92;
  knnTraining[i].accyvar4=47.7;
  knnTraining[i].acczmean1=-0.69;
  knnTraining[i].acczvar1=13.9;
  knnTraining[i].acczmean2=-1.17;
  knnTraining[i].acczvar2=11.5;
  knnTraining[i].acczmean3=-3.51;
  knnTraining[i].acczvar3=33.7;
  knnTraining[i].acczmean4=-3.23;
  knnTraining[i].acczvar4=8.96;
  knnTraining[i].gyrxmean1=-36.4;
  knnTraining[i].gyrxvar1=6360;
  knnTraining[i].gyrxmean2=54.3;
  knnTraining[i].gyrxvar2=6790;
  knnTraining[i].gyrxmean3=-21.4;
  knnTraining[i].gyrxvar3=4890;
  knnTraining[i].gyrxmean4=-0.16;
  knnTraining[i].gyrxvar4=1620;
  knnTraining[i].gyrymean1=-6.73;
  knnTraining[i].gyryvar1=8240;
  knnTraining[i].gyrymean2=-6.83;
  knnTraining[i].gyryvar2=2030;
  knnTraining[i].gyrymean3=9.36;
  knnTraining[i].gyryvar3=5490;
  knnTraining[i].gyrymean4=16.3;
  knnTraining[i].gyryvar4=6480;
  knnTraining[i].gyrzmean1=-107;
  knnTraining[i].gyrzvar1=11500;
  knnTraining[i].gyrzmean2=98.3;
  knnTraining[i].gyrzvar2=4190;
  knnTraining[i].gyrzmean3=-120;
  knnTraining[i].gyrzvar3=6760;
  knnTraining[i].gyrzmean4=124;
  knnTraining[i].gyrzvar4=8450;
  knnTraining[i].knnclass=3;
  i++;
  //sample 14
  knnTraining[i].accxmean1=6.72;
  knnTraining[i].accxvar1=7.63;
  knnTraining[i].accxmean2=5.13;
  knnTraining[i].accxvar2=5.49;
  knnTraining[i].accxmean3=5.61;
  knnTraining[i].accxvar3=9.86;
  knnTraining[i].accxmean4=7.36;
  knnTraining[i].accxvar4=18.5;
  knnTraining[i].accymean1=5.07;
  knnTraining[i].accyvar1=32.6;
  knnTraining[i].accymean2=9.9;
  knnTraining[i].accyvar2=9.57;
  knnTraining[i].accymean3=4.85;
  knnTraining[i].accyvar3=22.1;
  knnTraining[i].accymean4=6.5;
  knnTraining[i].accyvar4=30;
  knnTraining[i].acczmean1=-2.37;
  knnTraining[i].acczvar1=8.45;
  knnTraining[i].acczmean2=-1.01;
  knnTraining[i].acczvar2=7.54;
  knnTraining[i].acczmean3=-3.66;
  knnTraining[i].acczvar3=9.47;
  knnTraining[i].acczmean4=-2.39;
  knnTraining[i].acczvar4=5.7;
  knnTraining[i].gyrxmean1=-21.2;
  knnTraining[i].gyrxvar1=3420;
  knnTraining[i].gyrxmean2=24.7;
  knnTraining[i].gyrxvar2=4720;
  knnTraining[i].gyrxmean3=-33.5;
  knnTraining[i].gyrxvar3=6140;
  knnTraining[i].gyrxmean4=-20.3;
  knnTraining[i].gyrxvar4=3830;
  knnTraining[i].gyrymean1=-7.77;
  knnTraining[i].gyryvar1=4820;
  knnTraining[i].gyrymean2=7.19;
  knnTraining[i].gyryvar2=2470;
  knnTraining[i].gyrymean3=-9.68;
  knnTraining[i].gyryvar3=3050;
  knnTraining[i].gyrymean4=20.5;
  knnTraining[i].gyryvar4=5950;
  knnTraining[i].gyrzmean1=-83.7;
  knnTraining[i].gyrzvar1=4980;
  knnTraining[i].gyrzmean2=51.8;
  knnTraining[i].gyrzvar2=4540;
  knnTraining[i].gyrzmean3=-47.8;
  knnTraining[i].gyrzvar3=8710;
  knnTraining[i].gyrzmean4=109;
  knnTraining[i].gyrzvar4=10500;
  knnTraining[i].knnclass=3;
  i++;
  //sample 15
  knnTraining[i].accxmean1=6.13;
  knnTraining[i].accxvar1=5.43;
  knnTraining[i].accxmean2=8.51;
  knnTraining[i].accxvar2=14.5;
  knnTraining[i].accxmean3=11.6;
  knnTraining[i].accxvar3=36.8;
  knnTraining[i].accxmean4=3.53;
  knnTraining[i].accxvar4=27.4;
  knnTraining[i].accymean1=-2.56;
  knnTraining[i].accyvar1=27.9;
  knnTraining[i].accymean2=-1.16;
  knnTraining[i].accyvar2=13;
  knnTraining[i].accymean3=-2.01;
  knnTraining[i].accyvar3=3.69;
  knnTraining[i].accymean4=-1.52;
  knnTraining[i].accyvar4=18.5;
  knnTraining[i].acczmean1=-5.68;
  knnTraining[i].acczvar1=33.4;
  knnTraining[i].acczmean2=0.0485;
  knnTraining[i].acczvar2=12.5;
  knnTraining[i].acczmean3=-3.33;
  knnTraining[i].acczvar3=11.2;
  knnTraining[i].acczmean4=-7.82;
  knnTraining[i].acczvar4=76.5;
  knnTraining[i].gyrxmean1=44.8;
  knnTraining[i].gyrxvar1=1400;
  knnTraining[i].gyrxmean2=34.7;
  knnTraining[i].gyrxvar2=3680;
  knnTraining[i].gyrxmean3=-48.5;
  knnTraining[i].gyrxvar3=5790;
  knnTraining[i].gyrxmean4=29.3;
  knnTraining[i].gyrxvar4=3520;
  knnTraining[i].gyrymean1=106;
  knnTraining[i].gyryvar1=12900;
  knnTraining[i].gyrymean2=-52.2;
  knnTraining[i].gyryvar2=2090;
  knnTraining[i].gyrymean3=17.5;
  knnTraining[i].gyryvar3=7200;
  knnTraining[i].gyrymean4=-131;
  knnTraining[i].gyryvar4=14200;
  knnTraining[i].gyrzmean1=-77.7;
  knnTraining[i].gyrzvar1=4240;
  knnTraining[i].gyrzmean2=53.1;
  knnTraining[i].gyrzvar2=2680;
  knnTraining[i].gyrzmean3=-80.2;
  knnTraining[i].gyrzvar3=3240;
  knnTraining[i].gyrzmean4=108;
  knnTraining[i].gyrzvar4=21900;
  knnTraining[i].knnclass=3;
  i++;
  //sample 16
  knnTraining[i].accxmean1=8.24;
  knnTraining[i].accxvar1=0.0705;
  knnTraining[i].accxmean2=8.08;
  knnTraining[i].accxvar2=0.177;
  knnTraining[i].accxmean3=7.05;
  knnTraining[i].accxvar3=0.832;
  knnTraining[i].accxmean4=6.11;
  knnTraining[i].accxvar4=6.08;
  knnTraining[i].accymean1=-5.04;
  knnTraining[i].accyvar1=0.906;
  knnTraining[i].accymean2=-2.36;
  knnTraining[i].accyvar2=1.1;
  knnTraining[i].accymean3=1.48;
  knnTraining[i].accyvar3=4.31;
  knnTraining[i].accymean4=6.36;
  knnTraining[i].accyvar4=161;
  knnTraining[i].acczmean1=-2.74;
  knnTraining[i].acczvar1=0.462;
  knnTraining[i].acczmean2=-4.82;
  knnTraining[i].acczvar2=0.559;
  knnTraining[i].acczmean3=-5.98;
  knnTraining[i].acczvar3=0.56;
  knnTraining[i].acczmean4=-3.81;
  knnTraining[i].acczvar4=62.7;
  knnTraining[i].gyrxmean1=-37.1;
  knnTraining[i].gyrxvar1=36.8;
  knnTraining[i].gyrxmean2=-38.7;
  knnTraining[i].gyrxvar2=17.3;
  knnTraining[i].gyrxmean3=-40.2;
  knnTraining[i].gyrxvar3=242;
  knnTraining[i].gyrxmean4=-38.8;
  knnTraining[i].gyrxvar4=51900;
  knnTraining[i].gyrymean1=-10.4;
  knnTraining[i].gyryvar1=26.4;
  knnTraining[i].gyrymean2=-13.3;
  knnTraining[i].gyryvar2=27.5;
  knnTraining[i].gyrymean3=-14.5;
  knnTraining[i].gyryvar3=60;
  knnTraining[i].gyrymean4=-9.46;
  knnTraining[i].gyryvar4=1910;
  knnTraining[i].gyrzmean1=-9.31;
  knnTraining[i].gyrzvar1=12.5;
  knnTraining[i].gyrzmean2=-9.74;
  knnTraining[i].gyrzvar2=14.9;
  knnTraining[i].gyrzmean3=-14.2;
  knnTraining[i].gyrzvar3=63.3;
  knnTraining[i].gyrzmean4=0.846;
  knnTraining[i].gyrzvar4=8360;
  knnTraining[i].knnclass=4;
  i++;
  //sample 17
  knnTraining[i].accxmean1=8.59;
  knnTraining[i].accxvar1=0.645;
  knnTraining[i].accxmean2=8.08;
  knnTraining[i].accxvar2=0.443;
  knnTraining[i].accxmean3=6.86;
  knnTraining[i].accxvar3=0.381;
  knnTraining[i].accxmean4=4.83;
  knnTraining[i].accxvar4=7.56;
  knnTraining[i].accymean1=-5.66;
  knnTraining[i].accyvar1=2.18;
  knnTraining[i].accymean2=-3.08;
  knnTraining[i].accyvar2=0.521;
  knnTraining[i].accymean3=0.829;
  knnTraining[i].accyvar3=2.78;
  knnTraining[i].accymean4=6.27;
  knnTraining[i].accyvar4=78.1;
  knnTraining[i].acczmean1=-0.969;
  knnTraining[i].acczvar1=2.6;
  knnTraining[i].acczmean2=-4.45;
  knnTraining[i].acczvar2=0.933;
  knnTraining[i].acczmean3=-6.62;
  knnTraining[i].acczvar3=0.174;
  knnTraining[i].acczmean4=-4.74;
  knnTraining[i].acczvar4=9.32;
  knnTraining[i].gyrxmean1=-37.4;
  knnTraining[i].gyrxvar1=207;
  knnTraining[i].gyrxmean2=-37.9;
  knnTraining[i].gyrxvar2=76.5;
  knnTraining[i].gyrxmean3=-27.7;
  knnTraining[i].gyrxvar3=323;
  knnTraining[i].gyrxmean4=-42.8;
  knnTraining[i].gyrxvar4=16400;
  knnTraining[i].gyrymean1=-22.6;
  knnTraining[i].gyryvar1=156;
  knnTraining[i].gyrymean2=-14.7;
  knnTraining[i].gyryvar2=95.9;
  knnTraining[i].gyrymean3=-11.5;
  knnTraining[i].gyryvar3=82;
  knnTraining[i].gyrymean4=-2.25;
  knnTraining[i].gyryvar4=4080;
  knnTraining[i].gyrzmean1=-14.5;
  knnTraining[i].gyrzvar1=49.2;
  knnTraining[i].gyrzmean2=0.168;
  knnTraining[i].gyrzvar2=29.3;
  knnTraining[i].gyrzmean3=-9.25;
  knnTraining[i].gyrzvar3=75.7;
  knnTraining[i].gyrzmean4=-12.1;
  knnTraining[i].gyrzvar4=4990;
  knnTraining[i].knnclass=4;
  i++;
  //sample 18
  knnTraining[i].accxmean1=8.9;
  knnTraining[i].accxvar1=1.48;
  knnTraining[i].accxmean2=6.84;
  knnTraining[i].accxvar2=0.83;
  knnTraining[i].accxmean3=5.24;
  knnTraining[i].accxvar3=1.78;
  knnTraining[i].accxmean4=1.76;
  knnTraining[i].accxvar4=48.7;
  knnTraining[i].accymean1=-9.5;
  knnTraining[i].accyvar1=3;
  knnTraining[i].accymean2=-1.97;
  knnTraining[i].accyvar2=12.1;
  knnTraining[i].accymean3=2.31;
  knnTraining[i].accyvar3=12.2;
  knnTraining[i].accymean4=12.2;
  knnTraining[i].accyvar4=270;
  knnTraining[i].acczmean1=-0.324;
  knnTraining[i].acczvar1=4.77;
  knnTraining[i].acczmean2=-3.29;
  knnTraining[i].acczvar2=1.46;
  knnTraining[i].acczmean3=-3.13;
  knnTraining[i].acczvar3=7.61;
  knnTraining[i].acczmean4=-3.65;
  knnTraining[i].acczvar4=31.8;
  knnTraining[i].gyrxmean1=-72.1;
  knnTraining[i].gyrxvar1=1180;
  knnTraining[i].gyrxmean2=-95.7;
  knnTraining[i].gyrxvar2=178;
  knnTraining[i].gyrxmean3=-48.6;
  knnTraining[i].gyrxvar3=4590;
  knnTraining[i].gyrxmean4=-29.2;
  knnTraining[i].gyrxvar4=88100;
  knnTraining[i].gyrymean1=-32.2;
  knnTraining[i].gyryvar1=374;
  knnTraining[i].gyrymean2=-35.1;
  knnTraining[i].gyryvar2=425;
  knnTraining[i].gyrymean3=-49.6;
  knnTraining[i].gyryvar3=1200;
  knnTraining[i].gyrymean4=-8.32;
  knnTraining[i].gyryvar4=6800;
  knnTraining[i].gyrzmean1=-18.7;
  knnTraining[i].gyrzvar1=392;
  knnTraining[i].gyrzmean2=-60.7;
  knnTraining[i].gyrzvar2=234;
  knnTraining[i].gyrzmean3=-16.3;
  knnTraining[i].gyrzvar3=1800;
  knnTraining[i].gyrzmean4=-2.24;
  knnTraining[i].gyrzvar4=12900;
  knnTraining[i].knnclass=4;
  i++;
  //sample 19
  knnTraining[i].accxmean1=8.57;
  knnTraining[i].accxvar1=0.781;
  knnTraining[i].accxmean2=7.97;
  knnTraining[i].accxvar2=0.201;
  knnTraining[i].accxmean3=7.77;
  knnTraining[i].accxvar3=0.633;
  knnTraining[i].accxmean4=1.35;
  knnTraining[i].accxvar4=44;
  knnTraining[i].accymean1=-7.18;
  knnTraining[i].accyvar1=1.49;
  knnTraining[i].accymean2=-2.88;
  knnTraining[i].accyvar2=2.06;
  knnTraining[i].accymean3=0.793;
  knnTraining[i].accyvar3=3.77;
  knnTraining[i].accymean4=7.61;
  knnTraining[i].accyvar4=233;
  knnTraining[i].acczmean1=0.553;
  knnTraining[i].acczvar1=2.49;
  knnTraining[i].acczmean2=-3.62;
  knnTraining[i].acczvar2=0.568;
  knnTraining[i].acczmean3=-5.01;
  knnTraining[i].acczvar3=0.31;
  knnTraining[i].acczmean4=-3.58;
  knnTraining[i].acczvar4=65;
  knnTraining[i].gyrxmean1=-44;
  knnTraining[i].gyrxvar1=455;
  knnTraining[i].gyrxmean2=-47.3;
  knnTraining[i].gyrxvar2=111;
  knnTraining[i].gyrxmean3=-30.4;
  knnTraining[i].gyrxvar3=79.2;
  knnTraining[i].gyrxmean4=-72.2;
  knnTraining[i].gyrxvar4=17200;
  knnTraining[i].gyrymean1=-23;
  knnTraining[i].gyryvar1=107;
  knnTraining[i].gyrymean2=-12.7;
  knnTraining[i].gyryvar2=114;
  knnTraining[i].gyrymean3=-24.7;
  knnTraining[i].gyryvar3=80.8;
  knnTraining[i].gyrymean4=-19.5;
  knnTraining[i].gyryvar4=18200;
  knnTraining[i].gyrzmean1=-17.5;
  knnTraining[i].gyrzvar1=107;
  knnTraining[i].gyrzmean2=-18.3;
  knnTraining[i].gyrzvar2=69.4;
  knnTraining[i].gyrzmean3=-28.7;
  knnTraining[i].gyrzvar3=54.3;
  knnTraining[i].gyrzmean4=-49.6;
  knnTraining[i].gyrzvar4=64500;
  knnTraining[i].knnclass=4;
  i++;
  //sample 20
  knnTraining[i].accxmean1=9.44;
  knnTraining[i].accxvar1=1.47;
  knnTraining[i].accxmean2=8.77;
  knnTraining[i].accxvar2=0.213;
  knnTraining[i].accxmean3=7.42;
  knnTraining[i].accxvar3=0.427;
  knnTraining[i].accxmean4=5.71;
  knnTraining[i].accxvar4=15;
  knnTraining[i].accymean1=-4.96;
  knnTraining[i].accyvar1=0.412;
  knnTraining[i].accymean2=-3.21;
  knnTraining[i].accyvar2=2.83;
  knnTraining[i].accymean3=1.5;
  knnTraining[i].accyvar3=2.11;
  knnTraining[i].accymean4=6.98;
  knnTraining[i].accyvar4=187;
  knnTraining[i].acczmean1=2.61;
  knnTraining[i].acczvar1=4.05;
  knnTraining[i].acczmean2=-2.1;
  knnTraining[i].acczvar2=1.86;
  knnTraining[i].acczmean3=-5.11;
  knnTraining[i].acczvar3=1.28;
  knnTraining[i].acczmean4=-4.41;
  knnTraining[i].acczvar4=68.3;
  knnTraining[i].gyrxmean1=-40.6;
  knnTraining[i].gyrxvar1=302;
  knnTraining[i].gyrxmean2=-58.5;
  knnTraining[i].gyrxvar2=228;
  knnTraining[i].gyrxmean3=-30.1;
  knnTraining[i].gyrxvar3=90;
  knnTraining[i].gyrxmean4=-63.9;
  knnTraining[i].gyrxvar4=45500;
  knnTraining[i].gyrymean1=-35.2;
  knnTraining[i].gyryvar1=161;
  knnTraining[i].gyrymean2=-26.4;
  knnTraining[i].gyryvar2=42.1;
  knnTraining[i].gyrymean3=-22.5;
  knnTraining[i].gyryvar3=66.9;
  knnTraining[i].gyrymean4=-15.7;
  knnTraining[i].gyryvar4=2830;
  knnTraining[i].gyrzmean1=-5.17;
  knnTraining[i].gyrzvar1=41.3;
  knnTraining[i].gyrzmean2=-18.9;
  knnTraining[i].gyrzvar2=106;
  knnTraining[i].gyrzmean3=-11.7;
  knnTraining[i].gyrzvar3=132;
  knnTraining[i].gyrzmean4=-5.77;
  knnTraining[i].gyrzvar4=1640;
  knnTraining[i].knnclass=4;
  i++;
  //sample 21
  knnTraining[i].accxmean1=11.3;
  knnTraining[i].accxvar1=110;
  knnTraining[i].accxmean2=0.537;
  knnTraining[i].accxvar2=15.2;
  knnTraining[i].accxmean3=3.01;
  knnTraining[i].accxvar3=6.16;
  knnTraining[i].accxmean4=-2.5;
  knnTraining[i].accxvar4=4.68;
  knnTraining[i].accymean1=1.69;
  knnTraining[i].accyvar1=91.5;
  knnTraining[i].accymean2=-6.85;
  knnTraining[i].accyvar2=47.8;
  knnTraining[i].accymean3=-8.22;
  knnTraining[i].accyvar3=11.1;
  knnTraining[i].accymean4=-7.43;
  knnTraining[i].accyvar4=7.79;
  knnTraining[i].acczmean1=-1.92;
  knnTraining[i].acczvar1=60.4;
  knnTraining[i].acczmean2=-2.66;
  knnTraining[i].acczvar2=42.4;
  knnTraining[i].acczmean3=0.382;
  knnTraining[i].acczvar3=21.4;
  knnTraining[i].acczmean4=-5.99;
  knnTraining[i].acczvar4=23.1;
  knnTraining[i].gyrxmean1=-104;
  knnTraining[i].gyrxvar1=82600;
  knnTraining[i].gyrxmean2=44.9;
  knnTraining[i].gyrxvar2=96400;
  knnTraining[i].gyrxmean3=-70.7;
  knnTraining[i].gyrxvar3=16900;
  knnTraining[i].gyrxmean4=3.44;
  knnTraining[i].gyrxvar4=38800;
  knnTraining[i].gyrymean1=-29.5;
  knnTraining[i].gyryvar1=12500;
  knnTraining[i].gyrymean2=-122;
  knnTraining[i].gyryvar2=36300;
  knnTraining[i].gyrymean3=-9.68;
  knnTraining[i].gyryvar3=6970;
  knnTraining[i].gyrymean4=-49.4;
  knnTraining[i].gyryvar4=5770;
  knnTraining[i].gyrzmean1=7.78;
  knnTraining[i].gyrzvar1=39500;
  knnTraining[i].gyrzmean2=68.4;
  knnTraining[i].gyrzvar2=33500;
  knnTraining[i].gyrzmean3=0.498;
  knnTraining[i].gyrzvar3=10800;
  knnTraining[i].gyrzmean4=-47.7;
  knnTraining[i].gyrzvar4=24800;
  knnTraining[i].knnclass=5;
  i++;
  //sample 22
  knnTraining[i].accxmean1=9.61;
  knnTraining[i].accxvar1=12;
  knnTraining[i].accxmean2=6.91;
  knnTraining[i].accxvar2=11.2;
  knnTraining[i].accxmean3=10;
  knnTraining[i].accxvar3=3.76;
  knnTraining[i].accxmean4=8.31;
  knnTraining[i].accxvar4=6.41;
  knnTraining[i].accymean1=-1.73;
  knnTraining[i].accyvar1=16.4;
  knnTraining[i].accymean2=-1.55;
  knnTraining[i].accyvar2=12;
  knnTraining[i].accymean3=-3.32;
  knnTraining[i].accyvar3=10.2;
  knnTraining[i].accymean4=-5.15;
  knnTraining[i].accyvar4=48.6;
  knnTraining[i].acczmean1=-2.05;
  knnTraining[i].acczvar1=11.6;
  knnTraining[i].acczmean2=2.91;
  knnTraining[i].acczvar2=38;
  knnTraining[i].acczmean3=1.57;
  knnTraining[i].acczvar3=18.9;
  knnTraining[i].acczmean4=-1.41;
  knnTraining[i].acczvar4=26.7;
  knnTraining[i].gyrxmean1=-20.8;
  knnTraining[i].gyrxvar1=10200;
  knnTraining[i].gyrxmean2=55.3;
  knnTraining[i].gyrxvar2=12900;
  knnTraining[i].gyrxmean3=-57.2;
  knnTraining[i].gyrxvar3=3210;
  knnTraining[i].gyrxmean4=-6.31;
  knnTraining[i].gyrxvar4=74100;
  knnTraining[i].gyrymean1=26.3;
  knnTraining[i].gyryvar1=10800;
  knnTraining[i].gyrymean2=19.3;
  knnTraining[i].gyryvar2=21700;
  knnTraining[i].gyrymean3=-74.9;
  knnTraining[i].gyryvar3=3910;
  knnTraining[i].gyrymean4=-0.00975;
  knnTraining[i].gyryvar4=21800;
  knnTraining[i].gyrzmean1=14.9;
  knnTraining[i].gyrzvar1=2230;
  knnTraining[i].gyrzmean2=40.2;
  knnTraining[i].gyrzvar2=8620;
  knnTraining[i].gyrzmean3=-7.8;
  knnTraining[i].gyrzvar3=5340;
  knnTraining[i].gyrzmean4=-4.71;
  knnTraining[i].gyrzvar4=31000;
  knnTraining[i].knnclass=5;
  i++;
  //sample 23
  knnTraining[i].accxmean1=7.96;
  knnTraining[i].accxvar1=8.29;
  knnTraining[i].accxmean2=9.48;
  knnTraining[i].accxvar2=6.85;
  knnTraining[i].accxmean3=10.5;
  knnTraining[i].accxvar3=10.4;
  knnTraining[i].accxmean4=10.1;
  knnTraining[i].accxvar4=40;
  knnTraining[i].accymean1=-3.09;
  knnTraining[i].accyvar1=15.8;
  knnTraining[i].accymean2=-3.28;
  knnTraining[i].accyvar2=22.1;
  knnTraining[i].accymean3=-2.91;
  knnTraining[i].accyvar3=10.3;
  knnTraining[i].accymean4=-1.6;
  knnTraining[i].accyvar4=19;
  knnTraining[i].acczmean1=-4.34;
  knnTraining[i].acczvar1=15.1;
  knnTraining[i].acczmean2=-2.91;
  knnTraining[i].acczvar2=4.97;
  knnTraining[i].acczmean3=-0.505;
  knnTraining[i].acczvar3=8.63;
  knnTraining[i].acczmean4=2.06;
  knnTraining[i].acczvar4=35.4;
  knnTraining[i].gyrxmean1=49.9;
  knnTraining[i].gyrxvar1=17500;
  knnTraining[i].gyrxmean2=-5.74;
  knnTraining[i].gyrxvar2=24900;
  knnTraining[i].gyrxmean3=-32.2;
  knnTraining[i].gyrxvar3=8610;
  knnTraining[i].gyrxmean4=16.5;
  knnTraining[i].gyrxvar4=9710;
  knnTraining[i].gyrymean1=-1.05;
  knnTraining[i].gyryvar1=13000;
  knnTraining[i].gyrymean2=33.1;
  knnTraining[i].gyryvar2=9480;
  knnTraining[i].gyrymean3=-20.8;
  knnTraining[i].gyryvar3=11200;
  knnTraining[i].gyrymean4=-42.3;
  knnTraining[i].gyryvar4=33100;
  knnTraining[i].gyrzmean1=3;
  knnTraining[i].gyrzvar1=7420;
  knnTraining[i].gyrzmean2=18.7;
  knnTraining[i].gyrzvar2=7840;
  knnTraining[i].gyrzmean3=-23;
  knnTraining[i].gyrzvar3=8020;
  knnTraining[i].gyrzmean4=7.74;
  knnTraining[i].gyrzvar4=15300;
  knnTraining[i].knnclass=5;
  i++;
  //sample 24
  knnTraining[i].accxmean1=-4.29;
  knnTraining[i].accxvar1=21.4;
  knnTraining[i].accxmean2=3.03;
  knnTraining[i].accxvar2=70.4;
  knnTraining[i].accxmean3=-2.33;
  knnTraining[i].accxvar3=32;
  knnTraining[i].accxmean4=-6.24;
  knnTraining[i].accxvar4=12;
  knnTraining[i].accymean1=-0.69;
  knnTraining[i].accyvar1=7.83;
  knnTraining[i].accymean2=-3.27;
  knnTraining[i].accyvar2=46.9;
  knnTraining[i].accymean3=-2.36;
  knnTraining[i].accyvar3=40.8;
  knnTraining[i].accymean4=-4.18;
  knnTraining[i].accyvar4=20.6;
  knnTraining[i].acczmean1=-10.4;
  knnTraining[i].acczvar1=332;
  knnTraining[i].acczmean2=-3.81;
  knnTraining[i].acczvar2=430;
  knnTraining[i].acczmean3=-4.15;
  knnTraining[i].acczvar3=193;
  knnTraining[i].acczmean4=-9.19;
  knnTraining[i].acczvar4=18;
  knnTraining[i].gyrxmean1=4.65;
  knnTraining[i].gyrxvar1=13600;
  knnTraining[i].gyrxmean2=15.9;
  knnTraining[i].gyrxvar2=47600;
  knnTraining[i].gyrxmean3=-29.3;
  knnTraining[i].gyrxvar3=43300;
  knnTraining[i].gyrxmean4=-14.9;
  knnTraining[i].gyrxvar4=2870;
  knnTraining[i].gyrymean1=216;
  knnTraining[i].gyryvar1=40400;
  knnTraining[i].gyrymean2=10.7;
  knnTraining[i].gyryvar2=126000;
  knnTraining[i].gyrymean3=-241;
  knnTraining[i].gyryvar3=76700;
  knnTraining[i].gyrymean4=30.9;
  knnTraining[i].gyryvar4=2950;
  knnTraining[i].gyrzmean1=59.9;
  knnTraining[i].gyrzvar1=5830;
  knnTraining[i].gyrzmean2=2.79;
  knnTraining[i].gyrzvar2=2210;
  knnTraining[i].gyrzmean3=10.1;
  knnTraining[i].gyrzvar3=34700;
  knnTraining[i].gyrzmean4=-10.9;
  knnTraining[i].gyrzvar4=3660;
  knnTraining[i].knnclass=5;
  i++;
  //sample 25
  knnTraining[i].accxmean1=9.41;
  knnTraining[i].accxvar1=30.3;
  knnTraining[i].accxmean2=3.89;
  knnTraining[i].accxvar2=22.1;
  knnTraining[i].accxmean3=2.11;
  knnTraining[i].accxvar3=31.8;
  knnTraining[i].accxmean4=-2.48;
  knnTraining[i].accxvar4=32.3;
  knnTraining[i].accymean1=0.773;
  knnTraining[i].accyvar1=33.1;
  knnTraining[i].accymean2=-11.1;
  knnTraining[i].accyvar2=83.9;
  knnTraining[i].accymean3=-4.09;
  knnTraining[i].accyvar3=57;
  knnTraining[i].accymean4=-10.4;
  knnTraining[i].accyvar4=13;
  knnTraining[i].acczmean1=2.09;
  knnTraining[i].acczvar1=12;
  knnTraining[i].acczmean2=-5.72;
  knnTraining[i].acczvar2=121;
  knnTraining[i].acczmean3=-1.02;
  knnTraining[i].acczvar3=28.9;
  knnTraining[i].acczmean4=-6.43;
  knnTraining[i].acczvar4=179;
  knnTraining[i].gyrxmean1=-207;
  knnTraining[i].gyrxvar1=35300;
  knnTraining[i].gyrxmean2=-10.5;
  knnTraining[i].gyrxvar2=71800;
  knnTraining[i].gyrxmean3=-28.1;
  knnTraining[i].gyrxvar3=11300;
  knnTraining[i].gyrxmean4=16;
  knnTraining[i].gyrxvar4=38600;
  knnTraining[i].gyrymean1=-69.9;
  knnTraining[i].gyryvar1=9680;
  knnTraining[i].gyrymean2=11.7;
  knnTraining[i].gyryvar2=45700;
  knnTraining[i].gyrymean3=-89.7;
  knnTraining[i].gyryvar3=15200;
  knnTraining[i].gyrymean4=-5.39;
  knnTraining[i].gyryvar4=84000;
  knnTraining[i].gyrzmean1=152;
  knnTraining[i].gyrzvar1=18700;
  knnTraining[i].gyrzmean2=-94.1;
  knnTraining[i].gyrzvar2=43900;
  knnTraining[i].gyrzmean3=78.6;
  knnTraining[i].gyrzvar3=63400;
  knnTraining[i].gyrzmean4=-46.2;
  knnTraining[i].gyrzvar4=7550;
  knnTraining[i].knnclass=5;
  i++;

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behaviour, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Initialize Bluefruit with max concurrent connections as Peripheral = 2, Central = 0
  Bluefruit.begin(MAX_PRPH_CONNECTION, 0);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  Serial.println("Please use Adafruit's Bluefruit LE app to connect in UART mode");
  Serial.println("Once connected, enter character(s) that you wish to send");
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}


// print a string to Serial Uart and all connected BLE Uart
void printAll(uint8_t* buf, int count)
{
    size_t sent;
  // Send to all connected centrals
  for (uint8_t conn_hdl=0; conn_hdl < MAX_PRPH_CONNECTION; conn_hdl++)
  {
    sent = bleuart.write(conn_hdl, buf, count);
    //Serial.println(sent);
  }
}

//width - MINIMUM width
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}

int count;
float accX;
float accY;
float accZ;
float gyrX;
float gyrY;
float gyrZ;
uint8_t buf[20];
IMUSample data[2000];
char mode;
KNNDistance distances[25];


int knncompfunc(const void * a, const void* b){ //função de comparação de distancias
  return( (*(KNNDistance*)a).distance - (*(KNNDistance*)b).distance);

}



void loop()
{

  while(Serial.available() && bleuart.available() && Serial.read() == '1'){ //espera pela coneção e envio de dados pelo ble e serial monitor
    for(int j=0; j<1000;j++){   //recolhe amostras do sensor a cada 10ms pr 10 segundos
      while (millis() < last_interval_ms + 10);
      last_interval_ms = millis();
      data[j].timestamp = last_interval_ms;
      data[j].accX = myIMU.readFloatAccelX() * CONVERT_G_TO_MS2;
      data[j].accY = myIMU.readFloatAccelY() * CONVERT_G_TO_MS2;
      data[j].accZ = myIMU.readFloatAccelZ() * CONVERT_G_TO_MS2;
      data[j].gyrX = myIMU.readFloatGyroX();
      data[j].gyrY = myIMU.readFloatGyroY();
      data[j].gyrZ = myIMU.readFloatGyroZ();
    }
    float accxvariance=0;
    float accxmean=0;
    float accxsub=0;
    float accyvariance=0;
    float accymean=0;
    float accysub=0;
    float acczvariance=0;
    float acczmean=0;
    float acczsub=0;
    float gyrxvariance=0;
    float gyrxmean=0;
    float gyrxsub=0;
    float gyryvariance=0;
    float gyrymean=0;
    float gyrysub=0;
    float gyrzvariance=0;
    float gyrzmean=0;
    float gyrzsub=0;
    KNNSample newsample;
    //calculo dos 48 indices usados no knn
    //medias
    for(int j=0;j<250;j++){
      accxmean += data[j].accX;
      accymean += data[j].accY;
      acczmean += data[j].accZ;
      gyrxmean += data[j].gyrX;
      gyrymean += data[j].gyrY;
      gyrzmean += data[j].gyrZ;
    }
    newsample.accxmean1 = accxmean / 250;
    newsample.accymean1 = accymean / 250;
    newsample.acczmean1 = acczmean / 250;
    newsample.gyrxmean1 = gyrxmean / 250;
    newsample.gyrymean1 = gyrymean / 250;
    newsample.gyrymean1 = gyrymean / 250;
    accxmean=0;
    accymean=0;
    acczmean=0;
    gyrxmean=0;
    gyrymean=0;
    gyrzmean=0;

    for(int j=250;j<500;j++){
      accxmean += data[j].accX;
      accymean += data[j].accY;
      acczmean += data[j].accZ;
      gyrxmean += data[j].gyrX;
      gyrymean += data[j].gyrY;
      gyrzmean += data[j].gyrZ;
    }
    newsample.accxmean2 = accxmean / 250;
    newsample.accymean2 = accymean / 250;
    newsample.acczmean2 = acczmean / 250;
    newsample.gyrxmean2 = gyrxmean / 250;
    newsample.gyrymean2 = gyrymean / 250;
    newsample.gyrymean2 = gyrymean / 250;
    accxmean=0;
    accymean=0;
    acczmean=0;
    gyrxmean=0;
    gyrymean=0;
    gyrzmean=0;
    
    for(int j=500;j<750;j++){
      accxmean += data[j].accX;
      accymean += data[j].accY;
      acczmean += data[j].accZ;
      gyrxmean += data[j].gyrX;
      gyrymean += data[j].gyrY;
      gyrzmean += data[j].gyrZ;
    }
    newsample.accxmean3 = accxmean / 250;
    newsample.accymean3 = accymean / 250;
    newsample.acczmean3 = acczmean / 250;
    newsample.gyrxmean3 = gyrxmean / 250;
    newsample.gyrymean3 = gyrymean / 250;
    newsample.gyrymean3 = gyrymean / 250;
    accxmean=0;
    accymean=0;
    acczmean=0;
    gyrxmean=0;
    gyrymean=0;
    gyrzmean=0;

    for(int j=750;j<1000;j++){
      accxmean += data[j].accX;
      accymean += data[j].accY;
      acczmean += data[j].accZ;
      gyrxmean += data[j].gyrX;
      gyrymean += data[j].gyrY;
      gyrzmean += data[j].gyrZ;
    }
    newsample.accxmean4 = accxmean / 250;
    newsample.accymean4 = accymean / 250;
    newsample.acczmean4 = acczmean / 250;
    newsample.gyrxmean4 = gyrxmean / 250;
    newsample.gyrymean4 = gyrymean / 250;
    newsample.gyrymean4 = gyrymean / 250;

    //variancias
    for(int j=0;j<250;j++){
      accxsub = data[j].accX - newsample.accxmean1;
      accxsub = accxsub * accxsub;
      accxvariance += accxsub;

      accysub = data[j].accY - newsample.accymean1;
      accysub = accysub * accysub;
      accyvariance += accysub;

      acczsub = data[j].accZ - newsample.acczmean1;
      acczsub = acczsub * acczsub;
      acczvariance += acczsub;

      gyrxsub = data[j].gyrX - newsample.gyrxmean1;
      gyrxsub = gyrxsub * gyrxsub;
      gyrxvariance += gyrxsub;

      gyrysub = data[j].gyrY - newsample.gyrymean1;
      gyrysub = gyrysub * gyrysub;
      gyryvariance += gyrysub;

      gyrzsub = data[j].gyrZ - newsample.gyrzmean1;
      gyrzsub = gyrzsub * gyrzsub;
      gyrzvariance += gyrzsub;
    }
    newsample.accxvar1 = accxvariance/250;
    newsample.accyvar1 = accyvariance/250;
    newsample.acczvar1 = acczvariance/250;
    newsample.gyrxvar1 = gyrxvariance/250;
    newsample.gyryvar1 = gyryvariance/250;
    newsample.gyrzvar1 = gyrzvariance/250;

    accxvariance =0;
    accyvariance =0;
    acczvariance =0;
    gyrxvariance =0;
    gyryvariance =0;
    gyrzvariance =0;


    for(int j=250;j<500;j++){
      accxsub = data[j].accX - newsample.accxmean2;
      accxsub = accxsub * accxsub;
      accxvariance += accxsub;

      accysub = data[j].accY - newsample.accymean2;
      accysub = accysub * accysub;
      accyvariance += accysub;

      acczsub = data[j].accZ - newsample.acczmean2;
      acczsub = acczsub * acczsub;
      acczvariance += acczsub;

      gyrxsub = data[j].gyrX - newsample.gyrxmean2;
      gyrxsub = gyrxsub * gyrxsub;
      gyrxvariance += gyrxsub;

      gyrysub = data[j].gyrY - newsample.gyrymean2;
      gyrysub = gyrysub * gyrysub;
      gyryvariance += gyrysub;

      gyrzsub = data[j].gyrZ - newsample.gyrzmean2;
      gyrzsub = gyrzsub * gyrzsub;
      gyrzvariance += gyrzsub;
    }
    newsample.accxvar2 = accxvariance/250;
    newsample.accyvar2 = accyvariance/250;
    newsample.acczvar2 = acczvariance/250;
    newsample.gyrxvar2 = gyrxvariance/250;
    newsample.gyryvar2 = gyryvariance/250;
    newsample.gyrzvar2 = gyrzvariance/250;

    accxvariance =0;
    accyvariance =0;
    acczvariance =0;
    gyrxvariance =0;
    gyryvariance =0;
    gyrzvariance =0;

    for(int j=500;j<750;j++){
      accxsub = data[j].accX - newsample.accxmean3;
      accxsub = accxsub * accxsub;
      accxvariance += accxsub;

      accysub = data[j].accY - newsample.accymean3;
      accysub = accysub * accysub;
      accyvariance += accysub;

      acczsub = data[j].accZ - newsample.acczmean3;
      acczsub = acczsub * acczsub;
      acczvariance += acczsub;

      gyrxsub = data[j].gyrX - newsample.gyrxmean3;
      gyrxsub = gyrxsub * gyrxsub;
      gyrxvariance += gyrxsub;

      gyrysub = data[j].gyrY - newsample.gyrymean3;
      gyrysub = gyrysub * gyrysub;
      gyryvariance += gyrysub;

      gyrzsub = data[j].gyrZ - newsample.gyrzmean3;
      gyrzsub = gyrzsub * gyrzsub;
      gyrzvariance += gyrzsub;
    }
    newsample.accxvar3 = accxvariance/250;
    newsample.accyvar3 = accyvariance/250;
    newsample.acczvar3 = acczvariance/250;
    newsample.gyrxvar3 = gyrxvariance/250;
    newsample.gyryvar3 = gyryvariance/250;
    newsample.gyrzvar3 = gyrzvariance/250;

    accxvariance =0;
    accyvariance =0;
    acczvariance =0;
    gyrxvariance =0;
    gyryvariance =0;
    gyrzvariance =0;



    for(int j=750;j<1000;j++){
      accxsub = data[j].accX - newsample.accxmean4;
      accxsub = accxsub * accxsub;
      accxvariance += accxsub;

      accysub = data[j].accY - newsample.accymean4;
      accysub = accysub * accysub;
      accyvariance += accysub;

      acczsub = data[j].accZ - newsample.acczmean4;
      acczsub = acczsub * acczsub;
      acczvariance += acczsub;

      gyrxsub = data[j].gyrX - newsample.gyrxmean4;
      gyrxsub = gyrxsub * gyrxsub;
      gyrxvariance += gyrxsub;

      gyrysub = data[j].gyrY - newsample.gyrymean4;
      gyrysub = gyrysub * gyrysub;
      gyryvariance += gyrysub;

      gyrzsub = data[j].gyrZ - newsample.gyrzmean4;
      gyrzsub = gyrzsub * gyrzsub;
      gyrzvariance += gyrzsub;
    }
    newsample.accxvar4 = accxvariance/250;
    newsample.accyvar4 = accyvariance/250;
    newsample.acczvar4 = acczvariance/250;
    newsample.gyrxvar4 = gyrxvariance/250;
    newsample.gyryvar4 = gyryvariance/250;
    newsample.gyrzvar4 = gyrzvariance/250;

       

   //calculo das distancias , sendo estas a norma do vetor formado entre a amostra e a amostra de treino, com 48 dimensões
   for(int j=0;j<25;j++){
      float sub;
      float diff =0;
      sub = newsample.accxmean1 - knnTraining[j].accxmean1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxvar1 - knnTraining[j].accxvar1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxmean2 - knnTraining[j].accxmean2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxvar2 - knnTraining[j].accxvar2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxmean3 - knnTraining[j].accxmean3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxvar3 - knnTraining[j].accxvar3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxmean4 - knnTraining[j].accxmean4;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accxvar4 - knnTraining[j].accxvar4;
      sub = sub * sub;
      diff+=sub;

      sub = newsample.accymean1 - knnTraining[j].accymean1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accyvar1 - knnTraining[j].accyvar1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accymean2 - knnTraining[j].accymean2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accyvar2 - knnTraining[j].accyvar2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accymean3 - knnTraining[j].accymean3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accyvar3 - knnTraining[j].accyvar3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accymean4 - knnTraining[j].accymean4;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.accyvar4 - knnTraining[j].accyvar4;
      sub = sub * sub;
      diff+=sub;

      sub = newsample.acczmean1 - knnTraining[j].acczmean1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczvar1 - knnTraining[j].acczvar1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczmean2 - knnTraining[j].acczmean2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczvar2 - knnTraining[j].acczvar2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczmean3 - knnTraining[j].acczmean3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczvar3 - knnTraining[j].acczvar3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczmean4 - knnTraining[j].acczmean4;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.acczvar4 - knnTraining[j].acczvar4;
      sub = sub * sub;
      diff+=sub;

      sub = newsample.gyrxmean1 - knnTraining[j].gyrxmean1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxvar1 - knnTraining[j].gyrxvar1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxmean2 - knnTraining[j].gyrxmean2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxvar2 - knnTraining[j].gyrxvar2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxmean3 - knnTraining[j].gyrxmean3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxvar3 - knnTraining[j].gyrxvar3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxmean4 - knnTraining[j].gyrxmean4;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrxvar4 - knnTraining[j].gyrxvar4;
      sub = sub * sub;
      diff+=sub;

      sub = newsample.gyrymean1 - knnTraining[j].gyrymean1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyryvar1 - knnTraining[j].gyryvar1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrymean2 - knnTraining[j].gyrymean2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyryvar2 - knnTraining[j].gyryvar2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrymean3 - knnTraining[j].gyrymean3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyryvar3 - knnTraining[j].gyryvar3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrymean4 - knnTraining[j].gyrymean4;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyryvar4 - knnTraining[j].gyryvar4;
      sub = sub * sub;
      diff+=sub;

      sub = newsample.gyrzmean1 - knnTraining[j].gyrzmean1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzvar1 - knnTraining[j].gyrzvar1;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzmean2 - knnTraining[j].gyrzmean2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzvar2 - knnTraining[j].gyrzvar2;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzmean3 - knnTraining[j].gyrzmean3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzvar3 - knnTraining[j].gyrzvar3;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzmean4 - knnTraining[j].gyrzmean4;
      sub = sub * sub;
      diff+=sub;
      sub = newsample.gyrzvar4 - knnTraining[j].gyrzvar4;
      sub = sub * sub;
      diff+=sub;

      distances[j].distance = diff;
      distances[j].knnclass = knnTraining[j].knnclass;
    } 

    qsort(distances,25,sizeof(KNNDistance), knncompfunc); //ordenação do array pelas distancias

    int ACcount =0;
    int ABcount =0;
    int DCcount =0;
    int SDcount =0;
    int Wcount =0;
    for(int j=0;j<KNNK;j++){ //usando a macro KNNK para poder alteraro k do knn, percorre o array organizado da menor distancia para a maior, verificando as classes das k primeiras e adicionando a um contador conforme a classe
      switch(distances[j].knnclass){
        case 1:
          ACcount++;
          break;
        case 2:
          ABcount++;
          break;
        case 3:
          DCcount++;
          break;
        case 4:
          SDcount++;
          break;
        case 5:
          Wcount++;
          break;
      }
    } //verifica qual a classe em maioria, enviando a classe correspondente por bluetooth serial
    if((ACcount > ABcount)&& (ACcount > DCcount) && (ACcount > SDcount) && (ACcount > Wcount)){
      bleuart.print("move:Angle and Circle\n");
    } else if((ABcount > ACcount)&& (ABcount > DCcount) && (ABcount > SDcount) && (ABcount > Wcount)){
      bleuart.print("move:Angular Bursts\n");
    } else if((DCcount > ABcount)&& (DCcount > ACcount) && (DCcount > SDcount) && (DCcount > Wcount)){
      bleuart.print("move:Diagonal cuts\n");
    } else if((SDcount > ABcount)&& (SDcount > DCcount) && (SDcount > ACcount) && (SDcount > Wcount)){
      bleuart.print("move:Suspend and drop\n");
    } else if((Wcount > ABcount)&& (Wcount > DCcount) && (Wcount > SDcount) && (Wcount > ACcount)){
      bleuart.print("move:waves\n");
    } 
  

  }
  

    
  
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);

  connection_count++;
  Serial.print("Connection count: ");
  Serial.println(connection_count);
  
  // Keep advertising if not reaching max
  if (connection_count < MAX_PRPH_CONNECTION)
  {
    Serial.println("Keep advertising");
    Bluefruit.Advertising.start(0);
  }
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);

  connection_count--; 
  
  
}

