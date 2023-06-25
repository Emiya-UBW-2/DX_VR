#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class WeaponClass : public ObjectBaseClass {
		private:
			GraphHandle						m_WeaponPic;						//
			GraphHandle						m_reticlePic;					//
			int								m_ShotPhase{ 0 };				//
			int								m_HumanAnimType{ 0 };			//
			float							m_ReticleSize{ 1.f };
			bool							m_IsShot{ false };				//
			bool							m_in_chamber{ true };			//�`�����o�[���ɒe�����邩
			int								m_boltSoundSequence{ -1 };		//�T�E���h
			bool							m_IsChamberMove{ false };		//�`�����o�[���ɒe�����邩
			float							m_ChamberMovePer{ 0.f };

			std::vector<WeaponSoundSet>		m_WeaponSoundSet;
			int								m_SoundSel{ 0 };
		public://�Q�b�^�[
			const auto GetIsFrameActive(WeaponFrame frame) const noexcept { return this->m_Frames[(int)frame].first >= 0; }
			const auto GetFrameLocalMat(WeaponFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameLocalMat(WeaponFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto GetFrameWorldMat(WeaponFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto GetParentFrameWorldMat(WeaponFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix((int)GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			void ResetFrameLocalMat(WeaponFrame frame) noexcept { GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void SetFrameLocalMat(WeaponFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * this->m_Frames[(int)frame].second); }
			const auto& GetAnime(WeaponAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const auto& GetNowAnime(void) noexcept {
				size_t animsel = (size_t)(this->m_ShotPhase) - 2;
				return GetObj().get_anime(animsel);
			}
			void SetIsShot(bool value) noexcept { this->m_IsShot = value; }
			const auto GetCanShot(void) const noexcept { return this->m_in_chamber; }
			const auto GetName(void) const noexcept {//"data/Weapon/AR15/"
				auto now = 9;//"data/Weapon/"
				return this->m_FilePath.substr(now, m_FilePath.rfind("/") - now);
			}
			const auto& GetReticlePic(void) const noexcept { return this->m_reticlePic; }
			const auto& GetWeaponPic(void) const noexcept { return this->m_WeaponPic; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetIsShot(void) const noexcept { return this->m_IsShot; }
			void SetWeaponMatrix(const MATRIX_ref& value, int pShotPhase) noexcept {
				SetMove(value.GetRot(), value.pos());
				this->m_ShotPhase = pShotPhase;
			}
		public:
			WeaponClass(void) noexcept { this->m_objType = ObjType::Weapon; }
			~WeaponClass(void) noexcept {}
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					getparams::_str(mdata);
					this->m_SoundSel = getparams::_int(mdata);
					FileRead_close(mdata);
				}
				m_WeaponSoundSet.clear();
				//M4
				m_WeaponSoundSet.resize(m_WeaponSoundSet.size() + 1);
				m_WeaponSoundSet.back().m_Cock[0] = SoundEnum::Cocking2_0;
				m_WeaponSoundSet.back().m_Cock[1] = SoundEnum::Cocking2_1;
				m_WeaponSoundSet.back().m_Shot = SoundEnum::Shot2;
				m_WeaponSoundSet.back().m_Unload = SoundEnum::Unload2;
				m_WeaponSoundSet.back().m_Load = SoundEnum::Load2;
			}
			void			FirstExecute(void) noexcept override {
				auto SE = SoundPool::Instance();
				if (this->m_IsFirstLoop) {
				}

				WeaponAnimeID Sel = (WeaponAnimeID)(this->m_ShotPhase - 2);
				for (int i = 0; i < GetObj().get_anime().size(); i++) {
					if (Sel == (WeaponAnimeID)i) {
						GetObj().get_anime(i).per = 1.f;
						if (Sel == WeaponAnimeID::Shot) {
							SetAnimOnce(i, 1.5f);
						}
						else {
							SetAnimOnce(i, 1.5f);
						}
					}
					else {
						if ((WeaponAnimeID)i == WeaponAnimeID::Base) {
							GetObj().get_anime(i).per = 1.f;
						}
						else {
							GetObj().get_anime(i).Reset();
						}
					}
				}
				//0
				{
					if ((8.f < GetAnime(WeaponAnimeID::Cocking).time && GetAnime(WeaponAnimeID::Cocking).time < 9.f)) {
						if (m_boltSoundSequence != 1) {
							m_boltSoundSequence = 1;
							SE->Get((int)m_WeaponSoundSet[this->m_SoundSel].m_Cock[0]).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
					if ((18.f < GetAnime(WeaponAnimeID::Cocking).time && GetAnime(WeaponAnimeID::Cocking).time < 19.f)) {
						if (m_boltSoundSequence != 2) {
							m_boltSoundSequence = 2;
							SE->Get((int)m_WeaponSoundSet[this->m_SoundSel].m_Cock[1]).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
						}
					}
				}
				{
					ResetFrameLocalMat(WeaponFrame::Center);
					GetObj().work_anime();
					SetFrameLocalMat(WeaponFrame::Center, GetFrameLocalMat(WeaponFrame::Center).GetRot());//1�̃t���[���ړ��ʂ𖳎�����
				}
				//����
				ObjectBaseClass::FirstExecute();
			}
			void			Draw(void) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						int fog_enable;
						int fog_mode;
						int fog_r, fog_g, fog_b;
						float fog_start, fog_end;
						float fog_density;

						fog_enable = GetFogEnable();													// �t�H�O���L�����ǂ������擾����( TRUE:�L��  FALSE:���� )
						fog_mode = GetFogMode();														// �t�H�O���[�h���擾����
						GetFogColor(&fog_r, &fog_g, &fog_b);											// �t�H�O�J���[���擾����
						GetFogStartEnd(&fog_start, &fog_end);											// �t�H�O���n�܂鋗���ƏI�����鋗�����擾����( 0.0f �` 1.0f )
						fog_density = GetFogDensity();													// �t�H�O�̖��x���擾����( 0.0f �` 1.0f )

						SetFogEnable(TRUE);
						SetFogColor(0, 0, 0);
						SetFogStartEnd(Scale_Rate*1.f, Scale_Rate*5.f);
						SetFogEnable(FALSE);
						{
							this->GetObj().DrawModel();
						}
						SetFogEnable(fog_enable);
						SetFogMode(fog_mode);
						SetFogColor(fog_r, fog_g, fog_b);
						SetFogStartEnd(fog_start, fog_end);
						SetFogDensity(fog_density);
					}
				}
			}
			void			Dispose(void) noexcept override {}
		public:
			void			LoadReticle(void) noexcept {
				SetUseASyncLoadFlag(TRUE);
				this->m_reticlePic = GraphHandle::Load(this->m_FilePath + "reticle.png");
				this->m_WeaponPic = GraphHandle::Load(this->m_FilePath + "pic.bmp");
				SetUseASyncLoadFlag(FALSE);
			}
		};
	};
};
