#include "headers/skybox_class.h"

SkyBoxClass::SkyBoxClass()
{
    model_ = nullptr;
    vertex_buffer_ = nullptr;
    index_buffer_ = nullptr;
}

SkyBoxClass::SkyBoxClass(const SkyBoxClass&)
{
}

SkyBoxClass::~SkyBoxClass()
{
}

bool SkyBoxClass::Initialize(ID3D11Device* device)
{
    bool result;

    result = Load_skybox_model("data/skybox/skybox.txt");
    if (!result)
        return false;

    result = Initialize_buffers(device);
    if (!result)
        return false;

    apex_color_ = XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f);
    center_color_ = XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f);
    scale_ = 0.3f;
    brightness_ = 0.5f;
    translation_ = 0.0f;

    return true;
}

void SkyBoxClass::Shutdown()
{
    Release_buffers();
    Release_skybox_model();
}

void SkyBoxClass::Render(ID3D11DeviceContext* device_context)
{
    Render_buffers(device_context);
}

int SkyBoxClass::Get_index_count()
{
    return index_count_;
}

XMFLOAT4 SkyBoxClass::Get_apex_color()
{
    return apex_color_;
}

XMFLOAT4 SkyBoxClass::Get_center_color()
{
    return center_color_;
}

float SkyBoxClass::Get_scale()
{
    return scale_;
}

float SkyBoxClass::Get_translation()
{
    return translation_;
}

float SkyBoxClass::Get_brightness()
{
    return brightness_;
}

bool SkyBoxClass::Load_skybox_model(char* filename)
{
    ifstream fin;
    char input;
    int i;

    fin.open(filename);

    if (fin.fail())
        return false;

    fin.get(input);
    while (input != ':')
        fin.get(input);

    fin >> vertex_count_;
    index_count_ = vertex_count_;

    model_ = new MODEL_TYPE[vertex_count_];
    if (!model_)
        return false;

    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin.get(input);
    fin.get(input);

    for (i = 0; i < vertex_count_; i++)
    {
        fin >> model_[i].x >> model_[i].y >> model_[i].z;
        fin >> model_[i].tu >> model_[i].tv;
        fin >> model_[i].nx >> model_[i].ny >> model_[i].nz;
    }
    fin.close();
    return true;
}

void SkyBoxClass::Release_skybox_model()
{
    if(model_)
    {
        delete model_;
        model_ = nullptr;
    }
}

bool SkyBoxClass::Initialize_buffers(ID3D11Device* device)
{
    VERTEX_TYPE* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;
    int i;

    vertices = new VERTEX_TYPE[vertex_count_];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count_];
    if (!indices)
        return false;

    for (i = 0; i < vertex_count_; i++)
    {
        vertices[i].position = XMFLOAT3(model_[i].x, model_[i].y, model_[i].z);
        vertices[i].texture = XMFLOAT2(model_[i].tu, model_[i].tv);
        indices[i] = i;
    }

    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(VERTEX_TYPE) * vertex_count_;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer_);
    if (FAILED(result))
        return false;

    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(unsigned long) * index_count_;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer_);
    if (FAILED(result))
        return false;

    delete[] vertices;
    vertices = nullptr;

    delete indices;
    indices = nullptr;

    return true;
}

void SkyBoxClass::Release_buffers()
{
    if(index_buffer_)
    {
        index_buffer_->Release();
        index_buffer_ = nullptr;
    }

    if(vertex_buffer_)
    {
        vertex_buffer_->Release();
        vertex_buffer_ = nullptr;
    }
}

void SkyBoxClass::Render_buffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VERTEX_TYPE);
    offset = 0;

    device_context->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
    device_context->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
