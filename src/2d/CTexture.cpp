#include "CTexture.h"

CTexture::CTexture()
{
	m_texture = nullptr;
}

CTexture::~CTexture()
{

}

const bool CTexture::Create(
		ID3D11Device *device, 
		char *filename 
	)
{
	// Load the texture in.
	const HRESULT result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void CTexture::Destroy()
{
	// Release the texture resource.
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
}

ID3D11ShaderResourceView *CTexture::GetTexture() const
{
	return m_texture;
}


