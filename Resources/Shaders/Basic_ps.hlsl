Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(Input input) : SV_TARGET {
    // return float4(1, 0, 1, 1);

	float4 color = tex.Sample(samplerState, input.uv);
    return color;
}