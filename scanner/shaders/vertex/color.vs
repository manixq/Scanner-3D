//========
//Typedefs
struct VERTEX_INPUT_TYPE
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

struct HULL_INPUT_TYPE
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};


//=============
//Vertex Shader
HULL_INPUT_TYPE Color_vertex_shader(VERTEX_INPUT_TYPE input)
{
    HULL_INPUT_TYPE output;

    output.position = float4(input.position, 1.0f);
    
    output.color = input.color;
    output.tex = input.tex;
    output.tex2 = input.tex2;
    
    output.normal = input.normal;

    output.tangent = input.tangent;

    output.binormal = input.binormal;

    return output;
}