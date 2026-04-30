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
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};
ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	VSOutput output;

	output.sv_position = mul(ubo.projection, mul(ubo.view, mul(ubo.model, float4(input.inPosition, 1.0))));
    output.fragTexCoord = input.inTexCoord;

	return output;
}

Sampler2D texture[4];

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    return texture[int(inVert.fragTexCoord.b)].Sample(inVert.fragTexCoord.rg);
}
