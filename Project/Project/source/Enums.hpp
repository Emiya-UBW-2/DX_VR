#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//�萔
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };
	
	enum class Effect {
		ef_fire,			//���C��
		ef_reco,			//�����a���e
		ef_smoke,			//�e�̋O��
		ef_gndsmoke,		//�n�ʂ̋O��
		ef_fire2,			//���C��
		ef_hitblood,		//�����Ԃ�
		ef_greexp,			//�O������
		ef_greexp2,			//�O������2

		effects,			//�ǂݍ��ޑ���
	};
	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Vehicle,
			Ammo,
		};
		enum class SoundEnum {
			Tank_Shot,
			Tank_Ricochet,
			Tank_Damage,
			Tank_engine,
			Tank_Eject,
			Tank_Reload,

			Shot_Gun,
			Trigger,
			Cocking0,
			Cocking1,
			Cocking2,
			Cocking3,
			RunFoot,
			SlideFoot,
			StandupFoot,
			Heart,
			GateOpen,
			Num,
		};
		enum class CharaTypeID {
			Mine,
			Team,
			Enemy,
		};
	};


};
