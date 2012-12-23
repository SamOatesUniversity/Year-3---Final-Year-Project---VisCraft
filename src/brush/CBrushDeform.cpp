#include "CBrushDeform.h"

CBrushDeform::CBrushDeform()
{

}

CBrushDeform::~CBrushDeform()
{

}

void CBrushDeform::Apply( 
		CGizmo *gizmo,					//!< The gizmo controlling this brush
		CInput *input,					//!< The input device being used for the brush 
		CTerrain *terrain				//!< The terrain object we want to apply the brush too 
	)
{
	if (gizmo->GetGizmoState() != GizmoState::Locked)
		return;

	const D3DXVECTOR2 mousePos = input->GetMousePosition();
	const float moveAmount = (mousePos.y - gizmo->DragData().lastY) * 0.025f;

	if (moveAmount == 0.0f)
		return;

	static const int size = 2;

	for (int xOffset = -size; xOffset <= size; ++xOffset)
	{
		for (int zOffset = -size; zOffset <= size; ++zOffset)
		{
			HeightMap *hmap = terrain->GetTerrainVertexAt(gizmo->Position().x + xOffset, gizmo->Position().z + zOffset);

			float scale = 1;
			if (xOffset < 0) scale += -xOffset; else scale += xOffset;
			if (zOffset < 0) scale += -zOffset; else scale += zOffset;
			scale *= 0.75f;

			const float scaledMovedAmount = moveAmount / (scale);

			hmap->position.y -= scaledMovedAmount;
		}
	}

	terrain->UpdateHeightMap();
	gizmo->DragData().lastY = mousePos.y;
}
