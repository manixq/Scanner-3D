#include "headers/text_class.h"

TextClass::TextClass()
{
    font_ = nullptr;
    font_shader_ = nullptr;

    sentence1_ = nullptr;
    sentence2_ = nullptr;
}

TextClass::TextClass(const TextClass&)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, HWND hwnd, int screen_width, int screen_height, D3DXMATRIX view_matrix)
{
    bool result;

    screen_width_ = screen_width;
    screen_height_ = screen_height;

    base_view_matrix_ = view_matrix;

    font_ = new FontClass;
    if (!font_)
        return false;

    result = font_->Initialize(device, "data/fontdata.txt", L"data/font.dds");
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
        return false;
    }

    font_shader_ = new FontShaderClass;
    if (!font_shader_)
        return false;

    result = font_shader_->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object", L"Error", MB_OK);
        return false;
    }

    result = Initialize_sentence(&sentence1_, 16, device);
    if (!result)
        return false;
    result = Update_sentence(sentence1_, "FPS: -", 20, 20, 1.0f, 0.0f, 0.0f, device_context);
    if (!result)
        return false;

    result = Initialize_sentence(&sentence2_, 32, device);
    if (!result)
        return false;

    result = Update_sentence(sentence2_, "Someday here we will see GUI.", 20, screen_height - 20, 1.0f, 1.0f, 0.0f, device_context);
    if (!result)
        return false;

    return true;
}

void TextClass::Shutdown()
{
    Release_sentence(&sentence1_);
    Release_sentence(&sentence2_);

    if(font_shader_)
    {
        font_shader_->Shutdown();
        delete font_shader_;
        font_shader_ = nullptr;
    }

    if(font_)
    {
        font_->Shutdown();
        delete font_;
        font_ = nullptr;
    }
}

bool TextClass::Render(ID3D11DeviceContext* device_context, D3DXMATRIX world, D3DXMATRIX ortho)
{
    bool result;

    result = Render_sentence(device_context, sentence1_, world, ortho);
    if (!result)
        return false;

    result = Render_sentence(device_context, sentence2_, world, ortho);
    if (!result)
        return false;

    return true;
}

bool TextClass::Set_fps(int fps, ID3D11DeviceContext* device_context)
{
    char temp_string[16];
    char fps_string[16];
    float red, green, blue;
    bool result;

    if (fps > 9999)
        fps = 9999;

    _itoa_s(fps, temp_string, 10);
    strcpy_s(fps_string, "FPS: ");
    strcat_s(fps_string, temp_string);
    red = (60.0f - fps) / 60.0f;
    green = fps / 60.0f;
    blue = 0.0f;
    result = Update_sentence(sentence1_, fps_string, 20, 20, red, green, blue, device_context);
    if (!result)
        return false;

    return true;
}

bool TextClass::Initialize_sentence(SENTENCE_TYPE** sentence, int max_length, ID3D11Device* device)
{
    VERTEX_TYPE* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;
    int i;

    *sentence = new SENTENCE_TYPE;
    if (!*sentence)
        return false;

    (*sentence)->vertex_buffer_ = nullptr;
    (*sentence)->index_buffer_ = nullptr;
    (*sentence)->max_length = max_length;
    (*sentence)->vertex_count = 6 * max_length;
    (*sentence)->index_count = (*sentence)->vertex_count;

    vertices = new VERTEX_TYPE[(*sentence)->vertex_count];
    if (!vertices)
        return false;

    indices = new unsigned long[(*sentence)->index_count];
    if (!indices)
        return false;

    memset(vertices, 0, (sizeof(VERTEX_TYPE) * (*sentence)->vertex_count));

    for (i = 0; i < (*sentence)->index_count; i++)
        indices[i] = i;

    vertex_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    vertex_buffer_desc.ByteWidth = sizeof(VERTEX_TYPE) * (*sentence)->vertex_count;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &(*sentence)->vertex_buffer_);
    if (FAILED(result))
        return false;

    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(unsigned long)*(*sentence)->index_count;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&index_buffer_desc, &index_data, &(*sentence)->index_buffer_);
    if (FAILED(result))
        return false;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

bool TextClass::Update_sentence(SENTENCE_TYPE* sentence, char* text, int position_x, int position_y, float red, float green, float blue, ID3D11DeviceContext* device_context)
{
    int num_letters;
    VERTEX_TYPE* vertices;
    float draw_x, draw_y;
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    VERTEX_TYPE* vertices_ptr;

    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    num_letters = (int)strlen(text);
    if (num_letters > sentence->max_length)
        return false;

    vertices = new VERTEX_TYPE[sentence->vertex_count];
    if (!vertices)
        return false;

    memset(vertices, 0, (sizeof(VERTEX_TYPE)*sentence->vertex_count));

    draw_x = (float)(((screen_width_ / 2) * -1) + position_x);
    draw_y = (float)((screen_height_ / 2) - position_y);

    font_->Build_vertex_array((void*)vertices, text, draw_x, draw_y);

    result = device_context->Map(sentence->vertex_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    if (FAILED(result))
        return false;

    vertices_ptr = (VERTEX_TYPE*)mapped_resource.pData;
    memcpy(vertices_ptr, (void*)vertices, (sizeof(VERTEX_TYPE)* sentence->vertex_count));
    device_context->Unmap(sentence->vertex_buffer_, 0);

    delete[] vertices;
    vertices = nullptr;

    return true;
}

void TextClass::Release_sentence(SENTENCE_TYPE** sentence)
{
    if(*sentence)
    {
        if((*sentence)->vertex_buffer_)
        {
            (*sentence)->vertex_buffer_->Release();
            (*sentence)->vertex_buffer_ = nullptr;
        }

        if((*sentence)->index_buffer_)
        {
            (*sentence)->index_buffer_->Release();
            (*sentence)->index_buffer_ = nullptr;
        }
        delete *sentence;
        *sentence = nullptr;
    }
}

bool TextClass::Render_sentence(ID3D11DeviceContext* device_context, SENTENCE_TYPE* sentence, D3DXMATRIX world, D3DXMATRIX ortho)
{
    unsigned int stride, offset;
    D3DXVECTOR4 pixel_color;
    bool result;

    stride = sizeof(VERTEX_TYPE);
    offset = 0;

    device_context->IASetVertexBuffers(0, 1, &sentence->vertex_buffer_, &stride, &offset);
    device_context->IASetIndexBuffer(sentence->index_buffer_, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pixel_color = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);
    result = font_shader_->Render(device_context, sentence->index_count, world, base_view_matrix_, ortho, font_->Get_texture(), pixel_color);
    if (!result)
        return false;

    return true;
}
