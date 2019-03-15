#include <Shapes.h>

//Rectangle::
Rectangle::Rectangle(int16_t x, int16_t y, int16_t w, int16_t h)  {
//    Adafruit_GFX(w, y);
    this->x = x; 
    this->y = y;
    _width = w;
    _height = h;
//    this->name = name; // base class member

}

void Rectangle::drawPixel(int16_t x, int16_t y, uint16_t color) {
    cursor_x = x; 
    cursor_y = y;
}

//void Rectangle::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
//
//}

Circle::Circle(int16_t x, int16_t y, int16_t r) {
    x_coordinate = x; 
    y_coordinate = y;
    radius = r;
}

void Circle::drawPixel(int16_t x, int16_t y, uint16_t color) {
    cursor_x = x; 
    cursor_y = y;
}
