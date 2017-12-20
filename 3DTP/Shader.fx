cbuffer MonCB : register(b0)
{
  matrix World;
}

struct VertexInput
{
  float3 position : POSITION;
  float3 color : COLOR0;
};

struct VertexOutput
{
  float4 position : SV_POSITION;
  float3 color : COLOR0;
};

VertexOutput DiffuseVS(VertexInput input)
{
  VertexOutput output;

  output.position = mul(float4(input.position, 1.0f), World);
  output.color = input.color;

  return output;
}

float4 DiffusePS(VertexOutput input) :SV_Target
{
  return float4(input.color, 1.0f);
}
