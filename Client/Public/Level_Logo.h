#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	/* �� ���ӳ��� ����� �����ϴ� ��ü���� �߰��Ѵ�. */
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Sound();

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END