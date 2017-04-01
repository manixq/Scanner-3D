#include "headers/input_class.h"

InputClass::InputClass()
{
    direct_input_ = nullptr;
    mouse_ = nullptr;
}

InputClass::InputClass(const InputClass&)
{

}

InputClass::~InputClass()
{

}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screen_width, int screen_height)
{
    HRESULT result;

    screen_width_ = screen_width;
    screen_height_ = screen_height;

    mouse_x_ = 0;
    mouse_y_ = 0;

    int i;
    for (i = 0; i < 256; i++)
        keys_[i] = false;

    result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&direct_input_, nullptr);
    if (FAILED(result))
        return false;

    result = direct_input_->CreateDevice(GUID_SysMouse, &mouse_, nullptr);
    if (FAILED(result))
        return false;

    result = mouse_->SetDataFormat(&c_dfDIMouse);
    if (FAILED(result))
        return false;

    result = mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if (FAILED(result))
        return false;

    result = mouse_->Acquire();
    if (FAILED(result))
        return false;

    return true;
}

void InputClass::Shutdown()
{
    if(mouse_)
    {
        mouse_->Unacquire();
        mouse_->Release();
        mouse_ = nullptr;
    }


    if(direct_input_)
    {
        direct_input_->Release();
        direct_input_ = nullptr;
    }
}

bool InputClass::Frame()
{
    bool result;    
    result = Read_mouse();
    if (!result)
        return false;

    Process_input();
    return true;
}

void InputClass::Key_down(unsigned int input)
{
    keys_[input] = true;
}

void InputClass::Key_up(unsigned int input)
{
    keys_[input] = false;
}

void InputClass::Get_mouse_location(int &mouse_x, int &mouse_y)
{
    mouse_x = mouse_x_;
    mouse_y = mouse_y_;
}

bool InputClass::Is_left_mouse_button_down()
{
    if (mouse_state_.rgbButtons[0] & 0x80)
        return true;
    return false;
}

bool InputClass::Is_key_down(unsigned int key)
{
    return keys_[key];
}

bool InputClass::Read_mouse()
{
    HRESULT result;

    result = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state_);
    if(FAILED(result))
    {
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            mouse_->Acquire();
        else
            return false;
    }

    return true;
}

void InputClass::Process_input()
{
    mouse_x_ += mouse_state_.lX;
    mouse_y_ += mouse_state_.lY;

    if (mouse_x_ < 0) { mouse_x_ = 0; }
    if (mouse_y_ < 0) { mouse_y_ = 0; }

    if (mouse_x_ > screen_width_) { mouse_x_ = screen_width_; }
    if (mouse_y_ > screen_height_) { mouse_y_ = screen_height_; }
}




