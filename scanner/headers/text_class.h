#ifndef  _TEXT_CLASS_H_
#define  _TEXT_CLASS_H_

//========
//INCLUDES
#include "headers/font_class.h"
#include "headers/font_shader_class.h"

class TextClass
{
    struct SENTENCE_TYPE
    {
        ID3D11Buffer *vertex_buffer_, *index_buffer_;
        int vertex_count, index_count, max_length;
        float red, green, blue;
    };

    struct VERTEX_TYPE
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    TextClass();
    TextClass(const TextClass&);
    ~TextClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);
    bool Set_fps(int, ID3D11DeviceContext*);

private:
    bool Initialize_sentence(SENTENCE_TYPE**, int, ID3D11Device*);
    bool Update_sentence(SENTENCE_TYPE*, char*, int, int, float, float, float, ID3D11DeviceContext*);
    void Release_sentence(SENTENCE_TYPE**);
    bool Render_sentence(ID3D11DeviceContext*, SENTENCE_TYPE*, D3DXMATRIX, D3DXMATRIX);

    FontClass* font_;
    FontShaderClass* font_shader_;
    int screen_width_, screen_height_;
    D3DXMATRIX base_view_matrix_;

    SENTENCE_TYPE* sentence1_;
    SENTENCE_TYPE* sentence2_;
};
#endif
