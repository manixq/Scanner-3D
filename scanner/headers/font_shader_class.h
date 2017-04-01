#ifndef _FONT_SHADER_CLASS_H_
#define _FONT_SHADER_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

//==================
//FontShaderClass
class FontShaderClass
{
    struct MATRIX_BUFFER_TYPE
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

    struct PIXEL_BUFFER_TYPE
    {
        D3DXVECTOR4 pixel_color;
    };

public:
    FontShaderClass();
    FontShaderClass(const FontShaderClass&);
    ~FontShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:   
    bool Initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void Shutdown_shader();
    void Output_shader_error_message(ID3D10Blob*, HWND, WCHAR*);

    bool Set_shader_parameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);
    void Render_shader(ID3D11DeviceContext*, int);

    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;

    ID3D11InputLayout* layout_;

    ID3D11Buffer* matrix_buffer_;
    ID3D11Buffer* pixel_buffer_;

    ID3D11SamplerState* sample_state_;
};
#endif