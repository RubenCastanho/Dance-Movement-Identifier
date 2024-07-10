#include <LSM6DS3.h>

void setup() {
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
}
char* receivedMessage;
char* setting;

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("choose LED mode: 0-off, 1-red, 2-blue, 3- green");
  while(Serial.available() <= 0){
    Serial.println("waiting...");
    delay(1000);
  }
   if(Serial.available() > 0)  {
    int incomingData= Serial.read(); // can be -1 if read error
    switch(incomingData) { 
        case '0':
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_BLUE, HIGH);
          digitalWrite(LED_GREEN, HIGH);
          Serial.println("mode:off");
          delay(1000);
          break;
        case '1':
          digitalWrite(LED_BLUE, HIGH);
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_RED, LOW);
          Serial.println("mode:red");
          delay(1000);
           // handle ‘1’
           break;

        case '2':
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_BLUE, LOW);
          Serial.println("mode:blue");
          delay(1000);
           // handle ‘2’
           break;

        case '3':
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_BLUE, HIGH);
          digitalWrite(LED_GREEN, LOW);
          Serial.println("mode:green");
          delay(1000);
           // handle ‘3’
           break;

        default:
           // handle unwanted input here
            Serial.println("invalid mode,try again");
             delay(1000);
           break;
   }
 }

 
  
}
