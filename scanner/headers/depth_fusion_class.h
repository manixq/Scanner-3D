#ifndef _DEPTH_FUSION_CLASS_H_
#define _DEPTH_FUSION_CLASS_H_

//========
//INCLUDES
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <stdio.h>

using namespace DirectX;
using namespace std;

//================
//DepthFusionClass
class DepthFusionClass
{
    struct VERTEX_TYPE
    {
        XMFLOAT3 position;
        XMFLOAT3 color;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
        XMFLOAT2 texture2;
    };


    struct DEPTH_MAP_TYPE
    {
        float x, y, z;
        float r, g, b;
        float nx, ny, nz;
    };

    struct MODEL_TYPE
    {
        float x, y, z;
        float r, g, b;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float tu2, tv2;
    };

    struct VECTOR_TYPE
    {
        float x, y, z;
    };

    struct TEMP_VERTEX_TYPE
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

public:
    DepthFusionClass();
    DepthFusionClass(const DepthFusionClass&);
    ~DepthFusionClass();

    bool Initialize(ID3D11Device*, char*, char*);
    void Shutdown();
    bool Render(ID3D11DeviceContext*);
    int Get_index_count();

private:
    bool Load_bitmap_depth_map();
    bool Load_color_map();
    void Shutdown_depth_map();
    void Set_coordinates();
    bool Calculate_normals();
    bool Build_model();
    void Shutdown_model();
    void Calculate_vectors();
    void Calculate_tangent_binormal(TEMP_VERTEX_TYPE, TEMP_VERTEX_TYPE, TEMP_VERTEX_TYPE, VECTOR_TYPE&, VECTOR_TYPE&);
    void Render_buffers(ID3D11DeviceContext*);
    bool Initialize_buffers(ID3D11Device*);
    void Shutdown_buffers();

    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;

    int model_height_, model_width_;
    float depth_scale_;
    char *depth_filename_, *color_map_filename_;
    DEPTH_MAP_TYPE* depth_map_;
    MODEL_TYPE* model_;
};

#endif
