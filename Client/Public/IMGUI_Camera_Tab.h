#pragma once
#include "IMGUI_Tab.h"
#include "Camera.h"
#include "Virtual_Camera.h"

BEGIN(Client)

class CIMGUI_Camera_Tab : public CIMGUI_Tab
{
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
		}
	};

	//���� ����������
	enum CAMERA_MODELID { CAMERA_MODELID_NOT = -1, CAMERA_MODELID_NORMAL, CAMERA_MODELID_FREE, CAMERA_MODELID_SON, CAMERA_MODELID_21, CAMERA_MODELID_HIT, CAMERA_MODELID_FRIEZA, CAMERA_MODELID_END };
	//��ų�� �� �𵨸��� enum�� �޶� map
protected:
	CIMGUI_Camera_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Camera_Tab() = default;

public:
	virtual void Effect_Menu() override;
	virtual void Effect_Transform() override;
	virtual void Camera_Set(_uint iNumCameraModel) override;
	HRESULT Initialize() override;
	void Render(_float fTimeDelta) override;
	virtual const _char* GetTabName() const { return "Camera"; };

private:
	void IMGUI_Camera_Select_Model(_float fTimeDelta);
	void IMGUI_Camera_Select_Skill(_float fTimeDelta);
	void IMGUI_Camera_Select_Animation(_float fTimeDelta);

	void IMGUI_Save_Button();
	void UpdateCameraSelection();
	void IMGUI_Show_Points(_float fTimeDelta);	   // ���� ���� ī�޶��� ����Ʈ�� ������� �����ֱ�
	void IMGUI_Button();        // Add_Point ��ư ó�� �Լ�
	void IMGUI_Delete_Points();
	void IMGUI_Play_Button();	   // Play ��ư
	void IMGUI_Pause_Button();	   // Pause ��ư
	void IMGUI_Stop_Button();	   // Stop ��ư
	void IMGUI_Delete_Point(_int index); //Delete ��ư
	void IMGUI_Modify_Point(_int index); //Modify ��ư
	void IMGUI_Modify_Point_UI(_int index);
	void IMGUI_Point_Modify_Save();

	const _float4x4* Get_Model_Float4x4();
	void VisualizeCameraPoints(const vector<CameraPoint>& points, _float fTimeDelta);
	void DrawDebugText(const _float3& position, const std::wstring& text, const _float4& color);

private:
	CAMERA_MODELID m_iSelected_Model = CAMERA_MODELID_NORMAL;  // �� ���� ���¸� ����
	_int  m_iSelected_Skill = -1;
	_int m_iSelected_Animation = -1; // �ִϸ��̼� ���� ���¸� ���� (�ε��� ���)

	// �𵨺� ��ų ���
	unordered_map<CAMERA_MODELID, vector<string>> m_ModelSkills;
	// �� �̸� �迭 ����
	const char* MODEL_NAMES[CAMERA_MODELID_END];

	// ��ų�� �ִϸ��̼� �ε��� ���� Ű�� �ް� �ִϸ��̼� ����� ������ ���� ��
	unordered_map<pair<CAMERA_MODELID, int>, vector<string>, pair_hash> m_SkillAnimations;

	_int m_selectedPoint = -1;
	_bool m_isEditing = { false };     // ���� ��� ����
	_bool m_isCompleteCameraSelect = { true };
	//�𵨰� ��ų pair���� Ű�� �ް� ����ī�޶� �ε����� ������ ����
	unordered_map<pair<CAMERA_MODELID, _int>, _uint, pair_hash> m_CameraIndexMap;


	// �ӽ� ������ ����ü
	struct TempPointData {
		_float duration;
		CVirtual_Camera::InterpolationType interpType;
		_float damping = { 1.f };
	};

	TempPointData m_tempPointData;

	//����ī�޶�
	class CMain_Camera* m_pMainCamera = { nullptr };
	class CLine_Draw* m_pLineDraw = { nullptr };
public:
	static CIMGUI_Camera_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
