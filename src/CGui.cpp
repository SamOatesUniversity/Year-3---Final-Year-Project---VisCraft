#include "CGui.h"

CGui::CGui()
{
	m_visible = false;
	m_overlay = nullptr;

	for (unsigned int textOverlayIndex = 0; textOverlayIndex < TextOverlay::Noof; ++textOverlayIndex)
		m_textOverlay[textOverlayIndex] = nullptr;
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

	m_textOverlay[TextOverlay::Listening] = new CBitmap();
	if (!m_textOverlay[TextOverlay::Listening]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/listening.png", 256, 64))
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
	if (!m_visible)
	{
		return true;
	}

	int screenWidth = static_cast<int>(render->GetViewPort().Width);
	int screenHeight = static_cast<int>(render->GetViewPort().Height);

	if (!m_overlay->Render(render->GetDeviceContext(), 0, screenHeight - 256, screenWidth, 256))
		return false;

	if (!m_textureShader->Render(render->GetDeviceContext(), m_overlay->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_overlay->GetTexture()))
		return false;

	for (unsigned int textOverlayIndex = 0; textOverlayIndex < TextOverlay::Noof; ++textOverlayIndex)
	{
		m_textOverlay[textOverlayIndex]->Render(render->GetDeviceContext(), static_cast<int>((screenWidth * 0.5f) - 128), screenHeight - 128 - 32);

		if (!m_textureShader->Render(render->GetDeviceContext(), m_textOverlay[textOverlayIndex]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_textOverlay[textOverlayIndex]->GetTexture()))
			return false;
	}

	return true;
}

void CGui::SetVisible( 
		bool visible 
	)
{
	m_visible = visible;
}
