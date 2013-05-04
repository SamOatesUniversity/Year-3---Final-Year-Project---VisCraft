#include "IBrush.h"

CBrushRaise::CBrushRaise()
{

}

CBrushRaise::~CBrushRaise()
{

}

void CBrushRaise::Apply(
		CGizmo *gizmo, //!< The gizmo controlling this brush
		CInput *input, //!< The input device being used for the brush 
		CTerrain *terrain //!< The terrain object we want to apply the brush too
	)
{
	if (!input->IsMouseDown(MouseButton::Right))
		return;

	for (int xOffset = -m_size; xOffset <= m_size; ++xOffset)
	{
		for (int zOffset = -m_size; zOffset <= m_size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(gizmo->Position().x + xOffset, gizmo->Position().z + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}

			float scale = 1;
			if (xOffset < 0) scale += -xOffset; else scale += xOffset;
			if (zOffset < 0) scale += -zOffset; else scale += zOffset;
			scale *= 0.75f;

			const float scaledMovedAmount = -(m_strength / (scale));

			hmap->position.y -= scaledMovedAmount;
		}
	}

	terrain->UpdateHeightMap();
}

void CBrushRaise::Apply( 
	CGizmo *gizmo,					//!< The gizmo controlling this brush
	CKinect *kinect,				//!< The input device being used for the brush 
	CTerrain *terrain				//!< The terrain object we want to apply the brush too 
	)
{
	if (!kinect->GetHandState() == HandState::ClosedFist)
		return;

	for (int xOffset = -m_size; xOffset <= m_size; ++xOffset)
	{
		for (int zOffset = -m_size; zOffset <= m_size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(gizmo->Position().x + xOffset, gizmo->Position().z + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}

			float scale = 1;
			if (xOffset < 0) scale += -xOffset; else scale += xOffset;
			if (zOffset < 0) scale += -zOffset; else scale += zOffset;
			scale *= 5.0f;

			const float scaledMovedAmount = -(m_strength / (scale));

			hmap->position.y -= scaledMovedAmount;
		}
	}

	terrain->UpdateHeightMap();
}