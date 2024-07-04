#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::Cam2DControl* SingletonBase<FPS_n2::Sceneclass::Cam2DControl>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
			SE->Add((int)SoundEnum::Env2, 1, "data/Sound/SE/envi2.wav", false);

			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::CountDown, 2, "data/Sound/SE/CountDown.wav", false);

			SE->Add((int)SoundEnum::Second, 1, "data/Sound/SE/second.wav", false);
			SE->Add((int)SoundEnum::OneMunute, 1, "data/Sound/SE/OneMinute.wav", false);
			SE->Add((int)SoundEnum::TimeUp, 1, "data/Sound/SE/TimeUp.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::CountDown).SetVol_Local(128);

			SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::Env);
			SE->Delete((int)SoundEnum::Env2);
			SE->Delete((int)SoundEnum::RunFoot);
			SE->Delete((int)SoundEnum::CountDown);
			SE->Delete((int)SoundEnum::Second);
			SE->Delete((int)SoundEnum::OneMunute);
			SE->Delete((int)SoundEnum::TimeUp);
		}
		//
		void Cam2DControl::Set(void) noexcept {}
		void Cam2DControl::Update(void) noexcept {}
		void Cam2DControl::Dispose(void) noexcept {}
		//
		const Vector3DX Convert2DtoDisp(const Vector3DX& Pos2D) {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			Vector3DX Ret;

			Ret.x = y_r(1920 / 2 + (1080 / 2) * (Pos2D.x - CamPos.x) / CamPos.z);
			Ret.y = y_r(1080 / 2 - (1080 / 2) * (Pos2D.y - CamPos.y) / CamPos.z);
			Ret.z = Pos2D.z;

			return Ret;
		}
	};
};
