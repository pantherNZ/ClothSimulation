// Basic shader file

// cosntant buffer for matrices
cbuffer cbPerObject
{
	float4x4 gFinalMatrix; 
	float4x4 gWorldMatrix;
};

SamplerState gTriLinearSam
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexInputType
{
	float3 fPosLocal    : POSITION;
	float4 fColour      : COLOUR;
};

struct PixelInputType
{
	float4 fPosHom      : SV_POSITION;
    float3 fPosWorld    : POSITION;
	float4 fColour		: COLOUR;
};
 
PixelInputType VertexShaderFunction(VertexInputType input)
{
	PixelInputType output;

	// Transform to world space
	output.fPosWorld = mul(input.fPosLocal, gWorldMatrix);
		
	// Transform to homogeneous clip space
	output.fPosHom = mul(float4(input.fPosLocal, 1.0f), gFinalMatrix);

	output.fColour = input.fColour;

	return output;
}

float4 PixelShaderFunction(PixelInputType input) : SV_Target
{
	// return final colour
	return (input.fColour);
}

technique10 TextureTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader( vs_4_0, VertexShaderFunction()));
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader( ps_4_0, PixelShaderFunction()));
    }
}
