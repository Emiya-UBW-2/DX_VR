#include	"Gun_before.hpp"
#include	"Mod.hpp"

#include	"../../MainScene/Player/Player.hpp"
#include	"Character_before.hpp"
#include	"Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void AutoAimControl::Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (isActive) {
				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
					if (loop == MyPlayerID) { continue; }
					auto& c = PlayerMngr->GetPlayer(loop)->GetChara();
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
						this->m_AutoAimTimer = 1.f;
						this->m_AutoAim = loop;
						this->m_AutoAimPoint = pos;
						break;
					}
				}
			}

			auto prev = this->m_AutoAimTimer;
			this->m_AutoAimTimer = std::max(this->m_AutoAimTimer - DXLib_refParts->GetDeltaTime(), 0.f);
			if (prev > 0.f && this->m_AutoAimTimer == 0.f) {
				this->m_AutoAim = -1;
			}
			if (this->m_AutoAim != -1) {
				auto& TargetChara = PlayerMngr->GetPlayer(this->m_AutoAim)->GetChara();
				if (!TargetChara->IsAlive()) {
					this->m_AutoAim = -1;
				}
			}
			this->m_AutoAimActive = (isActive && this->m_AutoAim != -1);
			Easing(&this->m_AutoAimPer, this->m_AutoAimActive ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

			if (this->m_AutoAim != -1) {
				auto& TargetChara = PlayerMngr->GetPlayer(this->m_AutoAim)->GetChara();
				Easing(&this->m_AutoAimVec, (TargetChara->GetHitBoxList().at(this->m_AutoAimPoint).GetPos() - EyePos).normalized(), 0.8f, EasingType::OutExpo);
			}
			else {
				Easing(&this->m_AutoAimVec, AimVector, 0.95f, EasingType::OutExpo);
			}
		}
		//
		const bool	ModSlotControl::IsEffectParts(const SharedObj& SlotParts, GunFrame frame) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				if (SlotParts == GetPartsPtr(gunSlot)) {
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
						if (((const std::shared_ptr<ModClass>&)GetPartsPtr(gunSlot))->HaveFrame(static_cast<int>(frame))) {
							return true;
						}
					}
					return false;
				}
			}
			return false;
		}
		void		ModSlotControl::CalcAnyBySlot(const std::function<void(const SharedObj&)>& Doing) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				Doing(GetPartsPtr(gunSlot));
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				if (GetPartsPtr(gunSlot)) {
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
		//
		void ModSlotControl::AddMod(GunSlot gunSlot, const char* ItemPath, const SharedObj& NewObj, const SharedObj& BaseModel) noexcept {
			this->m_Parts_Ptr[static_cast<int>(gunSlot)] = NewObj;

			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->AddObject(NewObj);
			ObjMngr->LoadModel(NewObj, BaseModel, ItemPath);
			NewObj->Init();
		}
		void		ModSlotControl::RemoveMod(GunSlot gunSlot) noexcept {
			if (!GetPartsPtr(gunSlot)) { return; }
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DelObj(&this->m_Parts_Ptr[static_cast<int>(gunSlot)]);
			this->m_Parts_Ptr[static_cast<int>(gunSlot)].reset();
		}
		//
		void		ModSlotControl::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				if (GetPartsPtr(gunSlot) && false) {//現状のカスタム範囲では不要
					//1のフレーム移動量を無視する
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					for (int loop2 = 0; loop2 < Obj->GetObj().GetAnimNum(); ++loop2) {
						Obj->SetObj().SetAnim(loop2).SetPer(pParent.GetAnim(loop2).GetPer());
						Obj->SetObj().SetAnim(loop2).SetTime(pParent.GetAnim(loop2).GetTime());
					}
					Obj->SetObj().UpdateAnimAll();
				}
			}
		}
		void		ModSlotControl::UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot gunSlot) {
			if (!GetPartsPtr(gunSlot)) { return; }
			((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[static_cast<int>(gunSlot)])->SetModMatrix(Matrix3x3DX::Get33DX(pMat), pMat.pos());
		}
		void ModSlotControl::SetMod(GunSlot gunSlot, int ID, const SharedObj& BaseModel) noexcept {
			RemoveMod(gunSlot);
			if (GetPartsPtr(gunSlot) != nullptr) { return; }
			auto* Slots = GetModData()->GetPartsSlot(gunSlot);
			if (!Slots) { return; }
			auto& Data = *ModDataManager::Instance()->GetData(Slots->m_ItemsUniqueID.at(ID));

			switch (gunSlot) {
			case GunSlot::Magazine:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<MagazineClass>(), BaseModel);
				break;
			case GunSlot::Lower:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<LowerClass>(), BaseModel);
				break;
			case GunSlot::Upper:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<UpperClass>(), BaseModel);
				break;
			case GunSlot::Barrel:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<BarrelClass>(), BaseModel);
				break;
			case GunSlot::UnderRail:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<UnderRailClass>(), BaseModel);
				break;
			case GunSlot::Sight:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<SightClass>(), BaseModel);
				break;
			case GunSlot::MuzzleAdapter:
				AddMod(gunSlot, Data->GetPath().c_str(), std::make_shared<MuzzleClass>(), BaseModel);
				break;
			default:
				break;
			}
		}
	};
};
