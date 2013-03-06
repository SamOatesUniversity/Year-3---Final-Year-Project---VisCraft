#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "CTexture.h"

class CBitmap {

private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

private:

	ID3D11Buffer					*m_vertexBuffer;					//!< The render objects vertex buffer
	ID3D11Buffer					*m_indexBuffer;						//!< The render objects index buffer
	int								m_vertexCount;						//!< The render objects vertex count
	int								m_indexCount;						//!< The render objects index count
	CTexture						*m_texture;							//!< 

	int								m_screenWidth;
	int								m_screenHeight;
	int								m_bitmapWidth;
	int								m_bitmapHeight;
	int								m_previousPosX;
	int								m_previousPosY;

private:
									//! Create the 3d buffers we will use to draw onto
	const bool						CreateBuffers(
										ID3D11Device *device
									);

									//! Destroy the buffers
	void							DestroyBuffers();

									//! Update the buffers
	bool							UpdateBuffers(
										ID3D11DeviceContext *deviceContext, 
										int x, 
										int y
									);

									//! Render the buffers
	void							RenderBuffers(
										ID3D11DeviceContext *deviceContext
									);

									//! Load a texture at the given path
	bool							LoadTexture(
										ID3D11Device *device, 
										char *fileLocation
									);
	
									//! Release the texture resources
	void							ReleaseTexture();

public:

									//! Class constructor
									CBitmap();

									//! Class destructor
									~CBitmap();

									//! Create the bitmap
	const bool						Create(
										ID3D11Device *device,
										int screenWidth,
										int screenHeight,
										char* filePath,
										int width,
										int height
									);	

									//! Destroy the bitmap
	void							Destroy();

									//! Draw the bitmap
	const bool						Render(
										ID3D11DeviceContext *deviceContext, 
										int x, 
										int y
									);

									//! Get the index count
	const int						GetIndexCount() const;

									//! Get the texture resource
	ID3D11ShaderResourceView		*GetTexture() const;


};
