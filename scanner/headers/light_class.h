#ifndef _LIGHT_CLASS_H_
#define _LIGHT_CLASS_H_

//========
//Includes
#include <D3DX10math.h>

//==========
//LightClass
class LightClass
{
public:
 LightClass();
 LightClass(const LightClass&);
 ~LightClass();
 
 void Set_ambient_color(float, float, float, float);
 void Set_diffuse_color(float, float, float, float);
 void Set_position(float, float, float);
 void Set_direction(float, float, float);
 void Set_specular_color(float, float, float, float);
 void Set_specular_power(float);
 void Set_look_at(float, float, float);

 void Generate_view_matrix();
 void Generate_ortho_matrix(float, float, float);

 void Get_view_matrix(D3DXMATRIX&);
 void Get_ortho_matrix(D3DXMATRIX&);
 D3DXVECTOR4 Get_ambient_color();
 D3DXVECTOR4 Get_diffuse_color();
 D3DXVECTOR3 Get_position();
 D3DXVECTOR4 Get_specular_color();
 D3DXVECTOR3 Get_direction();
 float Get_specular_power();

private:
 D3DXVECTOR4 ambient_color_;
 D3DXVECTOR4 diffuse_color_;
 D3DXVECTOR4 specular_color_;
 D3DXVECTOR3 position_;
 D3DXVECTOR3 look_at_;
 D3DXMATRIX view_matrix_;
 D3DXMATRIX ortho_matrix_;
 D3DXVECTOR3 direction_;
 float specular_power_;
};
#endif