#include <iostream>
#include <math.h>

class Player{
    public:
        Player(int x, int y, int w, int h){
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }
        int x, y, w, h, speed = 0, bullets = 5;

        void update(){
            x += speed;
        }
        
};