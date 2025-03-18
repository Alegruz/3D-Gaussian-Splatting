struct VSOutput
{
    float4 position : SV_Position;
};

static float3 TRIANGLE_VERTICES[3] = {
    float3(0.0f, 0.5f, 0.0f),
    float3(0.5f, -0.5f, 0.0f),
    float3(-0.5f, -0.5f, 0.0f)
};

[shader("vertex")]
VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.position = float4(TRIANGLE_VERTICES[vertexID], 1.0f);
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