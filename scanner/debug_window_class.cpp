#include "headers/debug_window_class.h"

DebugWindowClass::DebugWindowClass()
{
 vertex_buffer_ = nullptr;
 index_buffer_ = nullptr;
}

DebugWindowClass::DebugWindowClass(const DebugWindowClass&)
{
 
}

DebugWindowClass::~DebugWindowClass()
{
 
}

bool DebugWindowClass::Initialize(ID3D11Device* device, int screen_width, int screen_height, int bitmap_width, int bitmap_height)
{
 bool result;

 screen_width_ = screen_width;
 screen_height_ = screen_height;

 bitmap_width_ = bitmap_width;
 bitmap_height_ = bitmap_height;

 previous_posx_ = -1;
 previous_posy_ = -1;

 result = Initialize_buffers(device);
 if (!result)
  return false;

 return true;
}

void DebugWindowClass::Shutdown()
{
 Shutdown_buffers();
}

bool DebugWindowClass::Render(ID3D11DeviceContext* device_context, int position_x, int position_y)
{
 bool result;

 result = Update_buffers(device_context, position_x, position_y);
 if (!result)
  return false;

 Render_buffers(device_context);
}

int DebugWindowClass::Get_index_count()
{
 return index_count_;
}

bool DebugWindowClass::Initialize_buffers(ID3D11Device* device)
{
 VERTEX_TYPE* vertices;
 unsigned long* indices;
 D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
 D3D11_SUBRESOURCE_DATA vertex_data, index_data;
 HRESULT result;
 int i;

 vertex_count_ = 6;
 index_count_ = vertex_count_;

 vertices = new VERTEX_TYPE[vertex_count_];
 if (!vertices)
  return false;

 indices = new unsigned long[index_count_];
 if (!indices)
  return false;

 memset(vertices, 0, (sizeof(VERTEX_TYPE) * vertex_count_));

 vertex_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
 vertex_buffer_desc.ByteWidth = sizeof(VERTEX_TYPE) * vertex_count_;
 vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 vertex_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

void DebugWindowClass::Shutdown_buffers()
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

bool DebugWindowClass::Update_buffers(ID3D11DeviceContext* device_context, int position_x, int position_y)
{
 float left, right, top, bottom;
 VERTEX_TYPE* vertices;
 D3D11_MAPPED_SUBRESOURCE mapped_resource;
 VERTEX_TYPE* vertices_ptr;
 HRESULT result;

 if ((position_x == previous_posx_) && (position_y == previous_posy_))
  return true;

 previous_posx_ = position_x;
 previous_posy_ = position_y;

 left = (float)((screen_width_ / 2) * -1) + (float)position_x;
 right = left + (float)bitmap_width_;
 top = (float)(screen_height_ / 2) - (float)position_y;
 bottom = top - (float)bitmap_height_;

 vertices = new VERTEX_TYPE[vertex_count_];
 if (!vertices)
  return false;

 // Load the vertex array with data.
 // First triangle.
 vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
 vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

 vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
 vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

 vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
 vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

 // Second triangle.
 vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
 vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

 vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
 vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

 vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
 vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

 result = device_context->Map(vertex_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
 if (FAILED(result))
  return false;

 vertices_ptr = (VERTEX_TYPE*)mapped_resource.pData;
 memcpy(vertices_ptr, (void*)vertices, (sizeof(VERTEX_TYPE)*vertex_count_));
 device_context->Unmap(vertex_buffer_, 0);

 delete[] vertices;
 vertices = nullptr;

 return true;
}

void DebugWindowClass::Render_buffers(ID3D11DeviceContext* device_context)
{
 unsigned int stride;
 unsigned int offset;

 stride = sizeof(VERTEX_TYPE);
 offset = 0;
 //set vertex buffer to active in the inpu assembler
 device_context->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
 //set index buffer to active in input assembler
 device_context->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
 device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

