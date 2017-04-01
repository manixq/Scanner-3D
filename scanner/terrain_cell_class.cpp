#include "headers/terrain_cell_class.h"

TerrainCellClass::TerrainCellClass()
{
    vertex_list = nullptr;
    vertex_buffer_ = nullptr;
    index_buffer_ = nullptr;
    line_vertex_buffer = nullptr;
    line_index_buffer = nullptr;
}

TerrainCellClass::TerrainCellClass(const TerrainCellClass&)
{
}

TerrainCellClass::~TerrainCellClass()
{
}

bool TerrainCellClass::Initialize(ID3D11Device* device, void* terrain_model_ptr, int node_index_x, int node_index_y, int cell_height, int cell_width, int terrain_width)
{
    MODEL_TYPE* terrain_model;
    bool result;

    terrain_model = (MODEL_TYPE*)terrain_model_ptr;

    result = Initialize_buffers(device, node_index_x, node_index_y, cell_height, cell_width, terrain_width, terrain_model);
    if (!result)
        return false;

    terrain_model = nullptr;

    Calculate_cell_dimensions();

    result = Build_line_buffers(device);
    if (!result)
        return false;
    return true;
}

void TerrainCellClass::Shutdown()
{
    Shutdown_line_buffers();
    Shutdown_buffers();
}

void TerrainCellClass::Render(ID3D11DeviceContext* device_context)
{
    Render_buffers(device_context);
}

void TerrainCellClass::Render_line_buffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(COLOR_VERTEX_TYPE);
    offset = 0;

    device_context->IASetVertexBuffers(0, 1, &line_vertex_buffer, &stride, &offset);
    device_context->IASetIndexBuffer(line_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

int TerrainCellClass::Get_vertex_count()
{
    return vertex_count_;
}

int TerrainCellClass::Get_index_count()
{
    return index_count_;
}

int TerrainCellClass::Get_line_buffers_index_count()
{
    return line_index_count_;
}

bool TerrainCellClass::Initialize_buffers(ID3D11Device* device, int node_index_x, int node_index_y, int cell_height, int cell_width, int terrain_width, MODEL_TYPE* terrain_model)
{
    VERTEX_TYPE* vertices;
    unsigned long* indices;

    int i, j, model_index, index;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;

    vertex_count_ = (cell_height - 1) * (cell_width - 1) * 6;
    index_count_ = vertex_count_;

    vertices = new VERTEX_TYPE[vertex_count_];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count_];
    if (!indices)
        return false;

    model_index = ((node_index_x * (cell_width - 1)) + (node_index_y * (cell_height - 1) * (terrain_width - 1))) * 6;
    index = 0;

    for (j = 0; j < (cell_height - 1); j++)
    {
        for (i = 0; i < ((cell_width - 1) * 6); i++)
        {
            vertices[index].position = XMFLOAT3(terrain_model[model_index].x, terrain_model[model_index].y, terrain_model[model_index].z);
            vertices[index].texture = XMFLOAT2(terrain_model[model_index].tu, terrain_model[model_index].tv);
            vertices[index].normal = XMFLOAT3(terrain_model[model_index].nx, terrain_model[model_index].ny, terrain_model[model_index].nz);
            vertices[index].tangent = XMFLOAT3(terrain_model[model_index].tx, terrain_model[model_index].ty, terrain_model[model_index].tz);
            vertices[index].binormal = XMFLOAT3(terrain_model[model_index].bx, terrain_model[model_index].by, terrain_model[model_index].bz);
            vertices[index].color = XMFLOAT3(terrain_model[model_index].r, terrain_model[model_index].g, terrain_model[model_index].b);
            vertices[index].texture2 = XMFLOAT2(terrain_model[model_index].tu2, terrain_model[model_index].tv2);
            indices[index] = index;
            model_index++;
            index++;
        }
        model_index += (terrain_width * 6) - (cell_width * 6);
    }

    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(VERTEX_TYPE) * vertex_count_;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
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

    vertex_list = new VECTOR_TYPE[vertex_count_];
    if (!vertex_list)
        return false;

    for (i = 0; i < vertex_count_; i++)
    {
        vertex_list[i].x = vertices[i].position.x;
        vertex_list[i].y = vertices[i].position.y;
        vertex_list[i].z = vertices[i].position.z;
    }

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void TerrainCellClass::Shutdown_buffers()
{
    if(vertex_list)
    {
        delete[] vertex_list;
        vertex_list = nullptr;
    }

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

void TerrainCellClass::Render_buffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VERTEX_TYPE);
    offset = 0;

    device_context->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
    device_context->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TerrainCellClass::Calculate_cell_dimensions()
{
    int i;
    float width, height, depth;

    max_width_ = -1000000.0f;
    max_height_ = -1000000.0f;
    max_depth_ = -1000000.0f;

    min_width_ = 1000000.0f;
    min_height_ = 1000000.0f;
    min_depth_ = 1000000.0f;

    for (i = 0; i < vertex_count_; i++)
    {
        width = vertex_list[i].x;
        height = vertex_list[i].y;
        depth = vertex_list[i].z;

        if (width > max_width_)
            max_width_ = width;
        if (width < min_width_)
            min_width_ = width;

        if (height > max_height_)
            max_height_ = height;
        if (height < min_height_)
            min_height_ = height;

        if (depth > max_depth_)
            max_depth_ = depth;
        if (depth < min_depth_)
            min_depth_ = depth;
    }
    position_x_ = (max_width_ - min_width_) + min_width_;
    position_y_ = (max_height_ - min_height_) + min_height_;
    position_z_ = (max_depth_ - min_depth_) + min_depth_;
}

bool TerrainCellClass::Build_line_buffers(ID3D11Device* device)
{
    COLOR_VERTEX_TYPE* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;
    XMFLOAT4 line_color;
    int index, vertex_count, index_count;

    line_color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);
    vertex_count = 24;
    index_count = vertex_count;

    vertices = new COLOR_VERTEX_TYPE[vertex_count];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count];
    if (!indices)
        return false;

    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(COLOR_VERTEX_TYPE) * vertex_count;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(unsigned long) * index_count;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    index = 0;

    vertices[index].position = XMFLOAT3(min_width_, min_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, min_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, min_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, min_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, min_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, min_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, min_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, min_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, max_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, max_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, max_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, max_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, max_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, max_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, max_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, max_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    // 4 Verticle lines.
    vertices[index].position = XMFLOAT3(max_width_, max_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, min_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, max_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, min_height_, max_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, max_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(max_width_, min_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, max_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;
    index++;

    vertices[index].position = XMFLOAT3(min_width_, min_height_, min_depth_);
    vertices[index].color = line_color;
    indices[index] = index;

    result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &line_vertex_buffer);
    if (FAILED(result))
        return false;

    result = device->CreateBuffer(&index_buffer_desc, &index_data, &line_index_buffer);
    if (FAILED(result))
        return false;

    line_index_count_ = index_count;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void TerrainCellClass::Shutdown_line_buffers()
{
    if(line_index_buffer)
    {
        line_index_buffer->Release();
        line_index_buffer = nullptr;
    }

    if(line_vertex_buffer)
    {
        line_vertex_buffer->Release();
        line_vertex_buffer = nullptr;
    }
}

void TerrainCellClass::Get_cell_dimensions(float& max_width, float& max_height, float& max_depth, float& min_width, float& min_height, float& min_depth)
{
    max_width = max_width_;
    max_height = max_height_;
    max_depth = max_depth_;
    min_width = min_width_;
    min_height = min_height_;
    min_depth = min_depth_;
}
