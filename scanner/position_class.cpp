#include "headers/position_class.h"

PositionClass::PositionClass()
{
 frame_time_ = 0.0f;
 rotation_y_ = 0.0f;
 rotation_x_ = 0.0f;
 left_turn_speed_ = 0.0f;
 right_turn_speed_ = 0.0f;
 up_turn_speed_ = 0.0f;
 down_turn_speed_ = 0.0f;
 up_speed_ = 0.0f;
 down_speed_ = 0.0f;
 left_speed_ = 0.0f;
 right_speed_ = 0.0f;
 pos_x_ = 0.0f;
 pos_z_ = -10.0f;

}

PositionClass::PositionClass(const PositionClass&)
{
 
}

PositionClass::~PositionClass()
{
 
}

void PositionClass::Set_frame_time(float time)
{
 frame_time_ = time;
}

void PositionClass::Get_rotation(float& x, float& y)
{
 x = rotation_x_; 
 y = rotation_y_;
}

void PositionClass::Get_zpos(float& z)
{
 z = pos_z_;
}

void PositionClass::Get_xpos(float& x)
{
 x = pos_x_;
}

void PositionClass::Turn_left(bool keydown)
{
 if(keydown)
 {
  left_turn_speed_ +=  0.05f;
  if (left_turn_speed_ > ( 1.0f))
   left_turn_speed_ = 1.0f;
 }
 else
 {
  slow_down(left_turn_speed_, 0.05f);
 }

 rotation_y_ -= left_turn_speed_;
 if (rotation_y_ < 0.0f)
  rotation_y_ += 360.0f;
}

void PositionClass::Turn_right(bool keydown)
{
 if (keydown)
 {
  right_turn_speed_ += 0.05f;
  if (right_turn_speed_ > ( 1.0f))
   right_turn_speed_ = 1.0f;
 }
 else
 {
  slow_down(right_turn_speed_, 0.05f);
 }

 rotation_y_ += right_turn_speed_;
 if (rotation_y_ > 360.0f)
  rotation_y_ -= 360.0f;
}

void PositionClass::Turn_up(bool keydown)
{
 if (keydown)
 {
  up_turn_speed_ += 0.05f;
  if (up_turn_speed_ > (1.0f))
   up_turn_speed_ = 1.0f;
 }
 else
 {
  slow_down(up_turn_speed_, 0.05f);
 }

 rotation_x_ -= up_turn_speed_;
 if (rotation_x_ < 0.0f)
  rotation_x_ += 360.0f;
}

void PositionClass::Turn_down(bool keydown)
{
 if (keydown)
 {
  down_turn_speed_ += 0.05f;
  if (down_turn_speed_ > (1.0f))
   down_turn_speed_ = 1.0f;
 }
 else
 {
  slow_down(down_turn_speed_, 0.05f);
 }

 rotation_x_ += down_turn_speed_;
 if (rotation_x_ > 360.0f)
  rotation_x_ -= 360.0f;
}

void PositionClass::Turn_x(int x_turn_speed)
{
    rotation_x_ -= x_turn_speed / 10.0f;
    if (rotation_x_ > 360.0f)
        rotation_x_ -= 360.0f;
    if (rotation_x_ < 0.0f)
        rotation_x_ += 360.0f;
}

void PositionClass::Turn_y(int y_turn_speed)
{
    rotation_y_ -= y_turn_speed / 10.0f;
    if (rotation_y_ > 360.0f)
        rotation_y_ -= 360.0f;
    if (rotation_y_ < 0.0f)
        rotation_y_ += 360.0f;
}

void PositionClass::Go_up(bool keydown)
{
 if (keydown)
 {
  up_speed_ += 0.15f;
  if (up_speed_ >(0.45f))
   up_speed_ = 0.45f;
 }
 else
 {
  slow_down(up_speed_, 0.05f);
 }

 pos_x_ += up_speed_ * sin(rotation_y_ * D3DX_PI / 180);
 pos_z_ += up_speed_ * cos(rotation_y_ * D3DX_PI / 180);
}

void PositionClass::Go_down(bool keydown)
{
 if (keydown)
 {
  down_speed_ += 0.15f;
  if (down_speed_ >(0.45f))
   down_speed_ = 0.45f;
 }
 else
 {
  slow_down(down_speed_, 0.05f);
 }

 pos_x_ -= down_speed_ * sin(rotation_y_ * D3DX_PI / 180);
 pos_z_ -= down_speed_ * cos(rotation_y_ * D3DX_PI / 180);
}

void PositionClass::Go_left(bool keydown)
{
 if (keydown)
 {
  left_speed_ += 0.15f;
  if (left_speed_ >(0.45f))
   left_speed_ = 0.45f;
 }
 else
 {
  slow_down(left_speed_, 0.05f);
 }

 pos_x_ -= left_speed_ * sin((rotation_y_ + 90) * D3DX_PI / 180);
 pos_z_ -= left_speed_ * cos((rotation_y_ + 90) * D3DX_PI / 180);
}

void PositionClass::Go_right(bool keydown)
{
 if (keydown)
 {
  right_speed_ += 0.15f;
  if (right_speed_ >(0.45f))
   right_speed_ = 0.45f;
 }
 else
 {
  slow_down(right_speed_, 0.05f);
 }

 pos_x_ += right_speed_ * sin((rotation_y_ + 90) * D3DX_PI / 180);
 pos_z_ += right_speed_ * cos((rotation_y_ + 90) * D3DX_PI / 180);
}

void PositionClass::slow_down(float& speed, float slow_down)
{
 if (speed > 0.0f)
  speed -= slow_down;
 else
  speed = 0.0f;
}
