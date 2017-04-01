#include "headers/texture_class.h"

TextureClass::TextureClass()
{
 textures_[0] = nullptr;
 textures_[1] = nullptr;
 textures_[2] = nullptr;
 texture_ = nullptr;
}


TextureClass::TextureClass(const TextureClass& other)
{
 
}


TextureClass::~TextureClass()
{
 
}


bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename_1, WCHAR* filename_2, WCHAR* filename_3)
{
 HRESULT result;

 result = D3DX11CreateShaderResourceViewFromFile(device, filename_1, nullptr, nullptr, &textures_[0], nullptr);
 if (FAILED(result))
  return false;
 result = D3DX11CreateShaderResourceViewFromFile(device, filename_2, nullptr, nullptr, &textures_[1], nullptr);
 if (FAILED(result))
  return false;
 result = D3DX11CreateShaderResourceViewFromFile(device, filename_3, nullptr, nullptr, &textures_[2], nullptr);
 if (FAILED(result))
  return false;

 texture_ = textures_[0];
 return true;
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename_1)
{
 HRESULT result;

 result = D3DX11CreateShaderResourceViewFromFile(device, filename_1, nullptr, nullptr, &texture_, nullptr);
 if (FAILED(result))
  return false;

 return true;
}
void TextureClass::Shutdown()
{
 if(textures_[0])
 {
  textures_[0]->Release();
  textures_[0] = nullptr;
  texture_ = nullptr;
 }
 if (textures_[1])
 {
  textures_[1]->Release();
  textures_[1] = nullptr;
 }

 if (textures_[2])
 {
  textures_[2]->Release();
  textures_[2] = nullptr;
 }

 if (texture_)
 {
  texture_->Release();
  texture_ = nullptr;
 }
}

ID3D11ShaderResourceView** TextureClass::Get_textures()
{
 return textures_;
}

ID3D11ShaderResourceView* TextureClass::Get_texture()
{
 return texture_;
}
