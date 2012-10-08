#pragma once

/**
	Header file includes
*/
#include <d3dx10math.h>
#include "cinput.h"

class CCamera {
private:
	D3DXVECTOR3			m_position;				//!< The current position of the camera in 3D space
	D3DXVECTOR3			m_rotation;				//!< The current roational angles of the camera (in radians)
	D3DXMATRIX			m_viewMatrix;			//!< A pre-calculated view matrix for easy access whilst rendering

private:
						//! Update the stored view matrix
	void				UpdateViewMatrix();	

public:
						//! 
						CCamera();

						//!
						~CCamera();

						//! Get the current position
	D3DXVECTOR3			GetPosition() const;

						//! Get the current rotation
	D3DXVECTOR3			GetRotation() const;

						//! Get the stored view matrix
	void				GetViewMatrix(
							D3DXMATRIX& viewMatrix			//!< Reference of the view matrix
						);

						//! Set the position of the camera in 3D space
						template <class unit>
	inline void			SetPosition(
							const unit x,					//!< The x coordinate of the cameras position
							const unit y,					//!< The y coordinate of the cameras position
							const unit z					//!< The z coordinate of the cameras position
						) 
	{
		m_position.x = static_cast<float>(x);
		m_position.y = static_cast<float>(y);
		m_position.z = static_cast<float>(z);
		UpdateViewMatrix();
	}

						//! Set the rotation of the camera in 3D space
						template <class unit>
	inline void			SetRotation(
							const unit x,					//!< The x coordinate of the cameras rotation
							const unit y,					//!< The y coordinate of the cameras rotation
							const unit z					//!< The z coordinate of the cameras rotation
						) 
	{
		m_rotation.x = static_cast<float>(x);
		m_rotation.y = static_cast<float>(y);
		m_rotation.z = static_cast<float>(z);
		UpdateViewMatrix();
	}

						//! 
	void				Control( 
							CInput *input					//!< 
						);
};