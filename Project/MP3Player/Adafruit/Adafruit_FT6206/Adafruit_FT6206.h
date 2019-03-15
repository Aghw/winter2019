/*************************************************** 
  This is a library for the Adafruit Capacitive Touch Screens

  ----> http://www.adafruit.com/products/1947
 
  Check out the links above for our tutorials and wiring diagrams
  This chipset uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef ADAFRUIT_FT6206_LIBRARY
#define ADAFRUIT_FT6206_LIBRARY

#include <stdint.h>
#include <pjdf.h>

#ifndef boolean
    #define boolean bool
#endif


#define FT6206_ADDR           0x38
#define FT6206_G_FT5201ID     0xA8
#define FT6206_REG_NUMTOUCHES 0x02

#define FT6206_NUM_X             0x33
#define FT6206_NUM_Y             0x34

#define FT6206_REG_MODE 0x00
#define FT6206_REG_CALIBRATE 0x02
#define FT6206_REG_WORKMODE 0x00
#define FT6206_REG_FACTORYMODE 0x40
#define FT6206_REG_THRESHHOLD 0x80
#define FT6206_REG_POINTRATE 0x88
#define FT6206_REG_FIRMVERS 0xA6
#define FT6206_REG_CHIPID 0xA3
#define FT6206_REG_VENDID 0xA8


// Graphics
#define RECT_X0_COORDINATES 29
#define RECT_Y0_COORDINATES 103
#define CIRC_X0_COORDINATES 40
#define CIRC_Y0_COORDINATES 115
#define BTN_RADIUS          35
#define CIRC_X1_COORDINATES 120
#define CIRC_Y1_COORDINATES 195
#define CIRC_X2_COORDINATES 200
#define CIRC_Y2_COORDINATES 275
#define CIRC_Y3_COORDINATES 300
#define BUT_RADIUS          30
#define VOL_ECHO_X          210
#define VOL_ECHO_Y          178
#define VOL_SHIFT           5

// calibrated for Adafruit 2.8" ctp screen
#define FT6206_DEFAULT_THRESSHOLD 128

class TS_Point {
 public:
  TS_Point(void);
  TS_Point(int16_t x, int16_t y, int16_t z);
  
  bool operator==(TS_Point);
  bool operator!=(TS_Point);

  int16_t x, y, z;
};

class Adafruit_FT6206 {
 public:

  Adafruit_FT6206(void);
  boolean begin(uint8_t thresh = FT6206_DEFAULT_THRESSHOLD);  

  void writeRegister8(uint8_t reg, uint8_t val);
  uint8_t readRegister8(uint8_t reg);

  void readData(uint16_t *x, uint16_t *y);
  void autoCalibrate(void); 

  boolean touched(void);
  TS_Point getPoint(void);
  void setPjdfHandle(HANDLE);
  
 private:
  HANDLE hI2C;
//  uint8_t ic2Buffer[FT6206_DEFAULT_THRESSHOLD];
//  uint8_t iIC2Buffer; /* current IC2 buffer empty */
  uint8_t touches; // a byte quantity which contains data from touch register on the touch panel
  uint16_t touchX[2], touchY[2], touchID[2];

};

#endif //ADAFRUIT_FT6206_LIBRARY
