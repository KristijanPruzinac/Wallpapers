#include <wallpaper.h>

using namespace std;

//Variables
const int num_parts = 8;

//Point
struct Point{
    int x;
    int y;
};

//Radians
double rad(double deg){
    return deg * pi / 180.0;
}

double deg(double rad){
    return (rad * 180.0) / pi;
}

//Angle
double atan(Point current, Point target){
	double atan_angle = atan2(target.y - current.y, target.x - current.x) + pi;

	if (atan_angle < 0){
        atan_angle = 0;
	}
	if (atan_angle > 2 * pi){
        atan_angle = 2 * pi;
	}

	return atan_angle;
}

//Part lengths
int part_lengths[3] = {62, 36, 46};

Image parts_images[4];
Image parts_images_inverted[4];

//Part class
class Part {
public:
    Point a;

    int length;
    double angle;

    Point b;

    Point previous;

    int part_num;

    Part(){}

    Part(Point a, int length, double angle, int part_num) {
        this -> a = a;
        this -> length = length;
        this -> angle = angle;
        this -> part_num = part_num;

        int x2 = a.x + cos(angle) * length;
        int y2 = a.y + sin(angle) * length;

        this -> b = {x2, y2};
    }

    void update(){
        angle = atan(b, previous);

        a = previous;

        int x2 = a.x + cos(angle) * length;
        int y2 = a.y + sin(angle) * length;

        b = {x2, y2};
    }

    void show(){
        glColor3f(1.0, 1.0, 1.0);
        glPushMatrix();
        glTranslated(a.x, a.y, 0);
        glRotatef(deg(angle) - 180, 0, 0, 1);
        if (deg(angle) > 270 || deg(angle) < 90){
            parts_images_inverted[part_num].draw(0, 0, parts_images[part_num].w, parts_images[part_num].h, CENTER);
        }
        else {
            parts_images[part_num].draw(0, 0, parts_images[part_num].w, parts_images[part_num].h, CENTER);
        }
        glPopMatrix();
    }
};

Part parts[num_parts];

Image bk;

POINT p_in;
Point p;

void setup(){
    bk.read("background.bmp", 1);
    fps = 60;

    //Images
    parts_images[0].read("Parts/head.bmp");
    parts_images[1].read("Parts/body1.bmp");
    parts_images[2].read("Parts/body2.bmp");
    parts_images[3].read("Parts/tail.bmp");

    parts_images_inverted[0].read("Parts/head_inverted.bmp");
    parts_images_inverted[1].read("Parts/body1_inverted.bmp");
    parts_images_inverted[2].read("Parts/body2_inverted.bmp");
    parts_images_inverted[3].read("Parts/tail_inverted.bmp");

    //Cursor pos
    GetCursorPos(&p_in);
    p = Point{p_in.x, p_in.y};

    Point last = p;
    for (int i = 0; i < num_parts; i++){

        //Length of part
        int l = 0;
        int p_num = 0;

        if (i == 0){
            l = part_lengths[0] / 2;
            p_num = 0;
        }
        else if (i == num_parts - 1){
            l = part_lengths[2] / 2;
            p_num = 3;
        }
        else {
            l = part_lengths[1] / 2;
            p_num = (i - 1) % 2 + 1;
        }

        //Part initialization
        parts[i] = Part(last, l, 0.0, p_num);

        last = parts[i].b;
    }
}

void draw(){
    glColor3f(1.0, 1.0, 1.0);
    bk.draw(0, 0, width, height);

    //Cursor pos
    GetCursorPos(&p_in);
    p = Point{p_in.x, p_in.y};

    //Update and show
    parts[0].previous = p;

    bool change = false;

    if ((p.x != parts[0].a.x) || (p.y != parts[0].a.y)){
        change = true;
    }

    for (int i = 0; i < num_parts; i++){

        if (i != 0){
            parts[i].previous = parts[i - 1].b;
        }

        if (change){
            parts[i].update();
        }
        parts[i].show();
    }
}

void exiting(){

}
