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
					auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
					if (!chara->IsAlive()) { continue; }
					if (!chara->GetCanLookByPlayer()) { continue; }
					int pos = InvalidID;
					float CosMax = -1.f;
					for (const auto& hitbox : chara->GetHitBoxList()) {
						auto EndPost = hitbox.GetPos();
						if (BackGround->CheckLinetoMap(EyePos, &EndPost)) { continue; }
						switch (hitbox.GetColType()) {
						case FPS_n2::Sceneclass::HitType::Head:
						case FPS_n2::Sceneclass::HitType::Body:
							break;
						default:
							continue;
							break;
						}
						float Cos = Vector3DX::Dot(AimVector, (hitbox.GetPos() - EyePos).normalized());
						if (Cos > cos(Radian)) {
							if (CosMax < Cos) {
								CosMax = Cos;
								pos = static_cast<int>(&hitbox - &chara->GetHitBoxList().front());
							}
						}
					}
					if (pos != InvalidID) {
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
				this->m_AutoAim = InvalidID;
			}
			if (this->m_AutoAim != InvalidID) {
				auto& TargetChara = PlayerMngr->GetPlayer(this->m_AutoAim)->GetChara();
				if (!TargetChara->IsAlive()) {
					this->m_AutoAim = InvalidID;
				}
			}
			this->m_AutoAimActive = (isActive && this->m_AutoAim != InvalidID);
			Easing(&this->m_AutoAimPer, this->m_AutoAimActive ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

			if (this->m_AutoAim != InvalidID) {
				auto& TargetChara = PlayerMngr->GetPlayer(this->m_AutoAim)->GetChara();
				Easing(&this->m_AutoAimVec, (TargetChara->GetHitBoxList().at(this->m_AutoAimPoint).GetPos() - EyePos).normalized(), 0.8f, EasingType::OutExpo);
			}
			else {
				Easing(&this->m_AutoAimVec, AimVector, 0.95f, EasingType::OutExpo);
			}
		}
		//
		const bool	GunPartsSlotControl::IsEffectParts(const SharedGunParts& SlotParts, GunFrame frame) const noexcept {
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
						if (GetPartsPtr(gunSlot)->HaveFrame(static_cast<int>(frame))) {
							return true;
						}
					}
					return false;
				}
			}
			return false;
		}
		void		GunPartsSlotControl::CalcAnyBySlot(const std::function<void(const SharedGunParts&)>& Doing) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				Doing(GetPartsPtr(static_cast<GunSlot>(loop)));
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				if (GetPartsPtr(static_cast<GunSlot>(loop))) {
					GetPartsPtr(static_cast<GunSlot>(loop))->GetGunPartsSlot()->CalcAnyBySlot(Doing);
				}
			}
		}
		//
		const bool GunPartsSlotControl::GetFramePartsMat(GunFrame frame, Matrix4x4DX* pOutMat) const noexcept {
			const SharedGunParts* pRet = nullptr;
			CalcAnyBySlot([&](const SharedGunParts& ptr) { if (IsEffectParts(ptr, frame)) { pRet = &ptr; } });
			if (!pRet) {
				return false;
			}
			*pOutMat = (*pRet)->GetFramePartsMat(frame);
			return true;
		}
		//
		void GunPartsSlotControl::AddGunParts(GunSlot gunSlot, const char* FilePath, const SharedGunParts& NewObj, const SharedObj& BaseModel) noexcept {
			SetPartsPtr(gunSlot) = NewObj;
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->AddObject(NewObj);
			ObjMngr->LoadModel(NewObj, BaseModel, FilePath);
			NewObj->Init();
		}
		void		GunPartsSlotControl::RemoveGunParts(GunSlot gunSlot) noexcept {
			if (!GetPartsPtr(gunSlot)) { return; }
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DelObj((SharedObj*)&SetPartsPtr(gunSlot));
			SetPartsPtr(gunSlot).reset();
		}
		//
		void		GunPartsSlotControl::UpdatePartsAnim(const MV1& pParent) {
			return;//現状のカスタム範囲では不要
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				if (GetPartsPtr(gunSlot)) {
					//1のフレーム移動量を無視する
					auto& Obj = SetPartsPtr(gunSlot);
					for (int loop2 = 0, max = static_cast<int>(Obj->GetObj().GetAnimNum()); loop2 < max; ++loop2) {
						Obj->SetObj().SetAnim(loop2).SetPer(pParent.GetAnim(loop2).GetPer());
						Obj->SetObj().SetAnim(loop2).SetTime(pParent.GetAnim(loop2).GetTime());
					}
					Obj->SetObj().UpdateAnimAll();
				}
			}
		}
		void		GunPartsSlotControl::UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot gunSlot) {
			if (!GetPartsPtr(gunSlot)) { return; }
			SetPartsPtr(gunSlot)->SetGunPartsMatrix(Matrix3x3DX::Get33DX(pMat), pMat.pos());
		}
		void GunPartsSlotControl::AttachGunParts(GunSlot gunSlot, int ID, const SharedObj& BaseModel) noexcept {
			auto* Slots = GetGunPartsData()->GetPartsSlot(gunSlot);
			if (!Slots) { return; }
			//パーツがある場合は削除
			if (GetPartsPtr(gunSlot)) {
				RemoveGunParts(gunSlot);
			}
			const auto* FilePath = (*GunPartsDataManager::Instance()->GetData(Slots->m_ItemsUniqueID.at(ID)))->GetPath().c_str();

			switch (gunSlot) {
			case GunSlot::Magazine:
				AddGunParts(gunSlot, FilePath, std::make_shared<MagazineClass>(), BaseModel);
				break;
			case GunSlot::Lower:
				AddGunParts(gunSlot, FilePath, std::make_shared<LowerClass>(), BaseModel);
				break;
			case GunSlot::Upper:
				AddGunParts(gunSlot, FilePath, std::make_shared<UpperClass>(), BaseModel);
				break;
			case GunSlot::Barrel:
				AddGunParts(gunSlot, FilePath, std::make_shared<BarrelClass>(), BaseModel);
				break;
			case GunSlot::UnderRail:
				AddGunParts(gunSlot, FilePath, std::make_shared<UnderRailClass>(), BaseModel);
				break;
			case GunSlot::Sight:
				AddGunParts(gunSlot, FilePath, std::make_shared<SightClass>(), BaseModel);
				break;
			case GunSlot::MuzzleAdapter:
				AddGunParts(gunSlot, FilePath, std::make_shared<MuzzleClass>(), BaseModel);
				break;
			default:
				break;
			}
		}
	};
};
