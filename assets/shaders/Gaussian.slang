struct VSInput
{
    uint vertexID : SV_VertexID;
    uint instanceID : SV_InstanceID;
    float3 position : POSITION;
}

struct VSOutput
{
    float4 position : SV_Position;
};

static float3 TRIANGLE_VERTICES[3] = {
    float3(0.00f, -0.05f, 0.0f),
    float3(0.05f, 0.05f, 0.0f),
    float3(-0.05f, 0.05f, 0.0f),
};

[shader("vertex")]
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = float4(TRIANGLE_VERTICES[input.vertexID], 1.0f);
    output.position.xyz += input.position;
    return output;
}

struct Fragment
{
    float4 color;
};

[shader("fragment")]
Fragment PSMain() : SV_Target0
{
    Fragment output;
    output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}