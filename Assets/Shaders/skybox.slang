struct VSInput
{
    float3 inPosition;
    float3 inTexCoord;
};

struct VSOutput {
    float4 sv_position : SV_Position;
    float3 worldPos : TEXCOORD0;
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
    output.worldPos = input.inPosition;

	return output;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target
{
    float3 dir = normalize(inVert.worldPos);

    float up = dir.y;

    float3 skyColor = float3(0.3, 0.55, 0.9);
    float3 horizonColor = float3(0.8, 0.9, 0.95);
    float3 groundColor = float3(0.1, 0.15, 0.2);

    float3 finalColor;
    if (up > 0.0)
    {
        finalColor = lerp(horizonColor, skyColor, pow(up, 0.5));
    }
    else
    {
        float fallOff = saturate(-up * 2.0);
        finalColor = lerp(horizonColor, groundColor, fallOff);
    }

    return float4(finalColor, 1.0);
}