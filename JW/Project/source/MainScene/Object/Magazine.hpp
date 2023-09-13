#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
		private:
			int											m_Capacity{ 0 };
			int											m_CapacityMax{ 0 };
			MATRIX_ref									HandMatrix;
			float										HandPer{ 0.f };
			std::vector<std::shared_ptr<AmmoData>>		m_AmmoSpec;
			RELOADTYPE									m_ReloadTypeBuf{ RELOADTYPE::MAG };
		public://ƒQƒbƒ^[
			void			SetHandMatrix(const MATRIX_ref& value, float pPer, RELOADTYPE ReloadType) noexcept {
				this->HandMatrix = value;
				this->HandPer = pPer;
				this->m_ReloadTypeBuf = ReloadType;
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
			void			FirstExecute(void) noexcept override {
				{
					switch (m_ReloadTypeBuf) {
					case RELOADTYPE::MAG:
					{
						{
							auto tmp = GetMove();
							SetMove(
								MATRIX_ref::RotX(deg2rad(-30.f*this->HandPer))*tmp.mat.GetRot(),
								Lerp(tmp.pos, this->HandMatrix.pos(), this->HandPer));
						}
					}
					break;
					case RELOADTYPE::AMMO:
					{
					}
					break;
					default:
						break;
					}
				}
				//‹¤’Ê
				ObjectBaseClass::FirstExecute();
			}
			void			Dispose(void) noexcept override {
				this->m_AmmoSpec.clear();
			}
		public:
		};
	};
};
