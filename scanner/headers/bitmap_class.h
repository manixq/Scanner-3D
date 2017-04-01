#ifndef _BITMAP_CLASS_H_
#define _BITMAP_CLASS_H_

//========
//INCLUDES
#include <d3d11.h>
#include <D3DX10math.h>
#include "headers/texture_class.h"

//===========
//BitmapClass
class BitmapClass
{
    struct VERTEX_TYPE
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    BitmapClass();
    BitmapClass(const BitmapClass&);
    ~BitmapClass();

    bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, int);

    int Get_index_count();
    ID3D11ShaderResourceView* Get_texture();
    
private:
    bool Initialize_buffers(ID3D11Device*);
    void Shutdown_buffers();
    bool Update_buffers(ID3D11DeviceContext*, int, int);
    void Render_buffers(ID3D11DeviceContext*);

    bool Load_texture(ID3D11Device*, WCHAR*);
    void Release_texture();

    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;
    TextureClass* texture_;

    int screen_width_, screen_height_;
    int bitmap_width_, bitmap_height_;
    int previous_pos_x_, previous_pos_y_;
};
#endif