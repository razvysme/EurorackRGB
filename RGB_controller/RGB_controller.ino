
#include <Filter.h> //from MegunoLink Library
#include <math.h>


//Define Pins
int greenPin = 9;
int redPin = 10;
int bluePin = 11;
int modeButton = 5;
int h_cv = 2;
int s_cv = 3;
int v_cv = 4;

//Analog input values
int huePot, satPot, lightPot, audioIn; 

//Global Max advancement for triggers on HSV. 
double step = 0.5; 

//offset to be added to the current HSV - mode 2 and 3
double offset[3];

//normalized HSV
double normaledHSV[3];

//HSV to be converted into RGB
double HSV[3];

//Modes of operation: 0 = Manual ,  1 = Sequencer, 2 = Amplitude 3
int mode = 1; 

//RGB values to output after HSV conversion
int RGB[3];

//global states for trigger checking
int lastHState, lastSState, lastVState = 0;

ExponentialFilter<long> FH(10, 0); //Exponential filters with 10% weight/step
ExponentialFilter<long> FS(10, 0);
ExponentialFilter<long> FV(10, 0);


void setup() { 
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT); 
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(h_cv, INPUT);
  pinMode(s_cv, INPUT);
  pinMode(v_cv, INPUT);
  pinMode(modeButton,INPUT);

}

void loop() {
  //reading the Potentiometers
	huePot = analogRead(0);
  satPot = analogRead(1);
  lightPot = analogRead(2);
  audioIn = analogRead(3);
  
  //reading the CV inputs
  readTrigger(h_cv);
  readTrigger(s_cv);
  readTrigger(v_cv);
  readTrigger(modeButton);

  //filtering the inputs 
  FH.Filter(huePot);
  FS.Filter(satPot);
  FV.Filter(lightPot);

  //map pots and normalize HSV
  normaledHSV[0] = map(FH.Current(),0,1023,0, 1000)/1000.0;
  normaledHSV[1] = map(FS.Current(),0,1023,0, 1000)/1000.0;
  normaledHSV[3] = map(FV.Current(),0,1023,0, 1000)/1000.0;
  
  setColors();

  //calculate HSV
  HSV[0] = fmod((normaledHSV[0]  + offset[0]), 1.0);
  HSV[1] = fmod((normaledHSV[1] + offset[1]), 1.0);
  HSV[2] = fmod((normaledHSV[3] + offset[2]), 1.0);
  
  //convert HueSatValue to RGB

  hsvToRgb(HSV[0], HSV[1], HSV[2]);

  //output Colors
  analogWrite(redPin,RGB[0]);
  analogWrite(greenPin,RGB[1]);
  analogWrite(bluePin,RGB[2]);

 /* 
  Serial.println(digitalRead(h_cv));
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

void hsvToRgb(double h, double s, double v) {
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

int readTrigger(int pin) {         
  int inputPinState = digitalRead(pin);
  if (inputPinState != lastHState) {
    if (inputPinState == HIGH) {
      // if the current state is HIGH then the state went from off to on:
      Serial.print(pin);
      Serial.println(" is on");  
      if(pin = modeButton){
        mode = (mode+1) % 3; //Cycle through modes
        Serial.println(mode);
      }
      setColors();
    } else {
      // if the current state is LOW then the state went from on to off:
      Serial.println("off");
    }
  }
  // save the current state as the last state, for next time through the loop
  lastHState = inputPinState;     
}

void setColors() {
  switch (mode)
    { 
      case 1: //second mode, offset is determined by pot value
        offset[1] = normaledHSV[0] * step;
        offset[2] = normaledHSV[1] * step;
        offset[3] = normaledHSV[3] * step;
        break;

      case 2: // third mode, "Value" determined by audio input
        offset[3] = map(audioIn, 0, 1023, 0.0, 1.0);
        break;

      default: // first mode, colors directly mapped to potentiometers
        for (int i = 0; i < 3; i++)
          {
            offset[i]= 0.0;
          }
          
        break;
    }

}

