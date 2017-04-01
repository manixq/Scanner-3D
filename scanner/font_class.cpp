#include "headers/font_class.h"

FontClass::FontClass()
{
    font_ = nullptr;
    texture_ = nullptr;
}

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, char* font_filename, WCHAR* texture_filename)
{
    bool result;
    result = Load_font_data(font_filename);
    if (!result)
        return false;

    result = Load_texture(device, texture_filename);
    if (!result)
        return false;

    return true;
}

void FontClass::Shutdown()
{
    Release_texture();
    Release_font_data();
}

ID3D11ShaderResourceView* FontClass::Get_texture()
{
    return texture_->Get_texture();
}

void FontClass::Build_vertex_array(void* vertices, char* sentence, float draw_x, float draw_y)
{
    VERTEX_TYPE* vertex_ptr;
    int num_letters, index, i, letter;
    
    vertex_ptr = (VERTEX_TYPE*)vertices;
    num_letters = (int)strlen(sentence);

    index = 0;

    for (i = 0; i < num_letters; i++)
    {
        letter = ((int)sentence[i]) - 32;
        if (letter == 0)
            draw_x = draw_x + 30.0f;
        else
        {
            vertex_ptr[index].position = D3DXVECTOR3(draw_x, draw_y + 16, 0.0f);
            vertex_ptr[index].texture = D3DXVECTOR2(font_[letter].left, 0.0f);
            index++;

            vertex_ptr[index].position = D3DXVECTOR3((draw_x + font_[letter].size / 2) , (draw_y - 16), 0.0f);
            vertex_ptr[index].texture = D3DXVECTOR2(font_[letter].right, 1.0f);
            index++;

            vertex_ptr[index].position = D3DXVECTOR3(draw_x, (draw_y - 16), 0.0f);
            vertex_ptr[index].texture = D3DXVECTOR2(font_[letter].left, 1.0f);
            index++;

            vertex_ptr[index].position = D3DXVECTOR3(draw_x, draw_y + 16, 0.0f);
            vertex_ptr[index].texture = D3DXVECTOR2(font_[letter].left, 0.0f);
            index++;

            vertex_ptr[index].position = D3DXVECTOR3((draw_x + font_[letter].size / 2), draw_y + 16, 0.0f);
            vertex_ptr[index].texture = D3DXVECTOR2(font_[letter].right, 0.0f);
            index++;

            vertex_ptr[index].position = D3DXVECTOR3((draw_x + font_[letter].size / 2) , (draw_y - 16), 0.0f);
            vertex_ptr[index].texture = D3DXVECTOR2(font_[letter].right, 1.0f);
            index++;

            draw_x = draw_x + font_[letter].size / 2 + 1.0f;
        }
    }
}

bool FontClass::Load_font_data(char* filename)
{
    ifstream fin;
    int i;
    char temp;
    font_ = new FONT_TYPE[95];
    if (!font_)
        return false;

    fin.open(filename);
    if (fin.fail())
        return false;

    for (i = 0; i < 95; i++)
    {      
        fin >> font_[i].left;
        fin >> font_[i].right;
        fin >> font_[i].size;
    }
    fin.close();
    return true;
}

void FontClass::Release_font_data()
{
    if(font_)
    {
        delete[] font_;
        font_ = nullptr;
    }
}

bool FontClass::Load_texture(ID3D11Device* device, WCHAR* filename)
{
    bool result;

    texture_ = new TextureClass;
    if (!texture_)
        return false;

    result = texture_->Initialize(device, filename);
    if (!result)
        return false;

    return true;
}

void FontClass::Release_texture()
{
    if(texture_)
    {
        texture_->Shutdown();
        delete texture_;
        texture_ = nullptr;
    }
}
