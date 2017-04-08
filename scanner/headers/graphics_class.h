#ifndef _GRAPHICS_CLASS_H_
#define _GRAPHICS_CLASS_H_

//========
//includes
#include "headers/d3d_class.h"
#include "headers/camera_class.h"
#include "headers/frustum_class.h"
#include "headers/light_class.h"
#include "headers/debug_window_class.h"
#include "headers/render_texture_class.h"
#include "headers/ortho_window_class.h"
#include "headers/bitmap_class.h"
#include "headers/text_class.h"
#include "headers/terrain_class.h"
#include "headers/texture_manager_class.h"
#include "headers/shader_manager_class.h"
#include "headers/skybox_class.h"
#include "headers/deferred_buffers_class.h"
#include "headers/depth_fusion_class.h"

//=======
//globals
static bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 500.0f;
const float SCREEN_NEAR = 0.1f;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f; 
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

//==============
//GraphicsClass
class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame(float, float, float, float, float);
    bool Render();

private:
    bool Render_scene();
    bool Render2d_texture_scene();

    D3DClass* d3d_;
    CameraClass* camera_;
    DebugWindowClass* debug_window_;
    OrthoWindowClass *small_window_, *full_sceen_window_;
    FrustumClass* frustum_;
    LightClass* light_;
    SkyBoxClass* skybox_; 
    DeferredBuffersClass* deferred_buffers_;
    DepthFusionClass* object_2d3d_;

    TerrainClass* terrain_;
    TextClass* text_;

    TextureManagerClass* texture_manager_;
    ShaderManagerClass* shader_manager_;

    RenderTextureClass *render_texture_;
    RenderTextureClass *shadow_texture_;   

    BitmapClass* mouse_;
    bool begin_check_;
    int mouse_x_ = 0;
    int mouse_y_ = 0;

    float water_height_, water_translation_;
    float rotation_x_ = 0.0f;
    float rotation_y_ = 0.0f;
    float frame_time_ = 0.0f;

    float screen_width_;
    float screen_height_;

    bool fire_on_ = false;
};
#endif
