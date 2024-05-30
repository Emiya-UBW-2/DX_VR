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
			const auto GetFrameWorldMat(WeaponFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame((int)frame)); }
		public:
			void			ResetAnim(void) noexcept { m_UpperAnim = 0.f; }
		public:
			const auto	GetGunTotalTime(EnumWeaponAnimType ID) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[0].Anim.at((int)ID));
				if (!Ptr) { return 0.f; }
				return (float)Ptr->GetTotalTime() / 60.f;
			}
			const auto	GetGunAnimePer(EnumWeaponAnimType ID) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[0].Anim.at((int)ID));
				if (!Ptr) { return 0.f; }
				float totalTime = (float)Ptr->GetTotalTime();
				return (totalTime > 0.f) ? (this->m_UpperAnim / totalTime) : 1.f;
			}
			const auto	GetGunAnime(EnumWeaponAnimType ID, Matrix4x4DX* Ret) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[0].Anim.at((int)ID));
				if (!Ptr) { return false; }
				*Ret = AnimMngr->GetAnimNow(Ptr, this->m_UpperAnim).GetMatrix();
				return true;
			}
		public:
			WeaponClass(void) noexcept { this->m_objType = (int)ObjType::Weapon; }
			~WeaponClass(void) noexcept {}
		public:
			int	GetFrameNum() noexcept override { return (int)WeaponFrame::Max; }
			const char*	GetFrameStr(int id) noexcept override { return WeaponFrameName[id]; }


			void			FirstExecute(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				m_UpperAnim += 60.f / DrawParts->GetFps();

				int num = MV1GetMaterialNum(GetObj().get());
				for (int i = 0; i < num; i++) {
					MV1SetMaterialDifColor(GetObj().get(), i, GetColorF(1.f, 1.f, 1.f, 1.f));
					MV1SetMaterialAmbColor(GetObj().get(), i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
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
