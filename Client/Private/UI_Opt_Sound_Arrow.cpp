#include "stdafx.h"

#include "UI_Opt_Sound_Arrow.h"
#include "RenderInstance.h"

CUI_Opt_Sound_Arrow::CUI_Opt_Sound_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Opt_Sound{ pDevice ,pContext }
{
}

CUI_Opt_Sound_Arrow::CUI_Opt_Sound_Arrow(const CUI_Opt_Sound_Arrow& Prototype)
	:CUI_Opt_Sound{ Prototype }
{
}

HRESULT CUI_Opt_Sound_Arrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound_Arrow::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = 390.f, m_fPosY = 320.f;
	m_fSizeX = 50.f, m_fSizeY = 50.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Opt_Sound_Arrow::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Opt_Sound_Arrow::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(m_bKeyInput)
		PostionUpdate();

}

void CUI_Opt_Sound_Arrow::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Opt_Sound_Arrow::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound_Arrow::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptArrow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_Opt_Sound_Arrow::PostionUpdate()
{

	switch (m_eMenuValue)
	{
	case BGM:
		 m_fPosY = 320.f;
		break;

	case VOICE:
		m_fPosY = 360.f;
		break;

	case SFX:
		m_fPosY = 400.f;
		break;


	case EXIT:
		m_fPosY = 440.f;
		break;
	
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

}

CUI_Opt_Sound_Arrow* CUI_Opt_Sound_Arrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Opt_Sound_Arrow* pInstatnce = new CUI_Opt_Sound_Arrow(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Opt_Sound_Arrow"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Opt_Sound_Arrow::Clone(void* pArg)
{
	CUI_Opt_Sound_Arrow* pInstatnce = new CUI_Opt_Sound_Arrow(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Opt_Sound_Arrow"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Opt_Sound_Arrow::Free()
{
	__super::Free();
}