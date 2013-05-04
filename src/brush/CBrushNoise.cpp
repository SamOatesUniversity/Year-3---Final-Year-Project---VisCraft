#include "IBrush.h"

CBrushNoise::CBrushNoise()
{

}

CBrushNoise::~CBrushNoise()
{

}

void CBrushNoise::Apply(
		CGizmo *gizmo, //!< The gizmo controlling this brush
		CInput *input, //!< The input device being used for the brush 
		CTerrain *terrain //!< The terrain object we want to apply the brush too
	)
{
	if (!input->IsMouseDown(MouseButton::Right))
		return;

	D3DXVECTOR2 position = D3DXVECTOR2(gizmo->Position().x, gizmo->Position().z);

	HeightMap *centerhmap = terrain->GetTerrainVertexAt(position.x, position.y);
	if (centerhmap == nullptr)
	{
		return;
	}

	for (int xOffset = -m_size; xOffset <= m_size; ++xOffset)
	{
		for (int zOffset = -m_size; zOffset <= m_size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(position.x + xOffset, position.y + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}
			float noise = (500 - (rand() % 1000)) * 0.00025f;
			hmap->position.y += noise;
		}
	}

	terrain->UpdateHeightMap();
}

void CBrushNoise::Apply( 
	CGizmo *gizmo,					//!< The gizmo controlling this brush
	CKinect *kinect,				//!< The input device being used for the brush 
	CTerrain *terrain				//!< The terrain object we want to apply the brush too 
	)
{
	if (!kinect->GetHandState() == HandState::ClosedFist)
		return;

	D3DXVECTOR2 position = D3DXVECTOR2(gizmo->Position().x, gizmo->Position().z);

	HeightMap *centerhmap = terrain->GetTerrainVertexAt(position.x, position.y);
	if (centerhmap == nullptr)
	{
		return;
	}

	for (int xOffset = -m_size; xOffset <= m_size; ++xOffset)
	{
		for (int zOffset = -m_size; zOffset <= m_size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(position.x + xOffset, position.y + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}
			float noise = (500 - (rand() % 1000)) * 0.00025f;
			hmap->position.y += noise;
		}
	}

	terrain->UpdateHeightMap();
}