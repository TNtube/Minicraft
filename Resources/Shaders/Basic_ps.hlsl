struct Input {
    float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(Input input) : SV_TARGET {
    // return float4(1, 0, 1, 1);
    return float4(input.uv, 0, 1);
}