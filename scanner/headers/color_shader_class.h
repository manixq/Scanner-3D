#ifndef _COLOR_SHADER_CLASS_H_
#define _COLOR_SHADER_CLASS_H_

//=========
//includes
#include <d3d11.h>
#include  <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

//================
//ColorShaderClass
class ColorShaderClass
{
    struct MATRIX_BUFFER_TYPE
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

    struct TESSELATION_BUFFER_TYPE
    {
        float tessellation_amount;
        D3DXVECTOR3 padding;
    };

    struct LIGHT_BUFFER_TYPE
    {
        D3DXVECTOR3 light_direction;
        float padding;
    };

    struct CAMERA_BUFFER_TYPE
    {
        D3DXVECTOR3 camera_position;
        float padding;
    };


public:
    ColorShaderClass();
    ColorShaderClass(const ColorShaderClass&);
    ~ColorShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, float, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3);

private:    
    bool Initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*, WCHAR*);
    void Shutdown_shader();
    void Output_shader_error_message(ID3D10Blob*, HWND, WCHAR*);
 
    bool Set_shader_parameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, float, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3);
    void Render_shader(ID3D11DeviceContext*, int);

    ID3D11HullShader* hull_shader_;
    ID3D11DomainShader* domain_shader_;
    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;
    ID3D11InputLayout* layout_;
    ID3D11SamplerState* sample_state_;
    ID3D11Buffer* matrix_buffer_;
    ID3D11Buffer* light_buffer_;
    ID3D11Buffer* tessellation_buffer_;
};
#endif