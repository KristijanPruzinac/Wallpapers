#include <wallpaper.h>

int cvt2dto1d_cols = 0;
int cvt2dto1d_colors = 0;

void set2dto1d(int cols, int colors){
    cvt2dto1d_cols = cols;
    cvt2dto1d_colors = colors;
}

int width = GetSystemMetrics(SM_CXSCREEN);
int height = GetSystemMetrics(SM_CYSCREEN);

using namespace std;

// TWEAKABLES ---------------------------------------------------------------------------
int fps = 60; //IMPORTANT - Number of frames per second
// --------------------------------------------------------------------------------------

HWND workerw = nullptr;
HDC window_hdc;
HGLRC window_hrc;

//Timer
auto start1 = std::chrono::system_clock::now();
auto stop1 = std::chrono::system_clock::now();

auto start2 = std::chrono::system_clock::now();
auto stop2 = std::chrono::system_clock::now();

auto start3 = std::chrono::system_clock::now();
auto stop3 = std::chrono::system_clock::now();

void t_start(int t){
    if (t == 1)
        start1 = std::chrono::system_clock::now();
    else if (t == 2)
        start2 = std::chrono::system_clock::now();
    else if (t == 3)
        start3 = std::chrono::system_clock::now();
}

void t_stop(int t){
    if (t == 1)
        stop1 = std::chrono::system_clock::now();
    else if (t == 2)
        stop2 = std::chrono::system_clock::now();
    else if (t == 3)
        stop3 = std::chrono::system_clock::now();
}
/*
void t_print_millis(int t){
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << "Took " << duration.count() << " milliseconds." << endl;
}

void t_print_micros(int t){
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    cout << "Took " << duration.count() << " microseconds." << endl;
}*/

long long int t_millis(int t){
    if (t == 1){
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
        return (long long int)(duration.count());
    }
    else if (t == 2){
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
        return (long long int)(duration.count());
    }
    else if (t == 3){
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop3 - start3);
        return (long long int)(duration.count());
    }
}

long long int t_micros(int t){
    if (t == 1){
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
        return (long long int)(duration.count());
    }
    else if (t == 2){
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
        return (long long int)(duration.count());
    }
    else if (t == 3){
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop3 - start3);
        return (long long int)(duration.count());
    }
}

void Image::bgr(){
    set2dto1d(w, bpp);
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            unsigned char tmp = arr[ac(y, x, 0)];
            arr[ac(y, x, 0)] = arr[ac(y, x, 2)];
            arr[ac(y, x, 2)] = tmp;
        }
    }
    initTexture();
}

bool Image::read(string filename, bool bgr_){
    unsigned int offset;
    int padding;

    //Open image file
    ifstream file;
    file.open(filename, std::ios::binary);

    //Fail
    if (!file.is_open()) {
        file.close();
        return 0;
    }

    string empty_ = "..........................";

    //Data offset
    file.read(&empty_[0], 10);
    file.read((char*)&offset, 4);

    //Width and height
    file.read(&empty_[0], 4);
    file.read((char*)&w, 4);
    file.read((char*)&h, 4);

    //Bits per pixel
    file.read(&empty_[0], 2);
    file.read((char*)&bpp, 2);
    bpp /= 8;

    //Calculate row padding
    padding = (4 - ((w * bpp) % 4)) % 4;

    //Go to start of data
    file.seekg(file.beg + offset);

    arr = new unsigned char[w*h*bpp];
    allocated = true;

    //Read
    for (int r = h - 1; r > -1; r--){
        file.read((char*)&arr[r * w*bpp], w*bpp);
        file.read(&empty_[0], padding);
        //file.seekg(file.tellg() + padding);
    }

    file.close();

    if (bgr_){
        bgr();
    }

    initTexture();

    return 1;
}

void Image::write(string filename, bool bgr_){
    int padding;

    //Open image file
    ofstream file;
    file.open(filename, std::ios::binary);

    //Fail
    if (file.fail()) {
        file.close();
        cout << "Cant open file!" << endl;
    }

    //Calculate row padding
    padding = (4 - ((w * bpp) % 4)) % 4;

    //Header ---------------------------

    //Signature
    file.write("BM", 2);

    //File size
    unsigned int file_size = h * (w * bpp + padding) + 54;
    file.write((char*) &file_size, 4);

    //Reserved
    unsigned short tmp_s = 0;
    file.write((char*) &tmp_s, 2);
    file.write((char*) &tmp_s, 2);

    //Data offset
    unsigned int offset = 54;
    file.write((char*) &offset, 4);

    //Size of header
    unsigned int tmp = 40;
    file.write((char*) &tmp, 4);

    //Width and height
    signed int w_s = w;
    signed int h_s = h;

    file.write((char*) &w_s, 4);
    file.write((char*) &h_s, 4);

    //Planes
    tmp_s = 1;
    file.write((char*) &tmp_s, 2);

    //Bits per pixel
    unsigned short bpp_fxed = bpp * 8;
    file.write((char*) &bpp_fxed, 2);

    //Compression
    tmp = 0;
    file.write((char*) &tmp, 4);

    file.write((char*) &tmp, 4);
    file.write((char*) &tmp, 4);
    file.write((char*) &tmp, 4);
    file.write((char*) &tmp, 4);
    file.write((char*) &tmp, 4);

    string padded = "";
    for (int i = 0; i < padding; i++){
        padded += ' ';
    }

    //Write
    for (int r = h - 1; r > -1; r--){
        file.write((char*)&arr[r * w*bpp], w*bpp);
        file.write((char*) &padded, padding);
    }

    file.close();
    return;
}

void Image::setSize(unsigned int w, unsigned int h){
    this -> w = w;
    this -> h = h;
}

void Image::place(Image* from_image, unsigned int x, unsigned int y){
    unsigned int from_w = from_image -> w;
    unsigned int from_h = from_image -> h;
    unsigned short from_bpp = from_image -> bpp;
    unsigned char* from_arr = &(from_image -> arr[0]);

    unsigned int t_h = from_h;
    unsigned int t_w = from_w;

    if (y + t_h > h)
        t_h = h - y - 1;
    if (x + t_w > w)
        t_w = w - x - 1;

    for (unsigned int r = 0; r < t_h; r++){
        if (y + r >= h){
            break;
        }

        for (unsigned int s = 0; s < t_w; s++){
            if (x + s >= w){
                break;
            }

            for (int c = 0; c < from_bpp; c++){
                arr[(y + r) * w * from_bpp + (x + s) * from_bpp + c] = from_arr[r * from_w * from_bpp + s * from_bpp + c];
            }
        }
    }
}

void Image::copyFrom(Image* from_image, unsigned int x_, unsigned int y_, unsigned int w_, unsigned int h_, unsigned int off_x, unsigned int off_y){
    unsigned int max_y = y_ + h_;
    unsigned int max_x = x_ + w_;

    unsigned int from_w = from_image -> w;
    unsigned int from_h = from_image -> h;
    unsigned int from_bpp = from_image -> bpp;
    unsigned char* from_arr = &(from_image -> arr[0]);

    if (max_y > from_h)
        max_y = from_h;
    if (max_x > from_w)
        max_x = from_w;

    w = w_;
    h = h_;
    bpp = from_bpp;

    bool aloc = allocated;

    allocate(w*h*bpp);

    for (unsigned int r = y_; r < max_y; r++){
        for (unsigned int s = x_; s < max_x; s++){
            for (unsigned int i = 0; i < from_bpp; i++)
                arr[((r - y_) + off_y) * w * from_bpp + ((s - x_) + off_x) * from_bpp + i] = from_arr[r * from_w * from_bpp + s * from_bpp + i];
        }
    }

    if (!aloc){
        initTexture();
    }
}

void Image::initTexture(){
    if (first_read){
        first_read = false;

        glGenTextures(1, &texture);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (bpp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &arr[0]);
    else if (bpp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, &arr[0]);
    else if (bpp == 2)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w, h, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, &arr[0]);
    else if (bpp == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &arr[0]);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::draw(int x_, int y_, int w_, int h_, int mode){
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    switch (mode){
        case CORNER:
            glTexCoord2i(0, 0); glVertex2i(x_, y_);
            glTexCoord2i(0, 1); glVertex2i(x_, y_ + h_);
            glTexCoord2i(1, 1); glVertex2i(x_ + w_, y_ + h_);
            glTexCoord2i(1, 0); glVertex2i(x_ + w_, y_);
        break;

        case CORNERS:
            glTexCoord2i(0, 0); glVertex2i(x_, y_);
            glTexCoord2i(0, 1); glVertex2i(x_, h_);
            glTexCoord2i(1, 1); glVertex2i(w_, h_);
            glTexCoord2i(1, 0); glVertex2i(w_, y_);
        break;

        case CENTER:
            glTexCoord2i(0, 0); glVertex2i(x_ - (int) (w_ / 2), y_ - (int) (h_ / 2));
            glTexCoord2i(0, 1); glVertex2i(x_ - (int) (w_ / 2), y_ + (int) (h_ / 2));
            glTexCoord2i(1, 1); glVertex2i(x_ + (int) (w_ / 2), y_ + (int) (h_ / 2));
            glTexCoord2i(1, 0); glVertex2i(x_ + (int) (w_ / 2), y_ - (int) (h_ / 2));
        break;
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::allocate(long int size_){
    if (!(this -> allocated)){
        this -> arr = new unsigned char[size_];
        this -> allocated = true;
    }
    else {
        delete[] arr;
        arr = new unsigned char[size_];
    }
}

Image::~Image(){
    delete[] arr;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
	{
	HWND p = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	if (p != NULL)
		{
		workerw = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
		}
	return true;
}

//enable OpenGL
void EnableOpenGL( HWND hWnd, HDC * hDC, HGLRC * hRC )
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    // get the device context (DC)
    *hDC = GetDC( hWnd );

    // set the pixel format for the DC
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    //pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat( *hDC, &pfd );
    SetPixelFormat( *hDC, iFormat, &pfd );

    // create and enable the render context (RC)
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
}

// Disable OpenGL
void DisableOpenGL( HWND hWnd, HDC hDC, HGLRC hRC )
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC(hWnd, hDC);
}

int main(){
    //GET HWND
    HWND progman = FindWindow("ProgMan", NULL);
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);

    EnumWindows(EnumWindowsProc, NULL);

    EnableOpenGL(workerw, &window_hdc, &window_hrc);

    glViewport(0, 0, width, height);
    glLoadIdentity();
    glOrtho(0.f, width, height, 0.f, 0.f, 1.f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    setup();

    atexit(exit);

    while (true){
        t_start(1);
        draw();
        glFlush();
        t_stop(1);

        while (t_micros(1) < 1000000 / float(fps)){
            Sleep(1);
            t_stop(1);
        }
    }

    return 0;
}

void exit(){
    DisableOpenGL(workerw, window_hdc, window_hrc);
    exiting();
}
