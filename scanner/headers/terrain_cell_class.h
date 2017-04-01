#ifndef _TERRAIN_CELL_CLASS_H_
#define _TERRAIN_CELL_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class TerrainCellClass
{
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

    struct VERTEX_TYPE
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
        XMFLOAT3 color;
        XMFLOAT2 texture2;
    };

    struct VECTOR_TYPE
    {
        float x, y, z;
    };

    struct COLOR_VERTEX_TYPE
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

public:
    TerrainCellClass();
    TerrainCellClass(const TerrainCellClass&);
    ~TerrainCellClass();

    bool Initialize(ID3D11Device*, void*, int, int, int, int, int);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    void Render_line_buffers(ID3D11DeviceContext*);

    int Get_vertex_count();
    int Get_index_count();
    int Get_line_buffers_index_count();
    void Get_cell_dimensions(float&, float&, float&, float&, float&, float&);

    VECTOR_TYPE* vertex_list;

private:
    bool Initialize_buffers(ID3D11Device*, int, int, int, int, int, MODEL_TYPE*);
    void Shutdown_buffers();
    void Render_buffers(ID3D11DeviceContext*);
    void Calculate_cell_dimensions();
    bool Build_line_buffers(ID3D11Device*);
    void Shutdown_line_buffers();

    int vertex_count_, index_count_, line_index_count_;
    ID3D11Buffer* vertex_buffer_, *index_buffer_, *line_vertex_buffer, *line_index_buffer;
    float max_width_, max_height_, max_depth_, min_width_, min_height_, min_depth_;
    float position_x_, position_y_, position_z_;

};
#endif
