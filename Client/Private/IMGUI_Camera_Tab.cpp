#include "stdafx.h"
#include "IMGUI_Camera_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Main_Camera.h"
#include "Virtual_Camera.h"
#include "Line_Draw.h"
#include "Character.h"
_bool bShowCameraWindow = true;  // IMGUI â ǥ�� ���θ� �����ϴ� ���� ����
_wstring filename = L"../Bin/CameraPoints.txt"; //������ ����Ǵ� txt


CIMGUI_Camera_Tab::CIMGUI_Camera_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}

void CIMGUI_Camera_Tab::Effect_Menu()
{
}

void CIMGUI_Camera_Tab::Effect_Transform()
{
}

void CIMGUI_Camera_Tab::Camera_Set(_uint iNumCameraModel)
{
	m_iSelected_Model = static_cast<CAMERA_MODELID>(iNumCameraModel);
	UpdateCameraSelection();
}

HRESULT CIMGUI_Camera_Tab::Initialize()
{
	// �� �̸� �迭 �ʱ�ȭ
	MODEL_NAMES[CAMERA_MODELID_NORMAL] = "Normal";
	MODEL_NAMES[CAMERA_MODELID_FREE] = "Free";
	MODEL_NAMES[CAMERA_MODELID_SON] = "Son";
	MODEL_NAMES[CAMERA_MODELID_21] = "21";
	MODEL_NAMES[CAMERA_MODELID_HIT] = "Hit";
	MODEL_NAMES[CAMERA_MODELID_FRIEZA] = "Frieza";

	_int index = 0;

	// �𵨺� ��ų ��� �ʱ�ȭ
	m_ModelSkills[CAMERA_MODELID_SON] = { "Camera_Son_Heavy", "Camera_Son_Knock_Away_Up","Camera_Son_Air_Smash","Camera_Son_Grab","Camera_Son_Same_Grab","Camera_Son_Energy","Camera_Son_Ultimate" };
	m_ModelSkills[CAMERA_MODELID_21] = { "Camera_21_Heavy", "Camera_21_Knock_Away_Up", "Camera_21_Air_Smash","Camera_21_Grab","Camera_21_Same_Grab","Camera_21_Grab_Special","Camera_21_Energy","Camera_21_Ultimate" };
	m_ModelSkills[CAMERA_MODELID_HIT] = { "Camera_Hit_Heavy", "Camera_Hit_Knock_Away_Up","Camera_Hit_Grab","Camera_Hit_214_Middle","Camera_Hit_236_Middle","Camera_Hit_236_Heavy","Camera_Hit_1_Ultimate","Camera_Hit_3_Ultimate" };
	m_ModelSkills[CAMERA_MODELID_FRIEZA] = { "Camera_Frieza_Light_Final", "Camera_Frieza_Heavy", "Camera_Frieza_Knock_Away_Up","Camera_Frieza_Grab","Camera_Frieza_Ultimate_1","Camera_Frieza_Ultimate_3","Camera_Frieza_Golden", "Camera_Frieza_Down_Forward_J","Camera_Frieza_Fall_Reflect","Camera_Common_Appear","Camera_Goku_Entry","Camera_21_Entry","Camera_Hit_Entry","Camera_Frieza_Entry","Camera_Goku_vs_Frieza_Entry","Camera_Hit_Win"};

	// �𵨰� ��ų �ε����� ���� ī�޶� �ε��� ���� �ʱ�ȭ
	// �⺻ ī�޶� ����
	m_CameraIndexMap[{CAMERA_MODELID_NORMAL, -1}] = index++;
	m_CameraIndexMap[{CAMERA_MODELID_FREE, -1}] = index++;

	// �� �𵨰� �׿� �ش��ϴ� ��ų�� ��ȸ�ϸ� ���� ����
	for (const auto& modelSkillPair : m_ModelSkills)
	{
		CAMERA_MODELID model = modelSkillPair.first;
		const std::vector<std::string>& skills = modelSkillPair.second;

		for (int skillIdx = 0; skillIdx < static_cast<int>(skills.size()); ++skillIdx)
		{
			m_CameraIndexMap[{model, skillIdx}] = index++;

			// ��ų�� �ִϸ��̼� ��� �ʱ�ȭ (���� ������)
			if (model == CAMERA_MODELID_SON)
			{
				if (skillIdx == 0)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Heavy_Anim1" };
				}
				else if (skillIdx == 1)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 2)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Air_Smash_Anim1" };
				}
				else if (skillIdx == 3)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Grab_Anim1" };
				}
				else if (skillIdx == 4)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Same_Grab_Anim1" };
				}
				else if (skillIdx == 5)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Energy_Anim1" };
				}
				else if (skillIdx == 6)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Son_Ultimate_Anim1", "Son_Ultimate_Anim2", "Son_Ultimate_Anim3", "Son_Ultimate_Anim4", "Son_Ultimate_Anim5" };
				}
			}
			else if (model == CAMERA_MODELID_21)
			{
				if (skillIdx == 0) // 21_Skill1
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Heavy_Anim1" };
				}
				else if (skillIdx == 1) // 21_Skill2
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 2) // 21_Skill3
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Air_Smash_Anim1" };
				}
				else if (skillIdx == 3)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Grab_Anim1" };
				}
				else if (skillIdx == 4)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Same_Grab_Anim1" };
				}
				else if (skillIdx == 5)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Grab_Special_Anim1" };
				}
				else if (skillIdx == 6)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Energy_Anim1" };
				}
				else if (skillIdx == 7)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Ultimate_Anim1", "21_Ultimate_Anim2", "21_Ultimate_Anim3_Success", "21_Ultimate_Anim3_Fail", "21_Ultimate_Anim4", "21_Ultimate_Anim4_Flip" };
				}
			}
			else if (model == CAMERA_MODELID_HIT)
			{
				if (skillIdx == 0) // 21_������
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Heavy_Anim1" };
				}
				else if (skillIdx == 1) // ����
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 2) // ���
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Grab_Anim1" };
				}
				else if (skillIdx == 3) // ��Ʈ 214 ��
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_214_Middle_Anim1" };
				}
				else if (skillIdx == 4) // ��Ʈ 236 ��
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_236_Middle_Anim1" };
				}
				else if (skillIdx == 5) // ��Ʈ 236 ��
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_236_Heavy_Anim1" };
				}
				else if (skillIdx == 6) // ��Ʈ 1��
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_1_Ultimate_Anim1","Hit_1_Ultimate_Anim2" };
				}
				else if (skillIdx == 7) // ��Ʈ 3��
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_3_Ultimate_Anim1","Hit_3_Ultimate_Anim2","Hit_3_Ultimate_Anim3","Hit_3_Ultimate_Anim4" };
				}
			}
			// �ٸ� �𵨰� ��ų�� ���� �ִϸ��̼ǵ� �����ϰ� �ʱ�ȭ
			else if (model == CAMERA_MODELID_FRIEZA)
			{
				if (skillIdx == 0) // ����� ������
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Light_Final_Anim1","Frieza_Light_Final_Anim1_Flip","Frieza_Light_Final_Anim2","Frieza_Light_Final_Anim2_Flip" };
				}
				else if (skillIdx == 1)  //������
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Heavy_Anim1" };
				}
				else if (skillIdx == 2) //������
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Knock_Away_Up_Anim1" };
				}
				else if (skillIdx == 3)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Grab_Anim1" };
				}
				else if (skillIdx == 4)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Ultimate_1_Anim1","Frieza_Ultimate_1_Anim1_Flip" };
				}
				else if (skillIdx == 5)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Ultimate_3_Anim1","Frieza_Ultimate_3_Anim1_Flip" };
				}
				else if (skillIdx == 6)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Golden_Anim1" };
				}
				else if (skillIdx == 7)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Down_Forward_J","Frieza_Down_Forward_J_Flip" };
				}
				else if (skillIdx == 8)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Fall_Reflect" };
				}
				else if (skillIdx == 9)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Common_Appear_Anim_1","Common_Appear_Anim_1_Flip" };
				}
				else if (skillIdx == 10)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Goku_Entry_Anim_1","Goku_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 11)
				{
					m_SkillAnimations[{model, skillIdx}] = { "21_Entry_Anim_1","21_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 12)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Entry_Anim_1","Hit_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 13)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Frieza_Entry_Anim_1","Frieza_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 14)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Goku_VS_Frieza_Entry_Anim_1","Goku_VS_Frieza_Entry_Anim_1_Flip" };
				}
				else if (skillIdx == 15)
				{
					m_SkillAnimations[{model, skillIdx}] = { "Hit_Win_Anim_1"};
				}
			}
		}
	}

	return S_OK;
}

void CIMGUI_Camera_Tab::Render(_float fTimeDelta)
{
	////�ʱ� ����ī�޶� ����
	if (m_pMainCamera == nullptr)
	{
		CGameObject* camera = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera"));
		m_pMainCamera = static_cast<CMain_Camera*>(camera);

		Safe_AddRef(m_pMainCamera);
		return;
	}

	//// �� ���� UI ȣ��
	//IMGUI_Camera_Select_Model(fTimeDelta);

	//// ���� ���õ� ��쿡�� ��ų ���� UI�� ǥ��
	//if (m_iSelected_Model >= CAMERA_MODELID_SON) {
	//	ImGui::Spacing();  // �� �� ����
	//	ImGui::Separator();  // ��輱 �׸���

	//	// ��ų ���� UI ȣ��
	//	IMGUI_Camera_Select_Skill(fTimeDelta);
	//}


	////���þȵǸ� -1��.
	//if (m_iSelected_Skill >= 0)
	//{
	//	ImGui::Spacing();  // �� �� ����
	//	ImGui::Separator();  // ��輱 �׸���

	//	// �ִϸ��̼� ���� UI ȣ��
	//	IMGUI_Camera_Select_Animation(fTimeDelta);
	//}

	//if (m_iSelected_Animation >= 0)
	//{
	//	// ����Ʈ �����ֱ�
	//	IMGUI_Show_Points(fTimeDelta);

	//	// Point ��ư ȣ��
	//	IMGUI_Button();
	//}

	//////�� �𵨰� �� ��ų�� ����� ī�޶� ������ �ִ� Point�� �޸������� �����ϴ� ���
	//IMGUI_Save_Button();
}

void CIMGUI_Camera_Tab::IMGUI_Camera_Select_Model(_float fTimeDelta)
{
	// Model selection dropdown
	_int iSelected_Model = static_cast<_int>(m_iSelected_Model);

	ImGui::Text("Select Model");
	if (ImGui::Combo("Model", &iSelected_Model, MODEL_NAMES, IM_ARRAYSIZE(MODEL_NAMES))) {
		// ���� ����� ��쿡�� ��ų ������ �ʱ�ȭ
		if (m_iSelected_Model != static_cast<CAMERA_MODELID>(iSelected_Model)) {
			m_iSelected_Model = static_cast<CAMERA_MODELID>(iSelected_Model); // ���ο� �𵨷� ������Ʈ
			m_iSelected_Skill = -1;  // ��ų ���� �ʱ�ȭ
			m_iSelected_Animation = -1;

		}
	}

	//ī�޶� ���� ����Ʈ�� FREE����� �� ���� �� �ٷ� ��ü
	if (m_iSelected_Model == CAMERA_MODELID_NORMAL || m_iSelected_Model == CAMERA_MODELID_FREE)
		UpdateCameraSelection();
}

void CIMGUI_Camera_Tab::IMGUI_Camera_Select_Skill(_float fTimeDelta)
{
	// ���� ���õ� �𵨿� ���� ��ų ��� ��������
	auto it = m_ModelSkills.find(m_iSelected_Model);
	if (it == m_ModelSkills.end()) {
		ImGui::Text("No skills available for this model.");
		return;
	}

	//���� ������ �ִ� string �迭 ��������
	const vector<string>& skills = it->second;

	// ��ų ����� const char* �迭�� ��ȯ
	vector<const char*> skill_options;
	for (const auto& skill : skills) {
		skill_options.push_back(skill.c_str());
	}

	// ���� ���õ� ��ų �ε��� ��������
	int iSelected_Skill = m_iSelected_Skill;

	ImGui::Text("Select Skill");
	if (ImGui::Combo("Skill", &iSelected_Skill, skill_options.data(), static_cast<int>(skill_options.size()))) {
		// ��ų ������ ����� ���
		if (m_iSelected_Skill != iSelected_Skill) {
			m_iSelected_Skill = iSelected_Skill; // ���ο� ��ų���� ������Ʈ (�ε��� ���)
			m_iSelected_Animation = -1;
			//����ī�޶��� ����ī�޶� ��ų�� �°� ����
			UpdateCameraSelection();
		}
	}
}

void CIMGUI_Camera_Tab::IMGUI_Camera_Select_Animation(_float fTimeDelta)
{
	// ���� ���õ� �𵨰� ��ų�� ���� �ִϸ��̼� ��� ��������
	auto key = make_pair(m_iSelected_Model, m_iSelected_Skill);
	auto it = m_SkillAnimations.find(key);
	if (it == m_SkillAnimations.end()) {
		ImGui::Text("No animations available for this skill.");
		return;
	}

	const vector<string>& animations = it->second;

	// �ִϸ��̼� ����� const char* �迭�� ��ȯ
	vector<const char*> animation_options;
	for (const auto& anim : animations) {
		animation_options.push_back(anim.c_str());
	}

	// ���� ���õ� �ִϸ��̼� �ε��� ��������
	int iSelected_Animation = m_iSelected_Animation;

	ImGui::Text("Select Animation");
	if (ImGui::Combo("Animation", &iSelected_Animation, animation_options.data(), static_cast<int>(animation_options.size()))) {
		// �ִϸ��̼� ������ ����� ���
		if (m_iSelected_Animation != iSelected_Animation) {
			m_iSelected_Animation = iSelected_Animation; // ���ο� �ִϸ��̼����� ������Ʈ

			if (m_iSelected_Animation >= 0) {
				// �ִϸ��̼� ���ÿ� ���� �߰� ������ �ʿ��ϸ� ���⼭ ����
			}
		}
	}
}

//��ų�� �ٲٸ�, MainCamera���� ����ī�޶� ���Ƴ������ ��
void CIMGUI_Camera_Tab::UpdateCameraSelection()
{
	auto key = make_pair(m_iSelected_Model, m_iSelected_Skill);
	auto it = m_CameraIndexMap.find(key);

	if (it != m_CameraIndexMap.end())
	{
		_int cameraIndex = it->second;
		m_pMainCamera->Set_Virtual_Camera((CMain_Camera::VIRTUAL_CAMERA)cameraIndex);
	}
	else
	{
		// ���ε� ī�޶� ���� ��� ó�� (��: �⺻ ī�޶� ���� �Ǵ� ���� �޽��� ǥ��)
		ImGui::Text("No camera mapped for this model and skill combination.");
	}
}


void CIMGUI_Camera_Tab::IMGUI_Show_Points(_float fTimeDelta) {

	//��ų�� ���� �ִϸ��̼��� ���õ��� �ʾҴٸ�
	if (m_iSelected_Animation == -1)
		return;

	//�� ��ų���� ����ī�޶� �ִµ�
	//�� ��ų�� �ִϸ��̼� �ε����� ���� Points�� �����ش�.
	vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);  // ����Ʈ ���� ��������

	if (points.empty()) {
		ImGui::Text("No camera points available.");
		return;
	}

	ImGui::Separator();
	ImGui::Text("Camera Points:");

	for (size_t i = 0; i < points.size(); ++i) {
		ImGui::PushID(static_cast<int>(i));  // �� ����Ʈ�� ���� ID �ο�

		bool isSelected = (m_selectedPoint == static_cast<int>(i));
		if (isSelected) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));  // ����� �ؽ�Ʈ
		}

		ImGui::BulletText("Point %d:", static_cast<int>(i) + 1);

		if (isSelected) {
			ImGui::PopStyleColor();  // �ؽ�Ʈ ���� ����
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(50, 0)); // 50 �ȼ��� ���� ���� (�ʿ信 ���� ����)
		ImGui::SameLine();

		// Delete ��ư
		if (ImGui::Button("Delete")) {
			// ���� Ȯ�� �˾��� ������ �߰� ���� �ʿ�
			IMGUI_Delete_Point(static_cast<int>(i));
			ImGui::PopID();
			break;  // ���� �� ���� ����
		}

		// Modify ��ư
		ImGui::SameLine();
		if (ImGui::Button("Modify")) {
			IMGUI_Modify_Point(static_cast<int>(i));
		}

		// ����Ʈ ���� ǥ�� (���� ���¿� ���� �ؽ�Ʈ ���� ����)
		if (isSelected) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));  // ����� �ؽ�Ʈ
		}

		CameraPoint& point = points[i];
		ImGui::Text("  Position: (%.2f, %.2f, %.2f)", point.position.x, point.position.y, point.position.z);
		ImGui::Text("  Quaternion: (%.2f, %.2f, %.2f)", point.rotation.x, point.rotation.y, point.rotation.z);
		ImGui::Text("  Duration: %.2f", point.duration);
		ImGui::Text("  Interpolation: %s",
			(point.interpolationType == CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE)
			? "Linear"
			: (point.interpolationType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE)
			? "Damping"
			: "Skip");

		// **Damping �� ǥ�� (Damping Mode�� ���)**
		if (point.interpolationType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE) {
			ImGui::Text("  Damping: %.2f", point.damping);
		}

		// ���õ� ����Ʈ�� ���� ���� UI ǥ��
		if (isSelected)
		{
			ImGui::PopStyleColor();  // �ؽ�Ʈ ���� ����
			IMGUI_Modify_Point_UI(static_cast<int>(i));
		}

		ImGui::Separator();
		ImGui::PopID();
	}

	_bool isPlay = m_pMainCamera->Get_IsPlay();
	if (!isPlay)
		// ����Ʈ �ð�ȭ �Լ� ȣ��
		VisualizeCameraPoints(points, fTimeDelta);
}


void CIMGUI_Camera_Tab::IMGUI_Delete_Point(_int index)
{
	// ����Ʈ ����
	m_pMainCamera->Remove_Point(index, m_iSelected_Animation);

	// ���õ� ����Ʈ�� ������ ��� ���� ����
	if (m_selectedPoint == index) {
		m_selectedPoint = -1;
	}
	else if (m_selectedPoint > index) {
		// ������ ����Ʈ ������ ����Ʈ�� �����Ǹ�, ���õ� ����Ʈ�� �ε����� �����մϴ�.
		m_selectedPoint -= 1;
	}
}

void CIMGUI_Camera_Tab::IMGUI_Modify_Point(_int index)
{
	if (m_selectedPoint == index) {
		// �̹� ���õ� ���¶�� ���� ����
		m_selectedPoint = -1;
		m_isEditing = false;
	}
	else {
		// ���ο� ����Ʈ ����
		m_selectedPoint = index;
		m_isEditing = true;

		// ���õ� ����Ʈ�� �����͸� �ӽ� ������ ����
		vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);
		if (index >= 0 && index < static_cast<int>(points.size())) {
			CameraPoint& point = points[index];
			m_tempPointData.duration = point.duration;
			m_tempPointData.interpType = (CVirtual_Camera::InterpolationType)point.interpolationType;
			m_tempPointData.damping = point.damping;
		}

		// ���õ� ����Ʈ�� ��ġ�� ȸ������ ī�޶� ������Ʈ
		m_pMainCamera->Move_Point(index, m_iSelected_Animation);
	}
}

void CIMGUI_Camera_Tab::IMGUI_Modify_Point_UI(_int index)
{
	// �ش� ����Ʈ�� ���� ���� ��������
	std::vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);

	// ���õ� ����Ʈ�� ���� ������� Ȯ��
	if (!m_isEditing || m_selectedPoint != index) {
		return;
	}

	ImGui::Indent(); // �鿩���� ����

	// Duration ���� (�ӽ� ������ ���ε�)
	ImGui::InputFloat("Duration", &m_tempPointData.duration, 0.1f, 1.0f, "%.2f");

	// Interpolation Type ���� (�ӽ� ������ ���ε�)
	const char* interp_options[] = { "Linear", "Damping", "Skip" };
	int interpIndex = static_cast<int>(m_tempPointData.interpType);
	if (ImGui::BeginCombo("Interpolation Type", interp_options[interpIndex])) {
		for (int n = 0; n < IM_ARRAYSIZE(interp_options); n++) {
			bool is_selected = (interpIndex == n);
			if (ImGui::Selectable(interp_options[n], is_selected)) {
				m_tempPointData.interpType = static_cast<CVirtual_Camera::InterpolationType>(n);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Damping ��� ���� (Damping Mode�� ���� ǥ��)
	if (m_tempPointData.interpType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE)
	{
		ImGui::SliderFloat("Damping Coefficient", &m_tempPointData.damping, 0.0f, 2.0f, "%.2f");
	}

	// "Save" ��ư �߰�
	if (ImGui::Button("Save")) {
		// ����ڿ��� ������� �˸�
		IMGUI_Point_Modify_Save();
	}

	ImGui::Unindent(); // �鿩���� ����
}

void CIMGUI_Camera_Tab::IMGUI_Point_Modify_Save()
{
	// ���õ� ����Ʈ �ε��� Ȯ��
	if (m_selectedPoint < 0) {
		return;
	}

	vector<CameraPoint>& points = m_pMainCamera->Get_VectorPoint(m_iSelected_Animation);
	if (m_selectedPoint >= static_cast<int>(points.size())) {
		return;
	}

	// ���õ� ����Ʈ�� �ӽ� ������ ����
	CameraPoint& point = points[m_selectedPoint];
	point.duration = m_tempPointData.duration;
	point.interpolationType = m_tempPointData.interpType;
	point.damping = m_tempPointData.damping;

	m_pMainCamera->Modify_Transform(m_selectedPoint, m_iSelected_Animation);
}

const _float4x4* CIMGUI_Camera_Tab::Get_Model_Float4x4()
{
	CGameObject* model = nullptr;
	CTransform* modelTransform = nullptr;

	switch (m_iSelected_Model)
	{
	case CAMERA_MODELID_NORMAL:
		return nullptr;
	case CAMERA_MODELID_SON:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0);
		break;
	case CAMERA_MODELID_21:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
		break;
	case CAMERA_MODELID_HIT:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0);
		break;
	case CAMERA_MODELID_FRIEZA:
		model = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
		break;
	}
	// �ش���� Transform���� �����Ʈ���� Ptr�� �־�� �Ѵ�.
	// �� ī�޶� ���ε� ���� Transform�� �������°͵� ������ؾ���

	modelTransform = static_cast<CTransform*>(model->Get_Component(TEXT("Com_Transform")));

	return  modelTransform->Get_WorldMatrixPtr();
}

void CIMGUI_Camera_Tab::VisualizeCameraPoints(const vector<CameraPoint>& points, _float fTimeDelta)
{
	if (points.size() < 2)
		return;

	if (m_pLineDraw == nullptr)
	{
		m_pLineDraw = static_cast<CLine_Draw*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Line_Draw")));
		return;
	}

	// **�÷��̾��� ���� �������� (1�̸� �״��, -1�̸� ����)**
	CCharacter* character = static_cast<CCharacter*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0));
	_int direction = character->Get_iDirection();


	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		const CameraPoint& point = points[i];
		const CameraPoint& nextPoint = points[i + 1];

		_float3 startPos = point.position;
		_float3 endPos = nextPoint.position;

		// **direction�� ���� ������ ����**
		if (direction == -1)
		{
			// x�� ��ȣ ����
			startPos.x = -startPos.x;
			endPos.x = -endPos.x;
		}

		if (point.hasWorldFloat4x4 && point.pWorldFloat4x4)
		{
			_vector localPos = XMLoadFloat3(&startPos);
			_matrix worldMatrix = XMLoadFloat4x4(point.pWorldFloat4x4);

			// **�����ϸ� ���Ÿ� ���� ��� ����**
			_vector modelScale;
			_vector modelRotationQuat;
			_vector modelTranslation;
			XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, worldMatrix);

			// **�����ϸ��� ���ŵ� ���� ��� �籸��**
			_matrix rotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
			_matrix translationMatrix = XMMatrixTranslationFromVector(modelTranslation);
			_matrix worldMatrixNoScale = rotationMatrix * translationMatrix;

			// ���� �������� �����ϸ��� ���ŵ� ���� ��ķ� ��ȯ
			_vector worldPosVec = XMVector3TransformCoord(localPos, worldMatrixNoScale);

			XMStoreFloat3(&startPos, worldPosVec);
		}

		if (nextPoint.hasWorldFloat4x4 && nextPoint.pWorldFloat4x4)
		{
			_vector localPos = XMLoadFloat3(&endPos);
			_matrix worldMatrix = XMLoadFloat4x4(nextPoint.pWorldFloat4x4);

			// **�����ϸ� ���Ÿ� ���� ��� ����**
			_vector modelScale;
			_vector modelRotationQuat;
			_vector modelTranslation;
			XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, worldMatrix);

			// **�����ϸ��� ���ŵ� ���� ��� �籸��**
			_matrix rotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
			_matrix translationMatrix = XMMatrixTranslationFromVector(modelTranslation);
			_matrix worldMatrixNoScale = rotationMatrix * translationMatrix;

			// ���� �������� �����ϸ��� ���ŵ� ���� ��ķ� ��ȯ
			_vector worldPosVec = XMVector3TransformCoord(localPos, worldMatrixNoScale);

			XMStoreFloat3(&endPos, worldPosVec);
		}

		m_pLineDraw->Set_LinePoints(points, startPos, endPos);
		m_pLineDraw->Render(fTimeDelta);
	}

	m_pLineDraw->TextRender(points, fTimeDelta);
}

void CIMGUI_Camera_Tab::DrawDebugText(const _float3& position, const std::wstring& text, const _float4& color)
{
	// ���� ��ǥ�� ��ũ�� ��ǥ�� ��ȯ
	_vector worldPos = XMLoadFloat3(&position);
	_matrix viewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);    // �� ��� ��������
	_matrix projMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);   // �������� ��� ��������

	_vector screenPos = XMVector3TransformCoord(worldPos, viewMatrix * projMatrix);

	// ��ũ�� ��ǥ�� �����ɴϴ�.
	_float screenX = (XMVectorGetX(screenPos) + 1.0f) * 0.5f * g_iWinSizeX;
	_float screenY = (1.0f - XMVectorGetY(screenPos)) * 0.5f * g_iWinSizeY;

	// �ؽ�Ʈ�� ȭ�鿡 ������
	//AddText(ImVec2(screenX, screenY), ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, color.w)), text.c_str());
}


void CIMGUI_Camera_Tab::IMGUI_Button()
{
	// ���� Ȱ��ȭ�� ���� ī�޶� ��������
	int cameraIndex = static_cast<int>(m_pMainCamera->Get_Virtual_Camera());

	// ����� �Է��� ������ ����
	static float duration = 1.0f;
	static int selected_interp = 0;
	static float damping = 1.0f; // �⺻ Damping ���
	const char* interp_options[] = { "Linear", "Damping", "Skip" }; // ���ο� ��� �߰�

	// Add_Point ��ư�� �Է� �ʵ� ��ġ
	ImGui::Separator();
	ImGui::Text("Add Camera Point:");
	ImGui::InputFloat("Duration", &duration, 0.1f, 1.0f, "%.2f");

	if (ImGui::BeginCombo("Interpolation Type", interp_options[selected_interp])) {
		for (int n = 0; n < IM_ARRAYSIZE(interp_options); n++) {
			bool is_selected = (selected_interp == n);
			if (ImGui::Selectable(interp_options[n], is_selected))
				selected_interp = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Damping ��� ���� (Damping Mode�� ���� ǥ��)
	if (selected_interp == 1) // "Damping" ���� ��
	{
		ImGui::SliderFloat("Damping Coefficient", &damping, 0.0f, 2.0f, "%.2f");
	}

	// Add Point�� Delete ��ư�� ���� ���ο� ��ġ
	if (ImGui::Button("Add Point")) {
		// Interpolation Type ����
		CVirtual_Camera::InterpolationType interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
		switch (selected_interp) {
		case 0:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
			break;
		case 1:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE;
			break;
		case 2:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_SKIP_MODE;
			break;
		}

		//������ ���� ������� ����Ʈ �ּ� �����ϱ�
		const _float4x4* worldMatrixPtr = Get_Model_Float4x4();
		_bool hasWorldFloat4x4 = worldMatrixPtr == nullptr ? false : true;

		// ���� ī�޶��� Add_Point �Լ��� ȣ���Ͽ� ����Ʈ �߰�
		m_pMainCamera->Add_Point(duration, interpType, worldMatrixPtr, (interpType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE) ? damping : 1.0f, hasWorldFloat4x4, m_iSelected_Animation);

		// ����ڿ��� �߰����� �˸�
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Added new camera point.");

		// �Է� �ʵ� �ʱ�ȭ
		duration = 1.0f;
		selected_interp = 0;
		damping = 1.0f;
	}

	if (ImGui::Button("Add Default_Point")) {
		// Interpolation Type ����
		CVirtual_Camera::InterpolationType interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
		switch (selected_interp) {
		case 0:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_LINEAR_MODE;
			break;
		case 1:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE;
			break;
		case 2:
			interpType = CVirtual_Camera::InterpolationType::INTERPOLATION_SKIP_MODE;
			break;
		}

		//������ ���� ������� ����Ʈ �ּ� �����ϱ�
		const _float4x4* worldMatrixPtr = Get_Model_Float4x4();
		_bool hasWorldFloat4x4 = worldMatrixPtr == nullptr ? false : true;

		// ���� ī�޶��� Add_Point �Լ��� ȣ���Ͽ� ����Ʈ �߰�
		m_pMainCamera->Add_NormalPoint(duration, interpType, worldMatrixPtr, (interpType == CVirtual_Camera::InterpolationType::INTERPOLATION_DAMPING_MODE) ? damping : 1.0f, hasWorldFloat4x4, m_iSelected_Animation);

		// ����ڿ��� �߰����� �˸�
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Added new camera point.");

		// �Է� �ʵ� �ʱ�ȭ
		duration = 1.0f;
		selected_interp = 0;
		damping = 1.0f;
	}

	// Add a small space and place Delete button on the same line
	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		m_pMainCamera->Delete_Points(m_iSelected_Animation);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "All camera points have been deleted.");
	}


	// �÷��� ��ư
	IMGUI_Play_Button();
	// �Ͻ����� ��ư
	IMGUI_Pause_Button();
	// ���� ��ư
	IMGUI_Stop_Button();
}

void CIMGUI_Camera_Tab::IMGUI_Delete_Points()
{
	//���� ����ī�޶��� ����Ʈ���� ���� �����.
	if (ImGui::Button("Delete")) {
		m_pMainCamera->Delete_Points(m_iSelected_Animation);
	}
}

void CIMGUI_Camera_Tab::IMGUI_Play_Button()
{
	ImVec2 button_size = ImVec2(20, 20);
	if (ImGui::InvisibleButton("play_button", button_size))
	{
		CGameObject* gameObject_1P = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"));
		m_pMainCamera->IMGUI_Play(m_iSelected_Animation, gameObject_1P);
	}

	ImVec2 p = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float size = 20.0f;
	ImVec2 triangle_pos[3] = {
		ImVec2(p.x + (button_size.x - size) * 0.5f, p.y),
		ImVec2(p.x + (button_size.x + size) * 0.5f, p.y + size / 2),
		ImVec2(p.x + (button_size.x - size) * 0.5f, p.y + size)
	};

	ImU32 triangle_color;

	if (ImGui::IsItemHovered())
		triangle_color = IM_COL32(155, 255, 155, 255);
	else
		triangle_color = IM_COL32(85, 255, 85, 255);

	draw_list->AddTriangleFilled(triangle_pos[0], triangle_pos[1], triangle_pos[2], triangle_color);
}

void CIMGUI_Camera_Tab::IMGUI_Pause_Button()
{
	ImVec2 button_size = ImVec2(20, 20);
	ImGui::SameLine();
	if (ImGui::InvisibleButton("pause_button", button_size))
		m_pMainCamera->Pause();

	ImVec2 p = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float size = 20.0f;

	ImVec2 pause_rect1_pos[2] = {
		ImVec2(p.x + 4, p.y),
		ImVec2(p.x + 8, p.y + size)
	};
	ImVec2 pause_rect2_pos[2] = {
		ImVec2(p.x + 12, p.y),
		ImVec2(p.x + 16, p.y + size)
	};
	ImU32 pause_color = IM_COL32(255, 255, 65, 255);
	if (ImGui::IsItemHovered())
		pause_color = IM_COL32(255, 255, 185, 255);

	draw_list->AddRectFilled(pause_rect1_pos[0], pause_rect1_pos[1], pause_color);
	draw_list->AddRectFilled(pause_rect2_pos[0], pause_rect2_pos[1], pause_color);

}

void CIMGUI_Camera_Tab::IMGUI_Stop_Button()
{
	ImVec2 button_size = ImVec2(20, 20);
	ImGui::SameLine();
	if (ImGui::InvisibleButton("reset_button", button_size))
		m_pMainCamera->Button_Stop();

	ImVec2 p = ImGui::GetItemRectMin();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float size = 20.0f;

	ImVec2 reset_rect_pos[2] = {
		ImVec2(p.x, p.y),
		ImVec2(p.x + size, p.y + size)
	};
	ImU32 reset_color = IM_COL32(255, 85, 85, 255);
	if (ImGui::IsItemHovered())
		reset_color = IM_COL32(255, 135, 135, 255);

	draw_list->AddRectFilled(reset_rect_pos[0], reset_rect_pos[1], reset_color);
}


void CIMGUI_Camera_Tab::IMGUI_Save_Button()
{
	if (ImGui::Button("Save"))
	{
		// CameraSaveData ����ü �ʱ�ȭ
		CameraSaveData saveData;

		// �𵨺� ������ ��ȸ
		for (const auto& modelPair : m_ModelSkills)
		{
			//�� Enum
			CAMERA_MODELID modelID = modelPair.first;
			//�� string
			const vector<string>& skills = modelPair.second;

			CameraSaveData::ModelData modelData;
			//�� ID
			modelData.modelID = (_int)modelID;

			//�𵨵� ��ȸ
			for (size_t skillIdx = 0; skillIdx < skills.size(); ++skillIdx)
			{
				//��ų �̸�
				const string& skillName = skills[skillIdx];
				CameraSaveData::ModelData::SkillData skillData;
				skillData.skillName = skillName;

				// �ش� �𵨰� ��ų�� ���ε� ī�޶� �ε��� ã��
				auto cameraKey = make_pair(modelID, static_cast<int>(skillIdx));
				auto cameraIt = m_CameraIndexMap.find(cameraKey);

				_int cameraIndex = cameraIt->second;

				//�� �𵨰� ��ų�� ���� ����� ī�޶�
				CVirtual_Camera* virtualCamera = m_pMainCamera->m_vecVirtualCamera[cameraIndex];

				// �ش� ��ų�� ���� �ִϸ��̼� ��� ��������
				auto animIt = m_SkillAnimations.find(cameraKey);

				//�ִϸ��̼ǵ��� �̸�
				const vector<string>& animations = animIt->second;

				for (size_t animIdx = 0; animIdx < animations.size(); ++animIdx)
				{
					const string& animationName = animations[animIdx];
					CameraSaveData::ModelData::SkillData::AnimationData animData;
					animData.animationName = animationName;

					// �ش� �ִϸ��̼��� ����Ʈ ��������
					vector<CameraPoint> points = virtualCamera->m_mapPoints[static_cast<int>(animIdx)];
					animData.points = points;

					skillData.animations.push_back(animData);
				}

				modelData.skills.push_back(skillData);
			}

			saveData.models.push_back(modelData);
		}

		// ���� �Ŵ����� ���� ����
		HRESULT hr = m_pGameInstance->Save_All_CameraPoints(filename, &saveData);
	}
}


CIMGUI_Camera_Tab* CIMGUI_Camera_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Camera_Tab* pInstance = new CIMGUI_Camera_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Camera_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Camera_Tab::Free()
{
	Safe_Release(m_pMainCamera);
	Safe_Release(m_pLineDraw);

	__super::Free();
}
