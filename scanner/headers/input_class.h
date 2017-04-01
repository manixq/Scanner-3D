#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//=======
//INCLUDE
#include <dinput.h>

//============
//InputClass
class InputClass
{
public:
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();

    void Key_down(unsigned int);
    void Key_up(unsigned int);
    void Get_mouse_location(int&, int&);
    bool Is_left_mouse_button_down();

    bool Is_key_down(unsigned int);

private:
    bool Read_mouse();
    void Process_input();
    

    IDirectInput8* direct_input_;
    IDirectInputDevice8* mouse_;

    bool keys_[256];
    DIMOUSESTATE mouse_state_;

    int screen_width_, screen_height_;
    int mouse_x_, mouse_y_;
};
#endif