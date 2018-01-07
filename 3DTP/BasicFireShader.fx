cbuffer WorldBuffer : register(b0)
{
	matrix World;
}

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

// Vertex Shader (first to be called)
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

// Pixel Shader

Texture2D fireTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D alphaTexture : register(t2);
SamplerState SampleType;
SamplerState SampleType2;

cbuffer DistortionBuffer : register(b2)
{
    float2 distortion1;
    float2 distortion2;
    float2 distortion3;
    float distortionScale;
    float distortionBias;
};

float4 DiffusePS(VertexOutput input) :SV_Target
{
	float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;
	
	// Sample the same noise texture using the three different texture coordinates to get three different noise scales.
    noise1 = noiseTexture.Sample(SampleType, input.texCoords1);
    noise2 = noiseTexture.Sample(SampleType, input.texCoords2);
    noise3 = noiseTexture.Sample(SampleType, input.texCoords3);

    // Move the noise from the (0, 1) range to the (-1, +1) range.
    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;
	
	
	// Distort the three noise x and y coordinates by the three different distortion x and y values.
    noise1.xy = noise1.xy * distortion1.xy;
    noise2.xy = noise2.xy * distortion2.xy;
    noise3.xy = noise3.xy * distortion3.xy;

    // Combine all three distorted noise results into a single noise result.
    finalNoise = noise1 + noise2 + noise3;
	
	// Perturb the input texture Y coordinates by the distortion scale and bias values.  
    // The perturbation gets stronger as you move up the texture which creates the flame flickering at the top effect.
    perturb = ((1.0f - input.texCoord0.y) * distortionScale) + distortionBias;

    // Now create the perturbed and distorted texture sampling coordinates that will be used to sample the fire color texture.
    noiseCoords.xy = (finalNoise.xy * perturb) + input.texCoord0.xy;
	
	// Sample the color from the fire texture using the perturbed and distorted texture sampling coordinates.
    // Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
    fireColor = fireTexture.Sample(SampleType2, noiseCoords.xy);

    // Sample the alpha value from the alpha texture using the perturbed and distorted texture sampling coordinates.
    // This will be used for transparency of the fire.
    // Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
    alphaColor = alphaTexture.Sample(SampleType2, noiseCoords.xy);
	
	// Set the alpha blending of the fire to the perturbed and distored alpha texture value.
    fireColor.a = alphaColor;
	
    return fireColor;
}