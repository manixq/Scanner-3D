#include "headers/ortho_window_class.h"

OrthoWindowClass::OrthoWindowClass()
{
    vertex_buffer_ = nullptr;
    index_buffer_ = nullptr;
}

OrthoWindowClass::OrthoWindowClass(const OrthoWindowClass&)
{
    
}

OrthoWindowClass::~OrthoWindowClass()
{
    
}


bool OrthoWindowClass::Initialize(ID3D11Device* device, int window_width, int window_height)
{
    bool result;
    result = Initialize_buffers(device, window_width, window_height);
    if (!result)
        return false;
    return true;
}

void OrthoWindowClass::Shutdown()
{
    Shutdown_buffers();
}

void OrthoWindowClass::Render(ID3D11DeviceContext* device_context)
{
    Render_buffers(device_context);
}

int OrthoWindowClass::Get_index_count()
{
    return index_count_;
}

bool OrthoWindowClass::Initialize_buffers(ID3D11Device* device, int window_width, int window_height)
{
    float left, right, top, bottom;
    VERTEX_TYPE* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;
    int i;

    left = (float)((window_width / 2) * -1);
    right = left + (float)window_width;
    top = (float)(window_height / 2);
    bottom = top - (float)window_height;

    vertex_count_ = 6;
    index_count_ = vertex_count_;

    vertices = new VERTEX_TYPE[vertex_count_];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count_];
    if (!indices)
        return false;

    vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
    vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

    vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
    vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

    vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
    vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

    vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
    vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

    vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
    vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

    vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
    vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

    for (i = 0; i < index_count_; i++)
        indices[i] = i;

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

    delete[] indices;
    indices = nullptr;

    return true;
}

void OrthoWindowClass::Shutdown_buffers()
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

void OrthoWindowClass::Render_buffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VERTEX_TYPE);
    offset = 0;

    device_context->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
    device_context->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

