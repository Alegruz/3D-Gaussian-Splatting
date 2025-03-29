// Stage Structs
struct VSInput
{
    uint VertexId : SV_VertexID;
    uint InstanceId : SV_InstanceID;
    float3 Position : POSITION;
    float3 Translate : TRANSLATE;
}

struct VSOutput
{
    float4 Position : SV_Position;
    uint InstanceId : SV_InstanceID;
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

[shader("vertex")]
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    // output.Position = float4(TRIANGLE_VERTICES[input.VertexId], 1.0f);
    output.Position = float4(input.Position, 1.0f);
    output.Position.xyz += input.Translate;
    output.Position = mul(output.Position, CameraInfo.View);
    output.Position = mul(output.Position, CameraInfo.Projection);
    output.InstanceId = input.InstanceId;
    return output;
}

struct Fragment
{
    float4 color;
};

typedef VSOutput PSInput;

static const float4 COLORS[8] = {
    float4(1.0f, 0.0f, 0.0f, 1.0f), // Red
    float4(0.0f, 1.0f, 0.0f, 1.0f), // Green
    float4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
    float4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow
    float4(1.0f, 0.0f, 1.0f, 1.0f), // Magenta
    float4(0.0f, 1.0f, 1.0f, 1.0f), // Cyan
    float4(0.5f, 0.5f, 0.5f, 1.0f), // Gray
    float4(1.0f, 0.5f, 0.0f, 1.0f)  // Orange
};

[shader("fragment")]
Fragment PSMain(PSInput input) : SV_Target0
{
    Fragment output;
    
    output.color = COLORS[input.InstanceId % 8];
    return output;
}