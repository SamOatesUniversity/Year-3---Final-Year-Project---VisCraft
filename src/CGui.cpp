#include "CGui.h"

CGui::CGui()
{
	m_visible = false;
	m_overlay = nullptr;
	m_state = GuiState::MainMenu;

	for (unsigned int textOverlayIndex = 0; textOverlayIndex < TextOverlay::Noof; ++textOverlayIndex) {
		m_textOverlay[textOverlayIndex] = nullptr;
	}
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

	m_textOverlay[TextOverlay::MainMenu] = new CBitmap();
	if (!m_textOverlay[TextOverlay::MainMenu]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/main_menu.png", 512, 128))
	{
		return false;
	}

	m_textOverlay[TextOverlay::Brushes] = new CBitmap();
	if (!m_textOverlay[TextOverlay::Brushes]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes.png", 512, 128))
	{
		return false;
	}

	m_textOverlay[TextOverlay::File] = new CBitmap();
	if (!m_textOverlay[TextOverlay::File]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/file.png", 512, 128))
	{
		return false;
	}

	m_textOverlay[TextOverlay::About] = new CBitmap();
	if (!m_textOverlay[TextOverlay::About]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/main_menu.png", 512, 128))
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

	switch (m_state) 
	{
		case GuiState::MainMenu:
		{
			CBitmap *const overlay = m_textOverlay[TextOverlay::MainMenu];
			overlay->Render(render->GetDeviceContext(), static_cast<int>((screenWidth * 0.5f) - (overlay->GetWidth() * 0.5f)), screenHeight - 256 + (static_cast<int>((256 - overlay->GetHeight()) * 0.5f )));
			if (!m_textureShader->Render(render->GetDeviceContext(), overlay->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, overlay->GetTexture()))
				return false;
		}
		break;

		case GuiState::Brushes:
		{
			CBitmap *const overlay = m_textOverlay[TextOverlay::Brushes];
			overlay->Render(render->GetDeviceContext(), static_cast<int>((screenWidth * 0.5f) - (overlay->GetWidth() * 0.5f)), screenHeight - 256 + (static_cast<int>((256 - overlay->GetHeight()) * 0.5f )));
			if (!m_textureShader->Render(render->GetDeviceContext(), overlay->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, overlay->GetTexture()))
				return false;
		}
		break;

		case GuiState::File:
		{
			CBitmap *const overlay = m_textOverlay[TextOverlay::File];
			overlay->Render(render->GetDeviceContext(), static_cast<int>((screenWidth * 0.5f) - (overlay->GetWidth() * 0.5f)), screenHeight - 256 + (static_cast<int>((256 - overlay->GetHeight()) * 0.5f )));
			if (!m_textureShader->Render(render->GetDeviceContext(), overlay->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, overlay->GetTexture()))
				return false;
		}
		break;

		case GuiState::About:
		{
			CBitmap *const overlay = m_textOverlay[TextOverlay::About];
			overlay->Render(render->GetDeviceContext(), static_cast<int>((screenWidth * 0.5f) - (overlay->GetWidth() * 0.5f)), screenHeight - 256 + (static_cast<int>((256 - overlay->GetHeight()) * 0.5f )));
			if (!m_textureShader->Render(render->GetDeviceContext(), overlay->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, overlay->GetTexture()))
				return false;
		}
		break;
	}

	return true;
}

void CGui::SetVisible( 
		bool visible 
	)
{
	m_visible = visible;
}

void CGui::Release()
{
	if (m_overlay != nullptr)
	{
		m_overlay->Destroy();
		SafeDelete(m_overlay);
	}

	for (unsigned int textOverlayIndex = 0; textOverlayIndex < TextOverlay::Noof; ++textOverlayIndex)
	{
		if (m_textOverlay[textOverlayIndex] != nullptr)
		{
			m_textOverlay[textOverlayIndex]->Destroy();
			SafeDelete(m_textOverlay[textOverlayIndex]);
		}
	}

	if (m_textureShader != nullptr)
	{
		m_textureShader->Destroy();
		SafeDelete(m_textureShader);	
	}	
}

void CGui::SetState( 
		GuiState::Enum newState /*!< */ 
	)
{
	m_state = newState;
}

GuiState::Enum CGui::GetState() const
{
	return m_state;
}
