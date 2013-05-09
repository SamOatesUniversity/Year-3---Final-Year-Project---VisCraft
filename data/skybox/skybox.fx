//====================================================
// Sky Box
//====================================================

//---------------------------------------------------
// Original by Evolved - www.evolved-software.com
// Edited by Sam Oates - www.samoatesgames.com
//----------------------------------------------------
// Change log - 17-04-2011
//----------------------------------------------------
// * Implemented fog.
// * Changed to vs/ps 3.0 for use within XNA 4.0.
// * renamed structs and some vars.

//--------------
// un-tweaks
//--------------
matrix world_view_projection_xform; 
matrix world_xform;   
matrix view_inverse_xform; 

//--------------
// tweaks
//--------------
float  fog_start  = 0.0f;
float  fog_end	  = 400000.0f;
float4 fog_colour = float4( 0.6, 0.6, 0.6, 0.0 );

//--------------
// Textures
//--------------
texture cubeMapTX;
samplerCUBE cubeMap = sampler_state 
{
	Texture = <cubeMapTX>;
};

//--------------
// structs 
//--------------
struct VS_INPUT
{
	float4 position : POSITION;
	float2 UV		: TEXCOORD;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float3 view		: TEXCOORD0; 
	float  depth	: TEXCOORD1;
};

//--------------
// vertex shader
//--------------
PS_INPUT VS(VS_INPUT vertex) 
{
	PS_INPUT pixel;
	pixel.position = mul( vertex.position, world_view_projection_xform ); 
	pixel.view = ( mul( vertex.position, world_xform ) - view_inverse_xform[3].xyz ); 
	pixel.depth = pixel.position.z;
	return pixel;
}

//--------------
// pixel shader
//--------------
float4 PS(PS_INPUT input)  : COLOR
{		
	float4 cube = texCUBE( cubeMap, input.view );
	float l = saturate( ( input.depth - fog_start ) / ( fog_end - fog_start ) );
	return cube;
}

//--------------
// techniques   
//--------------
technique SkySphere
{
	pass passone
	{		
		vertexShader = compile vs_3_0 VS(); 
		pixelShader  = compile ps_3_0 PS();	
	}
}