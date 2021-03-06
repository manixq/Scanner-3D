#include "headers/color_shader_class.h"

ColorShaderClass::ColorShaderClass()
{
    hull_shader_ = nullptr;
    domain_shader_ = nullptr;
    vertex_shader_ = nullptr;
    pixel_shader_ = nullptr;
    layout_ = nullptr;
    tessellation_buffer_ = nullptr;
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

    result = Initialize_shader(device, hwnd, L"shaders/hull/color.hs", L"shaders/domain/color.ds", L"shaders/vertex/color.vs", L"shaders/pixel/color.ps");
    if (!result)
        return false;
    return true;
}

void ColorShaderClass::Shutdown()
{
    Shutdown_shader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* device_context, int index_count, D3DXMATRIX world_matrix, D3DXMATRIX view_matrix, D3DXMATRIX projection_matrix, float tessellation_amount, ID3D11ShaderResourceView* displacement, ID3D11ShaderResourceView* normal, D3DXVECTOR3 light_direction)
{
    bool result;

    result = Set_shader_parameters(device_context, world_matrix, view_matrix, projection_matrix, tessellation_amount, displacement, normal, light_direction);
    if (!result)
        return false;

    Render_shader(device_context, index_count);
    return true;
}

bool ColorShaderClass::Initialize_shader(ID3D11Device* device, HWND hwnd, WCHAR* hs_filename, WCHAR* ds_filename, WCHAR* vs_filename, WCHAR* ps_filename)
{
    HRESULT result;
    ID3D10Blob* error_message;
    ID3D10Blob* hull_shader_buffer;
    ID3D10Blob* domain_shader_buffer;
    ID3D10Blob* vertex_shader_buffer;
    ID3D10Blob* pixel_shader_buffer;
    D3D11_INPUT_ELEMENT_DESC polygon_layout[7];
    unsigned int num_elements;
    D3D11_BUFFER_DESC matrix_buffer_desc;
    D3D11_BUFFER_DESC  light_buffer_desc;
    D3D11_BUFFER_DESC tessellation_buffer_desc;
    D3D11_SAMPLER_DESC sampler_desc;


    error_message = nullptr;
    hull_shader_buffer = nullptr;
    domain_shader_buffer = nullptr;
    vertex_shader_buffer = nullptr;
    pixel_shader_buffer = nullptr;

    result = D3DX11CompileFromFile(hs_filename, nullptr, nullptr, "ColorHullShader", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &hull_shader_buffer, &error_message, nullptr);
    if (FAILED(result))
    {
        if (error_message)
            Output_shader_error_message(error_message, hwnd, hs_filename);
        else
            MessageBox(hwnd, hs_filename, L"Missing Shader File", MB_OK);
        return false;
    }

    result = D3DX11CompileFromFile(ds_filename, nullptr, nullptr, "ColorDomainShader", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &domain_shader_buffer, &error_message, nullptr);
    if (FAILED(result))
    {
        if (error_message)
            Output_shader_error_message(error_message, hwnd, ds_filename);
        else
            MessageBox(hwnd, ds_filename, L"Missing Shader File", MB_OK);
        return false;
    }

    result = D3DX11CompileFromFile(vs_filename, nullptr, nullptr, "Color_vertex_shader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertex_shader_buffer, &error_message, nullptr);
    if (FAILED(result))
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

    result = device->CreateHullShader(hull_shader_buffer->GetBufferPointer(), hull_shader_buffer->GetBufferSize(), nullptr, &hull_shader_);
    if (FAILED(result))
        return false;

    result = device->CreateDomainShader(domain_shader_buffer->GetBufferPointer(), domain_shader_buffer->GetBufferSize(), nullptr, &domain_shader_);
    if (FAILED(result))
        return false;

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

    polygon_layout[2].SemanticName = "TEXCOORD";
    polygon_layout[2].SemanticIndex = 0;
    polygon_layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygon_layout[2].InputSlot = 0;
    polygon_layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[2].InstanceDataStepRate = 0;

    polygon_layout[3].SemanticName = "NORMAL";
    polygon_layout[3].SemanticIndex = 0;
    polygon_layout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[3].InputSlot = 0;
    polygon_layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[3].InstanceDataStepRate = 0;

    polygon_layout[4].SemanticName = "TANGENT";
    polygon_layout[4].SemanticIndex = 0;
    polygon_layout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[4].InputSlot = 0;
    polygon_layout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[4].InstanceDataStepRate = 0;

    polygon_layout[5].SemanticName = "BINORMAL";
    polygon_layout[5].SemanticIndex = 0;
    polygon_layout[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[5].InputSlot = 0;
    polygon_layout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[5].InstanceDataStepRate = 0;


    polygon_layout[6].SemanticName = "TEXCOORD";
    polygon_layout[6].SemanticIndex = 1;
    polygon_layout[6].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygon_layout[6].InputSlot = 0;
    polygon_layout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[6].InstanceDataStepRate = 0;

    num_elements = sizeof(polygon_layout) / sizeof(polygon_layout[0]);

    result = device->CreateInputLayout(polygon_layout, num_elements, vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), &layout_);
    if (FAILED(result))
        return false;

    hull_shader_buffer->Release();
    hull_shader_buffer = nullptr;

    domain_shader_buffer->Release();
    domain_shader_buffer = nullptr;

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

    tessellation_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    tessellation_buffer_desc.ByteWidth = sizeof(TESSELATION_BUFFER_TYPE);
    tessellation_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    tessellation_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tessellation_buffer_desc.MiscFlags = 0;
    tessellation_buffer_desc.StructureByteStride = 0;

    result = device->CreateBuffer(&tessellation_buffer_desc, nullptr, &tessellation_buffer_);
    if (FAILED(result))
        return false;

    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MipLODBias = 0.0f;
    sampler_desc.MaxAnisotropy = 1;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_desc.BorderColor[0] = 0;
    sampler_desc.BorderColor[1] = 0;
    sampler_desc.BorderColor[2] = 0;
    sampler_desc.BorderColor[3] = 0;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&sampler_desc, &sample_state_);
    if (FAILED(result))
        return false;

    light_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    light_buffer_desc.ByteWidth = sizeof(LIGHT_BUFFER_TYPE);
    light_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    light_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    light_buffer_desc.MiscFlags = 0;
    light_buffer_desc.StructureByteStride = 0;

    result = device->CreateBuffer(&light_buffer_desc, nullptr, &light_buffer_);
    if (FAILED(result))
        return false;

    return true;
}


void ColorShaderClass::Shutdown_shader()
{
    if(tessellation_buffer_)
    {
        tessellation_buffer_->Release();
        tessellation_buffer_ = nullptr;
    }

    if (matrix_buffer_)
    {
        matrix_buffer_->Release();
        matrix_buffer_ = nullptr;
    }

    if (layout_)
    {
        layout_->Release();
        layout_ = nullptr;
    }

    if(domain_shader_)
    {
        domain_shader_->Release();
        domain_shader_ = nullptr;
    }

    if(hull_shader_)
    {
        hull_shader_->Release();
        hull_shader_ = nullptr;
    }

    if (pixel_shader_)
    {
        pixel_shader_->Release();
        pixel_shader_ = nullptr;
    }

    if (vertex_shader_)
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


bool ColorShaderClass::Set_shader_parameters(ID3D11DeviceContext* device_context, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, float tessellation_amount, ID3D11ShaderResourceView* displacement, ID3D11ShaderResourceView* normal, D3DXVECTOR3 light_direction)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    MATRIX_BUFFER_TYPE *data_ptr;
    LIGHT_BUFFER_TYPE *data_ptr2;
    TESSELATION_BUFFER_TYPE * data_ptr3;
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
    device_context->DSSetConstantBuffers(buffer_number, 1, &matrix_buffer_);
    device_context->PSSetShaderResources(0, 1, &normal);
    device_context->DSSetShaderResources(0, 1, &displacement);

    result = device_context->Map(light_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    if (FAILED(result))
        return false;

    data_ptr2 = static_cast<LIGHT_BUFFER_TYPE*>(mapped_resource.pData);
    data_ptr2->light_direction = light_direction;
    data_ptr2->padding = 0.0f;

    device_context->Unmap(light_buffer_, 0);
    buffer_number = 0;
    device_context->PSSetConstantBuffers(buffer_number, 1, &light_buffer_);

    result = device_context->Map(tessellation_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    if (FAILED(result))
        return false;

    data_ptr3 = static_cast<TESSELATION_BUFFER_TYPE*>(mapped_resource.pData);
    data_ptr3->tessellation_amount = tessellation_amount;
    data_ptr3->padding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    device_context->Unmap(tessellation_buffer_, 0);
    buffer_number = 0;
    device_context->HSSetConstantBuffers(buffer_number, 1, &tessellation_buffer_);

    return true;
}

void ColorShaderClass::Render_shader(ID3D11DeviceContext* device_context, int index_count)
{
    //vertex input layout
    device_context->IASetInputLayout(layout_);

    //vertex and pixel shader that ll be used to render 
    device_context->PSSetSamplers(0, 1, &sample_state_);
    device_context->DSSetSamplers(0, 1, &sample_state_);
    device_context->VSSetShader(vertex_shader_, nullptr, 0);
    device_context->HSSetShader(hull_shader_, nullptr, 0);
    device_context->DSSetShader(domain_shader_, nullptr, 0);
    device_context->PSSetShader(pixel_shader_, nullptr, 0);

    //render
    device_context->DrawIndexed(index_count, 0, 0);
}

