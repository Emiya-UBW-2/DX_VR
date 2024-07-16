#include	"sub.hpp"

#include	"MainScene/Player/Player.hpp"
#include	"CommonScene/Object/Object2DManager.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::Cam2DControl* SingletonBase<FPS_n2::Sceneclass::Cam2DControl>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::Effect2DControl* SingletonBase<FPS_n2::Sceneclass::Effect2DControl>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add(static_cast<int>(SoundEnum::RunFoot), 6, "data/Sound/SE/move/runfoot.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Shot1), 6, "data/Sound/SE/move/shot1.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Shot2), 6, "data/Sound/SE/move/shot2.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Shot3), 6, "data/Sound/SE/move/shot3.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Bomb), 1, "data/Sound/SE/move/bomb.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Hit), 6, "data/Sound/SE/move/hit.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Guard), 6, "data/Sound/SE/move/guard.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Normal), 2, "data/Sound/SE/Normal.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			SE->Get(static_cast<int>(SoundEnum::RunFoot)).SetVol_Local(128);
			SE->Get(static_cast<int>(SoundEnum::Normal)).SetVol_Local(128);

			SE->Get(static_cast<int>(SoundEnum::Shot1)).SetVol_Local(64);
			SE->Get(static_cast<int>(SoundEnum::Shot2)).SetVol_Local(64);
			SE->Get(static_cast<int>(SoundEnum::Shot3)).SetVol_Local(64);
			SE->Get(static_cast<int>(SoundEnum::Bomb)).SetVol_Local(64);
			SE->Get(static_cast<int>(SoundEnum::Hit)).SetVol_Local(64);
			SE->Get(static_cast<int>(SoundEnum::Guard)).SetVol_Local(64);

			SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete(static_cast<int>(SoundEnum::RunFoot));
			SE->Delete(static_cast<int>(SoundEnum::Shot1));
			SE->Delete(static_cast<int>(SoundEnum::Shot2));
			SE->Delete(static_cast<int>(SoundEnum::Shot3));
			SE->Delete(static_cast<int>(SoundEnum::Bomb));
			SE->Delete(static_cast<int>(SoundEnum::Hit));
			SE->Delete(static_cast<int>(SoundEnum::Guard));
			SE->Delete(static_cast<int>(SoundEnum::Normal));
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
		const Vector3DX Cam2DControl::GetCamPos(void) const noexcept {
			auto Shake = CameraShake::Instance()->GetCamShake();Shake.z = 0.f;
			return this->m_Pos + Shake;
		}
		// 
		void Convert2DtoDisp(const Vector3DX& Pos2D, Vector3DX* pRet) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			pRet->x = static_cast<float>(DrawParts->GetScreenY(static_cast<int>(1920 / 2 + (1080 / 2) * (Pos2D.x - CamPos.x) * CamPos.z / Base_CamScale)));
			pRet->y = static_cast<float>(DrawParts->GetScreenY(static_cast<int>(1080 / 2 - (1080 / 2) * (Pos2D.y - CamPos.y) * CamPos.z / Base_CamScale)));
			pRet->z = Pos2D.z;
		}

		bool Is2DPositionInDisp(const Vector3DX& Pos2D, int Radius) noexcept
		{
			auto* DrawParts = DXDraw::Instance();
			Radius = Radius * 100 / DrawParts->GetScreenY(100);
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			int x = 1920 / 2 + static_cast<int>((1080.f / 2.f) * (Pos2D.x - CamPos.x) * CamPos.z / Base_CamScale);
			int y = 1080 / 2 - static_cast<int>((1080.f / 2.f) * (Pos2D.y - CamPos.y) * CamPos.z / Base_CamScale);
			return HitPointToRectangle(x, y, -Radius, -Radius, 1920 + Radius, 1080 + Radius);
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
			auto* DrawParts = DXDraw::Instance();
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			return DrawParts->GetScreenY(static_cast<int>(value * Tile_DispSize * CamPos.z));
		}
		void Effect2DControl::Init() noexcept {
			for (auto& g : m_GuardPos) {
				g.m_Per = 0.f;
			}
			m_GuardPosNum = 0;
		}
		void Effect2DControl::Update() noexcept {
			auto* DrawParts = DXDraw::Instance();
			for (auto& g : m_GuardPos) {
				g.m_Per = std::max(g.m_Per - 1.f / DrawParts->GetFps() / 0.5f, 0.f);
			}
		}
		void Effect2DControl::Draw() noexcept {
			float Radius = static_cast<float>(GetDispSize(5.f));
			Vector3DX DispPos;
			for (auto& g : m_GuardPos) {
				if (g.m_Per <= 0.f) { continue; }
				Convert2DtoDisp(g.m_Pos, &DispPos);
				switch (g.m_EffectType) {
					case EffectType::Damage:
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(128.f * (g.m_Per / g.m_PerMax)), 0, 255));
						DrawCircleAA(DispPos.x, DispPos.y, Radius * g.m_PerMax * std::pow(1.f - (g.m_Per / g.m_PerMax), 0.5f), 8, GetColor(192, 0, 0), TRUE, 1.f, static_cast<double>(deg2rad(360.f*g.m_Per)));
						break;
					case EffectType::Guard:
						SetDrawBlendMode(DX_BLENDMODE_ADD, std::clamp(static_cast<int>(128.f * (g.m_Per / g.m_PerMax)), 0, 255));
						DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(Radius * g.m_PerMax * std::pow(1.f - (g.m_Per / g.m_PerMax), 0.5f)), GetColor(150, 150, 255));
						break;
					case EffectType::WallHit:
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(128.f * (g.m_Per / g.m_PerMax)), 0, 255));
						DrawCircleAA(DispPos.x, DispPos.y, Radius * g.m_PerMax * std::pow(1.f - (g.m_Per / g.m_PerMax), 0.5f), 5, GetColor(255, 192, 0), TRUE, 1.f, static_cast<double>(deg2rad(360.f*g.m_Per)));
						break;
					default:
						break;
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	};
};
