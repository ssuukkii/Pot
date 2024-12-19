#pragma once

#include "Base.h"

/* 1. ȭ�鿡 �׷����� ��ü���� �׸��� ������� �����Ѵ�. */
/* 2. �����ϰ� �ִ� ��ü���� �����Լ��� ȣ���Ѵ�.(������) */

BEGIN(Engine)

class CCollider;

class CCollider_Manager final : public CBase
{
public:
	enum COLLIDERGROUP {
		CG_1P_BODY,
		CG_1P_Energy_Attack,
		CG_1P_Ranged_Attack,   // 1P ���Ÿ� ����
		CG_1P_Melee_Attack,    // 1P ���� ����
		CG_1P_REFLECT,
		CG_2P_BODY,
		CG_2P_Energy_Attack,
		CG_2P_Ranged_Attack,   // 2P ���Ÿ� ����
		CG_2P_Melee_Attack,    // 2P ���� ����
		CG_2P_REFLECT,
		CG_EFFECT_LAYER,
		CG_END
	};

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	void Update(_float fTimeDelta);

public:
	HRESULT Add_ColliderObject(COLLIDERGROUP eRenderGroup, CCollider* pColliderCom);
	HRESULT Check_Collision(_float);

	_bool IsColliding(CCollider* a, CCollider* b);
	HRESULT Release_Collider(const CCollider* Collider);
	HRESULT Destory_ColliderGroup();
	void	Destroy_Reserve(COLLIDERGROUP eRenderGroup);
	void	Destroy_Reserve(CCollider* pCollider);       // ���� �ݶ��̴� ���� ���� �Լ�

private:
	void Process_1P_Body_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_Skill_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Body_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_Skill_2P_Body_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);

	void Process_1P_Ranged_Skill_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Ranged_Skill_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Ranged_Skill_2P_Ranged_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_Skill_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Body_2P_Ranged_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_Skill_2P_Ranged_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Body_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Range_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_2P_Range_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Reflect_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Reflect_2P_Range_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Reflect_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Energy_2P_Reflect_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Range_2P_Reflect(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);
	void Process_1P_Melee_2P_Reflect(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions);


private:
	list<CCollider*>			m_Colliders[CG_END];
	list<COLLIDERGROUP>			m_Destroy_Reserve_Collider_Group;
	list<CCollider*>			m_Destory_Reserve_Collider; // ���� �ݶ��̴� ���� ���� ����Ʈ

	//m_CollisionHistory: ���� �����ӿ����� �浹 ���¸� �����ϴ� ���Դϴ�.
	// Ű�� �� �ݶ��̴��� �����͸� ���� ��(pair)�̰�, ���� �� ���� �浹 ������ ���θ� ��Ÿ���� �ο� ���Դϴ�.
	map<pair<CCollider*, CCollider*>, _bool> m_CollisionHistory;
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void ProcessCollisionResults(_float fTimeDelta);
private:
	vector<pair<CCollider*, CCollider*>> m_CollisionResults;
	mutex m_ResultMutex;

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};
END