#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerControl {
		private:
			std::shared_ptr<CharacterClass>	m_Chara{ nullptr };
			std::shared_ptr<VehicleClass>	m_Vehicle{ nullptr };
			float							m_Score{ 0.f };							//スコア
		public:
			PlayerControl(void) noexcept {
				this->m_Score = 0.f;
			}
			~PlayerControl(void) noexcept {
				this->Dispose();
			}
		public:
			void		SetChara(const std::shared_ptr<CharacterClass>& pChara) noexcept { m_Chara = pChara; }
			void		SetVehicle(const std::shared_ptr<VehicleClass>& pVehicle) noexcept { m_Vehicle = pVehicle; }
			void		AddScore(float value) noexcept { this->m_Score += value; }
			void		SubScore(float value) noexcept { this->m_Score -= value; }
			void		SetScore(float value) noexcept { this->m_Score = value; }
			auto&		GetChara(void) noexcept { return m_Chara; }
			auto&		GetVehicle(void) noexcept { return m_Vehicle; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }
			const auto	IsRide(void) const noexcept { return (bool)m_Vehicle; }

			const auto		GetNetSendMove(void) const noexcept {
				struct SendInfo {
					VECTOR_ref			m_Pos;
					VECTOR_ref			m_Vec;
					float				m_Yrad{ 0.f };
					const DamageEvent*	m_Damage{ nullptr };
					char				m_DamageSwitch{ 0 };
				};
				SendInfo ans;
				if (!IsRide()) {
					ans.m_Pos = m_Chara->GetMove().pos;
					ans.m_Vec = m_Chara->GetMove().vec;
					ans.m_Vec.y(0);
					ans.m_Yrad = m_Chara->GetRadBuf().y();
					ans.m_Damage = nullptr;
					ans.m_DamageSwitch = 0;
				}
				else {
					ans.m_Pos = m_Vehicle->GetMove().pos;
					ans.m_Vec = m_Vehicle->GetMove().vec;
					ans.m_Yrad = m_Vehicle->Get_body_yrad();
					ans.m_Damage = &m_Vehicle->GetDamageEvent();
					ans.m_DamageSwitch = (m_Vehicle->GetDamageSwitch() ? 1 : 0);
				}
				return ans;
			}

			const auto		GetPos(void) const noexcept { return ((!IsRide()) ? m_Chara->GetMatrix().pos() : m_Vehicle->GetMatrix().pos()); }
			const auto		GetAim(void) const noexcept { return ((!IsRide()) ? m_Chara->GetCharaDir().zvec()*-1.f : m_Vehicle->GetLookVec().zvec()*-1.f); }
			const auto		GetRadBuf(void) const noexcept { return ((!IsRide()) ? m_Chara->GetRadBuf() : m_Vehicle->GetViewRad()); }
		public:
			void Init(void) noexcept {
				m_Chara = nullptr;
				m_Vehicle = nullptr;
			}

			void Dispose(void) noexcept {
				m_Chara = nullptr;
				m_Vehicle = nullptr;
			}
		};

		class PlayerManager {
			std::vector<PlayerControl> m_Player;
		private:
			//シングルトン化
#if true

		private:
			static inline  PlayerManager*	m_Singleton = nullptr;
		public:
			static void Create() noexcept {
				if (m_Singleton == nullptr) {
					m_Singleton = new PlayerManager();
				}
			}
			static PlayerManager* Instance(void) noexcept {
				if (m_Singleton == nullptr) {
					m_Singleton = new PlayerManager();
				}
				return m_Singleton;
			}
		private:

#endif
		public:
			auto&		GetPlayer(int ID) noexcept { return m_Player[ID]; }
		public:
			void Init(int playerNum) noexcept {
				m_Player.resize(playerNum);
			}
			void Dispose(void) noexcept {
				m_Player.clear();
			}
		};

	};
};
