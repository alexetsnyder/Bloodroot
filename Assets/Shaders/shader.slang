struct VSInput
{
    float3 inPosition;
    float2 inTexCoord;
    uint faceId;
};

struct VSOutput {
    float4 sv_position : SV_Position;
    float2 fragTexCoord;
    uint faceId;
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
    output.faceId = input.faceId;

	return output;
}

Sampler2D texture[4];

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    return texture[inVert.faceId].Sample(inVert.fragTexCoord);
}
