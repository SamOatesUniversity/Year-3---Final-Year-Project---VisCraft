//====================================================
// Sky Box
//====================================================

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix invViewMarix;
};

struct VertexInputType
{
    float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
	float3 view : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	
	output.texcoord = input.texcoord;
	
	output.normal = input.normal;
	
	output.view = mul(input.position, worldMatrix) - invViewMarix[3].xyz; 

    return output;
}
