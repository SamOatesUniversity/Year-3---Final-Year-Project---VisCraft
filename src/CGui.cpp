#include "CGui.h"
#include "cviscraft.h"

CGui::CGui()
{
	m_visible = false;
	m_overlay = nullptr;
	m_state = GuiState::MainMenu;
	m_brushChangeOverlay = BrushType::Raise;

	for (unsigned int textOverlayIndex = 0; textOverlayIndex < TextOverlay::Noof; ++textOverlayIndex) {
		m_textOverlay[textOverlayIndex] = nullptr;
	}

	for (unsigned int brushOverlayIndex = 0; brushOverlayIndex < BrushType::Noof; ++brushOverlayIndex) {
		m_brushOverlay[brushOverlayIndex] = nullptr;
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

	m_brushOverlay[BrushType::Raise] = new CBitmap();
	if (!m_brushOverlay[BrushType::Raise]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes/raise.bmp", 256, 128))
	{
		return false;
	}

	m_brushOverlay[BrushType::Lower] = new CBitmap();
	if (!m_brushOverlay[BrushType::Lower]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes/lower.bmp", 256, 128))
	{
		return false;
	}

	m_brushOverlay[BrushType::Deform] = new CBitmap();
	if (!m_brushOverlay[BrushType::Deform]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes/deform.bmp", 256, 128))
	{
		return false;
	}

	m_brushOverlay[BrushType::Level] = new CBitmap();
	if (!m_brushOverlay[BrushType::Level]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes/level.bmp", 256, 128))
	{
		return false;
	}

	m_brushOverlay[BrushType::Noise] = new CBitmap();
	if (!m_brushOverlay[BrushType::Noise]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes/noise.bmp", 256, 128))
	{
		return false;
	}

	m_brushOverlay[BrushType::Smooth] = new CBitmap();
	if (!m_brushOverlay[BrushType::Smooth]->Create(render->GetDevice(), static_cast<int>(render->GetViewPort().Width), static_cast<int>(render->GetViewPort().Height), "graphics/GUI/text overlay/brushes/smooth.bmp", 256, 128))
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
	const int screenWidth = static_cast<int>(render->GetViewPort().Width);
	const int screenHeight = static_cast<int>(render->GetViewPort().Height);

	if (m_brushChangeOverlay != BrushType::Noof)
	{
		CBitmap *const brush = m_brushOverlay[m_brushChangeOverlay];
		if (brush != nullptr)
		{
			brush->Render(render->GetDeviceContext(), static_cast<int>((screenWidth * 0.5f) - (brush->GetWidth() * 0.5f)), static_cast<int>(screenHeight * 0.1f) + (static_cast<int>((256 - brush->GetHeight()) * 0.5f )));
			if (!m_textureShader->Render(render->GetDeviceContext(), brush->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, brush->GetTexture()))
				return false;
		}
	}

	if (!m_visible)
	{
		return true;
	}

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
	if (!m_visible)
	{
		m_state = GuiState::MainMenu;
	}
	::ShowCursor(m_visible);
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

	for (unsigned int brushOverlayIndex = 0; brushOverlayIndex < BrushType::Noof; ++brushOverlayIndex)
	{
		if (m_brushOverlay[brushOverlayIndex] != nullptr)
		{
			m_brushOverlay[brushOverlayIndex]->Destroy();
			SafeDelete(m_brushOverlay[brushOverlayIndex]);
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

void CGui::SetActiveBrush( 
		BrushType::Enum newBrush 
	)
{
	m_brushChangeOverlay = newBrush;
}

void CGui::HandleMouseInput( 
		const D3DXVECTOR2 mousePosition 
	)
{
	CBitmap *const overlay = m_textOverlay[m_state];
	if (overlay != nullptr)
	{
		D3DXVECTOR2 x1 = overlay->GetPosition() + D3DXVECTOR2(0, -40);
		D3DXVECTOR2 x2 = x1 + D3DXVECTOR2(static_cast<FLOAT>(overlay->GetWidth()), static_cast<FLOAT>(overlay->GetHeight()));
			
		if (mousePosition.x < x1.x) return;
		if (mousePosition.y < x1.y) return;
		if (mousePosition.x > x2.x) return;
		if (mousePosition.y > x2.y) return;

		// The mouse is over this overlay.
		// This code should only be called on mouse click, so handle what ever this overlay is.

		switch (m_state)
		{
		case TextOverlay::File:
			{
				D3DXVECTOR2 bx1 = x1 + D3DXVECTOR2(54, 10);
				D3DXVECTOR2 bx2 = x1 + D3DXVECTOR2(135, 50);
				if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
				{
					CVisCraft::GetInstance()->NewTerrain();
					SetVisible(false);
					return;
				}

				bx1 = x1 + D3DXVECTOR2(154, 10);
				bx2 = x1 + D3DXVECTOR2(268, 50);
				if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
				{
					CVisCraft::GetInstance()->OpenTerrain();
					SetVisible(false);
					return;
				}

				bx1 = x1 + D3DXVECTOR2(276, 10);
				bx2 = x1 + D3DXVECTOR2(375, 50);
				if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
				{
					CVisCraft::GetInstance()->SaveTerrain();
					SetVisible(false);
					return;
				}

				bx1 = x1 + D3DXVECTOR2(396, 10);
				bx2 = x1 + D3DXVECTOR2(474, 50);
				if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
				{
					CVisCraft::GetInstance()->Close();
					SetVisible(false);
					return;
				}
			}
			break;
		}

		// Brushes text
		D3DXVECTOR2 bx1 = x1 + D3DXVECTOR2(10, 90);
		D3DXVECTOR2 bx2 = x1 + D3DXVECTOR2(124, 124);
		if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
		{
			m_state = GuiState::Brushes;
			return;
		}

		// File text
		bx1 = x1 + D3DXVECTOR2(230, 90);
		bx2 = x1 + D3DXVECTOR2(300, 124);
		if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
		{
			m_state = GuiState::File;
			return;
		}

		// About text
		bx1 = x1 + D3DXVECTOR2(410, 90);
		bx2 = x1 + D3DXVECTOR2(510, 124);
		if (mousePosition.x > bx1.x && mousePosition.x < bx2.x && mousePosition.y > bx1.y && mousePosition.y < bx2.y)
		{
			ShellExecute(NULL, "open", "http://www.samoatesgames.com", NULL, NULL, SW_SHOWMAXIMIZED);
			SetVisible(false);
			return;
		}
	}
}
