#include	"sub.hpp"

#include	"MainScene/Player/Player.hpp"
#include	"CommonScene/Object/Object2DManager.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::Cam2DControl* SingletonBase<FPS_n2::Sceneclass::Cam2DControl>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
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
			SE->Delete((int)SoundEnum::RunFoot);
			SE->Delete((int)SoundEnum::CountDown);
			SE->Delete((int)SoundEnum::Second);
			SE->Delete((int)SoundEnum::OneMunute);
			SE->Delete((int)SoundEnum::TimeUp);
		}
		void CommonBattleResource::AddCharacter(PlayerID value) noexcept 
		{
			auto* BackGround = BackGroundClassBase::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			p.SetChara(std::make_shared<CharacterObject>());
			Obj2DParts->AddObject(p.GetChara());
			p.GetChara()->SetObjType((int)Object2DType::Human);
			p.GetChara()->SetPlayerID(value);
			p.SetAI(std::make_shared<AIControl>());
			p.GetAI()->SetPlayerID(value);
			p.GetAI()->Init();
		}
		void CommonBattleResource::DelCharacter(PlayerID value) noexcept 
		{
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			Obj2DParts->DelObj(p.GetChara());
			p.Dispose();
		}
		// 
		const Vector3DX Convert2DtoDisp(const Vector3DX& Pos2D) noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			Vector3DX Ret;

			Ret.x = (float)y_r(1920 / 2 + (1080 / 2) * (Pos2D.x - CamPos.x) / Base_CamScale);
			Ret.y = (float)y_r(1080 / 2 - (1080 / 2) * (Pos2D.y - CamPos.y) / Base_CamScale);
			Ret.z = Pos2D.z;

			return Ret;
		}

		const float GetRadVec2Vec(const Vector3DX& vec1, const Vector3DX& vec2) noexcept { return std::atan2f(vec1.x - vec2.x, vec1.y - vec2.y); }

		// 空間上のタイルごとのサイズを取得(タイルvalue個ぶん)
		const float Get2DSize(float value) noexcept {
			return (value * Tile_DispSize) * Base_CamScale / (1080 / 2);
		}
		// 画面上のタイルごとのサイズを取得(タイルvalue個ぶん)
		const int GetDispSize(float value) noexcept {
			return y_r(value * Tile_DispSize);
		}
	};
};
