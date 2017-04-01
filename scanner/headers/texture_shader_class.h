#ifndef _TEXTURE_SHADER_CLASS_H_
#define _TEXTURE_SHADER_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

//==================
//TextureShaderClass
class TextureShaderClass
{
public:
 TextureShaderClass();
 TextureShaderClass(const TextureShaderClass&);
 ~TextureShaderClass();

 bool Initialize(ID3D11Device*, HWND);
 void Shutdown();
 bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
 struct MATRIX_BUFFER_TYPE
 {
  D3DXMATRIX world;
  D3DXMATRIX view;
  D3DXMATRIX projection;
 };

 bool Initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
 void Shutdown_shader();
 void Output_shader_error_message(ID3D10Blob*, HWND, WCHAR*);

 bool Set_shader_parameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
 void Render_shader(ID3D11DeviceContext*, int);

 ID3D11VertexShader* vertex_shader_;
 ID3D11PixelShader* pixel_shader_;
 ID3D11InputLayout* layout_;
 ID3D11Buffer* matrix_buffer_;

 ID3D11SamplerState* sample_state_;
};
#endif