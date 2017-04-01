#ifndef _TERRAIN_SHADER_CLASS_H_
#define _TERRAIN_SHADER_CLASS_H_

//========
//INCLUDES
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

//=================
//TerrainShaderClass
class TerrainShaderClass
{
    struct MATRIX_BUFFER_TYPE
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
        D3DXMATRIX light_view;
        D3DXMATRIX light_projection;
    };

    struct LIGHT_BUFFER_TYPE
    {
        D3DXVECTOR4 ambient_color;
        D3DXVECTOR4 diffuse_color;
        D3DXVECTOR3 light_direction;
        float padding;
    };


public:
    TerrainShaderClass();
    TerrainShaderClass(const TerrainShaderClass&);
    ~TerrainShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

private:
    bool Initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void Shutdown_shader();
    void Output_shader_error_message(ID3D10Blob*, HWND, WCHAR*);
    bool Set_shader_parameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
    void Render_shader(ID3D11DeviceContext*, int);

    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;
    ID3D11InputLayout* layout_;
    ID3D11SamplerState* sample_state_wrap_;
    ID3D11SamplerState* sample_state_clamp_;
    ID3D11Buffer* matrix_buffer_;
    ID3D11Buffer* light_buffer_;
};
#endif