#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {

	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{ 127 };
		PlayerID				DamageID{ 127 };
		HitPoint				Damage{ 0 };
		int						HitType{};
		Vector3DX				StartPos;
		Vector3DX				EndPos;
	public:
		void Set(PlayerID shotID, PlayerID damageID, HitPoint damage, int hitType, const Vector3DX& startPos, const Vector3DX& endPos) noexcept {
			this->ShotID = shotID;
			this->DamageID = damageID;
			this->Damage = damage;
			this->HitType = hitType;
			this->StartPos = startPos;
			this->EndPos = endPos;
		}
	};
	class DamageEventControl {
		std::array<DamageEvent, 16>							m_Damage;
		uint8_t												m_DamageCount{ 0 };
	public:
		//データの総数リセット
		void			Reset(void) noexcept {
			this->m_DamageCount = 0;
		}
		//データを追加
		void			Add(PlayerID shotID, PlayerID damageID, HitPoint damage, int hitType, const Vector3DX& startPos, const Vector3DX& endPos) noexcept {
			this->m_Damage[this->m_DamageCount].Set(shotID, damageID, damage, hitType, startPos, endPos);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
		//データを引数のVectorに引き渡す
		void			Pop(std::vector<DamageEvent>* pRet) noexcept {
#if defined(DEBUG)
			if (!pRet) { return; }
#endif
			for (uint8_t loop = 0; loop < this->m_DamageCount; ++loop) {
				pRet->emplace_back(this->m_Damage[loop]);
			}
			Reset();
		}
	};
	//
	class FadeControl : public SingletonBase<FadeControl> {
	private:
		friend class SingletonBase<FadeControl>;
	private:
		bool						m_IsBlackOut{ false };//カットシーン中フラグ
		float						m_BlackOutAlpha{ 0.0f };
		float						m_BlackSpeed{ 0.5f };
	private:
		FadeControl() noexcept {}
		virtual ~FadeControl()noexcept {}
	public:
		auto		IsClear(void) const noexcept { return this->m_BlackOutAlpha == 0.0f; }
		auto		IsAll(void) const noexcept { return this->m_BlackOutAlpha >= 1.0f; }
	public:
		void			SetBlackOut(bool value, float speed = 0.5f) noexcept {
			this->m_IsBlackOut = value;
			this->m_BlackSpeed = speed;
		}
	public:
		void			Init(void) noexcept;
		void			Update(void) noexcept;
		void			Draw(void) const noexcept;
	};

	class ScreenPosition {
		int	m_XPos{};
		int	m_YPos{};
		int	m_XScreenPos{};
		int	m_YScreenPos{};
	public:
		ScreenPosition(void) noexcept {}
		virtual ~ScreenPosition(void) noexcept {}
	public:
		const int XPos(void) const noexcept { return this->m_XPos; }
		const int YPos(void) const noexcept { return this->m_YPos; }
		const int XScreenPos(void) const noexcept { return this->m_XScreenPos; }
		const int YScreenPos(void) const noexcept { return this->m_YScreenPos; }
	public:
		bool Calc(const Vector3DX& Pos3D) noexcept {
			Vector3DX Pos = ConvWorldPosToScreenPos(Pos3D.get());
			if (0.0f < Pos.z && Pos.z < 1.0f) {
				auto* WindowSizeParts = WindowSizeControl::Instance();
				this->m_XPos = static_cast<int>(Pos.x);
				this->m_YPos = static_cast<int>(Pos.y);
				this->m_XScreenPos = this->m_XPos * 1980 / WindowSizeParts->GetScreenY(1980);
				this->m_YScreenPos = this->m_YPos * 1080 / WindowSizeParts->GetScreenY(1080);
				return true;
			}
			return false;
		}
	};

	class HitMarkerPool : public SingletonBase<HitMarkerPool> {
	private:
		friend class SingletonBase<HitMarkerPool>;
	private:
		class HitMarker {
			bool			m_IsActive{ false };
			bool			m_IsDraw{ false };
			float			m_Hit_alpha{ 0.0f };
			ScreenPosition	m_Hit_DispPos;
			int				m_Hit_AddX{ 0 };
			int				m_Hit_AddY{ 0 };
			HitPoint		m_Damage{ 0 };
			ArmerPoint		m_ArmerDamage{ 0 };
			float			m_HitTimer{ 0.0f };
			Vector3DX		pos;
		public:
			void			AddMarker(const Vector3DX& position, HitPoint Damage, ArmerPoint ArmerDamage) noexcept {
				this->pos = position;
				this->m_Damage = Damage;
				this->m_ArmerDamage = ArmerDamage;
				this->m_HitTimer = 0.25f;
				this->m_Hit_alpha = 1.0f;
				this->m_Hit_AddX = GetRand(32);
				this->m_Hit_AddY = GetRand(32);
				this->m_IsActive = true;
			}
		public: //コンストラクタ、デストラクタ
			HitMarker(void) noexcept {}
			virtual ~HitMarker(void) noexcept {}
		public: //継承
			void			Init(void) noexcept {
				this->m_IsActive = false;
			}
			void			Update(void) noexcept {
				if (!this->m_IsActive) { return; }
				auto* DXLib_refParts = DXLib_ref::Instance();
				this->m_HitTimer = std::clamp(this->m_HitTimer - DXLib_refParts->GetDeltaTime(), 0.0f, 0.25f);
				if (this->m_Hit_alpha > 0.0f) {
					Easing(&this->m_Hit_alpha, (this->m_HitTimer > 0.0f) ? 2.0f : 0.0f, 0.95f, EasingType::OutExpo);
					if (this->m_Hit_alpha <= 0.01f) {
						this->m_Hit_alpha = 0;
						this->m_IsActive = false;
					}
				}
				this->m_IsDraw = false;
			}
			void			CheckDraw(void) noexcept {
				if (!this->m_IsActive) { return; }
				this->m_IsDraw |= this->m_Hit_DispPos.Calc(this->pos);
			}
			bool			Draw(const GraphHandle& hit_Graph, const GraphHandle& guard_Graph) const noexcept {
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				if (this->m_IsActive && this->m_IsDraw) {
					int			Alpha = static_cast<int>(this->m_Hit_alpha * 255.0f);
					if (Alpha >= 10) {
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
						//
						int red = static_cast<int>(255 * std::clamp(static_cast<float>(this->m_Damage) / 100.0f * 2.0f, 0.0f, 1.0f));
						int green = 255 - red;
						if (this->m_Damage > 0) {
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, red, green, 0);
							DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &hit_Graph, static_cast<int>(this->m_Hit_DispPos.XScreenPos()), static_cast<int>(this->m_Hit_DispPos.YScreenPos()), static_cast<float>(static_cast<int>(static_cast<float>(Alpha) / 255.0f * 0.5f * 100.0f)) / 100.0f, 0.0f, true);
						}
						if (this->m_ArmerDamage > 0) {
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
							DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &guard_Graph, static_cast<int>(this->m_Hit_DispPos.XScreenPos()), static_cast<int>(this->m_Hit_DispPos.YScreenPos()), static_cast<float>(static_cast<int>(static_cast<float>(Alpha) / 255.0f * 0.5f * 100.0f)) / 100.0f, 0.0f, true);
						}
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						//
						if (this->m_Damage > 0) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
								24, FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								static_cast<int>(this->m_Hit_DispPos.XScreenPos() + this->m_Hit_AddX), static_cast<int>(this->m_Hit_DispPos.YScreenPos() + this->m_Hit_AddY), GetColor(red, green, 0), Black, "%d", this->m_Damage);
						}
						//防いだダメージ
						if (this->m_ArmerDamage > 0) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
								20, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
								static_cast<int>(this->m_Hit_DispPos.XScreenPos() + this->m_Hit_AddX) - 10, static_cast<int>(this->m_Hit_DispPos.YScreenPos() + this->m_Hit_AddY), Gray50, Black, "%d", this->m_ArmerDamage);
						}
						return true;
					}
				}
				return false;
			}
		};
	private:
		std::array<HitMarker, 64>	m_HitMarkerList;
		int							m_Now{ 0 };
		GraphHandle					hit_Graph;
		GraphHandle					guard_Graph;
	private:
		HitMarkerPool(void) noexcept { Init(); }
		virtual ~HitMarkerPool(void) noexcept { Dispose(); }
	public:
		void			AddMarker(const Vector3DX& position, HitPoint Damage, ArmerPoint ArmerDamage) noexcept {
			this->m_HitMarkerList[this->m_Now].AddMarker(position, Damage, ArmerDamage);
			++this->m_Now %= static_cast<int>(this->m_HitMarkerList.size());
		}
	public:
		void			Init(void) noexcept {
			this->hit_Graph.Load("data/UI/battle_hit.bmp");
			this->guard_Graph.Load("data/UI/battle_guard.bmp");
			for (auto& hitmarker : this->m_HitMarkerList) {
				hitmarker.Init();
			}
		}
		void			Update(void) noexcept {
			for (auto& hitmarker : this->m_HitMarkerList) {
				hitmarker.Update();
			}
		}
		void			Check(void) noexcept {
			for (auto& hitmarker : this->m_HitMarkerList) {
				hitmarker.CheckDraw();
			}
		}
		void			Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			bool isDraw = false;
			for (auto& hitmarker : this->m_HitMarkerList) {
				isDraw |= hitmarker.Draw(this->hit_Graph, this->guard_Graph);
			}
			if (isDraw) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
		void			Dispose(void) noexcept {
			this->hit_Graph.Dispose();
			this->guard_Graph.Dispose();
		}
	};

	class EffectSingleton : public SingletonBase<EffectSingleton> {
	private:
		friend class SingletonBase<EffectSingleton>;
	private:
		EffectSingleton(void) noexcept {
			this->m_EffectControl.Init();
		}
		virtual ~EffectSingleton(void) noexcept {
			this->m_EffectControl.Dispose();
		}
	private:
		EffectControl								m_EffectControl;
	public:
		// 複数エフェクトの再生
		void		SetOnce_Any(Effect ID, const Vector3DX& pos, const Vector3DX& nomal, float scale = 1.0f, float speed = 1.0f) noexcept {
			this->m_EffectControl.SetOnce_Any(static_cast<EffectControl::EffectID>(ID), pos, nomal, scale, speed);
		}
		// 単体で制御したいエフェクトの制御
		void		StopEffect(Effect ID) noexcept {
			this->m_EffectControl.StopEffect(static_cast<EffectControl::EffectID>(ID));
		}
		void		SetLoop(Effect ID, const Vector3DX& pos) noexcept {
			this->m_EffectControl.SetLoop(static_cast<EffectControl::EffectID>(ID), pos);
		}
		void		Update_LoopEffect(Effect ID, const Vector3DX& pos, const Vector3DX& nomal, float scale = 1.0f) noexcept {
			this->m_EffectControl.Update_LoopEffect(static_cast<EffectControl::EffectID>(ID), pos, nomal, scale);
		}

		void		StopEffectAny(Effect ID, int UniqueID) noexcept {
			this->m_EffectControl.StopEffectAny(static_cast<EffectControl::EffectID>(ID), UniqueID);
		}
		int 		SetLoopAny(Effect ID, const Vector3DX& pos) noexcept {
			return this->m_EffectControl.SetLoopAny(static_cast<EffectControl::EffectID>(ID), pos);
		}
		void		Update_LoopAnyEffect(Effect ID, int UniqueID, const Vector3DX& pos, const Vector3DX& nomal, float scale = 1.0f) noexcept {
			this->m_EffectControl.Update_LoopAnyEffect(static_cast<EffectControl::EffectID>(ID), UniqueID, pos, nomal, scale);
		}

		void		SetOnce(Effect ID, const Vector3DX& pos, const Vector3DX& nomal, float scale = 1.0f) noexcept {
			this->m_EffectControl.SetOnce(static_cast<EffectControl::EffectID>(ID), pos, nomal, scale);
		}
		void		SetEffectSpeed(Effect ID, float speed) noexcept {
			this->m_EffectControl.SetEffectSpeed(static_cast<EffectControl::EffectID>(ID), speed);
		}
		void		SetEffectColor(Effect ID, int red, int green, int blue, int alpha) noexcept {
			this->m_EffectControl.SetEffectColor(static_cast<EffectControl::EffectID>(ID), red, green, blue, alpha);
		}
	public:
		void			Update(void) noexcept {
			this->m_EffectControl.Update();
		}
	};
}