/*
  See https://www.RoxXxtar.com/bmc for more details
  Copyright (c) 2020 RoxXxtar.com
  Licensed under the MIT license.
  See LICENSE file in the project root for full license information.
*/
// This class handles both Pixels and RGB Pixels and they are fed fom the same data pin
// If you are using both Pixels and RGB Pixels then BMC will address
// Pixels first then RGB Pixels.
// For example: Pixel#1, Pixel#2, Pixel#3, RgbPixel#1, RgbPixel#2, RgbPixel#3
// If you're only using one type then just chain as usual
// The current state and color of each pixel is stored in currentColor[n]
// It's 8-bits and it holds the BMC color not and RGB value, there are 15 BMC Colors
// so only 4 bits of that byte are used, then the 8th bit represents weather the
// Pixel is ON or OFF, obviously if that bit is 1 then the Pixel is ON otherwise OFF
// BMC also allows you to rearrange the order of Pixels, that is Pixel #1 can
// positioned last in the chain or anywhere in the chain for that matter. The
// same goes for RGB Pixels.

// RGB Pixels behave different, Red, Green & Blue are all addressed independently.
// Each color has it's own event, so their color is based on which event is true.

#ifndef BMC_PIXELS_H
#define BMC_PIXELS_H

#include "utility/BMC-Def.h"

#if ((BMC_PIXELS_PORT > 0) && (BMC_MAX_PIXELS > 0 || BMC_MAX_RGB_PIXELS > 0))

#include <WS2812Serial.h>

#define BMC_PIXELS_FLAG_SHOW 0
#define BMC_PIXELS_FLAG_USE_DIM 1
#define BMC_PIXELS_FLAG_RAINBOW_CHANGED 2

// Default Data Trasmition order is RGB, most WS2812 however are GRB
// I made this the default since the first NeoPixels I tried were the 5mm diffused
// NeoPixels which are RGB
#if !defined(BMC_PIXELS_RGB_MODE)
  #define BMC_PIXELS_RGB_MODE WS2812_RGB
#endif

// BMC_PIXEL_MAX_BRIGHTNESS must be higher than BMC_PIXEL_DIM_BRIGHTNESS
#if BMC_PIXEL_DIM_BRIGHTNESS >= BMC_PIXEL_MAX_BRIGHTNESS
  #error "BMC_PIXEL_DIM_BRIGHTNESS must be a lower value than BMC_PIXEL_MAX_BRIGHTNESS"
#endif

// The value of the dimmed color (or brightness) is stored by an array "dimColors",
// this is an 8-bit unsigned, each element of the array stores 2 dim values.
// the first element's four least significant bits hold the dim value for the first pixel
// the four most significant bits hold the dim value for the second pixel,
// the second element's four least significant bits hold the dim value for
// the third pixel and so on.
// this define determines how many elements that array should have
#define BMC_TOTAL_DIM_COLORS (BMC_IS_EVEN(BMC_TOTAL_PIXELS)?(BMC_TOTAL_PIXELS/2):((BMC_TOTAL_PIXELS/2)+1))

class BMCPixels {
public:
  BMCPixels():pixels(BMC_TOTAL_PIXELS,
                  displayMemory,
                  drawingMemory,
                  BMC_PIXELS_PIN,
                  BMC_PIXELS_RGB_MODE){
    // begin right away to avoid issues with leds being started with
    // full brightness, tho so far no luck there at least with the pixels i own...
    pixels.begin();
    pixels.setBrightness(BMC_PIXEL_MAX_BRIGHTNESS);
    pixels.show();

#if defined(BMC_PIXELS_ENABLE_PIN)
  #if !defined(BMC_PIXELS_ENABLE_PIN_MODE)
    #define BMC_PIXELS_ENABLE_PIN_MODE 0
  #endif
    // If your pixels turn on by default when they are first powered up
    // you can setup a pin to go HIGH when BMC starts, this pin can then be
    // connected to a MOSFET or Relay, this circuit can then turn on the power
    // line to the pixels, you should also put a large capacitor between
    // the 5V and GND lines right before feeding the pixles, a 1000uF capacitor
    // is usually recommended tho if you are not using a lot of pixels you could
    // use a smaller capacitor, I usually use 220uF capacitors for BMC
    // builds with a few pixels. The 250ms delay should give the capacitor some time
    // to charge up to help keep the LEDs from turning on.
    delay(250);
    pinMode(BMC_PIXELS_ENABLE_PIN, OUTPUT);
    digitalWriteFast(BMC_PIXELS_ENABLE_PIN, ((BMC_PIXELS_ENABLE_PIN_MODE==0)?LOW:HIGH));
    delay(1);
    pixels.show();
#endif

    reset(false);
    delay(50);
  }

  void begin(){
    BMC_PRINTLN("BMCPixels::begin()");
    BMC_PRINTLN("    BMC_PIXEL_MAX_BRIGHTNESS:", BMC_PIXEL_MAX_BRIGHTNESS);
    BMC_PRINTLN("    BMC_PIXEL_DIM_BRIGHTNESS:", BMC_PIXEL_DIM_BRIGHTNESS);

#if BMC_MAX_PIXELS > 0
    rainbowCurrentColor = BMC_COLOR_RED;
    for(uint8_t i=0; i<BMC_MAX_PIXELS; i++){
      setDimColor(i, BMCBuildData::getPixelDefaultColor(i));
      #if !defined(BMC_NO_LED_TEST_AT_LAUNCH)
        test(i);
      #endif
    }
#endif

#if BMC_MAX_RGB_PIXELS > 0
    for(uint8_t i=0,n=BMC_MAX_PIXELS; i<BMC_MAX_RGB_PIXELS; i++, n++){
      setDimColor(n, BMCBuildData::getRgbPixelDefaultColor(i));
      #if !defined(BMC_NO_LED_TEST_AT_LAUNCH)
        testRgb(i);
      #endif
    }
#endif
    BMC_PRINTLN("");
  }

  void clockBeat(uint16_t speed){
#if BMC_MAX_PIXELS > 0
    updateRainbowColor(60000/speed);
#else
    speed = 0;
#endif
  }

  void reassign(){
#if BMC_MAX_PIXELS > 0
    for(uint8_t i=0; i<BMC_MAX_PIXELS; i++){
      off(i);
    }
#endif

#if BMC_MAX_RGB_PIXELS > 0
    for(uint8_t i=0; i<BMC_MAX_RGB_PIXELS; i++){
      off(BMC_MAX_PIXELS+i);
    }
#endif
    reset();
  }

  void test(uint8_t n){
    if(n >= BMC_TOTAL_PIXELS){
      return;
    }
    // turn pixel on and off
    setPixelValue(n, BMCPixelColors::getRgbColor(random(1,13)));
    pixels.show();
    delay(BMC_MAX_LED_TEST_DELAY);
    setPixelValue(n, 0);
    pixels.show();
    delay(BMC_MAX_LED_TEST_DELAY);

    // return pixel to original state
    uint8_t prev = currentColor[n];
    if(bitRead(prev, 7)){
      setPixelValue(n, BMCPixelColors::getRgbColor(prev));
    } else {
      turnPixelOff(n);
    }
    pixels.show();
    delay(5);
  }
#if BMC_MAX_RGB_PIXELS > 0
  void testRgb(uint8_t n){
    test(n + BMC_MAX_PIXELS);
  }
#endif
  // store dim colors in an array of uint8_t, each pixel uses 4 bits
  // the 4 least significant bits are the first pixel, the 4 most significant
  // are the next pixel, then the next element of the array the same and so on
  // this function is used when the pixel is assigned so that dim color is
  // always ready to be used.
  void setDimColor(uint8_t n, uint8_t color){
    color &= 0x0F;
    if(color==0){
      // if the dimmed color is 0/black use default color
      color = getDefaultColor(n);

#if BMC_MAX_PIXELS > 0
    } else if(color==BMC_COLOR_RAINBOW){
      // only Pixels (not RGB) use rainbow
      color = rainbowCurrentColor;
#endif
    }
    uint8_t offset = (uint8_t) ((n>0) ? (n/2) : 0);
    if(offset > BMC_TOTAL_DIM_COLORS){
      return;
    }
    uint8_t tmp = dimColors[offset];
    if(BMC_IS_EVEN(n)){
      tmp &= 0xF0;
      tmp |= (color&0x0F);
    } else {
      tmp &= 0x0F;
      tmp |= (color<<4);
    }
    dimColors[offset] = tmp;
  }

  void update(){
#if BMC_MAX_PIXELS > 0
    for(uint8_t i=0; i<BMC_MAX_PIXELS; i++){
      if(pulseTimer[i].complete()){
        off(i);
      }
    }
#endif

#if BMC_MAX_RGB_PIXELS > 0
    for(uint8_t i=0,n=BMC_MAX_PIXELS; i<BMC_MAX_RGB_PIXELS; i++,n++){
      if(pulseTimer[n].complete()){
        // restore the pulsed led
        // since RGB pixels can have each of the 3 colors controller separately
        // we have to only turn off the color that was pulsed, the color being
        // pulse is stored by rgbPulseReset so we XOR it agains the current color
        // in case any of the other 2 colors have an event and are being turned
        // on by something else.
        write(n, ( (currentColor[n]&7) ^ rgbPulseReset[i] ));
        // reset pulse the color
        rgbPulseReset[i] = 0;
      }
    }
#endif

    if(flags.toggleIfTrue(BMC_PIXELS_FLAG_SHOW)){
      pixels.show();
      flags.off(BMC_PIXELS_FLAG_RAINBOW_CHANGED);
    }
  }

  void setPwmOffValue(bool value){
    if(value != flags.read(BMC_PIXELS_FLAG_USE_DIM)){
      flags.write(BMC_PIXELS_FLAG_USE_DIM, value);
      for(uint8_t i=0;i<BMC_TOTAL_PIXELS;i++){
        if(bitRead(currentColor[i], 7)){
          // swap the on state of the led
          bitWrite(currentColor[i], 7, 0);
          on(i, currentColor[i]);
        } else {
          // swap the on state of the led
          bitWrite(currentColor[i], 7, 1);
          off(i);
        }
      }
    }
  }

#if BMC_MAX_PIXELS > 0
  uint32_t getPixelsState(){
    return pixelStates;
  }
  void pulse(uint8_t t_index, uint8_t t_color=255){
    if(t_index>=BMC_MAX_PIXELS){
      return;
    }
    write(t_index, t_color);
    pulseTimer[t_index].start(BMC_LED_PULSE_TIMEOUT);
  }
  // this would be very complicated to do for RGB pixels so it's not used
  void setBrightness(uint8_t t_index, uint8_t t_brightness=127, uint8_t t_color=255){
    if(t_color==BMC_COLOR_RAINBOW || t_index >= BMC_MAX_PIXELS){
      return;
    }
    // we start by writting the new color to that pixel
    // if brightness is 0 we set the pixel to OFF
    write(t_index, (t_brightness==0) ? 0 :t_color);
    // get the maximum brightness we'll allow
    // since BMC_COLOR_RGB_WHITE has all 24 bits set to the highest brightness
    // based on BMC_PIXEL_MAX_BRIGHTNESS we take the first 8-bits
    uint8_t max = (BMC_COLOR_RGB_WHITE & 0xFF);
    t_brightness = map(t_brightness, 0, 127, 0, max);
    // if this new brightness has changed then we want to continue
    if(currentBrightness[t_index] != t_brightness){
      // store the new brightness
      currentBrightness[t_index] = t_brightness;
      // convert the BMC color to it's RGB counterpart
      uint32_t cc = BMCPixelColors::getRgbColor(currentColor[t_index]);
      // now map each color within cc to the new brightness
      cc = map((cc & 0xFF), 0, max, 0, t_brightness) |
          (map(((cc>>8) & 0xFF), 0, max, 0, t_brightness)<<8) |
          (map(((cc>>16) & 0xFF), 0, max, 0, t_brightness)<<16);
      // set the new pixel brightness
      setPixelValue(t_index, cc);
      // immediately show() the new color
      show();
    }
  }
#endif

#if BMC_MAX_RGB_PIXELS > 0
  uint32_t getRgbPixelsStateR(){
    return rgbPixelStates[0];
  }
  uint32_t getRgbPixelsStateG(){
    return rgbPixelStates[1];
  }
  uint32_t getRgbPixelsStateB(){
    return rgbPixelStates[2];
  }
  void setStateRgbRed(uint8_t n, bool t_state){
    // 0=red, 1=green, 2=blue
    setStateRgb(n, 0, t_state);
  }
  void setStateRgbGreen(uint8_t n, bool t_state){
    // 0=red, 1=green, 2=blue
    setStateRgb(n, 1, t_state);
  }
  void setStateRgbBlue(uint8_t n, bool t_state){
    // 0=red, 1=green, 2=blue
    setStateRgb(n, 2, t_state);
  }
  void setStateRgb(uint8_t n, uint8_t color, bool t_state){
    n += BMC_MAX_PIXELS;
    // get the current color of the led
    // but don't account for for the MS bit
    uint8_t cColor = currentColor[n] & 0x07;
    bitWrite(cColor, color, t_state);
    write(n, cColor);
  }
  void pulseRgb(uint8_t t_index, uint8_t t_color){
    if(t_index>=BMC_MAX_RGB_PIXELS){
      return;
    }
    // since RGB pixels can have each of the 3 colors controller separately
    // we pulse them independently of the others, rgbPulseReset will hold the
    // bit of the color being pulsed and we pulse more than 1 of the colors
    // then we have to update the bits that are being pulsed to add any additional
    // bits so they can be turned off by update(), we do this by ORing the values
    // rgbPulseReset is indexed by BMC_MAX_RGB_PIXELS, so from 0 to BMC_MAX_RGB_PIXELS
    bitWrite(rgbPulseReset[t_index], t_color, 1);
    t_index += BMC_MAX_PIXELS;
    t_color = bitWrite(currentColor[t_index],t_color,1);
    write(t_index, t_color);
    pulseTimer[t_index].start(BMC_LED_PULSE_TIMEOUT);
  }
  // @t_input   = the value we're comparing to
  // @t_default = the value that will turn the led on either way
  // @compare   = the flags with the operator and operation value
  bool compare(uint16_t t_input, uint16_t t_default, uint8_t compare){
    // if t_input is the same as t_default return true
    if(t_input == t_default){
      return true;
    }
    // compare's bits 6 and 7 are the operator
    // compare's bits 0 to 5 are teh operator value
    switch(((compare >> 6) & 0x03)){
      case 0: // skip
        return false;
      case 1: // ==
        return (t_input == (compare & 0x3F));
      case 2: // ORing
        return (t_input  | (compare & 0x3F));
      case 3: // ANDin
        return (t_input  & (compare & 0x3F));
    }
    return false;
  }
#endif

  void setState(uint8_t n, uint8_t t_color=255){
    write(n, t_color);
  }
  void on(uint8_t n, uint8_t t_color=255){
    write(n, (t_color==0 ? 255 : t_color));
  }
  void off(uint8_t n, uint8_t t_color=255){
    write(n, 0);
  }
  void toggle(uint8_t n, uint8_t t_color=255){
    write(n, (getState(n)>0) ? 0 : t_color);
  }
  uint8_t getState(uint8_t n){
    return (n<BMC_TOTAL_PIXELS ? currentColor[n] : 0);
  }
#if BMC_MAX_RGB_PIXELS > 0
  uint8_t getRgbState(uint8_t n){
    return (((BMC_MAX_PIXELS+n)<BMC_MAX_RGB_PIXELS) ? currentColor[BMC_MAX_PIXELS+n] : 0);
  }
#endif

private:
  //  3 bytes per LED
  uint8_t drawingMemory[BMC_TOTAL_PIXELS*3];
  // 12 bytes per LED
  // DMAMEM can NOT be used inside a class?????
  byte displayMemory[BMC_TOTAL_PIXELS*12];
  // pixels
  WS2812Serial pixels;
  // this array will store the color of each pixel
  // the most significant bit however specifies if the pixel is on (1) or off(0)
  uint8_t currentColor[BMC_TOTAL_PIXELS];
#if BMC_MAX_PIXELS > 0
  // brightness, only used for pot controls as of right now
  uint8_t currentBrightness[BMC_TOTAL_PIXELS];
#endif
  // store the dim value for each pixel, this value is based on color that the
  // pixel event has each byte stores 2 pixel's dim values, if your pixel has
  // an event and it's set to green then green becomes the DIM color if you pixel
  // doesn't have an event then dim color will be the last color that was used
  // at startup there's no last color so then the initial color is the Pixels
  // default colors assigned by the Config File.
  // This is updated when pixels are reassigned
  uint8_t dimColors[BMC_TOTAL_DIM_COLORS];
  // pulse timers, one per led
  BMCTimer pulseTimer[BMC_TOTAL_PIXELS];

#if BMC_MAX_RGB_PIXELS > 0
  uint8_t rgbPulseReset[BMC_MAX_RGB_PIXELS];
#endif

  // on/off state of each pixel
#if BMC_MAX_PIXELS > 16
  uint32_t pixelStates = 0;
#elif BMC_MAX_PIXELS > 8
  uint16_t pixelStates = 0;
#elif BMC_MAX_PIXELS > 0
  uint8_t pixelStates = 0;
#endif

  // on/off state of each pixel
#if BMC_MAX_RGB_PIXELS > 16
  uint32_t rgbPixelStates[3] = {0,0,0};
#elif BMC_MAX_RGB_PIXELS > 8
  uint16_t rgbPixelStates[3] = {0,0,0};
#elif BMC_MAX_RGB_PIXELS > 0
  uint8_t rgbPixelStates[3] = {0,0,0};
#endif

#if defined(BMC_PIXELS_REARRANGE)
  const uint8_t pixelsSort[BMC_TOTAL_PIXELS] = BMC_PIXELS_REARRANGE;
#endif

  void setPixelValue(uint8_t n, uint32_t color){
    if(n>=BMC_TOTAL_PIXELS){
      return;
    }
#if defined(BMC_PIXELS_REARRANGE)
    pixels.setPixel(pixelsSort[n], color);
#else
    pixels.setPixel(n, color);
#endif
  }

  // flags
  BMCFlags <uint16_t> flags;

#if BMC_MAX_PIXELS > 0
  // RAINBOW
  // in Rainbow mode the led uses a different color anytime you set the color
  // the rainbowCurrentColor variable is changed only when pixels.show() is called
  uint8_t rainbowCurrentColor = BMC_COLOR_RED;
  // time the rainbow to be triggered after 10ms
  elapsedMillis rainbowTimeout;

  void updateRainbowColor(uint16_t speed=500){
    if(rainbowTimeout >= speed){
      if(!flags.read(BMC_PIXELS_FLAG_RAINBOW_CHANGED)){
        rainbowCurrentColor++;
        // BMC_COLOR_RAINBOW is the highest index number
        // once we reach it we go back to the start of the colors
        if(rainbowCurrentColor >= BMC_COLOR_RAINBOW){
          rainbowCurrentColor = BMC_COLOR_RED;
        } else if(rainbowCurrentColor == BMC_COLOR_WHITE){
          // if it's white we skip it, that way we avoid drawing all the
          // max led current
          rainbowCurrentColor++;
        }
        rainbowTimeout = 0;
      }
      flags.on(BMC_PIXELS_FLAG_RAINBOW_CHANGED);
    }
  }
#endif
  uint8_t getDimColor(uint8_t n){
    uint8_t offset = (uint8_t) ((n>0) ? (n/2) : 0);
    if(offset > BMC_TOTAL_DIM_COLORS){
      return BMC_COLOR_RED;
    }
    if(BMC_IS_EVEN(n)){
      return (dimColors[offset] & 0x0F);
    }
    return ((dimColors[offset] >> 4) & 0x0F);
  }
  void turnPixelOff(uint8_t n){
    uint32_t offValue = 0;
    if(flags.read(BMC_PIXELS_FLAG_USE_DIM)){
      offValue = BMCPixelColors::getDimmedColor(getDimColor(n));
    }
    setPixelValue(n, offValue);
  }
  void show(){
    flags.on(BMC_PIXELS_FLAG_SHOW);
  }
  void setValuesChanged(uint8_t n, bool t_state){
    if(n >= BMC_TOTAL_PIXELS){
      return;
    }
#if BMC_MAX_PIXELS > 0
    if(n < BMC_MAX_PIXELS){
      bitWrite(pixelStates, n, t_state);
      return;
    }
#endif

#if BMC_MAX_RGB_PIXELS > 0
    if(n < BMC_TOTAL_PIXELS && n >= BMC_MAX_PIXELS){
      uint8_t color = currentColor[n]&0x0F;
      n -= BMC_MAX_PIXELS;
      bitWrite(rgbPixelStates[0], n, (bitRead(color, 0)));
      bitWrite(rgbPixelStates[1], n, (bitRead(color, 1)));
      bitWrite(rgbPixelStates[2], n, (bitRead(color, 2)));
    }
#endif
  }

  void write(uint8_t t_index, uint8_t t_color=255){
    if(t_index >= BMC_TOTAL_PIXELS){
      return;
    }
    // hold the current color assigned to the pixel
    uint8_t cColor = getState(t_index);

    if(t_color==255){
      // if target color is 255 we use the last assigned color
      t_color = cColor & 0x0F;

    } else if(t_color==127){
      // if target color is 127 we use default color
      t_color = getDefaultColor(t_index);

    } else if(t_color==BMC_COLOR_RAINBOW){
#if BMC_MAX_PIXELS > 0
       // if the target color BMC_COLOR_RAINBOW then we will go thru all colors
       t_color = rainbowCurrentColor;
       setDimColor(t_index, t_color);
#endif
    }

    // if t_color is 0, we are turning the pixel off
    if(t_color == 0){
      // if it's not already off we set it to off/black
      // we do all these checks because we have to change the states of all
      // pixels everytime one changes, so if there's no need to do that, we don't
      if(bitRead(cColor, 7)){

        turnPixelOff(t_index);
        // set bit 7 to 0
        // thats the bit that tells you if the led is on(1) or off(0)
        // we don't change the actual color just the on/off state
#if BMC_MAX_PIXELS > 0 && BMC_MAX_RGB_PIXELS > 0
        if(t_index>=BMC_MAX_PIXELS){
          currentColor[t_index] = 0;
        } else {
          bitWrite(currentColor[t_index], 7, 0);
        }
#elif BMC_MAX_PIXELS > 0
        bitWrite(currentColor[t_index], 7, 0);
#else
        currentColor[t_index] = 0;
#endif
        // we specify that a value has changed for BMC to provide feedback
        // to the editor
        setValuesChanged(t_index, false);
        // show just sets a flag so on the next loop() the pixel.show()
        // method is called, this way we don't do it every time a pixel
        // color/state has changed
        show();
      }
    } else {
      // we are setting a color aka turning the pixel ON
      // we are going to check if the pixel if OFF or if the color is different
      // than the current color, in either case we want to update the pixel
      if(!bitRead(cColor,7) || (cColor&0x7F)!=t_color){
        // convert the 4 bit color to the full rgb value
        setPixelValue(t_index, BMCPixelColors::getRgbColor(t_color));
        // set bit 7 to 0
        // thats the bit that tells you if the led is on(1) or off(0)

        bitWrite(t_color, 7, 1);
        currentColor[t_index] = t_color;

        // we specify that a value has changed for BMC to provide feedback
        // to the editor
        setValuesChanged(t_index, true);
        // show just sets a flag so on the next loop() the pixel.show()
        // method is called, this way we don't do it every time a pixel
        // color/state has changed
        show();
      }
    }
  }
  void reset(bool showPixels=true){
    flags.reset(1 << BMC_PIXELS_FLAG_USE_DIM);

#if BMC_MAX_PIXELS > 0
    pixelStates = 0;
    // the default color to each LED
    for(uint8_t i=0; i<BMC_MAX_PIXELS; i++){
      currentColor[i] = BMCBuildData::getPixelDefaultColor(i);
      pulseTimer[i].stop();
      currentBrightness[i] = 0;
    }
#endif

#if BMC_MAX_RGB_PIXELS > 0
    rgbPixelStates[0] = 0; // red
    rgbPixelStates[1] = 0; // green
    rgbPixelStates[2] = 0; // blue
    // the default color to each LED
    for(uint8_t i=0, n=BMC_MAX_PIXELS; i<BMC_MAX_RGB_PIXELS; i++, n++){
      pulseTimer[n].stop();
      pulseTimer[n].stop();
      currentColor[n] = 0;
    }
    memset(rgbPulseReset, 0, BMC_MAX_RGB_PIXELS);
#endif

    if(showPixels){
      show();
    }
  }
  uint8_t getDefaultColor(uint8_t n){
#if BMC_MAX_PIXELS > 0 && BMC_MAX_RGB_PIXELS > 0
      if(n<BMC_MAX_PIXELS){
        return BMCBuildData::getPixelDefaultColor(n);
      } else if(n<BMC_MAX_PIXELS){
        return BMCBuildData::getRgbPixelDefaultColor(n-BMC_MAX_PIXELS);
      }
      return BMC_COLOR_RED;

#elif BMC_MAX_PIXELS > 0
      return  BMCBuildData::getPixelDefaultColor(n);
#else
      return BMCBuildData::getRgbPixelDefaultColor(n);
#endif

  }
};

#endif

#endif
