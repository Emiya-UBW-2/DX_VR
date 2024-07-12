#include	"sub.hpp"

#include	"MainScene/Player/Player.hpp"
#include	"CommonScene/Object/Object2DManager.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::Cam2DControl* SingletonBase<FPS_n2::Sceneclass::Cam2DControl>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add(static_cast<int>(SoundEnum::RunFoot), 6, "data/Sound/SE/move/runfoot.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Normal), 2, "data/Sound/SE/Normal.wav", false);
			SE->Add(static_cast<int>(SoundEnum::CountDown), 2, "data/Sound/SE/CountDown.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Second), 1, "data/Sound/SE/second.wav", false);
			SE->Add(static_cast<int>(SoundEnum::OneMunute), 1, "data/Sound/SE/OneMinute.wav", false);
			SE->Add(static_cast<int>(SoundEnum::TimeUp), 1, "data/Sound/SE/TimeUp.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			SE->Get(static_cast<int>(SoundEnum::RunFoot)).SetVol_Local(128);
			SE->Get(static_cast<int>(SoundEnum::CountDown)).SetVol_Local(128);

			SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete(static_cast<int>(SoundEnum::RunFoot));
			SE->Delete(static_cast<int>(SoundEnum::Normal));
			SE->Delete(static_cast<int>(SoundEnum::CountDown));
			SE->Delete(static_cast<int>(SoundEnum::Second));
			SE->Delete(static_cast<int>(SoundEnum::OneMunute));
			SE->Delete(static_cast<int>(SoundEnum::TimeUp));
		}
		void CommonBattleResource::AddCharacter(PlayerID value) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);

			const auto& Obj = std::make_shared<CharacterObject>();
			p->SetChara(Obj);
			Obj2DParts->AddObject(Obj);
			Obj->SetObjType(static_cast<int>(Object2DType::Human));
			Obj->SetPlayerID(value);
			p->SetAI(std::make_shared<AIControl>());
			p->GetAI()->SetPlayerID(value);
			p->GetAI()->Init();
		}
		void CommonBattleResource::DelCharacter(PlayerID value) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			Obj2DParts->DelObj(p->GetChara());
			p->GetAI()->Dispose();
			p->Dispose();
		}
		// 
		void Convert2DtoDisp(const Vector3DX& Pos2D, Vector3DX* pRet) noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			pRet->x = static_cast<float>(y_r(1920 / 2 + (1080 / 2) * (Pos2D.x - CamPos.x) / Base_CamScale));
			pRet->y = static_cast<float>(y_r(1080 / 2 - (1080 / 2) * (Pos2D.y - CamPos.y) / Base_CamScale));
			pRet->z = Pos2D.z;
		}

		float GetRadVec2Vec(const Vector3DX& vec1, const Vector3DX& vec2) noexcept { return std::atan2f(vec1.x - vec2.x, vec1.y - vec2.y); }

		// 空間上のタイルごとのサイズを取得(タイルvalue個ぶん)
		float Get2DSize(float value) noexcept {
			return (value * Tile_DispSize) * Base_CamScale / (1080.f / 2.f);
		}
		// 空間上のサイズからタイル枚数を取得
		float Get2DSizetoTile(float value) noexcept {
			return (value * (1080.f / 2.f) / Base_CamScale) / Tile_DispSize;
		}
		// 画面上のタイルごとのサイズを取得(タイルvalue個ぶん)
		int GetDispSize(float value) noexcept {
			return y_r(value * Tile_DispSize);
		}
	};
};
