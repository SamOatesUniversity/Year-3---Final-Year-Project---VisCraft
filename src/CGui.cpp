#include "CGui.h"

CGui::CGui()
{

}

CGui::~CGui()
{

}

const bool CGui::Create( 
		CRenderer *render
	)
{
	m_overlay = new CBitmap();
	if (!m_overlay->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/overlay.png", 256, 256))
	{
		return false;
	}

	m_textureShader = new CTextureShader();
	if (!m_textureShader->Create(render->GetDevice()))
	{
		return false;
	}

	return true;
}

const bool CGui::Render( 
		CRenderer *render,					/*!< */ 
		D3DXMATRIX worldMatrix,				/*!< */  
		D3DXMATRIX viewMatrix,				/*!< */ 
		D3DXMATRIX projectionMatrix			/*!< */ 
	)
{
	if (!m_overlay->Render(render->GetDeviceContext(), 0, 0))
		return false;

	if (!m_textureShader->Render(render->GetDeviceContext(), m_overlay->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_overlay->GetTexture()))
		return false;

	return true;
}
