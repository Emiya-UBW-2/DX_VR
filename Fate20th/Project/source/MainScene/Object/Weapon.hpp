#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class WeaponClass :
			public ObjectBaseClass,
			public EffectControl
		{
		private:
			GraphHandle						m_WeaponPic;					//
			int								m_ShotPhase{ 0 };				//
			int								m_HumanAnimType{ 0 };			//
			bool							m_IsDrawEffectRec{ false };		//
			bool							m_IsDrawEffect{ false };		//
			float							m_EffectAlphaPer{ 0.f };		//
			int								m_boltSoundSequence{ -1 };		//サウンド

			std::vector<WeaponSoundSet>		m_WeaponSoundSet;
			int								m_SoundSel{ 0 };
		public://ゲッター
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
			void SetIsDrawEffect(bool value) noexcept { this->m_IsDrawEffect = value; }
			const auto GetName(void) const noexcept {//"data/Weapon/AR15/"
				auto now = 9;//"data/Weapon/"
				return this->m_FilePath.substr(now, m_FilePath.rfind("/") - now);
			}
			const auto& GetWeaponPic(void) const noexcept { return this->m_WeaponPic; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
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
					this->m_HumanAnimType = getparams::_int(mdata);
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
					this->m_IsDrawEffectRec = this->m_IsDrawEffect;
				}

				if (this->m_IsDrawEffect != this->m_IsDrawEffectRec) {
					if (this->m_IsDrawEffect) {
						EffectControl::SetLoop((EffectResource::Effect)3, GetMatrix().pos());
						EffectControl::SetLoop((EffectResource::Effect)4, GetMatrix().pos());
						EffectControl::SetLoop((EffectResource::Effect)5, GetMatrix().pos());
						this->m_EffectAlphaPer = 1.f;
					}
				}
				this->m_IsDrawEffectRec = this->m_IsDrawEffect;
				if (this->m_IsDrawEffect) {
					EffectControl::Update_LoopEffect((EffectResource::Effect)3, GetMatrix().pos() + GetMatrix().yvec()*Scale_Rate*0.9f, GetMatrix().zvec()*-1.f, 0.12f*0.9f);
					EffectControl::Update_LoopEffect((EffectResource::Effect)4, GetMatrix().pos() + GetMatrix().yvec()*Scale_Rate*0.8f, GetMatrix().zvec()*-1.f, 0.12f*0.6f);
					EffectControl::Update_LoopEffect((EffectResource::Effect)5, GetMatrix().pos() + GetMatrix().yvec()*Scale_Rate*0.6f, GetMatrix().zvec()*-1.f, 0.12f*0.3f);
				}
				else {
					Easing(&this->m_EffectAlphaPer, 0.f, 0.95f, EasingType::OutExpo);
					int Alpha = std::clamp((int)(255.f* this->m_EffectAlphaPer), 0, 255);
					EffectControl::SetEffectColor((EffectResource::Effect)3, 255, 255, 255, Alpha);
					EffectControl::SetEffectColor((EffectResource::Effect)4, 255, 255, 255, Alpha);
					EffectControl::SetEffectColor((EffectResource::Effect)5, 255, 255, 255, Alpha);
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
					SetFrameLocalMat(WeaponFrame::Center, GetFrameLocalMat(WeaponFrame::Center).GetRot());//1のフレーム移動量を無視する
				}
				//共通
				ObjectBaseClass::FirstExecute();
				EffectControl::Execute();
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

						fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
						fog_mode = GetFogMode();														// フォグモードを取得する
						GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
						GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
						fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

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
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
			void			Dispose(void) noexcept override {
				EffectControl::Dispose();
			}
		public:
			void			LoadReticle(void) noexcept {
				SetUseASyncLoadFlag(TRUE);
				this->m_WeaponPic = GraphHandle::Load(this->m_FilePath + "pic.bmp");
				SetUseASyncLoadFlag(FALSE);
			}
		};
	};
};
