cbuffer ModelData : register(b0) {
	float4x4 model;
};

cbuffer CameraData : register(b1) {
	float4x4 view;
	float4x4 projection;
}

struct Input {
	float4 pos : POSITION;
	float4 normal: NORMAL0;
	float2 uv : TEXCOORD0;
};

struct Output {
	float4 pos : SV_POSITION;
	float4 normal: NORMAL0;
	float2 uv : TEXCOORD0;
};

Output main(Input input) {
	Output output = (Output)0;
	output.pos = mul( input.pos, model );
	output.pos = mul( output.pos, view );
	output.pos = mul( output.pos, projection );
	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}