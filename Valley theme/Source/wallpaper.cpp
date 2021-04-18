#include <wallpaper.h>

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
auto start = std::chrono::system_clock::now();
auto stop = std::chrono::system_clock::now();

void t_start(){
    start = std::chrono::system_clock::now();
}

void t_stop(){
    stop = std::chrono::system_clock::now();
}

void t_print_millis(){
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << "Took " << duration.count() << " milliseconds." << endl;
}

void t_print_micros(){
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    cout << "Took " << duration.count() << " microseconds." << endl;
}

long long int t_millis(){
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    return (long long int)(duration.count());
}

long long int t_micros(){
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    return (long long int)(duration.count());
}

Image::Image(){
    first_read = true;
}

void Image::read(string filename){
    unsigned int offset;
    int padding;

    //Open image file
    ifstream file;
    file.open(filename, std::ios::binary);

    //Fail
    if (file.fail()) {
        file.close();
        cout << "Invalid file name!" << endl;
        exit(0);
    }

    string tmp = "................................................................................................................................";

    //Data offset
    //file.seekg(file.beg + 10);
    file.read(&tmp[0], 10);
    file.read((char*)&offset, 4);

    //Width and height
    file.read(&tmp[0], 4);
    //file.seekg(file.beg + 18);
    file.read((char*)&w, 4);
    file.read((char*)&h, 4);

    //Bits per pixel
    //file.seekg(file.beg + 28);
    file.read(&tmp[0], 2);
    file.read((char*)&bpp, 2);
    bpp /= 8;

    //Calculate row padding
    padding = (4 - ((w * bpp) % 4)) % 4;

    //Go to start of data
    file.read(&tmp[0], offset - 30);
    //file.seekg(file.beg + offset);

    //Read
    for (int r = h - 1; r > -1; r--){
        file.read((char*)&arr[r * w*bpp], w*bpp);
        file.read(&tmp[0], padding);
    }

    long long int index = 0;

    file.close();

    initTexture();
}

void Image::write(string filename){
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

void Image::initTexture(){
    if (first_read){
        first_read = false;

        glGenTextures(1, &texture);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (bpp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, arr);
    else if (bpp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, arr);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::draw(int x_, int y_, int w_, int h_, int mode = CORNER){
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
    glClearColor(1.0, 1.0, 1.0, 0.0);

    while (true){
        t_start();
        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        glFlush();
        t_stop();

        while (t_micros() < 1000000 / float(fps)){
            Sleep(1);
            t_stop();
        }
    }

    return 0;
}

void exit(){
    DisableOpenGL(workerw, window_hdc, window_hrc);
}
