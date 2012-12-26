#include "cgizmo.h"

/*
 *	\brief Class constructor
*/
CGizmo::CGizmo()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_gizmoBuffer = nullptr;

	m_position = D3DXVECTOR3(0, 4, 0);

	m_gizmoState = GizmoState::Free;

	m_currentBrush = BrushType::Raise;
	m_brush.resize(BrushType::Noof);
}

/*
 *	\brief Class destructor
*/
CGizmo::~CGizmo()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);

	for (IBrush* brush : m_brush)
	{
		SafeDelete(brush);
	}
}

/*
 *	\brief Create the gizmo
*/
bool CGizmo::Create( 
		CRenderer *renderer						//!< Pointer to the renderer
	)
{
	m_renderer = renderer;

	m_indexCount = m_vertexCount = 3;

	// Create the vertex array.
	CGizmo::Vertex *const vertices = new CGizmo::Vertex[m_vertexCount];

	// Create the index array.
	unsigned long *const indices = new unsigned long[m_indexCount];

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);		// Bottom left.
	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// Top middle.
	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);		// Bottom right.

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(CGizmo::Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	if (FAILED(m_renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		delete[] vertices;
		delete[] indices;
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if (FAILED(m_renderer->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		delete[] vertices;
		delete[] indices;
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	delete[] indices;

	// Compile the vertex shader code.
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DX11CompileFromFile("shaders/gizmo.vs", NULL, NULL, "ColorVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
		return false;
	
	// Compile the pixel shader code.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	result = D3DX11CompileFromFile("shaders/gizmo.ps", NULL, NULL, "ColorPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
		return false;

	// Create the vertex shader from the buffer.
	result = m_renderer->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	// Create the pixel shader from the buffer.
	result = m_renderer->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = m_renderer->GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SafeRelease(vertexShaderBuffer);
	SafeRelease(pixelShaderBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CGizmo::MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = m_renderer->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC gizmoBufferDesc;
	gizmoBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gizmoBufferDesc.ByteWidth = sizeof(CGizmo::GizmoBuffer);
	gizmoBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gizmoBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gizmoBufferDesc.MiscFlags = 0;
	gizmoBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = m_renderer->GetDevice()->CreateBuffer(&gizmoBufferDesc, NULL, &m_gizmoBuffer);
	if (FAILED(result))
		return false;

	m_brush[BrushType::Deform] = new CBrushDeform(); 
	m_brush[BrushType::Lower] = new CBrushLower();
	m_brush[BrushType::Raise] = new CBrushRaise();

	return true;
}

/*
 *	\brief Render the gizmo
*/
void CGizmo::Render(
		D3DXMATRIX worldMatrix,					//!< 
		D3DXMATRIX viewMatrix,					//!< 
		D3DXMATRIX projectionMatrix,			//!< 
		CCamera *camera
	)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(CGizmo::Vertex); 
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	m_renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	m_renderer->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	m_renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// move to the position, but offset the y coordinate by one, to compensate for it been drawn around 0, 0, 0 local space
	D3DXMatrixTranslation(&worldMatrix, m_position.x, m_position.y + 1.0f, m_position.z); 
	
	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);	

	// Lock the constant buffer so it can be written to.
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_renderer->GetDeviceContext()->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return;
	
	// Get a pointer to the data in the constant buffer.
	CGizmo::MatrixBuffer *const matrixDataPtr = (CGizmo::MatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	m_renderer->GetDeviceContext()->Unmap(m_matrixBuffer, 0);

	result = m_renderer->GetDeviceContext()->Map(m_gizmoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	CGizmo::GizmoBuffer *const gizmoDataPtr = (CGizmo::GizmoBuffer*)mappedResource.pData;

	// Copy the data into the constant buffer.

	// color based upon gizmo state
	if (m_gizmoState == GizmoState::Free)
	{
		gizmoDataPtr->color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		gizmoDataPtr->color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	// Unlock the constant buffer.
	m_renderer->GetDeviceContext()->Unmap(m_gizmoBuffer, 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	m_renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	m_renderer->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_gizmoBuffer);

	// Set the vertex input layout.
	m_renderer->GetDeviceContext()->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_renderer->GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	m_renderer->GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	m_renderer->GetDeviceContext()->DrawIndexed(m_indexCount, 0, 0);
}

/*
*	\breif 
*/
void CGizmo::Control( 
		CInput *input,									//!< 
		CTerrain *terrain,								//!<
		CCamera *camera									//!< 
	)
{
	IBrush *const brush = m_brush[m_currentBrush];	

	if (brush->IsLockable())
	{
		if (input->IsMouseDown(MouseButton::Right))
		{
			if (m_gizmoState != GizmoState::Locked)
			{
				m_dragData.startMousePosition = input->GetMousePosition();
				m_dragData.lastY = m_dragData.startMousePosition.y;
				m_gizmoState = GizmoState::Locked;
			}		
		}
		else
		{
			if (m_gizmoState == GizmoState::Locked)
			{
				::SetCursorPos(
					static_cast<int>(m_dragData.startMousePosition.x), 
					static_cast<int>(m_dragData.startMousePosition.y)
					);
			}
			m_gizmoState = GizmoState::Free;
		}
	}

	if (m_gizmoState == GizmoState::Free)
	{
		const D3DXVECTOR2 mousePos = input->GetMousePosition();

		D3D11_VIEWPORT viewport = m_renderer->GetViewPort();

		D3DXMATRIX projection;
		m_renderer->GetProjectionMatrix(projection);

		D3DXMATRIX view;
		camera->GetViewMatrix(view);

		D3DXVECTOR3 ray;
		ray.x = ( ( ( 2.1f * mousePos.x ) / viewport.Width ) - 1 ) / projection._11;
		ray.y = -( ( ( 2.1f * mousePos.y ) / viewport.Height ) - 1 ) / projection._22;
		ray.z = 1.0f;

		D3DXMATRIX inverseView;
		D3DXMatrixInverse(&inverseView, NULL, &view);

		D3DXVECTOR3 rayOrigin, rayDirection;
		rayDirection.x = ray.x * inverseView._11 + ray.y * inverseView._21 + ray.z * inverseView._31;
		rayDirection.y = ray.x * inverseView._12 + ray.y * inverseView._22 + ray.z * inverseView._32;
		rayDirection.z = ray.x * inverseView._13 + ray.y * inverseView._23 + ray.z * inverseView._33;
		rayOrigin.x = inverseView._41;
		rayOrigin.y = inverseView._42;
		rayOrigin.z = inverseView._43;

		const D3DXVECTOR3 floorNorm = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		const float distance = -(D3DXVec3Dot(&floorNorm, &rayOrigin) / D3DXVec3Dot(&floorNorm, &rayDirection));

		m_position.x = rayOrigin.x + (rayDirection.x * distance);
		m_position.z = rayOrigin.z + (rayDirection.z * distance);
		m_position.y = terrain->GetTerrainHeightAt(m_position.x, m_position.z);
	}

	brush->Apply(this, input, terrain);
}

void CGizmo::SetCurrentBrush( 
		const BrushType::Enum brushType 
	)
{
	m_currentBrush = brushType;
}
