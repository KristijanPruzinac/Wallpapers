#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <chrono>
#include <windows.h>
#include <gl.h>

#include <ctime>
#include <stdlib.h>

#include <cmath>

#define CORNER 0
#define CORNERS 2

#define LEFT 0
#define CENTER 1
#define RIGHT 2
#define TOP 0
#define BOTTOM 2

#define pi 3.1415

extern int width;
extern int height;

extern int fps;

extern HWND workerw;
extern HDC window_hdc;
extern HGLRC window_hrc;

void exit();
void setup();
void draw();
void t_start();
void t_stop();
void t_print_millis();
void t_print_micros();
long long int t_millis();
long long int t_micros();

class Image{
private:
public:
    unsigned int w;
    unsigned int h;
    unsigned short bpp;
    unsigned char arr[3840*2160*4];
    GLuint texture;
    bool first_read;

    Image();
    void read(std::string);
    void write(std::string);
    void initTexture();
    void draw(int, int, int, int, int);
};

BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);

void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
