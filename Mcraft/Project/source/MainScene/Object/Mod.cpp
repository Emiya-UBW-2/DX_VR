#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Guns {
		const Matrix4x4DX GunPartsObj::GetFramePartsMat(GunFrame frame) const noexcept {
			Matrix4x4DX Mat;
			if (GetModifySlot()->GetPartsFrameMatChild(frame, &Mat)) {
				return Mat;
			}
			if (HaveFrame(static_cast<int>(frame))) {
				auto FrameID = GetFrame(static_cast<int>(frame));
				Matrix4x4DX Ret = GetObj().GetFrameLocalWorldMatrix(FrameID);
				if (frame == GunFrame::Sight) {
					if (GetObj().GetFrameChildNum(FrameID) > 0) {
						Vector3DX vec = (GetObj().GetChildFrameWorldMatrix(FrameID, 0).pos() - Ret.pos()).normalized();
						Ret = (Ret.rotation() * Matrix4x4DX::RotVec2(Ret.yvec(), vec)) * Matrix4x4DX::Mtrans(Ret.pos());
					}
				}
				return Ret;
			}
			return Matrix4x4DX::identity();
		}
		void GunPartsObj::Init_Sub(void) noexcept {
			this->m_ModifySlot = std::make_unique<ModifySlot>(GetFilePath());
			SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
			SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			this->m_IsMeshDraw.resize(GetObj().GetMeshNum());
			for (auto& isdraw : this->m_IsMeshDraw) {
				isdraw = true;
			}
			Init_GunParts();
		}
		void GunPartsObj::SetGunPartsMatrix(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
			SetMove().SetMat(rotation);
			SetMove().SetPos(pos);
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			this->m_ModifySlot->UpdatePartsAnim(GetObj());
			this->m_ModifySlot->UpdatePartsMove(GunSlot::UnderRail, GetFramePartsMat(GunFrame::UnderRail));
			this->m_ModifySlot->UpdatePartsMove(GunSlot::Sight, GetFramePartsMat(GunFrame::Sight));
			this->m_ModifySlot->UpdatePartsMove(GunSlot::MuzzleAdapter, GetFramePartsMat(GunFrame::MuzzleAdapter));
		}
		void GunPartsObj::Dispose_Sub(void) noexcept {
			this->m_IsMeshDraw.clear();
			this->m_ModifySlot.reset();
			Dispose_GunParts();
		}
		//
		void MagazinePartsObj::Init_GunParts(void) noexcept {
			const char* Path = Objects::AmmoDataManager::Instance()->Get(GetModifySlot()->GetMyData()->GetAmmoSpecID(0))->GetPath().c_str();
			for (auto& ammo : this->m_Ammo) {
				ammo = std::make_shared<Objects::AmmoInChamberObj>();
				ObjectManager::Instance()->InitObject(ammo, Path);
				ammo->SetActive(true);
			}
		}
	};
};
