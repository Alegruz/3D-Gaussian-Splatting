// Stage Structs
struct VSInput
{
    uint VertexId : SV_VertexID;
    uint InstanceId : SV_InstanceID;
    float3 Position : POSITION;
}

struct VSOutput
{
    float4 Position : SV_Position;
};

// Constant Buffers
struct ViewProjection
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer CameraBuffer
{
    ViewProjection CameraInfo;
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
    output.Position = float4(TRIANGLE_VERTICES[input.VertexId], 1.0f);
    output.Position.xyz += input.Position;
    output.Position = mul(output.Position, CameraInfo.View);
    output.Position = mul(output.Position, CameraInfo.Projection);
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