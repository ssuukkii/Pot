#pragma once

#include <process.h>
#include "GameInstance.h"

namespace Client
{
	
	//��� �����ӿ� �ϸ� ĳ���ʹ� �� 80�� �ִϸ��̼�.  ���� �ϸ�  1600����Ʈ   4���̴� 6400����Ʈ

	//play animation �߿� ���� ĳ���͸� �̿��� ����, ���� �ִϸ��̼��� 


	const static enum CHARACTER_INDEX {
		SELECT_GOKU=0, SELECT_FRN, SELECT_21, SELECT_HIT,
		PLAY_GOKU,  PLAY_FRN, PLAY_21, PLAY_HIT, 
		OPENING_GOKU, OPENIING_21, SKILL_GOKU, OPENING_KRN, CHARACTER_INDEX_END
	};

	const static enum HitMotion { HIT_LIGHT, HIT_CHASE, HIT_MEDIUM, HIT_HEAVY, HIT_HEAVY_DOWN, HIT_CROUCH_MEDIUM, HIT_KNOCK_AWAY_LEFT, HIT_KNOCK_AWAY_LEFT_NONEBOUNDE, HIT_KNOCK_AWAY_UP, HIT_KNOCK_AWAY_UP_GRAVITY, HIT_KNOCK_AWAY_LEFTDOWN, HIT_SPIN_AWAY_LEFTUP, HIT_SPIN_AWAY_UP,HIT_WALLBOUNCE, HIT_NONE};
	const static enum AttackGrade { GRADE_NORMAL, GRADE_ULTIMATE};
	const static enum AttackType { ATTACKTYPE_LOW ,ATTACKTYPE_MIDDLE, ATTACKTYPE_HIGH , ATTACKTYPE_GRAB_GROUND, ATTACKTYPE_GRAB_AIR, ATTACKTYPE_COMMANDGRAB};
	const static enum AttackColliderResult { RESULT_HIT, RESULT_GUARD, RESULT_MISS, RESULT_DRAW ,RESULT_NONE};

	using Event = list<string>;
	using FrameEvent_KeyFrame = map<_float, Event>;
	using FrameEvent_AnimationIndex = map<_int, FrameEvent_KeyFrame>; //�ִϸ��̼� �ε��� ����
	using FrameEventMap = map<CHARACTER_INDEX, FrameEvent_AnimationIndex>;  //ĳ����enum,������



	//FrameEventMap FrameEvent;


	//1. ĳ���ͷ� ����
	//2. �ִϸ��̼� �ε����� ����
	//3. ���������� ����
	 
	//map<_float, _float>FrameEvent_KeyFrame;
	//map<_int, FrameEvent_KeyFrame>FrameEvent_AnimationIndex; //�ִϸ��̼� �ε��� ����
	//map<CHARACTER_INDEX, Frame_AnimationIndex>FrameEvent;  //ĳ����enum,������
	
	 
	//[ĳ����enum][�ִϸ��̼�index][Ű������] =  ��¼��;

	//FrameEvent[CHARACTER_INDEX_END][287][10] = 5.f;

	//FrameEvent[PLAY_GOKU][IDLE][5].push_back("test");
	
	//FrameEvent[PLAY_GOKU][IDLE][0].push_back(�ӵ�2��);
	//FrameEvent[PLAY_GOKU][ATTACK][2].push_back(�浹ó��);



	//�̺�Ʈ�� �����Ҷ��� ������Ʈ ���� �����Ӱ�  ������Ʈ ���� ������ ������ ���� ������




}