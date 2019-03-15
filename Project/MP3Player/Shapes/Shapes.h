#ifndef SHAPES_LIBRARY
#define SHAPES_LIBRARY

#include <Adafruit_GFX.h>

#define BACKCOLOR 0xFFFF // White
#define FORECOLOR 0xFF0x7BEFFF // dark-gray

class Rectangle : public Adafruit_GFX {
    public:
        Rectangle(int16_t x = 2, int16_t y = 2, int16_t w = 5, int16_t h = 5); // constructor
        void drawPixel(int16_t x, int16_t y, uint16_t color) override; // base class virtual method
        
    private:
        int16_t x, y; // top left corner
};


class Circle : public Adafruit_GFX {
    public:
        Circle(int16_t x = 2, int16_t y = 2, int16_t radius = 5); // constructor
        void drawPixel(int16_t x, int16_t y, uint16_t color) override; // base class virtual method
        
    private:
        int16_t x_coordinate, y_coordinate, radius; // top left corner
};



#endif //SHAPES_LIBRARY
