#include <wallpaper.h>

using namespace std;

const float dot_radius = 4;
int dot_circle_length;
float dot_circle_offsets_x[100];
float dot_circle_offsets_y[100];

const int dot_count = 120;

const int connection_distance = 40000; //32400; //180

const int float_dist = 150;

float p_mouse_x;
float p_mouse_y;

float x_off;
float y_off;

float dist_depth = 35.0;

float rot_vec = 120;

float rot_x = 0;
float rot_y = 0;

class Dot {
public:
    float x;
    float y;
    float z;

    float speed_x;
    float speed_y;

    int dst;

    //Hard optimization
    float opt1, opt2, opt3, opt4, opt5, opt6;

    Dot* arr;

    Dot(){}

    Dot (float x, float y){
        this -> x = x;
        this -> y = y;

        this -> z = rand()%5 + 1;

        speed_x = rand()%3000 / 1500.0;
        speed_y = rand()%3000 / 1500.0;

        speed_x -= 1.0;
        speed_y -= 1.0;

        speed_x *= 0.5;
        speed_y *= 0.5;

        //Hard optimization
        opt1 = -dot_radius - 200;
        opt2 = width + dot_radius + 200;
        opt3 = y < -dot_radius - 200;
        opt4 = height + dot_radius + 200;

        opt5 = width + 2 * dot_radius + 400;
        opt6 = height + 2 * dot_radius + 400;

        //3D
        speed_x *= 1.0 - (1.0 - (z / 5.0)) * 0.7;
        speed_y *= 1.0 - (1.0 - (z / 5.0)) * 0.7;
    }

    void update(){
        x += speed_x;
        y += speed_y;

        x+=rot_x;
        y+=rot_y;

        if (x < opt1){
            x += opt5;
        }
        if (x >= opt2){
            x -= opt5;
        }

        if (y < opt3){
            y += opt6;
        }
        if (y >= opt4){
            y -= opt6;
        }
    }

    void show(){
        //Lines
        glLineWidth(1.0);
        for (int i = 0; i < dot_count; i++){
            dst = (arr[i].x - x)*(arr[i].x - x) + (arr[i].y - y)*(arr[i].y - y);
            if (dst <= connection_distance){
                glColor4f(0.36, 0.741, 0.666, 1.0 - (dst / (float)(connection_distance)));
                glBegin(GL_LINES);
                //glVertex3d(x, y, -z);
                //glVertex3d(arr[i].x, arr[i].y, -arr[i].z);
                glVertex2d(x + x_off * z * dist_depth, y + y_off * z * dist_depth);
                glVertex2d(arr[i].x + x_off * arr[i].z * dist_depth, arr[i].y + y_off * arr[i].z * dist_depth);
                glEnd();
            }
        }

        glColor4f(0.36, 0.741, 0.666, 1.0);

        //Dots
        glPushMatrix();
        glTranslatef(x + x_off * z * dist_depth, y + y_off * z * dist_depth, 0);

        glBegin(GL_POLYGON);
        for (int i = 0; i < dot_circle_length; i++){
            //glVertex3d(dot_circle_offsets_x[i], dot_circle_offsets_y[i], -z);
            glVertex2d(dot_circle_offsets_x[i], dot_circle_offsets_y[i]);
        }
        //glVertex3d(dot_circle_offsets_x[0], dot_circle_offsets_y[0], -z);
        glVertex2d(dot_circle_offsets_x[0], dot_circle_offsets_y[0]);
        glEnd();

        glPopMatrix();

        glColor4f(1.0, 1.0, 1.0, 1.0);
    }
};

Dot dots[dot_count];

POINT p;

void setup(){
    fps = 60;

    srand(0);

    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    dot_circle_length = ceil(2 * dot_radius * pi);

    for (int i = 0; i < dot_count; i++){
        dots[i] = Dot(rand()%(width + 400) - 200, rand()%(height + 400) - 200);
        dots[i].arr = &dots[0];
    }

    //Dot offsets
    for (int i = 0; i < dot_circle_length; i++){
        float rot = i / float(dot_circle_length) * 2 * pi;

        dot_circle_offsets_x[i] = cos(rot) * dot_radius;
        dot_circle_offsets_y[i] = sin(rot) * dot_radius;
    }

    glClearColor(15 / 255.0, 16 / 255.0, 30 / 255.0, 1.0);

    fps = 60;

    GetCursorPos(&p);
    p_mouse_x = p.x;
    p_mouse_y = p.y;
}

void draw(){
    glClear(GL_COLOR_BUFFER_BIT);

    rot_vec += (rand()%100 / 1000.0) - 0.05;
    rot_x = cos((rot_vec / 360.0) * 2 * 3.1415) * 0.1;
    rot_y = sin((rot_vec / 360.0) * 2 * 3.1415) * 0.1;

    fps = 60;

    GetCursorPos(&p);

    x_off = ((float) (p.x) - (float) (width) / 2.0) / (float) (height);
    y_off = ((float) (p.y) - (float) (height) / 2.0)  / (float) (height);

    p_mouse_x += ((float) (p.x) - p_mouse_x) * 0.1f;
    p_mouse_y += ((float) (p.y) - p_mouse_y) * 0.1f;

    glTranslatef(((p_mouse_x - width / 2.0) / (width / 2.0)) * float_dist, ((p_mouse_y - height / 2.0) / (height / 2.0)) * float_dist, 0);

    for (int i = 0; i < dot_count; i++){
        dots[i].update();
        dots[i].show();
    }

    glTranslatef(-((p_mouse_x - width / 2.0) / (width / 2.0)) * float_dist, (-(p_mouse_y - height / 2.0) / (height / 2.0)) * float_dist, 0);
}

void exiting(){

}
