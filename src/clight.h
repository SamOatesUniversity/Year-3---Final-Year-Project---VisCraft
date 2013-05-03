#pragma once

#include <D3DX10math.h>

class CLight {

private:

	D3DXVECTOR4								m_colorAmbiant;
	D3DXVECTOR4								m_colorDiffuse;
	D3DXVECTOR3								m_direction;

public:
											//! Class constructor
											CLight();

											//! Class destructor
											~CLight();

											//! 
	D3DXVECTOR4								GetAmbiant() const 
											{ 
												return m_colorAmbiant; 
											}

											//! 
	D3DXVECTOR4								GetDiffuse() const 
											{ 
												return m_colorDiffuse; 
											}

											//! 
	D3DXVECTOR3								GetDirection() const
											{
												return m_direction;
											}

											//! 
	void									SetDirection(
												D3DXVECTOR3 &direction
											);
};