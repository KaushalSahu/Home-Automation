 #include <SoftwareSerial.h>
int cel;
int tempval;
int temppin=A5;
int error;
int ledPin = 13;                //pin for ldr light 
int ledPin1 = 8;                // choose the pin for the LED for motion
int inputPin = 3;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int sensorPin = A0;             // select the input pin for the LDR
int sensorValue = 0;            // variable to store the value coming from the sensor

SoftwareSerial ser(10, 11); // RX, TX
void setup()
    {
        pinMode(ledPin, OUTPUT);      // declare LED as output
        pinMode(ledPin1, OUTPUT);
        pinMode(inputPin, INPUT);     // declare sensor as input
        Serial.begin(115200);
        ser.begin(115200);
        
        // reset ESP8266
        ser.println("AT+RST");
    }

void loop() 
        {
              // start working...
              Serial.println("=================================");
              Serial.println("Sample LM35...");
                    if(ser.available()){
                      Serial.println(ser.read());
                      }
                    //temp open
                    tempval = analogRead(temppin);
                    float mv = ( tempval/1024.0)*5000; 
                    float cel = mv/10;
                    //float farh = (cel*9)/5 + 32;
                    Serial.print("TEMPRATURE=");
                    Serial.print (cel);
                    Serial.print("*C");
                    Serial.println();
                    //delay(1000);
                    //temp close
                    
                    //ldr sensor start
                    sensorValue = analogRead(sensorPin);
                    Serial.println(sensorValue);
                    
                    val = digitalRead(inputPin);  // read input value
                    //Serial.println(val);
                    if (val == HIGH && cel>35) // check if the input is HIGH
                    {            
                    digitalWrite(ledPin1, HIGH);  // turn LED ON
                    
                    
                    if (sensorValue < 500)
                    {
                    Serial.println("LED light on");
                    digitalWrite(ledPin,HIGH);
                    //delay(1000);
                    }
                    else
                    {
                    Serial.println("LED light oFF");
                    digitalWrite(ledPin,LOW);
                    //delay(sensorValue);
                    }
                    //ldr stop
                    //motion sensor start
                    if (pirState == LOW) {
                    // we have just turned on
                    Serial.println("Motion detected and Fan On ");
                    // We only want to print on the output change, not state
                    pirState = HIGH;
                  }
               }
              else if (val == HIGH && cel<35) // check if the input is HIGH
               {            
              digitalWrite(ledPin1, HIGH);  // turn LED OFF
              if (sensorValue < 500)
              
              {
              Serial.println("LED light on");
              digitalWrite(ledPin,HIGH);
              //delay(1000);
              }
              else
              {
              Serial.println("LED light oFF");
              digitalWrite(ledPin,LOW);
              //delay(sensorValue);
              }
              if (pirState == LOW) {
              // we have just turned on
              Serial.println("Motion detected and Fan OFF ");
              // We only want to print on the output change, not state
              pirState = HIGH;
              }
              }
              
              else {
              digitalWrite(ledPin1, LOW);   
              digitalWrite(ledPin, LOW);// turn LED OFF
              if (pirState == HIGH){                               // we have just turned of
              Serial.println("Motion ended and Fan Off ");        // We only want to print on the output change, not state
              pirState = LOW;
              }
              }
              //motion sensor stop
              
              //data uploading to thingspeak
              sendToThingSpeak((int)cel,(int)sensorValue,(int)val);
              // LM35 sampling rate is 1HZ.
              //delay(1000);  
        }

void sendToThingSpeak(int t,int l,int h)
{

String cmd = "AT+CIPSTART=\"TCP\",\"";
cmd += "184.106.153.149"; // api.thingspeak.com
cmd += "\",80";
ser.println(cmd);

if(ser.find("Error")){
  Serial.println("AT+CIPSTART error");
  return;
}
// prepare GET string
String getStr = "GET https://api.thingspeak.com/update?api_key=L56YQEXNC1N4C123";
getStr +="&field1=";
getStr += String(t);
getStr +="&field2=";
getStr += String(l);
getStr +="&field3=";
getStr += String(h);

getStr += "\r\n\r\n";

// send data length
cmd = "AT+CIPSEND=";
cmd += String(getStr.length());
ser.println(cmd);

if(ser.find(">")){
  ser.print(getStr);
}
else{
  ser.println("AT+CIPCLOSE");
  // alert user
  Serial.println("AT+CIPCLOSE");
}

// thingspeak needs 15 sec delay between updates
delay(16000);  
}

