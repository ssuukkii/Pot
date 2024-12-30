#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "Main_Camera.h"
#include "GameInstance.h"
#include "RenderInstance.h"
#include "Effect_Manager.h"
#include "Imgui_Manager.h"
#include "QTE_Manager.h"
#include "UI_ComboNumber.h"

#include "UIObject.h"
#include "UI_GameState.h"

#include "Map_Manager.h"
#include "Character.h"
#include "Sound_Manager.h"
#include "BattleInterface.h"
#include "Opening_Kririn.h"
#include "Particle_Manager.h"
#include "SubTitle_Manager.h"
CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
	, m_pIMGUI_Manager{ CImgui_Manager::Get_Instance() }
	, m_pQTE_Manager{ CQTE_Manager::Get_Instance() }
	, m_pMap_Manager{ CMap_Manager::Get_Instance() }
	, m_pParticle_Manager{ CParticle_Manager::Get_Instance() }
	, m_pSubTitle_Manager{ CSubTitle_Manager::Get_Instance() }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_iLevelIndex = LEVEL_GAMEPLAY;
	m_pMap_Manager->Initialize(m_pDevice, m_pContext);

#pragma region ����Ʈ ����
	Create_Effect_Manager();
	Loading_For_Effect();
#pragma endregion

	Create_QTE_Manager();
	Create_Particle_Manager();
	Create_SubTitle_Manager();

#pragma region ���� �ε�
	if (FAILED(Ready_Sound()))
		return E_FAIL;
#pragma endregion
#pragma region �� �纻 ��ü

	if (FAILED(Ready_Volcano()))
		return E_FAIL;

	if (FAILED(Ready_Space()))
		return E_FAIL;

	m_pMap_Manager->Map_Change(CMap_Manager::MAP_VOLCANO);
#pragma endregion

#pragma region ĳ���� �纻 ��ü

	//�ݵ�� 1P,2P ������ �����ؾ��ϴ°�?

	//Prototype_GameObject_Play_Goku
	//1P
	CCharacter::Character_DESC CharacterDesc{};


	if (CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(0).eSlot == CUI_Define::SLOT_END)
	{
		CharacterDesc.iTeam = 1;
		CharacterDesc.ePlayerSlot = CUI_Define::LPLAYER1;



		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Hit"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;

		CharacterDesc.iTeam = 2;
		CharacterDesc.ePlayerSlot = CUI_Define::RPLAYER1;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_21"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;

		CharacterDesc.iTeam = 1;
		CharacterDesc.ePlayerSlot = CUI_Define::LPLAYER2;



		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Frieza"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;

		CharacterDesc.iTeam = 2;
		CharacterDesc.ePlayerSlot = CUI_Define::RPLAYER2;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Goku"), TEXT("Layer_Character"), &CharacterDesc)))
			return E_FAIL;
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			if (CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).eSlot == CUI_Define::SLOT_END)
				continue;

			CharacterDesc.iTeam = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).iTeam;
			CharacterDesc.ePlayerSlot = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).eSlot;
			_wstring strProtypeTag = CBattleInterface_Manager::Get_Instance()->Get_CharaDesc(i).PrototypeTag;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, strProtypeTag, TEXT("Layer_Character"), &CharacterDesc)))
				return E_FAIL;
		}
	}

	//if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Loading_GodDragon"), TEXT("Layer_UI_Loading_GodDragon"))))
	//	return E_FAIL;
#pragma endregion

#pragma region UI �纻 ��ü
	//ĳ���� ���� ���� IMGUI_UIMANAGER �ʱ�ȭ ���� 
	if (FAILED(Ready_UIObjects()))
		return E_FAIL;

#pragma endregion

#pragma region ī�޶� ����Ʈ �ε�

	//ī�޶� ����
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Main_Camera"))))
		return E_FAIL;

	// ī�޶� ����Ʈ �ε�
	_wstring cameraFilePath = L"../Bin/CameraPoints.txt";
	CameraSaveData cameraSaveData;

	HRESULT hr = m_pGameInstance->Load_All_CameraPoints(cameraFilePath, &cameraSaveData);
	if (FAILED(hr)) {
		// �ε� ���� ó��
		// ��: �α� ����� �Ǵ� �⺻ ���� ����
		return S_OK;
	}

	if (cameraSaveData.models.empty()) {
		// �ε�� �����Ͱ� ������ �⺻ ���� ����
		return S_OK;
	}

	// �ε�� �����͸� mainCamera�� ����
	CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, L"Layer_Main_Camera"));
	if (mainCamera) {
		mainCamera->ApplyCameraData(cameraSaveData);
	}

#pragma endregion



	//�� �غ�
	if (FAILED(Ready_Lights()))
		return E_FAIL;


	CCharacter::Character_DESC Opening_CharacterDesc{};
	Opening_CharacterDesc.iTeam = 1;
	Opening_CharacterDesc.ePlayerSlot = CUI_Define::SLOT_END;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Opening_Kririn"), TEXT("Layer_Model_Opening"), &Opening_CharacterDesc)))
		return E_FAIL;

	//CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
	//mainCamera->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_FREE);
	CBattleInterface_Manager::Get_Instance()->Character_Opening_AIO();

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Model_Preview"), TEXT("Layer_Model_Preview"))))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	//if (m_pGameInstance->Key_Down(DIK_E))
	//{
	//	m_pUI_Manager->CutSceneUI(TRUE);
	//}
	//
	//if (m_pGameInstance->Key_Down(DIK_R))
	//{
	//	m_pUI_Manager->CutSceneUI(FALSE);
	//}

	if (m_pGameInstance->Key_Down(DIK_F9))
	{
		//m_pRenderInstance->Switch_AllBlackOut(false);
	}
	if (m_pGameInstance->Key_Down(DIK_F10))
	{
		m_pRenderInstance->Switch_AllBlackOut();
	}

	if (m_pGameInstance->Key_Down(DIK_Z))
	{
		LIGHT_DESC			LightDesc{};

		ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		LightDesc.fRange = 30.f;
		LightDesc.vDiffuse = _float4(1.2f, 1.15f, 0.7f, 1.0f);
		//LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
		LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

		LightDesc.fAccTime = 0.f;
		LightDesc.fLifeTime = 3.f;
		LightDesc.strName = "Explosion";
		if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
			return;
	}
	if (m_pGameInstance->Key_Down(DIK_X))
	{
		LIGHT_DESC			LightDesc{};

		ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		LightDesc.fRange = 30.f;

		LightDesc.vDiffuse = _float4(0.9f, 1.1f, 1.7f, 1.0f); // �Ķ��� �迭�� ����

		LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

		LightDesc.fAccTime = 0.f;
		LightDesc.fLifeTime = 3.f;
		LightDesc.strName = "Ray";
		if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
			return;
	}

	m_pUI_Manager->GamePlayUpdate(fTimeDelta);
	m_pIMGUI_Manager->Update(fTimeDelta);
	m_pEffect_Manager->Update(fTimeDelta);
	m_pParticle_Manager->Update(fTimeDelta);
	m_pSubTitle_Manager->Update(fTimeDelta);
	m_pEffect_Manager->Late_Update(fTimeDelta);

	m_pQTE_Manager->Update(fTimeDelta);
	m_pQTE_Manager->Late_Update(fTimeDelta);
	m_pParticle_Manager->Late_Update(fTimeDelta);
	m_pSubTitle_Manager->Late_Update(fTimeDelta);
	m_pMap_Manager->Update(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_V))
		CUI_Manager::Get_Instance()->WinUI(LEVEL_GAMEPLAY);
}

HRESULT CLevel_GamePlay::Render(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("�����÷��̷���"));

	m_pEffect_Manager->Render(fTimeDelta);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Main_Camera"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(30.f, 50.f, 1800.f, 1.f);
	LightDesc.fRange = 10000.f;
	LightDesc.vDiffuse = _float4(0.78f, 0.95f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.3f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.1f);


	if (FAILED(m_pRenderInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UIObjects()
{
	//UIMANAGER �ʱ�ȭ
	m_pUI_Manager->InitUIObject();

	CUIObject::UI_DESC tHpDesc = {};

	for (int i = 0; i < 2; ++i)
	{
		tHpDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_HpPanel"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;

		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_HpEffect"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
		//	return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_HpGauge"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SubHpGauge"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SubHpPanel"), TEXT("Layer_UI_HpGauge"), &tHpDesc)))
			return E_FAIL;
	}
	//ĳ���� ������

	CUIObject::UI_DESC Icon_Desc = {};

	for (int i = 0; i < 2; ++i)
	{
		Icon_Desc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Chara_Icon_Panel"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Chara_Icon"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Chara_SubIcon"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Sub_Chara_Icon_Panel"), TEXT("Layer_UI_Chara_Icon"), &Icon_Desc)))
			return E_FAIL;
	}


	//���ݷ� ����

	CUIObject::UI_DESC AttBufoDesc = {};

	for (int i = 0; i < 2; ++i)
	{
		AttBufoDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufMark"), TEXT("Layer_UI_AttBufEffect"), &AttBufoDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBuf"), TEXT("Layer_UI_AttBufEffect"), &AttBufoDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_AttBufNone"), TEXT("Layer_UI_AttBuf"), &AttBufoDesc)))
			return E_FAIL;
	}

	// ��ų
	CUIObject::UI_DESC SkilloDesc = {};
	for (int i = 0; i < 2; i++)
	{
		SkilloDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillGauge"), TEXT("Layer_UI_SkillGauge"), &SkilloDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillGaugeBar"), TEXT("Layer_UI_SkillGauge"), &SkilloDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillNumber"), TEXT("Layer_UI_SkillNumber"), &SkilloDesc)))
			return E_FAIL;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_SkillEffect"), TEXT("Layer_UI_SkillGauge"), &SkilloDesc);


	}

	//Ÿ�̸�

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_TimerPanel"), TEXT("Layer_UI_Timer"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Timer"), TEXT("Layer_UI_Timer"))))
		return E_FAIL;


	//�޺�
	CUIObject::UI_DESC ComboDesc = {};
	for (int i = 0; i < 3; ++i)
	{
		ComboDesc.iNumUI = i;

		for (int k = 0; k < 2; k++)
		{
			ComboDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(k);
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ComboNumber"), TEXT("Layer_UI_Combo_Number"), &ComboDesc);
		}
	}

	for (int k = 0; k < 2; k++)
	{
		ComboDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(k);

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ComboFont"), TEXT("Layer_UI_Combo_Font"), &ComboDesc);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_ComboEffect"), TEXT("Layer_UI_Combo_Effect"), &ComboDesc);
	}

	//Key Input


	CUIObject::UI_DESC KeyInputDesc = {};
	for (int i = 0; i < 2; i++)
	{
		KeyInputDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputDirPanel"), TEXT("Layer_UI_Input"), &KeyInputDesc);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputDir"), TEXT("Layer_UI_Input"), &KeyInputDesc);
	}

	for (int i = 0; i < 2; i++)
	{
		KeyInputDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		for (int k = 0; k < 6; k++)
		{
			KeyInputDesc.iNumUI = k;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_InputAction"), TEXT("Layer_UI_InputAction"), &KeyInputDesc);
		}
	}

	CUIObject::UI_DESC FontNameDesc = {};
	for (int i = 0; i < 2; ++i)
	{
		KeyInputDesc.eLRPos = static_cast<CUIObject::UI_LRPOS>(i);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_FontName"), TEXT("Layer_UI_FontName"), &KeyInputDesc);

	}


	//�ɼ�

	//���� �ɼ�
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Panel"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Effect"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Title"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Font"), TEXT("Layer_UI_Option_Sound"));
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Arrow"), TEXT("Layer_UI_Option_Sound"));

	for (int i = 0; i < 3; i++)
	{
		CUIObject::UI_DESC VolumePanelDesc = {};
		VolumePanelDesc.iNumUI = i;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Volume_Panel"), TEXT("Layer_UI_Option_Sound"), &VolumePanelDesc);
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_Opt_Sound_Volume_Gauge"), TEXT("Layer_UI_Option_Sound"), &VolumePanelDesc);
	}




	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Sound()
{
	//���ֹ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGM/013_bat_space.ogg", CSound_Manager::SOUND_KEY_NAME::SPACE_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);
	//ȭ������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/BGM/004_bat_volcano.ogg", CSound_Manager::SOUND_KEY_NAME::VOLCANO_BGM, CSound_Manager::SOUND_CATEGORY::BGM, true);

#pragma region Goku_Sound

#pragma region Voice

	//�� ����1,2
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�� ����3 (ȣ��), (��������)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Heavy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//���� ���� (����ī)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Upper_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Upper_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//���Ÿ� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Range_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Range_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//���׸��� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Energy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Energy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//���� ��Ʈ �� 16��
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Hit_0.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Goku_Hit_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Hit_1.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Goku_Hit_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Goku, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Hit_2.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Goku_Hit_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//��� (�̱���)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Grab.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Grab, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//��� ���� ���� (�ƴٴٴ�)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Grab_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Grab_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);



	//�ʻ��_0
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Ultimate_0.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_Attack_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�ʻ��_1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Ultimate_1.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_Attack_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�ʻ��_2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Ultimate_2.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_Attack_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);


	//�����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/Entry.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Entry, CSound_Manager::SOUND_CATEGORY::VOICE, false);

#pragma endregion

#pragma region SFX

	//SFX

	//Light_Attack
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Light_Attack_SFX_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Light_Attack_SFX_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_2_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Light_Attack_SFX_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Goku_3_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�뽬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Dash_SFX.wav", CSound_Manager::SOUND_KEY_NAME::Common_Dash_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//�ٿ� �� �뽬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/DownHook_Dash_SFX.wav", CSound_Manager::SOUND_KEY_NAME::Common_DownHook_Dash_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//���׸����� �غ�
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Energy_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Energy_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//���׸����� �߻�
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Energy_Fire_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Energy_Fire_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Heavy_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Heavy_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);
	//���Ÿ� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Range_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Range_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);


	//���ʻ�� 1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_0.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_0_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//���ʻ�� 2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_1.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//���ʻ�� 3
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_2.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_2_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//���ʻ�� 4
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Ultimate_3.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Ultimate_3_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���Ž� Hit
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Smash_Hit.ogg", CSound_Manager::SOUND_KEY_NAME::Smash_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��ź Hit
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/J_Attack_Hit.ogg", CSound_Manager::SOUND_KEY_NAME::J_Attack_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);

	// �� + �� + �����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/Down_Forward_Light_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_Down_Forward_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);


	//�����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_SFX/ARC_BTL_DS02_EntryGKS_Start.wav", CSound_Manager::SOUND_KEY_NAME::Goku_Entry_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion


#pragma endregion

#pragma region 21_Sound

#pragma region 21_Voice

	//���
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_3.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Light_Attack_4.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_21_4, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//�Ʒ� �� ��� (��ź)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Down_Forward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Down_Forward_Light_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//��Ʈ
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_0.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_1.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_2.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_3.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_4.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_4, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_21, L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Hit_21_5.wav", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_21_5, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//�׷� (ī�װ�� ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Grab.ogg", CSound_Manager::SOUND_KEY_NAME::Grab_Attack_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//���� (���ٿ�)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/ChaseDash.ogg", CSound_Manager::SOUND_KEY_NAME::Chase_Attack_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�Ʒ� �� J (����) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Down_Forward_J.ogg", CSound_Manager::SOUND_KEY_NAME::Down_Forward_J_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�Ʒ� �� ����� (�����̵�) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Down_Backward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Down_Backward_Light_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//1�� (�̼��� �ưԸ�) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_1_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Start_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3�� ���� (������ �̳�����) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Start_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3�� �߰� (����!!) // �̰� �¿��̰� �պ������, ���� ��� �Ҷ� �ߺ� ȣ�⹮�� ������ X
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_3_Middle.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Middle_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3�� ������ (���翡 Ű�����̿��) // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Ultimate_3_Final.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Final_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//J ��ź ���̽� // O
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/J_Attack_Voice.ogg", CSound_Manager::SOUND_KEY_NAME::J_Attack_Voice_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//�������� (���ڳ�)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Air_Chase.ogg", CSound_Manager::SOUND_KEY_NAME::Air_Chase_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_Voice/Entry_0.ogg", CSound_Manager::SOUND_KEY_NAME::Entry_21, CSound_Manager::SOUND_CATEGORY::VOICE, false);
#pragma endregion

#pragma region 21_SFX

	//�׷�_SFX_0
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Grab_0.ogg", CSound_Manager::SOUND_KEY_NAME::Grab_Attack_0_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//�׷�_SFX_1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Grab_1.ogg", CSound_Manager::SOUND_KEY_NAME::Grab_Attack_1_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//J ��ź
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/J_Attack_21.ogg", CSound_Manager::SOUND_KEY_NAME::J_Attack_21, CSound_Manager::SOUND_CATEGORY::SFX, false, true);

	//1�� Start
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Start_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1�� Charge
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Charge.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Charge_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1�� Expl
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Expl.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Expl_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1�� Shot
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_1_Shot.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_1_Shot_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� Start
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Start_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� Dash
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Dash.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Dash_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� Attacks
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Attacks.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Attacks_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� Finish
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/Ultimate_3_Finish.ogg", CSound_Manager::SOUND_KEY_NAME::Ultimate_3_Finish_21_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/ARC_BTL_TON_Entry_1.ogg", CSound_Manager::SOUND_KEY_NAME::Entry_21_SFX_0, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/21/Real_SFX/ARC_BTL_TON_Entry_2.ogg", CSound_Manager::SOUND_KEY_NAME::Entry_21_SFX_1, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion

#pragma endregion

#pragma region Hit Sound

#pragma region Hit_Voice

	//����� 4�� �׷����
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_3.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_Hit_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//����� ������ ��Ÿ ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Light_Attack_Final_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Light_Attack_Final_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//J ���� (�ݻ�)
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::J_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_J_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_J_Attack_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::J_ATTACK_HIT, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_J_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_J_Attack_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_Hit, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �Ʒ� �� ����� (�غ��ڼ�) (���, ������)
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::HIT_Down_Back_Light, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Back_Light_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_Down_Back_Light_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::HIT_Down_Back_Light, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Back_Light_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::HIT_Down_Back_Light_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �Ʒ� �� ����� (�غ��ڼ�) (��ũ��)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �Ʒ� �� ����� + ����� (�d)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light_Light_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �Ʒ� �� ����� + �߰��� (��ġ���)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light_Middle_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Middle_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �Ʒ� �� ����� + ������ (��!)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Down_Forward_Light_Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Heavy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �������� ( ���̵��̷�, �綼
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Air_Chase_0.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Air_Chase, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Back_Dash.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_BackDash, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// ��� �õ� (����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Grab_Try.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Grab_Try, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//��� �����ؼ� ��Ÿ�Ҷ� (�ҿ��~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Grab_Success.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Grab_Success, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//1�� (��ī�� ���̳� ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_1.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//3��
	//ù��° (�̸��� ī������)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_0.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�ι�° (�ڿ��)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_1.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//����° (�Ͼƾ�~!!) ��Ÿ
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_2.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�׹�° (������ ����~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Ultimate_3_3.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//�ǰ�
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Hit, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Hit_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_Hit_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Hit, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Hit_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_Hit_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Hit, L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_Voice/Hit_Hit_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_Hit_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);

#pragma endregion

#pragma region Hit_SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Light_Attack_0_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//J ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_J_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_J_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// �Ʒ� �� ����� + ����� + �߰���
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Down_Forward_Light_Light_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// �Ʒ� �� ����� + ������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Down_Forward_Light_Heavy_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Heavy_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// �Ʒ� �� ����� ������� ���� ���� ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Down_Backward_Light_Cancle_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Down_Backward_Light_Cancle_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1�� ��ŸƮ, 3�� ��ŸƮ
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ultimate_1_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1_Start, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//1�� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ultimate_1_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1_Attack, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� ��Ÿ ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ultimate_3_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_Attack, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���̽� ���� ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/Hit_Ice_Hit_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Ice_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/Real_SFX/ARC_BTL_HTN_Entry.ogg", CSound_Manager::SOUND_KEY_NAME::Hit_Entry_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion


#pragma endregion

#pragma region Frieza Sound

#pragma region Frieza_Voice

	//�ǰ� 
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_0_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_1_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_2_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Hit_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Hit_3.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Hit_3_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);


	//��� 3��, �߰��� ����
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_0.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_0_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_1.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_1_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY::Light_Attack_Frieza, L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_2.ogg", CSound_Manager::SOUND_GROUP_KEY_NAME::Light_Attack_2_Frieza, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//��� ������ (�����, �Ʒ� ��ź)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Light_Attack_Final.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Light_Attack_Final, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//��뽬
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Back_Dash.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Back_Dash, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//J ��ź (�Ǵ���)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/J_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//������ (�Ҷ� �����õ���ī��~) , �Ʒ� ������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Heavy_Attack.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Heavy_Attack, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//�Ʒ� �� ����� (���� ��Ű�ٿ�), �߰���, ������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Down_Forward_Light.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Forward_Light, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//�Ʒ� �� J(��ź) (������!~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Down_Forward_J.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Forward_J, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// �Ʒ� �� J(��ź) (������ �����׷簡) (�θ޶�)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Down_Back_J.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Back_J, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// ���
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Grab.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Grab, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 1�� ���� (���ڸ�����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_0_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Start, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 1�� ������ (�ó�!)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_0_End.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_End, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 3�� ���� (�̸� ���׳�~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Start, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// 3�� ������ (����~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Ultimate_3_End.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_End, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// ���� ���� (��Ű������)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Air_Chase.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Air_Chase, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// ��� ������ (�Ʒ� + �� + ��Ư)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Golden.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Golden, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	// ���� �ݻ� (����~)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Reflect.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Reflect, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_Voice/Entry.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Entry, CSound_Manager::SOUND_CATEGORY::VOICE, false);

#pragma endregion

#pragma region Frieza_SFX

	//����� SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Light_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Light_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//����� ������ SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Light_Attack_Final_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Light_Attack_Final_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��ź SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/J_Attack_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_J_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false, false);

	//������ SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Heavy_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Heavy_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���� SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Knock_Away_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Knock_Away_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�Ʒ� �� �����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Down_Forward_Light_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Forward_Light_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// �Ʒ� �� J(��ź) (�θ޶�)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Down_Backward_J_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Down_Backward_J_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 1�� �������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_0_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 1�� �߻�
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_0_Fire.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Fire_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	// 1�� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_0_Expl.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_0_Expl_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_3_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//3�� ���� _0 (���� ����)
	//3�� ���� _1 (���� ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_3_Expl_0.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Expl_0_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Ultimate_3_Expl_1.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Ultimate_3_Expl_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Golden_Frieza.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Golden_Frieza_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���� �ݻ�
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/Reflect_SFX.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Reflect_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����1
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/ARC_BTL_FRN_Entry1.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Entry_0_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����2
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Frieza/Real_SFX/ARC_BTL_FRN_Entry2.ogg", CSound_Manager::SOUND_KEY_NAME::Frieza_Entry_1_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion

#pragma endregion

#pragma region UI �����̼� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0509_BattleCall_Start01.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_READY, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/Narration/JPN/NA_0513_BattleCall_Fight01.ogg", CSound_Manager::SOUND_KEY_NAME::NARRATION_FIGHT, CSound_Manager::SOUND_CATEGORY::VOICE, false);
#pragma endregion

#pragma region UI ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_MenuOpen.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_OPEN, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_MenuClose.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_CLOSE, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/ARC_MENU_SYS_Cursor.ogg", CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_SYS_Fight.ogg", CSound_Manager::SOUND_KEY_NAME::Common_FIGHT_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_SYS_Finish.ogg", CSound_Manager::SOUND_KEY_NAME::Common_FINISH_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion

#pragma region QTE ����
	//QTE ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/QTE_UI_SOUND/QTE_Sucess.ogg", CSound_Manager::SOUND_KEY_NAME::QTE_SUCCESS_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//������� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/QTE_UI_SOUND/QTE_SAME_GRAB_ICON_Sucess.ogg", CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_SUCCESS_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
	//������� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Audio/UI/QTE_UI_SOUND/QTE_SAME_GRAB_ICON_Fail.ogg", CSound_Manager::SOUND_KEY_NAME::QTE_SAMEGRAB_FAIL_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);
#pragma endregion

#pragma region ���� ����


#pragma region SFX

	//����� �¾��� ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Hit_Small-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Small_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�߰��� �¾��� ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Hit_Midle-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Medium_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//������ �¾��� ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Hit_Large-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Heavy_Hit_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Guard_Small-A.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Small_Guard_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�߰��� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Guard_Midle.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Medium_Guard_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//������ ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_Guard_Large.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Heavy_Guard_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_GK_Jump.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Jump_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_PC_AirDash.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Back_Dash_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���߿��� ���� ������ ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_GroundCrush.ogg", CSound_Manager::SOUND_KEY_NAME::Common_GroundCrush_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_JetDashStart.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Dash_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��� ������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_JetDashLoop_LP.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Dash_Loop_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��� ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_JetDashStop_Basa.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Dash_End_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//���� �ε����� ��
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_CrashAirWall.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Crash_Air_Wall_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��� ���� (�巡�� ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_DrgnRush_Start.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Grab_Start_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��� �� (�巡�� ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_DrgnRush_Rush.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Grab_Rush_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//��� �� (�巡�� ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Common/Real_SFX/ARC_BTL_CMN_DrgnRush_Fnsh.ogg", CSound_Manager::SOUND_KEY_NAME::Common_Grab_End_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion



#pragma endregion

#pragma region ���� + ũ���� vs ������

	//Voice
	// 
	//������ �׸���! (�߸޴� ������!)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-003.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_0, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//����!!		(ũ����)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-004.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_1, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//������ �����Ҹ� (ũũ)
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-006.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_2, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//�뼭 ����.. �ߵ� (�츣��..!) -> �տ����� ���
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-007.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_3, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//����..? (������) 
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-009.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_4, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	//������ �ͱ����! (�� ȭ���� ������) -> �տ��� ���
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ds02-010.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_5, CSound_Manager::SOUND_CATEGORY::VOICE, false);

	//SFX
	//������ ���� SFX
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_CMN_Hit_FRN_DthScr.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Expl_SFX_0, CSound_Manager::SOUND_CATEGORY::SFX, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_CMN_Hit_FRN_DtRtzn.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Expl_SFX_1, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�տ��� �������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_CMN_Charge_Loop_LP.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Goku_ChargeLoof_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�տ��� �������
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_GKS_SprKameha_Chrg.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Goku_Charge_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

	//�տ��� ȭ���� �� ȿ����
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku_vs_Frieza/ARC_BTL_SYS_ActSkill_Normal.ogg", CSound_Manager::SOUND_KEY_NAME::Goku_vs_Frieza_Goku_Attack_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion

#pragma region ��Ʈ �¸���
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Goku/Real_Voice/vhtn404_vs_gkb.ogg", CSound_Manager::SOUND_KEY_NAME::HIT_WIN, CSound_Manager::SOUND_CATEGORY::VOICE, false);
	m_pGameInstance->Register_Sound(L"../Bin/SoundSDK/AudioClip/Chara/Hit/SFX/ARC_BTL_HTN_Win.ogg", CSound_Manager::SOUND_KEY_NAME::HIT_WIN_SFX, CSound_Manager::SOUND_CATEGORY::SFX, false);

#pragma endregion


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Space()
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Shadow_Camera"), TEXT("Layer_A"))))
		return E_FAIL;

	CMap_Manager::Map_Object_Key tDesc{};
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceSky");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceSky"), TEXT("Layer_SpaceSky"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceHorizon");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceHorizon"), TEXT("Layer_SpaceHorizon"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceSun");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceSun"), TEXT("Layer_SpaceSun"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceGround");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceGround"), TEXT("Layer_SpaceGround"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceCliff");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceCliff"), TEXT("Layer_SpaceSky"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceStage");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceStage"), TEXT("Layer_SpaceStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceEarth");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceEarth"), TEXT("Layer_Space_Earth"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceEarth_Light");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceEarth_Light"), TEXT("Layer_SpaceEarth_Light"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_FallingStar");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_FallingStar"), TEXT("Layer__FallingStar"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceMoon");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceMoon"), TEXT("Layer_SpaceMoon"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceStone");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceStone"), TEXT("Layer_SpaceStone"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceRock");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceRock"), TEXT("Layer_SpaceRock"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceMeteoBreak");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceMeteoBreak"), TEXT("Layer_MeteoBreak"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_SpaceEF");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SpaceEF"), TEXT("Layer_MeteoBreakA"), &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Volcano()
{
	CMap_Manager::Map_Object_Key tDesc{};
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_VolcanoEF");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_VolcanoEF"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;

	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Stage");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Stage"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff_Back");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff_Back"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff_Far");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff_Far"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff04");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff04"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff03");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff03"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff02");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff02"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Cliff01");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Cliff01"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Ground");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Ground"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_GroundRock");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_GroundRock"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Island01");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Island01"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Island02");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Island02"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Lava_Pool");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Lava_Pool"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Lava_Fall");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Lava_Fall"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Lava_Ground");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Lava_Ground"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_SkyCloud");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_SkyCloud"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Smoke");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Smoke"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Mountain");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Mountain"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;
	tDesc.m_PrototypeKey = TEXT("Prototype_GameObject_Volcano_Destructive");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Volcano_Destructive"), TEXT("Layer_VolcanoStage"), &tDesc)))
		return E_FAIL;





	return S_OK;
}

void CLevel_GamePlay::Create_Effect_Manager()
{
	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	m_pEffect_Manager->Initialize(m_pDevice, m_pContext);
}

void CLevel_GamePlay::Create_QTE_Manager()
{
	m_pQTE_Manager = CQTE_Manager::Get_Instance();
	m_pQTE_Manager->Initialize(m_pDevice, m_pContext);
}

void CLevel_GamePlay::Create_Particle_Manager()
{
	m_pParticle_Manager = CParticle_Manager::Get_Instance();
	m_pParticle_Manager->Initialize(m_pDevice, m_pContext);
}

void CLevel_GamePlay::Create_SubTitle_Manager()
{
	m_pSubTitle_Manager = CSubTitle_Manager::Get_Instance();
	m_pSubTitle_Manager->Initialize(m_pDevice, m_pContext);
}

HRESULT CLevel_GamePlay::Loading_For_Effect()
{
	vector<EFFECT_LAYER_DATA>* pLoaded = static_cast<vector<EFFECT_LAYER_DATA>*>(m_pGameInstance->Load_All_Effects());

	return CEffect_Manager::Get_Instance()->Set_Saved_Effects(pLoaded);
}

HRESULT CLevel_GamePlay::Ready_Character()
{
	CCharacter::Character_DESC SlotDesc = {};

	//�÷��̾� ����
	SlotDesc.ePlayerSlot = CUI_Define::LPLAYER1;
	SlotDesc.fSpeedPerSec = 5.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_Goku"), TEXT("Layer_Character"), &SlotDesc)))
		return E_FAIL;

	//SlotDesc.ePlayerSlot = CUI_Define::LPLAYER2;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_21"), TEXT("Layer_Character"), &SlotDesc)))
	//	return E_FAIL;



	//2��

	SlotDesc.ePlayerSlot = CUI_Define::RPLAYER1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Play_21"), TEXT("Layer_Character"), &SlotDesc)))
		return E_FAIL;




	m_pUI_Manager->InitUIObject();

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	CFrameEvent_Manager::Destroy_Instance();
	Safe_Release(m_pEffect_Manager);

	Safe_Release(m_pQTE_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pMap_Manager);
	Safe_Release(m_pParticle_Manager);
	Safe_Release(m_pSubTitle_Manager);
}
