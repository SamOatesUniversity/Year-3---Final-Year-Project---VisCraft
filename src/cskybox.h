#pragma once

#include "cmesh.h"
#include <d3dx11async.h>

class CSkyBox {

private:

	struct MatrixBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX invViewMarix;
	};

private:

	CMesh					*m_mesh;
	ID3D11VertexShader		*m_vertexShader;					//!< 
	ID3D11PixelShader		*m_pixelShader;						//!< 
	ID3D11InputLayout		*m_layout;							//!< 
	ID3D11Buffer			*m_matrixBuffer;					//!< 

	ID3D11ShaderResourceView *m_texture;						//!< 
	ID3D11SamplerState		*m_sampleState;						//!< 

public:
							//! Class constructor
							CSkyBox();

							//! Class destructor
							~CSkyBox();

							//! 
	bool					Create(	
								CRenderer *renderer
							);

							//! 
	void					Render( 
								CRenderer *renderer,
								D3DXMATRIX world,		//!< 
								D3DXMATRIX view,		//!< 
								D3DXMATRIX projection	//!< 
							);
};