#include <wallpaper.h>

using namespace std;

Image bk_im;
Image dot_im;
Image star_im;

int particle_fade_time = 2;
float particle_fade_max = 0.9;
float particle_speed = 0.5;

const int particle_count = 20;
const int star_count = 6;

int star_visible_time = 4;

float scaling = 1;

class particle{
public:
    float x;
    float y;

    float x_speed;

    int f_count;
    int d_count;

    bool destroy;

    particle(){
        x = rand()%((int)(width - (width / 5.0))) + (width / 10.0);
        y = height - height / 20;

        f_count = 0;
        d_count = 0;

        x_speed = (rand()%20 - 10) / 20.0;

        destroy = false;
    }

    void update(){
        y -= particle_speed * (0.8 + rand()%10 / 50.0);

        x_speed += (rand()%20 - 10) / 100.0;

        if (abs(x_speed) > 0.7)
            x_speed *= 0.8;

        x += x_speed;

        if (y < height - height / 4)
            d_count++;

        if (d_count > fps * particle_fade_time)
            destroy = true;

        if (f_count < fps * particle_fade_time)
            f_count++;
    }

    void draw(){
        if (d_count != 0)
            glColor4f(1.0, 1.0, 1.0, particle_fade_max - particle_fade_max * (d_count / float(fps * particle_fade_time)));
        else if (f_count < fps * particle_fade_time)
            glColor4f(1.0, 1.0, 1.0, particle_fade_max * (f_count / float(fps * particle_fade_time)));
        else
            glColor4f(1.0, 1.0, 1.0, particle_fade_max);

        dot_im.draw((int) x, (int) y, dot_im.w * scaling, dot_im.h * scaling, CENTER);
    }
};

class star{
public:
    int x;
    int y;

    int f_count;

    bool visible;
    bool fade_out;

    bool destroy;

    star(){
        x = rand()%((int)(width - (width / 3)) / 5) * 5 + (width / 6);
        y = rand()%(height / 4) + (height / 12);

        f_count = 0;
        visible = false;
        fade_out = false;

        destroy = false;
    }

    void update(){
        if (!visible && !fade_out && f_count > fps * particle_fade_time){
            visible = true;
            f_count = 0;
        }
        else if (visible && f_count > fps * star_visible_time){
            visible = false;
            fade_out = true;
            f_count = 0;
        }
        else if (fade_out && f_count > fps * particle_fade_time){
            destroy = true;
        }

        f_count++;
    }

    void draw(){
        if (!visible && !fade_out)
            glColor4f(1.0, 1.0, 1.0, 1.0 * (f_count / float(fps * particle_fade_time)));
        else if (visible){
            glColor4f(1.0, 1.0, 1.0, 1.0);
        }
        else if (fade_out) {
            glColor4f(1.0, 1.0, 1.0, 1.0 - 1.0 * (f_count / float(fps * particle_fade_time)));
        }

        star_im.draw(x, y, star_im.w * scaling, star_im.h * scaling, CENTER);
    }
};

particle particles[particle_count];
star stars[star_count];

void setup(){
    if (width <= 1920){
        bk_im.read("backgroundHD.bmp");
        dot_im.read("dotHD.bmp");
        star_im.read("starHD.bmp");

    }
    else {
        bk_im.read("background2K.bmp");
        dot_im.read("dot2K.bmp");
        star_im.read("star2K.bmp");
    }

    if (width != 1920 && width != 2880)
        scaling = width / 1920.0;

    particle_speed *= width / 1920.0;

    for (int i = 0; i < particle_count; i++)
        particles[i].y = height - rand()%(height / 6);

    for (int i = 0; i < star_count; i++){
        stars[i].f_count = rand()%(fps * particle_fade_time);

        int tmp = rand()%3;
        if (tmp == 1)
            stars[i].visible = true;
        else if (tmp == 2)
            stars[i].fade_out = true;
    }
}

void draw(){
    glColor4f(1.0, 1.0, 1.0, 1.0);
    bk_im.draw(0, 0, width, height, CORNER);

    for (int i = 0; i < particle_count; i++){
        if (particles[i].destroy)
            particles[i] = particle();

        particles[i].update();
        particles[i].draw();
    }

    for (int i = 0; i < star_count; i++){
        if (stars[i].destroy){
            stars[i] = star();
        }

        stars[i].update();
        stars[i].draw();
    }
}
