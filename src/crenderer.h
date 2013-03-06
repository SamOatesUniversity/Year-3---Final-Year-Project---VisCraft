#pragma once

/**
	Header file includes
*/
#include "helper.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dx10math.h>

/**
	Additionally included libraries
*/

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")

class CRenderer {
private:

	ID3D11Device				*m_device;								//!< The d3d11 device
	IDXGISwapChain				*m_swapChain;							//!< The device swap chain
	ID3D11DeviceContext			*m_deviceContext;						//!< The device context

	D3D11_VIEWPORT				m_viewport;								//!< The devices viewport

	ID3D11RenderTargetView		*m_renderTargetView;					//!< The render target view
	ID3D11Texture2D				*m_depthStencilBuffer;					//!< The depth stencil buffer
	ID3D11DepthStencilState		*m_depthStencilState;					//!< The depth stencil state
	ID3D11DepthStencilView		*m_depthStencilView;					//!< The depth stencil view

	ID3D11RasterizerState		*m_rasterState;							//!< The raster state

	D3DXMATRIX					m_projectionMatrix;						//!< The projection matrix
	D3DXMATRIX					m_worldMatrix;							//!< The world matrix
	D3DXMATRIX					m_orthoMatrix;							//!< 

	ID3D11DepthStencilState		*m_depthDisabledStencilState;			//!< The disabled death stencil state
	ID3D11BlendState			*m_alphaEnableBlendingState;			//!< The enabled alpa blend state
	ID3D11BlendState			*m_alphaDisableBlendingState;			//!< The disabled alpha blend state

public:
								//! Constructor
								CRenderer();
								
								//! Destructor
								~CRenderer();

								//! Create and setup the render device
	const bool					Create(
									const HWND hwnd,					//!< Window handle to render too
									const int screenWidth,				//!< The screen width
									const int screenHeight				//!< The screen height
								);

								//! Free any allocations made by this class
	void						Release();

								//! Ready the scene for a render pass
	void						BeginScene( 
									const float red,					//!< Red color clear value
									const float green,					//!< Green color clear value
									const float blue					//!< Blue color clear value
								);

								//! Present the final render to the screen
	void						EndScene();

								//! Get the world matrix
	void						GetWorldMatrix( 
									D3DXMATRIX &worldMatrix				//!< Reference of the world matrix
								);
	
								//! Get the projection matrix
	void						GetProjectionMatrix( 
									D3DXMATRIX &projectionMatrix		//!< Reference of the projection matrix
								);

								//! Get the orthographic matrix
	void						GetOrthoMatrix( 
									D3DXMATRIX &orthoMatrix 
								);

								//! Get the d3d11 device
	inline ID3D11Device			*GetDevice()
								{
									return m_device;
								}

								//! Get the d3d11 device context
	inline ID3D11DeviceContext	*GetDeviceContext()
								{
									return m_deviceContext;
								}

								//! Get the devices viewport
	D3D11_VIEWPORT				GetViewPort();

								//! Enable or disable the zbuffer
	void						EnableZBuffer(
									bool enable
								);

								//! Enable or disable alpha blending
	void						EnableAlphaBlending(
									bool enable
								);

};

