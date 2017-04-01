#ifndef  _DEBUG_WINDOW_CLASS_H_
#define  _DEBUG_WINDOW_CLASS_H_

//========
//Includes
#include <d3d11.h>
#include <D3DX10math.h>

//===========
//DebugWindowClass
class DebugWindowClass
{
public: 

 DebugWindowClass();
 DebugWindowClass(const DebugWindowClass&);
 ~DebugWindowClass();

 bool Initialize(ID3D11Device*, int, int, int, int);
 void Shutdown();
 bool Render(ID3D11DeviceContext*, int, int);
 int Get_index_count();

private:

 struct VERTEX_TYPE
 {
  D3DXVECTOR3 position;
  D3DXVECTOR2 texture;
 };
 bool Initialize_buffers(ID3D11Device*);
 void Shutdown_buffers();
 bool Update_buffers(ID3D11DeviceContext*, int, int);
 void Render_buffers(ID3D11DeviceContext*);

 ID3D11Buffer *vertex_buffer_, *index_buffer_;
 int vertex_count_, index_count_; 
 int screen_width_, screen_height_;
 int bitmap_width_, bitmap_height_;
 int previous_posx_, previous_posy_;
};

#endif
