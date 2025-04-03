#pragma once
#include	"Header.hpp"

#include "MainScene/Object/CharacterEnum.hpp"
//
namespace FPS_n2 {

	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{ 127 };
		PlayerID				DamageID{ 127 };
		HitPoint				Damage{ 0 };
		ArmerPoint				ArmerDamage{ 0 };
		int						m_HitType{};
		Vector3DX				m_StartPos;
		Vector3DX				m_EndPos;
	public:
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage, int HitType, const Vector3DX& StartPos, const Vector3DX& EndPos) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
			this->ArmerDamage = pArmerDamage;
			this->m_HitType = HitType;
			this->m_StartPos = StartPos;
			this->m_EndPos = EndPos;
		}
	};
	class DamageEventControl {
		std::array<DamageEvent, 16>							m_Damage;
		uint8_t												m_DamageCount{ 0 };
	public:
		const auto& GetDamageEvent(void) const noexcept { return this->m_Damage; }
		const auto& GetDamageEventCount(void) const noexcept { return this->m_DamageCount; }
	public:
		void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept {
			if (!pRet) { return; }
			for (auto& e : this->m_Damage) {
				if ((&e - &this->m_Damage.front()) >= this->m_DamageCount) { break; }
				pRet->emplace_back(e);
			}
			this->m_DamageCount = 0;
		}
	public:
		void			Reset(void) noexcept {
			this->m_DamageCount = 0;
		}
		void			Add(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage, int HitType, const Vector3DX& StartPos, const Vector3DX& EndPos) noexcept {
			this->m_Damage.at(this->m_DamageCount).SetEvent(ShotID_t, DamageID_t, pDamage, pArmerDamage, HitType, StartPos, EndPos);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
	};
	//
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		};
	};


	class FadeControl {
		bool						m_IsBlackOut{ false };//カットシーン中フラグ
		float						m_BlackOutAlpha{ 0.f };
	public:
		auto		IsClear(void) const noexcept { return this->m_BlackOutAlpha == 0.f; }
		auto		IsAll(void) const noexcept { return this->m_BlackOutAlpha >= 1.f; }
	public:
		void			SetBlackOut(bool value) noexcept { this->m_IsBlackOut = value; }
	public:
		void			Init(void) noexcept;
		void			Update(void) noexcept;
		void			Draw(void) const noexcept;
	};

	class ScreenPosition {
		int m_XPos{};
		int m_YPos{};
		int m_XScreenPos{};
		int m_YScreenPos{};
	public:
		ScreenPosition(void) noexcept {}
		~ScreenPosition(void) noexcept {}
	public:
		const int XPos(void) const noexcept { return this->m_XPos; }
		const int YPos(void) const noexcept { return this->m_YPos; }
		const int XScreenPos(void) const noexcept { return this->m_XScreenPos; }
		const int YScreenPos(void) const noexcept { return this->m_YScreenPos; }
	public:
		bool Calc(const Vector3DX& Pos3D) noexcept {
			Vector3DX Pos = ConvWorldPosToScreenPos(Pos3D.get());
			if (0.f < Pos.z && Pos.z < 1.f) {
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

	class HitMarker {
		bool			m_IsActive{ false };
		bool			m_IsDraw{ false };
		float			m_Hit_alpha{ 0.f };
		ScreenPosition	m_Hit_DispPos;
		int				m_Hit_AddX{ 0 };
		int				m_Hit_AddY{ 0 };
		HitPoint		m_Damage{ 0 };
		ArmerPoint		m_ArmerDamage{ 0 };
		float			m_HitTimer{ 0.f };
		Vector3DX		pos;
	public:
		void			AddMarker(const Vector3DX& position, HitPoint Damage, ArmerPoint ArmerDamage) noexcept {
			this->pos = position;
			this->m_Damage = Damage;
			this->m_ArmerDamage = ArmerDamage;
			this->m_HitTimer = 0.25f;
			this->m_Hit_alpha = 1.f;
			this->m_Hit_AddX = GetRand(32);
			this->m_Hit_AddY = GetRand(32);
			this->m_IsActive = true;
		}
	public: //コンストラクタ、デストラクタ
		HitMarker(void) noexcept {}
		~HitMarker(void) noexcept {}
	public: //継承
		void			Init() noexcept {
			this->m_IsActive = false;
		}
		void			Update(void) noexcept {
			if (!this->m_IsActive) { return; }
			auto* DXLib_refParts = DXLib_ref::Instance();
			this->m_HitTimer = std::clamp(this->m_HitTimer - DXLib_refParts->GetDeltaTime(), 0.f, 0.25f);
			if (this->m_Hit_alpha > 0.f) {
				Easing(&this->m_Hit_alpha, (this->m_HitTimer > 0.f) ? 2.f : 0.f, 0.95f, EasingType::OutExpo);
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
		void			Draw(const GraphHandle& hit_Graph, const GraphHandle& guard_Graph) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			if (this->m_IsActive && this->m_IsDraw) {
				int			Alpha = static_cast<int>(this->m_Hit_alpha * 255.f);
				if (Alpha >= 10) {
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
					//
					int r = static_cast<int>(255 * std::clamp((float)this->m_Damage / 100.f * 2.f, 0.f, 1.f));
					int g = 255 - r;
					if (this->m_Damage > 0) {
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, r, g, 0);
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &hit_Graph, static_cast<int>(this->m_Hit_DispPos.XScreenPos()), static_cast<int>(this->m_Hit_DispPos.YScreenPos()), (float)static_cast<int>((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
					}
					if (this->m_ArmerDamage > 0) {
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &guard_Graph, static_cast<int>(this->m_Hit_DispPos.XScreenPos()), static_cast<int>(this->m_Hit_DispPos.YScreenPos()), (float)static_cast<int>((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
					}
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					//
					if (this->m_Damage > 0) {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
							24, FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							static_cast<int>(this->m_Hit_DispPos.XScreenPos() + this->m_Hit_AddX), static_cast<int>(this->m_Hit_DispPos.YScreenPos() + this->m_Hit_AddY), GetColor(r, g, 0), Black, "%d", this->m_Damage);
					}
					//防いだダメージ
					if (this->m_ArmerDamage > 0) {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
							20, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
							static_cast<int>(this->m_Hit_DispPos.XScreenPos() + this->m_Hit_AddX) - 10, static_cast<int>(this->m_Hit_DispPos.YScreenPos() + this->m_Hit_AddY), Gray50, Black, "%d", this->m_ArmerDamage);
					}
				}
			}
		}
	};
	class HitMarkerPool : public SingletonBase<HitMarkerPool> {
	private:
		friend class SingletonBase<HitMarkerPool>;
	private:
		HitMarkerPool() { Init(); }
		virtual ~HitMarkerPool() { Dispose(); }
	private:
		std::array<HitMarker, 64>	m_HitMarkerList;
		int							m_Now{ 0 };
		GraphHandle					hit_Graph;
		GraphHandle					guard_Graph;
	public:
		void			AddMarker(const Vector3DX& position, HitPoint Damage, ArmerPoint ArmerDamage) noexcept {
			this->m_HitMarkerList.at(this->m_Now).AddMarker(position, Damage, ArmerDamage);
		}
	public:
		void			Init(void) noexcept {
			this->hit_Graph.Load("data/UI/battle_hit.bmp");
			this->guard_Graph.Load("data/UI/battle_guard.bmp");
			for (auto& h : this->m_HitMarkerList) {
				h.Init();
			}
		}
		void			Update(void) noexcept {
			for (auto& h : this->m_HitMarkerList) {
				h.Update();
			}
		}
		void			Check(void) noexcept {
			for (auto& h : this->m_HitMarkerList) {
				h.CheckDraw();
			}
		}
		void			Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			for (auto& h : this->m_HitMarkerList) {
				h.Draw(this->hit_Graph, this->guard_Graph);
			}
			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
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
		EffectSingleton() { Init(); }
		virtual ~EffectSingleton() { Dispose(); }
	private:
		EffectControl								m_EffectControl;
	public:
		// 複数エフェクトの再生
		void		SetOnce_Any(Sceneclass::Effect ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f, float speed = 1.f) noexcept {
			this->m_EffectControl.SetOnce_Any(static_cast<int>(ID), pos_t, nomal_t, scale, speed);
		}
		// 単体で制御したいエフェクトの制御
		void		SetLoop(Sceneclass::Effect ID, const Vector3DX& pos_t) noexcept {
			this->m_EffectControl.SetLoop(static_cast<int>(ID), pos_t);
		}
		void		Update_LoopEffect(Sceneclass::Effect ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			this->m_EffectControl.Update_LoopEffect(static_cast<int>(ID), pos_t, nomal_t, scale);
		}
		void		SetOnce(Sceneclass::Effect ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			this->m_EffectControl.SetOnce(static_cast<int>(ID), pos_t, nomal_t, scale);
		}
		void		SetEffectSpeed(Sceneclass::Effect ID, float value) noexcept {
			this->m_EffectControl.SetEffectSpeed(static_cast<int>(ID), value);
		}
		void		SetEffectColor(Sceneclass::Effect ID, int r, int g, int b, int a) noexcept {
			this->m_EffectControl.SetEffectColor(static_cast<int>(ID), r, g, b, a);
		}
	public:
		void			Init(void) noexcept {
			this->m_EffectControl.Init();
		}
		void			Update(void) noexcept {
			this->m_EffectControl.Execute();
		}
		void			Dispose(void) noexcept {
			this->m_EffectControl.Dispose();
		}
	};

};
