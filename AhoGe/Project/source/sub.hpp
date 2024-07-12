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
			const auto& GetCamPos(void) const noexcept { return this->m_Pos; }
		public:
			void			SetCamPos(const Vector3DX& Pos) noexcept { this->m_Pos = Pos; }
			void			SetCamAim(const Vector3DX& Pos) noexcept {
				float z = this->m_Pos.z;
				Easing(&this->m_Pos, Pos, 0.9f, EasingType::OutExpo);
				this->m_Pos.z = z;
			}
		};
		// 空間上から画面上への変換
		extern inline void Convert2DtoDisp(const Vector3DX& Pos2D, Vector3DX* pRet) noexcept;
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
