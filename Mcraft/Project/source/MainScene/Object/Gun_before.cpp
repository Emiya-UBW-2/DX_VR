#include	"Gun_before.hpp"
#include	"Mod.hpp"

#include	"../../MainScene/Player/Player.hpp"
#include	"Character_before.hpp"
#include	"Character.hpp"

namespace FPS_n2 {
	namespace Guns {
		//
		void			AutoAimControl::Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			if (isActive) {
				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
					if (loop == MyPlayerID) { continue; }
					auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
					if (!chara->IsAlive()) { continue; }
					if (!chara->GetCanLookByPlayer()) { continue; }
					int pos = InvalidID;
					float CosMax = -1.f;
					for (const auto& hitbox : chara->GetHitBoxList()) {
						Vector3DX EndPost = hitbox.GetPos();
						if (BackGroundParts->CheckLinetoMap(EyePos, &EndPost)) { continue; }
						switch (hitbox.GetColType()) {
						case Charas::HitType::Head:
						case Charas::HitType::Body:
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
		const bool		ModifySlot::IsEffectParts(const SharedGunParts& SlotParts, GunFrame frame) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				if (SlotParts == this->m_PartsObj[static_cast<int>(gunSlot)]) {
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
						if (this->m_PartsObj[static_cast<int>(gunSlot)]->HaveFrame(static_cast<int>(frame))) {
							return true;
						}
					}
					return false;
				}
			}
			return false;
		}
		void			ModifySlot::GetAnyByChild(const std::function<void(const SharedGunParts&)>& Doing) const noexcept {
			//子供を参照
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				Doing(this->m_PartsObj[loop]);
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				if (this->m_PartsObj[loop]) {
					this->m_PartsObj[loop]->GetModifySlot()->GetAnyByChild(Doing);
				}
			}
		}
		//
		const bool		ModifySlot::GetPartsFrameMatChild(GunFrame frame, Matrix4x4DX* pOutMat) const noexcept {
			const SharedGunParts* pRet = nullptr;
			GetAnyByChild([&](const SharedGunParts& ptr) { if (IsEffectParts(ptr, frame)) { pRet = &ptr; } });
			if (!pRet) {
				return false;
			}
			*pOutMat = (*pRet)->GetFramePartsMat(frame);
			return true;
		}
		//
		void			ModifySlot::Add(GunSlot gunSlot, const char* FilePath, const SharedGunParts& NewObj, const SharedObj& BaseModel) noexcept {
			this->m_PartsObj[static_cast<int>(gunSlot)] = NewObj;
			ObjectManager::Instance()->LoadModelBefore(FilePath);
			ObjectManager::Instance()->InitObject(NewObj, BaseModel, FilePath);
		}
		void			ModifySlot::Remove(GunSlot gunSlot) noexcept {
			if (!IsAttachedParts(gunSlot)) { return; }
			ObjectManager::Instance()->DelObj(this->m_PartsObj[static_cast<int>(gunSlot)]);
			this->m_PartsObj[static_cast<int>(gunSlot)].reset();
		}
		//
		void			ModifySlot::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot gunSlot = static_cast<GunSlot>(loop);
				if (IsAttachedParts(gunSlot)) {
					//1のフレーム移動量を無視する
					auto& Obj = this->m_PartsObj[static_cast<int>(gunSlot)];
					for (int loop2 = 0, max = static_cast<int>(Obj->GetObj().GetAnimNum()); loop2 < max; ++loop2) {
						Obj->SetObj().SetAnim(loop2).SetPer(pParent.GetAnim(loop2).GetPer());
						Obj->SetObj().SetAnim(loop2).SetTime(pParent.GetAnim(loop2).GetTime());
					}
					Obj->SetObj().UpdateAnimAll();
				}
			}
		}
		void			ModifySlot::UpdatePartsMove(GunSlot gunSlot, const Matrix4x4DX& pMat) {
			if (!IsAttachedParts(gunSlot)) { return; }
			this->m_PartsObj[static_cast<int>(gunSlot)]->SetGunPartsMatrix(Matrix3x3DX::Get33DX(pMat), pMat.pos());
		}
		void			ModifySlot::Attach(GunSlot gunSlot, int ID, const SharedObj& BaseModel) noexcept {
			auto* pSlotInfo = GetMyData()->GetSlotInfo(gunSlot);
			if (!pSlotInfo) { return; }
			//パーツがある場合は削除
			if (IsAttachedParts(gunSlot)) {
				Remove(gunSlot);
			}
			const auto* FilePath = (*GunPartsDataManager::Instance()->GetData(pSlotInfo->m_CanAttachItemsUniqueID.at(ID)))->GetPath().c_str();

			switch (gunSlot) {
			case GunSlot::Magazine:
				Add(gunSlot, FilePath, std::make_shared<MagazinePartsObj>(), BaseModel);
				break;
			case GunSlot::Lower:
				Add(gunSlot, FilePath, std::make_shared<LowerPartsObj>(), BaseModel);
				break;
			case GunSlot::Upper:
				Add(gunSlot, FilePath, std::make_shared<UpperPartsObj>(), BaseModel);
				break;
			case GunSlot::Barrel:
				Add(gunSlot, FilePath, std::make_shared<BarrelPartsObj>(), BaseModel);
				break;
			case GunSlot::UnderRail:
				Add(gunSlot, FilePath, std::make_shared<UnderRailPartsObj>(), BaseModel);
				break;
			case GunSlot::Sight:
				Add(gunSlot, FilePath, std::make_shared<SightPartsObj>(), BaseModel);
				break;
			case GunSlot::MuzzleAdapter:
				Add(gunSlot, FilePath, std::make_shared<MuzzlePartsObj>(), BaseModel);
				break;
			default:
				break;
			}
		}
		//
		const std::unique_ptr<ModifySlot>&	GunsModify::SlotData::GetAttachedPartsSlot(void) const noexcept { return (*this->m_MySlot)->GetParts(this->m_SlotType)->GetModifySlot(); }
		//
		GunsModify::GunsModify(const std::shared_ptr<GunObj>& pBaseGun, bool isPreset) noexcept {
			this->m_BaseGun = pBaseGun;
			SetupDefaultGunParts(&pBaseGun->GetModifySlot(), nullptr, isPreset);
		}
		void			GunsModify::SetupDefaultGunParts(const std::unique_ptr<ModifySlot>* pBaseParts, const std::unique_ptr<SlotData>* pParentSlot, bool isPreset) noexcept {
			if (!pBaseParts) { return; }
			//子供にデフォルトの設定を追加
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot NowSlot = (GunSlot)loop;
				const auto& pSlotInfo = (*pBaseParts)->GetMyData()->GetSlotInfo(NowSlot);
				if (!pSlotInfo) { continue; }// スロットがないとスルー
				//追加
				this->m_SlotDataPool.emplace_back(std::make_unique<SlotData>(NowSlot, pParentSlot, pBaseParts));
				auto& data = this->m_SlotDataPool.back();
				//スロットに設定するオブジェクトの選択
				int Select = pSlotInfo->m_IsNeed ? 0 : InvalidID;
				if (isPreset) {
					for (const auto& S : this->m_SlotSave) {
						if (data->IsSavedSlot(S)) {
							Select = S.m_select;
							break;
						}
					}
				}
				//選択したパーツをセット
				data->Set(this->m_BaseGun, Select);
				//パーツがある場合
				if (data->IsAttachedParts()) {
					//スロットに設定したものに沿ってデフォルトの子パーツを設定
					SetupDefaultGunParts(&data->GetAttachedPartsSlot(), &data, isPreset);
				}
			}
		}
		bool			GunsModify::DeleteSlotsChildParts(const std::unique_ptr<SlotData>* pSlot) noexcept {
			//設定したいパーツが親であるパーツも子をすべて殺す
			for (int loop = 0; loop < static_cast<int>(this->m_SlotDataPool.size()); ++loop) {
				const auto& data = this->m_SlotDataPool[loop];
				if (&data == pSlot) { continue; }
				if (data->GetParentSlot() != pSlot) { continue; }
				if (DeleteSlotsChildParts(&data)) {
					--loop;
				}
			}
			for (auto& data : this->m_SlotDataPool) {
				if (&data == pSlot) { continue; }
				int index = static_cast<int>(&data - &this->m_SlotDataPool.front());
				//そのデータ自体を削除
				data.reset();
				this->m_SlotDataPool.erase(this->m_SlotDataPool.begin() + index);
				return true;
			}
			return false;
		}
		void			GunsModify::ChangeSelectData(const std::unique_ptr<SlotData>* pSlot, int select) noexcept {
			DeleteSlotsChildParts(pSlot);
			//
			//一旦クリーンにする
			auto& data = *pSlot;
			data->Set(this->m_BaseGun, select);
			//パーツがある場合
			if (data->IsAttachedParts()) {
				//スロットに設定したものに沿ってデフォルトのパーツを設定
				SetupDefaultGunParts(&data->GetAttachedPartsSlot(), &data, false);
			}
		}
		void			GunsModify::LoadSlots(const char* path) noexcept {
			this->m_SlotSave.clear();
			std::ifstream inputfile(path, std::ios::binary);
			if (inputfile) {
				{
					int ULTSelect = 0;
					inputfile.read(reinterpret_cast<char*>(&ULTSelect), sizeof(ULTSelect));
				}
				while (!inputfile.eof()) { //ファイルの最後まで続ける
					SlotSaveData Tmp;
					inputfile.read(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
					this->m_SlotSave.emplace_back(Tmp);
				}
			}
			inputfile.close();
		}
		void			GunsModify::SaveSlots(const char* path) noexcept {
			std::ofstream outputfile(path, std::ios::binary);
			{
				int ULTSelect = 0;
				outputfile.write(reinterpret_cast<char*>(&ULTSelect), sizeof(ULTSelect));
			}
			for (auto& data : this->m_SlotDataPool) {
				SlotSaveData Tmp = data->GetSaveData();
				outputfile.write(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
			}
			outputfile.close();
		}
};
};
