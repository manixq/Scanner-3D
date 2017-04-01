//============
//Globals
cbuffer matrix_buffer
{
 matrix world_matrix;
 matrix view_matrix;
 matrix projection_matrix;
};

//========
//TypeDefs
struct VERTEX_INPUT_TYPE
{
 float4 position : POSITION;
 float2 tex : TEXCOORD0;
};

struct PIXEL_INPUT_TYPE
{
 float4 position : SV_POSITION;
 float2 tex : TEXCOORD0;
};

//=============
//Vertex shader
PIXEL_INPUT_TYPE Texture_vertex_shader(VERTEX_INPUT_TYPE input)
{
 PIXEL_INPUT_TYPE output;

 //for proper matrix calc
 input.position.w = 1.0f;

 //calc positions
 output.position = mul(input.position, world_matrix);
 output.position = mul(output.position, view_matrix);
 output.position = mul(output.position, projection_matrix);

 //store the txt coordinates for the pixel shader
 output.tex = input.tex;

 return output;
}