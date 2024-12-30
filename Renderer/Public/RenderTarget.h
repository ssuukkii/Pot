#pragma once

#include "Base.h"
#include "Renderer_Defines.h"

/* ·»´õÅ¸°ÙÇÏ³ª¸¦ ÀÇ¹ÌÇÏ´Â °´Ã¼´Ù. */

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Renderer)

class RENDERER_DLL CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

	ID3D11RenderTargetView* Get_RTV_Debug() const {
		return m_pRTV_D;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor, const _wstring& strTargetTag);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	void Clear();
	HRESULT Copy_RenderTarget(ID3D11Texture2D* pTexture2D);
	
	ID3D11ShaderResourceView* Copy_ShaderResourceView();

	_wstring Get_TargetTag() { return m_strTargetTag; }
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer,_float4x4 WorldMatrix);
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	_float4						m_vClearColor = {};

	_wstring m_strTargetTag;
#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
#endif

	ID3D11Texture2D* m_pTexture2D_D = { nullptr };
	ID3D11RenderTargetView* m_pRTV_D = { nullptr };
	ID3D11ShaderResourceView* m_pSRV_D = { nullptr };
public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor, const _wstring& strTargetTag = L"");
	virtual void Free() override;
};

END