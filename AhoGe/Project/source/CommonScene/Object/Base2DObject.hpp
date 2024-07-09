#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ColTarget {
			All,	// すべて判定
			Wall,	// 地形のみ判定
			Object,	// オブジェクトのみ判定
			None,	// すべてすり抜け
		};
		enum class HitTarget {
			Physical,		// 位置に干渉
			HitOnly,	// ヒット判定のみ
		};

		class Base2DObject {
		private:
			int				m_ObjectID{ -1 };
			std::string		m_ObjName;
			int				m_ObjType{0};
			bool			m_IsDraw{true};
			bool			m_IsDelete{false};
			float			m_Size{ 1.f };
			Vector3DX		m_PrevPos{};// 前フレームの座標
			Vector3DX		m_Pos{};// 座標
			Vector3DX		m_Vec{};// 移動ベクトル
			float			m_Radius{1.f};// オブジェクトの半径
			ColTarget		m_ColTarget{ ColTarget::All };
			HitTarget		m_HitTarget{ HitTarget::Physical };
			int				m_HitObjectID{ -1 };
		private:
			struct BlurParts {
				Vector3DX	Pos;
				float		Time{};
				float		TimeMax{};
			public:
				const auto IsActive() const noexcept { return Time > 0.f; }
				const auto GetPer() const noexcept { return Time / TimeMax; }
			};
			std::array<BlurParts, 60>	m_Blur{};
			int							m_BlurNow{ 0 };
		protected:
			void AddBlur(float Blur) noexcept;
			const auto& GetBlur(void) const noexcept { return this->m_Blur; }
		protected:
			bool			m_IsFirstLoop{true};
		public:
			void SetObjName(std::string_view value) noexcept { this->m_ObjName = value; }
			void SetObjType(int value) noexcept { this->m_ObjType = value; }
			void SetPos(const Vector3DX& value) noexcept {
				this->m_Pos = value;
				this->m_PrevPos = this->m_Pos;
				
			}
			void SetVec(const Vector3DX& value) noexcept { this->m_Vec = value; }
		public:
			const auto& GetObjectID(void) const noexcept { return this->m_ObjectID; }

			const auto& GetObjName(void) const noexcept { return this->m_ObjName; }
			const auto& GetObjType(void) const noexcept { return this->m_ObjType; }
			const auto& GetIsDraw(void) const noexcept { return this->m_IsDraw; }
			const auto& GetIsDelete(void) const noexcept { return this->m_IsDelete; }

			const auto& GetSize(void) const noexcept { return this->m_Size; }
			const auto& GetPos(void) const noexcept { return this->m_Pos; }
			const auto& GetPrevPos(void) const noexcept { return this->m_PrevPos; }
			const auto& GetRadius(void) const noexcept { return this->m_Radius; }
			const auto& GetVec(void) const noexcept { return this->m_Vec; }

			const auto& GetHitTarget(void) const noexcept { return this->m_HitTarget; }
			const auto& GetHitObjectID(void) const noexcept { return this->m_HitObjectID; }
		public:
			void SetObjectID(int value) noexcept { this->m_ObjectID = value; }

			void SetHitObjectID(int value) noexcept { this->m_HitObjectID = value; }
			void SetSize(float value) noexcept { this->m_Size = value; }
		protected:
			void SetColTarget(ColTarget value) noexcept { this->m_ColTarget = value; }
			void SetHitTarget(HitTarget value) noexcept { this->m_HitTarget = value; }
			void SetDelete(void) noexcept { this->m_IsDelete = true; }
		protected:
			virtual void Init_Sub() noexcept {}
			virtual void Execute_Sub() noexcept {}
			virtual void DrawShadow_Sub() noexcept {}
			virtual void Draw_Sub() noexcept {}
			virtual void Dispose_Sub() noexcept {}
		public:
			//ヒットした瞬間に呼ぶ系のもの
			virtual void Execute_OnHitObject() noexcept {}
			virtual void Execute_OnHitWall() noexcept {}
		public:
			void Init() noexcept { Init_Sub(); }
			void Execute() noexcept;
			void ExecuteAfter() noexcept;
			void DrawShadow() noexcept { DrawShadow_Sub(); }
			void Draw() noexcept { Draw_Sub(); }
			void Dispose() noexcept { Dispose_Sub(); }
		};
	};
};
