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
#include <Arduino.h>
#include "FS.h"
#include <LITTLEFS.h>

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
#define FORMAT_LITTLEFS_IF_FAILED true

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

// SPIFFS-like write and delete file, better use #define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

void writeFile2(fs::FS &fs, const char * path, const char * message){
    if(!fs.exists(path)){
    if (strchr(path, '/')) {
            Serial.printf("Create missing folders of: %s\r\n", path);
      char *pathStr = strdup(path);
      if (pathStr) {
        char *ptr = strchr(pathStr, '/');
        while (ptr) {
          *ptr = 0;
          fs.mkdir(pathStr);
          *ptr = '/';
          ptr = strchr(ptr+1, '/');
        }
      }
      free(pathStr);
    }
    }

    Serial.printf("Writing file to: %s\r\n", path);
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void deleteFile2(fs::FS &fs, const char * path){
    Serial.printf("Deleting file and empty folders on path: %s\r\n", path);

    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }

    char *pathStr = strdup(path);
    if (pathStr) {
        char *ptr = strrchr(pathStr, '/');
        if (ptr) {
            Serial.printf("Removing all empty folders on path: %s\r\n", path);
        }
        while (ptr) {
            *ptr = 0;
            fs.rmdir(pathStr);
            ptr = strrchr(pathStr, '/');
        }
        free(pathStr);
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}

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
  }
t0 = millis();
#ifdef TWOPART
    if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED, "/lfs2", 5, "part2")){
    Serial.println("part2 Mount Failed");
    return;
    }
    appendFile(LITTLEFS, "/hello0.txt", "World0!\r\n");
    readFile(LITTLEFS, "/hello0.txt");
    LITTLEFS.end();

    Serial.println( "Done with part2, work with the first lfs partition..." );
#endif

    if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed");
        return;
    }
    createDir(LITTLEFS, "/datadir");
    writeFile(LITTLEFS, "/datadir/swdata.txt", "START_POINT");
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
        
          String Str = Serial.readString();
          if ((t0-millis())<300000){
          postData = Str + "\n" ;
          appendFile(LITTLEFS, "/datadir/swdata.txt", postData);
          }
          else if((t0-millis())>=300000){
            str= readFile(LITTLEFS, "/datadir/swdata.txt");
            char * pch;
            pch = strtok (str,"\n");
            while (pch != NULL){
              

//          HttpAddrStr =  ServerAddrStr + "post-data.php";

#ifdef Home
          http.begin("http://192.168.43.30/wtdata/post-data.php");              //Specify request destination
#endif

#ifdef FENET
          http.begin("http://192.168.2.101/wtdata/post-data.php");              //Specify request destination
#endif

 //         http.begin(HttpAddrStr);              //Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
          int httpCode = http.POST(pch);   //Send the request
          String payload = http.getString();    //Get the response payload

          Serial.println(payload);    //Print request response payload
          delay(50); // wait for 50ms.

          http.end();  //Close connection

         /*
          digitalWrite(LED, LOW);// turn the LED off.(Note that LOW is the voltage level but actually
                                //the LED is on; this is because it is acive low on the ESP8266.
          delay(100);            // wait for 1 second.
          digitalWrite(LED, HIGH); // turn the LED on.
        */
      
      //printf ("%s\n",pch);
      pch = strtok (NULL, "\n");
      }
      t0 = millis();
      } 
      }break; // wi-fi command
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
