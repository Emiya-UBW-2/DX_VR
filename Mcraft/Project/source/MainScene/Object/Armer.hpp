#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Objects {
		enum class ArmorFrame {
			Center,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			RightHandPos,
			RightHandYvec,
			RightHandZvec,
			Max,
		};
		static const char* ArmorFrameName[static_cast<int>(ArmorFrame::Max)] = {
			"ÉZÉìÉ^Å[",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"righthand",
			"righthand_yvec",
			"righthand_zvec",
		};


		class ArmorObj : public BaseObject {
		private:
			const auto			GetFrameMat(ArmorFrame frame) const noexcept {
				return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame)));
			}
		private:
			int					GetFrameNum(void) noexcept override { return static_cast<int>(ArmorFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return ArmorFrameName[id]; }
		public:
			ArmorObj(void) noexcept { this->m_objType = (int)ObjType::Armor; }
			~ArmorObj(void) noexcept {}
		public:
			const auto			GetLeftHandMat(void) const noexcept {
				Vector3DX HandPos = GetFrameMat(ArmorFrame::LeftHandPos).pos();
				Vector3DX Handyvec = GetFrameMat(ArmorFrame::LeftHandYvec).pos() - HandPos;
				Vector3DX Handzvec = GetFrameMat(ArmorFrame::LeftHandZvec).pos() - HandPos;
				return Matrix4x4DX::Axis1(Handyvec.normalized(), Handzvec.normalized(), HandPos);
			}
			const auto			GetRightHandMat(void) const noexcept {
				Vector3DX HandPos = GetFrameMat(ArmorFrame::RightHandPos).pos();
				Vector3DX Handyvec = GetFrameMat(ArmorFrame::RightHandYvec).pos() - HandPos;
				Vector3DX Handzvec = GetFrameMat(ArmorFrame::RightHandZvec).pos() - HandPos;
				return Matrix4x4DX::Axis1(Handyvec.normalized(), Handzvec.normalized(), HandPos);
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetActive(false);
				SetMinAABB(Vector3DX::vget(-1.f, 0.f, -1.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			}
			void				FirstUpdate(void) noexcept override {

			}
			void			Dispose_Sub(void) noexcept override {}
		};
	};
};
