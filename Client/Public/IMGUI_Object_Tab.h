#pragma once
#include "IMGUI_Tab.h"

BEGIN(Engine)
class CGameObject;
class CContainerObject;
class CCollider;
END

BEGIN(Client)

class CIMGUI_Object_Tab : public CIMGUI_Tab
{

protected:
	CIMGUI_Object_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Object_Tab() = default;

public:
	virtual void Effect_Menu() override;
	virtual void Effect_Transform() override;
	virtual void Camera_Set(_uint iNumCameraModel) override;
	HRESULT Initialize() override;
	void Render(_float fTimeDelta) override;

	virtual const _char* GetTabName() const { return "Object"; };

private:
	HRESULT IMGUI_Show_PrototypeObjects();
	HRESULT IMGUI_SpawnObjectAtZero(const std::string& type);
	HRESULT IMGUI_Show_LayerObjects();
	HRESULT IMGUI_Save_Button_Pressed(_bool*, _bool*);
	HRESULT IMGUI_Click_Button_Release(unordered_set<CGameObject*>& container, CGameObject* gameObject);
	HRESULT IMGUI_CopySelectedObject(const wstring& layerName, CGameObject* sourceObject);

	void ShowLayerList(const vector<pair<string, list<class CGameObject*>>>& objectLayersVector, _int& selectedItem);
	void ShowObjectListAndControls(const vector<pair<string, list<class CGameObject*>>>& objectLayersVector, _int selectedItem);
	void HandleContainerObject(class CContainerObject* containerObject);
	void HandleAABBColliderObject(class CGameObject* colliderObject);
	void HandleObjectPosition(class CGameObject* _pSelectedGameObject);
	void HandleColliderTransform(class CCollider* _pCollider);
	void HandleObjectRotation(class CGameObject* _pSelectedGameObject);
	void HandleObjectScale(class CGameObject* _pSelectedGameObject);
	void HandleObjectManagement(unordered_set<class CGameObject*>& container, class CGameObject* _pSelectedGameObject, const string& layerName);
	void HandleMultipleObjectControl(const unordered_set<class CGameObject*>& _hashSet_selectedGameObjects);

private:

	// ���� ������Ʈ
	_bool m_bIsClickedPrototype = { false };
	string m_SelectedPrototype;
	class CGameObject* m_pCurrentMovingObject = { nullptr };
	// ��ü �����͸� Ű��, ȸ�� ���¸� ������ �����ϴ� ��
	unordered_map<class CGameObject*, _float3> m_hashTable_Rotation;

	//�ߺ� ���ð� ���õ� �����̳�
	unordered_set<class CGameObject*> m_hashSet_selectedGameObjects;
	unordered_set<class CGameObject*> m_hashSet_previousSelectedGameObjects;
	//���� �ߺ��� ��� �����ؾ��ϱ� ������ ������� ����
	_float3 m_uiDeltaPosition = { 0.0f, 0.0f, 0.0f };
	_float3 m_uiDeltaRotation = { 0.0f, 0.0f, 0.0f };
	_float3 m_uiDeltaScale = { 0.0f, 0.0f, 0.0f };

	_bool bMovementEnabled = { false };
	vector<class CGameObject*> m_lastSelectedGameObjects;
	_float3 m_averageRotation = {};

public:
	static CIMGUI_Object_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
