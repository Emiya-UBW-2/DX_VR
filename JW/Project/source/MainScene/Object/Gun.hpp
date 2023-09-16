#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../MainScene/Object/ObjectBase.hpp"
#include "../../MainScene/Object/Character_before.hpp"
#include "../../MainScene/Object/GunEnum.hpp"
#include "../../MainScene/Object/AmmoData.hpp"
#include "../../MainScene/Object/Ammo.hpp"
#include "../../MainScene/Object/Magazine.hpp"
#include "../../MainScene/Object/Cart.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass {
		private:
			GraphHandle						m_GunPic;						//
			GraphHandle						m_reticlePic;					//
			GunAnimeID					m_ShotPhase{ GunAnimeID::Base };//
			std::vector<std::string>		m_MagName;						//
			std::vector<SHOTTYPE>			m_ShotType;						//
			std::vector<RELOADTYPE>			m_ReloadType;					//
			int								m_HumanAnimType{ 0 };			//
			float							m_ReticleSize{ 1.f };
			bool							m_IsShot{ false };				//
			bool							m_in_chamber{ true };			//É`ÉÉÉìÉoÅ[ì‡Ç…íeÇ™Ç†ÇÈÇ©
			int								m_boltSoundSequence{ -1 };		//ÉTÉEÉìÉh
			std::shared_ptr<MagazineClass>	m_Mag_Ptr{ nullptr };			//éhÇ≥Ç¡ÇƒÇ¢ÇÈÉ}ÉKÉWÉì
			std::shared_ptr<MagazineClass>	m_FallMag_Ptr{ nullptr };		//éhÇ≥Ç¡ÇƒÇ¢ÇÈÉ}ÉKÉWÉì
			std::shared_ptr<AmmoData>		m_AmmoData{ nullptr };			//
			bool							m_IsChamberMove{ false };		//É`ÉÉÉìÉoÅ[ì‡Ç…íeÇ™Ç†ÇÈÇ©
			float							m_ChamberMovePer{ 0.f };
			bool							m_UseMoveParts{ false };
			std::vector<GunSoundSet>		m_GunSoundSet;
			int								m_SoundSel{ 0 };
			//
			float m_CartInterval{ 0.f };
			std::array<std::shared_ptr<CartClass>, 4> m_CartPtr;
			int m_NowShotCart{ 0 };
			//
			std::array<VECTOR_ref, 15> m_Line;
			int m_LineSel = 0;
			float m_LinePer{ 0.f };
		public://ÉQÉbÉ^Å[
			const auto GetIsFrameActive(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first >= 0; }
			const auto GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			void ResetFrameLocalMat(GunFrame frame) noexcept { GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * this->m_Frames[(int)frame].second); }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType[0]; }
			const auto& GetReloadType(void) const noexcept { return this->m_ReloadType[0]; }
			const auto& GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const auto& GetNowAnime(void) noexcept { return GetObj().get_anime((size_t)this->m_ShotPhase); }
			void SetIsShot(bool value) noexcept { this->m_IsShot = value; }
			void SetUseMoveParts(bool value) noexcept { this->m_UseMoveParts = value; }
			const auto GetScopePos(void) const noexcept { return GetFrameWorldMat(GunFrame::Eyepos).pos(); }
			const auto GetLensPos(void) const noexcept { return GetFrameWorldMat(GunFrame::Lens).pos(); }
			const auto GetReticleSize(void) const noexcept { return this->m_ReticleSize; }
			const auto GetReticlePos(void) const noexcept { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize(void) const noexcept { return GetFrameWorldMat(GunFrame::LensSize).pos(); }
			const auto GetMuzzleMatrix(void) const noexcept { return GetFrameWorldMat(GunFrame::Muzzle); }
			const auto GetIsMagEmpty(void) const noexcept { return (this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->IsEmpty() : true; }//éüíeÇ™Ç»Ç¢
			const auto GetIsMagFull(void) const noexcept { return (this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->IsFull() : false; }
			const auto GetAmmoAll(void) const noexcept { return (this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->GetAmmoAll() : 0; }
			const auto GetAmmoNum(void) const noexcept { return ((this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->GetAmmoNum() : 0) + (this->m_in_chamber ? 1 : 0); }
			const auto GetCanShot(void) const noexcept { return this->m_in_chamber; }
			const auto GetName(void) const noexcept {//"data/gun/AR15/"
				auto now = 9;//"data/gun/"
				return this->m_FilePath.substr(now, m_FilePath.rfind("/") - now);
			}
			const auto& GetReticlePic(void) const noexcept { return this->m_reticlePic; }
			const auto& GetGunPic(void) const noexcept { return this->m_GunPic; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetIsShot(void) const noexcept { return this->m_IsShot; }
			void SetGunMatrix(const MATRIX_ref& value, GunAnimeID pShotPhase) noexcept {
				SetMove(value.GetRot(), value.pos());
				this->m_Mag_Ptr->SetMove(GetFrameWorldMat(GunFrame::Magpos).GetRot(), GetFrameWorldMat(GunFrame::Magpos).pos());
				this->m_ShotPhase = pShotPhase;
			}
			void SetMagazine(const char*) noexcept;//É}ÉKÉWÉìÇÃíÖíE
			void ExecuteCartInChamber(void) noexcept;//É`ÉÉÉìÉoÅ[Ç÷ÇÃëïíeÅAîrèo
			void SetBullet(void) noexcept;//î≠ñC

			const auto GetCartMat(void) noexcept { return GetFrameWorldMat(GunFrame::Cart); }
			const auto GetCartVec(void) noexcept { return (GetFrameWorldMat(GunFrame::CartVec).pos() - GetCartMat().pos()).Norm(); }

			void SetCart(void) noexcept {
				if (this->m_CartInterval == 0.f) {
					this->m_CartInterval = -1.f;//ïâÇÃílÇ»ÇÁâΩÇ≈Ç‡ÇÊÇ¢
				}
			}
			void ExecuteCart(void) noexcept {
				if (this->m_CartInterval < -0.5f) {
					this->m_CartInterval = 0.1f;

					float Spd = Scale_Rate*2.f / 60.f;
					this->m_CartPtr[this->m_NowShotCart]->Set(GetCartMat().pos(), GetMuzzleMatrix().GetRot(),
						GetCartVec()*Spd);
					++this->m_NowShotCart %= this->m_CartPtr.size();
				}
				else {
					this->m_CartInterval = std::max(this->m_CartInterval - 1.f / FPS, 0.f);
				}
			}

			void SetAmmoHandMatrix(const MATRIX_ref& value, float pPer) noexcept {
				this->m_Mag_Ptr->SetHandMatrix(value, pPer, GetReloadType());
			}

			void SetMagFall() noexcept {
				float Spd = Scale_Rate * 3.f / 60.f;
				this->m_FallMag_Ptr->SetFall(GetFrameWorldMat(GunFrame::Magpos).pos(), GetFrameWorldMat(GunFrame::Magpos).GetRot(),
					GetMove().mat.yvec()*-1.f*Spd
				);
			}
			
		public:
			GunClass(void) noexcept { this->m_objType = ObjType::Gun; }
			~GunClass(void) noexcept {}
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					std::string stp;
					while (true) {
						stp = getparams::Getstr(mdata);
						if (stp.find("usemag" + std::to_string(this->m_MagName.size())) == std::string::npos) {
							break;
						}
						this->m_MagName.resize(this->m_MagName.size() + 1);
						this->m_MagName.back() = getparams::getright(stp);
					}
					{
						auto sel = getparams::getright(stp);
						if (sel == "FULL") {
							this->m_ShotType.emplace_back(SHOTTYPE::FULL);
						}
						else if (sel == "SEMI") {
							this->m_ShotType.emplace_back(SHOTTYPE::SEMI);
						}
						else if (sel == "BOLT") {
							this->m_ShotType.emplace_back(SHOTTYPE::BOLT);
						}
					}
					this->m_HumanAnimType = getparams::_int(mdata);
					this->m_ReticleSize = getparams::_float(mdata);
					this->m_SoundSel = getparams::_int(mdata);
					{
						auto sel = getparams::_str(mdata);
						if (sel == "MAG") {
							this->m_ReloadType.emplace_back(RELOADTYPE::MAG);
						}
						else if (sel == "AMMO") {
							this->m_ReloadType.emplace_back(RELOADTYPE::AMMO);
						}
					}

					FileRead_close(mdata);
				}
				SetMagazine(this->m_MagName[0].c_str());
				//
				m_GunSoundSet.clear();
				//M4
				m_GunSoundSet.resize(m_GunSoundSet.size() + 1);
				m_GunSoundSet.back().m_Cock[0] = SoundEnum::Cocking2_0;
				m_GunSoundSet.back().m_Cock[1] = SoundEnum::Cocking2_1;
				m_GunSoundSet.back().m_Shot = SoundEnum::Shot2;
				m_GunSoundSet.back().m_Unload = SoundEnum::Unload2;
				m_GunSoundSet.back().m_Load = SoundEnum::Load2;
				//ÉnÉìÉhÉKÉì
				m_GunSoundSet.resize(m_GunSoundSet.size() + 1);
				m_GunSoundSet.back().m_Cock[0] = SoundEnum::Cocking3_0;
				m_GunSoundSet.back().m_Cock[1] = SoundEnum::Cocking3_1;
				m_GunSoundSet.back().m_Shot = SoundEnum::Shot3;
				m_GunSoundSet.back().m_Unload = SoundEnum::Unload3;
				m_GunSoundSet.back().m_Load = SoundEnum::Load3;

			}
			void			FirstExecute(void) noexcept override {
				auto SE = SoundPool::Instance();
				if (this->m_IsFirstLoop) {
					//
					for (auto& l : this->m_Line) {
						l = GetFrameWorldMat(GunFrame::Muzzle).pos();
					}
				}
				else {
					for (auto& l : this->m_Line) {
						l += VECTOR_ref::vget(
							GetRandf(0.6f*Scale_Rate / FPS),
							1.2f*Scale_Rate / FPS + GetRandf(0.8f*Scale_Rate / FPS),
							GetRandf(0.6f*Scale_Rate) / FPS);
					}
					this->m_Line[this->m_LineSel] = GetFrameWorldMat(GunFrame::Muzzle).pos();
					++this->m_LineSel %= this->m_Line.size();
				}

				GunAnimeID Sel = (GunAnimeID)(this->m_ShotPhase);
				for (int i = 0; i < GetObj().get_anime().size(); i++) {
					if (Sel == (GunAnimeID)i) {
						if ((GunAnimeID)i == GunAnimeID::ReloadOne) {
							if (GetObj().get_anime(i).time == 0.f) {
								switch (GetReloadType()) {
								case RELOADTYPE::MAG:
									this->m_Mag_Ptr->SetAmmo(GetAmmoAll());
									break;
								case RELOADTYPE::AMMO:
									this->m_Mag_Ptr->AddAmmo();
									break;
								default:
									break;
								}
							}
						}
						GetObj().get_anime(i).per = 1.f;
						if (Sel == GunAnimeID::Shot) {
							SetAnimOnce(i, 1.5f);
						}
						else {
							SetAnimOnce(i, 1.5f);
						}
					}
					else {
						if ((GunAnimeID)i == GunAnimeID::Base) {
							GetObj().get_anime(i).per = 1.f;
						}
						else if ((GunAnimeID)i == GunAnimeID::Open) {
							bool isHit = false;
							if (GetAmmoNum() == 0) {
								isHit = true;
							}
							else {
								if (Sel == GunAnimeID::Cocking) {
									if (GetObj().get_anime((int)Sel).time <= 22.f) {
										isHit = true;
									}
								}
								else if (GetIsMagFull() && !GetCanShot()) {
									isHit = true;
								}
							}
							GetObj().get_anime(i).per = std::clamp(GetObj().get_anime(i).per + 10.f / FPS * (isHit ? 1.f : -1.f), 0.f, 1.f);
						}
						else {
							GetObj().get_anime(i).Reset();
						}
					}
				}
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					//0
				{
					if ((5.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 6.f)) {
						if (m_boltSoundSequence != 1) {
							m_boltSoundSequence = 1;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[0]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((11.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 12.f)) {
						if (m_boltSoundSequence != 2) {
							m_boltSoundSequence = 2;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[1]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((28.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 29.f)) {
						if (m_boltSoundSequence != 3) {
							m_boltSoundSequence = 3;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[2]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((36.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 37.f)) {
						if (m_boltSoundSequence != 4) {
							m_boltSoundSequence = 4;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[3]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}
				//1
				{
					if ((5.f < GetAnime(GunAnimeID::ReloadStart_Empty).time && GetAnime(GunAnimeID::ReloadStart_Empty).time < 6.f)) {
						if (m_boltSoundSequence != 5) {
							m_boltSoundSequence = 5;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[0]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((11.f < GetAnime(GunAnimeID::ReloadStart_Empty).time && GetAnime(GunAnimeID::ReloadStart_Empty).time < 12.f)) {
						if (m_boltSoundSequence != 6) {
							m_boltSoundSequence = 6;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[1]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}

					if ((5.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 6.f)) {
						if (m_boltSoundSequence != 5) {
							m_boltSoundSequence = 5;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[0]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((11.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 12.f)) {
						if (m_boltSoundSequence != 6) {
							m_boltSoundSequence = 6;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[1]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}
				//2
				{
					if ((10.f < GetAnime(GunAnimeID::ReloadOne).time && GetAnime(GunAnimeID::ReloadOne).time < 11.f)) {
						if (m_boltSoundSequence != 7) {
							m_boltSoundSequence = 7;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[2]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					else if (GetAnime(GunAnimeID::ReloadOne).per != 0.f) {
						m_boltSoundSequence = -1;
					}
				}
				//3
				{
					if ((8.f < GetAnime(GunAnimeID::ReloadEnd).time && GetAnime(GunAnimeID::ReloadEnd).time < 9.f)) {
						if (m_boltSoundSequence != 8) {
							m_boltSoundSequence = 8;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[2]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((16.f < GetAnime(GunAnimeID::ReloadEnd).time && GetAnime(GunAnimeID::ReloadEnd).time < 17.f)) {
						if (m_boltSoundSequence != 9) {
							m_boltSoundSequence = 9;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[3]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}
				//4
				{
					if ((0.5f < GetAnime(GunAnimeID::Shot).time && GetAnime(GunAnimeID::Shot).time < 1.f)) {
						if (m_boltSoundSequence != 1) {
							m_boltSoundSequence = 1;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[0]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((2.f < GetAnime(GunAnimeID::Shot).time && GetAnime(GunAnimeID::Shot).time < 3.f)) {
						if (m_boltSoundSequence != 2) {
							m_boltSoundSequence = 2;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[1]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((5.f < GetAnime(GunAnimeID::Shot).time && GetAnime(GunAnimeID::Shot).time < 6.f)) {
						if (m_boltSoundSequence != 3) {
							m_boltSoundSequence = 3;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[2]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((7.5f < GetAnime(GunAnimeID::Shot).time && GetAnime(GunAnimeID::Shot).time < 8.5f)) {
						if (m_boltSoundSequence != 4) {
							m_boltSoundSequence = 4;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[3]).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}
				break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					//0
				{
					if ((8.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 9.f)) {
						if (m_boltSoundSequence != 1) {
							m_boltSoundSequence = 1;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[0]).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
					if ((18.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 19.f)) {
						if (m_boltSoundSequence != 2) {
							m_boltSoundSequence = 2;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Cock[1]).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
				}
				//1
				{
					if ((0.f < GetAnime(GunAnimeID::ReloadStart_Empty).time && GetAnime(GunAnimeID::ReloadStart_Empty).time < 1.f)) {
						if (m_boltSoundSequence != 3) {
							m_boltSoundSequence = 3;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Unload).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
					if ((0.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 1.f)) {
						if (m_boltSoundSequence != 3) {
							m_boltSoundSequence = 3;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Unload).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
				}
				//3
				{
					if ((0.f < GetAnime(GunAnimeID::ReloadEnd).time && GetAnime(GunAnimeID::ReloadEnd).time < 1.f)) {
						if (m_boltSoundSequence != 5) {
							m_boltSoundSequence = 5;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Load).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
				}
				//6
				{
					if ((0.f < GetAnime(GunAnimeID::CheckStart).time && GetAnime(GunAnimeID::CheckStart).time < 1.f)) {
						if (m_boltSoundSequence != 6) {
							m_boltSoundSequence = 6;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Unload).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
				}
				//7
				{
					if ((25.f < GetAnime(GunAnimeID::Checking).time && GetAnime(GunAnimeID::Checking).time < 26.f)) {
						if (m_boltSoundSequence != 7) {
							m_boltSoundSequence = 7;
							SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Load).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
				}
				break;
				default:
					break;
				}
				{
					ResetFrameLocalMat(GunFrame::Center);
					GetObj().work_anime();
					SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1ÇÃÉtÉåÅ[ÉÄà⁄ìÆó Çñ≥éãÇ∑ÇÈ
				}
				if (this->m_UseMoveParts) {
					ResetFrameLocalMat(GunFrame::MovePoint);
					auto baselocalmat = GetFrameLocalMat(GunFrame::MovePoint);
					auto baseworldmat = GetFrameWorldMat(GunFrame::MovePoint);
					auto axisworldmat = GetFrameWorldMat(GunFrame::MoveAxis);

					VECTOR_ref pp = baseworldmat.zvec()*-1.f;
					auto yrad = atan2f(-pp.x(), -pp.z());

					auto vec = MATRIX_ref::Vtrans(baseworldmat.pos() - axisworldmat.pos(), MATRIX_ref::RotY(yrad).Inverse());

					float rad = deg2rad(60.f);

					SetFrameLocalMat(GunFrame::MovePoint,
						MATRIX_ref::Mtrans(
							vec*-2.f
							+ VECTOR_ref::right() * vec.size() * std::cosf(rad)
							+ VECTOR_ref::up()*-1.5f * vec.size() * std::sinf(rad)
						)
						*
						MATRIX_ref::RotY(yrad)*
						baseworldmat.GetRot().Inverse()*baselocalmat
					);
				}
				else {
					ResetFrameLocalMat(GunFrame::MovePoint);
				}
				//ã§í 
				ObjectBaseClass::FirstExecute();
				//íeñÚÇÃââéZ
				ExecuteCartInChamber();
				ExecuteCart();
				m_LinePer = std::clamp(m_LinePer - 1.f / FPS / 10.f, 0.f, 1.f);
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.5f*Scale_Rate, -0.5f*Scale_Rate, -0.5f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.5f*Scale_Rate, 0.5f*Scale_Rate, 0.5f*Scale_Rate)).get()) == FALSE
						) {
						if (isDrawSemiTrans) {
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
						for (int i = 0; i < this->GetObj().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
				}
			}
			void			Dispose(void) noexcept override {
				this->m_MagName.clear();
			}
		public:
			void			LoadReticle(void) noexcept {
				SetUseASyncLoadFlag(TRUE);
				this->m_reticlePic = GraphHandle::Load(this->m_FilePath + "reticle.png");
				this->m_GunPic = GraphHandle::Load(this->m_FilePath + "pic.bmp");
				SetUseASyncLoadFlag(FALSE);
			}
		};
	};
};
