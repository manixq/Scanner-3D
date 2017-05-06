//=======
//Globals

cbuffer Tessellation_buffer
{
    float tessellation_amount;
    float3 padding;
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

struct CONSTANT_OUTPUT_TYPE
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HULL_OUTPUT_TYPE
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

CONSTANT_OUTPUT_TYPE ColorPatchConstantFunction(InputPatch<HULL_INPUT_TYPE, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
    CONSTANT_OUTPUT_TYPE output;

    output.edges[0] = tessellation_amount;
    output.edges[1] = tessellation_amount;
    output.edges[2] = tessellation_amount;

    output.inside = tessellation_amount;
    return output;
}

//===========
//hull shader
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HULL_OUTPUT_TYPE ColorHullShader(InputPatch<HULL_INPUT_TYPE, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HULL_OUTPUT_TYPE output;
    output.position = patch[pointId].position;
    output.color = patch[pointId].color;
    output.tex = patch[pointId].tex;
    output.normal = patch[pointId].normal;
    output.tangent = patch[pointId].tangent;
    output.binormal = patch[patchId].binormal;
    output.tex2 = patch[pointId].tex2;

    return output;
}