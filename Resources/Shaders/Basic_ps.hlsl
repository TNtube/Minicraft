Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
	float4 normal: NORMAL0;
	float2 uv : TEXCOORD0;
};


float4 main(Input input) : SV_TARGET {
	float4 color = tex.Sample(samplerState, input.uv);
	float4 lightDir = float4(0.2f, 1.0f, 0.8f, 1.0f);
	float4 dir = normalize(lightDir);
	float diff = saturate(dot(input.normal, dir));
	return float4(diff * color.rgb, color.a);
	// return input.normal;
}