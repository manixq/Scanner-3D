#include "headers/graphics_class.h"

GraphicsClass::GraphicsClass()
{
    d3d_ = nullptr;
    camera_ = nullptr;
    frustum_ = nullptr;
    light_ = nullptr;
    debug_window_ = nullptr;
    skybox_ = nullptr;
    deferred_buffers_ = nullptr;
    
    render_texture_ = nullptr;  

    small_window_ = nullptr;
    full_sceen_window_ = nullptr;

    mouse_ = nullptr;

    text_ = nullptr;
    texture_manager_ = nullptr;
    shader_manager_ = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
 
}

GraphicsClass::~GraphicsClass()
{
 
}

bool GraphicsClass::Initialize(int screen_width, int screen_height, HWND hwnd)
{
    bool result;
    D3DXMATRIX static_view_matrix;
    int down_sample_width, down_sample_height;

    screen_height_ = screen_height;
    screen_width_ = screen_width;

    down_sample_width = screen_width / 2;
    down_sample_height = screen_height / 2;

    //create d3d object
    d3d_ = new D3DClass;
    if (!d3d_)
        return false;

    //init d3d object
    result = d3d_->Initialize(
        screen_width,
        screen_height,
        VSYNC_ENABLED,
        hwnd,
        FULL_SCREEN,
        SCREEN_DEPTH,
        SCREEN_NEAR);

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    deferred_buffers_ = new DeferredBuffersClass;
    if (!deferred_buffers_)
        return false;
    result = deferred_buffers_->Initialize(d3d_->GetDevice(), screen_width, screen_height, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
        return false;

    frustum_ = new FrustumClass;
    if (!frustum_)
        return false;

    shader_manager_ = new ShaderManagerClass;
    if (!shader_manager_)
        return false;
    result = shader_manager_->Initialize(d3d_->GetDevice(), hwnd);
    if (!result)
        return false;

    texture_manager_ = new TextureManagerClass;
    if (!texture_manager_)
        return false;
    result = texture_manager_->Initialize(6);
    if (!result)
        return false;
    texture_manager_->Load_texture(d3d_->GetDevice(), L"data/dirt01d.dds", 0);
    texture_manager_->Load_texture(d3d_->GetDevice(), L"data/dirt01n.dds", 1);
    texture_manager_->Load_texture(d3d_->GetDevice(), L"data/distance01n.dds", 2);
    texture_manager_->Load_texture(d3d_->GetDevice(), L"data/perturb001.dds", 3);
    texture_manager_->Load_texture(d3d_->GetDevice(), L"data/cloud001.dds", 4);
    texture_manager_->Load_texture(d3d_->GetDevice(), L"data/depth_fusion/normal.dds", 5);

    skybox_ = new SkyBoxClass;
    if (!skybox_)
        return false;
    result = skybox_->Initialize(d3d_->GetDevice());
    if (!result)
        return false;

    object_2d3d_ = new DepthFusionClass;
    if (!object_2d3d_)
        return false;
    result = object_2d3d_->Initialize(d3d_->GetDevice(), "data/depth_fusion/disp.bmp", "data/depth_fusion/view.bmp");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize object_2d3d.", L"Error", MB_OK);
        return false;
    }

    terrain_ = new TerrainClass;
    if (!terrain_)
        return false;
    result = terrain_->Initialize(d3d_->GetDevice(), "data/setup.txt");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize terrain object.", L"Error", MB_OK);
        return false;
    }
            
    //create camera object
    camera_ = new CameraClass;
    if (!camera_)
        return false;
    camera_->Set_position(0.0f, 0.0f, -10.0f);
    camera_->Render();
    camera_->Render_static();
    camera_->Get_view_matrix(static_view_matrix);

    text_ = new TextClass;
    if (!text_)
        return false;
    result = text_->Initialize(d3d_->GetDevice(), d3d_->GetDeviceContext(), hwnd, screen_width, screen_height, static_view_matrix);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize text object.", L"Error", MB_OK);
        return false;
    }

    
    shadow_texture_ = new RenderTextureClass;
    if (!shadow_texture_)
        return false;
    result = shadow_texture_->Initialize(d3d_->GetDevice(), screen_width, screen_height, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"could not init shadow texture", L"Error", MB_OK);
        return false;
    }

    render_texture_ = new RenderTextureClass;
    if (!render_texture_)
        return false;
    result = render_texture_->Initialize(d3d_->GetDevice(), screen_width, screen_height, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"could not init render texture", L"Error", MB_OK);
        return false;
    }
    

    small_window_ = new OrthoWindowClass;
    if (!small_window_)
        return false;
    result = small_window_->Initialize(d3d_->GetDevice(), down_sample_width, down_sample_height);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the small window object.", L"Error", MB_OK);
        return false;
    }

    full_sceen_window_ = new OrthoWindowClass;
    if (!full_sceen_window_)
        return false;
    result = full_sceen_window_->Initialize(d3d_->GetDevice(), screen_width, screen_height);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the fullscreen window object.", L"Error", MB_OK);
        return false;
    }

    mouse_ = new BitmapClass;
    if (!mouse_)
        return false;
    result = mouse_->Initialize(d3d_->GetDevice(), screen_width, screen_height, L"data/mouse.dds", 32, 32);
    if(!result)
    {
        MessageBox(hwnd, L"could not initialize mouse.", L"Error", MB_OK);
        return false;
    }
    begin_check_ = false;
    
    light_ = new LightClass;
    if (!light_)
        return false;
    light_->Set_ambient_color(0.35f, 0.35f, 0.35f, 1.0f);
    light_->Set_diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
    light_->Set_position(0.0f, 18.0f, 0.5f);
    light_->Set_direction(0.0f, 0.0f, 1.0f);
    light_->Set_specular_color(1.0f, 1.0f, 1.0f, 1.0f);
    light_->Set_specular_power(32.0f);
    light_->Set_look_at(0.0f, 5.0f, 0.0f);
    light_->Generate_ortho_matrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

    water_height_ = 2.75f;
    water_translation_ = 0.0f;


    debug_window_ = new DebugWindowClass;
    if (!debug_window_)
        return false;
    result = debug_window_->Initialize(d3d_->GetDevice(), screen_width, screen_height, 100, 100);
    if (!result)
    {
        MessageBox(hwnd, L"Could not init the debug window object", L"Error", MB_OK);
    }

    return true;
}

//kill all graphics objects
void GraphicsClass::Shutdown()
{
    if(object_2d3d_)
    {
        object_2d3d_->Shutdown();
        delete object_2d3d_;
        object_2d3d_ = nullptr;
    }

    if(deferred_buffers_)
    {
        deferred_buffers_->Shutdown();
        delete deferred_buffers_;
        deferred_buffers_ = nullptr;
    }

    if(skybox_)
    {
        skybox_->Shutdown();
        delete skybox_;
        skybox_ = nullptr;
    }

    if(shader_manager_)
    {
        shader_manager_->Shutdown();
        delete shader_manager_;
        shader_manager_ = nullptr;
    }
    
    if(text_)
    {
        text_->Shutdown();
        delete text_;
        text_ = nullptr;
    }

    if(mouse_)
    {
        mouse_->Shutdown();
        delete mouse_;
        mouse_ = nullptr;
    }

    if (full_sceen_window_)
    {
        full_sceen_window_->Shutdown();
        delete full_sceen_window_;
        full_sceen_window_ = nullptr;
    }

    if (small_window_)
    {
        small_window_->Shutdown();
        delete small_window_;
        small_window_ = nullptr;
    }
    
    if (render_texture_)
    {
        render_texture_->Shutdown();
        delete render_texture_;
        render_texture_ = nullptr;
    }
    

    if (debug_window_)
    {
        debug_window_->Shutdown();
        delete debug_window_;
        debug_window_ = nullptr;
    }


    if (terrain_)
    {
        terrain_->Shutdown();
        delete terrain_;
        terrain_ = nullptr;
    }
    
    if (light_)
    {
        delete light_;
        light_ = nullptr;
    }
    
    if (camera_)
    {
        delete camera_;
        camera_ = nullptr;
    }

    if (d3d_)
    {
        d3d_->Shutdown();
        delete d3d_;
        d3d_ = nullptr;
    }
}

bool GraphicsClass::Frame(float frame_time, float rotation_x, float rotation_y, float x_pos, float z_pos)
{
    static float light_angle = 270.0f;
    float radians;
    static float light_pos_x = 9.0f;
    static float frame_count = 0;
    static float frame_acum = 0;

    bool found_height;
    float height;

    frame_acum += frame_time;
    frame_count++;
    if (frame_acum >= 1000.0f)
    {
        text_->Set_fps(frame_count, d3d_->GetDeviceContext());
        frame_count = 0;
        frame_acum = 0;
    }

    light_pos_x -= 0.003f * frame_time / 10;
    light_angle -= 0.03f * frame_time / 10;
    if (light_angle <= 90.0f)
    {
        light_angle = 270.0f;
        light_pos_x = 9.0f;
    }
    
    radians = light_angle * 0.0174532925f;
    light_->Set_direction(sinf(radians), cosf(radians), 0.0f);
    light_->Set_position(light_pos_x, 15.0f, -0.1f);
    light_->Set_look_at(-light_pos_x, 0.0f, 0.0f);

    water_translation_ += 0.001f;
    if (water_translation_ > 1.0)
        water_translation_ -= 1.0f;

    rotation_x_ = rotation_x;
    rotation_y_ = rotation_y;
    

    frame_time_ += frame_time / 1000.0f;    
    if (frame_time_ >= 1000.0f)
        frame_time_ = 0.0f;      
    
    // terrain_->Frame();
    camera_->Set_position(x_pos, 0.0f, z_pos);
    camera_->Set_rotation(rotation_x, rotation_y, 0.0f);
    return true;
}

bool GraphicsClass::Render()
{
    bool result;   

    result = Render_scene();
    if (!result)
        return false;

    result = Render2d_texture_scene();
    if (!result)
        return false;

    return true;
}


bool GraphicsClass::Render_scene()
{
    D3DXMATRIX world_matrix, view_matrix, projection_matrix;
    D3DXMATRIX ortho_matrix, static_view_matrix, light_view_matrix, light_ortho_matrix;
    D3DXVECTOR3 camera_position, light_dir;
    bool result;   

    camera_->Render();
    deferred_buffers_->Set_render_targets(d3d_->GetDeviceContext());
    deferred_buffers_->Clear_render_targets(d3d_->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
    light_->Generate_view_matrix();
    camera_->Get_view_matrix(view_matrix);
    camera_->Get_static_view_matrix(static_view_matrix);
    d3d_->GetProjectionMatrix(projection_matrix);
    d3d_->GetOrthoMatrix(ortho_matrix);
    light_->Get_view_matrix(light_view_matrix);
    light_->Get_ortho_matrix(light_ortho_matrix);
    d3d_->GetWorldMatrix(world_matrix);

    camera_position = camera_->Get_position();
    D3DXMatrixTranslation(&world_matrix, camera_position.x, camera_position.y, camera_position.z);
    d3d_->Turn_culling_off();
    d3d_->Turn_zbuffer_off();

    //skybox
    light_dir = light_->Get_direction();
    skybox_->Render(d3d_->GetDeviceContext());
    result = shader_manager_->Render_skybox_shader(d3d_->GetDeviceContext(), skybox_->Get_index_count(), world_matrix, view_matrix, projection_matrix, texture_manager_->Get_texture(3), texture_manager_->Get_texture(4), frame_time_, skybox_->Get_scale(), -light_dir.y, skybox_->Get_apex_color(), skybox_->Get_center_color());
    if (!result)
        return false;
    d3d_->Turn_zbuffer_on();
    d3d_->Turn_culling_on();
    
    d3d_->GetWorldMatrix(world_matrix);
    camera_position.y += 50;
    camera_->Set_position(camera_position.x, camera_position.y, camera_position.z);
    camera_->Render();
    camera_->Get_view_matrix(view_matrix);
    frustum_->ConstructFrustrum(SCREEN_DEPTH, projection_matrix, view_matrix);
    //terrain
    for (int i = 0; i < terrain_->Get_cell_count(); i++)
    {
        result = terrain_->Render_cell(d3d_->GetDeviceContext(), i, frustum_);
        if (result)
        {
            result = shader_manager_->Render_terrain_shader(d3d_->GetDeviceContext(), terrain_->Get_cell_index_count(i), world_matrix, view_matrix, projection_matrix, light_view_matrix, light_ortho_matrix, texture_manager_->Get_texture(0), shadow_texture_->Get_shader_resource_view(), texture_manager_->Get_texture(1), texture_manager_->Get_texture(2), light_->Get_direction(), light_->Get_ambient_color(), light_->Get_diffuse_color());
            if (!result)
                return false;
        }
    }
    d3d_->GetWorldMatrix(world_matrix);

    //depth object
    d3d_->TurnOnAlphaBlending();
    object_2d3d_->Render(d3d_->GetDeviceContext());
    result = shader_manager_->Render_color_shader(d3d_->GetDeviceContext(), object_2d3d_->Get_index_count(), world_matrix, view_matrix, projection_matrix, texture_manager_->Get_texture(5), light_dir);
    if (!result)
        return false;
    d3d_->TurnOffAlphaBlending();
    d3d_->GetWorldMatrix(world_matrix);

    d3d_->Set_back_buffer_render_target();

    return true;
}

bool GraphicsClass::Render2d_texture_scene()
{
    D3DXMATRIX world_matrix, view_matrix, ortho_matrix;
    bool result;

    d3d_->Begin_scene(1.0f, 0.0f, 0.0f, 0.0f);
    camera_->Render();
    camera_->Get_static_view_matrix(view_matrix);
    d3d_->GetWorldMatrix(world_matrix);
    d3d_->GetOrthoMatrix(ortho_matrix);
    d3d_->Turn_zbuffer_off();

    full_sceen_window_->Render(d3d_->GetDeviceContext());    
    result = shader_manager_->Render_texture_shader(d3d_->GetDeviceContext(), full_sceen_window_->Get_index_count(), world_matrix, view_matrix, ortho_matrix, deferred_buffers_->Get_shader_resource_view(0));
    if (!result)
        return false;

    d3d_->Turn_zbuffer_on();
    d3d_->End_scene();
    return true;
}
