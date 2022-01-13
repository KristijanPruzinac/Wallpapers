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

#define pi 3.1415926

//2D to 1D array access
extern int cvt2dto1d_cols;
extern int cvt2dto1d_colors;
void set2dto1d(int, int);
#define ac(ROW, COL, COLOR) (ROW*cvt2dto1d_cols*cvt2dto1d_colors + COL*cvt2dto1d_colors + COLOR)

extern int width;
extern int height;

extern int fps;

extern HWND workerw;
extern HDC window_hdc;
extern HGLRC window_hrc;

void exiting();
void exit();
void setup();
void draw();
void t_start(int);
void t_stop(int);
/*void t_print_millis();
void t_print_micros();*/
long long int t_millis(int);
long long int t_micros(int);

class Image{
public:
    unsigned int w = 0;
    unsigned int h = 0;
    unsigned short bpp = 0;

    unsigned char* arr; //Change when working with large images

    GLuint texture;

    bool allocated = false;
    bool first_read = true;

    void allocate(long int);
    bool read(std::string, bool = true);
    void write(std::string, bool = true);
    void bgr();
    void setSize(unsigned int, unsigned int);
    void place(Image*, unsigned int, unsigned int);
    void copyFrom(Image*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int = 0, unsigned int = 0);
    void initTexture();
    void draw(int, int, int, int, int = CORNER);
    ~Image();
};

BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);

void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
