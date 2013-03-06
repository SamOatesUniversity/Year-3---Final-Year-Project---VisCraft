#pragma once

#include "crenderer.h"
#include "2d/CBitmap.h"
#include "2d/CTextureShader.h"

class CGui {

private:

	CBitmap								*m_overlay;
	CTextureShader						*m_textureShader;

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


};