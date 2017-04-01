#include "headers/texture_manager_class.h"

TextureManagerClass::TextureManagerClass()
{
    texture_array_ = nullptr;
}

TextureManagerClass::TextureManagerClass(const TextureManagerClass&)
{
    
}

TextureManagerClass::~TextureManagerClass()
{
    
}

bool TextureManagerClass::Initialize(int count)
{
    texture_count_ = count;
    texture_array_ = new TextureClass[texture_count_];
    if (!texture_array_)
        return false;
    return true;
}

void TextureManagerClass::Shutdown()
{
    int i;
    if(texture_array_)
    {
        for (i = 0; i < texture_count_; i++)
            texture_array_[i].Shutdown();
        delete[] texture_array_;
        texture_array_ = nullptr;
    }
}

bool TextureManagerClass::Load_texture(ID3D11Device* device, WCHAR* filename, int location)
{
    bool result;

    result = texture_array_[location].Initialize(device, filename);
    if (!result)
        return false;
    return true;
}

ID3D11ShaderResourceView* TextureManagerClass::Get_texture(int id)
{
    return texture_array_[id].Get_texture();
}

ID3D11ShaderResourceView** TextureManagerClass::Get_textures(int id)
{
    return texture_array_[id].Get_textures();
}


