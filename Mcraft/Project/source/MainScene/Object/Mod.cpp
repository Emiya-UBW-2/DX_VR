#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const Matrix4x4DX GunPartsClass::GetFramePartsMat(GunFrame frame) const noexcept {
			Matrix4x4DX Mat;
			if (GetGunPartsSlot()->GetFramePartsMat(frame, &Mat)) {
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
		void GunPartsClass::Init_Sub(void) noexcept {
			this->m_GunPartsSlotControl = std::make_unique<GunPartsSlotControl>(GetFilePath());
			ObjectBaseClass::SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
			ObjectBaseClass::SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			this->m_IsMeshDraw.resize(GetObj().GetMeshNum());
			for (auto& isdraw : this->m_IsMeshDraw) {
				isdraw = true;
			}
			Init_GunParts();
		}
		void GunPartsClass::SetGunPartsMatrix(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
			SetMove().SetMat(rotation);
			SetMove().SetPos(pos);
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			this->m_GunPartsSlotControl->UpdatePartsAnim(GetObj());
			this->m_GunPartsSlotControl->UpdatePartsMove(GetFramePartsMat(GunFrame::UnderRail), GunSlot::UnderRail);
			this->m_GunPartsSlotControl->UpdatePartsMove(GetFramePartsMat(GunFrame::Sight), GunSlot::Sight);
			this->m_GunPartsSlotControl->UpdatePartsMove(GetFramePartsMat(GunFrame::MuzzleAdapter), GunSlot::MuzzleAdapter);
		}
		void GunPartsClass::Dispose_Sub(void) noexcept {
			this->m_IsMeshDraw.clear();
			this->m_GunPartsSlotControl.reset();
			Dispose_GunParts();
		}
		//
		void MagazineClass::Init_GunParts(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& ammo : this->m_Ammo) {
				ammo = std::make_shared<AmmoInChamberClass>();
				ObjMngr->AddObject(ammo);
				ObjMngr->LoadModel(ammo, ammo, GetGunPartsSlot()->GetGunPartsData()->GetAmmoSpecMagTop()->GetPath().c_str());
				ammo->Init();
				ammo->SetActive(true);
			}
		}
	};
};
