#include "headers/frustum_class.h"

FrustumClass::FrustumClass()
{
 
}

FrustumClass::FrustumClass(const FrustumClass&)
{
 
}

FrustumClass::~FrustumClass()
{
 
}

void FrustumClass::Initialize(float x)
{
     
}

void FrustumClass::ConstructFrustrum(float screen_depth, D3DXMATRIX projection_matrix, D3DXMATRIX view_matrix)
{
 float z_min, r;
 D3DXMATRIX matrix;

 z_min = -projection_matrix._43 / projection_matrix._33;
 r = screen_depth / (screen_depth - z_min);
 projection_matrix._33 = r;
 projection_matrix._43 = -r * z_min;

 D3DXMatrixMultiply(&matrix, &view_matrix, &projection_matrix);

 planes_[0].a = matrix._14 + matrix._13;
 planes_[0].b = matrix._24 + matrix._23;
 planes_[0].c = matrix._34 + matrix._33;
 planes_[0].d = matrix._44 + matrix._43;
 D3DXPlaneNormalize(&planes_[0], &planes_[0]);

 planes_[1].a = matrix._14 - matrix._13;
 planes_[1].b = matrix._24 - matrix._23;
 planes_[1].c = matrix._34 - matrix._33;
 planes_[1].d = matrix._44 - matrix._43;
 D3DXPlaneNormalize(&planes_[1], &planes_[1]);

 // Calculate left plane of frustum.
 planes_[2].a = matrix._14 + matrix._11;
 planes_[2].b = matrix._24 + matrix._21;
 planes_[2].c = matrix._34 + matrix._31;
 planes_[2].d = matrix._44 + matrix._41;
 D3DXPlaneNormalize(&planes_[2], &planes_[2]);

 // Calculate right plane of frustum.
 planes_[3].a = matrix._14 - matrix._11;
 planes_[3].b = matrix._24 - matrix._21;
 planes_[3].c = matrix._34 - matrix._31;
 planes_[3].d = matrix._44 - matrix._41;
 D3DXPlaneNormalize(&planes_[3], &planes_[3]);

 // Calculate top plane of frustum.
 planes_[4].a = matrix._14 - matrix._12;
 planes_[4].b = matrix._24 - matrix._22;
 planes_[4].c = matrix._34 - matrix._32;
 planes_[4].d = matrix._44 - matrix._42;
 D3DXPlaneNormalize(&planes_[4], &planes_[4]);

 // Calculate bottom plane of frustum.
 planes_[5].a = matrix._14 + matrix._12;
 planes_[5].b = matrix._24 + matrix._22;
 planes_[5].c = matrix._34 + matrix._32;
 planes_[5].d = matrix._44 + matrix._42;
 D3DXPlaneNormalize(&planes_[5], &planes_[5]); 
}

bool FrustumClass::Check_point(float x, float y, float z)
{
 int i;

 for (i = 0; i < 6; i++)
 {
  if(D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
  {
   return false;
  }
 }
 return true;
}

bool FrustumClass::Check_cube(float x_center, float y_center, float z_center, float radius)
{
 int i;

 for (i = 0; i < 6; i++)
 {
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - radius), (y_center - radius), (z_center - radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + radius), (y_center - radius), (z_center - radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - radius), (y_center + radius), (z_center - radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - radius), (y_center - radius), (z_center + radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + radius), (y_center - radius), (z_center + radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - radius), (y_center + radius), (z_center + radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + radius), (y_center + radius), (z_center - radius))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + radius), (y_center + radius), (z_center + radius))) >= 0.0f)
   continue;
  return false;
 }
 return true;
}

bool FrustumClass::Check_sphere(float x_center, float y_center, float z_center, float radius)
{
 int i;

 for (i = 0; i < 6; i++)
 {
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3(x_center, y_center, z_center)) < -radius)
   return false;
 }
 return true;
}

bool FrustumClass::Check_rectangle(float x_center, float y_center, float z_center, float x_size, float y_size, float z_size)
{
 int i;

 for (i = 0; i < 6; i++)
 {
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - x_size), (y_center - y_size), (z_center - z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + x_size), (y_center - y_size), (z_center - z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - x_size), (y_center + y_size), (z_center - z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - x_size), (y_center - y_size), (z_center + z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + x_size), (y_center - y_size), (z_center + z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center - x_size), (y_center + y_size), (z_center + z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + x_size), (y_center + y_size), (z_center - z_size))) >= 0.0f)
   continue;
  if (D3DXPlaneDotCoord(&planes_[i], &D3DXVECTOR3((x_center + x_size), (y_center + y_size), (z_center + z_size))) >= 0.0f)
   continue;
  return false;
 }
 return true;
}

bool FrustumClass::Check_rectangle2(float max_width, float max_height, float max_depth, float min_width, float min_height, float min_depth)
{
    int i;
    float dot_product;

    for (i = 0; i < 6; i++)
    {
        dot_product = ((planes_[i].a * min_width) + (planes_[i].b * min_height) + (planes_[i].c * min_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * max_width) + (planes_[i].b * min_height) + (planes_[i].c * min_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * min_width) + (planes_[i].b * max_height) + (planes_[i].c * min_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * max_width) + (planes_[i].b * max_height) + (planes_[i].c * min_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * min_width) + (planes_[i].b * min_height) + (planes_[i].c * max_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * max_width) + (planes_[i].b * min_height) + (planes_[i].c * max_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * min_width) + (planes_[i].b * max_height) + (planes_[i].c * max_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        dot_product = ((planes_[i].a * max_width) + (planes_[i].b * max_height) + (planes_[i].c * max_depth) + (planes_[i].d * 1.0f));
        if (dot_product >= 0.0f)
            continue;
        return false;
    }
    return true;
}
