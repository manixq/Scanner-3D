#include "headers/shader_manager_class.h"

ShaderManagerClass::ShaderManagerClass()
{
    font_shader_ = nullptr;
    terrain_shader_ = nullptr;
    texture_shader_ = nullptr;
    skybox_shader_ = nullptr;
    color_shader_ = nullptr;
}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass&)
{
    
}

ShaderManagerClass::~ShaderManagerClass()
{
    
}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;

    //Color Shader
    color_shader_ = new ColorShaderClass;
    if (!color_shader_)
        return false;
    result = color_shader_->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize color shader.", L"Error", MB_OK);
        return false;
    }

    //Skybox Shader
    skybox_shader_ = new SkyboxShaderClass;
    if (!skybox_shader_)
        return false;
    result = skybox_shader_->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize skybox shader.", L"Error", MB_OK);
        return false;
    }

    //Font Shader
    font_shader_ = new FontShaderClass;
    if (!font_shader_)
        return false;
    result = font_shader_->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize font shader.", L"Error", MB_OK);
        return false;
    }


    //Terrain Shader
    terrain_shader_ = new TerrainShaderClass;
    if (!terrain_shader_)
        return false;
    result = terrain_shader_->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize terrain shader.", L"Error", MB_OK);
        return false;
    }

    //Texture Shader
    texture_shader_ = new TextureShaderClass;
    if (!texture_shader_)
        return false;
    result = texture_shader_->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize texture shader.", L"Error", MB_OK);
        return false;
    }
}

void ShaderManagerClass::Shutdown()
{
    if (color_shader_)
    {
        color_shader_->Shutdown();
        delete color_shader_;
        color_shader_ = nullptr;
    }

    if (skybox_shader_)
    {
        skybox_shader_->Shutdown();
        delete skybox_shader_;
        skybox_shader_ = nullptr;
    }

    if(font_shader_)
    {
        font_shader_->Shutdown();
        delete font_shader_;
        font_shader_ = nullptr;
    }
    
    if(terrain_shader_)
    {
        terrain_shader_->Shutdown();
        delete terrain_shader_;
        terrain_shader_ = nullptr;
    }

    if(texture_shader_)
    {
        texture_shader_->Shutdown();
        delete texture_shader_;
        texture_shader_ = nullptr;
    }

}


bool ShaderManagerClass::Render_font_shader(ID3D11DeviceContext* device, int index_count, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
    ID3D11ShaderResourceView* texture, D3DXVECTOR4 pixel_color)
{
    bool result;
    result = font_shader_->Render(device, index_count, world, view, projection, texture, pixel_color);
    if (!result)
        return false;
    return true;
}


bool ShaderManagerClass::Render_terrain_shader(ID3D11DeviceContext* device, int index_count, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
    D3DXMATRIX light_view, D3DXMATRIX light_projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depth_map_texture, ID3D11ShaderResourceView* normal_map,
    ID3D11ShaderResourceView* normal_map2, D3DXVECTOR3 light_direction, D3DXVECTOR4 ambient_color, D3DXVECTOR4 diffuse_color)
{
    bool result;
    result = terrain_shader_->Render(device, index_count, world, view, projection, light_view, light_projection, texture, depth_map_texture, normal_map, normal_map2, light_direction,
        ambient_color, diffuse_color);
    if (!result)
        return false;
    return true;
}

bool ShaderManagerClass::Render_texture_shader(ID3D11DeviceContext* device, int index_count, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
    ID3D11ShaderResourceView* texture)
{
    bool result;
    result = texture_shader_->Render(device, index_count, world, view, projection, texture);
    if (!result)
        return false;
    return true;
}

bool ShaderManagerClass::Render_color_shader(ID3D11DeviceContext* device, int index_count, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, float tessellation_amount, ID3D11ShaderResourceView* disp, ID3D11ShaderResourceView* normal, D3DXVECTOR3 light_direction)
{
    bool result;
    result = color_shader_->Render(device, index_count, world, view, projection, tessellation_amount, disp, normal, light_direction);
    if (!result)
        return false;
    return true;
}

bool ShaderManagerClass::Render_skybox_shader(ID3D11DeviceContext* device, int index_count, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
    ID3D11ShaderResourceView* perturb, ID3D11ShaderResourceView* clouds, float translation, float scale, float brightness, XMFLOAT4 apex_color, XMFLOAT4 center_color)
{
    bool result;
    result = skybox_shader_->Render(device, index_count, world, view, projection, perturb, clouds, translation, scale, brightness, apex_color, center_color);
    if (!result)
        return false;
    return true;
}
