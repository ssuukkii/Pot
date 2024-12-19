#pragma once

#include "Base.h"
#include "Renderer_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Renderer)

class CLight_Manager final : public CBase
{
public:
	enum LIGHT_TYPE { LIGHT_BACKGROUND, LIGHT_PLAYER, LIGHT_EFFECT };
private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Manager() = default;


public:

	LIGHT_DESC* Get_LightDesc(LIGHT_TYPE eLightType, _uint iLightIndex, string strName = "");
	void		Remove_LightDesc(string strName);
	_int		Check_EffectLights();
public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	void BGLight_Pop_Front();
	HRESULT Add_Player_Light(string strKey, const LIGHT_DESC& LightDesc, _float4 vChaseColor = { 1.f,1.f,1.f,1.f }, _bool* pisChaseLight = { nullptr });
	HRESULT Add_Effect_Light(string strKey, const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(LIGHT_TYPE eLightType, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, string strName, _float fTimeDelta);
	void Clear_Light();
private:
	list<class CLight*>				m_Lights;
	// �÷��̾� ����Ʈ Ű�� �˻����� �ڱ� �� ��� ���� ������� ���� ����
	map<string, class CLight*>		m_PlayerLights; // �� �÷��̾�� �ٶ󺸴� �������� ���� �ް� �ִ�

	map<string, class CLight*>		m_EffectLights; // �ܼ��� ���� �ٲ� ���� ����Ʈ ó���� �� ���� ����ó���������

	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

public:
	static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END