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

struct PushConstants
{
    float4x4 model;
};

[shader("vertex")]
VSOutput vertMain(uniform PushConstants pc, VSInput input)
{
	VSOutput output;

	output.sv_position = mul(ubo.projection, mul(ubo.view, mul(pc.model, float4(input.inPosition, 1.0))));
    output.fragTexCoord = input.inTexCoord;

	return output;
}

layout(set = 0, binding = 1) Sampler2DArray textures;

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    return textures.Sample(inVert.fragTexCoord);
}
