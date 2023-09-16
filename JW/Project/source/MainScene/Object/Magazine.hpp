#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Object/ObjectBase.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
		private:
			bool										m_IsHand{ true };

			std::vector<std::shared_ptr<AmmoData>>		m_AmmoSpec;
			int											m_Capacity{ 0 };
			int											m_CapacityMax{ 0 };
			MATRIX_ref									HandMatrix;
			float										HandPer{ 0.f };
			RELOADTYPE									m_ReloadTypeBuf{ RELOADTYPE::MAG };

			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			bool m_SoundSwitch{ false };
		public://ƒQƒbƒ^[
			void			SetHandMatrix(const MATRIX_ref& value, float pPer, RELOADTYPE ReloadType) noexcept {
				this->HandMatrix = value;
				this->HandPer = pPer;
				this->m_ReloadTypeBuf = ReloadType;
			}

			void			SetIsHand(bool value) noexcept {
				this->m_IsHand = value;
				if (!this->m_IsHand) {
					this->m_Timer = 2.f;
					this->m_IsActive = false;
				}
			}
			void SetFall(const VECTOR_ref& pos, const MATRIX_ref& mat, const VECTOR_ref& vec) noexcept {
				if (!this->m_IsHand) {
					this->m_IsActive = true;
					this->m_move.pos = pos;
					this->m_move.vec = vec;
					this->m_yAdd = 0.f;
					this->m_move.repos = this->m_move.pos;
					this->m_move.mat = mat;
					this->m_Timer = 0.f;
					this->m_SoundSwitch = true;
				}
			}

			void			SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, m_CapacityMax); }
			void			SubAmmo(void) noexcept { SetAmmo(this->m_Capacity - 1); }
			void			AddAmmo(void) noexcept { SetAmmo(this->m_Capacity + 1); }
			const auto		IsEmpty(void) const noexcept { return this->m_Capacity == 0; }
			const auto		IsFull(void) const noexcept { return this->m_Capacity == this->m_CapacityMax; }
			const auto&		GetAmmoSpec(void) const noexcept { return this->m_AmmoSpec[0]; }
			const auto&		GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto&		GetAmmoAll(void) const noexcept { return  this->m_CapacityMax; }
		public:
			MagazineClass(void) noexcept { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) noexcept { }
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					this->m_CapacityMax = (HitPoint)getparams::_int(mdata);		//‘’e”
					while (true) {
						auto stp = getparams::Getstr(mdata);
						if (stp.find("useammo" + std::to_string(this->m_AmmoSpec.size())) == std::string::npos) {
							break;
						}
						this->m_AmmoSpec.emplace_back(std::make_shared<AmmoData>());
						this->m_AmmoSpec.back()->Set("data/ammo/" + getparams::getright(stp) + "/");
						//"data/ammo/"+ getparams::getright(stp)
					}
					FileRead_close(mdata);
				}
				this->m_Capacity = this->m_CapacityMax;
			}
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.1f*Scale_Rate, -0.1f*Scale_Rate, -0.1f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.1f*Scale_Rate, 0.1f*Scale_Rate, 0.1f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose(void) noexcept override {
				this->m_AmmoSpec.clear();
			}
		public:
		};
	};
};
