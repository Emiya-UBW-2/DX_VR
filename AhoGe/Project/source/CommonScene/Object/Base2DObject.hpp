#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ColTarget {
			All,	//すべて判定
			Wall,	//地形のみ判定
			Object,	//オブジェクトのみ判定
			None,	//すべてすり抜け
		};

		class Base2DObject {
		private:
			std::string		m_ObjName;
			int				m_ObjType{ 0 };
			bool			m_IsDraw{ true };
			bool			m_IsDelete{ false };
			Vector3DX		m_PrevPos{};//前フレームの座標
			Vector3DX		m_Pos{};//座標
			Vector3DX		m_Vec{};//移動ベクトル
			float			m_Radius{ 1.f };//オブジェクトの半径
			ColTarget		m_ColTarget{ ColTarget::All };
		protected:
			bool			m_IsFirstLoop{ true };
		public:
			void SetObjName(std::string_view value) noexcept { this->m_ObjName = value; }
			void SetObjType(int value) noexcept { this->m_ObjType = value; }
			void SetPos(const Vector3DX& value) noexcept { this->m_Pos = value; }
		public:
			const auto& GetObjName(void) const noexcept { return this->m_ObjName; }
			const auto& GetObjType(void) const noexcept { return this->m_ObjType; }
			const auto& GetIsDraw(void) const noexcept { return this->m_IsDraw; }
			const auto& GetIsDelete(void) const noexcept { return this->m_IsDelete; }
			const auto& GetPos(void) const noexcept { return this->m_Pos; }

			const auto& GetPrevPos(void) const noexcept { return this->m_PrevPos; }

			const auto& GetRadius(void) const noexcept { return this->m_Radius; }
		protected:
			void SetVec(const Vector3DX& value) noexcept { this->m_Vec = value; }
			void SetColTarget(ColTarget value) noexcept { this->m_ColTarget = value; }
		protected:
			const auto& GetVec(void) const noexcept { return this->m_Vec; }
		protected:
			virtual void Init_Sub() noexcept{}
			virtual void Execute_Sub() noexcept {}
			virtual void Draw_Sub() noexcept {}
			virtual void Dispose_Sub() noexcept {}
		public:
			void Init() noexcept { Init_Sub(); }
			void Execute() noexcept;
			void Draw() noexcept { Draw_Sub(); }
			void Dispose() noexcept { Dispose_Sub(); }
		};
	};
};
