#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass {
		private:
			GraphHandle						m_reticle;
			int								m_animSel{ 0 };
			std::vector<std::string>		m_MagName;						//
			bool							m_IsShot{ false };
			bool							m_in_chamber{ true };			//チャンバー内に初弾があるか(弾倉最大+1かどうか)
			int								m_boltSoundSequence{ -1 };		//サウンド
			std::shared_ptr<MagazineClass>	m_Mag_Ptr{ nullptr };			//刺さっているマガジン
			std::shared_ptr<CartClass>		m_CartPtr{ nullptr };			//薬莢
			const AmmoData*					m_NowAmmo{ nullptr };
		public://ゲッター
			const auto GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			void ResetFrameLocalMat(GunFrame frame) noexcept { GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * m_Frames[(int)frame].second); }
			const auto GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			void SetIsShot(bool value) noexcept { this->m_IsShot = value; }
			const auto GetScopePos(void) noexcept { return GetFrameWorldMat(GunFrame::Eyepos).pos(); }
			const auto GetLensPos(void) noexcept { return GetFrameWorldMat(GunFrame::Lens).pos(); }
			const auto GetReticlePos(void) noexcept { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize(void) noexcept { return GetFrameWorldMat(GunFrame::LensSize).pos(); }
			const auto GetMuzzleMatrix(void) noexcept { return GetFrameWorldMat(GunFrame::Muzzle); }
			const auto GetCartMat(void) noexcept { return GetFrameWorldMat(GunFrame::Cart); }
			const auto GetCartVec(void) noexcept { return (GetFrameWorldMat(GunFrame::CartVec).pos() - GetCartMat().pos()).Norm(); }
			const auto GetMagMat(void) noexcept { return GetFrameWorldMat(GunFrame::Magpos); }
			const auto GetIsEmpty(void) noexcept { return this->m_Mag_Ptr->IsEmpty(); }
			const auto GetIsMagFull(void) noexcept { return this->m_Mag_Ptr->IsFull(); }
			const auto GetAmmoAll(void) noexcept { return this->m_Mag_Ptr->GetAmmoAll(); }
			const auto GetAmmoNum(void) noexcept { return this->m_Mag_Ptr->GetAmmoNum() + (m_in_chamber ? 1 : 0); }
			const auto GetCanshot(void) noexcept { return !(this->GetAmmoNum() == 0); }
			const auto& GetReticle(void) noexcept { return this->m_reticle; }
			const auto& GetIsShot(void) noexcept { return this->m_IsShot; }
			const auto GetChamberIn(void) noexcept {
				return
					((GetAnime(GunAnimeID::Cocking).per == 1.f) && (GetAnime(GunAnimeID::Cocking).time >= 25.f))
					|| ((GetAnime(GunAnimeID::ReloadEnd).per == 1.f) && (GetAnime(GunAnimeID::ReloadEnd).time >= 5.f));
			}
			void SetGunMatrix(const MATRIX_ref& value, int pBoltAnim) noexcept {
				SetMove(value.GetRot(), value.pos());
				this->m_Mag_Ptr->SetMove(this->GetMagMat().GetRot(), this->GetMagMat().pos());
				this->m_animSel = pBoltAnim;
			}
			void SetMagazine(const char*) noexcept;
			void SetBullet(void) noexcept;
			void SetCart(void) noexcept;
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
			}
			void FirstExecute(void) noexcept override {
				auto SE = SoundPool::Instance();
				if (this->m_IsFirstLoop) {
					this->m_Mag_Ptr->SetAmmo(0);
					this->m_in_chamber = false;
				}
				for (int i = 0; i < GetObj().get_anime().size(); i++) {
					if (this->m_animSel == i) {
						if (i == 2) {
							if (GetObj().get_anime(i).time == 0.f) {
								if (this->GetIsMagFull()) {
									this->m_in_chamber = true;
								}
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
				ResetFrameLocalMat(GunFrame::Center);
				GetObj().work_anime();
				SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1のフレーム移動量を無視する
				if (this->m_CartPtr.get() != nullptr) {
					this->m_CartPtr->SetMove(this->GetCartMat().GetRot(), this->GetCartMat().pos());
				}
				//共通
				ObjectBaseClass::FirstExecute();

				this->m_Mag_Ptr->SetChamberIntime(this->GetChamberIn());
				this->m_Mag_Ptr->SetChamberMatrix(this->GetCartMat());
			}
			void			Dispose(void) noexcept override {
				this->m_MagName.clear();
			}
		public:
			void LoadReticle(void) noexcept {
				SetUseASyncLoadFlag(TRUE);
				this->m_reticle = GraphHandle::Load(this->m_FilePath + "reticle.png");
				SetUseASyncLoadFlag(FALSE);
			}
		};
	};
};
