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
		int8_t					m_HitType{};
		Vector3DX				m_Pos;
	public:
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, HitType pHitType, const Vector3DX& pPos) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
			this->m_HitType = static_cast<int8_t>(pHitType);
			this->m_Pos = pPos;
		}
	public:
		HitType GetHitType(void) const noexcept { return static_cast<HitType>(m_HitType); }
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
		void			Add(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, HitType pHitType, const Vector3DX& pPos) noexcept {
			this->m_Damage.at(this->m_DamageCount).SetEvent(ShotID_t, DamageID_t, pDamage, pHitType, pPos);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
	};
	//
	class CommonBattleResource : public SingletonBase<CommonBattleResource> {
	private:
		friend class SingletonBase<CommonBattleResource>;
	private:
		CommonBattleResource(void) noexcept {}
		CommonBattleResource(const CommonBattleResource&) = delete;
		CommonBattleResource(CommonBattleResource&& o) = delete;
		CommonBattleResource& operator=(const CommonBattleResource&) = delete;
		CommonBattleResource& operator=(CommonBattleResource&& o) = delete;

		virtual ~CommonBattleResource(void) noexcept {}
	public:
		void			Load(void) noexcept;
		void			Set(void) noexcept;
		void			Dispose(void) noexcept;
	public:
		void			LoadShip(const std::string& FolderPath, PlayerID ID) noexcept;
		void			LoadShipNPC(const std::string& FolderPath, int Num) noexcept;
	};

	class HitMark : public SingletonBase<HitMark> {
	private:
		friend class SingletonBase<HitMark>;
	private:
		struct HitPos
		{
			float Time{};
			float TimeMax{};
			Vector3DX	m_Pos2D;
			Vector3DX	m_Pos;
			HitType		m_Color{};
			float		m_Per{};
		};
	private:
		GraphHandle										MenGraph;
		GraphHandle										KoteGraph;
		GraphHandle										DoGraph;
		std::array<HitPos, 16>							m_HitPos;
		size_t											m_HitNow{ 0 };
	private:
		HitMark(void) noexcept {}
		HitMark(const HitMark&) = delete;
		HitMark(HitMark&& o) = delete;
		HitMark& operator=(const HitMark&) = delete;
		HitMark& operator=(HitMark&& o) = delete;

		virtual ~HitMark(void) noexcept {}
	public:
		void			Load(void) noexcept;
		void			Set(void) noexcept;
		void			Update(void) noexcept;
		void			Draw(void) noexcept;
		void			Dispose(void) noexcept;
	public:
		void			Add(const Vector3DX& Pos, HitType color, float Per) noexcept {
			m_HitPos.at(m_HitNow).m_Pos = Pos;
			m_HitPos.at(m_HitNow).m_Color = color;
			m_HitPos.at(m_HitNow).m_Per = Per;
			m_HitPos.at(m_HitNow).TimeMax = 2.f;
			m_HitPos.at(m_HitNow).Time = m_HitPos.at(m_HitNow).TimeMax;
			++m_HitNow %= m_HitPos.size();
		}
	};
};
