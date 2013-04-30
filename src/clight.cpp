#include "clight.h"


CLight::CLight()
{
	m_colorAmbiant = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f);
	m_colorDiffuse = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	m_direction = D3DXVECTOR3(0.0f, -0.9f, -0.75f);
}

CLight::~CLight()
{

}
