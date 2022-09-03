#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
		private:
			std::array<std::shared_ptr<CartClass>, 2> m_Cart;
			int m_Capacity{ 0 };
			int m_CapacityMax{ 0 };
			//èe
			bool ChamberIntime{ false };
			MATRIX_ref ChamberMatrix;
			float ShotPer{ 0.f };
			//
			MATRIX_ref HandMatrix;
			float HandPer{ 0.f };

			std::vector<AmmoData>			m_AmmoSpec;						//
		public://ÉQÉbÉ^Å[
			void			SetHandMatrix(const MATRIX_ref& value, float pPer) noexcept {
				this->HandMatrix = value;
				this->HandPer = pPer;
			}
			void			SetChamberIntime(bool value) noexcept { this->ChamberIntime = value; }
			void			SetChamberMatrix(const MATRIX_ref& value) noexcept { this->ChamberMatrix = value; }
			void			SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, this->m_CapacityMax); }
			void			SubAmmo(void) noexcept { SetAmmo(this->m_Capacity - 1); }
			void			AddAmmo(void) noexcept { SetAmmo(this->m_Capacity + 1); }
			const auto		IsEmpty(void) const noexcept { return this->m_Capacity == 0; }
			const auto		IsFull(void) const noexcept { return this->m_Capacity == this->m_CapacityMax; }
			const auto*		GetAmmoSpec(void) const noexcept { return &(this->m_AmmoSpec[0]); }
			const auto&		GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto&		GetAmmoAll(void) const noexcept { return  this->m_CapacityMax; }

			void			SetCartPtr(void) noexcept;
		public:
			MagazineClass(void) noexcept { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) noexcept { }
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					this->m_CapacityMax = (HitPoint)getparams::_int(mdata);		//ëçíeêî
					while (true) {
						auto stp = getparams::Getstr(mdata);
						if (stp.find("useammo" + std::to_string(this->m_AmmoSpec.size())) == std::string::npos) {
							break;
						}
						this->m_AmmoSpec.resize(this->m_AmmoSpec.size() + 1);
						this->m_AmmoSpec.back().Set("data/ammo/", getparams::getright(stp));
						//"data/ammo/"+ getparams::getright(stp)
					}
					FileRead_close(mdata);
				}
				this->m_Capacity = this->m_CapacityMax;
				SetCartPtr();
			}
			void			FirstExecute(void) noexcept override {
				this->ShotPer = std::clamp(this->ShotPer + 5.f / FPS, 0.f, (this->ChamberIntime) ? 1.f : 0.f);
				for (auto& b : this->m_Cart) {
					int i = (int)(&b - &this->m_Cart.front());
					b->SetActive(i < this->m_Capacity);
					auto mat = GetObj().GetFrameLocalWorldMatrix(1 + i);
					if (i == 0) {
						b->SetMove(
							MATRIX_ref::RotVec2(VECTOR_ref::front(), Lerp(mat.GetRot().zvec(), this->HandMatrix.zvec(), this->HandPer)),
							Lerp(Lerp(mat.pos(), this->ChamberMatrix.pos(), this->ShotPer), this->HandMatrix.pos(), this->HandPer)
						);
					}
					else {
						auto mat2 = GetObj().GetFrameLocalWorldMatrix(i);
						b->SetMove(
							mat.GetRot(),
							Lerp(mat.pos(), mat2.pos(), this->HandPer)
						);
					}
				}
				//ã§í 
				ObjectBaseClass::FirstExecute();
			}
			void			Dispose(void) noexcept override {
				this->m_AmmoSpec.clear();
			}
		public:
		};
	};
};
