cbuffer MonCB : register(b0)
{
	matrix World;
}

Texture2D ObjTexture : register(t0);
Texture2D DetailTexture : register(t1);
SamplerState ObjSamplerState : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexInput
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VertexOutput DiffuseVS(VertexInput input)
{
	VertexOutput output;

	output.position = mul(float4(input.position, 1.0f), World);
	output.texCoord = input.texCoord;

	return output;
}

float4 DiffusePS(VertexOutput input) :SV_Target
{
	float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texCoord);
	float4 detail = DetailTexture.Sample(ObjSamplerState, input.texCoord * 10);
	return diffuse * detail;
}