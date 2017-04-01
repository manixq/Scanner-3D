#ifndef  _DEFERRED_BUFFERS_CLASS_H_
#define  _DEFERRED_BUFFERS_CLASS_H_

const int BUFFER_COUNT = 2;

//========
//includes
#include <d3d11.h>
#include <D3DX10math.h>

//====================
//DeferredBuffersClass
class DeferredBuffersClass
{
public:
    DeferredBuffersClass();
    DeferredBuffersClass(const DeferredBuffersClass&);
    ~DeferredBuffersClass();

    bool Initialize(ID3D11Device*, int, int, float, float);
    void Shutdown();

    void Set_render_targets(ID3D11DeviceContext*);
    void Clear_render_targets(ID3D11DeviceContext*, float, float, float, float);

    ID3D11ShaderResourceView* Get_shader_resource_view(int);

private:
    int texture_width_, texture_height_;

    ID3D11Texture2D* render_target_texture_array_[BUFFER_COUNT];
    ID3D11RenderTargetView* render_target_view_array_[BUFFER_COUNT];
    ID3D11ShaderResourceView* shader_resource_view_array_[BUFFER_COUNT];

    ID3D11Texture2D* depth_stencil_buffer_;
    ID3D11DepthStencilView* depth_stencil_view_;
    D3D11_VIEWPORT view_port_;
};
#endif
