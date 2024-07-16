#pragma once
#include	"Header.hpp"

// 
namespace FPS_n2 {
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
			CommonBattleResource(void){}
			CommonBattleResource(const CommonBattleResource&) = delete;
			CommonBattleResource(CommonBattleResource&& o) = delete;
			CommonBattleResource& operator=(const CommonBattleResource&) = delete;
			CommonBattleResource& operator=(CommonBattleResource&& o) = delete;
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		public:
			void			AddCharacter(PlayerID value) noexcept;
			void			DelCharacter(PlayerID value) noexcept;
		};
		//
		enum class EffectType {
			Damage,
			Guard,
			WallHit,
		};
		class Effect2DControl : public SingletonBase<Effect2DControl> {
		private:
			friend class SingletonBase<Effect2DControl>;
		private:
			struct GuardPos {
				Vector3DX					m_Pos{};
				float						m_Per{};
				float						m_PerMax{};
				EffectType					m_EffectType{};
			};
		private:
			std::array<GuardPos, 8>		m_GuardPos;
			int							m_GuardPosNum{};
		private:
			Effect2DControl(void) {}
			Effect2DControl(const Effect2DControl&) = delete;
			Effect2DControl(Effect2DControl&& o) = delete;
			Effect2DControl& operator=(const Effect2DControl&) = delete;
			Effect2DControl& operator=(Effect2DControl&& o) = delete;
		public:
			void			Set(const Vector3DX& Pos2D, EffectType Type,float Per) noexcept {
				m_GuardPos.at(m_GuardPosNum).m_Pos = Pos2D;
				m_GuardPos.at(m_GuardPosNum).m_EffectType = Type;
				m_GuardPos.at(m_GuardPosNum).m_Per = Per;
				m_GuardPos.at(m_GuardPosNum).m_PerMax = Per;
				++m_GuardPosNum %= static_cast<int>(m_GuardPos.size());
			}
		public:
			void			Init() noexcept;
			void			Update() noexcept;
			void			Draw() noexcept;
		};
		// 
		class Cam2DControl : public SingletonBase<Cam2DControl> {
		private:
			friend class SingletonBase<Cam2DControl>;
		private:
			Vector3DX	m_Pos{};
		private:
			Cam2DControl(void) {}
			Cam2DControl(const Cam2DControl&) = delete;
			Cam2DControl(Cam2DControl&& o) = delete;
			Cam2DControl& operator=(const Cam2DControl&) = delete;
			Cam2DControl& operator=(Cam2DControl&& o) = delete;
		public:
			const Vector3DX GetCamPos(void) const noexcept;
		public:
			void			SetCamPos(const Vector3DX& Pos) noexcept {
				float z = this->m_Pos.z;
				this->m_Pos = Pos;
				this->m_Pos.z = z;
			}
			void			SetCamAim(const Vector3DX& Pos) noexcept {
				float z = this->m_Pos.z;
				Easing(&this->m_Pos, Pos, 0.9f, EasingType::OutExpo);
				this->m_Pos.z = z;
			}
			void			SetCamRangePos(float Z) noexcept {
				this->m_Pos.z = Z;
			}
			void			SetCamRangeAim(float Z) noexcept {
				Easing(&this->m_Pos.z, Z, 0.9f, EasingType::OutExpo);
			}
		};
		// 空間上から画面上への変換
		extern inline void Convert2DtoDisp(const Vector3DX& Pos2D, Vector3DX* pRet) noexcept;
		// 空間上から画面上への変換
		extern inline bool Is2DPositionInDisp(const Vector3DX& Pos2D, int Radius) noexcept;
		// ベクトルのxy間の角度を取得
		extern inline float GetRadVec2Vec(const Vector3DX& vec1, const Vector3DX& vec2) noexcept;
		// 空間上のタイルごとのサイズを取得
		extern inline float Get2DSize(float value) noexcept;
		// 空間上のサイズからタイル枚数を取得
		extern inline float Get2DSizetoTile(float value) noexcept;
		// 画面上のタイルごとのサイズを取得
		extern inline int GetDispSize(float value) noexcept;
	};
};
