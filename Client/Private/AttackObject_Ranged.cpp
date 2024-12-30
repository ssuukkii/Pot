#include "stdafx.h"
#include "..\Public\AttackObject_Ranged.h"

#include "RenderInstance.h"
#include "GameInstance.h"

#include "Character.h"
#include "Effect_Manager.h"

#include "Effect_Layer.h"
#include "ParryingRangedObject.h"
#include "Main_Camera.h"

CAttackObject_Ranged::CAttackObject_Ranged(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAttackObject{ pDevice, pContext }
{

}

CAttackObject_Ranged::CAttackObject_Ranged(const CAttackObject_Ranged& Prototype)
	: CAttackObject{ Prototype }
{

}

HRESULT CAttackObject_Ranged::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackObject_Ranged::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	ATTACK_RANGED_DESC* pDesc = static_cast<ATTACK_RANGED_DESC*>(pArg);

	m_fStartOffset = pDesc->fStartOffset;
	m_fRanged_Impus_NoneDirection = pDesc->fRanged_Impus_NoneDirection;
	m_iDirection = pDesc->iDirection;

	if (m_iDirection == -1)
	{
		m_pTransformCom->Set_Scaled(-1, 1, 1);
	}

	m_bPierce = pDesc->bPierce;
		

	m_bExplosion = pDesc->bExplosion;
	m_eRangeColor = pDesc->eRangeColor;

	_vector vPos = m_pOwner->Get_vPosition();
	_vector vStartOffset = { m_fStartOffset.x, m_fStartOffset.y, 0.f, 0.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vStartOffset);


	//�̸��� ������
	if (pDesc->strEffectName.length() != 0)
	{
		CEffect_Layer::COPY_DESC tDesc{};
		tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		m_pRangedEffect_Layer = CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(pDesc->strEffectName, &tDesc);
		//CEffect_Manager::Get_Instance()->Copy_Layer(pDesc->strEffectName, m_pTransformCom->Get_WorldMatrixPtr());

		if (pDesc->fEffectRotationDegree != 0)
		{
			if(m_iDirection == 1)
				m_pRangedEffect_Layer->Set_Copy_Layer_Rotation({ 0,0,pDesc->fEffectRotationDegree });
			else
				m_pRangedEffect_Layer->Set_Copy_Layer_Rotation({ 0,0,180-pDesc->fEffectRotationDegree });
		}

	}

	m_bExplosionEffectisHitEffect = pDesc->bExplosionEffectisHitEffect;
	m_bGroundExplosionEffect = pDesc->bGroundExplosionEffect;
	return S_OK;
}



void CAttackObject_Ranged::Update(_float fTimeDelta)
{

	if (Check_UpdateStop(fTimeDelta))
		return;



	m_fAccLifeTime += fTimeDelta;


	//�����ð� �����ų� �ʹٱ�(������)���� �������� ����
	if (m_fAccLifeTime > m_fLifeTime || Check_MapOut())
	{

		//���� �������
		if (Check_MapOut())
		{
			//�������� ��������Ʈ �Ǵ� �Ϲ� ��������Ʈ
			if (m_bGroundExplosionEffect)
			{
				if (m_bExplosion)
				{
					if (m_bExplosionEffectisHitEffect)
					{
						CEffect_Layer::COPY_DESC tDesc{};
						tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
						CEffect_Manager::Get_Instance()->Copy_Layer(m_strHitEffectName, &tDesc);
					}
					else
					{
						CEffect_Layer::COPY_DESC tDesc{};
						tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
						CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstJ3-Hit01"), &tDesc);
						m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::J_Attack_Hit_SFX, false, 1.f);
					}
				}
			}
			else
			{
				//CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Smoke03_Stop"));
				//CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("Smoke02_Small"));

				//���� ����Ʈ.
				//Character_Make_Effect(TEXT("Smoke03_Stop"), { 4.f,0.f }, true);
				//Character_Make_Effect(TEXT("Smoke02_Small"), { 2.7f,-0.3f }, true);



				_float4x4 Result4x4;

				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float3 fPos;
				XMStoreFloat3(&fPos, vPos);
				//_float ScaleX = (_float)Get_iDirection() * (1 - (2 * bFlipDirection));
				_matrix ovelapMatrix = XMMatrixScaling((_float)m_iDirection, 1.f, 1.f) * XMMatrixTranslation(fPos.x + (-0.3f * m_iDirection), fPos.y, 0.f);
				XMStoreFloat4x4(&Result4x4, ovelapMatrix);
				CEffect_Layer::COPY_DESC tDesc{};
				tDesc.pPlayertMatrix = &Result4x4;
				CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Smoke03_Stop"), &tDesc);

				_float4x4 Result4x4_2;
				ovelapMatrix = XMMatrixScaling((_float)m_iDirection, 1.f, 1.f) * XMMatrixTranslation(fPos.x + (-0.3f * m_iDirection), fPos.y-0.3f, 0.f);
				XMStoreFloat4x4(&Result4x4_2, ovelapMatrix);
				tDesc.pPlayertMatrix = &Result4x4_2;
				CEffect_Manager::Get_Instance()->Copy_Layer_AndGet(TEXT("Smoke02_Small"), &tDesc);



			}
		}

		if (m_pRangedEffect_Layer != nullptr)
			m_pRangedEffect_Layer->m_bIsDoneAnim = true;

		if (m_bPierce == false)
		{
			if (m_bEnableDestory)
			{
				CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
				//m_pGameInstance->Release_Collider(m_pColliderCom);
				m_bEnableDestory = false;
				Destory();

			}
		}
		else
		{
			if (m_bEnableDestory)
			{
				CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
				//m_pGameInstance->Release_Collider(m_pColliderCom);
				m_bEnableDestory = false;

			}

			Destory();

		}

		
		
		//���� ������
		//if (m_bEnableDestory)
		//{
		//
		//	if (m_pRangedEffect_Layer != nullptr)
		//		m_pRangedEffect_Layer->m_bIsDoneAnim = true;
		//
		//	CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);
		//	m_bEnableDestory = false;
		//	Destory();
		//}
	}
	else
	{
		m_pTransformCom->Add_Move({ m_fRanged_Impus_NoneDirection.x * m_iDirection * fTimeDelta , m_fRanged_Impus_NoneDirection.y * fTimeDelta ,0 });


		m_pColliderCom->Update(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

}

void CAttackObject_Ranged::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAttackObject_Ranged::Render(_float fTimeDelta)
{

#ifdef _DEBUG
	//m_pColliderCom->Render(fTimeDelta);
#endif // DEBUG


	return S_OK;
}



void CAttackObject_Ranged::OnCollisionEnter(CCollider* other, _float fTimeDelta)
{

	//�и��̶� ��
	if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_REFLECT || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_REFLECT)
	{
		BeReflect();


		//�и� ����
		CAttackObject* pAttackObject = static_cast<CAttackObject*>(other->GetMineGameObject());
		CCharacter* pCharacter = static_cast<CCharacter*>(pAttackObject->Get_pOwner());

		pCharacter->Character_Make_Effect(TEXT("Parrying_Hit"), { 0.5f,0.f });
		//pCharacter->Character_Make_Effect(TEXT("Parrying_Ball"), { 0.5f,0.f });

		pCharacter->Set_ReflectAttackBackEvent(true);

		pCharacter->Gain_KiAmount(15);



		//pAttackObject->Set_RemoteDestory();

	}


	//���Ÿ� vs ���Ÿ�
	else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Ranged_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Ranged_Attack)
	{


		//����Ʈ ó��
		Erase();

		if (m_eRangeColor != RANGED_LIGHT_NONE)
		{

			//if (m_eRangeColor == RANGED_LIGHT_YELLOW)
			if(m_bPierce == false)
			{
				//Add_YellowLight();

				Add_YellowLight(m_pColliderCom->Get_Overlap_Center_Position(other));

			}
		}

		//static_cast<CAttackObject_Ranged*>(other->GetMineGameObject())->Erase();
		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);

		if (m_bExplosion)
		{
			CEffect_Layer::COPY_DESC tDesc{};
			tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
			CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstJ3-Hit01"), &tDesc);
		}

		if (m_pRangedEffect_Layer != nullptr)
			m_pRangedEffect_Layer->m_bIsDoneAnim = true;

	}

	//�ƴϸ� ������ ���Ÿ� vs ��  ������ Ȥ�ø𸣴� if
	else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_BODY || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_BODY)
	{
		CCharacter* pCharacter = static_cast<CCharacter*>(other->GetMineGameObject());

	
		if (pCharacter->Check_bCurAnimationisChase())
		{
			BeReflect();
			return;

		}

		AttackColliderResult eResult =
			pCharacter->Set_Hit4(m_ihitCharacter_Motion, m_eAttackGrade, m_eAttackType, m_fhitCharacter_StunTime, m_iDamage, m_fAnimationLockTime, m_iOnwerDirection, m_fhitCharacter_Impus,!m_bPierce);

		if (eResult == RESULT_HIT)
		{
			pCharacter->Set_GroundSmash(m_bGroundSmash);
			m_pOwner->Set_AnimationStop(m_fAnimationLockTime);
			m_pOwner->Gain_KiAmount(m_iGainKiAmount);

			m_pOwner->Set_AttackBackEvent(true);
			pCharacter->Set_bNoGravity(m_bHitNoGravity, m_fMaxNoNoGravitySafeTime);

			if (m_fForcedGravityTime != 100)   //������ �⺻ ��. 0�� �� ���� �־ 100���� ��
			{
				pCharacter->Set_ForcveGravityTime(m_fForcedGravityTime);
			}

			if (m_bGrabbedEnd)
				pCharacter->Set_bGrabbed(false);


			m_pOwner->Gain_AttackStep(m_iGainAttackStep);
			m_pOwner->Gain_HitCount(m_iGainHitCount);

			if (m_isfxSoundIndex != 60000)
			{
				m_pGameInstance->Play_Sound((CSound_Manager::SOUND_KEY_NAME)m_isfxSoundIndex, false, m_fsfxVolume);
			}

			if (m_bOwnerNextAnimation)
			{
				m_pOwner->Set_NextAnimation(m_iOnwerNextAnimationIndex, 1.f);
			}

			if (m_eRangeColor != RANGED_LIGHT_NONE)
			{

				//if (m_eRangeColor == RANGED_LIGHT_YELLOW)
				if (m_bPierce == false)
				{
					//Add_YellowLight();
					Add_YellowLight(m_pColliderCom->Get_Overlap_Center_Position(other));
				}
			}
			if (m_bExplosion)
			{
				if (m_bExplosionEffectisHitEffect)
				{
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
					CEffect_Manager::Get_Instance()->Copy_Layer(m_strHitEffectName, &tDesc);
			

				}
				else
				{
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstJ3-Hit01"), &tDesc);
					m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::J_Attack_Hit_SFX, false, 1.f);
				}
			}
			if (m_fCameraShakeDuration != 0)
			{
				CMain_Camera* main_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Main_Camera")));
				main_Camera->StartCameraShake(m_fCameraShakeDuration, m_fCameraShakeMagnitude);
			}

			if (m_bPierce == false)
				if (m_pRangedEffect_Layer != nullptr)
					m_pRangedEffect_Layer->m_bIsDoneAnim = true;

			
			if (m_iCallAttackBackIndex != 60000)
			{
				m_pOwner->AttackEvent(m_iCallAttackBackIndex);
			}

		}
		else if (eResult == RESULT_GUARD) //������ص� �浹�� ������ �ð���������
		{
			//m_pOwner->Set_AnimationStop(0.08f);
			//pCharacter->Set_AnimationStop(0.08f);
			m_pOwner->Set_AttackBackEvent(true);

			if (m_eRangeColor != RANGED_LIGHT_NONE)
			{

				//if (m_eRangeColor == RANGED_LIGHT_YELLOW)
				if (m_bPierce == false)
				{
					//Add_YellowLight();
					Add_YellowLight(m_pColliderCom->Get_Overlap_Center_Position(other));
				}
			}
			if (m_bExplosion)
			{
				if (m_bExplosionEffectisHitEffect)
				{
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
					CEffect_Manager::Get_Instance()->Copy_Layer(m_strHitEffectName, &tDesc);
				}
				else
				{
					CEffect_Layer::COPY_DESC tDesc{};
					tDesc.pPlayertMatrix = m_pTransformCom->Get_WorldMatrixPtr();
					CEffect_Manager::Get_Instance()->Copy_Layer(TEXT("BurstJ3-Hit01"), &tDesc);
					m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::J_Attack_Hit_SFX, false, 1.f);
				}
			}



			if (m_bPierce == false)
				if (m_pRangedEffect_Layer != nullptr)
					m_pRangedEffect_Layer->m_bIsDoneAnim = true;


			

		}

		//else if (eResult == RESULT_DRAW)
		//{
		//	m_pOwner->Set_AnimationStop(0.3f);
		//	pCharacter->Set_AnimationStop(0.3f);
		//}

		else if (eResult == RESULT_MISS)
		{
			////���� �ѹ� �������� ��
			//if (m_eAttackType == ATTACKTYPE_GRAB_GROUND || m_eAttackType == ATTACKTYPE_GRAB_AIR)
			//{
			//	if (m_bEnableDestory)
			//	{
			//		CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
			//		m_bEnableDestory = false;
			//	}
			//}
			//
			////�� �ܿ��� �������� ������� ����
			//else
			return;

		}


		//Pierce �߰� �� �ڵ�
		//if (m_bEnableDestory)
		//{
		//	CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
		//	m_bEnableDestory = false;
		//	Destory();
		//}

		if (m_bPierce == false)
		{
			if (m_bEnableDestory)
			{
				CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
				//m_pGameInstance->Release_Collider(m_pColliderCom);
				m_bEnableDestory = false;
				Destory();

			}
		}
		else
		{
			if (m_bEnableDestory)
			{
				CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
				//m_pGameInstance->Release_Collider(m_pColliderCom);
				m_bEnableDestory = false;

			}

			//if (m_fLifeTime > m_fAccLifeTime)
			//{
			//	Destory();
			//}

		}

	}

	//vs ���������� ��� ���÷�Ʈ�� �����Ѱ� Ȯ��
	//else if (other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_1P_Melee_Attack || other->m_ColliderGroup == CCollider_Manager::COLLIDERGROUP::CG_2P_Melee_Attack)
	//{
	//	CAttackObject* pAttackOjbect = static_cast<CAttackObject*>(other->GetMineGameObject());
	//	pAttackOjbect->Set_AttackBackEvent();
	//
	//	if (pAttackOjbect->Get_bReflect() == true)
	//	{
	//		////Range-Melee�� �浹�� �� �ڵ� Destroy���� ������ �־����
	//		//m_pGameInstance->Release_Collider(m_pColliderCom); 
	//		//CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;	
	//		//
	//		////����Ʈ, �ʹ����� �̵�
	//
	//		BeReflect();
	//
	//	}
	//	
	//}


	_bool Debug = true;
}

void CAttackObject_Ranged::OnCollisionStay(CCollider* other, _float fTimeDelta)
{
	_bool Debug = true;

}

void CAttackObject_Ranged::OnCollisionExit(CCollider* other)
{
	_bool Debug = true;

}

void CAttackObject_Ranged::CollisingAttack()
{
}

void CAttackObject_Ranged::CollisingPlayer()
{
}

_bool CAttackObject_Ranged::Check_MapOut()
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

void CAttackObject_Ranged::Add_YellowLight(_float3 fPosition)
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


void CAttackObject_Ranged::Add_YellowLight()
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


void CAttackObject_Ranged::Erase()
{
	if(m_bPierce == false)
	{
		if (m_bEnableDestory)
		{
			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
			//m_pGameInstance->Release_Collider(m_pColliderCom);
			m_bEnableDestory = false;
			Destory();

		}
	}
	else
	{
		if (m_bEnableDestory)
		{
			CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
			//m_pGameInstance->Release_Collider(m_pColliderCom);
			m_bEnableDestory = false;

		}
	
		if (m_fLifeTime > m_fAccLifeTime)
		{
			Destory();
		}

	}
}

_bool CAttackObject_Ranged::BeReflect()
{

	if (m_bCreateReflectEnable == false)
		return false;

	m_bCreateReflectEnable = false;

	//Range-Melee�� �浹�� �� �ڵ� Destroy���� ������ �־����
	CGameInstance::Get_Instance()->Destroy_Reserve(m_pColliderCom);;
	//m_pGameInstance->Release_Collider(m_pColliderCom);

	//����Ʈ, �ʹ����� �̵�
	if (m_pRangedEffect_Layer != nullptr)
		m_pRangedEffect_Layer->m_bIsDoneAnim = true;


	CParryingRangedObject::PARRYING_RANGED_DESC Desc{};
	Desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Desc.bDestroyObject = false;
	Desc.iColor = m_eRangeColor;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ParryingRangedObject"), TEXT("Layer_AttackObject"), &Desc);


	Destory();
	return true;

}







CAttackObject_Ranged* CAttackObject_Ranged::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackObject_Ranged* pInstance = new CAttackObject_Ranged(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAttackObject_Ranged"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackObject_Ranged::Clone(void* pArg)
{
	CAttackObject_Ranged* pInstance = new CAttackObject_Ranged(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAttackObject_Ranged"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackObject_Ranged::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);

}
