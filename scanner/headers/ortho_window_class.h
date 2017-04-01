#ifndef _ORTHO_WINDOW_CLASS_H_
#define _ORTHO_WINDOW_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <D3DX10math.h>

//================
//OrthoWindowClass
class OrthoWindowClass
{
public:
    OrthoWindowClass();
    OrthoWindowClass(const OrthoWindowClass&);
    ~OrthoWindowClass();

    bool Initialize(ID3D11Device*, int, int);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int Get_index_count();

private:
    struct VERTEX_TYPE
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

    bool Initialize_buffers(ID3D11Device*, int, int);
    void Shutdown_buffers();
    void Render_buffers(ID3D11DeviceContext*);

    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;
};
#endif