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

private:
	CRenderer						*m_renderer;				//!< 

	ID3D11VertexShader				*m_vertexShader;			//!< 
	ID3D11PixelShader				*m_pixelShader;				//!< 

	ID3D11InputLayout				*m_layout;					//!< 
	ID3D11Buffer					*m_matrixBuffer;			//!< 

public:
									//! 							
									CShader();

									//! 
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