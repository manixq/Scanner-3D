#include "headers/camera_class.h"

CameraClass::CameraClass()
{
 position_x_ = 0.0f;
 position_y_ = 0.0f;
 position_z_ = 0.0f;

 rotation_x_ = 0.0f;
 rotation_y_ = 0.0f;
 rotation_z_ = 0.0f;
}


CameraClass::CameraClass(const CameraClass&)
{
 
}


CameraClass::~CameraClass()
{
 
}


void CameraClass::Set_position(float x, float y , float z)
{
 position_x_ = x;
 position_y_ = y;
 position_z_ = z;
}


void CameraClass::Set_rotation(float x, float y, float z)
{
 rotation_x_ = x;
 rotation_y_ = y;
 rotation_z_ = z;
}


D3DXVECTOR3 CameraClass::Get_position()
{
 return D3DXVECTOR3(position_x_, position_y_, position_z_);
}


D3DXVECTOR3 CameraClass::Get_rotation()
{
 return D3DXVECTOR3(rotation_x_, rotation_y_, rotation_z_);
}


void CameraClass::Render()
{
 D3DXVECTOR3 up, position, look_at;
 float yaw, pitch, roll;
 D3DXMATRIX rotation_matrix;

 up.x = 0.0f;
 up.y = 1.0f;
 up.z = 0.0f;

 position.x = position_x_;
 position.y = position_y_;
 position.z = position_z_;

 look_at.x = 0.0f;
 look_at.y = 0.0f;
 look_at.z = 1.0f;

 //rotations in radians
 pitch = rotation_x_ * 0.0174532925f;
 yaw = rotation_y_ * 0.0174532925f;
 roll = rotation_z_ * 0.0174532925f;

 D3DXMatrixRotationYawPitchRoll(&rotation_matrix, yaw, pitch, roll);
 D3DXVec3TransformCoord(&look_at, &look_at, &rotation_matrix);
 D3DXVec3TransformCoord(&up, &up, &rotation_matrix);

 look_at = position + look_at;

 //finally create view matrix
 D3DXMatrixLookAtLH(&view_matrix_, &position, &look_at, &up);
}

void CameraClass::Render_static()
{
    D3DXVECTOR3 up, position, look_at;
    float yaw, pitch, roll;
    D3DXMATRIX rotation_matrix;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    position.x = position_x_;
    position.y = position_y_;
    position.z = position_z_;

    look_at.x = 0.0f;
    look_at.y = 0.0f;
    look_at.z = 1.0f;

    //rotations in radians
    pitch = rotation_x_ * 0.0174532925f;
    yaw = rotation_y_ * 0.0174532925f;
    roll = rotation_z_ * 0.0174532925f;

    D3DXMatrixRotationYawPitchRoll(&rotation_matrix, yaw, pitch, roll);
    D3DXVec3TransformCoord(&look_at, &look_at, &rotation_matrix);
    D3DXVec3TransformCoord(&up, &up, &rotation_matrix);

    look_at = position + look_at;

    //finally create view matrix
    D3DXMatrixLookAtLH(&static_view_matrix_, &position, &look_at, &up);
}

void CameraClass::Get_view_matrix(D3DXMATRIX& view_matrix)
{
     view_matrix = view_matrix_;
}

void CameraClass::Get_static_view_matrix(D3DXMATRIX& static_view_matrix)
{
    static_view_matrix = static_view_matrix_;
}

void CameraClass::Render_reflection(float height)
{
 D3DXVECTOR3 up, position, look_at;
 float yaw, pitch, roll;
 D3DXMATRIX rotation_matrix;

 up.x = 0.0f;
 up.y = 1.0f;
 up.z = 0.0f;

 position.x = position_x_;
 position.y = -position_y_;
 position.z = position_z_;

 pitch = rotation_x_ * 0.0174532925f;
 yaw = rotation_y_ * 0.0174532925f;
 roll = rotation_z_ * 0.0174532925f;

 look_at.x = - position_x_;
 look_at.y = position_y_ * 2;
 look_at.z = - position_z_;

 D3DXMatrixLookAtLH(&reflection_view_matrix_, &position, &look_at, &up);
}

D3DXMATRIX CameraClass::Get_reflection_view_matrix()
{
 return reflection_view_matrix_;
}

