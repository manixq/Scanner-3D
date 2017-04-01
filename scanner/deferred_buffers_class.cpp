#include "headers/deferred_buffers_class.h"

DeferredBuffersClass::DeferredBuffersClass()
{
    int i;
    for (i = 0; i < BUFFER_COUNT;i++)
    {
        render_target_texture_array_[i] = nullptr;
        render_target_view_array_[i] = nullptr;
        shader_resource_view_array_[i] = nullptr;
    }
    depth_stencil_buffer_ = nullptr;
    depth_stencil_view_ = nullptr;
}

DeferredBuffersClass::DeferredBuffersClass(const DeferredBuffersClass&)
{
    
}

DeferredBuffersClass::~DeferredBuffersClass()
{
    
}

bool DeferredBuffersClass::Initialize(ID3D11Device* device, int texture_width, int texture_height, float screen_depth, float screen_near)
{
    D3D11_TEXTURE2D_DESC texture_desc;
    HRESULT result;
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
    int i;

    texture_width_ = texture_width;
    texture_height_ = texture_height;

    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = texture_width;
    texture_desc.Height = texture_height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    for (i = 0; i < BUFFER_COUNT; i++)
    {
        result = device->CreateTexture2D(&texture_desc, nullptr, &render_target_texture_array_[i]);
        if (FAILED(result))
            return false;
    }

    render_target_view_desc.Format = texture_desc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    render_target_view_desc.Texture2D.MipSlice = 0;

    for (i = 0; i < BUFFER_COUNT; i++)
    {
        result = device->CreateRenderTargetView(render_target_texture_array_[i], &render_target_view_desc, &render_target_view_array_[i]);
        if (FAILED(result))
            return false;
    }

    shader_resource_view_desc.Format = texture_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    for (i = 0; i < BUFFER_COUNT; i++)
    {
        result = device->CreateShaderResourceView(render_target_texture_array_[i], &shader_resource_view_desc, &shader_resource_view_array_[i]);
        if (FAILED(result))
            return false;
    }

    ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
    depth_buffer_desc.Width = texture_width;
    depth_buffer_desc.Height = texture_height;
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.SampleDesc.Count = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0;
    depth_buffer_desc.MiscFlags = 0;

    result = device->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_stencil_buffer_);
    if (FAILED(result))
        return false;

    ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

    result = device->CreateDepthStencilView(depth_stencil_buffer_, &depth_stencil_view_desc, &depth_stencil_view_);
    if (FAILED(result))
        return false;

    view_port_.Width = (float)texture_width;
    view_port_.Height = (float)texture_height;
    view_port_.MinDepth = 0.0f;
    view_port_.MaxDepth = 1.0f;
    view_port_.TopLeftX = 0.0f;
    view_port_.TopLeftY = 0.0f;

    return true;
}

void DeferredBuffersClass::Shutdown()
{
    int i;

    if(depth_stencil_view_)
    {
        depth_stencil_view_->Release();
        depth_stencil_view_ = nullptr;
    }

    if(depth_stencil_buffer_)
    {
        depth_stencil_buffer_->Release();
        depth_stencil_buffer_ = nullptr;
    }

    for (i = 0; i < BUFFER_COUNT; i++)
    {
        if(shader_resource_view_array_[i])
        {
            shader_resource_view_array_[i]->Release();
            shader_resource_view_array_[i] = nullptr;
        }

        if(render_target_view_array_[i])
        {
            render_target_view_array_[i]->Release();
            render_target_view_array_[i] = nullptr;
        }

        if(render_target_texture_array_[i])
        {
            render_target_texture_array_[i]->Release();
            render_target_texture_array_[i] = nullptr;
        }
    }
}

void DeferredBuffersClass::Set_render_targets(ID3D11DeviceContext* device_context)
{
    device_context->OMSetRenderTargets(BUFFER_COUNT, render_target_view_array_, depth_stencil_view_);
    device_context->RSSetViewports(1, &view_port_);
}

void DeferredBuffersClass::Clear_render_targets(ID3D11DeviceContext* device_context, float red, float green, float blue, float alpha)
{
    float color[4];
    int i;

    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    for (i = 0; i < BUFFER_COUNT; i++)
        device_context->ClearRenderTargetView(render_target_view_array_[i], color);

    device_context->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* DeferredBuffersClass::Get_shader_resource_view(int view)
{
    return shader_resource_view_array_[view];
}
