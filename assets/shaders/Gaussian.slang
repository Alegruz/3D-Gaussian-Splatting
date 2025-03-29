// Stage Structs
struct VSInput
{
    uint VertexId : SV_VertexID;
    uint InstanceId : SV_InstanceID;

    // Vertex
    float3 Position : POSITION;

    // Instance
    float3 Translate : TRANSLATE;
    float3 ScaleInLogScale : SCALE;
    float4 Quaternion : ROTATE;
    float4 ColorAsShDcComponentAndAlphaBeforeSigmoidActivision : COLOR;
}

struct VSOutput
{
    float4 Position : SV_Position;

    float4 ColorAsShDcComponentAndAlphaBeforeSigmoidActivision : COLOR;
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

    output.Position = float4(input.Position, 1.0f);
    output.Position.xyz *= exp(input.ScaleInLogScale);
    const float3 t = 2.0f * cross(input.Quaternion.xyz, output.Position.xyz);
    output.Position.xyz += input.Quaternion.w * t + cross(input.Quaternion.xyz, t);
    output.Position.xyz += input.Translate;
    output.Position = mul(output.Position, CameraInfo.View);
    output.Position = mul(output.Position, CameraInfo.Projection);

    output.ColorAsShDcComponentAndAlphaBeforeSigmoidActivision = input.ColorAsShDcComponentAndAlphaBeforeSigmoidActivision;

    return output;
}

struct Fragment
{
    float4 color;
};

typedef VSOutput PSInput;

float sigmoid(float x)
{
    return 1.0f / (1.0f + exp(-x));
}

[shader("fragment")]
Fragment PSMain(PSInput input) : SV_Target0
{
    Fragment output;

    output.color.rgb = 0.5 + 0.282095 * input.ColorAsShDcComponentAndAlphaBeforeSigmoidActivision.rgb;
    output.color.a = sigmoid(input.ColorAsShDcComponentAndAlphaBeforeSigmoidActivision.a);
    
    return output;
}