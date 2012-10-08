#include "ccamera.h"

/*!
 * \brief Class constructor
 */
CCamera::CCamera() {
	m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

/*!
 * \brief Class destructor
 */
CCamera::~CCamera() {

}

/*!
 * \brief Gets the cameras current position
 * \return The current position of the camera
 */
D3DXVECTOR3 CCamera::GetPosition() const
{
	return m_position;
}

/*!
 * \brief Gets the cameras current rotation
 * \return The current rotation of the camera
 */
D3DXVECTOR3 CCamera::GetRotation() const
{
	return m_rotation;
}

/*!
 * \brief Get the stored view matrix
 */
void CCamera::GetViewMatrix(
		D3DXMATRIX& viewMatrix 
	)
{
	viewMatrix = m_viewMatrix;
}

/*!
 * \brief Update the stored view matrix
 */
void CCamera::UpdateViewMatrix()
{
	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, m_rotation.y, m_rotation.x, m_rotation.z);

	// Setup the vector that points upwards.
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Setup where the camera is looking by default.
	D3DXVECTOR3 lookAt = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = m_position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &lookAt, &up);
}

/*!
 * \brief Control the camera with the given input device
 */
void CCamera::Control( 
		CInput *input					//!< 
	)
{
	if (input->IsKeyPressed(DIK_W)) 
	{
		SetPosition(m_position.x, m_position.y, m_position.z + 1.0f);
		return;
	}

	if (input->IsKeyPressed(DIK_S))
	{
		SetPosition(m_position.x, m_position.y, m_position.z - 1.0f);
		return;
	}

	if (input->IsKeyPressed(DIK_A))
	{
		SetPosition(m_position.x - 1.0f, m_position.y, m_position.z);
		return;
	}

	if (input->IsKeyPressed(DIK_D))
	{
		SetPosition(m_position.x + 1.0f, m_position.y, m_position.z);
		return;
	}
}
