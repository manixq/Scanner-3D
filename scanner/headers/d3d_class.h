#ifndef _D3D_CLASS_H_
#define _D3D_CLASS_H_

//=======
//linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

//========
//includes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>

//===========
//D3DClass
class D3DClass
{
public:
 D3DClass();
 D3DClass(const D3DClass&);
 ~D3DClass();

 bool Initialize(int, int, bool, HWND, bool, float, float);
 void Shutdown();

 void Begin_scene(float, float, float, float);
 void End_scene();

 ID3D11Device* GetDevice();
 ID3D11DeviceContext* GetDeviceContext();

 void GetProjectionMatrix(D3DXMATRIX&);
 void GetWorldMatrix(D3DMATRIX&);
 void GetOrthoMatrix(D3DXMATRIX&);

 void Turn_zbuffer_on();
 void Turn_zbuffer_off();
 void Turn_culling_on();
 void Turn_culling_off();

 void TurnOnAlphaBlending();
 void TurnOffAlphaBlending();

 ID3D11DepthStencilView* Get_depth_stencil_view();
 void Set_back_buffer_render_target();
 void Reset_viewport();

private:
 bool vsync_enabled_;
 IDXGISwapChain* swap_chain_;
 ID3D11Device* device_;
 ID3D11DeviceContext* device_context_;
 ID3D11RenderTargetView* render_target_view_;
 ID3D11Texture2D* depth_stencil_buffer_;
 ID3D11DepthStencilState* depth_stencil_state_;
 ID3D11DepthStencilView* depth_stencil_view_;
 ID3D11RasterizerState *raster_state_, *raster_state_no_culling_;
 
 D3DXMATRIX projection_matrix_;
 D3DXMATRIX world_matrix_;
 D3DXMATRIX ortho_matrix_;

 D3D11_VIEWPORT view_port_;
 ID3D11DepthStencilState* depth_disabled_stencil_state;
 ID3D11BlendState* alpha_enable_blending_state_;
 ID3D11BlendState* alpha_disable_blending_state_;
};

#endif