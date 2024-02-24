#pragma once
#include	"../../Header.hpp"

#include "../../ObjectManager.hpp"
#include "FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		class FallControl {
		private:
			std::array<std::shared_ptr<FallObjClass>, 4>	m_Ptr;
			int												m_Now{0};
		public:
			void		Init(const std::shared_ptr<BackGroundClassBase>& backGround, const std::string& pPath) {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& c : m_Ptr) {
					auto* Ptr = ObjMngr->MakeObject(ObjType::FallObj);
					ObjMngr->LoadObjectModel((*Ptr).get(), pPath.c_str());
					MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
					c = (std::shared_ptr<FallObjClass>&)(*Ptr);
					c->SetMapCol(backGround);
					(*Ptr)->Init();
				}
			}
			void		SetFall(const VECTOR_ref& pPos, const MATRIX_ref& pMat, const VECTOR_ref& pVec, float time, SoundEnum sound) {
				this->m_Ptr[this->m_Now]->SetFall(pPos, pMat, pVec, time, sound);
				++this->m_Now %= this->m_Ptr.size();
			}
			void		Dispose() noexcept {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& c : m_Ptr) {
					ObjMngr->DelObj((SharedObj*)&c);
					c.reset();
				}
			}
		};
		//
		class SlotPartsControl {
		private:
			std::array<SharedObj, (int)GunSlot::Max>	m_Parts_Ptr{nullptr};
		public:
			const bool	HasParts(GunSlot objType) const noexcept { return (this->m_Parts_Ptr[(int)objType].get() != nullptr); }
			const auto&	GetPartsPtr(GunSlot objType) const noexcept { return this->m_Parts_Ptr[(int)objType]; }
			const bool	IsEffectParts(GunSlot objType, GunFrame frame) const noexcept;
			const bool	HasFrame(GunFrame frame) const noexcept;
			const bool	GetPartsFrameLocalMat(GunFrame frame, MATRIX_ref* pRet) const noexcept;
			const bool	GetPartsFrameWorldMat(GunFrame frame, MATRIX_ref* pRet) const noexcept;
			void		GetChildPartsList(std::vector<const SharedObj*>* Ret) const noexcept;
			void		ResetPartsFrameLocalMat(GunFrame frame) noexcept;
			void		SetPartsFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept;
			void		SetActive(bool value) noexcept;

			SharedObj& ChangeMagazine(int MagUniqueID);
		public:
			const SharedObj*	SetParts(int uniqueID, GunSlot objType, const MV1& BaseModel);
			void		RemoveParts(GunSlot objType);
			void		UpdatePartsAnim(const MV1& pParent);
			void		UpdatePartsMove(const MATRIX_ref& pMat, GunSlot objType);
			void		DisposeSlotPartsControl();
		};
		//
		class MuzzleSmokeControl {
		private://キャラパラメーター
			std::array<VECTOR_ref, 8>		m_Line;
			int								m_LineSel = 0;
			float							m_LinePer{0.f};
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
	};
};
