#include "headers/system_class.h"

SystemClass::SystemClass()
{
 input_ = nullptr;
 graphics_ = nullptr;
 fps_ = nullptr;
 cpu_ = nullptr;
 timer_ = nullptr;
 position_ = nullptr;
}

SystemClass::SystemClass(const SystemClass& other_class)
{
}
SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
    int screen_width, screen_height;
    bool result;

    screen_width = 0;
    screen_height = 0;

    //Init winapi
    Initialize_windows(screen_width, screen_height);

    //Keyboard input
    input_ = new InputClass;
    if (!input_)
        return false;

    //init
    result = input_->Initialize(hinstance, hwnd, screen_width, screen_height);
    if (!result)
        return false;

    //Rendering and graphics
    graphics_ = new GraphicsClass;
    if (!graphics_)
        return false;

    //init graphics
    result = graphics_->Initialize(screen_width, screen_height, hwnd);
    if (!result)
        return false;

    fps_ = new FpsClass;
    if (!fps_)
        return false;

    fps_->Initialize();

    cpu_ = new CpuClass;
    if (!cpu_)
        return false;

    cpu_->Initialize();

    timer_ = new TimerClass;
    if (!timer_)
        return false;

    result = timer_->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
        return false;
    }

    position_ = new PositionClass;
    if (!position_)
        return false;
    return true;
}

void SystemClass::Shutdown()
{
    if (position_)
    {
        delete position_;
        position_ = nullptr;
    }

    if (timer_)
    {
        delete timer_;
        timer_ = nullptr;
    }

    if (cpu_)
    {
        delete cpu_;
        cpu_ = nullptr;
    }

    if (fps_)
    {
        delete fps_;
        fps_ = nullptr;
    }
    //Release graphics object
    if (graphics_)
    {
        graphics_->Shutdown();
        delete graphics_;
        graphics_ = nullptr;
    }

    //Release input
    if (input_)
    {
        input_->Shutdown();
        delete input_;
        input_ = nullptr;
    }

    //bye, bye
    Shutdown_windows();
}

void SystemClass::Run()
{
    MSG msg;
    bool done, result;

    //Initialize the message structure (fill with zeros)
    ZeroMemory(&msg, sizeof(MSG));

    //Loop until broken
    done = false;
    while (!done)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //if window has been closed
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            //frame proc
            result = Frame();
            if (!result)
                done = true;
        }
    }
}

bool SystemClass::Frame()
{
    bool key_down, result;
    float rotation_x, rotation_y, x_pos, z_pos;
    bool lmb;
    int mouse_x, mouse_y;

    int old_mouse_x;
    int old_mouse_y;

    //escape? no.
    result = input_->Frame();
    if (!result)
        return false;

    if (input_->Is_key_down(VK_ESCAPE))
        return false;

    input_->Get_mouse_location(mouse_x, mouse_y);
    lmb = input_->Is_left_mouse_button_down();
    input_->Set_default_mouse_location();
    input_->Get_mouse_location(old_mouse_x, old_mouse_y);
    timer_->Frame();

    position_->Set_frame_time(timer_->Get_time());
    position_->Turn_up(input_->Is_key_down(VK_UP));
    position_->Turn_down(input_->Is_key_down(VK_DOWN));
    position_->Turn_left(input_->Is_key_down(VK_LEFT));
    position_->Turn_right(input_->Is_key_down(VK_RIGHT));
    position_->Turn_y(old_mouse_x - mouse_x);
    position_->Turn_x(old_mouse_y - mouse_y);
    position_->Go_up(input_->Is_key_down('W'));
    position_->Go_down(input_->Is_key_down('S'));
    position_->Go_left(input_->Is_key_down('A'));
    position_->Go_right(input_->Is_key_down('D'));
    position_->Get_rotation(rotation_x, rotation_y);
    position_->Get_xpos(x_pos);
    position_->Get_zpos(z_pos);

    result = graphics_->Frame(timer_->Get_time(), rotation_x, rotation_y, x_pos, z_pos);
    if (!result)
        return false;

    result = graphics_->Render();
    if (!result)
        return false;
    
    return true;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        //check if key has been pressed
    case WM_KEYDOWN:
    {
        //if pressed - send to input object to record its state
        input_->Key_down(static_cast<unsigned int>(wparam));
        return 0;
    }
    case WM_KEYUP:
    {
        //unset state
        input_->Key_up(static_cast<unsigned int>(wparam));
        return 0;
    }
    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}


void SystemClass::Initialize_windows(int& screen_width, int& screen_height)
{
    WNDCLASSEX wc;
    DEVMODE dm_screen_settings;
    int pos_x, pos_y;

    //external pointer
    application_handle = this;

    //instance of app
    hinstance = GetModuleHandle(nullptr);

    //app name
    application_name = L"Scanner";

    //window class
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = application_name;
    wc.cbSize = sizeof(WNDCLASSEX);

    //register window class
    RegisterClassEx(&wc);

    //res
    screen_width = GetSystemMetrics(SM_CXSCREEN);
    screen_height = GetSystemMetrics(SM_CYSCREEN);

    //fullscreen and windowed settings
    if (FULL_SCREEN)
    {
        //fullscreen
        memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
        dm_screen_settings.dmSize = sizeof(dm_screen_settings);
        dm_screen_settings.dmPelsWidth = (unsigned long)screen_width;
        dm_screen_settings.dmPelsHeight = (unsigned long)screen_height;
        dm_screen_settings.dmBitsPerPel = 32;
        dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
        //left top corner
        pos_x = pos_y = 0;
    }
    else
    {
        screen_width = 800;
        screen_height = 600;

        pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
        pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;
    }

    //create window
    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        application_name, application_name,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        pos_x, pos_y,
        screen_width, screen_height,
        nullptr, nullptr,
        hinstance, nullptr);

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    ShowCursor(false);
}


void SystemClass::Shutdown_windows()
{
    ShowCursor(true);

    if (FULL_SCREEN)
        ChangeDisplaySettings(nullptr, 0);

    DestroyWindow(hwnd);
    hwnd = nullptr;

    UnregisterClass(application_name, hinstance);
    hinstance = nullptr;

    application_handle = nullptr;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    default:
        return application_handle->MessageHandler(hwnd, umessage, wparam, lparam);
    }
}