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


void loop()
{
  
  /*char str[20];
  uint8_t bufFirst[21];
  char strFirst[21];
  uint8_t bufSecond[21];
  char strSecond[21];
  uint8_t bufThird[21];
  char strThird[21];
  char fbuf[10];
  */
  while(Serial.available() && bleuart.available()){
    Serial.println("enter 1 for 10 seconds or 2 for 20 seconds of capture\n");
    mode = Serial.read();
    switch (mode){
      case '1':
        for(int i=0; i<1000;i++){
          while (millis() < last_interval_ms + 10); 
          last_interval_ms = millis();
          data[i].timestamp = last_interval_ms;
          data[i].accX = myIMU.readFloatAccelX() * CONVERT_G_TO_MS2;
          data[i].accY = myIMU.readFloatAccelY() * CONVERT_G_TO_MS2;
          data[i].accZ = myIMU.readFloatAccelZ() * CONVERT_G_TO_MS2;
          data[i].gyrX = myIMU.readFloatGyroX();
          data[i].gyrY = myIMU.readFloatGyroY();
          data[i].gyrZ = myIMU.readFloatGyroZ();
        }
        for(int j =0; j<1000; j++){
          bleuart.printf("%ld,\t", data[j].timestamp);
          bleuart.printf("%03.2f,%03.2f,", data[j].accX, data[j].accY);
          bleuart.printf("%03.2f,%03.2f,",data[j].accZ, data[j].gyrX);
          bleuart.printf("%03.2f,%03.2f\n", data[j].gyrY, data[j].gyrZ);

        }
        break;

      case '2':
        for(int i=0; i<2000 ;i++){
          
          while (millis() < last_interval_ms + 10);
  
  
          last_interval_ms = millis();

          data[i].timestamp = last_interval_ms;

          data[i].accX = myIMU.readFloatAccelX() * CONVERT_G_TO_MS2;

          data[i].accY = myIMU.readFloatAccelY() * CONVERT_G_TO_MS2;
    
          data[i].accZ = myIMU.readFloatAccelZ() * CONVERT_G_TO_MS2;
   
          data[i].gyrX = myIMU.readFloatGyroX();
    
          data[i].gyrY = myIMU.readFloatGyroY();
    
         data[i].gyrZ = myIMU.readFloatGyroZ();
        }
        for(int j =0; j<2000; j++){
          bleuart.printf("%ld,\t", data[j].timestamp);
          bleuart.printf("%.4f,%.4f,", data[j].accX, data[j].accY);
          bleuart.printf("%.4f,%.2f,",data[j].accZ, data[j].gyrX);
          bleuart.printf("%.2f,%.2f\n", data[j].gyrY, data[j].gyrZ);

        }
        break;
      default:
      Serial.println("invalid input, 1 for 10seconds, 2 for 20 seconds\n");
      break;

    }
  

  }
  /*
    dtostrf(accX, 5, 4, fbuf);
    strcat(strFirst, fbuf);
    strcat(strFirst, ",");
    dtostrf(accY, 5, 4, fbuf);
    strcat(strFirst, fbuf);
    strcat(strFirst, ",\0");



    dtostrf(accZ, 5, 4, fbuf);
    strcat(strSecond, fbuf);
    strcat(strSecond, ",");
    dtostrf(gyrX, 3, 2, fbuf);
    strcat(strSecond, fbuf);
    strcat(strSecond, ",\0");

    
    dtostrf(gyrY, 3, 2, fbuf);
    strcat(strThird, fbuf);
    strcat(strThird, ",");
    dtostrf(gyrZ, 3, 2, fbuf);
    strcat(strThird, fbuf);
    strcat(strThird, "\n\0");

    ltoa(last_interval_ms, str, 10 );

    for (int i = 0; i < sizeof(strThird); i++){
      bufThird[i] = uint8_t(strThird[i]);
      bufFirst[i] = uint8_t(strFirst[i]);
      bufSecond[i] = uint8_t(strSecond[i]);
      if (i < sizeof(str)) {
        buf[i] = uint8_t(str[i]);
      }

    }
    

    
    


    count = sizeof(buf);
    printAll(buf, count);

    count = sizeof(bufFirst);
    printAll(bufFirst, count);

    printAll(bufSecond, count);

    printAll(bufThird, count);
    */
    
   /* bleuart.printf("%ld\t", last_interval_ms);
    bleuart.printf("%.4f,%.4f,", accX, accY);
    bleuart.printf("%.4f,%.2f,",accZ, gyrX);
    bleuart.printf("%.2f,%.2f\n", gyrY, gyrZ);
    */





    

    
   

    
  
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
