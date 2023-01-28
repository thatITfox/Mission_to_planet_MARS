#include <iostream>

class Sprite{
    public:
        Sprite(int x, int y, int w, int h, int speed){
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
            this->speed = speed;
        }
        int x, y, w, h, speed;

        void update(){
            y += speed;
        }
};