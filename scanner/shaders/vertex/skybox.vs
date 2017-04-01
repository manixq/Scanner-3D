cbuffer MatrixBuffer
{
    matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
};

//========
//Typedefs
struct VERTEX_INPUT_TYPE
{
     float4 position : POSITION;
     float2 tex : TEXCOORD0;
};

struct PIXEL_INPUT_TYPE
{
     float4 position : SV_POSITION;
     float2 tex : TEXCOORD0;    
     float4 skybox_position : TEXCOORD1;
};


//=============
//Vertex Shader

PIXEL_INPUT_TYPE Skybox_vertex_shader(VERTEX_INPUT_TYPE input)
{
    PIXEL_INPUT_TYPE output;

    input.position.w = 1.0f;

    output.position = mul(input.position, world_matrix);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);

    //store the input color for the pixel shader to use
    output.tex = input.tex;
    output.skybox_position = input.position;
    return output;
}