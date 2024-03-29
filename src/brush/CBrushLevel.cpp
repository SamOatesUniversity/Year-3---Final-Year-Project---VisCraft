#include "IBrush.h"

CBrushLevel::CBrushLevel()
{

}

CBrushLevel::~CBrushLevel()
{

}

void CBrushLevel::Apply(
		CGizmo *gizmo, //!< The gizmo controlling this brush
		CInput *input, //!< The input device being used for the brush 
		CTerrain *terrain //!< The terrain object we want to apply the brush too
	)
{
	if (!input->IsMouseDown(MouseButton::Right))
		return;

	HeightMap *centerhmap = terrain->GetTerrainVertexAt(gizmo->Position().x, gizmo->Position().z);
	if (centerhmap == nullptr)
	{
		return;
	}

	for (int xOffset = -m_size; xOffset <= m_size; ++xOffset)
	{
		for (int zOffset = -m_size; zOffset <= m_size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(gizmo->Position().x + xOffset, gizmo->Position().z + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}
			hmap->position.y = centerhmap->position.y;
		}
	}

	terrain->UpdateHeightMap();
}

void CBrushLevel::Apply( 
	CGizmo *gizmo,					//!< The gizmo controlling this brush
	CKinect *kinect,				//!< The input device being used for the brush 
	CTerrain *terrain				//!< The terrain object we want to apply the brush too 
	)
{
	if (!kinect->GetHandState() == HandState::ClosedFist)
		return;

	HeightMap *centerhmap = terrain->GetTerrainVertexAt(gizmo->Position().x, gizmo->Position().z);
	if (centerhmap == nullptr)
	{
		return;
	}

	for (int xOffset = -m_size; xOffset <= m_size; ++xOffset)
	{
		for (int zOffset = -m_size; zOffset <= m_size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(gizmo->Position().x + xOffset, gizmo->Position().z + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}
			hmap->position.y = centerhmap->position.y;
		}
	}

	terrain->UpdateHeightMap();
}