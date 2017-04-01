//=======
//GLOBALS
cbuffer matrix_buffer
{
    matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
    matrix light_view_matrix;
    matrix light_projection_matrix;
};

//========
//TypeDefs
struct VERTEX_INPUT_TYPE
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 color : COLOR;
    float2 tex2 : TEXCOORD1;
};

struct PIXEL_INPUT_TYPE
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 light_view_position : TEXCOORD1;
    float4 color : COLOR;
    float2 tex2 : TEXCOORD2;
    float4 depth_position : TEXCOORD3;
};

//============
//VertexShader
PIXEL_INPUT_TYPE Terrain_vertex_shader(VERTEX_INPUT_TYPE input)
{
    PIXEL_INPUT_TYPE output;
    float4 world_position;

    input.position.w = 1.0f;

    output.position = mul(input.position, world_matrix);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);

    output.light_view_position = mul(input.position, world_matrix);
    output.light_view_position = mul(output.light_view_position, light_view_matrix);
    output.light_view_position = mul(output.light_view_position, light_projection_matrix);
    
    output.tex = input.tex;
    output.tex2 = input.tex2;

    //calc the normal vec against world
    output.normal = mul(input.normal, (float3x3) world_matrix);

    //normalize normal vector
    output.normal = normalize(output.normal);    

    output.tangent = mul(input.tangent, (float3x3) world_matrix);
    output.tangent = normalize(output.tangent);

    output.binormal = mul(input.binormal, (float3x3) world_matrix);
    output.binormal = normalize(output.binormal);

    output.color = float4(input.color, 1.0f);

    output.depth_position = output.position;

    return output;
}