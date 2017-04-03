//========
//Typedefs
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VERTEX_INPUT_TYPE
{
     float4 position : POSITION;
     float3 color : COLOR;
};

struct PIXEL_INPUT_TYPE
{
     float4 position : SV_POSITION;
     float4 color : COLOR;
};


//=============
//Vertex Shader
PIXEL_INPUT_TYPE Color_vertex_shader(VERTEX_INPUT_TYPE input)
{
    PIXEL_INPUT_TYPE output;

    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.color = float4(input.color, 1.0f);
    
    return output;
}