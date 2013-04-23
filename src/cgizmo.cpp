#include "cgizmo.h"
#include "cviscraft.h"

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

	m_position = D3DXVECTOR3(0, 0, 0);

	m_gizmoState = GizmoState::Free;

	m_currentBrush = BrushType::Raise;
	m_brush.resize(BrushType::Noof);

	m_inputType = InputType::Kinect;
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

bool CGizmo::LoadGizmoMesh()
{
	std::ifstream objFile;
	objFile.open("gizmo.obj", std::ios_base::in);

	struct Face {
		float position[3];
		float normal[3];
	};
	std::vector<Face> faces;
	std::vector<D3DXVECTOR3> verts, vertNorms;

	std::string line;
	while (std::getline(objFile, line))
	{
		std::vector<std::string> tokens;
		std::istringstream iss(line);
		std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter<std::vector<std::string> >(tokens));

		if (tokens.empty())
		{
			continue;
		}

		if (tokens[0] == "v")
		{
			D3DXVECTOR3 newVert;
			newVert.x = static_cast<float>(::atof(tokens[1].c_str()));
			newVert.y = static_cast<float>(::atof(tokens[2].c_str()));
			newVert.z = static_cast<float>(::atof(tokens[3].c_str()));
			verts.push_back(newVert);
		}
		else if (tokens[0] == "vn")
		{
			D3DXVECTOR3 newVertNormal;
			newVertNormal.x = static_cast<float>(::atof(tokens[1].c_str()));
			newVertNormal.y = static_cast<float>(::atof(tokens[2].c_str()));
			newVertNormal.z = static_cast<float>(::atof(tokens[3].c_str()));
			vertNorms.push_back(newVertNormal);
		}
		else if (tokens[0] == "f")
		{
			Face newFace;

			for (int tokenIndex = 1; tokenIndex <= 3; ++tokenIndex)
			{
				std::string part = tokens[tokenIndex];
				std::string posPart = part.substr(0, part.find('/'));
				std::string normPart = part.substr(part.rfind('/') + 1);

				newFace.position[tokenIndex - 1] = static_cast<float>(::atof(posPart.c_str())) - 1;
				newFace.normal[tokenIndex - 1] = static_cast<float>(::atof(normPart.c_str())) - 1;
			}

			faces.push_back(newFace);
		}
	}

	objFile.close();

	m_vertexCount = verts.size();
	m_indexCount = faces.size() * 3;

	// Create the vertex array.
	CGizmo::Vertex *const vertices = new CGizmo::Vertex[m_vertexCount];

	// Create the index array.
	unsigned long *const indices = new unsigned long[m_indexCount];

	int vertIndex = 0;
	for (D3DXVECTOR3 vert : verts)
	{
		vertices[vertIndex++].position = vert;
	}

	// Load the index array with data.
	int indIndex = 0;
	for (unsigned int faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
	{
		indices[indIndex]		= static_cast<unsigned int>(faces[faceIndex].position[0]);  
		indices[indIndex + 1]	= static_cast<unsigned int>(faces[faceIndex].position[1]);  
		indices[indIndex + 2]	= static_cast<unsigned int>(faces[faceIndex].position[2]); 
		indIndex += 3;
	}


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

	return true;
}

/*
 *	\brief Create the gizmo
*/
bool CGizmo::Create( 
		CRenderer *renderer						//!< Pointer to the renderer
	)
{
	m_renderer = renderer;
	if (!LoadGizmoMesh())
	{
		return false;
	}

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
	m_brush[BrushType::Level] = new CBrushLevel();
	m_brush[BrushType::Noise] = new CBrushNoise();

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

	IBrush *const brush = m_brush[m_currentBrush];

	D3DXMATRIX xformmat, scalemat;
	D3DXMatrixTranslation(&xformmat, m_position.x, m_position.y + 2.0f, m_position.z); 
	D3DXMatrixScaling(&scalemat, static_cast<float>(brush->GetSize()) * 0.4f, 1.0f, static_cast<float>(brush->GetSize()) * 0.4f);
	
	D3DXMatrixMultiply (&worldMatrix, &scalemat, &xformmat);

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
		// green
		gizmoDataPtr->color = D3DXVECTOR4(0.0f, 0.6f, 0.0f, 0.01f);
	}
	else
	{
		// red
		gizmoDataPtr->color = D3DXVECTOR4(0.6f, 0.0f, 0.0f, 0.01f);
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

	m_renderer->EnableAlphaBlending(true);

	// Render the triangle.
	m_renderer->GetDeviceContext()->DrawIndexed(m_indexCount, 0, 0);

	m_renderer->EnableAlphaBlending(false);
}

/*
*	\breif 
*/
void CGizmo::Control( 
		CInput *input,									//!< 
		CTerrain *terrain,								//!<
		CCamera *camera,								//!< 
		CKinect *kinect									//!< 
	)
{
	if (terrain->GetFlag(TERRAIN_FLAG_LOCK) == true) {
		return;
	}

	if (kinect == nullptr)
	{
		m_inputType = InputType::Mouse;
	}

	IBrush *const brush = m_brush[m_currentBrush];	

	if (m_inputType == InputType::Mouse)
	{
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
			D3DXVECTOR2 mousePos = input->GetMousePosition();

			CVisCraft *vis = CVisCraft::GetInstance();
			D3DXVECTOR2 screensize = vis->GetWindowDimension();

			int xpanDirection = 0;
			int ypanDirection = 0;

			if (mousePos.x < screensize.x * 0.1f)
			{
				mousePos.x = screensize.x * 0.1f;
				xpanDirection = -1; 
			} else if (mousePos.x > screensize.x - (screensize.x * 0.1f))
			{
				mousePos.x = screensize.x - (screensize.x * 0.1f);
				xpanDirection = 1; 
			}

			if (mousePos.y < screensize.y * 0.6f)
			{
				mousePos.y = screensize.y * 0.6f;
				ypanDirection = 1; 
			} else if (mousePos.y > screensize.y - (screensize.y * 0.1f))
			{
				mousePos.y = screensize.y - (screensize.y * 0.1f);
				ypanDirection = -1; 
			}

			if (xpanDirection != 0 || ypanDirection != 0)
			{
				D3DXVECTOR3 campos = camera->GetPosition();
				campos.x += xpanDirection * 0.5f;
				campos.z += ypanDirection * 0.5f;
				camera->SetPosition(campos.x, campos.y, campos.z);
			}

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

			const D3DXVECTOR2 terrainSize = terrain->GetSize();

			m_position.x = m_position.x < 0 ? 0 : m_position.x > terrainSize.x - 1 ? terrainSize.x - 1 : m_position.x;
			m_position.z = m_position.z < 0 ? 0 : m_position.z > terrainSize.y - 1 ? terrainSize.y - 1 :  m_position.z;

		}

		brush->Apply(this, input, terrain);
	}
	else if (m_inputType == InputType::Kinect)
	{
		if (brush->IsLockable())
		{
			if (kinect->GetHandState() == HandState::ClosedFist)
			{
				if (m_gizmoState != GizmoState::Locked)
				{
					m_dragData.startMousePosition = kinect->GetHandPosition();
					m_dragData.lastY = m_dragData.startMousePosition.y;
					m_gizmoState = GizmoState::Locked;
				}		
			}
			else if (kinect->GetHandState() == HandState::OpenHand)
			{
				if (m_gizmoState == GizmoState::Locked)
				{
					m_gizmoState = GizmoState::Free;
				}
			}
		}

		if (m_gizmoState == GizmoState::Free)
		{
			D3DXVECTOR2 mousePos = kinect->GetHandPosition();

			CVisCraft *vis = CVisCraft::GetInstance();
			D3DXVECTOR2 screensize = vis->GetWindowDimension();

			int xpanDirection = 0;
			int ypanDirection = 0;

			if (mousePos.x < screensize.x * 0.1f)
			{
				mousePos.x = screensize.x * 0.1f;
				xpanDirection = -1; 
			} else if (mousePos.x > screensize.x - (screensize.x * 0.1f))
			{
				mousePos.x = screensize.x - (screensize.x * 0.1f);
				xpanDirection = 1; 
			}

			if (mousePos.y < screensize.y * 0.6f)
			{
				mousePos.y = screensize.y * 0.6f;
				ypanDirection = 1; 
			} else if (mousePos.y > screensize.y - (screensize.y * 0.1f))
			{
				mousePos.y = screensize.y - (screensize.y * 0.1f);
				ypanDirection = -1; 
			}

			if (xpanDirection != 0 || ypanDirection != 0)
			{
				D3DXVECTOR3 campos = camera->GetPosition();
				campos.x += xpanDirection * 0.5f;
				campos.z += ypanDirection * 0.5f;
				camera->SetPosition(campos.x, campos.y, campos.z);
			}

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

			const D3DXVECTOR2 terrainSize = terrain->GetSize();

			m_position.x = m_position.x < 0 ? 0 : m_position.x > terrainSize.x - 1 ? terrainSize.x - 1 : m_position.x;
			m_position.z = m_position.z < 0 ? 0 : m_position.z > terrainSize.y - 1 ? terrainSize.y - 1 :  m_position.z;
		}

		brush->Apply(this, kinect, terrain);
	}
}

void CGizmo::SetCurrentBrush( 
		const BrushType::Enum brushType 
	)
{
	m_currentBrush = brushType;
}

IBrush *CGizmo::GetCurrentBrush()
{
	return m_brush[m_currentBrush];
}
