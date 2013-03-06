#pragma once

#include "crenderer.h"
#include "2d/CBitmap.h"
#include "2d/CTextureShader.h"

class CGui {

private:

	struct TextOverlay {
		enum Enum {
			Listening,
			Noof
		};
	};

private:

	CBitmap								*m_overlay;
	CBitmap								*m_textOverlay[TextOverlay::Noof];

	CTextureShader						*m_textureShader;

	bool								m_visible;

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

};