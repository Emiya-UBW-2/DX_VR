#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass {
		private:
			GraphHandle						m_reticle;						//
			int								m_ShotPhase{ 0 };				//
			std::vector<std::string>		m_MagName;						//
			bool							m_IsShot{ false };				//
			bool							m_in_chamber{ true };			//É`ÉÉÉìÉoÅ[ì‡Ç…íeÇ™Ç†ÇÈÇ©
			int								m_boltSoundSequence{ -1 };		//ÉTÉEÉìÉh
			std::shared_ptr<MagazineClass>	m_Mag_Ptr{ nullptr };			//éhÇ≥Ç¡ÇƒÇ¢ÇÈÉ}ÉKÉWÉì
			std::shared_ptr<CartClass>		m_CartPtr{ nullptr };			//ñÚ‰∞
			const AmmoData*					m_NowAmmo{ nullptr };			//
			bool							m_IsChamberMove{ false };		//É`ÉÉÉìÉoÅ[ì‡Ç…íeÇ™Ç†ÇÈÇ©
			float							m_ChamberMovePer{ 0.f };
			bool							m_UseMoveParts{ false };
		public://ÉQÉbÉ^Å[
			const auto GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			void ResetFrameLocalMat(GunFrame frame) noexcept { GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * m_Frames[(int)frame].second); }
			const auto& GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const auto& GetNowAnime(void) noexcept { return GetObj().get_anime((size_t)(this->m_ShotPhase - 2)); }
			void SetIsShot(bool value) noexcept { this->m_IsShot = value; }
			void SetUseMoveParts(bool value) noexcept { this->m_UseMoveParts = value; }
			const auto GetScopePos(void) noexcept { return GetFrameWorldMat(GunFrame::Eyepos).pos(); }
			const auto GetLensPos(void) noexcept { return GetFrameWorldMat(GunFrame::Lens).pos(); }
			const auto GetReticlePos(void) noexcept { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize(void) noexcept { return GetFrameWorldMat(GunFrame::LensSize).pos(); }
			const auto GetMuzzleMatrix(void) noexcept { return GetFrameWorldMat(GunFrame::Muzzle); }
			const auto GetIsMagEmpty(void) noexcept { return (this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->IsEmpty() : true; }//éüíeÇ™Ç»Ç¢
			const auto GetIsMagFull(void) noexcept { return (this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->IsFull() : false; }
			const auto GetAmmoAll(void) noexcept { return (this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->GetAmmoAll() : 0; }
			const auto GetAmmoNum(void) noexcept { return ((this->m_Mag_Ptr.get() != nullptr) ? this->m_Mag_Ptr->GetAmmoNum() : 0) + (this->m_in_chamber ? 1 : 0); }
			const auto GetCanshot(void) noexcept { return this->m_in_chamber && (this->m_ShotPhase <= 1); }
			const auto& GetReticle(void) noexcept { return this->m_reticle; }
			const auto& GetIsShot(void) noexcept { return this->m_IsShot; }
			void SetGunMatrix(const MATRIX_ref& value, int pShotPhase) noexcept {
				SetMove(value.GetRot(), value.pos());
				this->m_Mag_Ptr->SetMove(GetFrameWorldMat(GunFrame::Magpos).GetRot(), GetFrameWorldMat(GunFrame::Magpos).pos());
				this->m_ShotPhase = pShotPhase;
			}
			void SetMagazine(const char*) noexcept;//É}ÉKÉWÉìÇÃíÖíE
			void ExecuteCartInChamber(void) noexcept;//É`ÉÉÉìÉoÅ[Ç÷ÇÃëïíeÅAîrèo
			void SetBullet(void) noexcept;//î≠ñC
			void SetAmmoHandMatrix(const MATRIX_ref& value, float pPer) noexcept { this->m_Mag_Ptr->SetHandMatrix(value, pPer); }
		public:
			GunClass(void) noexcept { this->m_objType = ObjType::Gun; }
			~GunClass(void) noexcept {}
		public:
			void Init(void) noexcept override {
				ObjectBaseClass::Init();
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					while (true) {
						auto stp = getparams::Getstr(mdata);
						if (stp.find("usemag" + std::to_string(this->m_MagName.size())) == std::string::npos) {
							break;
						}
						this->m_MagName.resize(this->m_MagName.size() + 1);
						this->m_MagName.back() = getparams::getright(stp);
					}
					FileRead_close(mdata);
				}
				SetMagazine(this->m_MagName[0].c_str());
				//SetMagazine(nullptr);
			}
			void FirstExecute(void) noexcept override {
				auto SE = SoundPool::Instance();
				if (this->m_IsFirstLoop) {
				}
				for (int i = 0; i < GetObj().get_anime().size(); i++) {
					if ((GunAnimeID)(this->m_ShotPhase - 2) == (GunAnimeID)i) {
						if ((GunAnimeID)i == GunAnimeID::ReloadOne) {
							if (GetObj().get_anime(i).time == 0.f) {
								this->m_Mag_Ptr->AddAmmo();
							}
						}
						GetObj().get_anime(i).per = 1.f;
						SetAnimLoop(i, 1.5f);
					}
					else {
						GetObj().get_anime(i).Reset();
					}
				}
				//0
				{
					if ((5.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 6.f)) {
						if (m_boltSoundSequence != 1) {
							m_boltSoundSequence = 1;
							SE->Get((int)SoundEnum::Cocking0).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((11.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 12.f)) {
						if (m_boltSoundSequence != 2) {
							m_boltSoundSequence = 2;
							SE->Get((int)SoundEnum::Cocking1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((28.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 29.f)) {
						if (m_boltSoundSequence != 3) {
							m_boltSoundSequence = 3;
							SE->Get((int)SoundEnum::Cocking2).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((36.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 37.f)) {
						if (m_boltSoundSequence != 4) {
							m_boltSoundSequence = 4;
							SE->Get((int)SoundEnum::Cocking3).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}
				//1
				{
					if ((5.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 6.f)) {
						if (m_boltSoundSequence != 5) {
							m_boltSoundSequence = 5;
							SE->Get((int)SoundEnum::Cocking0).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((11.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 12.f)) {
						if (m_boltSoundSequence != 6) {
							m_boltSoundSequence = 6;
							SE->Get((int)SoundEnum::Cocking1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}
				//2
				{
					if ((10.f < GetAnime(GunAnimeID::ReloadOne).time && GetAnime(GunAnimeID::ReloadOne).time < 11.f)) {
						if (m_boltSoundSequence != 7) {
							m_boltSoundSequence = 7;
							SE->Get((int)SoundEnum::Cocking2).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
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
							SE->Get((int)SoundEnum::Cocking2).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					if ((16.f < GetAnime(GunAnimeID::ReloadEnd).time && GetAnime(GunAnimeID::ReloadEnd).time < 17.f)) {
						if (m_boltSoundSequence != 9) {
							m_boltSoundSequence = 9;
							SE->Get((int)SoundEnum::Cocking3).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
				}

				{
					ResetFrameLocalMat(GunFrame::Center);
					GetObj().work_anime();
					SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1ÇÃÉtÉåÅ[ÉÄà⁄ìÆó Çñ≥éãÇ∑ÇÈ
				}
				if(this->m_UseMoveParts){
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
			}
			void			Dispose(void) noexcept override {
				this->m_MagName.clear();
			}
		public:
			void			LoadReticle(void) noexcept {
				SetUseASyncLoadFlag(TRUE);
				this->m_reticle = GraphHandle::Load(this->m_FilePath + "reticle.png");
				SetUseASyncLoadFlag(FALSE);
			}
		};
	};
};
