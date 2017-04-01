#ifndef _TEXTURE_MANAGER_CLASS_H_
#define _TEXTURE_MANAGER_CLASS_H_

#include "headers/texture_class.h"

class TextureManagerClass
{
public:
    TextureManagerClass();
    TextureManagerClass(const TextureManagerClass&);
    ~TextureManagerClass();

    bool Initialize(int);
    void Shutdown();

    bool Load_texture(ID3D11Device*, WCHAR*, int);

    ID3D11ShaderResourceView* Get_texture(int); 
    ID3D11ShaderResourceView** TextureManagerClass::Get_textures(int);

private:
    TextureClass* texture_array_;
    int texture_count_;
};
#endif
