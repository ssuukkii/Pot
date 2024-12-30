#include "stdafx.h"
#include "..\Public\AttackObject_Energy.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "Character.h"
#include "Effect_Manager.h"


#include <iostream>
#include"Main_Camera.h"

CAttackObject_Energy::CAttackObject_Energy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAttackObject{ pDevice, pContext }
{

}

CAttackObject_Energy::CAttackObject_Energy(const CAttackObject_Energy& Prototype)
	: CAttackObject{ Prototype }
{

}

HRESULT CAttackObject_Energy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackObject_Energy::Initialize(void* pArg)
{


	ATTACK_RANGED_DESC* pDesc = static_cast<ATTACK_RANGED_DESC*>(pArg);
	pDesc->bNoCreateMainCollider = true;

	m_ecolliderGroup = pDesc->ColliderDesc.colliderGroup;

	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_bHitNoGravity = true;

	m_fStartOffset = pDesc->fStartOffset;
	//m_fRanged_Impus_NoneDirection = pDesc->fRanged_Impus_NoneDirection;
	//m_iDirection = pDesc->iDirection;
	m_fMoveSpeedNoneDirection = pDesc->fMoveSpeedNoneDirection;
	m_iAttackCount = pDesc->iAttackCount;
	m_iPlayerDirection = pDesc->iPlayerDirection;

	m_fMoveSpeedNoneDirection.x *= m_iPlayerDirection;


	m_fAttackDelayTime = pDesc->fAttackDelayTime;
	m_fColliderfCY = pDesc->fColliderfCY;

	m_bDying = false;


	m_eEnegrgyColor = pDesc->eEnergyColor;
	m_bHeightCorrection = pDesc->bHeightCorrection;

	_vector vPos = m_pOwner->Get_vPosition();
	_vector vStartOffset = { m_fStartOffset.x, m_fStartOffset.y, 0.f, 0.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vStartOffset);

	return S_OK;
}



void CAttackObject_Energy::Update(_float fTimeDelta)
{
	if (Check_UpdateStop(fTimeDelta))
		return;

	m_fAccLifeTime += fTimeDelta;
	m_fAccAttackDelayTime += fTimeDelta;

	//�����ð� �����ų� �ʹٱ�(������)���� �������� ����
	if (m_fAccLifeTime > m_fLifeTime)
	{
		if (m_bEnableDestory)
		{
			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
			
		

			//m_pGameInstance->Release_Collider(m_pColliderCom);
			m_bEnableDestory = false;
			Destory();
		}
	}
	else
	{
		//for (auto& iter : m_vecColliderCom)
		//	iter->Update(m_pTransformCom->Get_WorldMatrix());



		_float speed = 0.1f;

		m_fEndPos.x += m_fMoveSpeedNoneDirection.x * fTimeDelta;
		m_fEndPos.y += m_fMoveSpeedNoneDirection.y * fTimeDelta;


		Make_Collider(m_pColliderCom->m_ColliderGroup, _float2(0.f, 0.f), _float2(m_fEndPos.x, m_fEndPos.y));
		//Make_Collider(m_ecolliderGroup, _float2(0.f, 0.f), _float2(m_fEndPos.x, m_fEndPos.y));

		

		_vector vPosOffset = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ m_fStartOffset.x,m_fStartOffset.y,0,0 };


		//for (auto& iter : m_vecColliderCom)
		//	iter->UpdateVector(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ m_fStartOffset.x,m_fStartOffset.y,0,0});


		for (auto& iter : m_vecColliderCom)
			iter->Update(vPosOffset);

	}
}

void CAttackObject_Energy::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAttackObject_Energy::Render(_float fTimeDelta)
{
#ifdef _DEBUG
	//m_pColliderCom->Render(fTimeDelta);

	for (auto pCollider : m_vecColliderCom)
	{
		pCollider->Render(fTimeDelta);
	}
#endif // DEBUG

	return S_OK;
}



void CAttackObject_Energy::OnCollisionEnter(CCollider* other, _float fTimeDelta)
{

	if (m_bEnterEnable == false)
		return;
	


	//�������� vs ���÷�Ʈ 
	if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_REFLECT || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_REFLECT)
	{
		m_iAttackCount--;
		if (m_iAttackCount <= 0)
		{

			Erase();
		}
	}


	//�������� vs ��� 
	else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_BODY || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_BODY)
	{
		
			m_bEnterEnable = false;

			CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

		
			if (pCharacter->Get_bReflect())
			{
				//m_iAttackCount--;
				//if (m_iAttackCount <= 0)
				//{
				//	Erase();
				//}
				return;
			}

			//AttackColliderResult eResult =
			//	pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), {});


			//AttackColliderResult eResult =
			//	pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), { m_fhitCharacter_Impus.x *0.2f, m_fhitCharacter_Impus .y *0.2f});


			HitMotion eHitMotion;

			if (m_iAttackCount != 1 && m_ihitCharacter_Motion == HIT_KNOCK_AWAY_LEFT)
				eHitMotion = HIT_KNOCK_AWAY_LEFT_NONEBOUNDE;
			else
				eHitMotion = m_ihitCharacter_Motion;


			//AttackColliderResult eResult =
			//	pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), { m_fhitCharacter_Impus.x * 0.01f,0.001 });

			AttackColliderResult eResult =
				pCharacter->Set_Hit4(eHitMotion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), { m_fhitCharacter_Impus.x * 0.01f,0.001 },false);


			if (eResult == RESULT_HIT)
			{
				m_pOwner->Gain_AttackStep(m_iGainAttackStep);
				m_pOwner->Gain_HitCount(m_iGainHitCount);

				pCharacter->Set_GroundSmash(m_bGroundSmash);

				if(m_bOnwerHitNoneStop == false)
					m_pOwner->Set_AnimationStop(m_fAnimationLockTime);

				m_pOwner->Gain_KiAmount(m_iGainKiAmount);

				pCharacter->Set_bNoGravity(m_bHitNoGravity);

				//pCharacter->Set_fGravityTime(0.f);


				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float fEnergyHeight = XMVectorGetY(vPos);
				_float fpCharacterHeight = pCharacter->Get_fHeight();

				if (m_bHeightCorrection)
				{
				
	
					if (m_fMoveSpeedNoneDirection.y == 0)
					{
						//�������İ� �÷��̾�� ���� ���� ��� ĳ���͸� ������ �Ʒ��� �����
						if (fpCharacterHeight - fEnergyHeight > 0.3f)
						{
							pCharacter->Add_Move({ 0.f,-0.2f });
						}
						//�������İ� �÷��̾�� ���� ���� ��� ĳ���͸� ������ ����ø�
						else if (fpCharacterHeight - fEnergyHeight < -0.3f)
						{
							pCharacter->Add_Move({ 0.f,0.2f });
						}

					}


					//_float fHeight = pCharacter->Get_fHeight();  //���� ������������ 0.2�γ���
					if (fpCharacterHeight < 0.3)
					{
						pCharacter->Add_Move({ 0.f,0.15f });
						pCharacter->Set_HitAnimation(m_ihitCharacter_Motion, { 0.f,0.02f },false);
					}
				}
				m_iAttackCount--;

				if (m_iAttackCount != 0)
				{
					//cout << m_iAttackCount << endl;
				}
				else
				{
					CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
					Destory();
				}

				if (m_fForcedGravityTime != 100)   //������ �⺻ ��. 0�� �� ���� �־ 100���� ��
				{
					pCharacter->Set_ForcveGravityTime(m_fForcedGravityTime);
				}

				if (m_bGrabbedEnd)
					pCharacter->Set_bGrabbed(false);


				//��Ʈ�� ����Ʈ
				switch (m_ihitCharacter_Motion)
				{
				case Client::HIT_LIGHT:
				case Client::HIT_CHASE:
				{				//m_pEffect_Manager->Copy_Layer(TEXT("BurstU-1"), m_pTransformCom->Get_WorldMatrixPtr());

					_float3 fPos = m_pColliderCom->Get_Overlap_Center_Position(other);// +_float3{ 0.2 - (rand() % 5 * 0.1), 0.1 - (rand() % 3 * 0.1), 0.f };  //xyz��ǥ�ε�
					_matrix ovelapMatrix = XMMatrixScaling((_float)m_pOwner->Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x, fPos.y, fPos.z);
					XMFLOAT4X4 Result4x4;
					XMStoreFloat4x4(&Result4x4, ovelapMatrix);
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = &Result4x4;
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstU-1"), &tDesc);
					//m_pEffect_Manager->Copy_Layer(TEXT("BurstU-1"), resultMatrix);
				}

				break;


				case Client::HIT_CROUCH_MEDIUM:
				case Client::HIT_MEDIUM:
				{
					_float3 fPos = m_pColliderCom->Get_Overlap_Center_Position(other);  //xyz��ǥ�ε�
					_matrix ovelapMatrix = XMMatrixScaling((_float)m_pOwner->Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x, fPos.y, fPos.z);
					XMFLOAT4X4 Result4x4;
					XMStoreFloat4x4(&Result4x4, ovelapMatrix);
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = &Result4x4;
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstU-2"), &tDesc);
				}

				break;

				case Client::HIT_HEAVY:
				case Client::HIT_HEAVY_DOWN:
				case Client::HIT_KNOCK_AWAY_LEFT:
				case Client::HIT_KNOCK_AWAY_UP:
				case Client::HIT_KNOCK_AWAY_LEFTDOWN:
				case Client::HIT_SPIN_AWAY_LEFTUP:
				{
					_float3 fPos = m_pColliderCom->Get_Overlap_Center_Position(other);  //xyz��ǥ�ε�
					_matrix ovelapMatrix = XMMatrixScaling((_float)m_pOwner->Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x, fPos.y, fPos.z);
					XMFLOAT4X4 Result4x4;
					XMStoreFloat4x4(&Result4x4, ovelapMatrix);
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = &Result4x4;
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstU-2"), &tDesc);
					break;
				}
				case Client::HIT_WALLBOUNCE:
				case Client::HIT_NONE:
				default:
					break;
				}



			}
			else if (eResult == RESULT_GUARD) //����
			{
				
				//CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
				//Destory();
				
				m_iAttackCount--;
				if (m_iAttackCount <= 0)
				{
					CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
					Destory();
				}

			}

			else if (eResult == RESULT_DRAW) //�������� vs ����ε� DRAW�� ���?
			{
				//m_pOwner->Set_AnimationStop(0.3f);
				//pCharacter->Set_AnimationStop(0.3f);
			}

			else if (eResult == RESULT_MISS)
			{

				return;

			}


	}

	
}

void CAttackObject_Energy::OnCollisionStay(CCollider* other, _float fTimeDelta)
{

	//�������� vs ���÷�Ʈ 
	if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_REFLECT || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_REFLECT)
	{
		if (m_fAccAttackDelayTime > m_fAttackDelayTime)
		{

			m_fAccAttackDelayTime = 0.f;

			m_iAttackCount--;
			if (m_iAttackCount <= 0)
			{

				Erase();
			}
		}
	}
	//�������� vs ��� 
	else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_BODY || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_BODY)
	{

		//���� ���� ���صθ� 0.07�ʸ��� ��Ʈ����
		if (m_fAccAttackDelayTime > m_fAttackDelayTime)
		{
	
			m_fAccAttackDelayTime = 0.f;
	
			CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

			if (pCharacter->Get_bReflect())
			{
				//m_iAttackCount--;
				//if (m_iAttackCount <= 0)
				//{
				//	//CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
				//	Erase();
				//}
				return;
			}

			AttackColliderResult eResult{ RESULT_NONE };
	

			
			if (m_iAttackCount != 0)
			{
				m_iAttackCount--;
				//eResult = pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), {});
	
				HitMotion eHitMotion;

				if (m_iAttackCount != 1 && m_ihitCharacter_Motion == HIT_KNOCK_AWAY_LEFT)
					eHitMotion = HIT_KNOCK_AWAY_LEFT_NONEBOUNDE;
				else
					eHitMotion = m_ihitCharacter_Motion;


				//AttackColliderResult eResult =
				//	pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), { m_fhitCharacter_Impus.x * 0.01f,0.001 });

				eResult = pCharacter->Set_Hit4(eHitMotion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), { m_fhitCharacter_Impus.x * 0.01f,0.001 },false);

			}
			//else if (m_iAttackCount<=0)
			//{
			//	OnCollisionExit(other);
			//}
	
			
	
			if(eResult == RESULT_HIT)
			{
	
				pCharacter->Set_GroundSmash(m_bGroundSmash);

				if (m_bOnwerHitNoneStop == false)
					m_pOwner->Set_AnimationStop(m_fAnimationLockTime);

				m_pOwner->Gain_KiAmount(m_iGainKiAmount);
				m_pOwner->Gain_HitCount(m_iGainHitCount);

				pCharacter->Set_bNoGravity(m_bHitNoGravity);

				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float fEnergyHeight = XMVectorGetY(vPos);
				_float fpCharacterHeight = pCharacter->Get_fHeight();

				if (m_fCameraShakeDuration != 0)
				{
					CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
					main_Camera->StartCameraShake(m_fCameraShakeDuration, m_fCameraShakeMagnitude);
				}


				if (m_bHeightCorrection)
				{

					if (m_fMoveSpeedNoneDirection.y == 0)
					{
						//�������İ� �÷��̾�� ���� ���� ��� ĳ���͸� ������ �Ʒ��� �����
						if (fpCharacterHeight - fEnergyHeight > 0.3f)
						{
							pCharacter->Add_Move({ 0.f,-0.2f });
						}
						//�������İ� �÷��̾�� ���� ���� ��� ĳ���͸� ������ ����ø�
						else if (fpCharacterHeight - fEnergyHeight < -0.3f)
						{
							pCharacter->Add_Move({ 0.f,0.2f });
						}

					}
				}
				//if (m_iAttackCount == 0)
				//{
				//	//Erase();
				//	//CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
				//}
				if (m_iAttackCount <= 0)
				{
					OnCollisionExit(other);
				}

	
				if (m_fForcedGravityTime != 100)   //������ �⺻ ��. 0�� �� ���� �־ 100���� ��
				{
					pCharacter->Set_ForcveGravityTime(m_fForcedGravityTime);
				}
	
				if (m_bGrabbedEnd)
					pCharacter->Set_bGrabbed(false);
	
	
				//��Ʈ�� ����Ʈ
				switch (m_ihitCharacter_Motion)
				{
				case Client::HIT_LIGHT:
				case Client::HIT_CHASE:
				{				//m_pEffect_Manager->Copy_Layer(TEXT("BurstU-1"), m_pTransformCom->Get_WorldMatrixPtr());
	
					_float3 fPos = m_pColliderCom->Get_Overlap_Center_Position(other);// +_float3{ 0.2 - (rand() % 5 * 0.1), 0.1 - (rand() % 3 * 0.1), 0.f };  //xyz��ǥ�ε�
					_matrix ovelapMatrix = XMMatrixScaling((_float)m_pOwner->Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x, fPos.y, fPos.z);
					XMFLOAT4X4 Result4x4;
					XMStoreFloat4x4(&Result4x4, ovelapMatrix);
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = &Result4x4;
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstU-1"), &tDesc);
					//m_pEffect_Manager->Copy_Layer(TEXT("BurstU-1"), resultMatrix);
				}
	
				break;
	
	
				case Client::HIT_CROUCH_MEDIUM:
				case Client::HIT_MEDIUM:
				{
					_float3 fPos = m_pColliderCom->Get_Overlap_Center_Position(other);  //xyz��ǥ�ε�
					_matrix ovelapMatrix = XMMatrixScaling((_float)m_pOwner->Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x, fPos.y, fPos.z);
					XMFLOAT4X4 Result4x4;
					XMStoreFloat4x4(&Result4x4, ovelapMatrix);
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = &Result4x4;
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstU-2"), &tDesc);
				}
	
				break;
	
				case Client::HIT_HEAVY:
				case Client::HIT_HEAVY_DOWN:
				case Client::HIT_KNOCK_AWAY_LEFT:
				case Client::HIT_KNOCK_AWAY_UP:
				case Client::HIT_KNOCK_AWAY_LEFTDOWN:
				case Client::HIT_SPIN_AWAY_LEFTUP:
				{
					_float3 fPos = m_pColliderCom->Get_Overlap_Center_Position(other);  //xyz��ǥ�ε�
					_matrix ovelapMatrix = XMMatrixScaling((_float)m_pOwner->Get_iDirection(), 1.f, 1.f) * XMMatrixTranslation(fPos.x, fPos.y, fPos.z);
					XMFLOAT4X4 Result4x4;
					XMStoreFloat4x4(&Result4x4, ovelapMatrix);
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = &Result4x4;
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstU-2"), &tDesc);
					break;
				}
				case Client::HIT_WALLBOUNCE:
				case Client::HIT_NONE:
				default:
					break;
				}
	
	
				
	
			}
			else if (eResult == RESULT_GUARD) //����
			{
				if (m_iAttackCount <= 0)
				{
					CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
					Destory();
				}
			}
	
			else if (eResult == RESULT_DRAW) //�������� vs ����ε� DRAW�� ���?
			{
				//m_pOwner->Set_AnimationStop(0.3f);
				//pCharacter->Set_AnimationStop(0.3f);
			}
	
			else if (eResult == RESULT_MISS)
			{
	
					return;
	
			}
	
			
		}
		
	}

}

void CAttackObject_Energy::OnCollisionExit(CCollider* other)
{
	CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

	AttackColliderResult eResult =
			pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_pOwner->Get_iDirection(), m_fhitCharacter_Impus,false);

	pCharacter->Set_bNoGravity(false);
	Erase();
	Destory();

	//CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
	//m_bEnableDestory = false;
	//Destory();

	//Erase();
}

void CAttackObject_Energy::CollisingAttack()
{
}

void CAttackObject_Energy::CollisingPlayer()
{
}

_bool CAttackObject_Energy::Check_MapOut()
{
	//���� üũ
	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 0.1)
	{
		//���� ���� ����Ʈ
		return true;
	}

	//�¿� �ٱ�üũ   �� ���?
	else if (false)
	{

	}
	else
		return false;

}



void CAttackObject_Energy::Make_Collider(CCollider_Manager::COLLIDERGROUP eColliderGroup, _float2 SourcePos, _float2 DestPos)
{

	// 1. �������� ���� ������ �Ÿ� �� ���� ���
	_float dx = DestPos.x - SourcePos.x;
	_float dy = DestPos.y - SourcePos.y;

	// �� �� ������ �Ÿ� ���
	_float distance = sqrtf(dx * dx + dy * dy);

	// ���� ���� �� ����ȭ
	_float2 direction = { dx / distance, dy / distance };

	// 2. �ʿ��� �ݶ��̴��� ���� ���
	//_float unitLength = m_UnitSize.x; // ���� �ݶ��̴��� ���� ũ��
	_float unitLength = 0.4f; // ���� �ݶ��̴��� ���� ũ��
	//_float unitHeight = 0.8f; // ���� �ݶ��̴��� ���� ũ��
	_float unitHeight = m_fColliderfCY; // ���� �ݶ��̴��� ���� ũ��



	int requiredColliders = static_cast<int>(ceil(distance / unitLength));

	// 3. ���� �ݶ��̴� �׷��� �ݶ��̴� �� Ȯ��
	int currentColliders = static_cast<int>(m_vecColliderCom.size());

	// 4. �ʿ��� ��� �ݶ��̴� �߰� ���� �� ��ġ ����
	for (int i = currentColliders; i < requiredColliders; ++i)
	{
		// �ݶ��̴��� ���� ��ġ ���
		_float currentDistance = unitLength * (i + 0.5f);
		_float2 colliderPos = {
		   SourcePos.x + direction.x * currentDistance,
		   SourcePos.y + direction.y * currentDistance
		};

		// �ݶ��̴� �߰� ����
		CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};
		//BoundingDesc.vExtents = _float3(m_UnitSize.x / 2.0f, m_UnitSize.y / 2.0f, 0.5f);
		BoundingDesc.vExtents = _float3(unitLength / 2.0f, unitHeight / 2.0f, 0.5f);
		//BoundingDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);


		BoundingDesc.vCenter = _float3(colliderPos.x, colliderPos.y, 0.f); // ���� �� ��ġ ����
		BoundingDesc.pMineGameObject = this;
		BoundingDesc.colliderGroup = eColliderGroup;

		CCollider* pNewCollider = nullptr;
		_wstring colliderName = L"Com_Collider_" + to_wstring(i);


		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			colliderName.c_str(), reinterpret_cast<CComponent**>(&pNewCollider), &BoundingDesc)))
			return; // ���� ó��

		/*

		CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
		ColliderDesc = pDesc->ColliderDesc;
		ColliderDesc.pMineGameObject = this;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;


		//m_pColliderCom->Update(m_pOwnerTransform->Get_WorldMatrix());
		m_pColliderCom->Update(m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));

		m_pGameInstance->Add_ColliderObject(ColliderDesc.colliderGroup, m_pColliderCom);

		*/

		pNewCollider->Update(m_pTransformCom->Get_WorldMatrix());

		// �ݶ��̴� ���Ϳ� �߰�
		m_vecColliderCom.push_back(pNewCollider);

		// �ݶ��̴� �Ŵ����� �߰�
		m_pGameInstance->Add_ColliderObject(eColliderGroup, pNewCollider);
	}
}


/*

void CAttackObject_Energy::Add_YellowLight()
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
	LightDesc.fLifeTime = 0.5f;
	LightDesc.strName = "Explosion";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}

void CAttackObject_Energy::Add_YellowLight(_float3 fPosition)
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(fPosition.x, fPosition.y, 0.f, 1.f);
	LightDesc.fRange = 3.f;
	LightDesc.vDiffuse = _float4(1.2f, 1.15f, 0.7f, 1.0f);
	//LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 0.95f, 0.45f, 1.f);

	LightDesc.fAccTime = 0.f;
	LightDesc.fLifeTime = 0.5f;
	LightDesc.strName = "Explosion";
	if (FAILED(m_pRenderInstance->Add_Effect_Light(LightDesc.strName, LightDesc)))
		return;
}
*/


void CAttackObject_Energy::Erase()
{
	if (m_bEnableDestory)
	{
		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);

	

		m_bEnableDestory = false;
	}
}







CAttackObject_Energy* CAttackObject_Energy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackObject_Energy* pInstance = new CAttackObject_Energy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAttackObject_Energy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackObject_Energy::Clone(void* pArg)
{
	CAttackObject_Energy* pInstance = new CAttackObject_Energy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAttackObject_Energy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackObject_Energy::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	for (auto& iter : m_vecColliderCom)
		Safe_Release(iter);

	if (m_pOwner == nullptr)
		return;

	if (m_pOwner->Get_iPlayerTeam() == 1)
		m_pGameInstance->Destroy_Reserve(CCollider_Manager::CG_1P_Energy_Attack);
	else
		m_pGameInstance->Destroy_Reserve(CCollider_Manager::CG_2P_Energy_Attack);


}
