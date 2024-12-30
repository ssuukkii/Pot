#include "stdafx.h"
#include "..\Public\Play_Hit.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "AttackObject.h"
#include "AttackObject_Grab.h"
#include "AttackObject_CommandGrab.h"
#include "AttackObject_Ranged.h"

#include "AttackObject_Energy.h"

#include "AttackObject_Reflect.h"

#include "UI_Manager.h"
#include "iostream"

#include "Effect.h"
#include "BattleInterface.h"


#include "Main_Camera.h"

#include "BoneEffectObject.h"
#include "Effect_Manager.h"	

#include "Effect_Layer.h"

#include "Animation.h"
#include "Main_Camera.h"

#include "QTE_Manager.h"
#include "Map_Manager.h"

CPlay_Hit::CPlay_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter{ pDevice, pContext }
{

}

CPlay_Hit::CPlay_Hit(const CPlay_Hit& Prototype)
	: CCharacter{ Prototype }
{

}

HRESULT CPlay_Hit::Initialize_Prototype()
{
	//m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();
	CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
	CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Hit.txt");

	return S_OK;
}

HRESULT CPlay_Hit::Initialize(void* pArg)
{

	
	m_ChaseEffectName = TEXT("Hit_BurstR");

	m_fAIrGrabEndAnimationPositon = 39.99f;

	m_eCharacterIndex = PLAY_HIT;
	m_iFallAnimationIndex = ANIME_JUMP_DOWN;
	m_iIdleAnimationIndex = ANIME_IDLE;
	m_iStandingMidAttackAnimationIndex = ANIME_ATTACK_MEDIUM;
	m_iChaseAnimationIndex = ANIME_CHASE;
	m_iForwardDashAnimationIndex = ANIME_FORWARD_DASH;

	m_iForwardDashEndAnimationIndex = { ANIME_FORWARD_DASH_END };

	m_iCrouchAnimationIndex = { ANIME_CROUCHING };
	m_iBackWalkAnimationIndex = { ANIME_BACK_WALK };
	m_iForwardWalkAnimationIndex = { ANIME_FORWARD_WALK };
	m_iHit_Away_LeftAnimationIndex = { ANIME_HIT_HEAVY_AWAY_LEFT };
	m_iHit_Away_UpAnimationIndex = { ANIME_HIT_HEAVY_AWAY_UP };
	m_iHit_Away_LeftDownAnimationIndex = { ANIME_HIT_HEAVY_AWAY_LEFTDOWN };

	m_iHit_WallBouce = { ANIME_HIT_WALLBOUNCE };

	m_iHit_Air_LightAnimationIndex = { ANIE_HIT_AIR };
	m_iHit_Air_FallAnimationIndex = { ANIME_HIT_FALL };

	m_iHit_Air_Spin_LeftUp = { ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP };
	m_iHit_Air_Spin_Up = { ANIME_HIT_HEAVY_AWAY_SPIN_UP };

	m_iAttack_Air1 = { ANIME_ATTACK_AIR1 };
	m_iAttack_Air2 = { ANIME_ATTACK_AIR2 };
	m_iAttack_Air3 = { ANIME_ATTACK_AIR3 };
	m_iAttack_AirUpper = { ANIME_ATTACK_UPPER_AIR };

	m_iAttack_LightLast = { ANIME_ATTACK_LIGHT3 };
	m_iAttack_Heavy = { ANIME_ATTACK_HEAVY };
	m_iAttack_Crouch_Heavy = { ANIME_ATTACK_CROUCH_HEAVY };

	m_iBound_Ground = { ANIME_HIT_BOUND_DOWN };
	m_iLayUp = { ANIME_LAYUP };

	m_iGuard_GroundAnimationIndex = { ANIME_GUARD_GROUND };
	m_iGuard_CrouchAnimationIndex = { ANIME_GUARD_CROUCH };
	m_iGuard_AirAnimationIndex = { ANIME_GUARD_AIR };

	m_iGrabReadyAnimationIndex = { ANIME_GRAB_READY };
	m_iGrabAnimationIndex = { ANIME_GRAB };




	m_iJumpAnimationIndex = { ANIME_JUMP_UP };
	m_iHit_Stand_LightAnimationIndex = { ANIME_HIT_LIGHT };
	m_iHit_Stand_MediumAnimationIndex = { ANIME_HIT_LIGHT_FRONT };
	m_iHit_Crouch_AnimationIndex = { ANIME_HIT_CROUCH };
	m_iBreakFall_Ground = { ANIME_BREAK_FALL_GROUND };   //100
	m_iBreakFall_Air = { ANIME_BREAK_FALL_AIR };   //101


	m_iSparkingAnimationIndex = { ANIME_SPARKING };
	m_iDyingStandingAnimationIndex = { ANIME_DIE_STAND };

	m_iReflectAnimationIndex = { ANIME_REFLECT };


	m_iStartAnimatonIndex = { ANIME_START_DEFAULT }; //600cs 
	m_iWinAnimationIndex = { ANIME_WIN_DEFAULT };	//610cs
	m_iNextRound_RightHandAppear_Cutscene_AnimationIndex = { ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE }; //620c
	m_iNextRound_RightHand_AnimationIndex = { ANIME_NEWROUND_RIGHTHAND };  //621cs ->631���� ����

	m_iNextRound_LeftHand_Cutscene_AnimationIndex = { ANIME_NEWROUND_LEFTHAND_CUTSCENE };  //630 Durtaio
	m_iNextRound_LeftHand_AnimationIndex = { ANIME_NEWROUND_LEFTHAND };  //631 Durtaion 24


	m_iNextAnimation.first = ANIME_IDLE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-2.f + (m_iPlayerTeam * 2), 0.f, 0.f, 1.f));


	m_tAttackMap.Initalize(this);
	Character_DESC* pDesc = static_cast<Character_DESC*>(pArg);
	m_strName = "HIT_" + to_string(m_iPlayerTeam) + "_" + to_string(pDesc->ePlayerSlot);
	m_RendererDesc.strName = m_strName;
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

	//LightDesc.vDirection = _float4(-0.15f, -0.7f, 0.5f, 0.f);
	LightDesc.vDirection = _float4(-1.f, -0.2f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 1.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.pPlayerDirection = &m_iLookDirection;
	LightDesc.strName = m_strName;
	//LightDesc.vAuraColor = _float4(1.411f, 2.066f, 192.9f, 12.89f);
	LightDesc.vAuraColor = _float4(0.f,0.f,0.f,0.f);

	m_fAuraColor = _float4(1.411f, 2.066f, 192.9f, 12.89f);


	if (FAILED(m_pRenderInstance->Add_Player_Light(m_strName, LightDesc, _float4(1.f, 1.54902f, 2.f, 1.f), &m_bChase)))
		return E_FAIL;
	/*
	�� ���� �������ֱ�
	*/

	//m_eChaseSoundIndex = (_short)CSound_Manager::SOUND_KEY_NAME::Chase_At;

	//m_pModelCom->SetUp_Animation(16, true);
	m_pModelCom->SetUp_Animation(0, true);
	m_pModelCom->Play_Animation(0.f);

	//MoveCommandPatterns.push_back({ hadoukenPattern, ShowInputBuffer});

	MoveCommandPatterns.push_back({ Command_236Attack, 66 });
	MoveCommandPatterns.push_back({ Command_236Attack_Extra, 66 });

	MoveCommandPatterns.push_back({ Command_LightAttack, ANIME_ATTACK_LIGHT1 });
	MoveCommandPatterns.push_back({ Command_MediumAttack, ANIME_ATTACK_MEDIUM });
	MoveCommandPatterns.push_back({ Command_HeavyAttack, ANIME_ATTACK_HEAVY });


	MoveCommandPatternsFunction.push_back({ Command_236UltimateAttack, bind(&CHit_MeleeAttack::Attack_236Ultimate, &m_tAttackMap) });




	MoveCommandPatternsFunction.push_back({ Command_236Attack,  bind(&CHit_MeleeAttack::Attack_236, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_236Attack_Extra,  bind(&CHit_MeleeAttack::Attack_236, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_214Attack,  bind(&CHit_MeleeAttack::Attack_214, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_214Attack_Extra,  bind(&CHit_MeleeAttack::Attack_214, &m_tAttackMap) });

	MoveCommandPatternsFunction.push_back({ Command_236Special,  bind(&CHit_MeleeAttack::Attack_236Special, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_214FinalAttack, bind(&CHit_MeleeAttack::Attack_214Final, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_LightAttack, bind(&CHit_MeleeAttack::Attack_Crouch_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack, bind(&CHit_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack, bind(&CHit_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Crouch_MediumAttack_Extra, bind(&CHit_MeleeAttack::Attack_Crouch_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_Crouch_HeavyAttack_Extra, bind(&CHit_MeleeAttack::Attack_Crouch_Heavy, &m_tAttackMap) });



	//������ ���� Ȯ���ϹǷ� ������ Ŀ�ǵ尡 �Ʒ��� ������
	MoveCommandPatternsFunction.push_back({ Command_LightAttack, bind(&CHit_MeleeAttack::Attack_Light, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack, bind(&CHit_MeleeAttack::Attack_Medium, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack, bind(&CHit_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_SpecialAttack, bind(&CHit_MeleeAttack::Attack_Special, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_HeavyAttack_Extra, bind(&CHit_MeleeAttack::Attack_Heavy, &m_tAttackMap) });
	MoveCommandPatternsFunction.push_back({ Command_MediumAttack_Extra, bind(&CHit_MeleeAttack::Attack_Medium, &m_tAttackMap) });


	MoveCommandPatternsFunction.push_back({ Command_Grab, bind(&CHit_MeleeAttack::Attack_Grab, &m_tAttackMap) });




	MoveCommandPatternsFunction_Exactly.push_back({ Command_Up_SpecialAttack, bind(&CHit_MeleeAttack::Attack_Up_Speical, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_Crouch_SpecialAttack, bind(&CHit_MeleeAttack::Attack_Crouch_Speical, &m_tAttackMap) });


	//MoveCommandPatternsFunction.push_back({ Command_BackDash, bind(&CHit_MeleeAttack::BackDash, &m_tAttackMap) });
	//MoveCommandPatternsFunction.push_back({ Command_Forward, bind(&CHit_MeleeAttack::ForwardDash, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_BackDash, bind(&CHit_MeleeAttack::BackDash, &m_tAttackMap) });
	MoveCommandPatternsFunction_Exactly.push_back({ Command_Forward, bind(&CHit_MeleeAttack::ForwardDash, &m_tAttackMap) });


	MoveCommandPatternsFunction_Exactly.push_back({ Command_Reflect, bind(&CHit_MeleeAttack::Reflect, &m_tAttackMap) });

	MoveCommandPatternsFunction_Exactly.push_back({ Command_BenishingAttack, bind(&CHit_MeleeAttack::Attack_Benishing, &m_tAttackMap) });

	m_eCharacterID = CUI_Define::PLAYER_ID::HIT;

	CBattleInterface_Manager::Get_Instance()->Regist_Character(m_iPlayerTeam, this, m_ePlayerSlot);
	if (m_ePlayerSlot != CUI_Define::PLAYER_SLOT::LPLAYER1 && m_ePlayerSlot != CUI_Define::PLAYER_SLOT::RPLAYER1)
	{
		m_bPlaying = false;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f * m_iPlayerTeam, 100.f, 0.f, 1.f));

		//ĳ���� ����� �°� ���� �߰��ϴ��� m_pColliderCom�� ����
		m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		m_bPlaying = true;

	return S_OK;
}

void CPlay_Hit::Player_Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);


	m_pModelCom->m_fCurrentAnimPosition;

	Update_Tag_In(fTimeDelta);


	if (m_bPlaying == false)
		return;


	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		m_bDebugInputLock = !m_bDebugInputLock;
	}



	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_bDebugInputLock = !m_bDebugInputLock;
	}


	if (m_bDebugInputLock)
		return;


	Update_Opening(fTimeDelta);

	//cout << m_pModelCom->m_fCurrentAnimPosition << endl;

	LaserListUpdate(fTimeDelta);



	//����� ������Ʈ ó��.  return������
	if (m_bDying)
	{
		if (m_bAnimationLock == true)
			Update_AnimationLock(fTimeDelta);
		else
		{
			Character_Play_Animation(fTimeDelta);

			if (Get_fHeight() > 0)
				Update_StunImpus(fTimeDelta);

			Gravity(fTimeDelta);

			if (Check_bWall())
			{
				Move_ForWall();
			}
			_uint iAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;

			//if (m_bDestructiveFinish)
			//{
			//	m_fAccDyingTime += fTimeDelta;
			//	if (m_fAccDyingTime > 7.f)
			//	{
			//		Tag_In(m_ePlayerSlot);
			//		//static_cast<CMain_Camera*>(*(m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")).begin()))->Set_Virtual_Camera(CMain_Camera::VIRTUAL_CAMERA_NORMAL);

			//	}
			//}

			m_fAccDyingTime += fTimeDelta;
			if (m_fAccDyingTime > m_fMaxDyingTime)
			{
				m_bDestructiveFinish = false;
				CBattleInterface_Manager::Get_Instance()->Check_NextRoundFromDeathCharacter(m_iPlayerTeam, Get_NewCharacterslot());
				m_bPlaying = false;
				m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			}
			else if (m_bDyingBlack && m_fAccDyingTime > m_fMaxDyingTime - 0.5f)
			{
				
				CRenderInstance::Get_Instance()->Switch_AllBlackOut();
				m_bDyingBlack = false;
			}
			else if (iAnimationIndex == m_iDyingStandingAnimationIndex)
			{
				Stun_Shake();
			}

		}

		return;
	}
	else
		Update_Dying(fTimeDelta);



	Update_CounterPose(fTimeDelta);
	Update_214Pose(fTimeDelta);

	Update_LoofAnimationCreate(fTimeDelta);

	Update_PreviousXPosition();

	if (m_bGrabbed)
	{

		if (m_bAnimationLock && m_fMaxAnimationLock == 0.f)
		{
			m_bAnimationLock = false;
		}

		if (m_bGrabbedGravity && m_bAnimationLock == false)
		{
			if (Get_fHeight() > 0)
			{
				_float fGravity = (-0.7f * (2 * m_fGravityTime - m_fJumpPower) * (2 * m_fGravityTime - m_fJumpPower) + 4) * 0.1;
				//Add_Move({ 0,-fGravity });
				Add_Move({ m_fImpuse.x * fTimeDelta, -fGravity + m_fImpuse.y * fTimeDelta });
			}

			if (m_fGravityTime * 2.f < m_fJumpPower)
			{
				m_fGravityTime += fTimeDelta;

			}

		}
		if (m_bAnimationLock == false)
		{
			Character_Play_Animation(fTimeDelta);

			if (m_pModelCom->m_iCurrentAnimationIndex == m_iStartAnimatonIndex)
			{
				if (m_bMotionPlaying == false)
				{
					if (m_fAccfirstOpeningTime >= 0.5f)
					{
						m_fAccfirstOpeningTime = 0.f;
						CBattleInterface_Manager::Get_Instance()->Character_Opening_EndForCharacter(m_iPlayerTeam);
					}
					else
					{
						if (m_fAccfirstOpeningTime == 0)
						{
							CRenderInstance::Get_Instance()->Switch_AllBlackOut();
						}
						m_fAccfirstOpeningTime += fTimeDelta;
					}
				}
				else if (m_pGameInstance->Key_Down(DIK_RETURN))
				{
					//���� �ִϸ��̼� ���κ�  ��Ʈ �� 366
					Set_CurrentAnimationPositionJump(365.99);
				}
			}
		}
		else
		{
			Update_AnimationLock(fTimeDelta);
		}
		m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		return;
	}


	Update_BeReflecting(fTimeDelta);

	pEnemyCheck();

	//������ȯ �ڵ�.  �� Ž���� �߰��� ���Ŀ�  CCharacter�� �ű��
	if (Check_bCurAnimationisGroundMove() || m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
	{
		CTransform* pEnemyTransform = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")));

		//�� ������ X�� üũ
		_float fX = XMVectorGetX(pEnemyTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));


		//���̰� ������ ���� ����. �� �� ���� �پ����� �� ���.
		if (fabsf(fX) > 0.05)
		{

			if (fX > 0)
			{
				FlipDirection(1);
			}
			else
			{
				FlipDirection(-1);
			}

		}
	}



	//�ԷµǾ��ִ� Ŀ�ǵ� �ð��� ���� �ֹ�
	InputedCommandUpdate(fTimeDelta);

	//Chase == true�ų�  Stun==ture�� �ȹ���
	if (m_bChase == false && m_bStun == false)   //���Է��� �޾ƾ� �ϴ°�?
	{
		if (InputCommand())
		{
			CheckAllCommands();
		}

	}
	else   //�Է� �Ұ� ���¿��� ���Է¹���.  Ŀ�ǵ� üũ�� ���ϰ� �Է¸� ����
	{
		InputCommand();
	}



	if (m_bAnimationLock == false)
	{

		Sparking_ON(fTimeDelta);

		//���� ���� �ڵ�.
		if (m_iPlayerTeam == 1)
		{
			if (m_pGameInstance->Key_Down(DIK_R))
			{
				Chase_Ready(fTimeDelta);
			}
		}
		else  //Team2
		{
			if (m_pGameInstance->Key_Down(DIK_PGDN))  //PageDown Ű
			{
				Chase_Ready(fTimeDelta);
			}
		}


		if (m_bChase)
		{
			Chase2(fTimeDelta);

			if (m_bChase == false)
			{
				//����ϰ� ü�̽��� �������� �̷���.
				//�������� ���� ���� �ʴ´�?
				if (m_bStun == false)
				{
					m_fAccChaseTime = 0.f;

					//����� ���̰� ������ �� ����
					//if (m_pModelCom->m_fCurrentAnimPosition > 60)
					//{
					//
					//	m_fGravityTime = 0.05f;
					//}
					//else
					//	m_fGravityTime = 0.185f;


					m_fGravityTime = 0.185;// -(m_pModelCom->m_fCurrentAnimPosition) * 0.01;


					Set_Animation(ANIME_JUMP_DOWN, false);
				}
				else //�¾Ƽ� ü�̽��� �������� 
				{
					m_fAccChaseTime = 0.f;
					m_fGravityTime = 0.185f;
				}

			}

		}
		else
		{
			if (m_bChaseStoping)
			{
				m_bChaseStoping = false;
				Set_Animation(m_iFallAnimationIndex);
			}
		}

		if (m_bGrab)
		{
			Chase_Grab(fTimeDelta);
			Character_Attack_Grab(fTimeDelta);
		}




		if (m_bNoEventLoofAnimation)
		{
			Update_NoEventTime(fTimeDelta);

			if (m_bForcedEventLoofAnimation)
			{
				Update_ForcedEventAnimationLoof(fTimeDelta);
			}
			else
			{
				Update_NoEventAnimationLoof(fTimeDelta);
			}
		}
		else
		{



			Character_Play_Animation(fTimeDelta);
			Update_214FinalInvisible(fTimeDelta);




		}
		//�̰� �ݺ������ �ƴѵ� ����� ������� (=������ ��ü�� ������ ���),  �߶� �� ��� �ִϸ��̼� ����
		if (m_bMotionPlaying == false)
		{


			//���� �߰� ���� �ִ���
			//if(m_pModelCom->m_iCurrentAnimationIndex != ANIME_JUMP_DOWN)
			//	AnimeEndNextMoveCheck();



			//if (Check_bCurAnimationisAirHit() || Check_bCurAnimationisHitAway() || m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp)
			if (Check_bCurAnimationisAirHit() || Check_bCurAnimationisHitAway())
			{

				if (m_bHitGroundSmashed == false)
					Set_Animation(m_iHit_Air_FallAnimationIndex);

			}
			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_Air_Spin_LeftUp)
			{
				Set_Animation(m_iHit_Air_LightAnimationIndex);
				m_pModelCom->CurrentAnimationPositionJump(30.f);
			}

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iHit_WallBouce)
			{
				Set_Animation(m_iHit_Air_FallAnimationIndex);
			}
			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iBound_Ground)
			{
				Set_Animation(ANIME_LAYUP);
				Set_CurrentAnimationPositionJump(5.f);
			}
			else if (m_bStun == false && m_pModelCom->m_iCurrentAnimationIndex != m_iFallAnimationIndex && m_pModelCom->m_iCurrentAnimationIndex != m_iHit_Air_LightAnimationIndex)
				AnimeEndNextMoveCheck();

		}

		//�̰Ÿ³�?   m_bMotionPlaying ���ε� ������ ������ Hit Away ���� ���
		else if (m_bMotionPlaying && Check_bCurAnimationisHitAway() && m_bStun == false)
		{
			Set_Animation(m_iHit_Air_FallAnimationIndex);

		}




		//�ִϸ��̼� lock ���߿��� �ߵ� ���ؾ���.   �Ʒ��� ���ڴ� 
		if (m_bStun)
		{
			Stun_Shake();
			m_fAccStunTime += fTimeDelta;
			if (m_fAccStunTime > m_fMaxStunTime)
			{
				m_bStun = false;
				m_fAccStunTime = 0.f;

				if (Check_bCurAnimationisHitGround())
				{
					Reset_AttackStep();
				}

			}
			Update_StunImpus(fTimeDelta);
		}
		else
		{





			BreakFall_Air();


			if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex && Get_fHeight() == 0)
				m_fImpuse = { 0.f,0.f };



		}
		Gravity(fTimeDelta);

		//����� �ű�� 2�������� �ȵ�
		//AttckCancleJump();
		//Move(fTimeDelta);
	}
	else
	{
		Update_AnimationLock(fTimeDelta);

		//m_fAccAnimationLock += fTimeDelta;
		//if (m_fAccAnimationLock > m_fMaxAnimationLock)
		//{
		//	m_fAccAnimationLock = 0.f;
		//	m_bAnimationLock = false;
		//}
	}



	//�߷� ó��.    ANimation Lock�� ������ �޾ƾ��ϳ�? ���� �Űܺ�
	//Gravity(fTimeDelta);

	//�Ϻ� ���� ĵ��
	AttckCancleJump();

	Sparking_TimeCount(fTimeDelta);


	//Animation Lock ���¿����� �����̴� ���� ����

	if (m_bAnimationLock == false)
		Move(fTimeDelta);









	m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));




	if (Check_bWall())
	{
		Move_ForWall();
	}


	Tag_KeyCheck();


	if (m_pGameInstance->Key_Down(DIK_8))
	{
		ShowInputBuffer();
		m_pModelCom->m_fCurrentAnimPosition;
		_float fHeight = Get_fHeight();
		_bool bDebugf = true;
	}
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			DebugPositionReset(-1);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			DebugPositionReset(1);
		}
		else
			DebugPositionReset();

	}

	if (m_pGameInstance->Key_Down(DIK_2))
	{
		m_iAttackStepCount = 0;
		m_iDebugComoboDamage = 0;

		m_iHP = 10000;

		//m_LaserList.push_back({ 0.1f, { 0.2f,0.2f,0.2f } });
		//m_LaserList.push_back({ 0.1f, { 60 + (_float)(rand() % 11), (_float)(rand() % 11),(_float)(rand() % 181)}});


		//m_LaserListRS.push_back({ 0.1f, {0.f, 0.f,(_float)(rand() % 181)},{0.1f,1.f,1.f } });

		//CEffect_Layer* pEffect = Character_Make_BoneEffect("GD_fist_L", TEXT("Hit_SDO-03"));
		//
		//if (pEffect != nullptr)
		//	pEffect->Set_Copy_Layer_Scaled({ -0.5f,1.f,1.f });



	}
	if (m_pGameInstance->Key_Down(DIK_3))
	{
		//system("cls");
		m_iHP = 100;
		//m_LaserListRS.push_back({ 0.1f, {0.f, 0.f,(_float)(rand() % 181)},{1.f,0.1f,1.f } });
		//for (int i = 0; i < 50; i++)
		//{
		//	//m_LaserListRS.push_back({ 0.70f + i*0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
		//	//m_LaserListRS.push_back({ 0.73f + i*0.1f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
		//	//m_LaserListRS.push_back({ 0.76f + i*0.1f, {0.f, 0.f, (_float)(rand() % 361)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
		//
		//	m_LaserListRS.push_back({ 0.70f + i * 0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
		//	m_LaserListRS.push_back({ 0.75f + i * 0.1f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
		//
		//
		//}
		
		//CEffect_Layer* pEffect = Character_Make_BoneEffect("GD_fist_L", TEXT("Hit_SDO-04"));
		//if (pEffect != nullptr)
		//	pEffect->Set_Copy_Layer_Scaled({ 0.2f,1.f,1.f });
		

	}

	
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		Set_bFinalSkillQTE(true);

		//for (int i = 0; i < 3; i++)
		//{
		//	//m_LaserListRS.push_back({ 0.70f + i*0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
		//	//m_LaserListRS.push_back({ 0.73f + i*0.1f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
		//	//m_LaserListRS.push_back({ 0.76f + i*0.1f, {0.f, 0.f, (_float)(rand() % 361)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
		//
		//	m_LaserListRS.push_back({ i * 0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
		//	m_LaserListRS.push_back({ 0.05f + i * 0.1f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
		//
		//}

		//Set_Animation(ANIME_WIN_DEFAULT);

	}
	if (m_pGameInstance->Key_Down(DIK_INSERT))
	{
		CBattleInterface_Manager::Get_Instance()->Gain_KiGuage(50, m_iPlayerTeam);
	}

}

void CPlay_Hit::Camera_Update(_float fTimeDelta)
{

}

void CPlay_Hit::Update(_float fTimeDelta)
{
	__super::Player_Update(fTimeDelta);

}

void CPlay_Hit::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CPlay_Hit::Render(_float fTimeDelta)
{

	if (m_bInvisible == true)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/* ���� ������ �ִ� ���׸��� �� i��° �޽ð� ����ؾ��ϴ� ���׸�����ü�� aiTextureType_DIFFUSE��° �ؽ��ĸ� */
		/* m_pShaderCom�� �ִ� g_DiffuseTexture������ ����. */
		if (m_iPlayerTeam == 1)
		{
			if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_p2PTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
		}
		//m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

	   /* ���� ������ �ִ� ���� �߿��� ���� �������ҷ��� �ߴ� i��°�� �޽ð� ����ϴ� ������ �迭�� ���� ���̴��� �����ش�.  */
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;

	}




	//corlorChange Test
	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	/* ���� ������ �ִ� ���׸��� �� i��° �޽ð� ����ؾ��ϴ� ���׸�����ü�� aiTextureType_DIFFUSE��° �ؽ��ĸ� */
	//	/* m_pShaderCom�� �ִ� g_DiffuseTexture������ ����. */
	//	if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, (aiTextureType)m_iPlayerTeam, "g_DiffuseTexture", i)))
	//		return E_FAIL;
	//	// m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);
	//
	//	/* ���� ������ �ִ� ���� �߿��� ���� �������ҷ��� �ߴ� i��°�� �޽ð� ����ϴ� ������ �迭�� ���� ���̴��� �����ش�.  */
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
	//
	//	if (FAILED(m_pShaderCom->Begin(0)))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}
#ifdef _DEBUG
	//m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG

	return S_OK;
}

_bool CPlay_Hit::Update_CounterPose(_float fTimeDelta)
{

	if (m_bCounterPose == false)
		return false;


	//�ݰ� ������ �ʱ�ȭ �ϰ� ������Ʈ ����
	if (m_bCounterSucces == true)
	{
		m_bCounterPose = false;
		m_fAccPoseTime = 0.f;
		return false;
	}

	m_fAccPoseTime += fTimeDelta;


	if (m_fAccPoseTime > 0.4f)
	{
		m_bCounterPose = false;
		m_fAccPoseTime = 0.f;

		if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL)
			Set_CurrentAnimationPositionJump(64.999f);

		else if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL_AIR)
			Set_CurrentAnimationPositionJump(49.999f);


	}

	return true;
}

_bool CPlay_Hit::Update_214Pose(_float fTimeDelta)
{
	if (m_b214Posing == false)
		return false;


	_bool bKeyPressing = false;


	//�ڼ� ���������� Ȯ��
	if (m_iPlayerTeam == 1)
	{
		bKeyPressing = m_pGameInstance->Key_Pressing(DIK_U);
	}
	else if (m_iPlayerTeam == 2)
	{
		bKeyPressing = m_pGameInstance->Key_Pressing(DIK_NUMPAD7);
	}

	if (m_fAccPoseTime == 0)
	{
		//Character_Make_Effect(TEXT("Hit_SDU-01"), { 5.f ,-0.2f });
		m_214GlassList.push_back((Character_Make_Effect(TEXT("Hit_SDU-01"), { 5.f ,0.f })));
		m_i214GlassCount++;		
	}

	//�������̸� �ð����ϰ� ����Ʈó��
	if (bKeyPressing == true)
	{

		m_fAccPoseTime += fTimeDelta;
		//����Ʈó����?
		
		//�Ÿ� 5
		if(m_fAccPoseTime > m_i214GlassCount*0.5 )
		{


			//Character_Make_Effect(TEXT("Hit_SDU-01"), { 5.f - m_fAccPoseTime,0.f });
			//Character_Make_Effect(TEXT("Hit_SDU-01_Rotated_Right"), { 5.f + m_fAccPoseTime,0.f });

			if (m_i214GlassCount % 2 == 0)
			{

				m_214GlassList.push_back(Character_Make_Effect(TEXT("Hit_SDU-01"), { 5.f - m_fAccPoseTime,0.f }));
				m_214GlassList.push_back(Character_Make_Effect(TEXT("Hit_SDU-01_Rotated_Right"), { 5.f + m_fAccPoseTime,0.f }));
			}
			else
			{

				m_214GlassList.push_back((Character_Make_Effect(TEXT("Hit_SDU-01_Rotated_Right"), { 5.f - m_fAccPoseTime,0.f })));
				m_214GlassList.push_back((Character_Make_Effect(TEXT("Hit_SDU-01"), { 5.f + m_fAccPoseTime,0.f })));
			}
			m_i214GlassCount++;
		}
	}

	else
	{
		//�ڼ��� ���������� �ּ��� 0.2�ʴ� �����ؾ��� ������ ����ó��
		if (m_fAccPoseTime < 0.2f)
		{
			bKeyPressing = true;
			m_fAccPoseTime += fTimeDelta;
		}
		else
		{
			AttackEvent(400);
			m_b214Posing = false;


			for (auto Effectglass : m_214GlassList)
			{
				if(Effectglass != nullptr)
					Effectglass->m_bIsDoneAnim = true;
			}
			m_214GlassList.clear();


			Character_Make_Effect(TEXT("Hit_SDU-02"), { 5.f,0.f });
			
			for (int i = 1; i < m_i214GlassCount; i++)
			{
				Character_Make_Effect(TEXT("Hit_SDU-02"), {5.f+ 0.5f * i,-0.3f });
				Character_Make_Effect(TEXT("Hit_SDU-02"), {5.f- 0.5f * i,-0.3f });
			
			}
		}

	}

}

_bool CPlay_Hit::Get_b236Posing()
{
	return  m_b236Posing;

}

void CPlay_Hit::Set_b236Posing(_bool b236Posing)
{
	m_b236Posing = b236Posing;

}

_bool CPlay_Hit::Get_b236Sepcial()
{
	return m_b236Special;
}

void CPlay_Hit::Set_b236Special(_bool b236Special)
{
	m_b236Special = b236Special;
}




//���� ó�� �����ؼ� Character�� �̻簡�� ���.
/*
void CPlay_Hit::AttackNextMoveCheck()
{

	if (m_iNextAnimation.first != m_iIdleAnimationIndex)
	{
		Set_Animation(m_iNextAnimation.first);

		m_iNextAnimation.first = m_iIdleAnimationIndex;
		m_iNextAnimation.second = 1000.f;

		if (m_fNextAnimationCurrentPosition != 0)
		{
			m_pModelCom->CurrentAnimationPositionJump(m_fNextAnimationCurrentPosition);
			m_fNextAnimationCurrentPosition = 0.f;
		}
	}


}

void CPlay_Hit::AnimeEndNextMoveCheck()
{

	Set_Animation(m_iNextAnimation.first);

	m_iNextAnimation.first = ANIME_IDLE;
	m_iNextAnimation.second = 1000.f;


}



void CPlay_Hit::Set_Animation(_uint iAnimationIndex, _bool bloof)
{

	if (iAnimationIndex == m_iIdleAnimationIndex)
		m_pModelCom->SetUp_Animation(iAnimationIndex, true);
	else
		m_pModelCom->SetUp_Animation(iAnimationIndex, bloof);

}

*/




HRESULT CPlay_Hit::Ready_Components()
{
	/* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;
	//
	///* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;


	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	if (m_iPlayerTeam == 1)
		ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_BODY;
	else
		ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_BODY;
	ColliderDesc.pMineGameObject = this;
	ColliderDesc.vCenter = { 0.f,0.8f,0.f };
	ColliderDesc.vExtents = { 0.5f,0.7f,1.f };


	//Com_Collider
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	m_pGameInstance->Add_ColliderObject(ColliderDesc.colliderGroup, m_pColliderCom);


	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Hit"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;



	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HITOutLine"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pOutLineCom))))
		return E_FAIL;


	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HIT_1P"), TEXT("Com_2PTexture"), reinterpret_cast<CComponent**>(&m_p2PTextureCom))))
		return E_FAIL;




	return S_OK;
}

HRESULT CPlay_Hit::Bind_ShaderResources()
{

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CPlay_Hit::Check_bCurAnimationisGroundMove(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_IDLE || iModelIndex == ANIME_FORWARD_WALK || iModelIndex == ANIME_BACK_WALK || iModelIndex == ANIME_CROUCH_START || iModelIndex == ANIME_CROUCHING
		|| iModelIndex == ANIME_FORWARD_DASH)
	{
		return true;
	}

	return false;
}

_bool CPlay_Hit::Check_bCurAnimationisAttack(_uint iAnimation)
{

	return false;
}

_bool CPlay_Hit::Check_bCurAnimationisAirAttack(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_ATTACK_AIR1 || iModelIndex == ANIME_ATTACK_AIR2 || iModelIndex == ANIME_ATTACK_AIR3 || iModelIndex == ANIME_ATTACK_SPECIAL_AIR)
	{
		return true;
	}

	return false;
}

_bool CPlay_Hit::Check_bCurAnimationisHalfGravityStop(_uint iAnimation)
{
	_uint iModelIndex = iAnimation;

	if (iAnimation == 1000)
		iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iModelIndex == ANIME_ATTACK_SPECIAL_AIR)
	{
		return true;
	}

	return false;
}

_short CPlay_Hit::Check_bCurAnimationisCanChase()
{
	_uint 	iModelIndex = m_pModelCom->m_iCurrentAnimationIndex;



	//������ ü�̽��� �ӵ� �� ������
	//if(m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY || iModelIndex == ANIME_ATTACK_SPECIAL_AIR || iModelIndex == ANIME_ATTACK_LIGHT3))
	//if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY ))
	//{
	//	return 10;
	//}

	if (m_bAttackBackEvent && (iModelIndex == ANIME_ATTACK_HEAVY || iModelIndex == ANIME_ATTACK_CROUCH_HEAVY))
	{
		if (m_b236Special)
			return 0;

		return 10;
	}


	//m_b236Special


	else if (Check_bCurAnimationisGroundMove() || Check_bCurAnimationisAirMove() || iModelIndex == ANIME_ATTACK_LIGHT3)
	{
		return 1;
	}


	return 0;

}

void CPlay_Hit::Reset_AttackCount()
{

	__super::Reset_AttackCount();

	for (size_t i = 0; i < COUNT_END; i++)
		m_bAttackCount[i] = true;


}

void CPlay_Hit::Gravity(_float fTimeDelta)
{
	if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL_AIR)
	{
		return;
	}

	__super::Gravity(fTimeDelta);


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fHeight = XMVectorGetY(vPos);
	_ushort iAnimationIndex = m_pModelCom->m_iCurrentAnimationIndex;



	if (iAnimationIndex == ANIME_ATTACK_AIR1 || iAnimationIndex == ANIME_ATTACK_AIR2 || iAnimationIndex == ANIME_ATTACK_AIR3)
	{

		if (fHeight == 0)
		{

			//m_pModelCom->SetUp_Animation(m_iIdleAnimationIndex, true);
			Set_Animation(m_iIdleAnimationIndex);


			Set_fGravityTime(0.f);
			Set_fJumpPower(0.f);
			Set_fImpulse(0.f);
			m_bAriDashEnable = true;
			Set_bAttackGravity(true);
		}

	}



}

void CPlay_Hit::AttackEvent(_int iAttackEvent, _int AddEvent)
{
	//case Client::CPlay_Hit::ANIME_236_SPECIAL_RIGHT:
	//	break;
	//case Client::CPlay_Hit::ANIME_236_SPECIAL_UP:
	//	break;
	//case Client::CPlay_Hit::ANIME_236_SPECIAL_DOWN:
	//	break;

	switch (m_pModelCom->m_iCurrentAnimationIndex)
	{
	case Client::CPlay_Hit::ANIME_ATTACK_LIGHT1:
	{

		if(iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 0.4f;
			Desc.iDamage = 400 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;


			Desc.iCallAttackBackIndex = 1001;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
		else if (iAttackEvent == 1001)
		{
			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 1.3f,1.2f });
			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,90.f + 90.f * m_iLookDirection });
			pEffect->Set_Copy_Layer_Scaled({ 0.5f,0.9f,1.f });
		}

	}
	break;

	case Client::CPlay_Hit::ANIME_ATTACK_LIGHT2:
	{
		//���� �� �Ⱥ��� + ������ + ���ʿ� ����
		if (iAttackEvent == 0)
		{

			//�Ⱥ��̰�,
			m_bInvisible = true;

			//������ ����Ʈ,

			//��ñ���

			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			m_pEffect_Manager->Copy_Layer(TEXT("Smoke01"), &tDesc);



			Character_Create_Distortion({ -1.f,0.f,0.f });


			//m_pEffect_Manager->Copy_Layer(TEXT("Smoke01_BackZ"), m_pTransformCom->Get_WorldMatrixPtr());
			//m_pEffect_Manager->Copy_Layer(TEXT("Smoke02"), m_pTransformCom->Get_WorldMatrixPtr());
			//m_pEffect_Manager->Copy_Layer(TEXT("Smoke02_Small"), m_pTransformCom->Get_WorldMatrixPtr());
			//m_pEffect_Manager->Copy_Layer(TEXT("Smoke04"), m_pTransformCom->Get_WorldMatrixPtr());


			////Character_Make_Effect(TEXT("Moving_Line_Right"));

			//_float4 fPos{};
			//XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			//m_pRenderInstance->Create_HitDistortion(fPos, { 1,0,0 }, {},{5.f, 5.f},0.3f);



			//CEffect_Layer* pEffect = m_pEffect_Manager->Get_Instance()->Copy_Layer_AndGet(TEXT("Moving_Line_Right"), m_pTransformCom->Get_WorldMatrixPtr());


			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);

		}
		else if (iAttackEvent == 1)
		{


			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);

			//�������� �Ÿ��� Ž���ϰ� �����Ÿ� �ȿ������� �� �ձ���, �׺��� �ָ� �����Ÿ������� �̵��ϴ� �Լ� �ʿ�
			MoveToEnemy_Ground(2.f);
			m_bInvisible = false;

			////Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			//CEffect_Layer* pEffect = m_pEffect_Manager->Get_Instance()->Copy_Layer_AndGet(TEXT("Moving_Line_Right"),m_pTransformCom->Get_WorldMatrixPtr());

			//���� ����

			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 0.4f;


			Desc.iDamage = 700 * Get_DamageScale();
			//Desc.iDamage = 4200 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;

			Desc.iCallAttackBackIndex = 1001;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			//m_pEffect_Manager->Copy_Layer(TEXT("Smoke03_Stop"), m_pTransformCom->Get_WorldMatrixPtr());
			Character_Make_Effect(TEXT("Smoke03_Stop"), {}, true);
		}


		else if (iAttackEvent == 1001)
		{
			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 0.4f,1.2f });
			//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 0.7f,1.6f });
			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 1.2f,1.6f });

			//���ʺ��� 45  �����ʺ��� 135
			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f, 90.f+ 45.f*m_iLookDirection });
			pEffect->Set_Copy_Layer_Scaled({ 0.9f,0.3f,1.f });
		}


	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_LIGHT3:
	{

		// ���������� �Ⱥ���, ���λ�����
		if (iAttackEvent == 0)
		{

			//�Ⱥ��̰�,
			m_bInvisible = true;

			//������ ����Ʈ,
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			//CEffect_Layer* pEffect = m_pEffect_Manager->Get_Instance()->Copy_Layer_AndGet(TEXT("Moving_Line_Right"), m_pTransformCom->Get_WorldMatrixPtr());
			//if (pEffect != nullptr)
			//	pEffect->Set_Copy_Layer_Scaled({ 1.f,0.3f,1.f });

			//��ñ���
			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			m_pEffect_Manager->Copy_Layer(TEXT("Smoke01"), &tDesc);

			if (m_bSparking)
			{
				m_iAttackLightLoofCount = 6;
				//m_pModelCom->Set_MaxAnimationUpdate_Time(fValue[0]);
				m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 240.f;
			}
			else
				m_iAttackLightLoofCount = 2;


		}


		//�Ⱥ��̰��־��ٸ� ���̰�, �� ������ �����Ÿ� �����̵�,  ���λ�����  ,��������, Grab�����ϰ�? ���� ���ϰ��ֱ�, �Ǵ� �׳� ���ϰ�?
		else if (iAttackEvent == 1)
		{
			m_bInvisible = false;


			//�������������� ������ȯ
			if (m_bAttackBackEvent == true)
			{
				FlipDirection();

				if (m_bSparking)
				{
					//m_LaserListRS.push_back({ 0.03f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} }); 

					m_LaserListRS.push_back({ 0.03f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.8f + (rand() % 2) * 0.1f,0.1f + (rand() % 3) * 0.1f} });

				}
			}

			MoveToEnemy_Ground(3.f);

			//������ ����Ʈ,
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			//CEffect_Layer* pEffect = m_pEffect_Manager->Get_Instance()->Copy_Layer_AndGet(TEXT("Moving_Line_Right"), m_pTransformCom->Get_WorldMatrixPtr());
			//
			//if (pEffect != nullptr)
			//	pEffect->Set_Copy_Layer_Scaled({ 1.f,0.3f,1.f });

			if (m_iAttackLightLoofCount != 0)
			{
				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				//Desc.ColliderDesc.vExtents = { 1.4f,2.2f,1.f };
				Desc.ColliderDesc.vExtents = { 0.8f,0.6f,1.f };
				Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 20.f;
				if (m_bSparking)
					Desc.iDamage = 280 * Get_DamageScale(true);
				else
					Desc.iDamage = 320 * Get_DamageScale(true);

				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.fAnimationLockTime = 0.1f;
				Desc.fAnimationLockTime = 0.06f;

				Desc.pOwner = this;

				Desc.eAttackType = ATTACKTYPE_HIGH;
				//Desc.eAttackGrade = GRADE_ULTIMATE;

				Desc.fDistance = { 0.8f * m_iLookDirection,0.f };
				Desc.fForcedGravityTime = 0.f;
				//Desc.fGrabAnimationPosition = 40.f;
				//Desc.fGrabAnimationPosition = 25.f;
				Desc.iGainAttackStep = 0;
				Desc.iGrabAnimationIndex = ANIME_ATTACK_LIGHT3;
				Desc.bOwnerNextAnimation = false;
				//Desc.iGainKiAmount=3; //������ ���� �ȵ�

				//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_SON_ULTIMATE;
				//Desc.ianimationIndex = 1;
				Desc.fCameraShakeDuration = 0.5f;
				Desc.fCameraShakeMagnitude = 0.2f;


				Desc.fForcedGravityTime = 0.f;
				//Desc.bOnwerHitNoneStop = true;
			
				if (m_bSparking)
				{
					//Desc.bOnwerHitNoneStop = true;
					Desc.bOnwerHitNoneStop = true;
				}
				else
				{
					//Desc.strHitEffectName = TEXT("Hit_Hand_Lazer");
					//Desc.fHitEffectOffset = { -0.5f,1.f };

					Desc.iCallAttackBackIndex = 1001;
					
				}

				//Desc.bOnwerHitNoneStop = true;
				Desc.bOwnerNextAnimation = false;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			}

			else if (m_iAttackLightLoofCount == 0)
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
				Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };
			

				Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 5.f;

				Desc.bCameraZoom = false;
				//if (m_iAttackLightLoofCount == 0)
				{
					Desc.iDamage = 450 * Get_DamageScale();
					Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
					Desc.iGainAttackStep = 1;
					Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				

				}
				//else
				//{
				//	Desc.iDamage = 180 * Get_DamageScale();
				//	Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				//	Desc.iGainAttackStep = 0;
				//
				//	Desc.bOnwerHitNoneStop = true;
				//}

				Desc.fLifeTime = 0.1f;



				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.4f;
				Desc.pOwner = this;
				Desc.bGrabbedEnd = true;

				if (m_bSparking)
				{

				}
				else
				{
					Desc.strHitEffectName = TEXT("Hit_Hand_Lazer");
					Desc.fHitEffectOffset = { -0.5f,1.f };

					//Desc.iCallAttackBackIndex = 1002;
				}

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


				//m_pEffect_Manager->Copy_Layer(TEXT("Smoke03_Stop"), m_pTransformCom->Get_WorldMatrixPtr());

			}

		}

		//�������� 55�� �̵��ϰ� ������.  35�� ���� �ո�
		else if (iAttackEvent == 2)
		{
			if (m_bAttackBackEvent == false)
			{
				//������ ����  ���Ѿй� ������
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 40.f;
				Set_CurrentAnimationPositionJump(55.f);


			}
			else if (m_iAttackLightLoofCount == 0)
			{
				Set_CurrentAnimationPositionJump(55.f);
			}
			else
			{
				;
			}

		}

		//,����Ÿ�̹�.  �Ⱥ��̰�, �޸���ô �ϴٰ� �� �ڷ��̵�,  ���λ�����, 24�� �̵��ؼ� ��������. �ʹ������� 20? ���ð��������� ����
		else if (iAttackEvent == 3)
		{
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));

			//CEffect_Layer* pEffect = m_pEffect_Manager->Get_Instance()->Copy_Layer_AndGet(TEXT("Moving_Line_Right"), m_pTransformCom->Get_WorldMatrixPtr());
			//
			//if (pEffect != nullptr)
			//	pEffect->Set_Copy_Layer_Scaled({ 1.f,0.1f,1.f });


			Add_Move({ m_iLookDirection * 3.f,0.f });

			Set_CurrentAnimationPositionJump(24.f);

			m_iAttackLightLoofCount--;

			//�� �ڷ� �̵��� ���� �����ϴ���
			//1. Grab�������� ����� �ִϸ��̼����� �̵��ϱ�
			//2. Add_Move�� �����̵�
		}

		//AttackObject���� ȣ���� ����Ʈ��
		else if (iAttackEvent == 1001)
		{
			//0 ������ 
			//180 ����

			//m_LaserListRS.push_back({ 0.03f, {0.f, 0.f,  90.f-90.f*m_iLookDirection}, {0.8f,0.8f} });
			//m_LaserListRS.push_back({ 0.03f, {0.f, 0.f,  0.f}, {0.8f,0.8f} });


			//m_LaserListRS.push_back({ 0.03f, {0.f, 0.f,  90.f + 90.f * m_iLookDirection}, {0.8f,0.9f} });

			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 0.4f,1.2f });
			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,90.f + 90.f * m_iLookDirection });
			pEffect->Set_Copy_Layer_Scaled({ 0.8f,0.9f,1.f });
		}


	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_MEDIUM:
	{
		//,�Ⱥ��̰�, ���λ�����
		if (iAttackEvent == 0)
		{
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);


			Character_Make_Effect(TEXT("Hit_BurstI-01"));
		}

		//,�� ������ �����̵�, ��������.  �Ⱥ��̰��־��ٸ� ���̰�?
		else if (iAttackEvent == 1)
		{
			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.1f);

			m_bInvisible = false;

			Character_Make_Effect(TEXT("Hit_BurstI-02"));

			Character_Make_BoneEffect("GD_fist_L",TEXT("Hit_BurstI-03"));
			//Character_Make_BoneEffect_Offset("GD_fist_L", TEXT("Hit_BurstI-03"),{-0.3f,0.f});

			

			MoveToEnemy_Ground(2.f);
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 0.7;
			//Desc.ColliderDesc.height = 0.8;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.3f,0.2f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.9f,0.f };
			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 0.4f;
			//Desc.fhitCharacter_StunTime = 0.55f;

			Desc.iDamage = 700 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;

			//Desc.strHitEffectName = TEXT("Hit_BurstI-03");
			//Desc.fHitEffectOffset = { -0.5f,1.f };

			Desc.strHitEffectName = TEXT("Hit_Hand_Lazer");
			Desc.fHitEffectOffset = { -0.5f,1.f };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_HEAVY:
	{

		m_bGrab_Air = false;

		//,�Ⱥ��� + ���λ�����
		if (iAttackEvent == 0)
		{
			m_bInvisible = true;

			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);
		}

		else if (iAttackEvent == 1)
		{

			m_bInvisible = false;

			MoveToEnemy_Ground(3.f);
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);

			{
				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
				Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0.5f };
				Desc.fhitCharacter_StunTime = 1.0f;
				Desc.iDamage = 1000;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.7f;

				Desc.fDistance = { 0.8f,0.05f };
				Desc.iGrabAnimationIndex = CPlay_Hit::ANIME_ATTACK_HEAVY;
				Desc.bOwnerNextAnimation = false;

				Desc.iGainAttackStep = 0;
				Desc.iGainHitCount = 0;

				Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA::VIRTUAL_CAMERA_HIT_HEAVY;
				Desc.fCameraShakeDuration = 0.5f;
				Desc.fCameraShakeMagnitude = 0.2f;

				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;
				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_Hit, false, 1.f);
				Desc.isfxSoundIndex = (_ushort)CSound_Manager::SOUND_KEY_NAME::Goku_Heavy_Attack_SFX;
				Desc.fsfxVolume = 1.f;


				Desc.strHitEffectName = TEXT("Hit_Hand_Lazer");

				if(m_iLookDirection == 1)
					Desc.fHitEffectOffset = { -0.3f,0.9f };
				else
					Desc.fHitEffectOffset = { -1.8f,0.9f };
					//Desc.fHitEffectOffset = { -1.f,0.9f };
			
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			}

			if (false)
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
				Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 1.0f;
				Desc.iDamage = 1000 * Get_DamageScale();;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.7f;
				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;


				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}

			Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.7f,0.f });
		}
		//Character_Make_Effect(TEXT("Ring_Dust_Right"), { 0.4f,0.f });



		//Ring_Dust

	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_SPECIAL:
	{

		//�ݰ� �ڼ� ����
		if (iAttackEvent == 0)
		{
			m_bCounterPose = true;
			//Character_Make_Effect(TEXT("Hit_SDO-02"), { 0.f,0.6f });
			Character_Make_Effect(TEXT("Hit_BurstJ-01"), { 0.f,0.4f });

		}

		//�ݰݼ���. �� �������
		else if (iAttackEvent == 1)
		{
			m_bCounterPose = false;
			m_bCounterSucces = false;
			m_bInvisible = false;

			_vector vTargetPos = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			vTargetPos += {1.3f * m_iLookDirection, 0.f, 0, 0};
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);

			//Teleport_ToEnemy(1.3f, 0.1f);

			FlipDirection();

			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);


			//���� ���� ������ �ݰ�
			if (XMVectorGetY(vTargetPos) == 0)
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
				Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 1.0f;
				Desc.iDamage = 1000 * Get_DamageScale();;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.7f;
				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;

				main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_214_MIDDLE, 0, this);
				main_Camera->StartCameraShake(0.5f, 0.2f);


				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}

			//���� ���߿� ������ ������ ���� 
			else
			{
				Set_Animation(m_iJumpAnimationIndex);
				Set_ForcedGravityTime_LittleUp();
			}

		}


		//���� �ݰ� ���� �� ���ƿ��� ���
		else if (iAttackEvent == 2)
		{
			m_bCounterPose = false;
			Set_Animation(ANIME_IDLE);

		}

	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_CROUCH_LIGHT:
	{

		if (iAttackEvent == 301)
		{
			if (m_b236Special)
			{
				//120�� ������?
				//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 120.f;

				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 180.f;

			}

		}

		else if (iAttackEvent == 0)
		{
			if (m_b236Special == false)
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
				Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



				//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_Impus = { 0.1f * m_iLookDirection,-10.f };

				Desc.fhitCharacter_StunTime = 0.3f;
				Desc.iDamage = 400 * Get_DamageScale();
				Desc.fLifeTime = 0.1f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };//HIT_KNOCK_AWAY_LEFTDOWN //
				Desc.iTeam = m_iPlayerTeam;

				if (m_pEnemy->Get_fHeight() != 0)
					Desc.fAnimationLockTime = 0.25f;
				else
					Desc.fAnimationLockTime = 0.1f;

				Desc.pOwner = this;
				Desc.bOnwerHitNoneStop = true;

				m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, false, 1.f);
				m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}
			else
			{
				//���Ÿ� ������ ��� �տ� �Ķ�����Ʈ�� ����, ���Ÿ� ��������
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };


				//center�� ��� ��ǥ�� �Ұ� �ƴ϶� ��~��� ������ �Ÿ��� �ؾ���

				_vector vEnemyPos = m_pEnemy->Get_vPosition();
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_vector vLength = vEnemyPos - vMyPos;

				Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),XMVectorGetY(vLength),0.f };


				Desc.fhitCharacter_StunTime = 0.4f;
				Desc.iDamage = 700 * Get_DamageScale();;
				Desc.fLifeTime = 0.2f;
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY };
				//Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,8.f };

				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY_DOWN };


				Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };

				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;
				Desc.bOnwerHitNoneStop = true;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

				//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 40.f;
			
				CEffect_Layer* pEffect=	Character_Make_Effect(TEXT("Hit_SDI_U"), { XMVectorGetX(vLength) * m_iLookDirection, XMVectorGetY(vLength) + 0.8f });
				if(m_iLookDirection == -1)
					pEffect->Set_Copy_Layer_Rotation({0.f,0.f,135.f});
				else
					pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,45.f });

			}
		}

		else if (iAttackEvent == 101)
		{
			if (m_b236Special == false)
				AttackNextMoveCheck();
		}

		else if (iAttackEvent == 102)
		{
			if (m_b236Special == false)
				AttackNextMoveCheck();
		}
		else if (iAttackEvent == 103)
		{
			if (m_b236Special == false)
				AttackNextMoveCheck();
		}

		else if (iAttackEvent == 201)
		{
			//if (m_b236Special == false)
			//	Set_Animation(ANIME_IDLE);
			//else
			//{
			//	m_b236Special = false;
			//}

			m_b236Special = false;
			Set_Animation(ANIME_IDLE);

		}

	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_CROUCH_MEDUIM:
	{
		if (iAttackEvent == 0)
		{
			//�ʹ� ũ�� z�� �ȸ���
			//m_pEffect_Manager->Copy_Layer(TEXT("Smoke01_BackZ"), m_pTransformCom->Get_WorldMatrixPtr());
			//Character_Make_Effect(TEXT("Smoke01"), {0.3f,0.f}, true);

			//�ϳ� �߸��� Ƽ��, Ÿ�̹� �̻���
			//Character_Make_Effect(TEXT("Smoke_Run"), { 0.25f,0.f },true);
			// 
			//�޹�ħ���� �����־��µ� ���������� �ű�
			//Character_Make_Effect(TEXT("Smoke03_Stop"),{0.f,0.f},true);


			Character_Make_Effect(TEXT("Smoke03_Stop"), { 0.25f,0.f });

			CAttackObject::ATTACK_DESC Desc{};
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };


			Desc.ColliderDesc.vExtents = { 0.7f,0.5f,1.f };
			Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.3f,0.f };

			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 700 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_CROUCH_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.13f;
			Desc.pOwner = this;

			Desc.eAttackType = { ATTACKTYPE_LOW };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}

	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_CROUCH_HEAVY:
	{
		//������ ��� �ӵ� 60���� ����. 
		//236Ư�� ��� ���� �ӵ� ����
		if (iAttackEvent == 0)
		{
			if (m_b236Special == false)
			{
				//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 60.f;
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 90.f;


				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Light_Attack_Final_Attack, false, 1.f);
			}
			else
			{

			}
		}


		else if (iAttackEvent == 1)
		{
			if (m_b236Special == false)
			{
				CAttackObject::ATTACK_DESC Desc{};
				//Desc.ColliderDesc.width = 2.0;
				//Desc.ColliderDesc.height = 1.3;
				//Desc.ColliderDesc.vCenter = { 0.4f * m_iLookDirection,1.2f,0.f };
				//Desc.ColliderDesc.width = 1.5;
				//Desc.ColliderDesc.height = 1.3;
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 1.5f,1.3f,1.f };
				Desc.ColliderDesc.vCenter = { 0.2f * m_iLookDirection,1.2f,0.f };


				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 10.f };  //���� ���ӵ�.
				Desc.fhitCharacter_Impus = { 0.6f * m_iLookDirection, 10.f };  //���� ���ӵ�.

				//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 3.f };  //���� �׽�Ʈ�� �ӽ�

				Desc.fhitCharacter_StunTime = 1.0f;
				Desc.iDamage = 1000 * Get_DamageScale();;
				Desc.fLifeTime = 0.3f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.5f;
				Desc.pOwner = this;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


				//Character_Make_Effect(TEXT("Ring_Dust"), { m_iLookDirection * 0.7f,0.5f });
				//Character_Make_Effect(TEXT("Ring_Dust"), { m_iLookDirection * 0.4f,0.9f });
				Character_Make_Effect(TEXT("Ring_Dust"), { 0.4f,0.9f });
			}
			else
			{
				//���Ÿ� ������ ��� �տ� �Ķ�����Ʈ�� ����, ���Ÿ� ��������
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };


				//center�� ��� ��ǥ�� �Ұ� �ƴ϶� ��~��� ������ �Ÿ��� �ؾ���

				_vector vEnemyPos = m_pEnemy->Get_vPosition();
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_vector vLength = vEnemyPos - vMyPos;

				Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),XMVectorGetY(vLength),0.f };



				Desc.fhitCharacter_StunTime = 0.4f;
				Desc.iDamage = 700 * Get_DamageScale();;
				Desc.fLifeTime = 0.2f;
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY };
				//Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,8.f };

				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };
				Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,2.f };

				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;
				Desc.bOnwerHitNoneStop = true;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 40.f;

				CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_SDI_U"), { XMVectorGetX(vLength) * m_iLookDirection, XMVectorGetY(vLength) + 0.8f });

				if(m_iLookDirection == 1)
					pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,315.f });
				else
					pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,225.f });

			}
		}

		else if (iAttackEvent == 3)
		{

			m_b236Special = false;

		}


	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_AIR1:
	{

		if (m_bBenishingAttack == false)
		{
			CAttackObject::ATTACK_DESC Desc{};
			//Desc.ColliderDesc.width = 0.7;
			//Desc.ColliderDesc.height = 0.8;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;

			if (Get_fHeight() > 3)  //���߿��� �� ���߱� ������ ����
			{
				Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			}
			else
			{
				Desc.ColliderDesc.vExtents = { 0.4f,0.4f,1.f };
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,1.0f,0.f };
			}

			Desc.fhitCharacter_Impus = { m_fImpuse.x,0 };

			//if(abs(m_fImpuse.x)<0.3 )
			//	Desc.fhitCharacter_Impus = { m_fImpuse.x*m_iLookDirection,0 };
			//else
			//	Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };


			//Desc.fhitCharacter_Impus = { m_fImpuse.x * 0.9f,0 };


			Desc.fhitCharacter_StunTime = 0.6f;
			Desc.iDamage = 400 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.04f;
			//Desc.bOwnerGravityTimeReset = true;
			Desc.pOwner = this;
			Desc.eAttackType = { ATTACKTYPE_MIDDLE };

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		else //��Ͻ� ����
		{
			m_bInvisible = false;
			m_bBenishingAttack = false;
			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 120.f;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 0.f,1.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;

			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };

			Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.0f;
			Desc.iDamage = 1000 * Get_DamageScale();;
			///Desc.iDamage = 30000 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.3f;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.iGainAttackStep = 2;

			Desc.bCameraZoom = false;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.f,1.f }, 0.2f);

		}

	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_AIR2:
	{

		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 0.7;
		//Desc.ColliderDesc.height = 0.8;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;

		Desc.ColliderDesc.pMineGameObject = this;
		if (Get_fHeight() > 3)  //���߿��� �� ���߱� ������ ����
		{
			Desc.ColliderDesc.vExtents = { 0.7f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
		}
		else
		{
			Desc.ColliderDesc.vExtents = { 0.6f,0.5f,1.f };
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,1.0f,0.f };
		}
		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };

		Desc.fhitCharacter_Impus = { m_fImpuse.x  ,0 };

		//if (abs(m_fImpuse.x) < 0.3)
		//	Desc.fhitCharacter_Impus = { m_fImpuse.x * m_iLookDirection,0 };
		//else
		//	Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };


		Desc.fhitCharacter_StunTime = 0.4f;

		if (Get_fHeight() > 2)
			Desc.fhitCharacter_StunTime = 0.6f;


		Desc.iDamage = 700 * Get_DamageScale();
		Desc.fLifeTime = 0.1f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.04f;
		//Desc.bOwnerGravityTimeReset = true;
		Desc.pOwner = this;
		Desc.eAttackType = { ATTACKTYPE_MIDDLE };

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_HIT, false, 1.f);
		m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_AIR3:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 1.0;
		//Desc.ColliderDesc.height = 1.0;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;

		if (Get_fHeight() > 3)  //���߿��� �� ���߱� ������ ����
		{
			Desc.ColliderDesc.vExtents = { 1.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
		}
		else
		{
			Desc.ColliderDesc.vExtents = { 0.4f,0.8f,1.f };
			Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.8f,0.f };
		}
		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
		Desc.fhitCharacter_StunTime = 0.9f;
		Desc.iDamage = 850 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };

		Desc.iTeam = m_iPlayerTeam;


		//���̰� 5 �̻��̸� ���������� ����.  ����Ʈ�� false�̹Ƿ�  else ó�� ����
		//if(Get_fHeight() > 5)
		if (m_bGrab_Air)
		{
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.7f;
			m_bGrab_Air = false;

		}
		else if (Get_fHeight() > 3)
		{
			Desc.bGroundSmash = true;
			Desc.fAnimationLockTime = 0.7f;
		}
		else
		{
			Desc.bCameraZoom = false;
			Desc.fAnimationLockTime = 0.1f;
		}

		Desc.bGrabbedEnd = true;
		Desc.pOwner = this;
		Desc.eAttackType = { ATTACKTYPE_MIDDLE };

		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Heavy_Attack_Hit, false, 1.f);

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_UPPER_AIR:
	{
		CAttackObject::ATTACK_DESC Desc{};
		//Desc.ColliderDesc.width = 1.0;
		//Desc.ColliderDesc.height = 1.0;
		if (m_iPlayerTeam == 1)
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
		else
			Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
		Desc.ColliderDesc.pMineGameObject = this;

		Desc.ColliderDesc.vExtents = { 0.5f,0.7f,1.f };
		Desc.ColliderDesc.vCenter = { 0.7f * m_iLookDirection,0.8f,0.f };

		//Desc.ColliderDesc.pTransform = m_pTransformCom;
		//Desc.fhitCharacter_Impus = { 0.f, 6.f }; //�ʹ� ����
		Desc.fhitCharacter_Impus = { 0.f, 4.f };

		//Desc.fhitCharacter_StunTime = 0.6f;
		Desc.fhitCharacter_StunTime = 0.8f;

		Desc.iDamage = 1000 * Get_DamageScale();;
		Desc.fLifeTime = 0.2f;
		Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP };
		Desc.iTeam = m_iPlayerTeam;
		Desc.fAnimationLockTime = 0.08f;
		Desc.pOwner = this;

		if (Get_fHeight() > 2)
		{
			Desc.bCameraZoom = false;
			//Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			//Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };
			//Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 6.f };
			Desc.fhitCharacter_Impus = { 0.4f * m_iLookDirection, 5.f };

		}
		else
		{
			Desc.fhitCharacter_Impus = { 0.2f * m_iLookDirection, 10.f };  //���� ���ӵ�.
			Desc.bCameraZoom = true;
			Desc.fAnimationLockTime = 0.5f;
		}

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);





		Character_Make_Effect(TEXT("Ring_Dust"), { m_iLookDirection * 1.2f,0.7f });
	}
	break;
	case Client::CPlay_Hit::ANIME_ATTACK_SPECIAL_AIR:
	{
		//�ݰ� �ڼ� ����
		if (iAttackEvent == 0)
		{
			m_bCounterPose = true;
			//Character_Make_Effect(TEXT("Hit_SDO-02"), { 0.f,0.6f });
			Character_Make_Effect(TEXT("Hit_BurstJ-01"), { 0.f,0.4f });

			
		}

		//�ݰݼ���. �� �������
		else if (iAttackEvent == 1)
		{
			m_bCounterPose = false;
			m_bCounterSucces = false;
			m_bInvisible = false;

			//_vector vTargetPos = static_cast<CTransform*>(m_pEnemy->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			//vTargetPos += {1.3f * m_iLookDirection, 0.1f, 0, 0};
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);

			Teleport_ToEnemy( 1.3f,0.1f );

			FlipDirection();

			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 0.f,1.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->StartCameraShake(0.1f, 0.3f);



			//������ ���� 
			Set_Animation(m_iJumpAnimationIndex);
			Set_ForcedGravityTime_LittleUp();


		}


		//�ݰ� ���н� ���ƿ��� ���
		else if (iAttackEvent == 2)
		{
			m_bCounterPose = false;
			Set_ForcedGravityDown();
			Set_Animation(ANIME_JUMP_DOWN);

		}
	}
	break;
	case Client::CPlay_Hit::ANIME_GRAB:
		switch (iAttackEvent)
		{
			//��� ����
		case 0:
		{
			CAttackObject_Grab::ATTACK_Grab_DESC Desc{};
			//Desc.ColliderDesc.width = 1.4f;
			//Desc.ColliderDesc.height = 2.2f;
			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			//Desc.ColliderDesc.vExtents = { 1.4f,2.2f,1.f };
			Desc.ColliderDesc.vExtents = { 1.2f,1.0f,1.f };

			Desc.ColliderDesc.vCenter = { 0.3f * m_iLookDirection,0.7f,0.f };
			//Desc.ColliderDesc.pTransform = m_pTransformCom;
			//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 700 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.pOwner = this;

			if (m_bGrab_Air)
				Desc.eAttackType = ATTACKTYPE_GRAB_AIR;
			else
				Desc.eAttackType = ATTACKTYPE_GRAB_GROUND;

			Desc.fDistance = { 1.2f * m_iLookDirection,0.f };
			//Desc.fGrabAnimationPosition = 40.f;
			Desc.fGrabAnimationPosition = 25.f;

			Desc.iOnwerNextAnimationIndex = m_iGrabAnimationIndex;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Grab"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Grab_Try, false, 1.f);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);

		}
		break;

		//��Ÿ
		case 1:
		case 3:
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };



			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 80 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.pOwner = this;
			Desc.bDrawNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		break;
		case 2:
		case 4:
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 0.3f,0.5f,0.2f };


			Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.f;
			Desc.iDamage = 80 * Get_DamageScale();
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.iGainAttackStep = 0;
			Desc.pOwner = this;
			Desc.bDrawNoneStop = true;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::LIGHT_ATTACK_Hit_SFX, false, 1.f);
		}
		}
		break;
		break;
	case Client::CPlay_Hit::ANIME_SPARKING:
		break;
	case Client::CPlay_Hit::ANIME_REFLECT:
	{
		if (iAttackEvent == 0)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_REFLECT;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_REFLECT;
			Desc.ColliderDesc.pMineGameObject = this;

			Desc.ColliderDesc.vCenter = { 0.f,0.8f,0.f };
			Desc.ColliderDesc.vExtents = { 1.f,1.f,0.2f };


			Desc.fhitCharacter_Impus = { 0,0 };
			Desc.fhitCharacter_StunTime = 0.f;
			Desc.iDamage = 0;
			Desc.fLifeTime = 1.f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_NONE };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			Desc.pOwner = this;
			Desc.iGainHitCount = 0;
			Desc.iGainAttackStep = 0;

			Desc.bReflect = true;
			Desc.iGainKiAmount = 15;

			//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);
			m_pReflectObject = m_pGameInstance->Add_GameObject_ToLayer_AndGet(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Reflect"), TEXT("Layer_AttackObject"), &Desc);


			m_bReflect = true;
		}
		if (iAttackEvent == 1)
		{
			if (m_bAttackBackEvent)
			{
				Set_Animation(CPlay_Hit::ANIME_IDLE);

				//if (m_pReflectObject != nullptr)
				//{
				//	//static_cast<CAttackObject_Reflect*>(m_pReflectObject)->Set_RemoteDestory();
				//	static_cast<CAttackObject_Reflect*>(m_pReflectObject)->Destory();
				//
				//	m_pReflectObject = nullptr;
				//}
			}

			if (m_pReflectObject != nullptr)
			{
				if (m_pReflectObject->m_iGameObjectData != 0)
				{
					m_pReflectObject = nullptr;
				}
				else
					static_cast<CAttackObject_Reflect*>(m_pReflectObject)->Set_RemoteDestory();

				m_pReflectObject = nullptr;
			}

			m_bReflect = false;
		}
	}
	break;
	case Client::CPlay_Hit::ANIME_CROUCH_START:
		break;
	case Client::CPlay_Hit::ANIME_CROUCHING:
		break;
	case Client::CPlay_Hit::ANIME_GUARD_GROUND:
		break;
	case Client::CPlay_Hit::ANIME_GUARD_CROUCH:
		break;
	case Client::CPlay_Hit::ANIME_GUARD_AIR:
		break;
	case Client::CPlay_Hit::ANIME_JUMP_UP:
		break;
	case Client::CPlay_Hit::ANIME_JUMP_DOWN:
		break;
	case Client::CPlay_Hit::ANIME_IDLE:
		break;
	case Client::CPlay_Hit::ANIME_FORWARD_WALK:
		break;
	case Client::CPlay_Hit::ANIME_BACK_WALK:
		break;
	case Client::CPlay_Hit::ANIME_FORWARD_DASH:
		break;
	case Client::CPlay_Hit::ANIME_BACK_DASH:
		break;
	case Client::CPlay_Hit::ANIME_FORWARD_DASH_END:
		break;
	case Client::CPlay_Hit::ANIME_BACK_DASH_END:
		break;
	case Client::CPlay_Hit::ANIME_CHASE:
		break;
	case Client::CPlay_Hit::ANIME_GRAB_READY:
		break;
	case Client::CPlay_Hit::ANIME_HIT_LIGHT:
		break;
	case Client::CPlay_Hit::ANIME_HIT_LIGHT_FRONT:
		break;
	case Client::CPlay_Hit::ANIME_HIT_CROUCH:
		break;
	case Client::CPlay_Hit::ANIE_HIT_AIR:
		break;
	case Client::CPlay_Hit::ANIME_HIT_FALL:
		break;
	case Client::CPlay_Hit::ANIME_HIT_BOUND_DOWN:
		break;
	case Client::CPlay_Hit::ANIME_HIT_HEAVY_AWAY_LEFT:
		break;
	case Client::CPlay_Hit::ANIME_HIT_HEAVY_AWAY_UP:
		break;
	case Client::CPlay_Hit::ANIME_HIT_HEAVY_AWAY_LEFTDOWN:
		break;
	case Client::CPlay_Hit::ANIME_HIT_HEAVY_AWAY_SPIN_UP:
		break;
	case Client::CPlay_Hit::ANIME_HIT_HEAVY_AWAY_SPIN_LEFTUP:
		break;
	case Client::CPlay_Hit::ANIME_HIT_HEAVY_AWAY_SPIN_LEFT:
		break;
	case Client::CPlay_Hit::ANIME_HIT_WALLBOUNCE:
		break;
	case Client::CPlay_Hit::ANIME_DIE_STAND:
		break;
	case Client::CPlay_Hit::ANIME_LAYUP:
		break;
	case Client::CPlay_Hit::ANIME_BREAK_FALL_GROUND:
		break;
	case Client::CPlay_Hit::ANIME_BREAK_FALL_AIR:
		break;
	case Client::CPlay_Hit::ANIME_236_POSE:
	{
		if (iAttackEvent == 0)
		{
			m_b236Posing = true;
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light, false, 1.f);

		}
		else if (iAttackEvent == 1)
		{
			m_b236Posing = false;
		}

	}
	break;
	case Client::CPlay_Hit::ANIME_236_POSE_LIGHT:
	{
		if (iAttackEvent == 0)
		{
			if (m_b236Special == false)
			{
				m_bInvisible = true;
				//Character_Make_Effect(TEXT("Moving_Line_Right"));
				Character_Create_Distortion({ -1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack, false, 1.f);
				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack_SFX, false, 1.f);
			}
			else
			{
				//���Ÿ� ������ ��� ��ŵ ��� �ӵ� �� ������. ���� 61
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 121.f;
			}


		}
		else if (iAttackEvent == 1)
		{
			if (m_b236Special == false)
			{
				m_b236LightSuccess = false;
				m_bInvisible = false;
				MoveToEnemy_Ground(7.f);
				//Character_Make_Effect(TEXT("Moving_Line_Right"));
				Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });
				m_b236Posing = false;

				CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				main_Camera->StartCameraShake(0.1f, 0.5f);
				{
					CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};

					if (m_iPlayerTeam == 1)
						Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
					else
						Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
					Desc.ColliderDesc.pMineGameObject = this;
					Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
					Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

					//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0.5f };
					Desc.fhitCharacter_Impus = { 7.f * m_iLookDirection,0.5f };

					Desc.fhitCharacter_StunTime = 1.0f;
					Desc.iDamage = 0;
					Desc.fLifeTime = 0.05f;
					Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
					Desc.iTeam = m_iPlayerTeam;
					Desc.fAnimationLockTime = 0.7f;

					Desc.fDistance = { 0.8f,0.5f };
					Desc.iGrabAnimationIndex = CPlay_Hit::ANIME_236_POSE_LIGHT;
					Desc.bOwnerNextAnimation = false;

					Desc.iGainAttackStep = 0;
					Desc.iGainHitCount = 0;
					Desc.iGainKiAmount = 0;

					Desc.bGrabbedEnd = true;
					Desc.pOwner = this;

					Desc.fCameraShakeDuration = 0.5f;
					Desc.fCameraShakeMagnitude = 0.2f;

					m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);



				}
				{
					CAttackObject::ATTACK_DESC Desc{};

					if (m_iPlayerTeam == 1)
						Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
					else
						Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
					Desc.ColliderDesc.pMineGameObject = this;
					Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
					Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

					//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
					Desc.fhitCharacter_Impus = { 7.f * m_iLookDirection,0 };

					Desc.fhitCharacter_StunTime = 1.0f;
					Desc.iDamage = 900 * Get_DamageScale();;
					Desc.fLifeTime = 0.05f;
					Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
					Desc.iTeam = m_iPlayerTeam;
					Desc.fAnimationLockTime = 0.7f;
					Desc.bGrabbedEnd = true;
					Desc.pOwner = this;
					Desc.bCameraZoom = false;


					Desc.iCallAttackBackIndex = 1001;

					m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
				}



				//CEffect_Layer* pEffect = Character_Make_BoneEffect("GD_fist_L", TEXT("Hit_SDO-03"));
				m_p236LightEffect = Character_Make_BoneEffect("GD_fist_L", TEXT("Hit_SDI-I"));
				if (m_p236LightEffect != nullptr)
					m_p236LightEffect->Set_Copy_Layer_Scaled({ -0.8f,1.f,1.f });

			}
			else
			{
				//���Ÿ� ������ ��� �տ� �Ķ�����Ʈ�� ����, ���Ÿ� ��������
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };


				//center�� ��� ��ǥ�� �Ұ� �ƴ϶� ��~��� ������ �Ÿ��� �ؾ���

				_vector vEnemyPos = m_pEnemy->Get_vPosition();
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_vector vLength = vEnemyPos - vMyPos;

				Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),XMVectorGetY(vLength),0.f };


				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 1.0f;
				Desc.iDamage = 900 * Get_DamageScale();;
				Desc.fLifeTime = 0.1f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT_NONEBOUNDE };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;
				Desc.bOnwerHitNoneStop = true;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);


				//m_pEnemy->Character_Make_Effect(TEXT("Hit_SDI_U"));
				Character_Make_Effect(TEXT("Hit_SDI_U"), { XMVectorGetX(vLength) * m_iLookDirection, XMVectorGetY(vLength)+0.8f });

			}
		}
		else if (iAttackEvent == 2)
		{
			//����ϰ� ���� �Ұ;���. ��Ģ�� �����ϴ��� Ʋ ����
			//if (m_b236Special == false)
			//{
			//}
			//else
			//{
			//	m_b236Special = false;
			//}

			if (m_b236LightSuccess == false)
			{
				if (m_p236LightEffect != nullptr)
				{
					m_p236LightEffect->m_bIsDoneAnim = true;
					m_p236LightEffect = nullptr;
				}
			}

		}

		//�⺻ ����� ���߽� ����Ʈ
		else if (iAttackEvent == 1001)
		{
			if(m_iLookDirection == 1)
			{
				CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 1.3f,1.6f });
				pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,90.f + 90.f * m_iLookDirection });
				pEffect->Set_Copy_Layer_Scaled({ 0.8f,0.7f,1.f });
			}
			else if (m_iLookDirection == -1)
			{
				CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 0.5f,1.6f });
				pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,90.f + 90.f * m_iLookDirection });
				pEffect->Set_Copy_Layer_Scaled({ 0.8f,0.7f,1.f });
			}
			m_b236LightSuccess = true;
		}
	}
	break;
	case Client::CPlay_Hit::ANIME_236_POSE_MEDIUM:
	{
		//,�Ⱥ��̰� + ������
		if (iAttackEvent == 0)
		{
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f });
		}
		else if (iAttackEvent == 1)
		{
			m_bInvisible = false;
			MoveToEnemy_Ground(7.f);
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });
			m_b236Posing = false;

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Middle_Attack, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Light_Attack_SFX, false, 1.f);

			{
				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
				Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0.5f };
				//Desc.fhitCharacter_StunTime = 1.0f;
				Desc.fhitCharacter_StunTime = 0.3f;

				Desc.iDamage = 0;
				Desc.fLifeTime = 0.1f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.7f;

				Desc.fDistance = { 0.8f,0.5f };
				Desc.iGrabAnimationIndex = CPlay_Hit::ANIME_236_POSE_MEDIUM;
				Desc.bOwnerNextAnimation = false;

				Desc.iGainAttackStep = 0;
				Desc.iGainHitCount = 0;
				Desc.iGainKiAmount = 0;

				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;

				Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_HIT_236_MIDDLE;
				Desc.fCameraShakeDuration = 1.4f;
				Desc.fCameraShakeMagnitude = 0.2f;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);

			}
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
				Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

				Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,-1.f };
				Desc.fhitCharacter_StunTime = 1.0f;
				Desc.iDamage = 1450 * Get_DamageScale();;
				Desc.fLifeTime = 0.1f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT_NONEBOUNDE };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.7f;
				Desc.bGrabbedEnd = true;
				Desc.pOwner = this;
				Desc.bGroundSmash = true;

				Desc.iCallAttackBackIndex = 1001;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}
		}
		//�⺻ ����� ���߽� ����Ʈ
		else if (iAttackEvent == 1001)
		{
			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 1.3f,1.6f });

			//0, 180
			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,90.f + 70.f * m_iLookDirection });
			pEffect->Set_Copy_Layer_Scaled({ 1.0f,0.7f,1.f });


			//135, 45
			//pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f, 90.f+ 45.f*m_iLookDirection });
			

			//60~120
			//90-30, 90+30


		}
	}
	break;
	case Client::CPlay_Hit::ANIME_236_POSE_HEAVY:
	{
		if (iAttackEvent == 0)
		{
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f });

			Character_Make_Effect(TEXT("DIR_K"));
		}
		else if (iAttackEvent == 1)
		{
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Heavy_Attack, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Forward_Light_Heavy_Attack_SFX, false, 1.f);


			m_bInvisible = false;
			MoveToEnemy_Ground(5.f);
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });
			m_b236Posing = false;


			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 0.2f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 0.8f * m_iLookDirection,0.8f,0.f };

			Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,3.f };
			//Desc.fhitCharacter_StunTime = 1.0f;
			Desc.fhitCharacter_StunTime = 0.7f;

			Desc.iDamage = 1500 * Get_DamageScale();;
			Desc.fLifeTime = 0.1f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.7f;
			Desc.pOwner = this;
			Desc.iGainKiAmount = 7;

			Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_HIT_236_HEAVY;
			Desc.fCameraShakeDuration = 0.8f;
			Desc.fCameraShakeMagnitude = 0.3f;
			//CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			//main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_236_HEAVY, 0, this);
			//main_Camera->StartCameraShake(0.8f, 0.3f);

			Desc.iCallAttackBackIndex = 1001;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}
		else if (iAttackEvent == 2)
		{
			if (m_bAttackBackEvent == true)
			{
				//���� 61�̾����� ���߽� �� ������
				//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 181.f;
				//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 211.f;
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 301.f;

			}
		}
		else if (iAttackEvent == 1001)
		{
			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 1.2f,1.6f });

			//���ʺ��� 45  �����ʺ��� 135
			//pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f, 90.f + 45.f * m_iLookDirection });
			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f, 90.f + 30.f * m_iLookDirection });

			//pEffect->Set_Copy_Layer_Scaled({ 0.9f,0.3f,1.f });

			pEffect->Set_Copy_Layer_Scaled({ 1.2f,1.0f,1.f });
		}
	}
	break;


	case Client::CPlay_Hit::ANIME_214_POSE:
	{
		if (iAttackEvent == 0) //Position 20���� �߻�
		{
			//�� 20.f���� ���ߴ� �� ������
			Set_CurrentAnimationPositionJump(20.05f);

			m_b214Posing = true;
			m_i214GlassCount = 0;
			m_fAccPoseTime = 0.f;
			m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 0.f;

			m_pGameInstance->Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY::HIT_Down_Back_Light, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_J_Attack_SFX, false, 1.f);
		}

		//txt�κ��� ȣ���Ұ� �ƴ϶� Update214�κ��� ȣ���Ұ�
		//ó���� ���� �������ѵΰ� ���߿� �߰����� ��
		else if (iAttackEvent == 400)
		{

			//�� ��ġ�κ��� AccTime��ŭ ũ��� ��������׽�Ʈ

			// ���н� �ִϸ��̼� Position 80���� ����, ������ �ӵ� ����.. �����ص� �ӵ��� �ٲ����  
			//m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 61.f;

			//�ݶ��̴� ���� �ǵ��� �ӵ� ����ߵ�
			m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 5.f;

			//�������� ����
			{



				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;


				//_vector vEnemyPos = m_pEnemy->Get_vPosition();
				//_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				//_vector vLength = vEnemyPos - vMyPos;

				//Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),XMVectorGetY(vLength),0.f };
				Desc.ColliderDesc.vCenter = { 5.f * m_iLookDirection,0.f ,0.f };




				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { m_fAccPoseTime * 2.f,0.5f,1.f };

				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 10.f;
				Desc.iDamage = 0;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;


				Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_HIT_214_MIDDLE;
				Desc.fCameraShakeDuration = 1.4f;
				Desc.fCameraShakeMagnitude = 0.15f;

				Desc.eAttackType = ATTACKTYPE_COMMANDGRAB;

				//Desc.fDistance = { XMVectorGetX(vLength),XMVectorGetY(vLength)};

				///_vector vLength = vEnemyPos - vMyPos;
				//Desc.fDistance = { XMVectorGetX(vLength),0.f };

				_float fLength = m_pEnemy->Get_fPositionX() - Get_fPositionX();
				Desc.fDistance = { fLength,0.f };



				//Desc.fGrabAnimationPosition = 40.f;
				//Desc.fGrabAnimationPosition = 25.f;
				Desc.iGainAttackStep = 0;
				Desc.iGainKiAmount = 0;

				Desc.bOwnerNextAnimation = false;

				Desc.iGrabAnimationIndex = ANIME_214_POSE;
				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Down_Backward_Light_Cancle_Attack_SFX, false, 1.f);

				//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_21_GRAB_SPECIAL;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);
			}

		}
		else if (iAttackEvent == 1)
		{
			m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 61.f;

			if (m_bAttackBackEvent == false)
			{
				Set_CurrentAnimationPositionJump(80.f);
			}

		}
		else if (iAttackEvent == 2)
		{
			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { m_fAccPoseTime * 3.f,1.f,1.f };


			//center�� ��� ��ǥ�� �Ұ� �ƴ϶� ��~��� ������ �Ÿ��� �ؾ���

			_vector vEnemyPos = m_pEnemy->Get_vPosition();
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			_vector vLength = vEnemyPos - vMyPos;

			//Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength), 0.f ,0.f };
			Desc.ColliderDesc.vCenter = { 5.f * m_iLookDirection, 0.f ,0.f };



			Desc.fhitCharacter_StunTime = 0.4f;
			Desc.iDamage = 1334 * Get_DamageScale();;
			Desc.fLifeTime = 0.2f;
			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY };
			//Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,8.f };

			//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
			Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };

			//Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
			Desc.fhitCharacter_Impus = { 4.f * m_iLookDirection,2.f };



			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.1f;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;
			Desc.bCameraZoom = false;
			Desc.bOnwerHitNoneStop = true;

			Desc.iCallAttackBackIndex = 1001;

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Ice_Hit_SFX, false, 1.f);

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
		}

		else if (iAttackEvent == 1001)
		{

			m_LaserListRS.push_back({ 0.01f, {0.f, 0.f,45.f}, {1.f,0.1f} });
			m_LaserListRS.push_back({ 0.03f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			m_LaserListRS.push_back({ 0.06f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			m_LaserListRS.push_back({ 0.10f, {0.f, 0.f,135.f}, {1.f,0.1f} });
			m_LaserListRS.push_back({ 0.13f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			m_LaserListRS.push_back({ 0.16f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//m_LaserListRS.push_back({ 0.20f, {0.f, 0.f,90.f}, {1.f,0.1f} });
			//m_LaserListRS.push_back({ 0.23f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//m_LaserListRS.push_back({ 0.26f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });

		}

		//ó���� ���� �������ѵΰ� �߰����� ��

		//Position 95���� Idle�� ������ txt�� ��

	}
	break;
	case Client::CPlay_Hit::ANIME_236_SPECIAL:
		break;
	case Client::CPlay_Hit::ANIME_236_ULTIMATE:
	{
		//,20�ð�����  + ī�޶�
		if (iAttackEvent == 0)
		{
			Set_AnimationStopWithoutMe(1.5f);
			Set_AnimationStop(0.6f);

			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_1_ULTIMATE, 0, this);
			mainCamera->StartCameraShake(1.f, 0.1f);

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1, false, 1.f);
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1_Start, false, 1.f);


			//Character_Make_Effect(TEXT("Hit_SDO-01"));
			m_pUltimateAuraEffect = Character_Make_BoneEffect("G_root",TEXT("Hit_SDO-01"));
			
			//Character_Make_Effect(TEXT("Hit_SDO-02"));
			Character_Make_Effect(TEXT("Hit_SDO-02"),{0.f,0.6f});

			Set_bAura(true);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);

		}


		//,85�Ⱥ��̰� + ������
		else if (iAttackEvent == 1)
		{
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f });


		}


		//,100 �� ������ �̵� + ���̰� + ������ + ��������
		else if (iAttackEvent == 2)
		{

			if (m_pUltimateAuraEffect != nullptr)
			{
				m_pUltimateAuraEffect->m_bIsDoneAnim = true;
				m_pUltimateAuraEffect = nullptr;
			}

			m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 90.f;

			//�߰��ִ� ����
			//if(false)
			//{
			//	MoveToEnemy_Ground(12.f);
			//	m_bInvisible = false;
			//	//Character_Make_Effect(TEXT("Moving_Line_Right"));
			//
			//	CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
			//	if (m_iPlayerTeam == 1)
			//		Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			//	else
			//		Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			//
			//
			//	Desc.ColliderDesc.vCenter = { 0.4f * m_iLookDirection,0.f ,0.f };
			//
			//
			//
			//
			//	Desc.ColliderDesc.pMineGameObject = this;
			//	Desc.ColliderDesc.vExtents = { 2.f,0.5f,1.f };
			//
			//	Desc.fhitCharacter_StunTime = 10.f;
			//	Desc.iDamage = 0;
			//	Desc.fLifeTime = 0.2f;
			//	Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
			//	Desc.iTeam = m_iPlayerTeam;
			//	Desc.fAnimationLockTime = 0.1f;
			//	Desc.pOwner = this;
			//
			//
			//	Desc.eAttackType = ATTACKTYPE_HIGH;
			//
			//
			//
			//	//_float fLength = m_pEnemy->Get_fPositionX() - Get_fPositionX();
			//	//Desc.fDistance = { fLength,0.f };
			//
			//	_float fPos = Get_fPositionX();
			//
			//	if (fPos > 6 && m_iLookDirection == 1)
			//	{
			//		Desc.fDistance = { -3.f,0.f };
			//	}
			//	else if (fPos < -6 && m_iLookDirection == -1)
			//	{
			//		Desc.fDistance = { 3.f,0.f };
			//
			//	}
			//	else
			//		Desc.fDistance = { 0.9f * m_iLookDirection,0.f };
			//
			//	Desc.iGainAttackStep = 0;
			//	Desc.iGainKiAmount = 0;
			//
			//	Desc.bOwnerNextAnimation = false;
			//
			//	Desc.iGrabAnimationIndex = ANIME_236_ULTIMATE;
			//
			//
			//	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);
			//}

			{



				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;



				_vector vEnemyPos = m_pEnemy->Get_vPosition();
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_vector vLength = vEnemyPos - vMyPos;

				Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength),0.4f,0.f };




				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 2.f,1.0f,1.f };

				Desc.fhitCharacter_StunTime = 10.f;
				Desc.iDamage = 0;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 2.f;
				Desc.bOnwerHitNoneStop = true;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;

				Desc.eAttackType = ATTACKTYPE_HIGH;
				Desc.eAttackGrade = GRADE_ULTIMATE;


				//_float fLength = m_pEnemy->Get_fPositionX() - Get_fPositionX();
				//Desc.fDistance = { fLength,0.f };

				_float fPos = Get_fPositionX();

				if (fPos > 6 && m_iLookDirection == 1)
				{
					Desc.fDistance = { -3.f,0.f };
				}
				else if (fPos < -6 && m_iLookDirection == -1)
				{
					Desc.fDistance = { 3.f,0.f };

				}
				else
					Desc.fDistance = { 0.9f * m_iLookDirection,0.f };

				Desc.iGainAttackStep = 0;
				Desc.iGainKiAmount = 0;

				Desc.bOwnerNextAnimation = false;

				Desc.iGrabAnimationIndex = ANIME_236_ULTIMATE;

				Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_HIT_1_ULTIMATE;
				Desc.ianimationIndex = 1;
				Desc.fCameraShakeDuration = 1.f;
				Desc.fCameraShakeMagnitude = 0.2f;




				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);
			}


			Character_Make_Effect(TEXT("Hit_SDO-04"),{0.f,1.3f});
			
			Character_Make_Effect(TEXT("Hit_SDO-05"));
			
		}

		//,110 ���� + �� ������ �̵� + ī�޶� + �߰����� + ����Ʈ,  �ִϸ��̼Ǽӵ� ����, 120���� �޿�

		//���� �����ߴ��� ���� ������ ���� + �� ������ �̵� + ī�޶� +�ִϸ��̼Ǽӵ� ����,
		else if (iAttackEvent == 3)
		{
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_1_Attack, false, 1.f);

			m_bInvisible = false;

			m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 161.f;

			if (m_bAttackBackEvent == true)
			{



				//Add_Move({ 20.f * m_iLookDirection,0.f });
				Teleport_ToEnemy(7.f, 0.f);
				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 10.f;
				Character_Create_Distortion({ 1.f,0.f,0.f }, { -2.f * m_iLookDirection,0.f }, { 1.5f,1.f }, { 0.5f });

				Character_Create_Distortion({ 1.f,0.f,0.f }, { 0.f,0.f }, { 1.5f,1.f }, { 0.5f });

				Character_Make_BoneEffect("GD_fist_R",TEXT("Hit_SDO-03"));


				Character_Make_Effect(TEXT("Hit_SDO-05"),{-4.f,0.f});
				


				m_LaserListRS.push_back({ 0.70f, {0.f, 0.f,45.f}, {1.f,0.1f} });
				m_LaserListRS.push_back({ 0.73f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
				m_LaserListRS.push_back({ 0.76f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });


				m_LaserListRS.push_back({ 0.80f, {0.f, 0.f,135.f}, {1.f,0.1f} });
				m_LaserListRS.push_back({ 0.83f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
				m_LaserListRS.push_back({ 0.76f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });

				m_LaserListRS.push_back({ 0.90f, {0.f, 0.f,90.f}, {1.f,0.1f} });
				m_LaserListRS.push_back({ 0.93f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
				m_LaserListRS.push_back({ 0.96f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });


			}
			else
			{
				MoveToEnemy_Ground(12.f);
				Character_Create_Distortion({ 1.f,0.f,0.f });
				m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

			}
		}

		//,120  �ӵ�1 ?  ��ſ� �߰����� ����
		else if (iAttackEvent == 4)
		{
			if (m_bAttackBackEvent)
			{
				CAttackObject::ATTACK_DESC Desc{};

				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 20.f,3.f,1.f };


				//center�� ��� ��ǥ�� �Ұ� �ƴ϶� ��~��� ������ �Ÿ��� �ؾ���

				_vector vEnemyPos = m_pEnemy->Get_vPosition();
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_vector vLength = vEnemyPos - vMyPos;

				//Desc.ColliderDesc.vCenter = { XMVectorGetX(vLength), 0.f ,0.f };
				Desc.ColliderDesc.vCenter = { -5.f * m_iLookDirection, 1.f ,0.f };



				Desc.fhitCharacter_StunTime = 0.4f;
				//Desc.iDamage = 1334 * Get_DamageScale();;
				Desc.iDamage = 2200 * Get_DamageScale(true);;

				Desc.fLifeTime = 0.2f;
				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_UP_GRAVITY };
				//Desc.fhitCharacter_Impus = { 0.5f * m_iLookDirection,8.f };

				//Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFTDOWN };
				Desc.ihitCharacter_Motion = { HitMotion::HIT_SPIN_AWAY_LEFTUP };

				//Desc.fhitCharacter_Impus = { 3.f * m_iLookDirection,-20.f };
				Desc.fhitCharacter_Impus = { 1.f * m_iLookDirection,2.f };


				Desc.iGainHitCount = 16;
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;
				Desc.bCameraZoom = false;
				Desc.bOnwerHitNoneStop = true;
				Desc.bGrabbedEnd = true;
				Desc.iGainKiAmount = 0;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);



			}
			else
			{
				Set_CurrentAnimationPositionJump(159.9f);

				m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 91.f;
			}
		}

		//123 �ӵ� ����ȭ
		else if (iAttackEvent == 5)
		{

			m_pModelCom->m_Animations[m_pModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 91.f;
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}

		//155 ���������� Idle��
		else if (iAttackEvent == 6)
		{
			if (m_bAttackBackEvent == true)
			{
				Set_Animation(ANIME_IDLE);
				Set_bAura(false);
				m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

			}
		}


		else if (iAttackEvent == 7)
		{
			//if (m_bAttackBackEvent == false)
			//{
			//	Set_Animation(ANIME_IDLE);
			//}
		}
		else if (iAttackEvent == 8)
		{
			if (m_bAttackBackEvent == false)
			{
				Set_Animation(ANIME_IDLE);
				Set_bAura(false);
				m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

			}
		}

	}
	break;
	case Client::CPlay_Hit::ANIME_214_FINAL_START:
	{
		//,��븸 �ð�����
		if (iAttackEvent == 0)
		{
			Set_AnimationStopWithoutMe(1.5f);

			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_3_ULTIMATE, 0, this);
			//main_Camera->StartCameraShake(1.1f, 0.05f);

			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_0, false, 1.f);


			if (m_pEnemy->Get_bStun() == true || m_pEnemy->Check_bCurAnimationisGroundSmash())
			{
				Character_Start_QTE(CQTE_Manager::QTE_ID_1P_SAME_GRAB);
			}

			Set_bAura(true);
			m_iFinalInvisibleCount = 0;
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(true);

		}

		//,�Ⱥ��̰�,�ӵ�������, ������
		else if (iAttackEvent == 1)
		{
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f });
			//,0�ϋ� �ӵ��� 61
			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 181.f;

		}
		//���Ⱑ ��� �ϳ��� �ִϸ��̼�
		else if (iAttackEvent == 2)
		{
			MoveToEnemy_Ground(12.f);

			m_bInvisible = false;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 61.f;

			{
				CAttackObject_CommandGrab::ATTACK_COMMANDGRAB_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.8f,0.6f,1.f };
				Desc.ColliderDesc.vCenter = { 0.3f,0.7f,0.f };
				Desc.fhitCharacter_StunTime = 20.f;
				Desc.iDamage = 500 * Get_DamageScale(true);;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_LIGHT };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;

				Desc.eAttackType = ATTACKTYPE_HIGH;
				Desc.eAttackGrade = GRADE_ULTIMATE;

				Desc.fDistance = { 0.8f * m_iLookDirection,0.f };
				Desc.fForcedGravityTime = 0.f;
				Desc.iGainAttackStep = 2;
				Desc.iGrabAnimationIndex = ANIME_214_FINAL_START;
				Desc.iOnwerNextAnimationIndex = ANIME_214_FINAL_SUCESS;


				//Desc.iVirtualCameraindex = CMain_Camera::VIRTUAL_CAMERA_SON_ULTIMATE;
				Desc.ianimationIndex = 1;
				Desc.fCameraShakeDuration = 0.5f;
				Desc.fCameraShakeMagnitude = 0.2f;


				Desc.fForcedGravityTime = 0.f;

				Desc.bOwnerNextAnimation = true;

				Desc.iCallAttackBackIndex = 1001;  //SUCESS��  1001�� ��

				m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::Hit_Ultimate_3_1, false, 1.f);

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_CommandGrab"), TEXT("Layer_AttackObject"), &Desc);
			}

		}
		
		else if (iAttackEvent == 3)
		{
			if (m_bAttackBackEvent == false)
			{
				Set_Animation(ANIME_IDLE);
				Set_bAura(false);

				if (m_bCreateQTE)
				{
					Character_Start_QTE(CQTE_Manager::QTE_ID_1P_SAME_GRAB);
					m_bCreateQTE = false;
				}
				m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

			}


		}


		//������ ��ġ
		else if (iAttackEvent == 4)
		{
			if (m_iQTE == 1) // 1
			{
				m_pAttackFinalGlassEffect_Layer = Character_Make_Effect(TEXT("Hit_SAO-02"), { 1.f,-0.3f });
				m_pAttackFinalGlassEffect_Layer->Set_Copy_Layer_Scaled({ 2.f,2.f,1.f });

				//(*m_pAttackFinalGlassEffect_Layer->m_MixtureEffects.begin())->m_iRenderGroupIndex = static_cast<_int>(CRenderer::RG_BACKSIDE_EFFECT);
				(*m_pAttackFinalGlassEffect_Layer->m_MixtureEffects.begin())->m_iChangePassIndex = 11;
				(*m_pAttackFinalGlassEffect_Layer->m_MixtureEffects.begin())->m_vColor = _float4(255.f, 0.f, 0.f, 10.f);


				CEffect_Layer* pAttackFinalGlassEffect_Layer = Character_Make_Effect(TEXT("Hit_SAO-03"), { 1.f,1.f });
				pAttackFinalGlassEffect_Layer->Set_Copy_Layer_Scaled({ 2.f,2.f,1.f });
				(*pAttackFinalGlassEffect_Layer->m_MixtureEffects.begin())->m_vColor = _float4(255.f, 0.f, 0.f, 100.f);
			}
			else
			{

				//CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_SAO-02"),{1.f,-0.3f});
				//pEffect->Set_Copy_Layer_Scaled({ 2.f,2.f,1.f });


				CEffect_Layer* pAttackFinalGlassEffect_Layer = Character_Make_Effect(TEXT("Hit_SAO-03"), { 1.f,1.f });

				m_pAttackFinalGlassEffect_Layer = Character_Make_Effect(TEXT("Hit_SAO-02"), { 1.f,-0.3f });
				m_pAttackFinalGlassEffect_Layer->Set_Copy_Layer_Scaled({ 2.f,2.f,1.f });

				//(*m_pAttackFinalGlassEffect_Layer->m_MixtureEffects.begin())->m_iChangePassIndex = 11;
				//(*m_pAttackFinalGlassEffect_Layer->m_MixtureEffects.begin())->m_vColor = _float4(255.f, 0.f, 0.f, 10.f);


			}
			cout << "Event4 , Position : " << m_pModelCom->m_fCurrentAnimPosition << endl;

			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 20.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };

			Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.0f;
			Desc.iDamage = 1000 * Get_DamageScale(true);;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 2.f;
			//Desc.bGrabbedEnd = true;
			Desc.bOnwerHitNoneStop = true;
			Desc.pOwner = this;

			Desc.fCameraShakeDuration = 0.2f;
			Desc.fCameraShakeMagnitude = 0.2f;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			//m_pModelCom->Get_pCurrentAnimation()->m_fCurrentPosition = 91.f;
			//m_pModelCom->Get_pCurrentAnimation()->m_fCurrentPosition = 281.f;
			
		}

		else if (iAttackEvent == 255)
		{
			//������,�Ⱥ��̰�, Position290 (�ڵ���) �������� �̵�?
			m_bInvisible = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f });
			Set_CurrentAnimationPositionJump(285.f);
		}
		else if (iAttackEvent == 290)
		{
			//������,���̰�,
			m_bInvisible = false;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f });

			//m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 30.f;
			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 20.f;



		}
		else if (iAttackEvent == 5)
		{
			if(m_pAttackFinalGlassEffect_Layer!= nullptr)
			{
				m_pAttackFinalGlassEffect_Layer->m_bIsDoneAnim = true;
				m_pAttackFinalGlassEffect_Layer = nullptr;
			}
			CMap_Manager::Get_Instance()->All_Black(false);

			CEffect_Layer* pEffectLayerGlassBreak = Character_Make_Effect(TEXT("Hit_SAO-04"), { 1.f,0.f });
			
			if(m_iQTE == 1)
			{
				for (auto& iter : pEffectLayerGlassBreak->m_MixtureEffects)
				{
					iter->m_iChangePassIndex = 11;
					iter->m_vColor = _float4(255.f, 0.f, 0.f, 100.f);
				}
			}

			//(*pEffectLayerGlass->m_MixtureEffects.begin())->m_iChangePassIndex = 11;
			//(*pEffectLayerGlass->m_MixtureEffects.begin())->m_vColor = _float4(255.f, 0.f, 0.f, 10.f);

			//CEffect_Layer* pEffectBreak = Character_Make_Effect(TEXT("Hit_SAO-04"), { 1.f,0.f });
			//pEffectBreak->Set_Copy_Layer_Scaled({ 2.f,2.f,2.f });

			CAttackObject::ATTACK_DESC Desc{};

			if (m_iPlayerTeam == 1)
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
			else
				Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
			Desc.ColliderDesc.pMineGameObject = this;
			Desc.ColliderDesc.vExtents = { 20.f,1.f,1.f };
			Desc.ColliderDesc.vCenter = { 1.0f * m_iLookDirection,0.8f,0.f };

			Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0 };
			Desc.fhitCharacter_StunTime = 1.0f;
			Desc.iDamage = 1000 * Get_DamageScale(true);;
			Desc.fLifeTime = 0.2f;
			Desc.ihitCharacter_Motion = { HitMotion::HIT_KNOCK_AWAY_LEFT };
			Desc.iTeam = m_iPlayerTeam;
			Desc.fAnimationLockTime = 0.f;
			//Desc.bOwnerNextAnimation = true;
			Desc.bGrabbedEnd = true;
			Desc.pOwner = this;

			Desc.bCameraZoom = false;

			Desc.fCameraShakeDuration = 0.2f;
			Desc.fCameraShakeMagnitude = 0.2f;

			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

			if(m_bSparking == false)
				Set_bAura(false);

		}
		else if (iAttackEvent == 6)
		{

			Set_bAura(false);

			Set_Animation(ANIME_IDLE);
			m_pRenderInstance->Get_Instance()->Switch_BlackOut(false);

		}
	}
	break;
	case Client::CPlay_Hit::ANIME_214_FINAL_SUCESS:
	{

		if (iAttackEvent == 1001)
		{

			CEffect_Layer* pEffect = Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 1.3f,1.2f });
			pEffect->Set_Copy_Layer_Rotation({ 0.f,0.f,90.f + 90.f * m_iLookDirection });
			//pEffect->Set_Copy_Layer_Scaled({ 1.f,0.9f,1.f });


			//�� �̰� ������ ���� ����
			m_LaserListRS.push_back({ 0.90f , {0.f, 0.f,300.f}	, {1.f,1.f} });
			m_LaserListRS.push_back({ 1.63f , {30.f, 0.f,90.f + 70.f * m_iLookDirection}	, {0.7f,0.3f} });



		}

		//Position0.1
		else if (iAttackEvent == 100)
		{
			m_bInvisible = true;
			m_bDynamicMove = true;
			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ -1.f,0.f,0.f });

			//ù��°��
			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_3_ULTIMATE, 1, this);
			main_Camera->StartCameraShake(0.3f, 0.2f);
		}

		//Position 48
		else if (iAttackEvent == 0)
		{
			m_bInvisible = false;

			//Character_Make_Effect(TEXT("Moving_Line_Right"));
			Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

			Set_CurrentAnimationPositionJump(48.01f);


			{
				CAttackObject::ATTACK_DESC Desc{};
				//Desc.ColliderDesc.width = 0.7;
				//Desc.ColliderDesc.height = 0.8;
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 10.f,0.2f,1.f };
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.9f,0.f };
				Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 0.4f;
				//Desc.fhitCharacter_StunTime = 0.55f;

				Desc.iDamage = 90 * Get_DamageScale(true);;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;
				Desc.iGainKiAmount = 0;
				Desc.iGainAttackStep = 0;
				Desc.fCameraShakeDuration = 0.3f;
				Desc.fCameraShakeMagnitude = 0.2f;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}
		}
		//Position 48.1
		else if (iAttackEvent == 101)
		{
			//���� ���� ����
			if (isNearlyEqual(m_pModelCom->m_fCurrentAnimPosition, 48))
			{
				return;
			}

			else
			{
				m_bInvisible = true;
				//Character_Make_Effect(TEXT("Moving_Line_Right"));
				Character_Create_Distortion({ -1.f,0.f,0.f });
				//cout << "Event 101 " << endl;
			}


		}
		//Position 77
		else if (iAttackEvent == 1)
		{
			m_bInvisible = false;
			{
				CAttackObject::ATTACK_DESC Desc{};
				//Desc.ColliderDesc.width = 0.7;
				//Desc.ColliderDesc.height = 0.8;
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 10.f,0.2f,1.f };
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.9f,0.f };
				Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 0.4f;
				//Desc.fhitCharacter_StunTime = 0.55f;

				Desc.iDamage = 90 * Get_DamageScale(true);;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;
				Desc.iGainKiAmount = 0;
				Desc.iGainAttackStep = 0;
				Desc.fCameraShakeDuration = 0.3f;
				Desc.fCameraShakeMagnitude = 0.2f;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);

				//�ι�°�� 
			}
		}
		//Position 77.2
		else if (iAttackEvent == 102)
		{
			//���� ���� ����
			if (isNearlyEqual(m_pModelCom->m_fCurrentAnimPosition, 77))
			{
				return;
			}

			else
			{
				//m_bInvisible = true;
				//���� �Ⱥ��̰� �ؾ� �ߴµ� �ٲٷ�����

				//cout << XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << endl;
				//cout << "Event 102 " << endl;
				m_pTransformCom->Add_Move({ 1.f,0.f,-1.5f });
				//cout << XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << endl;

				//Character_Make_Effect(TEXT("Moving_Line_Right"));
				Character_Create_Distortion({ 1.f,0.f,0.f });

			}


		}
		//Position 96 
		else if (iAttackEvent == 2)
		{
			m_bInvisible = false;

			//�� ����


			for (int i = 0; i < 50; i++)
			{
				m_LaserListRS.push_back({ 0.70f + i*0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
				m_LaserListRS.push_back({ 0.73f + i*0.1f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
				m_LaserListRS.push_back({ 0.76f + i*0.1f, {0.f, 0.f, (_float)(rand() % 361)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			
				//m_LaserListRS.push_back({ 0.70f + i * 0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
				//m_LaserListRS.push_back({ 0.75f + i * 0.1f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			
			
			}

		}

		//Position 100
		else if (iAttackEvent == 3)
		{
			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_3_ULTIMATE, 2, this, m_pEnemy);
			// ���� ȸ�� ���� (�ð� ����, ���� �ð� 10��, ���� ���� �ӵ�)
			main_Camera->Set_CirclePlay(true, XM_2PI / 15.f);

			//m_pModelCom->Get_pCurrentAnimation()->m_fDuration = 0.20f;


			//�ʹ� ������
			//m_pModelCom->Set_MaxAnimationUpdate_Time(0.05f);
			//m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 20.00f;

			//�ʹ� ������
			//m_pModelCom->Set_MaxAnimationUpdate_Time(0.01f);
			//m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 100.00f;


			//Set_CurrentAnimationPositionJump(105.f);
			//m_pModelCom->Set_MaxAnimationUpdate_Time(0.02f);
			//m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 50.00f;

			Set_CurrentAnimationPositionJump(135.f);
			Add_Move({ 1.f * m_iLookDirection,0.f });
			m_pModelCom->Set_MaxAnimationUpdate_Time(0.04f);
			m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 25.00f;
			//m_pModelCom->Get_pCurrentAnimation()->m_fTickPerSecond = 40.00f;


			m_bInvisible = true;
			//for (int i = 0; i < 5; i++)
			//{
			//	m_LaserListRS.push_back({ 0.50f + i*0.2f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
			//	m_LaserListRS.push_back({ 0.53f + i*0.2f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//	m_LaserListRS.push_back({ 0.56f + i*0.2f, {0.f, 0.f, (_float)(rand() % 361)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//
			//}

		}

		//Position 148
		else if (iAttackEvent == 201)
		{
			//cout << XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << endl;
			m_pTransformCom->Add_Move({ -1.f,0.f,1.f });
			//cout << "Event 201 " << endl;
			//cout << XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) << endl;


			m_bInvisible = false;
			{
				CAttackObject::ATTACK_DESC Desc{};
				//Desc.ColliderDesc.width = 0.7;
				//Desc.ColliderDesc.height = 0.8;
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 10.f,0.2f,1.f };
				Desc.ColliderDesc.vCenter = { 0.9f * m_iLookDirection,0.9f,0.f };
				Desc.fhitCharacter_Impus = { 0.3f * m_iLookDirection,0 };
				Desc.fhitCharacter_StunTime = 0.4f;
				//Desc.fhitCharacter_StunTime = 0.55f;

				Desc.iDamage = 90 * Get_DamageScale(true);;
				Desc.fLifeTime = 0.2f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_MEDIUM };
				Desc.iTeam = m_iPlayerTeam;
				Desc.fAnimationLockTime = 0.1f;
				Desc.pOwner = this;
				Desc.iGainKiAmount = 0;
				Desc.iGainAttackStep = 0;

				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack"), TEXT("Layer_AttackObject"), &Desc);
			}


			Add_Move({ -3.f * m_iLookDirection,0.f });
			{
				CAttackObject_Energy::ATTACK_RANGED_DESC Desc{};
				if (m_iPlayerTeam == 1)
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_1P_Energy_Attack;
				else
					Desc.ColliderDesc.colliderGroup = CCollider_Manager::COLLIDERGROUP::CG_2P_Energy_Attack;
				Desc.ColliderDesc.pMineGameObject = this;
				Desc.ColliderDesc.vExtents = { 0.f,0.f,1.f };
				Desc.ColliderDesc.vCenter = { -10.f * m_iLookDirection,1.2f,0.f };
				//Desc.ColliderDesc.pTransform = m_pTransformCom;
				//Desc.fhitCharacter_Impus = { 20.f * m_iLookDirection,0.1f };
				Desc.fhitCharacter_Impus = {  };

				Desc.fhitCharacter_StunTime = 10.f;
				//Desc.iDamage = 120 * Get_DamageScale();

				//if(m_bFinalSkillss3)

				Desc.fLifeTime = 15.f;
				Desc.ihitCharacter_Motion = { HitMotion::HIT_HEAVY };
				Desc.iTeam = m_iPlayerTeam;
				//Desc.bGroundSmash = true;
				Desc.fAnimationLockTime = 0.0f;
				Desc.pOwner = this;
				Desc.eAttackGrade = { GRADE_ULTIMATE };
				Desc.bCameraZoom = false;

				Desc.fStartOffset = { -2.f * m_iLookDirection,0.f };
				//�ִϸ��̼� �ӵ� 50
				//Desc.fMoveSpeedNoneDirection = { 5.f,0.f };

				//�ִϸ��̼� �ӵ� 100
				Desc.fMoveSpeedNoneDirection = { 10.f,0.f };

				//�ӵ� 0.02
				//Desc.iDamage = 100 * Get_DamageScale(true);
				//Desc.iAttackCount = 50;

				Desc.iDamage = 50 * Get_DamageScale(true);
				Desc.iAttackCount = 100;


				Desc.iPlayerDirection = m_iLookDirection;		//
				Desc.iGainHitCount = 2;
				Desc.iGainAttackStep = 0;
				//Desc.fAttackDelayTime = 0.04f;  //�� ���� Ǯ��Ʈ ����
				//Desc.fAttackDelayTime = 0.03f; //���� Ǯ��Ʈ ����
				Desc.fAttackDelayTime = 0.02f; //QTE ������������ ����?

				Desc.eEnergyColor = CAttackObject_Energy::ENERGY_LIGHT_BLUE;
				Desc.iGainKiAmount = 0;

				Desc.fColliderfCY = 1.2f;
				Desc.fAnimationLockTime = 1.f;
				Desc.bOnwerHitNoneStop = true;

				//Desc.bGrabbedEnd = true;
				Desc.bCameraZoom = false;



				Desc.fCameraShakeDuration = 0.05f;
				Desc.fCameraShakeMagnitude = 0.03f;



				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Attack_Energy"), TEXT("Layer_AttackObject"), &Desc);

			}
			Add_Move({ 3.f * m_iLookDirection,0.f });

			//for (int i = 0; i < 5; i++)
			//{
			//	m_LaserListRS.push_back({ 0.50f + i * 0.2f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
			//	m_LaserListRS.push_back({ 0.53f + i * 0.2f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//	m_LaserListRS.push_back({ 0.56f + i * 0.2f, {0.f, 0.f, (_float)(rand() % 361)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//
			//}
		}
		//Position 148.2
		else if (iAttackEvent == 202)
		{
			//���� ���� ����
			if (isNearlyEqual(m_pModelCom->m_fCurrentAnimPosition, 148))
			{
				return;
			}

			else
			{
				m_bInvisible = true;
				//Character_Make_Effect(TEXT("Moving_Line_Right"));
				Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });
			}


		}
		//162 ~ 255�� Update�� �ذ�

		//255 Ŭ����� ,  �����ִϸ��̼� ����, START�� 240���� �̵�
		else if (iAttackEvent == 4)
		{
		
			
			CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			main_Camera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_3_ULTIMATE, 3, this);
			//main_Camera->StartCameraShake(0.f, 0.f);
			m_bInvisible = false;
			m_bDynamicMove = false;

			Set_NextAnimation(ANIME_214_FINAL_START, 100.f, 240.f);

			CMap_Manager::Get_Instance()->All_Black(true);
		}
	}
	break;
	case ANIME_NEWROUND_RIGHTHAND_APEEAR_CUTSCENE:
	{
		if (iAttackEvent == 2001)
		{
			Character_Make_Effect(TEXT("Start_Battle-01"), { -0.7f*m_iLookDirection,0.f });
		}
		else if (iAttackEvent == 2002)
		{
			CEffect_Layer::COPY_DESC pDesc{};

			_float4x4 fCamMat = {};

			XMStoreFloat4((_float4*)&fCamMat.m[3][0], m_pGameInstance->Get_CamPosition_Vector());

			pDesc.pPlayertMatrix = &fCamMat;

			CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Start_Battle-04"), &pDesc);


			m_pEnemy->Set_AnimationStop(0.5f);
			Set_AnimationStop(0.5f);

		}
		else if (iAttackEvent == 2003)
		{
			Character_Make_Effect(TEXT("Start_Battle-02"), { -0.7f * m_iLookDirection,0.f });



			//Set_AnimationStop(1.5f);
			//m_pEnemy->Set_AnimationStop(1.5f);

		}
		else if (iAttackEvent == 2004)
		{
			CEffect_Layer::COPY_DESC pDesc{};

			_float4x4 fCamMat = {};

			XMStoreFloat4((_float4*)&fCamMat.m[3][0], m_pGameInstance->Get_CamPosition_Vector());

			pDesc.pPlayertMatrix = &fCamMat;

			CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Start_Battle-03"), &pDesc);
			CUI_Manager::Get_Instance()->CutSceneUI(true);

		}


	}
	break;
	case ANIME_START_DEFAULT:
	{
		if (iAttackEvent == 0)
		{
			Add_Move({ 0.f,0.3f });

			m_pOpeningGlass = Character_Make_BoneEffect("G_root", TEXT("Hit_Opening"));
			m_pOpeningGlass->m_fTickPerSecond *= 0.7f;

		}
		else if (iAttackEvent == 3)
		{
			//m_pOpeningGlass->Set_Copy_Layer_Scaled({ 2.f,2.f,2.f });

			//Add_Move({ 0.f,1.f });
		}
		else if (iAttackEvent == 1)
		{
			//CEffect_Layer* pEffect = Character_Make_BoneEffect("G_root", TEXT("Hit_Opening"));
			//pEffect->m_fTickPerSecond *= 0.5f;
			//m_pOpeningGlass->m_

			m_bInvisible = true;
			Add_Move({ 0.f,-0.3f });

		}
		else if (iAttackEvent == 2)
		{
			m_bInvisible = false;
			Character_Create_Distortion({ 1.f,0.f,0.f }, { 1.f*m_iLookDirection,0.f },{2.f,1.f},0.2f);
		}
		else if (iAttackEvent == 230)
		{
			Set_AnimationStop(1.f);
		}
	}
	break;
	case ANIME_WIN_DEFAULT:
	{
		if (iAttackEvent == 0)
		{
			CMain_Camera* mainCamera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
			mainCamera->Play(CMain_Camera::VIRTUAL_CAMERA_HIT_WIN, 0, this, nullptr, false);
		}

		//�ڵ��� �� ����
		if (iAttackEvent == 220)
		{
			Set_AnimationStop(1000.f);
			CUI_Manager::Get_Instance()->WinUI(LEVEL_GAMEPLAY);
		}

	}
	break;
	default:
		break;
	}

	/*





	*/
}

void CPlay_Hit::Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)
{
	m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName, bisLoof, fvolume);
}

void CPlay_Hit::Play_Group_Sound(_uint groupKey, _bool loop, _float volume)
{
	m_pGameInstance->Play_Group_Sound((CSound_Manager::SOUND_GROUP_KEY)groupKey, loop, volume);
}

void CPlay_Hit::Play_Sound_Stop(_uint SoundName)
{
	m_pGameInstance->Stop_Sound((CSound_Manager::SOUND_KEY_NAME)SoundName);
}

void CPlay_Hit::Reset_PoseTime()
{
	m_fAccPoseTime = 0.f;
}


void CPlay_Hit::Add_YellowLight()
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
	LightDesc.fLifeTime = 1.f;
	LightDesc.strName = "Explosion";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}

void CPlay_Hit::Add_BlueLight()
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
	LightDesc.fLifeTime = 2.7f;
	LightDesc.strName = "Ray";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}


void CPlay_Hit::Update_214FinalInvisible()
{
	if (m_pModelCom->m_iCurrentAnimationIndex != CPlay_Hit::ANIME_214_FINAL_SUCESS)
		return;


	_float fCurrentAnimationPosition = m_pModelCom->m_fCurrentAnimPosition;

	if (fCurrentAnimationPosition < 150 || fCurrentAnimationPosition > 254.9)
		return;



	//if (fCurrentAnimationPosition == 164 || fCurrentAnimationPosition == 172 || fCurrentAnimationPosition == 178 ||
	//	fCurrentAnimationPosition == 183 || fCurrentAnimationPosition == 196 || fCurrentAnimationPosition == 202)
	//{
	//	m_bInvisible = false;
	//}

	if (isNearlyEqual(fCurrentAnimationPosition, 155) || isNearlyEqual(fCurrentAnimationPosition, 164) || isNearlyEqual(fCurrentAnimationPosition, 172) || isNearlyEqual(fCurrentAnimationPosition, 178) ||
		isNearlyEqual(fCurrentAnimationPosition, 183) || isNearlyEqual(fCurrentAnimationPosition, 196) || isNearlyEqual(fCurrentAnimationPosition, 202)
		|| isNearlyEqual(fCurrentAnimationPosition, 208) || isNearlyEqual(fCurrentAnimationPosition, 217) || isNearlyEqual(fCurrentAnimationPosition, 225) || isNearlyEqual(fCurrentAnimationPosition, 148))
	{
		m_bInvisible = false;
	}

	else
	{
		m_bInvisible = true;
	}


}

void CPlay_Hit::Update_214FinalInvisible(_float fTimeDelta)
{
	if (m_pModelCom->m_iCurrentAnimationIndex != CPlay_Hit::ANIME_214_FINAL_SUCESS)
		return;


	_float fCurrentAnimationPosition = m_pModelCom->m_fCurrentAnimPosition;

	if (fCurrentAnimationPosition < 150 || fCurrentAnimationPosition > 254.9)
		return;


	m_fAccFinalTime += fTimeDelta;

	//if (m_fAccFinalTime > 0.02f)
	if (m_fAccFinalTime > 0.1f)
	{
		m_fAccFinalTime = 0.f;

		//-2~+2,  0~1.5

		Character_Create_Distortion({ 1.f,0.f,0.f }, { (rand() % 600 - 300) * 0.01f , rand() % 150 * 0.01f, (rand() % 200 - 100) * 0.01f }, { 1.5f,1.5f }, 0.2f);

		
		//CEffect_Layer* pDustEffect = Character_Make_Effect(TEXT("Hit_SAO-01"));
		//pDustEffect->Set_Copy_Layer_Position({ (rand() % 600 - 300) * 0.01f , rand() % 150 * 0.01f, (rand() % 200 - 100) * 0.01f });


		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 fPos;
		XMStoreFloat3(&fPos, vPos);
		_matrix ovelapMatrix{};
	
		//if (fCurrentAnimationPosition <200)
		//	ovelapMatrix = XMMatrixScaling((_float)Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x + ((rand() % 600 - 300) * 0.01f * Get_iDirection()), fPos.y, fPos.z + (rand() % 200 - 100) * 0.01f);
		//else
		//	ovelapMatrix = XMMatrixScaling((_float)Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x + ((rand() % 600 ) * 0.01f * Get_iDirection()), fPos.y, fPos.z + (rand() % 200 - 100) * 0.01f);

		if (fCurrentAnimationPosition < 200)
			ovelapMatrix = XMMatrixScaling(-1.f + rand() % 2 * 2.f, 1.f, 1.f) * XMMatrixTranslation(fPos.x + ((rand() % 600 - 300) * 0.01f * Get_iDirection()), fPos.y, fPos.z + (rand() % 200 - 100) * 0.01f);
		else
			ovelapMatrix = XMMatrixScaling(-1.f + rand() % 2 * 2.f, 1.f, 1.f) * XMMatrixTranslation(fPos.x + ((rand() % 600) * 0.01f * Get_iDirection()), fPos.y, fPos.z + (rand() % 200 - 100) * 0.01f);


		CEffect_Layer::COPY_DESC tDesc{};
		XMFLOAT4X4 Result4x4;
		XMStoreFloat4x4(&Result4x4, ovelapMatrix);
		tDesc.pPlayertMatrix = &Result4x4;
		CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Hit_SAO-01"), &tDesc);

		
			
	}


	if (isNearlyEqual(fCurrentAnimationPosition, 155) || isNearlyEqual(fCurrentAnimationPosition, 164) || isNearlyEqual(fCurrentAnimationPosition, 172) || isNearlyEqual(fCurrentAnimationPosition, 178) ||
		isNearlyEqual(fCurrentAnimationPosition, 183) || isNearlyEqual(fCurrentAnimationPosition, 196) || isNearlyEqual(fCurrentAnimationPosition, 202)
		|| isNearlyEqual(fCurrentAnimationPosition, 208) || isNearlyEqual(fCurrentAnimationPosition, 217) || isNearlyEqual(fCurrentAnimationPosition, 225) || isNearlyEqual(fCurrentAnimationPosition, 148))
	{
		m_bInvisible = false;
		
			//m_LaserListRS.push_back({0.1f, {0.f, 0.f,45.f}						, {1.f,0.1f} });
			//m_LaserListRS.push_back({0.13f, {0.f, 0.f, 20 + (_float)(rand() % 141)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });
			//m_LaserListRS.push_back({0.16f, {0.f, 0.f, (_float)(rand() % 361)}, {0.5f + (rand() % 4) * 0.1f,0.1f + (rand() % 3) * 0.1f} });

		
	}

	else
	{
		if (fCurrentAnimationPosition > 200)
		{
			m_bInvisible = true;
		}
		else
		{

			
			//m_bFinalInvisibleToggle = !m_bFinalInvisibleToggle;
			//
			//if (m_bFinalInvisibleToggle)
			//	m_bInvisible = true;
			//else
			//	m_bFinalInvisibleToggle = false;



			m_iFinalInvisibleCount++;

			if (m_iFinalInvisibleCount >= 2)
			{
				m_iFinalInvisibleCount = 0;
				m_bInvisible = true;
			}
			else
				m_bInvisible = false;

			//m_bInvisible = false;
		}
	}


}

_bool CPlay_Hit::isNearlyEqual(_float CurValue, _float TargetValue)
{

	return std::fabs(CurValue - TargetValue) <= 0.1;

}

//AttackColliderResult CPlay_Hit::Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus)
AttackColliderResult CPlay_Hit::Set_Hit4(_uint eAnimation, AttackGrade eAttackGrade, AttackType eAttackType, _float fStunTime, _uint iDamage, _float fStopTime, _short iDirection, _float2 Impus,_bool bParticle)
{

	if (m_bCounterSucces)
		return RESULT_MISS;

	if (m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Air || m_pModelCom->m_iCurrentAnimationIndex == m_iBreakFall_Ground || m_pModelCom->m_iCurrentAnimationIndex == m_iBound_Ground
		|| m_pModelCom->m_iCurrentAnimationIndex == m_iLayUp)
		return RESULT_MISS;


	if (m_bCounterPose)
	{
		m_bCounterSucces = true;

		//�ð������� ���⼭? �ƴϸ� ����?
		Set_AnimationStopWithoutMe(0.5f);
		Set_AnimationStop(0.1f);
		m_bInvisible = true;

		//�ݰ� ���� ����Ʈ �����
		//Character_Make_Effect(TEXT("Moving_Line_Right"));
		Character_Create_Distortion({ 1.f,0.f,0.f }, { -0.6f * m_iLookDirection,0.f });

		CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
		main_Camera->StartCameraShake(0.1f, 0.3f);


		if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL)
		{
			Set_CurrentAnimationPositionJump(24.97f);
		}
		else if (m_pModelCom->m_iCurrentAnimationIndex == ANIME_ATTACK_SPECIAL_AIR)
		{
			Set_CurrentAnimationPositionJump(34.99f);
		}

		//Teleport_ToEnemy(1.3f, 0.1f);

		return RESULT_MISS;
	}
	//���ϻ���, ���ٴڿ� �������� �˻�
	else if (m_bHitGroundSmashed && Get_fHeight() == 0)
	{
		if (eAttackGrade != GRADE_ULTIMATE)
			return RESULT_MISS;
	}


	m_bInvisible = false;
	m_b236Posing = false;
	m_b236Special = false;
	m_b214Posing = false;
	m_fAccPoseTime = 0.f;
	m_bCounterSucces = false;

	//���� ���°� �ƴϸ� ���� üũ
	if (m_bStun == false)
	{

		AttackColliderResult eResult = Guard_Check3(eAttackType);

		//�������� ������ iDirection  �ǰ��ڴ� ���ֺ����ϴ� �� �ݴ�.


		if (eResult == RESULT_GUARD)
		{
			if (m_pModelCom->m_iCurrentAnimationIndex == m_iCrouchAnimationIndex)
				Set_Animation(m_iGuard_CrouchAnimationIndex);

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iIdleAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iBackWalkAnimationIndex)
				Set_Animation(m_iGuard_GroundAnimationIndex);

			else if (m_pModelCom->m_iCurrentAnimationIndex == m_iJumpAnimationIndex || m_pModelCom->m_iCurrentAnimationIndex == m_iFallAnimationIndex)
				Set_Animation(m_iGuard_AirAnimationIndex);


			//HitMotion { HIT_LIGHT, HIT_CHASE, HIT_MEDIUM, HIT_HEAVY, HIT_HEAVY_DOWN, HIT_CROUCH_MEDIUM, 
			// HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_UP, HIT_KNOCK_AWAY_LEFTDOWN, HIT_SPIN_AWAY_LEFTUP, HIT_WALLBOUNCE, HIT_NONE };


			//���� ����Ʈ
			switch (eAnimation)
			{
			case Client::HIT_LIGHT:
			case Client::HIT_CHASE:
			{

				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();

				m_pEffect_Manager->Copy_Layer(TEXT("Guard01"), &tDesc);
				break;
			}
			case Client::HIT_CROUCH_MEDIUM:
			case Client::HIT_MEDIUM:
			{
				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
				m_pEffect_Manager->Copy_Layer(TEXT("Guard02"), &tDesc);
				break;
			}
			case Client::HIT_HEAVY:
			case Client::HIT_HEAVY_DOWN:
			case Client::HIT_KNOCK_AWAY_LEFT:
			case Client::HIT_KNOCK_AWAY_UP:
			case Client::HIT_KNOCK_AWAY_LEFTDOWN:
			case Client::HIT_SPIN_AWAY_LEFTUP:
			case Client::HIT_SPIN_AWAY_UP:
			{
				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
				m_pEffect_Manager->Copy_Layer(TEXT("Guard03"), &tDesc);
				break;
			}

			case Client::HIT_WALLBOUNCE:
			case Client::HIT_NONE:
			default:
				break;
			}


			Set_CurrentAnimationPositionJump(0.f);

			return RESULT_GUARD;

		}
		else if (eResult != RESULT_HIT)    //ȸ�ǳ� ��� ��� �ƴϸ� �ϴ� ����.   �� ���� ���߿� ���� ó��
			return eResult;


	}


	m_bHit = TRUE;
	m_bStun = true;

	//m_fMaxStunTime = fStunTime;

	//����  �޺��� ���.7�����,  �������� �� 0.9��? ��� ���߿��� 1.2��?

	m_fMaxStunTime = fStunTime;// *(30 - CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(m_iPlayerTeam);

	if (Get_fHeight() > 1)
	{
		m_fMaxStunTime *= 1.1f;
	}

	m_fMaxStunTime *= 1.f - 0.01f * CBattleInterface_Manager::Get_Instance()->Get_HitAttackStep(m_iPlayerTeam);



	m_fAccStunTime = 0.f;

	Set_HitAnimation(eAnimation, Impus,bParticle);
	Set_AnimationStop(fStopTime);

	Set_bRedHP(true);

	if (iDirection == 1)
		FlipDirection(-1);
	else if (iDirection == -1)
		FlipDirection(1);

	//0�ΰ��� ������ ����

	m_iHP -= iDamage;


	m_iDebugComoboDamage += iDamage;

	//cout << "Dagage : " << iDamage << "  ,  Total : " << m_iDebugComoboDamage << endl;

	if (m_iHP < 0)
	{
		m_iHP = 0;
	}

	return RESULT_HIT;
}

void CPlay_Hit::MoveToEnemy_Ground(_float fMaxDistance, _float fOffset)
{
	// ��2
	//�� 9
	//�Ÿ� 7

	_float fEnemyPosX = m_pEnemy->Get_fPositionX();
	_float fMyPosX = Get_fPositionX();

	//������ �����ִ°��
	if (m_iLookDirection == 1)
	{
		//���� ���ʿ� �ִ°�쿣 ���ڸ�
		if (fEnemyPosX <= fMyPosX)
			return;


		//���� ������ ���� ���� ��������.

		_float fDistance = fEnemyPosX - fMyPosX;

		//���� �������̰�, �Ÿ��� �ִ�ġ���� �� ��� �ִ�ġ������ �̵�
		if (fDistance > fMaxDistance)
		{
			Add_Move({ fMaxDistance,0.f });
		}

		//���� �������̰�, �Ÿ��� �ִ�ġ���� ������� ���� �ձ��� �̵�
		else //if (fDistance <= fMaxDistance)
		{
			//myPos�� fEnemyPos�� �ǵ��� fDistance��ŭ �̵��ؾ��ϴµ� Offset�� �������Ѵ�?
			Add_Move({ fDistance - fOffset,0.f });
		}


	}

	//������ ���� �ִ� ���
	else if (m_iLookDirection == -1)
	{
		//���� ������ �ִ°�쿣 ���ڸ�
		if (fEnemyPosX >= fMyPosX)
			return;


		//���� ������ ���� ���� ������.

		_float fDistance = fMyPosX - fEnemyPosX;

		//���� �����̰�, �Ÿ��� �ִ�ġ���� �� ��� �ִ�ġ������ �̵�
		if (fDistance > fMaxDistance)
		{
			Add_Move({ -fMaxDistance,0.f });
		}

		//���� �������̰�, �Ÿ��� �ִ�ġ���� ������� ���� �ձ��� �̵�
		else //if (fDistance <= fMaxDistance)
		{
			//myPos�� fEnemyPos�� �ǵ��� fDistance��ŭ �̵��ؾ��ϴµ� Offset�� �������Ѵ�?
			Add_Move({ -fDistance + fOffset,0.f });
		}
	}



}

void CPlay_Hit::LaserListUpdate(_float fTimeDelta)
{

	//if (m_LaserList.size() == 0)
	//	return;
	//
	//for (auto& laser : m_LaserList)
	//{
	//	laser.first -= fTimeDelta;
	//
	//	if (laser.first <= 0)
	//	{
	//		//�� ��ġ ã�Ƽ� ������ ����Ʈ ����
	//		//CEffect_Layer* pTest = m_pEnemy->Character_Make_Effect(TEXT("Hit_Hand_Lazer"), {0.f,0.6f});
	//		CEffect_Layer* pTest = m_pEnemy->Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 0.f,0.8f });
	//		//0,0,0 ��  �߾�->���� ���� ����
	//		//pTest->Set_Copy_Layer_Rotation({ 0.f,0.f, 0.f });
	//
	//		pTest->Set_Copy_Layer_Rotation(laser.second);
	//
	//
	//	}
	//
	//}
	//
	//m_LaserList.remove_if([fTimeDelta](std::pair<_float, _float3>& laser) {
	//	return laser.first <= 0.0f; // LifeTime�� 0 �����̸� ����
	//});


	//for (auto& laser : m_LaserListRS)
	//{
	//	laser.fLifeTime -= fTimeDelta;
	//	if (laser.fLifeTime <= 0)
	//	{
	//		//CEffect_Layer* pTest = m_pEnemy->Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { 0.f,0.8f });
	//		CEffect_Layer* pTest = m_pEnemy->Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { rand()%30 *0.01f,0.6f + rand() % 30 * 0.01f });
	//
	//		pTest->Set_Copy_Layer_Rotation(laser.Rotation);
	//		pTest->Set_Copy_Layer_Scaled({ laser.Scale.x,laser.Scale.y,1.f });
	//
	//
	//	}
	//
	//}
	//m_LaserListRS.erase(
	//	std::remove_if(m_LaserListRS.begin(), m_LaserListRS.end(),
	//		[](const LaserData& laser) { return laser.fLifeTime <= 0; }),
	//	m_LaserListRS.end()
	//);

	m_LaserListRS.erase(std::remove_if(m_LaserListRS.begin(), m_LaserListRS.end(),[&](LaserData& laser) 
		{
			laser.fLifeTime -= fTimeDelta;
			if (laser.fLifeTime <= 0) 
			{
				//CEffect_Layer* pTest = m_pEnemy->Character_Make_Effect(TEXT("Hit_Hand_Lazer"),{ rand() % 30 * 0.01f, 0.6f + rand() % 30 * 0.01f });
				CEffect_Layer* pTest = m_pEnemy->Character_Make_Effect(TEXT("Hit_Hand_Lazer"), { rand() % 30 * -0.01f, 0.6f + rand() % 30 * 0.01f });
				pTest->Set_Copy_Layer_Rotation(laser.Rotation);
				pTest->Set_Copy_Layer_Scaled({ laser.Scale.x, laser.Scale.y, 1.f });
				return true; // ������ �׸�
			}
				return false; // ������ �׸�
		}),
		m_LaserListRS.end()
	);


}



CPlay_Hit* CPlay_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlay_Hit* pInstance = new CPlay_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlay_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlay_Hit::Clone(void* pArg)
{
	CPlay_Hit* pInstance = new CPlay_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlay_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlay_Hit::Free()
{
	__super::Free();


	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);

	Safe_Release(m_p2PTextureCom);

}
