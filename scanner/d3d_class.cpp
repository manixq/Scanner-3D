#include "headers/d3d_class.h"

D3DClass::D3DClass()
{
 swap_chain_ = 0;
 device_ = 0;
 device_context_ = 0;
 render_target_view_ = 0;
 depth_stencil_buffer_ = 0;
 depth_stencil_state_ = 0;
 depth_stencil_view_ = 0;
 raster_state_ = 0;
 depth_disabled_stencil_state = 0;
 alpha_disable_blending_state_ = 0;
 alpha_enable_blending_state_ = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{
 
}

D3DClass::~D3DClass()
{
 
}

bool D3DClass::Initialize(int screen_width, int screen_height, bool vsync, HWND hwnd, bool fullscreen, float screen_depth, float screen_near)
{
 HRESULT result;
 IDXGIFactory* factory;
 IDXGIAdapter* adapter;
 IDXGIOutput* adapter_output;
 unsigned int num_modes, i, numerator, denominator;
 DXGI_MODE_DESC* display_mode_list;
 DXGI_SWAP_CHAIN_DESC swap_chain_desc;
 D3D_FEATURE_LEVEL feature_level;
 ID3D11Texture2D* back_buffer_ptr;
 D3D11_TEXTURE2D_DESC depth_buffer_desc;
 D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
 D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
 D3D11_RASTERIZER_DESC raster_desc;
 float field_of_view, screen_aspect;
 D3D11_BLEND_DESC blend_state_desc;
 D3D11_DEPTH_STENCIL_DESC depth_disabled_stencil_desc;

 //vsync settings
 vsync_enabled_ = vsync;

 //graphics interface factory
 result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
 if (FAILED(result))
  return false;

 //use created factory
 result = factory->EnumAdapters(0, &adapter);
 if (FAILED(result))
  return false;

 result = adapter->EnumOutputs(0, &adapter_output);
 if (FAILED(result))
  return false;

 result = adapter_output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, nullptr);
 if (FAILED(result))
  return false;

 display_mode_list = new DXGI_MODE_DESC[num_modes];
 if (!display_mode_list)
  return false;

 result = adapter_output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_mode_list);
 if (FAILED(result))
  return false;

 //find display mode that match with chosen one
 for (i = 0; i < num_modes; i++)
 {
  if(display_mode_list[i].Width == (unsigned int)screen_width)
  {
   if (display_mode_list[i].Height == (unsigned int)screen_height)
   {
    numerator = display_mode_list[i].RefreshRate.Numerator;
    denominator = display_mode_list[i].RefreshRate.Denominator;
   }
  }
 }

 //release what is not needed
 delete [] display_mode_list;
 display_mode_list = nullptr;

 adapter_output->Release();
 adapter_output = nullptr;

 adapter->Release();
 adapter = nullptr;

 factory->Release();
 factory = nullptr;

 ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

 //set single back buffer
 swap_chain_desc.BufferCount = 1;

 //set the width and height of the back buffer
 swap_chain_desc.BufferDesc.Width = screen_width;
 swap_chain_desc.BufferDesc.Height = screen_height;

 //set 32bit surface for back buffer
 swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

 //set the refresh rate of the back buffer
 if(vsync_enabled_)
 {
  swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
  swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
 }
 else
 {
  swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
  swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
 }

 //set usage of back buffer
 swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

 //set the handle for window to render
 swap_chain_desc.OutputWindow = hwnd;

 //turn multisampling off
 swap_chain_desc.SampleDesc.Count = 1;
 swap_chain_desc.SampleDesc.Quality = 0;

 //Set to full or windowed mode
 if(fullscreen)
 {
  swap_chain_desc.Windowed = false;
 }
 else
 {
  swap_chain_desc.Windowed = true;
 }

 //scanline ordering and scaling
 swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
 swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

 //discard the back buffer contents
 swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

 //dont set the advanced flags
 swap_chain_desc.Flags = 0;

 //set the feature level to dx11
 feature_level = D3D_FEATURE_LEVEL_11_0;

 //creating swap chain, direct3d device and direct3d device context
 result = D3D11CreateDeviceAndSwapChain(
  nullptr,
  D3D_DRIVER_TYPE_HARDWARE,
  nullptr,
  0,
  &feature_level,
  1,
  D3D11_SDK_VERSION,
  &swap_chain_desc,
  &swap_chain_,
  &device_,
  nullptr,
  &device_context_);

 if (FAILED(result))
  return false;

 //choose correct gpu device
 //pointer to backbuffer
 result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
 if (FAILED(result))
  return false;

 //render target view with back buffer
 result = device_->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view_);
 if (FAILED(result))
  return false;

 //release back buffer ptr
 back_buffer_ptr->Release();
 back_buffer_ptr = nullptr;

 //depth buffer (my favourite)
 ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

 //description of or depth buffer
 depth_buffer_desc.Width = screen_width;
 depth_buffer_desc.Height = screen_height;
 depth_buffer_desc.MipLevels = 1;
 depth_buffer_desc.ArraySize = 1;
 depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
 depth_buffer_desc.SampleDesc.Count = 1;
 depth_buffer_desc.SampleDesc.Quality = 0;
 depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
 depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
 depth_buffer_desc.CPUAccessFlags = 0;
 depth_buffer_desc.MiscFlags = 0;

 //create texture for depth buffer
 result = device_->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_stencil_buffer_);
 if (FAILED(result))
  return false;

 //depth stencil setup
 ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

 depth_stencil_desc.DepthEnable = true;
 depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
 depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

 depth_stencil_desc.StencilEnable = true;
 depth_stencil_desc.StencilReadMask = 0xFF;
 depth_stencil_desc.StencilWriteMask = 0xFF;

 //stencil operations for front-facing pixels
 depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
 depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

 //stencil operations for back-facing
 depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
 depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

 //create depth stencil state
 result = device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_);
 if (FAILED(result))
  return false;

 //set stencil state
 device_context_->OMSetDepthStencilState(depth_stencil_state_, 1);

 //init depth stencil view
 ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

 //set description for depth stencil view
 depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
 depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
 depth_stencil_view_desc.Texture2D.MipSlice = 0;

 //create depth stencil view
 result = device_->CreateDepthStencilView(depth_stencil_buffer_, &depth_stencil_view_desc, &depth_stencil_view_);
 if (FAILED(result))
  return false;

 //bind the render target view and depth stencil buffer to the output render pipeline
 device_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);

 //determine how polygons ll be drawn
 raster_desc.AntialiasedLineEnable = true;
 raster_desc.CullMode = D3D11_CULL_BACK;
 raster_desc.DepthBias = 0;
 raster_desc.DepthBiasClamp = 0.0f;
 raster_desc.DepthClipEnable = true;
 raster_desc.FillMode = D3D11_FILL_SOLID;
 raster_desc.FrontCounterClockwise = false;
 raster_desc.MultisampleEnable = false;
 raster_desc.ScissorEnable = false;
 raster_desc.SlopeScaledDepthBias = 0.0f;

 //do it
 result = device_->CreateRasterizerState(&raster_desc, &raster_state_);
 if (FAILED(result))
  return false;

 //set it up
 device_context_->RSSetState(raster_state_);

 raster_desc.CullMode = D3D11_CULL_NONE;
 result = device_->CreateRasterizerState(&raster_desc, &raster_state_no_culling_);
 if (FAILED(result))
     return false;

 //set up view-port
 view_port_.Width = static_cast<float>(screen_width);
 view_port_.Height = static_cast<float>(screen_height);
 view_port_.MinDepth = 0.0f;
 view_port_.MaxDepth = 1.0f;
 view_port_.TopLeftX = 0.0f;
 view_port_.TopLeftY = 0.0f;

 //create view-port
 device_context_->RSSetViewports(1, &view_port_);

 //setup projection matrix
 field_of_view = static_cast<float>(D3DX_PI) / 4.0f;
 screen_aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);

 //create projection matrix
 D3DXMatrixPerspectiveFovLH(&projection_matrix_, field_of_view, screen_aspect, screen_near, screen_depth);

 //init world matrix to the identity matrix
 D3DXMatrixIdentity(&world_matrix_);

 //create ortho projection matrix for 2d rend
 D3DXMatrixOrthoLH(&ortho_matrix_, static_cast<float>(screen_width), static_cast<float>(screen_height), screen_near, screen_depth);
 
 ZeroMemory(&depth_disabled_stencil_desc, sizeof(depth_disabled_stencil_desc));

 depth_disabled_stencil_desc.DepthEnable = false;
 depth_disabled_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
 depth_disabled_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
 depth_disabled_stencil_desc.StencilEnable = true;
 depth_disabled_stencil_desc.StencilReadMask = 0xFF;
 depth_disabled_stencil_desc.StencilWriteMask = 0xFF;
 depth_disabled_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 depth_disabled_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
 depth_disabled_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 depth_disabled_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
 depth_disabled_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 depth_disabled_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
 depth_disabled_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 depth_disabled_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

 result = device_->CreateDepthStencilState(&depth_disabled_stencil_desc, &depth_disabled_stencil_state);
 if (FAILED(result))
  return false;

 ZeroMemory(&blend_state_desc, sizeof(D3D11_BLEND_DESC));
 blend_state_desc.RenderTarget[0].BlendEnable = true;
 blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
 blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
 blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
 blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
 blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
 blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
 blend_state_desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
 result = device_->CreateBlendState(&blend_state_desc, &alpha_enable_blending_state_);
 if (FAILED(result))
  return false;

 blend_state_desc.RenderTarget[0].BlendEnable = FALSE;
 blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
 result = device_->CreateBlendState(&blend_state_desc, &alpha_disable_blending_state_);
 if (FAILED(result))
  return false;

 return true;
}

void D3DClass::Shutdown()
{
 if(alpha_enable_blending_state_)
 {
  alpha_enable_blending_state_->Release();
  alpha_enable_blending_state_ = nullptr;
 }

 if(alpha_disable_blending_state_)
 {
  alpha_disable_blending_state_->Release();
  alpha_disable_blending_state_ = nullptr;
 }

 if(depth_disabled_stencil_state)
 {
  depth_disabled_stencil_state->Release();
  depth_disabled_stencil_state = nullptr;
 }

 if(swap_chain_)
  swap_chain_->SetFullscreenState(false, nullptr);
 
 if(raster_state_)
 {
  raster_state_->Release();
  raster_state_ = nullptr;
 }

 if (depth_stencil_view_)
 {
  depth_stencil_view_->Release();
  depth_stencil_view_ = nullptr;
 }

 if(depth_stencil_state_)
 {
  depth_stencil_state_->Release();
  depth_stencil_state_ = nullptr;
 }

 if(depth_stencil_buffer_)
 {
  depth_stencil_buffer_->Release();
  depth_stencil_buffer_ = nullptr;
 }

 if(raster_state_no_culling_)
 {
     raster_state_no_culling_->Release();
     raster_state_no_culling_ = nullptr;
 }

 if(render_target_view_)
 {
  render_target_view_->Release();
  render_target_view_ = nullptr;
 }

 if(device_context_)
 {
  device_context_->Release();
  device_context_ = nullptr;
 }

 if(device_)
 {
  device_->Release();
  device_ = nullptr;
 }

 if(swap_chain_)
 {
  swap_chain_->Release();
  swap_chain_ = nullptr;
 }
}


void D3DClass::Begin_scene(float red, float green, float blue, float alpha)
{
 float color[4] = {red, green, blue, alpha};
 
 //clear back buffer
 device_context_->ClearRenderTargetView(render_target_view_, color);
 
 //clear depth buffer
 device_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void D3DClass::End_scene()
{
 if (vsync_enabled_)
  //lock to screen refresh
  swap_chain_->Present(1, 0);
 else
  //as fast as possible
  swap_chain_->Present(0, 0);
}

ID3D11Device* D3DClass::GetDevice()
{
 return device_;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
 return device_context_;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projection_matrix)
{
 projection_matrix = projection_matrix_;
}

void D3DClass::GetWorldMatrix(D3DMATRIX& world_matrix)
{
 world_matrix = world_matrix_;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& ortho_matrix)
{
 ortho_matrix = ortho_matrix_;
}

void D3DClass::Turn_zbuffer_on()
{
 device_context_->OMSetDepthStencilState(depth_stencil_state_, 1);
}

void D3DClass::Turn_zbuffer_off()
{
 device_context_->OMSetDepthStencilState(depth_disabled_stencil_state, 1);
}

void D3DClass::Turn_culling_on()
{
    device_context_->RSSetState(raster_state_);
}

void D3DClass::Turn_culling_off()
{
    device_context_->RSSetState(raster_state_no_culling_);
}

ID3D11DepthStencilView* D3DClass::Get_depth_stencil_view()
{
 return depth_stencil_view_;
}

void D3DClass::Set_back_buffer_render_target()
{
 device_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);
}

void D3DClass::Reset_viewport()
{
    device_context_->RSSetViewports(1, &view_port_);
}

void D3DClass::TurnOffAlphaBlending()
{
 float blend_factor[4];

 blend_factor[0] = 0.0f;
 blend_factor[1] = 0.0f;
 blend_factor[2] = 0.0f;
 blend_factor[3] = 0.0f;

 device_context_->OMSetBlendState(alpha_disable_blending_state_, blend_factor, 0xffffffff);
}

void D3DClass::TurnOnAlphaBlending()
{
 float blend_factor[4];

 blend_factor[0] = 0.0f;
 blend_factor[1] = 0.0f;
 blend_factor[2] = 0.0f;
 blend_factor[3] = 0.0f;
 
 device_context_->OMSetBlendState(alpha_enable_blending_state_, blend_factor, 0xffffffff);
}