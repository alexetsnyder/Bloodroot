struct VSInput
{
    float3 inPosition;
    float3 inTexCoord;
};

struct VSOutput {
    float4 sv_position : SV_Position;
    float3 fragTexCoord;
};

struct PushConstants
{
    float4x4 model;
};

[shader("vertex")]
VSOutput vertMain(uniform PushConstants pc, VSInput input)
{
	VSOutput output;

	output.sv_position = float4(input.inPosition, 1.0);
    output.fragTexCoord = input.inTexCoord;

	return output;
}

layout(set = 0, binding = 2)Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    float4 color = texture.Sample(inVert.fragTexCoord.xy);
    if (color.a < 0.5)
    {
        discard;
    }

    return color;
}
