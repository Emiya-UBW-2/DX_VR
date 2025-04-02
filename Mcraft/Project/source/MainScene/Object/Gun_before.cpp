#include	"Gun_before.hpp"
#include "Mod.hpp"

#include "../../MainScene/Player/Player.hpp"
#include "Character_before.hpp"
#include "Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void AutoAimControl::Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (isActive) {
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == MyPlayerID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!c->IsAlive()) { continue; }
					if (!c->GetCanLookByPlayer()) { continue; }
					int pos = -1;
					float CosMax = -1.f;
					for (const auto& h : c->GetHitBoxList()) {
						auto EndPost = h.GetPos();
						if (BackGround->CheckLinetoMap(EyePos, &EndPost)) { continue; }
						switch (h.GetColType()) {
						case FPS_n2::Sceneclass::HitType::Head:
						case FPS_n2::Sceneclass::HitType::Body:
							break;
						default:
							continue;
							break;
						}
						float Cos = Vector3DX::Dot(AimVector, (h.GetPos() - EyePos).normalized());
						if (Cos > cos(Radian)) {
							if (CosMax < Cos) {
								CosMax = Cos;
								pos = static_cast<int>(&h - &c->GetHitBoxList().front());
							}
						}
					}
					if (pos != -1) {
						m_AutoAimTimer = 1.f;
						m_AutoAim = i;
						m_AutoAimPoint = pos;
						break;
					}
				}
			}

			auto prev = m_AutoAimTimer;
			m_AutoAimTimer = std::max(m_AutoAimTimer - DXLib_refParts->GetDeltaTime(), 0.f);
			if (prev > 0.f && m_AutoAimTimer == 0.f) {
				m_AutoAim = -1;
			}
			if (m_AutoAim != -1) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AutoAim)->GetChara();
				if (!Chara->IsAlive()) {
					m_AutoAim = -1;
				}
			}
			m_AutoAimActive = (isActive && this->m_AutoAim != -1);
			Easing(&m_AutoAimPer, m_AutoAimActive ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

			if (this->m_AutoAim != -1) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_AutoAim)->GetChara();
				Easing(&m_AutoAimVec, (Chara->GetHitBoxList().at(this->m_AutoAimPoint).GetPos() - EyePos).normalized(), 0.8f, EasingType::OutExpo);
			}
			else {
				Easing(&m_AutoAimVec, AimVector, 0.95f, EasingType::OutExpo);
			}
		}
		//
		const bool	ModSlotControl::IsEffectParts(const SharedObj& SlotParts, GunFrame frame) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (SlotParts == this->m_Parts_Ptr[loop]) {
					bool HasActiveFrame = false;
					switch (static_cast<GunSlot>(loop)) {
					case GunSlot::Magazine:
						break;
					case GunSlot::Lower:
						switch (frame) {
						case GunFrame::Magpos:
						case GunFrame::LeftHandPos:
						case GunFrame::LeftHandYvec:
						case GunFrame::LeftHandZvec:
						case GunFrame::RightHandPos:
						case GunFrame::RightHandYvec:
						case GunFrame::RightHandZvec:
							HasActiveFrame = true;
							break;
						default:
							break;
						}
						break;
					case GunSlot::Upper:
						switch (frame) {
						case GunFrame::Cart:
						case GunFrame::CartVec:
						case GunFrame::Eyepos:
						case GunFrame::Lens:
						case GunFrame::LensSize:
							HasActiveFrame = true;
							break;
						default:
							break;
						}
						break;
					case GunSlot::Barrel:
						switch (frame) {
						case GunFrame::Muzzle:
							HasActiveFrame = true;
							break;
						default:
							break;
						}
						break;
					case GunSlot::UnderRail:
						switch (frame) {
						case GunFrame::Eyepos:
						case GunFrame::Lens:
						case GunFrame::LensSize:
						case GunFrame::LaserSight:
						case GunFrame::Light:
							HasActiveFrame = true;
							break;
						default:
							break;
						}
						break;
					case GunSlot::Sight:
						switch (frame) {
						case GunFrame::Sight:
						case GunFrame::Eyepos:
						case GunFrame::Lens:
						case GunFrame::LensSize:
							HasActiveFrame = true;
							break;
						default:
							break;
						}
						break;
					case GunSlot::MuzzleAdapter:
						switch (frame) {
						case GunFrame::Muzzle:
							HasActiveFrame = true;
							break;
						default:
							break;
						}
						break;
					default:
						break;
					}
					if (HasActiveFrame) {
						if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->HaveFrame(static_cast<int>(frame))) {
							return true;
						}
					}
					return false;
				}
			}
			return false;
		}
		void		ModSlotControl::CalcAnyBySlot(const std::function<void(const SharedObj&)>& Doing) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				Doing(this->m_Parts_Ptr[loop]);
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetModSlot().CalcAnyBySlot(Doing);
				}
			}
		}
		//
		const SharedObj* ModSlotControl::HasFrameBySlot(GunFrame frame) const noexcept {
			const SharedObj* pRet = nullptr;
			CalcAnyBySlot([&](const SharedObj& ptr) { if (IsEffectParts(ptr, frame)) { pRet = &ptr; } });
			return pRet;
		}
		void		ModSlotControl::RemoveMod(GunSlot Slot) noexcept {
			if (this->m_Parts_Ptr[static_cast<int>(Slot)]) {
				auto* ObjMngr = ObjectManager::Instance();
				ObjMngr->DelObj(&this->m_Parts_Ptr[static_cast<int>(Slot)]);
				this->m_Parts_Ptr[static_cast<int>(Slot)].reset();
			}
		}
		void		ModSlotControl::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop] && false) {//現状のカスタム範囲では不要
					//1のフレーム移動量を無視する
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					for (int i = 0; i < Obj->GetObj_const().GetAnimNum(); i++) {
						Obj->GetObj().SetAnim(i).SetPer(pParent.GetAnim(i).GetPer());
						Obj->GetObj().SetAnim(i).SetTime(pParent.GetAnim(i).GetTime());
					}
					Obj->GetObj().UpdateAnimAll();
				}
			}
		}
		void		ModSlotControl::UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot Slot) {
			if (this->m_Parts_Ptr[static_cast<int>(Slot)]) {
				((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[static_cast<int>(Slot)])->SetModMatrix(Matrix3x3DX::Get33DX(pMat), pMat.pos());
			}
		}
		const SharedObj& ModSlotControl::SetMod(GunSlot Slot, int ID, const SharedObj& BaseModel) noexcept {
			RemoveMod(Slot);
			auto* Slots = this->m_ModDataClass->GetPartsSlot(Slot);
			if (Slots) {
				if (this->m_Parts_Ptr[static_cast<int>(Slot)] == nullptr) {
					auto AddObj = [&](const SharedObj& NewObj) {
						this->m_Parts_Ptr[static_cast<int>(Slot)] = NewObj;
						auto& Data = *ModDataManager::Instance()->GetData(Slots->m_ItemsUniqueID.at(ID));

						auto* ObjMngr = ObjectManager::Instance();
						ObjMngr->AddObject(NewObj);
						ObjMngr->LoadModel(NewObj, BaseModel, Data->GetPath().c_str());
						NewObj->Init();
						};
					switch (Slot) {
					case GunSlot::Magazine:
						AddObj(std::make_shared<MagazineClass>());
						break;
					case GunSlot::Lower:
						AddObj(std::make_shared<LowerClass>());
						break;
					case GunSlot::Upper:
						AddObj(std::make_shared<UpperClass>());
						break;
					case GunSlot::Barrel:
						AddObj(std::make_shared<BarrelClass>());
						break;
					case GunSlot::UnderRail:
						AddObj(std::make_shared<UnderRailClass>());
						break;
					case GunSlot::Sight:
						AddObj(std::make_shared<SightClass>());
						break;
					case GunSlot::MuzzleAdapter:
						AddObj(std::make_shared<MuzzleClass>());
						break;
					default:
						break;
					}
				}
			}

			return this->m_Parts_Ptr[static_cast<int>(Slot)];
		}
	};
};
