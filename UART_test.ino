#include <LSM6DS3.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}
String receivedMessage;
String setting;

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
    char receivedChar = Serial.read();
    if (receivedChar == '\n') {
      Serial.print("i received: ");
      Serial.println(receivedMessage);  // Print the received message in the Serial monitor
      setting = receivedMessage;
      receivedMessage = "";  // Reset the received message
    } else {
      receivedMessage += receivedChar;  // Append characters to the received message
    }
  }

  char num = setting.charAt(0);
  
}
