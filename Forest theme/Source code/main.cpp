#include <wallpaper.h>
#include <cmath>

using namespace std;

const int c = 4;

Image bk;
Image dot;

class Circle{
public:
    float x;
    float y;

    float speed_y = -1;
    float speed_x = 0;

    bool shouldBeDestroyed = false;

    Circle(){
        x = rand()%width - 200 + 100;
        y = rand()%100 + 1130;
    }

    void update(){
        speed_x += ((rand()%1000) - 500) / 20000.0;

        if (abs(speed_x) > 0.4)
            speed_x -= speed_x * 0.01;

        y += speed_y;
        x += speed_x;

        if (y < -50){
            shouldBeDestroyed = true;
        }
    }

    void draw(){
        dot.draw((int) x, (int) y, dot.w, dot.h, CENTER);
    }
};

const int c_num = 50;
Circle circles[c_num];

void setup(){
    bk.read("background.bmp");
    dot.read("dot.bmp");

    for (int i = 0; i < c_num; i++){
        circles[i].y = rand()%height;
    }
}

void draw(){
    bk.draw(0, 0, width, height, CORNER);

    for (int i = 0; i < c_num; i++){
        circles[i].update();
        circles[i].draw();

        if (circles[i].shouldBeDestroyed)
            circles[i] = Circle();
    }
}
