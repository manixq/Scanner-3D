#ifndef _SKYBOX_SHADER_CLASS_H_
#define _SKYBOX_SHADER_CLASS_H_

//========
//includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <d3dx11async.h>
#include <D3DX10.h>

using namespace DirectX;
using namespace std;

//=================
//SkyboxShaderClass
class SkyboxShaderClass
{
    struct MATRIX_BUFFER_TYPE
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

    struct COLOR_BUFFER_TYPE
    {
        XMFLOAT4 apex_color;
        XMFLOAT4 center_color;
    };

    struct SKY_BUFFER_TYPE
    {
        float translate;
        float brightness;
        float padding1;
        float padding2;
    };

public:
    SkyboxShaderClass();
    SkyboxShaderClass(const SkyboxShaderClass&);
    ~SkyboxShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, float, XMFLOAT4, XMFLOAT4);

private:
    bool Initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void Shutdown_shader();
    void Output_shader_error_message(ID3D10Blob*, HWND, WCHAR*);
    bool Set_shader_parameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, float, XMFLOAT4, XMFLOAT4);
    void Render_shader(ID3D11DeviceContext*, int);

    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;
    ID3D11InputLayout* input_layout_;
    ID3D11Buffer* matrix_buffer_;
    ID3D11Buffer* color_buffer_;
    ID3D11Buffer* sky_buffer_;
    ID3D11SamplerState* sample_state_;
};
#endif
