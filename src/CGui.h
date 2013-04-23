#pragma once

#include "crenderer.h"
#include "2d/CBitmap.h"
#include "2d/CTextureShader.h"

struct GuiState {
	enum Enum {
		MainMenu,
		Brushes,
		File,
		About,
		Noof
	};
};

struct BrushType {
	enum Enum {
		Deform,
		Raise,
		Lower,
		Level,
		Noise,
		Smooth,
		Noof
	};
};

class CGui {

private:

	struct TextOverlay {
		enum Enum {
			MainMenu,
			Brushes,
			File,
			About,
			Noof
		};
	};

private:

	CBitmap								*m_overlay;
	CBitmap								*m_textOverlay[TextOverlay::Noof];
	CBitmap								*m_brushOverlay[BrushType::Noof];

	CTextureShader						*m_textureShader;

	bool								m_visible;

	GuiState::Enum						m_state;
	BrushType::Enum						m_brushChangeOverlay;

public:
										//! 
										CGui();

										//! 
										~CGui();

										//! 
	const bool							Create(
											CRenderer *render				//!< 
										);

										//! 
	void								Release();

										//! 
	const bool							Render(
											CRenderer *render,				//!< 
											D3DXMATRIX worldMatrix,			//!< 
											D3DXMATRIX viewMatrix,			//!<
											D3DXMATRIX projectionMatrix		//!< 
										);

										//! 
	void								SetVisible( 
											bool visible 
										);

										//!
	const bool							IsVisible() const
										{
											return m_visible;
										}

										//!
	void								SetState(
											GuiState::Enum newState			//!< 
										);

										//! 
	GuiState::Enum						GetState() const;

										//! 
	void								SetActiveBrush(
											BrushType::Enum newBrush
										);

										//! 
	void								HandleMouseInput( 
											const D3DXVECTOR2 mousePosition 
										);
};