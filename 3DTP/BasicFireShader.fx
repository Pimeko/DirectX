cbuffer MonCB : register(b0)
{
	matrix World;
}

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VertexInput
{
	float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
    float2 texCoord0 : TEXCOORD0;
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};

cbuffer NoiseBuffer : register(b1)
{
    float frameTime;
    float3 scrollSpeeds;
    float3 scales;
    float padding;
};

// first to be called
VertexOutput DiffuseVS(VertexInput input)
{
	VertexOutput output;

	output.position = mul(float4(input.position, 1.0f), World);
	
    output.texCoord0 = input.texCoord;
	
    output.texCoords1 = (input.texCoord * scales.x);
    output.texCoords1.y += (frameTime * scrollSpeeds.x);

    output.texCoords2 = (input.texCoord * scales.y);
    output.texCoords2.y += (frameTime * scrollSpeeds.y);

    output.texCoords3 = (input.texCoord * scales.z);
    output.texCoords3.y += (frameTime * scrollSpeeds.z);
	
	return output;
}

float4 DiffusePS(VertexOutput input) :SV_Target
{
	float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texCoord0);
	return diffuse;
}