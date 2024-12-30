#include "stdafx.h"
#include "IMGUI_Object_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include <codecvt>
#include "Transform.h"
#include "GameObject.h"
#include "ContainerObject.h"
#include "PartObject.h"
#include "Main_Camera.h"
CIMGUI_Object_Tab::CIMGUI_Object_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}

void CIMGUI_Object_Tab::Effect_Menu()
{
}

void CIMGUI_Object_Tab::Effect_Transform()
{
}

void CIMGUI_Object_Tab::Camera_Set(_uint iNumCameraModel)
{
}

HRESULT CIMGUI_Object_Tab::Initialize()
{
	return S_OK;
}

void CIMGUI_Object_Tab::Render(_float fTimeDelta)
{
	static bool bShowPrototypeObjects = true;
	static bool bShowLayerObjects = true;
	static bool bShowSettings = true;
	static bool bShowSaveSuccessMessage = false;
	static bool bShowSaveFailMessage = false;

	if (ImGui::BeginTabBar("Object_InnerTabs")) { // ���� �� �� ����
		if (ImGui::BeginTabItem("Layer_ObjectList", &bShowLayerObjects)) { // ���� Layer ��
			if (bShowLayerObjects) {
				IMGUI_Show_LayerObjects(); // Layer ������Ʈ ǥ��
			}
			ImGui::EndTabItem(); // ���� Layer �� ����
		}

		if (ImGui::BeginTabItem("Prototype_ObjectList", &bShowPrototypeObjects)) { // ���� Prototype ��
			if (bShowPrototypeObjects) {
				IMGUI_Show_PrototypeObjects(); // Prototype ������Ʈ ǥ��
			}
			ImGui::EndTabItem(); // ���� Prototype �� ����
		}

		if (ImGui::BeginTabItem("Save", &bShowSettings)) {
			if (ImGui::Button("Save")) { // "Save" ��ư
				IMGUI_Save_Button_Pressed(&bShowSaveSuccessMessage, &bShowSaveFailMessage);
			}

			if (bShowSaveSuccessMessage) {
				ImGui::Text("File saved successfully!");
			}

			if (bShowSaveFailMessage) {
				ImGui::Text("Failed to save file!");
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar(); // ���� �� �� ����
	}
}


HRESULT CIMGUI_Object_Tab::IMGUI_Show_PrototypeObjects()
{
	vector<string> objectPrototypes;
	m_pGameInstance->Get_Prototype_Names(&objectPrototypes);
	int selectedItem = -1; // �ʱ� ���� �׸� ����

	static bool showTerrainInput = false;
	static string selectedTerrainType;

	// ����Ʈ �ڽ� ����
	if (ImGui::BeginListBox("##PrototypesListBox", ImVec2(-FLT_MIN, 30 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < objectPrototypes.size(); i++) {
			if (ImGui::Selectable(objectPrototypes[i].c_str(), selectedItem == i)) {
				selectedItem = i;
				// ������Ʈ ���� ����
				IMGUI_SpawnObjectAtZero(objectPrototypes[i]);

				// ���� ������Ʈ
				m_bIsClickedPrototype = true;
				m_SelectedPrototype = objectPrototypes[i];
				m_pCurrentMovingObject = nullptr; // ���� �̵� ��ü �ʱ�ȭ
			}
		}
		ImGui::EndListBox();
	}

	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_SpawnObjectAtZero(const std::string& type)
{
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();

	// ���ڿ����� "Prototype_GameObject_" ���ξ� ����
	std::string prefix = "Prototype_GameObject_";
	std::string suffix;

	if (type.substr(0, prefix.size()) == prefix) {
		suffix = type.substr(prefix.size());
	}
	else {
		suffix = type; // ���ξ ���ٸ� ��ü Ÿ���� ���
	}

	// Layer �̸� ����
	std::string layerName = "Layer_" + suffix;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring converted = converter.from_bytes(type);
	std::wstring wLayerName = converter.from_bytes(layerName);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(currentLevel, converted.c_str(), wLayerName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_Show_LayerObjects()
{
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();
	ImGui::Columns(2, NULL, true); // �� ���� �÷� ����

	//���° ���̾ Ŭ���ߴ��� �˱� ����
	//ShowLayerList(���� �г�)���� ���� ��.
	static _int iSelectedItemIndex = -1; // �ʱ� ���� �׸� ����

	//���� ������ �ִ� ���̾���� �̸���, ���̾ �����ִ� GameObject list�� ���� ��������.
	vector<pair<string, list<CGameObject*>>> objectLayersVector;
	m_pGameInstance->Add_Object_Layers_Vector(currentLevel, &objectLayersVector);

	//���̾��� �������� ���� Left_Panel�� �̵�
	ShowLayerList(objectLayersVector, iSelectedItemIndex);
	ImGui::NextColumn(); // ���� �÷����� �̵�

	//�� ���̾���� �����ִ� ���� ���ӿ�����Ʈ�� �����ִ� �Լ�
	ShowObjectListAndControls(objectLayersVector, iSelectedItemIndex);

	ImGui::Columns(1); // �� ����

	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_Save_Button_Pressed(_bool* bShowSaveSuccessMessage, _bool* bShowSaveFailMessage)
{
	// ������Ʈ������ ����
	vector<FILEDATA> vecFileData;

	// ���� ���� 
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();

	vector<pair<wstring, list<CGameObject*>>> objectLayersVector;
	m_pGameInstance->Add_Object_Layers_Vector(currentLevel, &objectLayersVector);

	// ���⿡ ��ŵ�� ���̾� �̸��� ����
	unordered_set<wstring> skipLayers =
	{ L"Layer_Camera",L"Layer_IMGUI_Manager",L"Layer_Terrain",L"Layer_Particle_Manager",L"Layer_Item"
	,L"Layer_Inventory", };

	// "Layer_UI"�� �����ϴ� ��� ���̾ ��ŵ�� ���̾ �߰�
	for (const auto& object : objectLayersVector)
	{
		if (object.first.find(L"Layer_UI") == 0 ||
			object.first.find(L"Layer_Particle") == 0)
		{
			skipLayers.insert(object.first);
		}
	}

	for (auto& object : objectLayersVector)
	{
		// ���� ���̾ ��ŵ�� ����Ʈ�� �ִ��� �˻�
		if (skipLayers.find(object.first) != skipLayers.end()) {
			continue;  // ��ŵ�� ���̾�� ���� �ݺ����� �Ѿ
		}

		for (auto& iter : object.second)
		{
			CTransform* transform = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")));

			// Layer_ ���� ���ڿ� ����
			wstring layerName = object.first;
			size_t pos = layerName.find(L"Layer_");
			if (pos != wstring::npos) {
				// "Layer_" ���� ���ڿ� ���� ��ġ�� pos + 6
				wstring suffix = layerName.substr(pos + 6);

				// ���ο� prefix ����
				wstring newPrefix = L"Prototype_GameObject_" + suffix;

				// �ӽ� ������ ����Ͽ� XMVECTOR�� _float3�� ��ȯ
				_float3 position;
				_float3 rightRotation;
				_float3 upRotation;
				_float3 lookRotation;

				XMStoreFloat3(&position, transform->Get_State(CTransform::STATE_POSITION));
				XMStoreFloat3(&rightRotation, transform->Get_State(CTransform::STATE_RIGHT));
				XMStoreFloat3(&upRotation, transform->Get_State(CTransform::STATE_UP));
				XMStoreFloat3(&lookRotation, transform->Get_State(CTransform::STATE_LOOK));

				// �� ���ο� ���ڿ��� vecFileData�� �߰�
				vecFileData.emplace_back(FILEDATA({ true, newPrefix, object.first, currentLevel,
					position, rightRotation, upRotation,lookRotation,transform->Get_Scaled() }));
			}
		}
	}

	// currentLevel�� ���� ���� �̸� ����
	wstring filePath;
	switch (currentLevel)
	{
	case LEVEL_GAMEPLAY:
		filePath = L"../Bin/Level_GamePlay_Objects.txt";
		break;
	default:
		*bShowSaveFailMessage = true;
		*bShowSaveSuccessMessage = false;
		return E_FAIL;
	}

	// �̰� ������ �纻 ��ü ����Ʈ�� �Ѱ���� �Ѵ�.
	HRESULT result = m_pGameInstance->SaveObjects(filePath.c_str(), &vecFileData);
	if (result == S_OK) {
		*bShowSaveSuccessMessage = true;
		*bShowSaveFailMessage = false;
		return S_OK;
	}
	else {
		*bShowSaveFailMessage = true;
		*bShowSaveSuccessMessage = false;
		return E_FAIL;
	}
}

HRESULT CIMGUI_Object_Tab::IMGUI_Click_Button_Release(unordered_set<CGameObject*>& container, CGameObject* gameObject)
{
	for (auto it = container.begin(); it != container.end(); /* �� ���� */) {

		if (*it == gameObject)
		{
			gameObject->m_bDead = true;
			it = container.erase(it); // �����̳ʿ��� ������Ʈ ����
			return S_OK;
		}
		else
		{
			++it; // ���ͷ����� ����
		}
	}
	return S_OK;
}

HRESULT CIMGUI_Object_Tab::IMGUI_CopySelectedObject(const wstring& layerName, CGameObject* sourceObject)
{
	// Get the current level index
	_uint currentLevel = m_pGameInstance->Get_CurrentLevel_Index();

	// Extract the prototype suffix from the layer name
	std::wstring prototypeSuffix = layerName.substr(6); // "Layer_" ������ ���ڿ� ����
	std::wstring prototypeTag = L"Prototype_GameObject_" + prototypeSuffix;

	// Copy the position and scale from the source object
	CComponent* sourceComponent = sourceObject->Get_Component(TEXT("Com_Transform"));
	CTransform* sourceTransform = static_cast<CTransform*>(sourceComponent);

	FILEDATA copy_DESC{};
	copy_DESC.prototypeTag = prototypeTag;
	copy_DESC.layerName = layerName;
	copy_DESC.rightRotation = { 1.0f, 0.f, 0.f };
	copy_DESC.upRotation = { 0.f, 1.f, 0.f };
	copy_DESC.lookRotation = { 0.f, 0.f, 1.f };

	_vector positionVector = sourceTransform->Get_State(CTransform::STATE_POSITION);
	_vector rightRotationVector = sourceTransform->Get_State(CTransform::STATE_RIGHT);
	_vector upRotationVector = sourceTransform->Get_State(CTransform::STATE_UP);
	_vector lookRotationVector = sourceTransform->Get_State(CTransform::STATE_LOOK);

	XMStoreFloat3(&copy_DESC.position, positionVector);
	XMStoreFloat3(&copy_DESC.rightRotation, rightRotationVector);
	XMStoreFloat3(&copy_DESC.upRotation, upRotationVector);
	XMStoreFloat3(&copy_DESC.lookRotation, lookRotationVector);

	copy_DESC.scale = sourceTransform->Get_Scaled();
	copy_DESC.isParsing = true;

	// Create a new game object using the prototype
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(currentLevel, prototypeTag.c_str(), layerName.c_str(), &copy_DESC)))
		return E_FAIL;

	return S_OK;
}

void CIMGUI_Object_Tab::ShowLayerList(const vector<pair<string, list<CGameObject*>>>& objectLayersVector, _int& _iSelectedItemIndex)
{
	ImGui::BeginChild("LeftPane", ImVec2(150, 0), true); // ���� â ����
	//���̾��� ������ŭ for�� ������
	for (int i = 0; i < objectLayersVector.size(); i++) {
		//���� � ���̾ Ŭ���ߴٸ� ���° ���̾ Ŭ���ߴ��� Index ��ȯ
		if (ImGui::Selectable(objectLayersVector[i].first.c_str(), _iSelectedItemIndex == i)) {
			_iSelectedItemIndex = i;
		}
	}

	ImGui::EndChild();
}

void CIMGUI_Object_Tab::ShowObjectListAndControls(const vector<pair<string, list<CGameObject*>>>& objectLayersVector, _int selectedItem)
{
	// ������ â ����
	ImGui::BeginChild("RightPane", ImVec2(0, 0), true);

	// ���õ� ���ӿ�����Ʈ�� �ִٸ�?
	static CGameObject* pSelectGameObject = nullptr;
	static bool showContainerDetails = false;  // Container Object Details â�� ���� ���¸� �����ϱ� ���� ����
	static bool showColliderDetails = false;  // Container Object Details â�� ���� ���¸� �����ϱ� ���� ����

	// ���� ���� �гο��� Ư�� ���̾ Ŭ������ ���
	if (selectedItem != -1)
	{
		// ���̾��� �̸��� ��������
		string layerName = objectLayersVector[selectedItem].first;
		// �ش� ���̾���� ���ӿ�����Ʈ list�� ��������
		const list<CGameObject*>& gameObjects = objectLayersVector[selectedItem].second; // const�� ����Ͽ� ���� �ذ�
		// ���̾���� ���ӿ�����Ʈ�� ��ȣ�� ǥ���ϱ� ���� �ε���
		_uint index = 0;

		// ���̾��� ���ӿ�����Ʈ�� ������.
		for (auto it = gameObjects.begin(); it != gameObjects.end();) {
			CGameObject* gameObject = *it;

			// Selectable �Լ��� ��ü�� �Ű������� ���ڿ��� ������ش�.
			bool isSelected = m_hashSet_selectedGameObjects.find(gameObject) != m_hashSet_selectedGameObjects.end(); // ���õ� ���¸� Ȯ��
			if (ImGui::Selectable((layerName + " " + std::to_string(index)).c_str(), isSelected)) {
				if (ImGui::GetIO().KeyShift) {
					// Shift Ű�� ���� ���¿��� Ŭ���� ���
					if (isSelected) {
						m_hashSet_selectedGameObjects.erase(gameObject); // �̹� ���õ� ���¶�� ���� ����
						gameObject->SetImguiSelect(false);
					}
					else {
						m_hashSet_selectedGameObjects.insert(gameObject); // ���õ� ���°� �ƴ϶�� ���� �߰�
						gameObject->SetImguiSelect(true);
					}
				}
				else {
					for (auto& obj : m_hashSet_selectedGameObjects) {
						obj->SetImguiSelect(false); // ���� ������ ��� ����
					}

					// Shift Ű�� ������ ���� ���¿��� Ŭ���� ���
					m_hashSet_selectedGameObjects.clear(); // ���� ������ ��� ����
					m_hashSet_selectedGameObjects.insert(gameObject); // ���� ������ �߰�
					gameObject->SetImguiSelect(true); // ���� ������ true�� ����
					pSelectGameObject = gameObject;

					if (CContainerObject* containerObject = dynamic_cast<CContainerObject*>(pSelectGameObject)) {
						// Show a new window for CContainerObject
						showContainerDetails = true;
					}

					if (pSelectGameObject->Get_Component(TEXT("Com_Collider")) != nullptr) {
						// Show a new window for CContainerObject
						showColliderDetails = true;
					}
				}
			}

			++it; // �������� ������ ���� ������Ʈ�� �̵�
			index++;  // ���� ������Ʈ�� ���� �ε��� ����
		}
	}

	// ���� �����ӿ��� �ߺ� �����ߴ� ��ü�� ������ �� �ʱ�ȭ�ؾ���
	if (m_hashSet_selectedGameObjects != m_hashSet_previousSelectedGameObjects) {
		m_uiDeltaPosition = { 0.0f, 0.0f, 0.0f };
		m_uiDeltaRotation = { 0.0f, 0.0f, 0.0f };
		m_uiDeltaScale = { 0.0f, 0.0f, 0.0f };
		m_hashSet_previousSelectedGameObjects = m_hashSet_selectedGameObjects; // ���� ���õ� ��ü ������Ʈ
	}

	// ���� �����̶��
	if (m_hashSet_selectedGameObjects.size() == 1) {
		pSelectGameObject = *m_hashSet_selectedGameObjects.begin();
		if (pSelectGameObject != nullptr) {
			// Handle normal CGameObject transformations
			HandleObjectPosition(pSelectGameObject);
			HandleObjectRotation(pSelectGameObject);
			HandleObjectScale(pSelectGameObject);
			HandleObjectManagement(m_hashSet_selectedGameObjects, pSelectGameObject, objectLayersVector[selectedItem].first);

			// Ȱ��ȭ ���¸� ����ϴ� ��ư �߰�
			if (ImGui::Button(pSelectGameObject->IsActive() ? "Deactivate" : "Activate")) {
				pSelectGameObject->SetActive(!pSelectGameObject->IsActive());
			}
		}
	}
	// �ߺ� �����̶��
	else if (m_hashSet_selectedGameObjects.size() > 1) {
		HandleMultipleObjectControl(m_hashSet_selectedGameObjects);
	}

	ImGui::EndChild();

	if (pSelectGameObject != nullptr && pSelectGameObject->m_bDead)
		return;

	// Show Container Object Details window if required
	if (showContainerDetails && pSelectGameObject) {
		if (ImGui::Begin("Container Object Details", &showContainerDetails)) {
			HandleContainerObject(dynamic_cast<CContainerObject*>(pSelectGameObject));
		}
		ImGui::End();
	}

	if (showColliderDetails && pSelectGameObject) {
		if (ImGui::Begin("Collider Object Details", &showColliderDetails)) {
			HandleAABBColliderObject(pSelectGameObject);
		}
		ImGui::End();
	}
}

void CIMGUI_Object_Tab::HandleContainerObject(CContainerObject* containerObject)
{
	// CContainerObject�� m_Parts ����� ǥ���ϰ� ������ ��ü�� ���� ���� �����ϰ� ��
	const vector<CPartObject*>& parts = containerObject->GetParts();

	// ���õ� ���� ������Ʈ�� �����ϱ� ���� ���� ����
	static CPartObject* selectedPart = nullptr;

	// Container Object�� ��Ʈ ����� �����ִ� â�� ���

	for (size_t i = 0; i < parts.size(); ++i) {
		CPartObject* part = parts[i];
		if (part == nullptr)
			continue;

		// RTTI�� ����Ͽ� Ŭ���� �̸��� ��������
		std::string partName = typeid(*part).name();

		bool isSelected = (selectedPart == part); // ���õ� ���¸� Ȯ��
		if (ImGui::Selectable(partName.c_str(), isSelected)) {
			if (ImGui::GetIO().KeyShift) {
				// Shift Ű�� ���� ���¿��� Ŭ���� ���
				if (isSelected) {
					selectedPart = nullptr; // �̹� ���õ� ���¶�� ���� ����
					part->SetImguiSelect(false);
				}
				else {
					selectedPart = part; // ���õ� ���°� �ƴ϶�� ���� �߰�
					part->SetImguiSelect(true);
				}
			}
			else {
				if (isSelected) {
					selectedPart = nullptr; // �̹� ���õ� ���¶�� ���� ����
					part->SetImguiSelect(false);
				}
				else {
					selectedPart = part; // ���õ� ���°� �ƴ϶�� ���� �߰�
					part->SetImguiSelect(true);
				}
			}
		}

		// ���õ� ��Ʈ ��ü�� �� ������ ���� â���� ���� ǥ��
		if (selectedPart == part) {
			ImGui::Separator();
			ImGui::Text("Selected Part Details:");

			// Handle position, rotation, and scale for the selected part
			HandleObjectPosition(part);
			HandleObjectRotation(part);
			HandleObjectScale(part);

			// Ȱ��ȭ ���¸� ����ϴ� ��ư �߰�
			if (ImGui::Button(part->IsActive() ? "Deactivate" : "Activate")) {
				part->SetActive(!part->IsActive());
			}

			// ���� �ϴܿ� �ݱ� ��ư �߰�
			if (ImGui::Button("Close Details")) {
				selectedPart = nullptr;
			}
		}
	}
}

void CIMGUI_Object_Tab::HandleAABBColliderObject(CGameObject* colliderObject)
{
	CCollider* collider = dynamic_cast<CCollider*>(colliderObject->Get_Component(TEXT("Com_Collider")));
	if (collider == nullptr) return;

	ImGui::Separator();

	HandleColliderTransform(collider);
}

void CIMGUI_Object_Tab::HandleObjectPosition(CGameObject* _pSelectedGameObject)
{
	if (_pSelectedGameObject == nullptr || _pSelectedGameObject->m_bDead)
		return;

	// ��ü�� ���� ��ġ�� ��������
	_float3 currentPos = { 0.0f, 0.0f, 0.0f };

	//��ü�� Transform ������Ʈ ��������
	CComponent* component = _pSelectedGameObject->Get_Component(TEXT("Com_Transform"));
	CTransform* transform = nullptr;

	if (component != nullptr) {
		transform = static_cast<CTransform*>(component);
		_vector positionVector = transform->Get_State(CTransform::STATE_POSITION);
		//������ ��ġ������ ����� _float3���� ����
		XMStoreFloat3(&currentPos, positionVector);
	}
	else
		return;

	// Ű �Է��� ó���Ͽ� ������ �̵�
	if (bMovementEnabled)
	{
		_float3 deltaPosition = { 0.0f, 0.0f, 0.0f };

		_float offset = 0.1f;

		if (ImGui::IsKeyDown(ImGuiKey_W)) {
			deltaPosition.z += offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_S)) {
			deltaPosition.z -= offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_A)) {
			deltaPosition.x -= offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_D)) {
			deltaPosition.x += offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_Q)) {
			deltaPosition.y -= offset;
		}
		if (ImGui::IsKeyDown(ImGuiKey_E)) {
			deltaPosition.y += offset;
		}

		// deltaPosition�� _vector�� ��ȯ
		_vector deltaVector = XMLoadFloat3(&deltaPosition);

		// currentPosition�� deltaVector�� ����
		_vector currentPosition = XMLoadFloat3(&currentPos);
		currentPosition += deltaVector;

		// ��ġ ������ w ��Ҹ� 1�� ����
		currentPosition = XMVectorSetW(currentPosition, 1.0f);

		// Set_State�� currentPosition�� �ٽ� �ѱ�
		transform->Set_State(CTransform::STATE_POSITION, currentPosition);
		// currentPos�� ������Ʈ�� ��ġ�� ����
		XMStoreFloat3(&currentPos, currentPosition);
	}

	float sliderWidth = 150.0f; // �����̴��� �ʺ�
	float inputWidth = 50.0f;   // �Է� �ʵ��� �ʺ�
	float sliderPower = 0.01f;  // �����̴� ���� �ΰ���
	float rangeX = 50.0f; // �����̴� ���� ����
	float rangeY = 50.0f;
	float rangeZ = 50.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // ��� �� ���� ����
	ImGui::Separator(); // ���м� �߰�
	ImGui::Text("Position: "); // �� �߰�
	ImGui::PushButtonRepeat(true); // ��ư �ݺ��� Ȱ��ȭ
	ImGui::PushItemWidth(sliderWidth);

	// X �� ����
	ImGui::PushItemWidth(sliderWidth);
	if (ImGui::Button("-X")) currentPos.x -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##PosX", &currentPos.x, -rangeX, rangeX, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+X")) currentPos.x += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##PosXInput", &currentPos.x);
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	// Y �� ����
	ImGui::PushItemWidth(sliderWidth);
	if (ImGui::Button("-Y")) currentPos.y -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##PosY", &currentPos.y, -rangeY, rangeY, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Y")) currentPos.y += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##PosYInput", &currentPos.y);
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	// Z �� ����
	ImGui::PushItemWidth(sliderWidth);
	if (ImGui::Button("-Z")) currentPos.z -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##PosZ", &currentPos.z, -rangeZ, rangeZ, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Z")) currentPos.z += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##PosZInput", &currentPos.z);
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat(); // ��ư �ݺ��� ��Ȱ��ȭ
	ImGui::PopStyleVar();

	_vector newPosition = XMLoadFloat3(&currentPos);
	newPosition = XMVectorSetW(newPosition, 1.0f);

	if (CMain_Camera* camera = dynamic_cast<CMain_Camera*>(_pSelectedGameObject))
	{
		camera->SetPosition(newPosition);
	}
	else
	{
		transform->Set_State(CTransform::STATE_POSITION, newPosition);
	}
}

void CIMGUI_Object_Tab::HandleColliderTransform(CCollider* _pCollider)
{
	// ���� �ݶ��̴��� ���� ��ġ�� ũ�⸦ ��������
	BoundingBox boundingBox = _pCollider->AABB_GetDesc();
	static _float3 currentPos = { 0.0f, 0.0f, 0.0f }; // ��ġ�� 0���� �ʱ�ȭ
	static _float3 extents = boundingBox.Extents;

	float sliderWidth = 150.0f; // �����̴��� �ʺ�
	float inputWidth = 50.0f;   // �Է� �ʵ��� �ʺ�
	float sliderPower = 0.01f;  // �����̴� ���� �ΰ���
	float rangeX = 50.0f; // �����̴� ���� ����
	float rangeY = 50.0f;
	float rangeZ = 50.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // ��� �� ���� ����
	ImGui::Separator(); // ���м� �߰�
	ImGui::Text("Collider Transform: "); // �� �߰�
	ImGui::PushButtonRepeat(true); // ��ư �ݺ��� Ȱ��ȭ
	ImGui::PushItemWidth(sliderWidth);

	// ��ġ ����
	ImGui::Text("Position:");
	// X �� ����
	if (ImGui::Button("-X##Pos")) currentPos.x -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderPosX", &currentPos.x, -rangeX, rangeX, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+X##Pos")) currentPos.x += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderPosXInput", &currentPos.x);
	ImGui::PopItemWidth();

	// Y �� ����
	if (ImGui::Button("-Y##Pos")) currentPos.y -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderPosY", &currentPos.y, -rangeY, rangeY, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Y##Pos")) currentPos.y += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderPosYInput", &currentPos.y);
	ImGui::PopItemWidth();

	// Z �� ����
	if (ImGui::Button("-Z##Pos")) currentPos.z -= 0.1f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderPosZ", &currentPos.z, -rangeZ, rangeZ, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Z##Pos")) currentPos.z += 0.1f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderPosZInput", &currentPos.z);
	ImGui::PopItemWidth();

	ImGui::Separator();

	// ������ ����
	ImGui::Text("Scale:");
	// ������ X �� ����
	if (ImGui::Button("-X##Scale")) extents.x -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderScaleX", &extents.x, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+X##Scale")) extents.x += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderScaleXInput", &extents.x);
	ImGui::PopItemWidth();

	// ������ Y �� ����
	if (ImGui::Button("-Y##Scale")) extents.y -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderScaleY", &extents.y, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Y##Scale")) extents.y += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderScaleYInput", &extents.y);
	ImGui::PopItemWidth();

	// ������ Z �� ����
	if (ImGui::Button("-Z##Scale")) extents.z -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ColliderScaleZ", &extents.z, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+Z##Scale")) extents.z += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ColliderScaleZInput", &extents.z);
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat(); // ��ư �ݺ��� ��Ȱ��ȭ
	ImGui::PopStyleVar();

	// ���� ��ġ�� ������ ���� ������Ʈ�� ������ ����
	boundingBox.Center = currentPos;
	boundingBox.Extents = extents;
	_pCollider->AABB_SetDesc(boundingBox);
}

void CIMGUI_Object_Tab::HandleObjectRotation(CGameObject* _pSelectedGameObject)
{
	if (_pSelectedGameObject == nullptr || _pSelectedGameObject->m_bDead)
		return;

	// Transform ������Ʈ ��������
	CComponent* component = _pSelectedGameObject->Get_Component(TEXT("Com_Transform"));
	if (component == nullptr) return;

	CTransform* transform = static_cast<CTransform*>(component);

	// ���� ��Ŀ��� ȸ�� ��� ����
	_matrix worldMatrix = transform->Get_WorldMatrix();
	_matrix rotationMatrix = worldMatrix;
	rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // ��ġ ���͸� ����

	// ������ ���� ��ֶ������Ͽ� �и�
	rotationMatrix.r[0] = XMVector3Normalize(rotationMatrix.r[0]);
	rotationMatrix.r[1] = XMVector3Normalize(rotationMatrix.r[1]);
	rotationMatrix.r[2] = XMVector3Normalize(rotationMatrix.r[2]);

	// ȸ�� ����� ���Ϸ� ������ ��ȯ
	_float3 currentRotation = transform->GetEulerAnglesFromRotationMatrix(rotationMatrix);

	// ��ü�� ���� ȸ�� ���°� ������ �ʱ�ȭ
	if (m_hashTable_Rotation.find(_pSelectedGameObject) == m_hashTable_Rotation.end()) {
		m_hashTable_Rotation[_pSelectedGameObject] = _float3{
			XMConvertToDegrees(currentRotation.x),
			XMConvertToDegrees(currentRotation.y),
			XMConvertToDegrees(currentRotation.z)
		};
	}

	// ���� ��ü�� ȸ�� ���� �ҷ�����
	_float3& displayRotation = m_hashTable_Rotation[_pSelectedGameObject];


	float sliderWidth = 150.0f; // �����̴��� �ʺ�
	float inputWidth = 50.0f;   // �Է� �ʵ��� �ʺ�
	float sliderPower = 1.0f;   // �����̴� ���� �ΰ���

	bool rotationChanged = false;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // ��� �� ���� ����
	ImGui::Separator(); // ���м� �߰�
	ImGui::Text("Rotation: "); // �� �߰�
	ImGui::PushButtonRepeat(true); // ��ư �ݺ��� Ȱ��ȭ
	ImGui::PushItemWidth(sliderWidth);

	// X �� (Pitch) ȸ�� ����
	if (ImGui::Button("-X")) { displayRotation.x -= 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotX", &displayRotation.x, -180.0f, 180.0f, "%.3f", sliderPower)) {
		rotationChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+X")) { displayRotation.x += 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotXInput", &displayRotation.x)) {
		rotationChanged = true;
	}
	ImGui::PopItemWidth();

	// Y �� (Yaw) ȸ�� ����
	if (ImGui::Button("-Y")) { displayRotation.y -= 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotY", &displayRotation.y, -180.0f, 180.0f, "%.3f", sliderPower)) {
		rotationChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Y")) { displayRotation.y += 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotYInput", &displayRotation.y)) {
		rotationChanged = true;
	}
	ImGui::PopItemWidth();

	// Z �� (Roll) ȸ�� ����
	if (ImGui::Button("-Z")) { displayRotation.z -= 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotZ", &displayRotation.z, -180.0f, 180.0f, "%.3f", sliderPower)) {
		rotationChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Z")) { displayRotation.z += 1.0f; rotationChanged = true; }
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotZInput", &displayRotation.z)) {
		rotationChanged = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat(); // ��ư �ݺ��� ��Ȱ��ȭ
	ImGui::PopStyleVar();

	// ȸ�� ���� ����Ǿ��� ���� ������Ʈ
	if (rotationChanged) {
		_float3 newRotation = {
			XMConvertToRadians(displayRotation.x),
			XMConvertToRadians(displayRotation.y),
			XMConvertToRadians(displayRotation.z)
		};
		transform->Rotation(newRotation);
	}
}

void CIMGUI_Object_Tab::HandleObjectScale(CGameObject* _pSelectedGameObject)
{
	if (_pSelectedGameObject == nullptr || _pSelectedGameObject->m_bDead)
		return;

	// ��ü�� Transform ������Ʈ ��������
	CComponent* component = _pSelectedGameObject->Get_Component(TEXT("Com_Transform"));
	CTransform* transform = nullptr;
	_float3 currentScale = { 1.0f, 1.0f, 1.0f };

	if (component != nullptr) {
		transform = static_cast<CTransform*>(component);
		currentScale = transform->Get_Scaled();
	}
	else
		return; // Transform ������Ʈ�� ������ �Լ��� ����


	float sliderWidth = 150.0f; // �����̴��� �ʺ�
	float inputWidth = 50.0f;   // �Է� �ʵ��� �ʺ�
	float sliderPower = 0.01f;  // �����̴� ���� �ΰ���

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // ��� �� ���� ����
	ImGui::Separator(); // ���м� �߰�
	ImGui::Text("Scale: "); // �� �߰�
	ImGui::PushButtonRepeat(true); // ��ư �ݺ��� Ȱ��ȭ
	ImGui::PushItemWidth(sliderWidth);

	// ������ X �� ����
	if (ImGui::Button("-SX")) currentScale.x -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ScaleX", &currentScale.x, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+SX")) currentScale.x += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ScaleXInput", &currentScale.x);
	ImGui::PopItemWidth();

	// ������ Y �� ����
	if (ImGui::Button("-SY")) currentScale.y -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ScaleY", &currentScale.y, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+SY")) currentScale.y += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ScaleYInput", &currentScale.y);
	ImGui::PopItemWidth();

	// ������ Z �� ����
	if (ImGui::Button("-SZ")) currentScale.z -= 0.01f;
	ImGui::SameLine();
	ImGui::SliderFloat("##ScaleZ", &currentScale.z, 0.1f, 10.0f, "%.3f", sliderPower);
	ImGui::SameLine();
	if (ImGui::Button("+SZ")) currentScale.z += 0.01f;
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	ImGui::InputFloat("##ScaleZInput", &currentScale.z);
	ImGui::PopItemWidth();

	ImGui::PopItemWidth();
	ImGui::PopButtonRepeat(); // ��ư �ݺ��� ��Ȱ��ȭ
	ImGui::PopStyleVar();

	// ���� ������ ���� ������Ʈ�� �����Ϸ� ����
	if (transform != nullptr) {
		transform->Set_Scaled(currentScale.x, currentScale.y, currentScale.z); // ������ ����
	}
}

void CIMGUI_Object_Tab::HandleObjectManagement(unordered_set<CGameObject*>& container, CGameObject* _pSelectedGameObject, const string& layerName)
{
	// ���� ��ư �߰�
	ImGui::Separator(); // ���м� �߰�
	if (ImGui::Button("Delete Selected Object")) {
		IMGUI_Click_Button_Release(container, _pSelectedGameObject);
	}

	// ���� ��ư �߰�
	ImGui::Separator(); // ���м� �߰�
	if (ImGui::Button("Copy Selected Object")) {
		// ���̾� �̸��� �Բ� ��ü�� �����մϴ�.
		wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring wLayerName = converter.from_bytes(layerName);
		IMGUI_CopySelectedObject(wLayerName, _pSelectedGameObject);
	}
}

void CIMGUI_Object_Tab::HandleMultipleObjectControl(const unordered_set<CGameObject*>& selectedGameObjects)
{
	float sliderWidth = 150.0f; // �����̴��� �ʺ�
	float inputWidth = 50.0f;   // �Է� �ʵ��� �ʺ�
	float sliderPower = 0.01f;  // �����̴� ���� �ΰ���

	_float3 actualDeltaPosition = { 0.0f, 0.0f, 0.0f };
	_float3 actualDeltaRotation = { 0.0f, 0.0f, 0.0f };
	_float3 actualDeltaScale = { 0.0f, 0.0f, 0.0f };

	// ��ġ ���� UI
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // ��� �� ���� ����
	ImGui::Separator(); // ���м� �߰�
	ImGui::Text("Position: "); // �� �߰�
	ImGui::PushButtonRepeat(true); // ��ư �ݺ��� Ȱ��ȭ
	ImGui::PushItemWidth(sliderWidth);

	// X �� ����
	if (ImGui::Button("-X")) {
		m_uiDeltaPosition.x -= 0.1f;
		actualDeltaPosition.x -= 0.1f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##PosX", &m_uiDeltaPosition.x, -50.0f, 50.0f, "%.3f", sliderPower)) {
		actualDeltaPosition.x = m_uiDeltaPosition.x;
	}
	ImGui::SameLine();
	if (ImGui::Button("+X")) {
		m_uiDeltaPosition.x += 0.1f;
		actualDeltaPosition.x += 0.1f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##PosXInput", &m_uiDeltaPosition.x)) {
		actualDeltaPosition.x = m_uiDeltaPosition.x;
	}
	ImGui::PopItemWidth();

	// Y �� ����
	if (ImGui::Button("-Y")) {
		m_uiDeltaPosition.y -= 0.1f;
		actualDeltaPosition.y -= 0.1f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##PosY", &m_uiDeltaPosition.y, -50.0f, 50.0f, "%.3f", sliderPower)) {
		actualDeltaPosition.y = m_uiDeltaPosition.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Y")) {
		m_uiDeltaPosition.y += 0.1f;
		actualDeltaPosition.y += 0.1f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##PosYInput", &m_uiDeltaPosition.y)) {
		actualDeltaPosition.y = m_uiDeltaPosition.y;
	}
	ImGui::PopItemWidth();

	// Z �� ����
	if (ImGui::Button("-Z")) {
		m_uiDeltaPosition.z -= 0.1f;
		actualDeltaPosition.z -= 0.1f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##PosZ", &m_uiDeltaPosition.z, -50.0f, 50.0f, "%.3f", sliderPower)) {
		actualDeltaPosition.z = m_uiDeltaPosition.z;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Z")) {
		m_uiDeltaPosition.z += 0.1f;
		actualDeltaPosition.z += 0.1f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##PosZInput", &m_uiDeltaPosition.z)) {
		actualDeltaPosition.z = m_uiDeltaPosition.z;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat();
	ImGui::PopStyleVar();

	// ȸ�� ���� UI
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
	ImGui::Separator();
	ImGui::Text("Rotation: ");
	ImGui::PushButtonRepeat(true);
	ImGui::PushItemWidth(sliderWidth);

	// X �� (Pitch) ȸ�� ����
	if (ImGui::Button("-X")) {
		m_uiDeltaRotation.x -= 1.0f;
		actualDeltaRotation.x -= 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotX", &m_uiDeltaRotation.x, -180.0f, 180.0f, "%.3f", 1.0f)) {
		actualDeltaRotation.x = m_uiDeltaRotation.x;
	}
	ImGui::SameLine();
	if (ImGui::Button("+X")) {
		m_uiDeltaRotation.x += 1.0f;
		actualDeltaRotation.x += 1.0f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotXInput", &m_uiDeltaRotation.x)) {
		actualDeltaRotation.x = m_uiDeltaRotation.x;
	}
	ImGui::PopItemWidth();

	// Y �� (Yaw) ȸ�� ����
	if (ImGui::Button("-Y")) {
		m_uiDeltaRotation.y -= 1.0f;
		actualDeltaRotation.y -= 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotY", &m_uiDeltaRotation.y, -180.0f, 180.0f, "%.3f", 1.0f)) {
		actualDeltaRotation.y = m_uiDeltaRotation.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Y")) {
		m_uiDeltaRotation.y += 1.0f;
		actualDeltaRotation.y += 1.0f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotYInput", &m_uiDeltaRotation.y)) {
		actualDeltaRotation.y = m_uiDeltaRotation.y;
	}
	ImGui::PopItemWidth();

	// Z �� (Roll) ȸ�� ����
	if (ImGui::Button("-Z")) {
		m_uiDeltaRotation.z -= 1.0f;
		actualDeltaRotation.z -= 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##RotZ", &m_uiDeltaRotation.z, -180.0f, 180.0f, "%.3f", 1.0f)) {
		actualDeltaRotation.z = m_uiDeltaRotation.z;
	}
	ImGui::SameLine();
	if (ImGui::Button("+Z")) {
		m_uiDeltaRotation.z += 1.0f;
		actualDeltaRotation.z += 1.0f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##RotZInput", &m_uiDeltaRotation.z)) {
		actualDeltaRotation.z = m_uiDeltaRotation.z;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat();
	ImGui::PopStyleVar();

	// ������ ���� UI
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
	ImGui::Separator();
	ImGui::Text("Scale: ");
	ImGui::PushButtonRepeat(true);
	ImGui::PushItemWidth(sliderWidth);

	// X �� ������ ����
	if (ImGui::Button("-SX")) {
		m_uiDeltaScale.x -= 0.01f;
		actualDeltaScale.x -= 0.01f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##ScaleX", &m_uiDeltaScale.x, 0.1f, 10.0f, "%.3f", sliderPower)) {
		actualDeltaScale.x = m_uiDeltaScale.x;
	}
	ImGui::SameLine();
	if (ImGui::Button("+SX")) {
		m_uiDeltaScale.x += 0.01f;
		actualDeltaScale.x += 0.01f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##ScaleXInput", &m_uiDeltaScale.x)) {
		actualDeltaScale.x = m_uiDeltaScale.x;
	}
	ImGui::PopItemWidth();

	// Y �� ������ ����
	if (ImGui::Button("-SY")) {
		m_uiDeltaScale.y -= 0.01f;
		actualDeltaScale.y -= 0.01f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##ScaleY", &m_uiDeltaScale.y, 0.1f, 10.0f, "%.3f", sliderPower)) {
		actualDeltaScale.y = m_uiDeltaScale.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("+SY")) {
		m_uiDeltaScale.y += 0.01f;
		actualDeltaScale.y += 0.01f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##ScaleYInput", &m_uiDeltaScale.y)) {
		actualDeltaScale.y = m_uiDeltaScale.y;
	}
	ImGui::PopItemWidth();

	// Z �� ������ ����
	if (ImGui::Button("-SZ")) {
		m_uiDeltaScale.z -= 0.01f;
		actualDeltaScale.z -= 0.01f;
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##ScaleZ", &m_uiDeltaScale.z, 0.1f, 10.0f, "%.3f", sliderPower)) {
		actualDeltaScale.z = m_uiDeltaScale.z;
	}
	ImGui::SameLine();
	if (ImGui::Button("+SZ")) {
		m_uiDeltaScale.z += 0.01f;
		actualDeltaScale.z += 0.01f;
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##ScaleZInput", &m_uiDeltaScale.z)) {
		actualDeltaScale.z = m_uiDeltaScale.z;
	}
	ImGui::PopItemWidth();

	ImGui::PopButtonRepeat();
	ImGui::PopStyleVar();

	// ���õ� ��� ��ü�� ���� delta �� ����
	for (CGameObject* gameObject : selectedGameObjects)
	{
		// Transform ������Ʈ ��������
		CComponent* component = gameObject->Get_Component(TEXT("Com_Transform"));
		if (component == nullptr) continue;
		CTransform* transform = static_cast<CTransform*>(component);

		// ��ü�� ���� ȸ�� ���°� ������ �ʱ�ȭ
		if (m_hashTable_Rotation.find(gameObject) == m_hashTable_Rotation.end()) {
			_matrix worldMatrix = transform->Get_WorldMatrix();
			_matrix rotationMatrix = worldMatrix;
			rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // ��ġ ���͸� ����

			// ������ ���� ��ֶ������Ͽ� �и�
			rotationMatrix.r[0] = XMVector3Normalize(rotationMatrix.r[0]);
			rotationMatrix.r[1] = XMVector3Normalize(rotationMatrix.r[1]);
			rotationMatrix.r[2] = XMVector3Normalize(rotationMatrix.r[2]);

			// ȸ�� ����� ���Ϸ� ������ ��ȯ
			_float3 currentRotation = transform->GetEulerAnglesFromRotationMatrix(rotationMatrix);

			m_hashTable_Rotation[gameObject] = _float3{
				XMConvertToDegrees(currentRotation.x),
				XMConvertToDegrees(currentRotation.y),
				XMConvertToDegrees(currentRotation.z)
			};
		}

		// ���� ��ü�� ȸ�� ���� �ҷ�����
		_float3& displayRotation = m_hashTable_Rotation[gameObject];

		// ��ġ ������Ʈ
		_vector currentPosition = transform->Get_State(CTransform::STATE_POSITION);
		_float3 currentPos;
		XMStoreFloat3(&currentPos, currentPosition);
		currentPos.x += actualDeltaPosition.x;
		currentPos.y += actualDeltaPosition.y;
		currentPos.z += actualDeltaPosition.z;
		_vector newPosition = XMLoadFloat3(&currentPos);
		newPosition = XMVectorSetW(newPosition, 1.0f);
		transform->Set_State(CTransform::STATE_POSITION, newPosition);

		// ȸ�� ������Ʈ
		displayRotation.x += actualDeltaRotation.x;
		displayRotation.y += actualDeltaRotation.y;
		displayRotation.z += actualDeltaRotation.z;

		_float3 newRotation = {
			XMConvertToRadians(displayRotation.x),
			XMConvertToRadians(displayRotation.y),
			XMConvertToRadians(displayRotation.z)
		};
		transform->Rotation(newRotation);

		// ������ ������Ʈ
		_float3 currentScale = transform->Get_Scaled();
		currentScale.x += actualDeltaScale.x;
		currentScale.y += actualDeltaScale.y;
		currentScale.z += actualDeltaScale.z;
		transform->Set_Scaled(currentScale.x, currentScale.y, currentScale.z);
	}
}

CIMGUI_Object_Tab* CIMGUI_Object_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Object_Tab* pInstance = new CIMGUI_Object_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Object_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Object_Tab::Free()
{
	__super::Free();
}
