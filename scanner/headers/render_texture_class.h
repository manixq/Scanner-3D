#ifndef _RENDER_TEXTURE_CLASS_H_
#define _RENDER_TEXTURE_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <d3dx10math.h>

//==================
//RenderTextureClass
class RenderTextureClass
{
public:
 RenderTextureClass();
 RenderTextureClass(const RenderTextureClass&);
 ~RenderTextureClass();

 bool Initialize(ID3D11Device*, int, int);
 bool Initialize(ID3D11Device*, int, int, float, float);
 void Shutdown();

 void Set_render_target(ID3D11DeviceContext*);
 void Clear_render_target(ID3D11DeviceContext*, float, float, float, float);
 ID3D11ShaderResourceView* Get_shader_resource_view();
 
 void Get_projection_matrix(D3DXMATRIX&);
 void Get_ortho_matrix(D3DXMATRIX&);
 int Get_texture_width();
 int Get_texture_height();

private:
    int texture_width_, texture_height_;
    ID3D11Texture2D* render_target_texture_;
    ID3D11RenderTargetView* render_target_view_;
    ID3D11ShaderResourceView* shader_resource_view_;
    ID3D11Texture2D* depth_stencil_buffer_;
    ID3D11DepthStencilView* depth_stencil_view_;
    D3D11_VIEWPORT viewport_;
    D3DXMATRIX projection_matrix_;
    D3DXMATRIX ortho_matrix_;
};
#endif
