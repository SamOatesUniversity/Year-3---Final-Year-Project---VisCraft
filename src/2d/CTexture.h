#pragma once

#include <d3d11.h>
#include <d3dx11tex.h>

class CTexture
{
private:
	ID3D11ShaderResourceView						*m_texture;						//!< The D3D11 shader texture resource

public:
													//! Class constructor
													CTexture();
													
													//! Class destructor
													~CTexture();

													//! Create the texture
	const bool										Create(
														ID3D11Device *device, 
														char *filename
													);

													//! Destroy the texture resources
	void											Destroy();

													//! Get the D3D11 Texture resource
	ID3D11ShaderResourceView						*GetTexture() const;

};