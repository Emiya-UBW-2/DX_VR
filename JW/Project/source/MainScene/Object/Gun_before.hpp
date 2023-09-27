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
			std::shared_ptr<CartClass>	m_FallMag_Ptr{ nullptr };		//刺さっているマガジン
		protected:
			void		InitFallMagControl(const std::shared_ptr<BackGroundClassBase>& backGround, const std::string& pPath) {
				auto* ObjMngr = ObjectManager::Instance();
				auto* Ptr = ObjMngr->MakeObject(ObjType::Cart);
				ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), pPath.c_str());
				this->m_FallMag_Ptr = (std::shared_ptr<CartClass>&)(*Ptr);
				this->m_FallMag_Ptr->SetMapCol(backGround);
				(*Ptr)->Init();
			}
			void		SetFallMagControl(const VECTOR_ref& pPos, const MATRIX_ref& pMat, const VECTOR_ref& pVec) {
				this->m_FallMag_Ptr->SetFall(pPos, pMat.GetRot(), pVec,12.f, SoundEnum::MagFall);
			}
		};
		class FallCartControl {
		private:
			std::array<std::shared_ptr<CartClass>, 4>	m_Ptr;
			int											m_Now{ 0 };
		protected:
			void		InitFallCartControl(const std::shared_ptr<BackGroundClassBase>& backGround, const std::string& pPath) {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& c : m_Ptr) {
					auto* Ptr = ObjMngr->MakeObject(ObjType::Cart);
					ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), pPath.c_str());
					c = (std::shared_ptr<CartClass>&)(*Ptr);
					c->SetMapCol(backGround);
					(*Ptr)->Init();
				}
			}
			void		SetFallControl(const VECTOR_ref& pPos, const MATRIX_ref& pMat, const VECTOR_ref& pVec) {
				this->m_Ptr[this->m_Now]->SetFall(pPos, pMat.GetRot(), pVec, 2.f, SoundEnum::CartFall);
				++this->m_Now %= this->m_Ptr.size();
			}
		};

		class SlotPartsControl {
		private:
			std::array<std::shared_ptr<ModClass>, (int)ObjType::Max>	m_Parts_Ptr{ nullptr };
		public:
			const auto							HasParts(ObjType objType) const noexcept { return (this->m_Parts_Ptr[(int)objType].get() != nullptr); }
			std::shared_ptr<ModClass>&			SetPartsPtr(ObjType objType) noexcept { return this->m_Parts_Ptr[(int)objType]; }
			const std::shared_ptr<ModClass>&	GetPartsPtr(ObjType objType) const noexcept { return this->m_Parts_Ptr[(int)objType]; }
		public:
			const std::shared_ptr<MagazineClass>& GetMagazinePtr(void) const noexcept { return (std::shared_ptr<MagazineClass>&)GetPartsPtr(ObjType::Magazine); }
		public:
			void		UpdatePartsAnim(const MV1& pParent) {
				for (int loop = 0; loop < (int)ObjType::Max; loop++) {
					if (this->m_Parts_Ptr[loop]) {
						for (int i = 0; i < this->m_Parts_Ptr[loop]->GetObj().get_anime().size(); i++) {
							this->m_Parts_Ptr[loop]->GetAnime((GunAnimeID)i).per = pParent.getanime(i).per;
							this->m_Parts_Ptr[loop]->GetAnime((GunAnimeID)i).time = pParent.getanime(i).time;
						}
						this->m_Parts_Ptr[loop]->ResetFrameLocalMat(GunFrame::Center);
						this->m_Parts_Ptr[loop]->GetObj().work_anime();
						this->m_Parts_Ptr[loop]->SetFrameLocalMat(GunFrame::Center, this->m_Parts_Ptr[loop]->GetFrameLocalMat(GunFrame::Center).GetRot());//1のフレーム移動量を無視する
					}
				}
			}
			void		UpdatePartsMove(const MATRIX_ref& pMat, ObjType objType) {
				if (this->m_Parts_Ptr[(int)objType]) {
					this->m_Parts_Ptr[(int)objType]->SetMove(pMat.GetRot(), pMat.pos());
				}
			}
		protected:
			void		SetParts(const std::string& pPath, ObjType objType) {
				if (!this->m_Parts_Ptr[(int)objType]) {
					auto* ObjMngr = ObjectManager::Instance();
					auto* Ptr = ObjMngr->MakeObject(objType);
					ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), pPath.c_str());
					this->m_Parts_Ptr[(int)objType] = (std::shared_ptr<ModClass>&)(*Ptr);
					(*Ptr)->Init();
				}
			}
			void		DisposeSlotPartsControl() {
				for (auto& p : this->m_Parts_Ptr) {
					p.reset();
				}
			}
		};
	};
};
