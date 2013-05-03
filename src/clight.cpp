#include "clight.h"


CLight::CLight()
{
	m_colorAmbiant = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f);
	m_colorDiffuse = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	m_direction = D3DXVECTOR3(-0.01f, -0.4f, 0.0f);
}

CLight::~CLight()
{

}

void CLight::SetDirection(
		D3DXVECTOR3 &direction
	)
{
	m_direction = direction;
}