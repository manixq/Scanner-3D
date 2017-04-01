#ifndef _TEXTURE_CLASS_H_
#define _TEXTURE_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <D3DX11tex.h>

//============
//TextureClass
class TextureClass
{
public:
 TextureClass();
 TextureClass(const TextureClass&);
 ~TextureClass();

 bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
 bool Initialize(ID3D11Device*, WCHAR*);
 void Shutdown();
 ID3D11ShaderResourceView** Get_textures();
 ID3D11ShaderResourceView* Get_texture();

private:
 ID3D11ShaderResourceView* textures_[3];
 ID3D11ShaderResourceView* texture_;
};

#endif