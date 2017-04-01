#include "headers/light_class.h"

LightClass::LightClass()
{
 
}

LightClass::LightClass(const LightClass&)
{
 
}

LightClass::~LightClass()
{
 
}

void LightClass::Set_ambient_color(float red, float green, float blue, float alpha)
{
 ambient_color_ = D3DXVECTOR4(red, green, blue, alpha);
}

void LightClass::Set_diffuse_color(float red, float green, float blue, float alpha)
{
 diffuse_color_ = D3DXVECTOR4(red, green, blue, alpha);
}

void LightClass::Set_position(float x, float y, float z)
{
 position_ = D3DXVECTOR3(x, y, z);
}

void LightClass::Set_specular_color(float red, float green, float blue, float alpha)
{
 specular_color_ = D3DXVECTOR4(red, green, blue, alpha);
}

void LightClass::Set_specular_power(float power)
{
 specular_power_ = power;
}

void LightClass::Set_look_at(float x, float y, float z)
{
    look_at_.x = x;
    look_at_.y = y;
    look_at_.z = z;
}

void LightClass::Generate_view_matrix()
{
    D3DXVECTOR3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    D3DXMatrixLookAtLH(&view_matrix_, &position_, &look_at_, &up);
}

void LightClass::Generate_ortho_matrix(float width, float screen_depth, float screen_near)
{
    D3DXMatrixOrthoLH(&ortho_matrix_, width, width, screen_near, screen_depth);
}

void LightClass::Get_view_matrix(D3DXMATRIX& view_matrix)
{
    view_matrix = view_matrix_;
}

void LightClass::Set_direction(float x, float y, float z)
{
    direction_ = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR3 LightClass::Get_direction()
{
    return direction_;
}


void LightClass::Get_ortho_matrix(D3DXMATRIX& ortho_matrix)
{
    ortho_matrix = ortho_matrix_;
}

D3DXVECTOR4 LightClass::Get_ambient_color()
{
 return ambient_color_;
}


D3DXVECTOR4 LightClass::Get_diffuse_color()
{
 return diffuse_color_;
}

D3DXVECTOR3 LightClass::Get_position()
{
 return position_;
}

D3DXVECTOR4 LightClass::Get_specular_color()
{
 return specular_color_;
}

float LightClass::Get_specular_power()
{
 return specular_power_;
}




