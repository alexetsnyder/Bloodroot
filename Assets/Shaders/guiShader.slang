struct VSInput
{
    float3 inPosition;
    float2 inTexCoord;
};

struct VSOutput {
    float4 sv_position : SV_Position;
    float2 fragTexCoord;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
	VSOutput output;

	output.sv_position = float4(input.inPosition, 1.0);
    output.fragTexCoord = input.inTexCoord;

	return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    return texture.Sample(inVert.fragTexCoord);
}
