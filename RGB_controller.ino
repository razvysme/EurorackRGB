#include "Filter.h" //from MegunoLink Library
int huePot, satPot, lightPot; 

int greenPin = 9;
int redPin = 10;
int bluePin = 11;

ExponentialFilter<long> FH(10, 0); //Exponential filters with 10% weight/step
ExponentialFilter<long> FS(10, 0);
ExponentialFilter<long> FV(10, 0);

int RGB[3]; // used for HSV to RGB conversion

//test GIt

void setup() { 
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT); 
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

}

void loop() {
  //reading the Potentiometers
	huePot = analogRead(0);
  satPot = analogRead(1);
  lightPot = analogRead(2);

  //filtering the inputs 
  FH.Filter(huePot);
  FS.Filter(satPot);
  FV.Filter(lightPot);

  //convert HueSatValue to RGB
  hsvToRgb(map(FH.Current(),0,1023,0, 1000)/1000.0, map(FS.Current(),0,1023,0, 1000)/1000.0, map(FV.Current(),0,1023,0, 1000)/1000.0);

  //output Colors
  analogWrite(redPin,RGB[0]);
  analogWrite(greenPin,RGB[1]);
  analogWrite(bluePin,RGB[2]);

 /* 
  Serial.print("Hue is: ");
  Serial.print(huePot);
  Serial.print(", Saturation is: ");
  Serial.print(satPot);
  Serial.print(", Value is: ");
  Serial.println(lightPot);
  Serial.println(map(huePot,-1,1023,0, 1000)/1000.0);
  Serial.print(huePot);
  Serial.print(" ");
  Serial.println(FH.Current());
*/
  delay(50);
}

void hsvToRgb(double h, double s, double v){
    double r, g, b;
    int i = int(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    RGB[0] = r * 255;
    RGB[1] = g * 255;
    RGB[2] = b * 255;

}

