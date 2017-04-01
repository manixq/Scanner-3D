#ifndef _SKYBOX_CLASS_H_
#define _SKYBOX_CLASS_H_


//========
//Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

//===========
//SkyBoxClass
class SkyBoxClass
{
    struct MODEL_TYPE
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    struct VERTEX_TYPE
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    SkyBoxClass();
    SkyBoxClass(const SkyBoxClass&);
    ~SkyBoxClass();

    bool Initialize(ID3D11Device*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int Get_index_count();
    XMFLOAT4 Get_apex_color();
    XMFLOAT4 Get_center_color();
    float Get_scale();
    float Get_translation();
    float Get_brightness();


private:
    bool Load_skybox_model(char*);
    void Release_skybox_model();

    bool Initialize_buffers(ID3D11Device*);
    void Release_buffers();
    void Render_buffers(ID3D11DeviceContext*);

    MODEL_TYPE* model_;
    int vertex_count_, index_count_;
    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    XMFLOAT4 apex_color_, center_color_;
    float scale_, brightness_, translation_;
};
#endif

