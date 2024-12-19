#include "..\public\Graphic_Device.h"

CGraphic_Device::CGraphic_Device()
	: m_pDevice{ nullptr }
	, m_pDeviceContext{ nullptr }
{

}

HRESULT CGraphic_Device::Initialize(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	/* �׷��� ��ġ�� �ʱ�ȭ�Ѵ�. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	/* SwapChain ����� �ĸ���۸� �����ư��� ȭ�鿡 �����ش�.(Present) */

	/* ����۸� �����ϱ� ���� ID3D11Texture2D ����ž�. */
	if (FAILED(Ready_SwapChain(hWnd, isWindowed, iWinSizeX, iWinSizeY)))
		return E_FAIL;

	/* ����ü���� ��� �ִ� �ؽ��� 2D�� �����ͼ� �̸� �������� ����� ����Ÿ�� �並 �����.*/
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinSizeX, iWinSizeY)))
		return E_FAIL;

	/* ��ġ�� ���ε��س��� ����Ÿ�ٵ�� �������ٽǺ並 �����Ѵ�. */
	/* ��ġ�� ���ÿ� �ִ� 8���� ����Ÿ���� ��� ���� �� �ִ�. */
	ID3D11RenderTargetView* pRTVs[1] = {
		m_pBackBufferRTV,

	};

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs,
		m_pDepthStencilView);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWinSizeX;
	ViewPortDesc.Height = (_float)iWinSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDevice = m_pDevice;
	*ppContext = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* ����۸� �ʱ�ȭ�Ѵ�.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	/* ���� ���ۿ� �ĸ� ���۸� ��ü�Ͽ� �ĸ� ���۸� �������� �����ִ� ������ �Ѵ�. */
	/* �ĸ� ���۸� ���� ȭ�鿡 �����ٰ�. */
	return m_pSwapChain->Present(0, 0);
}


HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* ����ü���� �����Ѵ�. = �ؽ��ĸ� �����ϴ� ���� + �����ϴ� ����  */
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/*�ؽ�ó(�����)�� �����ϴ� ����*/
	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;

	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	SwapChain.BufferCount = 1;

	/*�����ϴ� ����*/
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	//_uint maxQuality;
	//m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &maxQuality);
	SwapChain.SampleDesc.Count = 1;
	SwapChain.SampleDesc.Quality = 0;


	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = isWindowed;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* ����۶�� �ؽ�ó�� �����ߴ�. */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;



	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}


HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	// ���� ü���� ��� �ִ� ����� �ؽ�ó ��������
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	// ����۷κ��� RenderTargetView ����
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
	{
		Safe_Release(pBackBufferTexture);
		return E_FAIL;
	}

	// ����۷κ��� ShaderResourceView ����
	if (FAILED(m_pDevice->CreateShaderResourceView(pBackBufferTexture, nullptr, &m_pBackBufferSRV)))
	{
		Safe_Release(pBackBufferTexture);
		return E_FAIL;
	}

	// ����� �ؽ�ó ���� (�䰡 ������ �����ϰ� ����)
	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;



	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	/* m_pBackBufferSRV */

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

CGraphic_Device* CGraphic_Device::Create(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDevice, ppDeviceContextOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CGraphic_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDepthTexture);
	Safe_Release(m_pBackBufferSRV); 
	
	//#if defined(DEBUG) || defined(_DEBUG)
	//	ID3D11Debug* d3dDebug;
	//	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	//	if (SUCCEEDED(hr))
	//	{
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//
	//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//	}
	//	if (d3dDebug != nullptr)            d3dDebug->Release();
	//#endif

		
	Safe_Release(m_pDevice);
}