#ifndef _TERRAIN_CLASS_H_
#define _TERRAIN_CLASS_H_

//========
//INCLUDES
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <stdio.h>
#include "headers/terrain_cell_class.h"
#include "headers/frustum_class.h"

using namespace DirectX;
using namespace std;

//============
//TerrainClass
class TerrainClass
{
    struct VERTEX_TYPE
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
        XMFLOAT3 color;
    };

    struct HEIGHT_MAP_TYPE
    {
        float x, y, z;
        float nx, ny, nz;
        float r, g, b;
    };

    struct MODEL_TYPE
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float r, g, b;
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
    TerrainClass();
    TerrainClass(const TerrainClass&);
    ~TerrainClass();

    bool Initialize(ID3D11Device*, char*);
    void Shutdown();

    void Frame();

    bool Render_cell(ID3D11DeviceContext*, int, FrustumClass*);
    void Render_cell_lines(ID3D11DeviceContext*, int);

    int Get_cell_index_count(int);
    int Get_cell_lines_index_count(int);
    int Get_cell_count();

    int Get_render_count();
    int Get_cells_drawn();
    int Get_cells_culled();

    bool Get_height_at_position(float, float, float&);

private:
    bool Load_setup_file(char*);
    bool Load_bitmap_height_map();
    bool Load_raw_height_map();
    void Shutdown_height_map();
    void Set_terrain_coordinates();
    bool Calculate_normals();
    bool Load_color_map();
    bool Build_terrain_model();
    void Shutdown_terrain_model();

    bool Load_terrain_cells(ID3D11Device*);
    void Shutdown_terrain_cells();

    void Calculate_terrain_vectors();
    void Calculate_tangent_binormal(TEMP_VERTEX_TYPE, TEMP_VERTEX_TYPE, TEMP_VERTEX_TYPE, VECTOR_TYPE&, VECTOR_TYPE&);
    
    bool Check_height_of_triangle(float, float, float&, float[3], float[3], float[3]);

    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;

    int terrain_height_, terrain_width_;
    float height_scale_;
    char *terrain_filename_, *color_map_filename_;
    HEIGHT_MAP_TYPE* height_map_;
    MODEL_TYPE* terrain_model_;
    TerrainCellClass* terrain_cells_;
    int cell_count_, render_count_, cells_drawn_, cells_culled_;
};
#endif
