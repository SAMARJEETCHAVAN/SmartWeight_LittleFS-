/*
 * HTTP Client POST Request
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "LittleFS.h"

#define LED LED_BUILTIN // onboard LED

//#define Home
#define FENET

#ifdef Home
/* Set these to your desired credentials. */
const char *ssid = "FENET";  //ENTER YOUR WIFI SETTINGS
const char *password = "54321543";
String ServerAddrStr = "http://192.168.43.30/wtdata/";
#endif

#ifdef FENET
/* Set these to your desired credentials. */
const char *ssid = "FENET";  //ENTER YOUR WIFI SETTINGS
const char *password = "12345678";
String ServerAddrStr = "http://192.168.2.101/wtdata/";
#endif

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {

  pinMode(LED, OUTPUT);    // LED pin as output.

  delay(1000);
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(100);
////////////////////////
// use the alternate serial port lines
// for avoiding distarbance with main serial lines
// without this the serial port doesnot work with PIC micro
  Serial.swap();
  delay(100);
///////////////////////

  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    digitalWrite(LED, LOW);
    delay(100);             // wait for 1 second.
    digitalWrite(LED, HIGH); // turn the LED on.
    delay(100);            // wait for 1 second.
    t0 = millis();//record boot time.

  }
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    delay(1000);
    return;
  }
}

//=======================================================================
//                    Main Program Loop
//=======================================================================


void loop() {
  char c;
  HTTPClient http;    //Declare object of class HTTPClient
  String wt_value, wt_status, station, postData;
  String HttpAddrStr;

  while(!Serial.available());
  c = Serial.read();



  switch(c)
  {
    case 'w':
      {
          if((t0-millis()) <=300000){
            String Str = Serial.readString();
            postData = "data=" + Str ;
            File file = LittleFS.open("/datatoupload.txt", "w");
            file.print(Str+"\n");
            file.close();
            delay(1); 
            Serial.println("Write successful");
            }
          else if((t0-millis())>300000){
          
            #ifdef Home
              http.begin("http://192.168.43.30/wtdata/post-data.php");              //Specify request destination
            #endif

            #ifdef FENET
              http.begin("http://192.168.2.101/wtdata/post-data.php");              //Specify request destination
            #endif
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
            int httpCode = http.POST(postData);   //Send the request
            String payload = http.getString();    //Get the response payload
            Serial.println(payload);    //Print request response payload
            delay(50); // wait for 50ms.
            http.end();  //Close connection
    }
      } break; // wi-fi command

    //////////////////////////////////
    // get username from RFID code

    case 'r':
      {
          // read the RFID code
          String Strrfid = Serial.readString();
          postData = "rfid=" + Strrfid ;

 //         HttpAddrStr =  ServerAddrStr + "getRFIDuser.php";

#ifdef Home
          http.begin("http://192.168.43.30/wtdata/getRFIDuser.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/getRFIDuser.php");              //Specify request destination
#endif

//          http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          delay(50); // wait for 50ms.

          http.end();  //Close connection

          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.

      } break; // rfid get user command

    //////////////////////////////////
    // get min and max wt, tare wt from RFID code
    // send small L (l) to get the wt limits (min and max wt, min and max tare wt.
    case 'l':
      {
          // read the RFID code
          String Strfgcode = Serial.readString();
          postData = "FGcode=" + Strfgcode ;

          //HttpAddrStr =  ServerAddrStr + "getMinWt.php";

#ifdef Home
          http.begin("http://192.168.43.30/wtdata/getMinWt.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/getMinWt.php");              //Specify request destination
#endif
          //http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          //delay(100); // wait for 50ms.

          http.end();  //Close connection

          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          //delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.

      } break; // rfid get user command


    //////////////////////////////////
    // get min and max wt, tare wt from RFID code
    // send small L (l) to get the wt limits (min and max wt, min and max tare wt.
    case 'm':
      {
          // read the RFID code
          String Strfgcode = Serial.readString();
          postData = "FGcode=" + Strfgcode ;

//          HttpAddrStr =  ServerAddrStr + "getMaxWt.php";


#ifdef Home
          http.begin("http://192.168.43.30/wtdata/getMaxWt.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/getMaxWt.php");              //Specify request destination
#endif



//          http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          //delay(100); // wait for 50ms.

          http.end();  //Close connection

          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          //delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.

      } break; // rfid get user command

    //////////////////////////////////
    // get min and max wt, tare wt from RFID code
    // send small L (l) to get the wt limits (min and max wt, min and max tare wt.
    case 'n':
      {
          // read the RFID code
          String Strfgcode = Serial.readString();
          postData = "FGcode=" + Strfgcode ;

//          HttpAddrStr =  ServerAddrStr + "getMinTare.php";


#ifdef Home
          http.begin("http://192.168.43.30/wtdata/getMinTare.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/getMinTare.php");              //Specify request destination
#endif



//          http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          //delay(100); // wait for 50ms.

          http.end();  //Close connection

          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          //delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.

      } break; // rfid get user command

    //////////////////////////////////
    // get min and max wt, tare wt from RFID code
    // send small L (l) to get the wt limits (min and max wt, min and max tare wt.
    case 'o':
      {
          // read the RFID code
          String Strfgcode = Serial.readString();
          postData = "FGcode=" + Strfgcode ;

//          HttpAddrStr =  ServerAddrStr + "getMaxTare.php";

#ifdef Home
          http.begin("http://192.168.43.30/wtdata/getMaxTare.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/getMaxTare.php");              //Specify request destination
#endif



 //         http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          //delay(100); // wait for 50ms.

          http.end();  //Close connection

          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          //delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.

      } break; // rfid get user command



    case 'p':
      {
          // read the RFID code
          String Strfgcode = Serial.readString();
          postData = "stationID=" + Strfgcode ;

//          HttpAddrStr =  ServerAddrStr + "getMaxTare.php";

#ifdef Home
          http.begin("http://192.168.43.30/wtdata/getLineFGControl.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/getLineFGControl.php");              //Specify request destination
#endif



 //         http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          //delay(100); // wait for 50ms.

          http.end();  //Close connection

          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          //delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.

      } break; // rfid get user command





     case 't': // test wi-fi module
      {
        postData = "a"; //dummy data
        // check wi-fi. If its ok check server...
        if(WiFi.status() == WL_CONNECTED)
        {

#ifdef Home
          http.begin("http://192.168.43.30/wtdata/ping.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/ping.php");              //Specify request destination
#endif



 //         http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(postData);   //Send the request
          String payload = http.getString();    //Get the response payload
          // if $ returned, then database is connected..
          Serial.println(payload);    //Print request response payload
          //delay(100); // wait for 50ms.

          http.end();  //Close connection
        }
        else
        {Serial.print('@');}

        delay(50); // wait for 50ms.
      } break;

     default:{}
  }

  //delay(500); // wait for 1 second.
}
//=======================================================================
