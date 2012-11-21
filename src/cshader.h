#pragma once

/**
	Header file includes
*/
#include "crenderer.h"
#include <d3dx11async.h>

class CShader {
private:
	struct MatrixBuffer
	{
		D3DXMATRIX world;										//!< 
		D3DXMATRIX view;										//!< 
		D3DXMATRIX projection;									//!< 
	};

	struct LightBuffer
	{
		D3DXVECTOR4 ambientColor;								//!< 
		D3DXVECTOR4 diffuseColor;								//!< 
		D3DXVECTOR3 lightDirection;								//!< 
		float __padding__;										//!< 
	};

private:
	CRenderer						*m_renderer;				//!< 

	ID3D11VertexShader				*m_vertexShader;			//!< 
	ID3D11PixelShader				*m_pixelShader;				//!< 

	ID3D11InputLayout				*m_layout;					//!< 
	ID3D11SamplerState				*m_sampleState;				//!< 
	ID3D11Buffer					*m_matrixBuffer;			//!< 
	ID3D11Buffer					*m_lightBuffer;				//!< 

public:
									//! Class Constructor							
									CShader();

									//! Class Destructor
									~CShader();

									//! 
	const bool						Create(
										CRenderer *renderer		//!< 
									);

									//! 
	void							Release();

									//! 
	const bool						Render(
										int indexCount,			//!< 
										D3DXMATRIX world,		//!< 
										D3DXMATRIX view,		//!< 
										D3DXMATRIX projection	//!< 
									);
};