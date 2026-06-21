struct VSInput
{
    float3 inPosition;
    float3 inTexCoord;
};

struct VSOutput {
    float4 sv_position : SV_Position;
    float3 fragTexCoord;
};

struct UniformBuffer
{
    float4x4 view;
    float4x4 projection;
};
layout(set = 0, binding = 0) ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	VSOutput output;

    float4x4 viewNoTranslation = float4x4(
        float4(ubo.view._m00, ubo.view._m01, ubo.view._m02, 0.0),
        float4(ubo.view._m10, ubo.view._m11, ubo.view._m12, 0.0),
        float4(ubo.view._m20, ubo.view._m21, ubo.view._m22, 0.0),
        float4(0.0, 0.0, 0.0, 1.0)
    );

	output.sv_position = mul(ubo.projection, mul(viewNoTranslation, float4(input.inPosition, 1.0)));
    output.fragTexCoord = input.inTexCoord;

	return output;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    return float4(0, 0, 1, 1);
}