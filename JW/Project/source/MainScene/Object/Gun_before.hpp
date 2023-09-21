#pragma once
#include	"../../Header.hpp"

#include "../../ObjectManager.hpp"
#include "Mod.hpp"
#include "Cart.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MuzzleSmokeControl {
		private://キャラパラメーター
			std::array<VECTOR_ref, 8>		m_Line;
			int								m_LineSel = 0;
			float							m_LinePer{ 0.f };
		private:
		public://ゲッター
			void			AddMuzzleSmokePower() noexcept { m_LinePer = std::clamp(m_LinePer + 0.1f, 0.f, 1.f); }
		protected:
			void		InitMuzzleSmoke(const VECTOR_ref& pPos) {
				for (auto& l : this->m_Line) {
					l = pPos;
				}
			}
			void		ExecuteMuzzleSmoke(const VECTOR_ref& pPos) {
				for (auto& l : this->m_Line) {
					l += VECTOR_ref::vget(
						GetRandf(0.2f*Scale_Rate / FPS),
						0.4f*Scale_Rate / FPS + GetRandf(0.3f*Scale_Rate / FPS),
						GetRandf(0.2f*Scale_Rate) / FPS);
				}
				this->m_Line[this->m_LineSel] = pPos;
				++this->m_LineSel %= this->m_Line.size();
				m_LinePer = std::clamp(m_LinePer - 1.f / FPS / 10.f, 0.f, 1.f);
			}
			void		DrawMuzzleSmoke() noexcept {
				SetUseLighting(FALSE);
				int max = (int)(this->m_Line.size());
				int min = 1 + (int)((1.f - m_LinePer) * (float)max);
				for (int i = max - 1; i >= min; i--) {
					int LS = (i + this->m_LineSel);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(128.f*((float)(i - min) / max)));
					auto p1 = (LS - 1) % max;
					auto p2 = LS % max;
					if (CheckCameraViewClip_Box(
						this->m_Line[p1].get(),
						this->m_Line[p2].get()) == FALSE
						) {
						DrawCapsule3D(this->m_Line[p1].get(), this->m_Line[p2].get(), (0.00762f)*Scale_Rate*1.f*((float)(i - min) / max), 3,
							GetColor(192, 128, 128),
							GetColor(96, 96, 64),
							TRUE);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetUseLighting(TRUE);
			}
		};
		class FallMagControl {
		private:
			std::shared_ptr<MagazineClass>	m_FallMag_Ptr{ nullptr };		//刺さっているマガジン
		protected:
			void		InitFallMagControl(const std::string& pPath) {
				auto* ObjMngr = ObjectManager::Instance();
				this->m_FallMag_Ptr = (std::shared_ptr<MagazineClass>&)(*ObjMngr->AddObject(ObjType::Magazine, PHYSICS_SETUP::DISABLE, false, pPath.c_str()));
				this->m_FallMag_Ptr->SetIsHand(false);
			}
			void		SetFallMagControl(const MATRIX_ref& pMat) {
				float Spd = Scale_Rate * 3.f / 60.f;
				this->m_FallMag_Ptr->SetFall(pMat.pos(), pMat.GetRot(), pMat.yvec()*-1.f*Spd);
			}
		};
		class FallCartControl {
		private:
			float							m_CartInterval{ 0.f };
			std::array<std::shared_ptr<CartClass>, 4> m_CartPtr;
			int								m_NowShotCart{ 0 };
		public:
			void		SetCart(void) noexcept {
				if (this->m_CartInterval == 0.f) {
					this->m_CartInterval = -1.f;//負の値なら何でもよい
				}
			}
		protected:
			void		InitFallCartControl(const std::string& pPath) {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& c : m_CartPtr) {
					c = (std::shared_ptr<CartClass>&)(*ObjMngr->AddObject(ObjType::Cart, PHYSICS_SETUP::DISABLE, false, pPath.c_str(), "ammo"));
				}
			}
			void		ExecuteFallCartControl(const VECTOR_ref& pPos, const MATRIX_ref& pMat, const VECTOR_ref& pVec) {
				if (this->m_CartInterval < -0.5f) {
					this->m_CartInterval = 0.1f;

					float Spd = Scale_Rate * 2.f / 60.f;
					this->m_CartPtr[this->m_NowShotCart]->Set(pPos, pMat.GetRot(), pVec*Spd);
					++this->m_NowShotCart %= this->m_CartPtr.size();
				}
				else {
					this->m_CartInterval = std::max(this->m_CartInterval - 1.f / FPS, 0.f);
				}
			}
		};
		class HaveMagControl {
		private:
			std::shared_ptr<MagazineClass>	m_Mag_Ptr{ nullptr };			//刺さっているマガジン
		public:
			const auto HasMagazine(void) const noexcept { return (this->m_Mag_Ptr.get() != nullptr); }

			const auto GetIsMagEmpty(void) const noexcept { return HasMagazine() ? this->m_Mag_Ptr->IsEmpty() : true; }//次弾がない
			const auto GetIsMagFull(void) const noexcept { return HasMagazine() ? this->m_Mag_Ptr->IsFull() : false; }
			const auto GetAmmoAll(void) const noexcept { return HasMagazine() ? this->m_Mag_Ptr->GetAmmoAll() : 0; }
			const auto GetAmmoNumHaveMagControl(void) const noexcept { return HasMagazine() ? this->m_Mag_Ptr->GetAmmoNum() : 0; }
			const auto&GetAmmoSpecMagTop(void) const noexcept { return this->m_Mag_Ptr->GetAmmoSpec(); }
		public:
			void		UpdateMagMove(const MATRIX_ref& pMat) { this->m_Mag_Ptr->SetMove(pMat.GetRot(), pMat.pos()); }
			void		SetAmmoHandMatrix(const MATRIX_ref& value, float pPer) noexcept { this->m_Mag_Ptr->SetHandMatrix(value, pPer); }
		public:
			void		AddOnceMag() noexcept { this->m_Mag_Ptr->AddAmmo(); }
			void		SubOnceMag() noexcept { this->m_Mag_Ptr->SubAmmo(); }
			void		FillMag() noexcept{ this->m_Mag_Ptr->SetAmmo(GetAmmoAll()); }
		protected:
			void		HaveMag(const std::string& pPath, RELOADTYPE ReloadType) {
				if (!HasMagazine()) {
					auto* ObjMngr = ObjectManager::Instance();
					this->m_Mag_Ptr = (std::shared_ptr<MagazineClass>&)(*ObjMngr->AddObject(ObjType::Magazine, PHYSICS_SETUP::DISABLE, false, pPath.c_str()));
					this->m_Mag_Ptr->SetIsHand(true);
					this->m_Mag_Ptr->SetAmmo(GetAmmoAll());
					this->m_Mag_Ptr->SetReloadType(ReloadType);
				}
			}
			void		DisposeHaveMagControl() {
				if (HasMagazine()) {
					this->m_Mag_Ptr.reset();
				}
			}
		};
		class SlotPartsControl {
		private:
			std::shared_ptr<LowerClass>	m_Lower_Ptr{ nullptr };
			std::shared_ptr<UpperClass>	m_Upper_Ptr{ nullptr };
			std::shared_ptr<BarrelClass>	m_Barrel_Ptr{ nullptr };
		public:
			const auto HasParts(ObjType objType) const noexcept {
				switch (objType) {
				case FPS_n2::Sceneclass::ObjType::Magazine:
					break;
				case FPS_n2::Sceneclass::ObjType::Lower:
					return (this->m_Lower_Ptr.get() != nullptr);
				case FPS_n2::Sceneclass::ObjType::Upper:
					return (this->m_Upper_Ptr.get() != nullptr);
				case FPS_n2::Sceneclass::ObjType::Barrel:
					return (this->m_Barrel_Ptr.get() != nullptr);
				default:
					break;
				}
				return (this->m_Lower_Ptr.get() != nullptr);
			}
			std::shared_ptr<ModClass>& SetPartsPtr(ObjType objType) noexcept {
				switch (objType) {
				case FPS_n2::Sceneclass::ObjType::Magazine:
					break;
				case FPS_n2::Sceneclass::ObjType::Lower:
					return (std::shared_ptr<ModClass>&)this->m_Lower_Ptr;
				case FPS_n2::Sceneclass::ObjType::Upper:
					return (std::shared_ptr<ModClass>&)this->m_Upper_Ptr;
				case FPS_n2::Sceneclass::ObjType::Barrel:
					return (std::shared_ptr<ModClass>&)this->m_Barrel_Ptr;
				default:
					break;
				}
				return (std::shared_ptr<ModClass>&)this->m_Lower_Ptr;
			}
			const std::shared_ptr<ModClass>& GetPartsPtr(ObjType objType) const noexcept {
				switch (objType) {
				case FPS_n2::Sceneclass::ObjType::Magazine:
					break;
				case FPS_n2::Sceneclass::ObjType::Lower:
					return (std::shared_ptr<ModClass>&)this->m_Lower_Ptr;
				case FPS_n2::Sceneclass::ObjType::Upper:
					return (std::shared_ptr<ModClass>&)this->m_Upper_Ptr;
				case FPS_n2::Sceneclass::ObjType::Barrel:
					return (std::shared_ptr<ModClass>&)this->m_Barrel_Ptr;
				default:
					break;
				}
				return (std::shared_ptr<ModClass>&)this->m_Lower_Ptr;
			}
			
		public:
			void		UpdatePartsAnim(const MV1& pParent) {
				for (int loop = 0; loop < (int)ObjType::Max; loop++) {
					if (HasParts((ObjType)loop)) {
						for (int i = 0; i < pParent.get_anime().size(); i++) {
							GetPartsPtr((ObjType)loop)->GetAnime((GunAnimeID)i).per = pParent.getanime(i).per;
							GetPartsPtr((ObjType)loop)->GetAnime((GunAnimeID)i).time = pParent.getanime(i).time;
						}
						GetPartsPtr((ObjType)loop)->ResetFrameLocalMat(GunFrame::Center);
						GetPartsPtr((ObjType)loop)->GetObj().work_anime();
						GetPartsPtr((ObjType)loop)->SetFrameLocalMat(GunFrame::Center, GetPartsPtr((ObjType)loop)->GetFrameLocalMat(GunFrame::Center).GetRot());//1のフレーム移動量を無視する
					}
				}
			}
			void		UpdatePartsMove(const MATRIX_ref& pMat, ObjType objType) {
				if (HasParts(objType)) {
					GetPartsPtr(objType)->SetMove(pMat.GetRot(), pMat.pos());
				}
			}
		protected:
			void		SetParts(const std::string& pPath, ObjType objType) {
				if (!HasParts(objType)) {
					auto* ObjMngr = ObjectManager::Instance();
					switch (objType) {
					case FPS_n2::Sceneclass::ObjType::Magazine:
						break;
					case FPS_n2::Sceneclass::ObjType::Lower:
						this->m_Lower_Ptr = (std::shared_ptr<LowerClass>&)(*ObjMngr->AddObject(ObjType::Lower, PHYSICS_SETUP::DISABLE, false, pPath.c_str()));
						break;
					case FPS_n2::Sceneclass::ObjType::Upper:
						this->m_Upper_Ptr = (std::shared_ptr<UpperClass>&)(*ObjMngr->AddObject(ObjType::Upper, PHYSICS_SETUP::DISABLE, false, pPath.c_str()));
						break;
					case FPS_n2::Sceneclass::ObjType::Barrel:
						this->m_Barrel_Ptr = (std::shared_ptr<BarrelClass>&)(*ObjMngr->AddObject(ObjType::Barrel, PHYSICS_SETUP::DISABLE, false, pPath.c_str()));
						break;
					default:
						break;
					}
				}
			}
			void		DisposeSlotPartsControl() {
				this->m_Lower_Ptr.reset();
			}
		};
	};
};
