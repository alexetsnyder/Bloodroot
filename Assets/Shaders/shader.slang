struct VSInput
{
    float2 inPosition;
    float3 inColor;
}

struct VSOutput {
	float3 color;
	float4 sv_position : SV_Position;
};

struct UniformBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};
ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	VSOutput output;

	output.sv_position = mul(ubo.projection, mul(ubo.view, mul(ubo.model, float4(input.inPosition, 0.0, 1.0))));
	output.color = input.inColor;

	return output;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    return float4(inVert.color, 1.0);
}
