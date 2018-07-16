// Self Include:
#include "DirectX.h"

// Local Includes:

// Library Includes:

// Implementation:
CDirectX::CDirectX()
{


}

CDirectX::~CDirectX()
{
	ReleaseCOM(m_pRenderTargetView);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pSwapChain);
	ReleaseCOM(m_pDepthStencilBuffer);
	ReleaseCOM(m_pD3DDevice);
}

bool CDirectX::Initialise(HWND _hWnd, int _iWidth, int _iHeight)
{
	// window settings
	m_hWnd = _hWnd;
	m_iWidth = _iWidth;
	m_iHeight = _iHeight;

	// create swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferDesc.Width  = _iWidth;
	swapChainDesc.BufferDesc.Height = _iHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count   = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount  = 1;
	swapChainDesc.OutputWindow = _hWnd;
	swapChainDesc.Windowed     = true;
	swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags        = 0;

	// create device & swap chain
	if(FAILED(D3D10CreateDeviceAndSwapChain(0, D3D10_DRIVER_TYPE_HARDWARE, 0, 0, D3D10_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pD3DDevice)))
	{
		return(false);
	}

	ID3D10Texture2D* pBackBuffer;

	if(FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&pBackBuffer))))
	{
		return(false);
	}

	if(FAILED(m_pD3DDevice->CreateRenderTargetView(pBackBuffer, 0, &m_pRenderTargetView)))
	{
		return(false);
	}

	ReleaseCOM(pBackBuffer);

	// Set the viewport transform
	D3D10_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width    = m_iWidth;
	viewPort.Height   = m_iHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	m_pD3DDevice->RSSetViewports(1, &viewPort);	

	D3D10_RASTERIZER_DESC rasterDesc;

	// Setup the raster description which will determine how and what polygons will be drawn
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D10_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D10_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out (wireframe)
	if(FAILED(m_pD3DDevice->CreateRasterizerState(&rasterDesc, &m_pRasterStateWireFrame)))
	{
		return(false);
	}

	// Solid raster
	rasterDesc.FillMode = D3D10_FILL_SOLID;

	if(FAILED(m_pD3DDevice->CreateRasterizerState(&rasterDesc, &m_pRasterStateSolid)))
	{
		return(false);
	}

	// Now set the rasterizer state
	m_pD3DDevice->RSSetState(m_pRasterStateSolid);

		// Create the depth/stencil buffer and view
	D3D10_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = m_iWidth;
	depthStencilDesc.Height    = m_iHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT
	depthStencilDesc.SampleDesc.Count   = 1; 
	depthStencilDesc.SampleDesc.Quality = 0; 
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	if(FAILED(m_pD3DDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_pDepthStencilBuffer)))
	{
		return(false);
	}

	if(FAILED(m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView)))
	{
		return(false);
	}

	// Bind the render target view and depth/stencil view to the pipeline
	m_pD3DDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	
	// Create the projection matrix for 3D rendering
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, (float)D3DX_PI / 4.0f, (float)m_iWidth / (float)m_iHeight, 1.0f, 1000.0f);

	return(true);
}

void CDirectX::SetWireFrameMode(bool _bOn)
{
	if(_bOn)
	{
		m_pD3DDevice->RSSetState(m_pRasterStateWireFrame);
	}
	else
	{
		m_pD3DDevice->RSSetState(m_pRasterStateSolid);
	}
}
	
void CDirectX::BeginScene()
{
	// Clear the back buffer.
	m_pD3DDevice->ClearRenderTargetView(m_pRenderTargetView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
    
	// Clear the depth buffer.
	m_pD3DDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

void CDirectX::EndScene()
{
	// present backbuffer
	m_pSwapChain->Present(0, 0);
}

ID3D10Device* CDirectX::GetDevice()
{
	return(m_pD3DDevice);
}

D3DXMATRIX CDirectX::GetProjectionMatrix()
{
	return(m_projectionMatrix);
}
