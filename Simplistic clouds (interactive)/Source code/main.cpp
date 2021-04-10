#include <wallpaper.h>

using namespace std;

int max_off_x;
int max_off_y;

int x_off = 0;
int y_off = 0;

int x_old = 0;
int y_old = 0;

int x_new = 0;
int y_new = 0;

Image im_b;

POINT p;

class circle{
public:
    float x, y;

    int radius;
    float speed = 2;

    float dist;

    bool shouldBeDestroyed = false;

    circle(){
        radius = (int) (20 * (width / 1920.0));

        x = - 200 - rand()%200;
        y = - 200 + rand()%(height + 400);

        dist = (rand()%2000 / 1000.0) + 1;

        speed /= dist;
        radius /= dist;
    }

    void update(){
        x += speed;

        if (x > (int)(1.3 * width) + radius)
            shouldBeDestroyed = true;
    }

    void draw(){
        glColor4f(1.0, 1.0, 1.0, 1.0 / (dist / 1.25));
        //glRectd(x + x_off - radius, y + y_off - radius, x + x_off + radius,  y + y_off + radius);
        im_b.draw(x + x_off - radius * 3, y + y_off - radius * 3, radius * 3, radius * 3, CORNER);
    }
};

void circle_sort(circle* arr, int len){
    for (int i = 0; i < len; i++){
        for (int j = 0; j < len - i - 1; j++){
            if (arr[j].radius > arr[j + 1].radius){
                circle tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

const int num = 200;
circle squares[num];

void setup(){
    max_off_x = (200 / 1920.0) * width;
    max_off_y = (100 / 1080.0) * height;

    srand(time(0));

    GetCursorPos(&p);
    x_old = p.x;
    y_old = p.y;

    x_new = p.x;
    y_new = p.y;

    im_b.read("cloud.bmp");

    //glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    //glEnable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);

    for (int i = 0; i < num; i++){
        squares[i] = circle();
        squares[i].x = rand()%((int) (1.3 * width));
        squares[i].y = rand()%((int) (1.2 * height));
    }

    circle_sort(&squares[0], num);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearColor(4 / 255.0, 30 / 255.0, 55 / 255.0, 0.0);
}

void draw(){
    glClear(GL_COLOR_BUFFER_BIT);

    //im.draw(0, 0, width, height, CORNER);

    GetCursorPos(&p);

    if (p.x < 0)
        p.x = 0;
    if (p.x > width - 1)
        p.x = width - 1;
    if (p.y < 0)
        p.y = 0;
    if (p.y > height - 1)
        p.y = height - 1;

    x_new = p.x;
    y_new = p.y;

    x_old += (x_new - x_old) * ((abs(x_new - x_old) / float(width)) / 2) * 0.35;
    y_old += (y_new - y_old) * ((abs(y_new - y_old) / float(height)) / 2) * 0.35;

    x_old = abs(x_old % width);
    y_old = abs(y_old % height);

    x_off = ((x_old - float(width / 2)) / float(width / 2)) * max_off_x - max_off_x;
    y_off = ((y_old - float(height / 2)) / float(height / 2)) * max_off_y - max_off_y;

    for (int i = 0; i < num; i++){
        squares[i].update();
        squares[i].draw();

        if (squares[i].shouldBeDestroyed){
            squares[i] = circle();
        }
    }

    circle_sort(&squares[0], num);
}
