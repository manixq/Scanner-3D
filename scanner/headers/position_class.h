#ifndef _POSITION_CLASS_H_
#define _POSITION_CLASS_H_

//========
//Includes
#include <math.h>
#include <D3DX10math.h>

//=============
//PositionClass
class PositionClass
{
public:
 PositionClass();
 PositionClass(const PositionClass&);
 ~PositionClass();

 void Set_frame_time(float);
 void Get_rotation(float&, float&);
 void Get_xpos(float&);
 void Get_zpos(float&);

 void Turn_left(bool);
 void Turn_right(bool);
 void Turn_up(bool);
 void Turn_down(bool);

 void Turn_x(int);
 void Turn_y(int);

 void Go_up(bool);
 void Go_down(bool);
 void Go_left(bool);
 void Go_right(bool);
private:
 void slow_down(float&, float);
 float frame_time_;
 float rotation_y_, rotation_x_;
 float left_turn_speed_, right_turn_speed_, up_turn_speed_, down_turn_speed_;
 float up_speed_, down_speed_, left_speed_, right_speed_;
 float pos_x_, pos_z_;
};
#endif