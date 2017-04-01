#ifndef  _FONT_CLASS_H_
#define  _FONT_CLASS_H_

//=======
//INCLUDE
#include <d3d11.h>
#include <D3DX10math.h>
#include <fstream>
#include "headers/texture_class.h"

using namespace std;

//=========
//FontClass
class FontClass
{
    struct FONT_TYPE
    {
        float left, right;
        int size;
    };

    struct VERTEX_TYPE
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    FontClass();
    FontClass(const FontClass&);
    ~FontClass();

    bool Initialize(ID3D11Device*, char*, WCHAR*);
    void Shutdown();

    ID3D11ShaderResourceView* Get_texture();
    void Build_vertex_array(void*, char*, float, float);

private:
    bool Load_font_data(char*);
    void Release_font_data();
    bool Load_texture(ID3D11Device*, WCHAR*);
    void Release_texture();

    FONT_TYPE* font_;
    TextureClass* texture_;
};
#endif
