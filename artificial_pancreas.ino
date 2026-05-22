/*
 * Artificial Pancreas model code
 * 
 */

 int pumpPin = 11; // pin used to control pump
 int pumpSpeed = 255; // pump speed, value between 0-255
 int conductivity;    // conductivity value, this will be 0-1023 from analogRead
 int threshold = 210; // conductivity threshold at which pump should turn off

void setup() {
  // put your setup code here, to run once:
  pinMode(pumpPin,OUTPUT); // set pump pin as output
  Serial.begin(9600);      // initialize serial communication

}

void loop() {
  // put your main code here, to run repeatedly:
  conductivity = analogRead(A0);  // get conductivity reading
  Serial.println(conductivity);
  if(conductivity>threshold){
    analogWrite(pumpPin,pumpSpeed); // turn pump on if threshold has not been reached yet
  }
  else{
    digitalWrite(pumpPin,LOW); // turn pump off if threshold has been reached
  }

}