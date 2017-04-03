#include "headers/color_shader_class.h"

ColorShaderClass::ColorShaderClass()
{
 vertex_shader_ = nullptr;
 pixel_shader_ = nullptr;
 layout_ = nullptr;
 matrix_buffer_ = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{
 
}

ColorShaderClass::~ColorShaderClass()
{
 
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
 bool result;

 result = Initialize_shader(device, hwnd, L"shaders/vertex/color.vs", L"shaders/pixel/color.ps");
 if (!result)
  return false;
 return true;
}

void ColorShaderClass::Shutdown()
{
 Shutdown_shader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* device_context, int index_count, D3DXMATRIX world_matrix, D3DXMATRIX view_matrix, D3DXMATRIX projection_matrix)
{
 bool result;

 result = Set_shader_parameters(device_context, world_matrix, view_matrix, projection_matrix);
 if (!result)
  return false;

 Render_shader(device_context, index_count);
 return true;
}

bool ColorShaderClass::Initialize_shader(ID3D11Device* device, HWND hwnd, WCHAR* vs_filename, WCHAR* ps_filename)
{
    HRESULT result;
    ID3D10Blob* error_message;
    ID3D10Blob* vertex_shader_buffer;
    ID3D10Blob* pixel_shader_buffer;
    D3D11_INPUT_ELEMENT_DESC polygon_layout[2];
    unsigned int num_elements;
    D3D11_BUFFER_DESC matrix_buffer_desc;

    error_message = nullptr;
    vertex_shader_buffer = nullptr;
    pixel_shader_buffer = nullptr;

    result = D3DX11CompileFromFile(vs_filename, nullptr, nullptr, "Color_vertex_shader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertex_shader_buffer, &error_message, nullptr);
    if(FAILED(result))
    {
        if (error_message)
            Output_shader_error_message(error_message, hwnd, vs_filename);
        else
            MessageBox(hwnd, vs_filename, L"Missing Shader File", MB_OK);
        return false;
    }

    result = D3DX11CompileFromFile(ps_filename, nullptr, nullptr, "Color_pixel_shader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixel_shader_buffer, &error_message, nullptr);
    if (FAILED(result))
    {
        if (error_message)
            Output_shader_error_message(error_message, hwnd, ps_filename);
        else
            MessageBox(hwnd, ps_filename, L"Missing Shader File", MB_OK);
        return false;
    }

    result = device->CreateVertexShader(vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), nullptr, &vertex_shader_);
    if (FAILED(result))
        return false;
    
    result = device->CreatePixelShader(pixel_shader_buffer->GetBufferPointer(), pixel_shader_buffer->GetBufferSize(), nullptr, &pixel_shader_);
    if (FAILED(result))
        return false;

    polygon_layout[0].SemanticName = "POSITION";
    polygon_layout[0].SemanticIndex = 0;
    polygon_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[0].InputSlot = 0;
    polygon_layout[0].AlignedByteOffset = 0;
    polygon_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[0].InstanceDataStepRate = 0;

    polygon_layout[1].SemanticName = "COLOR";
    polygon_layout[1].SemanticIndex = 0;
    polygon_layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygon_layout[1].InputSlot = 0;
    polygon_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[1].InstanceDataStepRate = 0;

    num_elements = sizeof(polygon_layout) / sizeof(polygon_layout[0]);

    result = device->CreateInputLayout(polygon_layout, num_elements, vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), &layout_);
    if(FAILED(result))
        return false;

    vertex_shader_buffer->Release();
    vertex_shader_buffer = nullptr;
    
    pixel_shader_buffer->Release();
    pixel_shader_buffer = nullptr;

    matrix_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    matrix_buffer_desc.ByteWidth = sizeof(MATRIX_BUFFER_TYPE);
    matrix_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrix_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrix_buffer_desc.MiscFlags = 0;
    matrix_buffer_desc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrix_buffer_desc, nullptr, &matrix_buffer_);
    if (FAILED(result))
        return false;
     
    return true;
}


void ColorShaderClass::Shutdown_shader()
{
    if(matrix_buffer_)
    {
        matrix_buffer_->Release();
        matrix_buffer_ = nullptr;
    }

    if(layout_)
    {
        layout_->Release();
        layout_ = nullptr;
    }

    if(pixel_shader_)
    {
        pixel_shader_->Release();
        pixel_shader_ = nullptr;
    }

    if(vertex_shader_)
    {
        vertex_shader_->Release();
        vertex_shader_ = nullptr;
    }
}

void ColorShaderClass::Output_shader_error_message(ID3D10Blob* error_message, HWND hwnd, WCHAR* shader_filename)
{
 char* compile_errors;
 unsigned long buffer_size, i;
 ofstream fout;

 compile_errors = static_cast<char*>(error_message->GetBufferPointer());
 buffer_size = error_message->GetBufferSize();

 fout.open("shader_error.txt");
 for (i = 0; i < buffer_size; i++)
  fout << compile_errors[i];
 fout.close();

 error_message->Release();
 error_message = nullptr;

 MessageBox(hwnd, L"Error compiling shader. check shader_error.txt", shader_filename, MB_OK);
}


bool ColorShaderClass::Set_shader_parameters(ID3D11DeviceContext* device_context, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    MATRIX_BUFFER_TYPE* data_ptr;
    unsigned int buffer_number;

    //transpose
    D3DXMatrixTranspose(&world, &world);
    D3DXMatrixTranspose(&view, &view);
    D3DXMatrixTranspose(&projection, &projection);

    result = device_context->Map(matrix_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    if (FAILED(result))
        return false;

    data_ptr = static_cast<MATRIX_BUFFER_TYPE*>(mapped_resource.pData);

    data_ptr->world = world;
    data_ptr->view = view;
    data_ptr->projection = projection;

    device_context->Unmap(matrix_buffer_, 0);
    buffer_number = 0;
    device_context->VSSetConstantBuffers(buffer_number, 1, &matrix_buffer_);


    return true;
}

void ColorShaderClass::Render_shader(ID3D11DeviceContext* device_context, int index_count)
{
 //vertex input layout
 device_context->IASetInputLayout(layout_);

 //vertex and pixel shader that ll be used to render 
 device_context->VSSetShader(vertex_shader_, nullptr, 0);
 device_context->PSSetShader(pixel_shader_, nullptr, 0);

 //render
 device_context->DrawIndexed(index_count, 0, 0);
}

