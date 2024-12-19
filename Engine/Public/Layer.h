#pragma once

#include "Base.h"

/* ��ü���� ����Ʈ�� �����Ѵ�. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Get_GameObject(_uint iIndex);
	list<class CGameObject*> Get_Object_List() { return m_GameObjects; }

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);

	//���� �������
	HRESULT Add_List(list<CGameObject*>*);

	void Player_Update(_float fTimeDelta);
	void Camera_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	_uint GetSize()
	{
		return m_GameObjects.size();
	}

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END