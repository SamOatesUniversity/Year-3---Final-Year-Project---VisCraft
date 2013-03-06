#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

class CTextureShader {
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

private:
	ID3D11VertexShader										*m_vertexShader;
	ID3D11PixelShader										*m_pixelShader;
	ID3D11InputLayout										*m_layout;
	ID3D11Buffer											*m_matrixBuffer;
	ID3D11SamplerState										*m_sampleState;

private:
															//! 
	bool													InitializeShader(
																ID3D11Device *device, 
																char* vertexShaderPath, 
																char* pixelShaderPath
															);
	
															//!
	void													DestroyShader();

															//! 
	bool													SetShaderParameters(
																ID3D11DeviceContext *deviceContext, 
																D3DXMATRIX worldMatrix, 
																D3DXMATRIX viewMatrix, 
																D3DXMATRIX projectionMatrix, 
																ID3D11ShaderResourceView *texture
															);

															//! 
	void													RenderShader(
																ID3D11DeviceContext *deviceContext, 
																int indexCount
															);

public:
															//! 
															CTextureShader();

															//! 
															~CTextureShader();

															//! 
	bool													Create(
																ID3D11Device *device
															);

															//! 
	void													Destroy();

															//! 
	bool													Render(
																ID3D11DeviceContext *deviceContext, 
																int indexCount, 
																D3DXMATRIX worldXform,
																D3DXMATRIX viewXform, 
																D3DXMATRIX projectionXform, 
																ID3D11ShaderResourceView* texture
															);


};