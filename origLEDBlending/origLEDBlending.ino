////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LED NeoPixel Value Blending                                                                            //
// Written by Kevin Edzenga                                                                               //
//                                                                                                        //
// This file is an example of doing cross cycle LED value blending for Adafruit NeoPixels                 //
// The LED color blending doesn't need to be for Adadfruit NeoPixels,                                     //
//   this is just the application I needed the scripting for.                                             //
// The line -   strip.setPixelColor(i, strip.Color(  int(fadeR),   int(fadeG), int(fadeB)));              //
// Is where individual NeoPixel values are set, prior to displaying on the strip                          //
//                                                                                                        //
// The line -   strip.show();                                                                             //
// Is where the NeoPixels values are set, you can use ' byte fadeR, fadeG, fadeB; ' variables             //
//   how ever you'd like once blended.                                                                    //
//                                                                                                        //
// For ease of use, NeoPixels are amazing-                                                                //
// https://www.adafruit.com/category/168                                                                  //
//                                                                                                        //
// Needed headers---                                                                                      //
// https://github.com/adafruit/Adafruit_NeoPixel                                                          //
//                                                                                                        //
// For the breadboard layout, 300-500 ohm resistor on pin 6 to SPI data on the NeoPixels                  //
// 1000-4700 uf 16v capacitor should be used on VCC / GND                                                 //
// http://www.studiopieters.nl/wp-content/uploads/2017/01/Knowledge-Base-%E2%80%93-NeoPixel-Strip_bb.png  //
//                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Adafruit_NeoPixel.h>
#include <math.h> // Linux
// Windows seems to use a capital M, you may need to change this per OS
// #include <Math.h>

// LED Strip
#define PIN 6 // This is your SPI out to your NeoPixel strip
const int maxled=3; // How many LEDs are on the NeoPixel string, or RGB leds you are working with
const int blendCount=10;

// For prepping maximum blended values; I'm using a 3 dimensional array (Below)
// The first array is the number of lights
// The second array is the maximum levels of blending
// The third array is R, G, B respectively
int lightVals[maxled][blendCount][3];

float runner=0;
float mmRGB=100;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(maxled, PIN, NEO_GRB + NEO_KHZ800);  //NeoPixel prep

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  int i,x,c;
  // Make sure all previous blend values are 0'ed out
  for(i=0; i<maxled;i++){  // lightVals[#][-][-] 
    for(x=0; x<blendCount;x++){ // lightVals[-][#][-]
      for(c=0; c<3;c++){ // lightVals[-][-][#]
        lightVals[i][x][c]=0;
      }
    }
  }

  //Serial.begin(9600);
}

void loop() {
  if(int(runner)%200<100){
    ledFader(10); // Fade 'random' colors through your string
  }else{
    ledRun(10); // Run white leds down your string, showing the fade off rates
  }
}

void ledRun(uint8_t wait) {
  float i, j, colorMode, div;
  byte fadeR, fadeG, fadeB;
  runner+=1; // Add an arbitrary ammount per cycle, this is how fast it will run through the noise pattern in the for loop below
  int mmRGB=255; // 255 is the max brightness an LED can be, but you can set it anywhere between 0-255
  
  // To use the test fade mode, leave alwaysOn=0
  int alwaysOn=0;
  int colorFor=50; // How long it will be adding colors into the blend
  int fadeOutFor=150; // How long it will be adding 0's into the colors; to show the color fade out
  
  for(i=0;i< strip.numPixels(); i++) {
    if(i == int(runner)%maxled){
      fadeR=255;
      fadeG=255;
      fadeB=255;
    }else{
      fadeR=0;
      fadeG=0;
      fadeB=0;
    }
    smoothRGB(i,blendCount, &fadeR,&fadeG,&fadeB);
    strip.setPixelColor(i, strip.Color(  int(fadeR),   int(fadeG), int(fadeB)));
  }
  strip.show();
  delay(wait);
}

void ledFader(uint8_t wait) {
  float i, j, colorMode, div;
  byte fadeR, fadeG, fadeB;
  runner+=1; // Add an arbitrary ammount per cycle, this is how fast it will run through the noise pattern in the for loop below
  int mmRGB=255; // 255 is the max brightness an LED can be, but you can set it anywhere between 0-255
  
  // To use the test fade mode, leave alwaysOn=0
  int alwaysOn=1;
  int colorFor=50; // How long it will be adding colors into the blend
  int fadeOutFor=150; // How long it will be adding 0's into the colors; to show the color fade out
  
  for(i=0;i< strip.numPixels(); i++) {
    if( int(floor(runner))%(colorFor+fadeOutFor) < colorFor || alwaysOn ){
      fadeR= max(0,min(mmRGB, (sin(i/(3.14159265*2)+sin(runner/35.515)*3.1415+sin(-runner/330.5)*30+sin(-runner/170.9)*25))*1.25*mmRGB ));
      fadeG= max(0,min(mmRGB, (cos(i/(3.14159265*2)-sin(-runner/25.58)*3.1415+sin(-runner/250.8)*30-cos(runner/102.2)*20))*1.25*mmRGB ));
      fadeB= max(0,min(mmRGB, (sin(i/(3.14159265*2)-cos(runner/50.45)*3.1415+sin(runner/300.1)*30-cos(-runner/158.3)*30))*1.25*mmRGB ));
    }else{
      fadeR=0;
      fadeG=0;
      fadeB=0;
    }      
    smoothRGB(i,blendCount, &fadeR,&fadeG,&fadeB);
    strip.setPixelColor(i, strip.Color(  int(fadeR),   int(fadeG), int(fadeB)));
  }
  strip.show();
  delay(wait);
}


////////////////////////////////////////////////////////////////////////////
// Blending Function                                                      //
////////////////////////////////////////////////////////////////////////////
// smoothRGB( # LED to blend in lightVals array, Smooth ammount, Red val, Green val, Blue val)
void smoothRGB(int i, float smoothDist, byte *fadeR, byte *fadeG, byte *fadeB){
  float dispVal, curvVal;
  int x,c,divCount;
  int runAdd=1;
  int blender=smoothDist-1;
  runAdd=(blender-1);
  int prevRunAdd=runAdd;
  for(c=0; c<3; c++){
    runAdd=prevRunAdd;
    if(c==0){
      curvVal=max(0,min(mmRGB, (*fadeR) ));
    }else if(c==1){
      curvVal=max(0,min(mmRGB, (*fadeG) ));
    }else{
      curvVal=max(0,min(mmRGB, (*fadeB) ));
    }
    runAdd+=smoothDist;
    curvVal=(curvVal+lightVals[i][blender][c]*(max(0,smoothDist)))/(smoothDist+1);
    dispVal+=curvVal*runAdd;
    divCount=runAdd;
    lightVals[i][blender][c]=curvVal;
    for(x=0;x<blender-1;x++){
      runAdd=(x+1);
      divCount+=runAdd;
      curvVal=lightVals[min(blender-1,i+1)][x][c];
      curvVal=(curvVal+lightVals[i][x][c]*(max(1,smoothDist)-1))/max(1,smoothDist);
      dispVal+=curvVal*runAdd;
      lightVals[i][x][c]=curvVal;
    }
    dispVal/=divCount;
    if(c==0){
      *fadeR=max(0,min(mmRGB, float(dispVal)));
    }else if(c==1){
      *fadeG=max(0,min(mmRGB, float(dispVal)));
    }else{
      *fadeB=max(0,min(mmRGB, float(dispVal)));
    }
  }
}
