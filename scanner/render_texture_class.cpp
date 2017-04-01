#include "headers/render_texture_class.h"

RenderTextureClass::RenderTextureClass()
{
 render_target_texture_ = nullptr;
 render_target_view_ = nullptr;
 shader_resource_view_ = nullptr;
 depth_stencil_buffer_ = nullptr;
 depth_stencil_view_ = nullptr;
}

RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
 
}

RenderTextureClass::~RenderTextureClass()
{
 
}

bool RenderTextureClass::Initialize(ID3D11Device* device, int texture_width, int texture_height)
{
 D3D11_TEXTURE2D_DESC texture_desc;
 HRESULT result;
 D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
 D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
 D3D11_TEXTURE2D_DESC depth_buffer_desc;
 D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;

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

 result = device->CreateTexture2D(&texture_desc, nullptr, &render_target_texture_);
 if (FAILED(result))
  return false;

 render_target_view_desc.Format = texture_desc.Format;
 render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
 render_target_view_desc.Texture2D.MipSlice = 0;

 result = device->CreateRenderTargetView(render_target_texture_, &render_target_view_desc, &render_target_view_);
 if (FAILED(result))
  return false;

 shader_resource_view_desc.Format = texture_desc.Format;
 shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
 shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
 shader_resource_view_desc.Texture2D.MipLevels = 1;

 result = device->CreateShaderResourceView(render_target_texture_, &shader_resource_view_desc, &shader_resource_view_);
 if (FAILED(result))
  return false;

 return true;
}

bool RenderTextureClass::Initialize(ID3D11Device* device, int texture_width, int texture_height, float screen_depth, float screen_near)
{
    D3D11_TEXTURE2D_DESC texture_desc;
    HRESULT result;
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc_;

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

    result = device->CreateTexture2D(&texture_desc, nullptr, &render_target_texture_);
    if (FAILED(result))
        return false;

    render_target_view_desc.Format = texture_desc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    render_target_view_desc.Texture2D.MipSlice = 0;

    result = device->CreateRenderTargetView(render_target_texture_, &render_target_view_desc, &render_target_view_);
    if (FAILED(result))
        return false;

    shader_resource_view_desc.Format = texture_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    result = device->CreateShaderResourceView(render_target_texture_, &shader_resource_view_desc, &shader_resource_view_);
    if (FAILED(result))
        return false;

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

    ZeroMemory(&depth_stencil_view_desc_, sizeof(depth_stencil_view_desc_));

    depth_stencil_view_desc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc_.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc_.Texture2D.MipSlice = 0;

    result = device->CreateDepthStencilView(depth_stencil_buffer_, &depth_stencil_view_desc_, &depth_stencil_view_);
    if (FAILED(result))
        return false;

    viewport_.Width = (float)texture_width;
    viewport_.Height = (float)texture_height;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;

    D3DXMatrixPerspectiveFovLH(&projection_matrix_, (static_cast<float>(D3DX_PI) / 4.0f), (static_cast<float>(texture_width) / static_cast<float>(texture_height)), screen_near, screen_depth);
    D3DXMatrixOrthoLH(&ortho_matrix_, (float)texture_width, (float)texture_height, screen_near, screen_depth);
    
    return true;
}

void RenderTextureClass::Shutdown()
{
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

    if(shader_resource_view_)
    {
        shader_resource_view_->Release();
        shader_resource_view_ = nullptr;
    }

    if(render_target_view_)
    {
        render_target_view_->Release();
        render_target_view_ = nullptr;
    }

    if(render_target_texture_)
    {
        render_target_texture_->Release();
        render_target_texture_ = nullptr;
    }
}

void RenderTextureClass::Set_render_target(ID3D11DeviceContext* device_context)
{
 device_context->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);
 device_context->RSSetViewports(1, &viewport_);
}

void RenderTextureClass::Clear_render_target(ID3D11DeviceContext* device_context, float red, float green, float blue, float alpha)
{
 float color[4];

 color[0] = red;
 color[1] = green;
 color[2] = blue;
 color[3] = alpha;

 device_context->ClearRenderTargetView(render_target_view_, color);
 device_context->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTextureClass::Get_shader_resource_view()
{
 return shader_resource_view_;
}

void RenderTextureClass::Get_projection_matrix(D3DXMATRIX& projection_matrix)
{
    projection_matrix = projection_matrix_;
}

void RenderTextureClass::Get_ortho_matrix(D3DXMATRIX& ortho_matrix)
{
    ortho_matrix = ortho_matrix_;
}

int RenderTextureClass::Get_texture_width()
{
    return texture_width_;
}

int RenderTextureClass::Get_texture_height()
{
    return texture_height_;
}
