#ifndef  _FRUSTRUM_CLASS_H_
#define  _FRUSTRUM_CLASS_H_

//========
//Includes
#include <D3DX10math.h>

//=============
//FrustrumClass
class FrustumClass
{
public:
    FrustumClass();
    FrustumClass(const FrustumClass&);
    ~FrustumClass();

    void Initialize(float);

    void ConstructFrustrum(float, D3DXMATRIX, D3DXMATRIX);

    bool Check_point(float, float, float);
    bool Check_cube(float, float, float, float);
    bool Check_sphere(float, float, float, float);
    bool Check_rectangle(float, float, float, float, float, float);
    bool Check_rectangle2(float, float, float, float, float, float);


private:
    D3DXPLANE planes_[6];
};
#endif