#pragma once
#include	"../../Header.hpp"

#include "WeaponEnum.hpp"
#include "CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class WeaponClass : public ObjectBaseClass {
		private:
			float							m_UpperAnim{0.f};
		public://ゲッター
			auto	GetFrameWorldMat(WeaponFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			Vector3DX GetFramePosition(WeaponFrame frame) const noexcept { return MV1GetFramePosition(GetObj_const().GetHandle(), GetFrame(static_cast<int>(frame))); }
			//
		public:
			void			ResetAnim(void) noexcept { m_UpperAnim = 0.f; }
		public:
			auto	GetGunTotalTime(EnumWeaponAnimType ID) const noexcept {
				//銃の位置を指定するアニメ
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[0].Anim.at(static_cast<size_t>(ID)));
				if (!Ptr) { return 0.f; }
				return static_cast<float>(Ptr->GetTotalTime()) / 60.f;
			}
			auto	GetGunAnimePer(EnumWeaponAnimType ID) const noexcept {
				//銃の位置を指定するアニメ
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[0].Anim.at(static_cast<size_t>(ID)));
				if (!Ptr) { return 0.f; }
				float totalTime = static_cast<float>(Ptr->GetTotalTime());
				return (totalTime > 0.f) ? (this->m_UpperAnim / totalTime) : 1.f;
			}
			auto	GetGunAnime(EnumWeaponAnimType ID, Matrix4x4DX* Ret) const noexcept {
				//銃の位置を指定するアニメ
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[0].Anim.at(static_cast<size_t>(ID)));
				if (!Ptr) { return false; }
				*Ret = AnimMngr->GetAnimNow(Ptr, this->m_UpperAnim).GetMatrix();
				return true;
			}
		public:
			WeaponClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Weapon); }
			WeaponClass(const WeaponClass&) = delete;
			WeaponClass(WeaponClass&& o) = delete;
			WeaponClass& operator=(const WeaponClass&) = delete;
			WeaponClass& operator=(WeaponClass&& o) = delete;

			virtual ~WeaponClass(void) noexcept {}
		public:
			int	GetFrameNum() noexcept override { return static_cast<int>(WeaponFrame::Max); }
			const char*	GetFrameStr(int id) noexcept override { return WeaponFrameName[id]; }


			void			FirstExecute(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				m_UpperAnim += 60.f / DrawParts->GetFps();

				int num = MV1GetMaterialNum(GetObj().GetHandle());
				for (int i = 0; i < num; i++) {
					MV1SetMaterialDifColor(GetObj().GetHandle(), i, GetColorF(1.f, 1.f, 1.f, 1.f));
					MV1SetMaterialAmbColor(GetObj().GetHandle(), i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (isDrawSemiTrans) { return; }
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-20, 20, -20)).get(),
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(20, 20, 20)).get()) == FALSE
						) {
						this->GetObj().DrawModel();
					}
				}
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
		};
	};
};
