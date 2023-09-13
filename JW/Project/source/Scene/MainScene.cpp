#pragma once
#include	"MainScene.hpp"
#include "../sub.hpp"
#include "../ObjectManager.hpp"
#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "../MainScene/Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* AnimMngr = GunAnimManager::Instance();
			SetAmbientLight(
				VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
				GetColorF(0.92f, 0.91f, 0.90f, 0.0f)
			);
			SetFarShadow(
				VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f),
				VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f)
			);
			SetMiddleShadow(
				VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f),
				VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f)
			);
			//Load
			//
			ObjMngr->Init(this->m_BackGround);
			for (int i = 0; i < Chara_num / 2; i++) {
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/Suit/")));
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}
			for (int i = Chara_num / 2; i < Chara_num; i++) {
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/Soldier/")));
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}
			m_Shader[0].Init("CubeMapTestShader_VS.vso", "CubeMapTestShader_PS.pso");
			m_RealTimeCubeMap.Init();
			cubeTex = LoadGraph("data/SkyCube.dds");	// キューブマップテクスチャの読み込み
			for (int i = 0; i < gun_num / 2; i++) {
				ObjMngr->AddObject(ObjType::Gun, "data/gun/M1911/");
			}
			for (int i = gun_num / 2; i < gun_num; i++) {
				ObjMngr->AddObject(ObjType::Gun, "data/gun/M16A4/");
			}
			//ロード
			SetCreate3DSoundFlag(FALSE);
			this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
			this->m_AimOn = SoundHandle::Load("data/Sound/SE/aim_on.wav");
			SetCreate3DSoundFlag(FALSE);
			this->m_Env.vol(64);
			//UI
			this->m_UIclass.Set();

			autoaimpoint_Graph = GraphHandle::Load("data/UI/battle_autoaimpoint.bmp");
			Enemyaimpoint_Graph = GraphHandle::Load("data/UI/battle_enemyaimpoint.bmp");
			hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");

			stand_AnimTime = 5.f;
			//Set
			//人
			for (int loop = 0; loop < (int)EnumGunAnim::Max; loop++) {
				std::string Path = "data/CharaAnime/";
				Path += EnumGunAnimName[loop];
				Path += ".txt";
				AnimMngr->LoadAction(Path.c_str(), (EnumGunAnim)loop);
			}
			for (auto& c : this->character_Pool) {
				size_t index = &c - &this->character_Pool.front();

				VECTOR_ref pos_t;
				float rad_t = 0.f;
				if (index < Chara_num / 2) {
					pos_t = VECTOR_ref::vget(12.f*Scale_Rate - (float)(index)*2.f*Scale_Rate, 0.f, 12.f*Scale_Rate);
					rad_t = deg2rad(45.f);
				}
				else {
					pos_t = VECTOR_ref::vget(-12.f*Scale_Rate + (float)((index - Chara_num / 2))*2.f*Scale_Rate, 0.f, -12.f*Scale_Rate);
					rad_t = deg2rad(180.f + 45.f);
				}



				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
				if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
				c->ValueSet(deg2rad(0.f), rad_t, pos_t, (PlayerID)index);
				c->SetGunPtr((std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)index)), 0);
				c->GunSetUp();
				if (index < Chara_num / 2) {
					//c->SetUseRealTimePhysics(true);
					c->SetUseRealTimePhysics(false);
					c->SetCharaType(CharaTypeID::Team);
				}
				else {
					c->SetUseRealTimePhysics(false);
					c->SetCharaType(CharaTypeID::Enemy);
				}
			}
			//player
			PlayerMngr->Init(Player_num);
			for (int i = 0; i < Player_num; i++) {
				PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i)));
				//PlayerMngr->GetPlayer(i).SetChara(nullptr);

				this->m_AICtrl[i]->Init(this->m_BackGround, (PlayerID)i);
			}
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			this->m_HPBuf = Chara->GetHP();
			this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();

			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			SE->Get((int)SoundEnum::Trigger).SetVol_Local(48);
			for (int i = 0; i < 4; i++) {
				SE->Get((int)SoundEnum::Cocking1_0 + i).SetVol_Local(128);
			}
			for (int i = 0; i < 2; i++) {
				SE->Get((int)SoundEnum::Cocking2_0 + i).SetVol_Local(255);
			}
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(216);
			SE->Get((int)SoundEnum::Unload2).SetVol_Local(255);
			SE->Get((int)SoundEnum::Load2).SetVol_Local(255);
			for (int i = 0; i < 2; i++) {
				SE->Get((int)SoundEnum::Cocking3_0 + i).SetVol_Local(255);
			}
			SE->Get((int)SoundEnum::Shot3).SetVol_Local(216);
			SE->Get((int)SoundEnum::Unload3).SetVol_Local(255);
			SE->Get((int)SoundEnum::Load3).SetVol_Local(255);

			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);

			//入力
			this->m_FPSActive.Set(true);
			this->m_MouseActive.Set(true);

			this->m_DamageEvents.clear();

			m_NetWorkBrowser.Init();

			m_MiniMapScreen = GraphHandle::Make(y_r(128) * 2, y_r(128) * 2, true);

			Timer = 10.f;
			//SetLightEnable(FALSE);
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			Pad->ChangeGuide(
				[&]() {
				auto* KeyGuide = KeyGuideClass::Instance();
				//KeyGuide->Reset();
				//return;
				if (DXDraw::Instance()->IsPause()) {
					KeyGuide->Reset();
					KeyGuide->AddGuide("ng.png", "決定");
					KeyGuide->AddGuide("ok.png", "戻る");
					KeyGuide->AddGuide("R_stick.png", "上下選択,調整");
				}
				else {
					KeyGuide->Reset();
					KeyGuide->AddGuide("R_stick.png", "機首下上(ピッチ),左右回転(ロール)");
					KeyGuide->AddGuide("L1.png", "");
					KeyGuide->AddGuide("R1.png", "左右傾け(ヨー)");
					KeyGuide->AddGuide("square.png", "");
					KeyGuide->AddGuide("triangle.png", "加減速");
					KeyGuide->AddGuide("R2.png", "射撃");
					KeyGuide->AddGuide("L_stick.png", "押し込みでフリールック");
					KeyGuide->AddGuide("L2.png", "エイム");
					KeyGuide->AddGuide("option.png", "ポーズ");
				}
			},
				[&]() {
				auto* KeyGuide = KeyGuideClass::Instance();
				//KeyGuide->Reset();
				//return;
				if (DXDraw::Instance()->IsPause()) {
					KeyGuide->Reset();
					KeyGuide->AddGuide("none.jpg", "決定");
					KeyGuide->AddGuide("X.jpg", "戻る");
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "上下選択");
					KeyGuide->AddGuide("A.jpg", "");
					KeyGuide->AddGuide("D.jpg", "調整");
				}
				else {
					KeyGuide->Reset();
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "機首下上(ピッチ)");
					KeyGuide->AddGuide("A.jpg", "");
					KeyGuide->AddGuide("D.jpg", "左右回転(ロール)");
					KeyGuide->AddGuide("Q.jpg", "");
					KeyGuide->AddGuide("E.jpg", "左右傾け(ヨー)");
					KeyGuide->AddGuide("R.jpg", "");
					KeyGuide->AddGuide("F.jpg", "加減速");
					KeyGuide->AddGuide("LM.jpg", "射撃");
					KeyGuide->AddGuide("MM.jpg", "フリールック");
					KeyGuide->AddGuide("RM.jpg", "エイム");
					KeyGuide->AddGuide("Tab.jpg", "ポーズ");
				}
			});

			if (DXDraw::Instance()->IsPause()) {
				auto* SE = SoundPool::Instance();
				if (!OptionWindowClass::Instance()->IsActive()) {
					if (Pad->GetUpKey().trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetDownKey().trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					for (int i = 0; i < 3; i++) {
						Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
					}
					if (Pad->GetOKKey().trigger()) {
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						switch (select) {
						case 0:
							DXDraw::Instance()->PauseExit();
							break;
						case 1:
							OptionWindowClass::Instance()->SetActive();
							break;
						case 2:
							DXDraw::Instance()->PauseExit();
							break;
						default:
							DXDraw::Instance()->PauseExit();
							break;
						}
					}
					if (Pad->GetNGKey().trigger()) {
						SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
						DXDraw::Instance()->PauseExit();
					}
				}
				return true;
			}
			else {
				select = 0;
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
				this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
				//auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//Chara->LoadReticle();//プレイヤー変更時注意
				fov_base = DrawParts->GetMainCamera().GetCamFov();
				Timer = 10.f;

				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP);
				SE->Get((int)SoundEnum::Env2).Play(0, DX_PLAYTYPE_LOOP);
			}
			else {
				Timer = std::max(Timer - 1.f / FPS, 0.f);
			}
			{
				/*
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				m_GameEnd |= (!(Chara->GetHP() != 0 && (Timer <= TotalTime)));
				if (m_GameEnd) {
					EndTimer = std::max(EndTimer - 1.f / FPS, 0.f);
				}
				//*/
			}
			//Input,AI
			{
				float cam_per = ((DrawParts->GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
				cam_per *= 0.6f;
				if (this->m_LookMode != 0) {
					cam_per *= 0.2f;
				}
				float pp_x = 0.f, pp_y = 0.f;
				InputControl MyInput;
				//auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//
				this->m_TPS_Xrad += std::clamp(-Pad->GetLS_Y()*0.15f, -1.f, 1.f) / 100.f;
				this->m_TPS_Yrad += std::clamp(Pad->GetLS_X()*0.15f, -1.f, 1.f) / 100.f;
				//
				pp_x = Pad->GetLS_Y() * cam_per;
				pp_y = Pad->GetLS_X() * cam_per;
				if (Pad->GetFreeLook().press()) {
					pp_x /= 2.f;
					pp_y /= 2.f;
				}

				MyInput.SetInput(
					pp_x,
					pp_y,
					Pad->GetUpKey().press(), Pad->GetDownKey().press(), Pad->GetLeftKey().press(), Pad->GetRightKey().press(),
					Pad->GetRunKey().press(),
					Pad->GetQKey().press(), Pad->GetEKey().press(),
					false, false, false, false,
					Pad->GetOKKey().press(),
					Pad->GetAccelKey().press(),
					Pad->GetBrakeKey().press(),
					Pad->GetNGKey().press(),
					(Pad->GetShotKey().press() && !DXDraw::Instance()->IsPause()),
					(Pad->GetLookKey().press() && !DXDraw::Instance()->IsPause())
				);


				this->m_FPSActive.Execute(false);
				this->m_LookMode = 0;
				Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

				this->m_TPS_Xrad += pp_x * 0.f;
				this->m_TPS_Yrad += pp_y * 0.f;
				this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-60), deg2rad(60));
				if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
				if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

				this->m_TPS_Xrad *= this->m_TPS_Per;
				this->m_TPS_Yrad *= this->m_TPS_Per;

				Easing(&this->m_TPS_XradR, m_TPS_Xrad, 0.5f, EasingType::OutExpo);

				this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
				MyInput.SetRadBuf(PlayerMngr->GetPlayer(GetMyPlayerID()).GetRadBuf());
				//ネットワーク
				m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
				//クライアント
				if (m_NetWorkBrowser.GetClient()) {
					for (auto& c : this->character_Pool) {
						if (c->GetMyPlayerID() == GetMyPlayerID()) {
							c->SetUseRealTimePhysics(false);
							//c->SetUseRealTimePhysics(true);
							c->SetCharaType(CharaTypeID::Team);
						}
						else {
							c->SetUseRealTimePhysics(false);
							if (c->GetMyPlayerID() < Chara_num * 1 / 3) {
								c->SetCharaType(CharaTypeID::Team);
							}
							else {
								c->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
				}
				//
				for (int i = 0; i < Chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
					c->aim_cnt = 0;
				}
				bool isready = (Timer == 0.f);
				for (int i = 0; i < Player_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
					if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = this->m_NetWorkBrowser.GetNowServerPlayerData(i, false);
						if (i == GetMyPlayerID()) {
							MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
							c->SetInput(MyInput, isready);
							m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
						}
						else {
							if (!m_NetWorkBrowser.GetClient()) {
								m_AICtrl[i]->AI_move(&tmp.Input);
							}
							c->SetInput(tmp.Input, isready);
							bool override_true = true;
							override_true = (tmp.CalcCheckSum() != 0);
							if (override_true) {
								c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
							}

						}
						//ダメージイベント処理
						if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
							if (tmp.DamageSwitch != c->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(tmp.Damage);
								c->SetDamageSwitchRec(tmp.DamageSwitch);
							}
						}
					}
					else {
						if (i == GetMyPlayerID()) {
							c->SetInput(MyInput, isready);
						}
						else {
							InputControl OtherInput;
							m_AICtrl[i]->AI_move(&OtherInput);//めっちゃ重い
							c->SetInput(OtherInput, isready);
						}
						//ダメージイベント処理
						if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
							if (c->GetDamageSwitch() != c->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(c->GetDamageEvent());
								c->SetDamageSwitchRec(c->GetDamageSwitch());
							}
						}
					}
				}
				m_NetWorkBrowser.LateExecute();
				//ダメージイベント
				for (auto& c : this->character_Pool) {
					for (int j = 0; j < this->m_DamageEvents.size(); j++) {
						if (c->SetDamageEvent(this->m_DamageEvents[j])) {
							std::swap(this->m_DamageEvents.back(), m_DamageEvents[j]);
							this->m_DamageEvents.pop_back();
							j--;
						}
					}
				}
			}
			//Execute
			ObjMngr->ExecuteObject();
			//
			for (int j = 0; j < gun_num; j++) {
				auto& Gun = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, j));
				if (Gun->GetIsShot()) {
					//エフェクト
					auto mat = Gun->GetMuzzleMatrix();
					EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 1.f);
				}
			}
			//いらないオブジェクトの除去
			ObjMngr->DeleteCheck();
			//弾の更新
			{
				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
						if (a->IsActive()) {
							//AmmoClass
							VECTOR_ref pos_tmp = a->GetMove().pos;
							VECTOR_ref norm_tmp;
							auto ColResGround = a->ColCheckGround(&norm_tmp);
							bool is_HitAll = false;
							for (auto& tgt : this->character_Pool) {
								if (tgt->GetHP() == 0) { continue; }
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								auto res = tgt->CheckAmmoHit(a.get(), tgt->GetMove().pos);
								is_HitAll |= res.second;
							}
							{
								VECTOR_ref pos_tmp2 = a->GetMove().pos;
								if (this->m_BackGround->HitLightCheck(a->GetMove().repos, &pos_tmp2)) {
									EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp2, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
								}
							}
							if (ColResGround && !is_HitAll) {
								a->HitGround(pos_tmp);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
							}
						}
					}
					else {
						break;
					}
					loop++;
				}
			}
			//背景更新
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//首の向き
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(WatchSelect).GetChara();//
				MATRIX_ref FreeLook = MATRIX_ref::RotX(std::clamp(m_TPS_XradR, deg2rad(-20), deg2rad(20))) * MATRIX_ref::RotY(m_TPS_YradR);
				Chara->SetCamEyeVec(FreeLook.zvec()*-1.f);
			}
			//視点
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				if (Chara->GetSendCamShake()) {
					this->m_CamShake = 1.f;
				}
				Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
				Easing(&this->m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
				this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

				if (this->m_FPSActive.on()) {
					VECTOR_ref CamPos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), Chara->GetADSPer());
					DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetEyeVecY());
				}
				else {
					MATRIX_ref FreeLook;
					FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), m_TPS_YradR);
					Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

					VECTOR_ref CamVec = Lerp(Chara->GetEyeVector(), MATRIX_ref::Vtrans(Chara->GetEyeVector(), m_FreeLookMat), m_TPS_Per);

					MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
					VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;
					CamPos += Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), m_EyeRunPer);

					DrawParts->SetMainCamera().SetCamPos(
						CamPos + CamVec * Lerp(Lerp(-20.f, -30.f, m_TPS_Per), 2.f, m_EyePosPer_Prone),
						CamPos + CamVec * 100.f,
						Chara->GetEyeVecY() + this->m_CamShake2 * 0.25f);
				}
				Easing(&this->m_EyeRunPer, 0.f, 0.95f, EasingType::OutExpo);
				auto fov_t = DrawParts->GetMainCamera().GetCamFov();
				auto near_t = DrawParts->GetMainCamera().GetCamNear();
				auto far_t = DrawParts->GetMainCamera().GetCamFar();
				if (this->m_FPSActive.on()) {
					if (Chara->GetIsADS()) {
						Easing(&near_t, Scale_Rate * 0.01f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 60.f, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&near_t, Scale_Rate * 0.01f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
					}
				}
				else {
					if (Chara->GetIsADS()) {
						Easing(&near_t, Scale_Rate * 0.1f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&near_t, Scale_Rate * 0.1f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
					}
				}
				//fov
				if (this->m_FPSActive.on()) {
					fov_t = Chara->GetFov();
				}
				else {
					float fov = 0;
					if (Chara->GetIsADS()) {
						fov = deg2rad(50);
					}
					else {
						fov = deg2rad(55);
					}
					Easing(&fov_t, fov, 0.9f, EasingType::OutExpo);
				}
				DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);

				if (Chara->GetIsADS()) {
					PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t - 0.1f, Scale_Rate * 0.3f, far_t);
				}
				else {
					PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t / 2, near_t, far_t);
				}
				m_BackGround->SetCamPos(DrawParts->GetMainCamera());
			}
			{
				SetNearShadow(
					VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f),
					VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f)
				);
			}
			//
			{
				if (CheckHitKeyWithCheck(KEY_INPUT_0) != 0) { WatchSelect = GetMyPlayerID(); }
				if (CheckHitKeyWithCheck(KEY_INPUT_1) != 0) { WatchSelect = 1; }
				if (CheckHitKeyWithCheck(KEY_INPUT_2) != 0) { WatchSelect = 2; }
				if (CheckHitKeyWithCheck(KEY_INPUT_3) != 0) { WatchSelect = 3; }
				if (CheckHitKeyWithCheck(KEY_INPUT_4) != 0) { WatchSelect = 4; }
				if (CheckHitKeyWithCheck(KEY_INPUT_5) != 0) { WatchSelect = 5; }
				if (CheckHitKeyWithCheck(KEY_INPUT_6) != 0) { WatchSelect = 6; }
				if (CheckHitKeyWithCheck(KEY_INPUT_7) != 0) { WatchSelect = 7; }
				if (CheckHitKeyWithCheck(KEY_INPUT_8) != 0) { WatchSelect = 8; }
				if (CheckHitKeyWithCheck(KEY_INPUT_9) != 0) { WatchSelect = 9; }
				//
				if (WatchSelect != GetMyPlayerID()) {
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(WatchSelect).GetChara();//
					if (Chara->GetHP() == 0) {
						for (auto& c : this->character_Pool) {
							if ((c->GetMyPlayerID() != GetMyPlayerID()) && (c->GetHP() != 0)) {
								WatchSelect = c->GetMyPlayerID();
								break;
							}
						}
					}
				}
			}
			this->m_BackGround->Execute();
			{
				SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Norm(), 0);
				SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(1)).Norm(), 1);
				SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(2)).Norm(), 2);
				SetisUpdateFarShadow(true);
			}
			//レーザーサイト
			for (auto& c : this->character_Pool) {
				c->SetLaser(&character_Pool);
				VECTOR_ref CamPos = Lerp(c->GetEyePosition(), c->GetScopePos(), c->GetADSPer());
				VECTOR_ref Laserpos = GetScreenPos(CamPos, CamPos + c->GetEyeVector(), c->GetMatrix().yvec(), c->GetFov(), c->GetLaser());
				if (0.f < Laserpos.z() && Laserpos.z() < 1.f) {
					c->SetLaser2D(Laserpos);
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("---");
#endif // DEBUG
			//UIパラメーター
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//Time,Score
				this->m_UIclass.SetfloatParam(0, Timer);
				this->m_UIclass.SetIntParam(6, PlayerMngr->GetPlayer(0).GetScore());
				//HP
				this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
				this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
				this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
				this->m_HPBuf += (int)(std::clamp((float)(Chara->GetHP() - this->m_HPBuf)*20.f, -15.f, 15.f) / FPS);
				//SPeed,ALT

				this->m_UIclass.SetIntParam(0, (int)(this->m_CamShake2.x()*100.f));
				this->m_UIclass.SetIntParam(1, (int)(this->m_CamShake2.y()*100.f));
				this->m_UIclass.SetIntParam(2, (int)(0.f*100.f));

				Easing(&m_DrawSpeed, Chara->GetSpeed(), 0.9f, EasingType::OutExpo);

				this->m_UIclass.SetfloatParam(3, m_DrawSpeed);
				this->m_UIclass.SetfloatParam(4, Chara->GetMove().pos.y() / Scale_Rate);
				if (SpeedUpdateTime <= 0.f) {
					this->m_UIclass.SetfloatParam(1, m_DrawSpeed);
					this->m_UIclass.SetfloatParam(2, Chara->GetMove().pos.y() / Scale_Rate);
					SpeedUpdateTime = 0.25f;
				}
				else {
					SpeedUpdateTime -= 1.f / FPS;
				}

				this->m_UIclass.SetItemGraph(0, &Gauge_Graph);
				this->m_UIclass.SetItemGraph(1, &Gauge_Graph);
			}
			EffectControl::Execute();
			//オートエイム
			for (auto& Chara : this->character_Pool) {
				break;
				//
				if (Chara->GetMyPlayerID() == GetMyPlayerID()) {
					m_AutoAimPos.clear();
				}
				bool CanAutoAim = false;
				bool IsCutpai = false;
				std::vector<VECTOR_ref> CutPaiVec;
				if (!Chara->GetIsADS() && false) {

					VECTOR_ref CamPos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), Chara->GetADSPer());
					VECTOR_ref AimPos;
					//
					{
						struct Buffer {
							float Len{ 0.f };
							VECTOR_ref Pos;
							std::vector<VECTOR_ref> Vec;
						};

						std::vector<Buffer> PosBuf;
						auto MyPos = Chara->GetFrameWorldMat(CharaFrame::Upper).pos();
						if (PosBuf.size() >= 2) {
							std::sort(PosBuf.begin(), PosBuf.end(), [&](const Buffer& a, const Buffer& b) {return a.Len < b.Len; });
							AimPos = PosBuf.at(1).Pos;
							CanAutoAim = true;
							IsCutpai = true;
							CutPaiVec = PosBuf.at(1).Vec;
						}
						else if (PosBuf.size() >= 1) {
							AimPos = PosBuf.at(0).Pos;
							CanAutoAim = true;
							IsCutpai = true;
							CutPaiVec = PosBuf.at(0).Vec;
						}
					}
					//敵相手のオートエイム再設定
					{
						float lenBuf = 1000.f*Scale_Rate;
						auto MyPos = Chara->GetGunPtrNow()->GetMuzzleMatrix().pos();
						for (const auto& c : this->character_Pool) {
							if (Chara->GetMyPlayerID() == c->GetMyPlayerID()) { continue; }

							if (Chara->GetCharaType() == c->GetCharaType()) { continue; }

							auto TargetPos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
							VECTOR_ref Tgtpos2D = GetScreenPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetMatrix().yvec(), Chara->GetFov(), TargetPos);
							if (0.f < Tgtpos2D.z() && Tgtpos2D.z() < 1.f) {
								Tgtpos2D = Tgtpos2D - Chara->GetAimPoint();
								Tgtpos2D.z(0.f);
								if (Tgtpos2D.Length() <= y_r(256)) {
									if (!this->m_BackGround->CheckLinetoMap(MyPos, &TargetPos, false)) {
										auto vecLen = (TargetPos - MyPos).Length();
										if (vecLen >= 50.f*Scale_Rate) { continue; }
										if (vecLen <= 1.8f*Scale_Rate) { continue; }
										if (Chara->GetMyPlayerID() == GetMyPlayerID()) {
											AimPosControl tmp;
											tmp.pos = TargetPos;
											tmp.pos2D = VECTOR_ref::vget(0.f, 0.f, -1.f);
											tmp.type = 1;
											m_AutoAimPos.emplace_back(tmp);
										}
										if (Tgtpos2D.Length() <= y_r(128) * Chara->GetActiveAutoScale()) {
											if (lenBuf >= vecLen) {
												lenBuf = vecLen;
												AimPos = TargetPos;
												CanAutoAim = true;
												IsCutpai = false;
											}
										}
									}
								}
							}
						}
					}

					//設定
					if (CanAutoAim) {
						Chara->SetAutoAim(&AimPos, IsCutpai, &CutPaiVec);
					}
				}
				if (!CanAutoAim) {
					Chara->SetAutoAim();
				}
			}
			//
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

				bool OLD = m_IsDrawAimPoint;
				m_IsDrawAimPoint = Chara->GetLaserActive();
				if (OLD != m_IsDrawAimPoint) {
					m_AimOn.play(DX_PLAYTYPE_BACK, TRUE);
				}

			}
			//
			for (auto& c : this->character_Pool) {
				VECTOR_ref campos; campos.z(-1.f);
				c->SetCameraPosition(campos);
			}
			//m_AimPoint.z(-1.f);
			//ミニマップ
			m_MiniMapScreen.SetDraw_Screen();
			{
				float size = 0.7f;
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				int xp = y_r(128);
				int yp = y_r(128);

				int xp1 = 0;
				int yp1 = 0;

				xp1 -= (int)(15.f + Chara->GetMove().pos.x());
				yp1 -= -(int)(15.f + Chara->GetMove().pos.z());

				float rad = std::atan2f(
					Chara->GetEyeVector().cross(VECTOR_ref::front()).y(),
					Chara->GetEyeVector().dot(VECTOR_ref::front())
				);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 216);
				this->m_BackGround->GetMapGraph().DrawRotaGraph3(
					xp + (int)(((float)xp1*std::cos(rad) - (float)yp1 * std::sin(rad))*size),
					yp + (int)(((float)yp1*std::cos(rad) + (float)xp1 * std::sin(rad))*size),
					this->m_BackGround->GetMapGraphXSize() / 2,
					this->m_BackGround->GetMapGraphYSize() / 2,
					size, -size,
					rad, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

				for (auto& c : this->character_Pool) {
					int xp2 = xp1 + (int)(15.f + c->GetMove().pos.x());
					int yp2 = yp1 - (int)(15.f + c->GetMove().pos.z());
					DrawCircle(
						xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, GetColor(255, 0, 0), TRUE);
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* OptionParts = OPTION::Instance();

			SE->Get((int)SoundEnum::Env).StopAll(0);
			SE->Get((int)SoundEnum::Env2).StopAll(0);
			SE->SetVol(OptionParts->Get_SE());

			for (auto& c : character_Pool) {
				c.reset();
			}
			character_Pool.clear();

			m_AICtrl.clear();
			m_NetWorkBrowser.Dispose();
			EffectControl::Dispose();
			PlayerMngr->Dispose();
			ObjMngr->DisposeObject();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
		}

		void			MAINLOOP::ShadowDraw_Far_Sub(void) noexcept {
			this->m_BackGround->Shadow_Draw_Far();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::ShadowDraw_NearFar_Sub(void) noexcept {
			this->m_BackGround->Shadow_Draw_NearFar();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			//this->m_BackGround->Shadow_Draw();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::MainDraw_Sub(void) noexcept {

			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);

			ObjMngr->DrawObject();
			this->m_BackGround->Draw();
			//レーザー
			if (Chara->GetLaserActive()) {
				VECTOR_ref Laserpos2D = ConvWorldPosToScreenPos(Chara->GetLaser().get());
				if (0.f < Laserpos2D.z() && Laserpos2D.z() < 1.f) {
					m_Laserpos2D = Laserpos2D;
				}
			}
			for (auto& c : this->character_Pool) {
				if (c->GetLaserActive()) {
					c->DrawLaser();
				}
			}

			//ObjMngr->DrawDepthObject();
			//シェーダー描画用パラメーターセット
			{
				//
				Set_is_Blackout(true);
				Set_Per_Blackout(0.5f + (1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
				//
				Set_is_lens(false);
			}
			for (auto& c : this->character_Pool) {
				if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
				//auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
				auto pos = c->GetMove().pos + c->GetMove().mat.zvec()*-1.f * 5.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					c->SetCameraPosition(campos);
					c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
				}
			}
			if (Chara->GetHP() != 0) {
				auto pos = Chara->GetMatrix().pos() + Chara->GetMatrix().zvec()*-1.f*100.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					//m_AimPoint = campos;
				}
			}
			if (false)
			{
				for (int i = 0; i < Chara_num; i++) {
					m_AICtrl[i]->Draw();
				}
			}
			//
			/*
			if (this->m_FreeLook_Per > 0.1f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_FreeLook_Per), 0, 255));
				SetUseLighting(FALSE);
				int dev = 36;

				int devX = 3;
				for (int Xdeg = -devX; Xdeg <= devX; Xdeg++) {
					float Xrad = deg2rad(Xdeg * 90 / devX);
					for (int deg = 0; deg < dev; deg++) {
						float prev_rad = deg2rad((deg - 1) * 360 / dev);
						float now_rad = deg2rad(deg * 360 / dev);

						VECTOR_ref prev = (MATRIX_ref::RotX(Xrad)*MATRIX_ref::RotY(prev_rad)).zvec();
						VECTOR_ref now = (MATRIX_ref::RotX(Xrad)*MATRIX_ref::RotY(now_rad)).zvec();
						//
						prev = prev * (5.f*Scale_Rate) + GetCameraPosition();
						now = now * (5.f*Scale_Rate) + GetCameraPosition();
						DrawCapsule_3D(prev, now, 0.01f*Scale_Rate, GetColor(0, 255, 0), GetColor(0, 255, 0));
					}
				}
				SetUseLighting(TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//*/
		}
		void			MAINLOOP::MainDrawbyDepth_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawDepthObject();
		}
		//UI表示
		void			MAINLOOP::DrawUI_Base_Sub(void) noexcept {
			//return;

			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			//auto* Fonts = FontPool::Instance();
			auto* DrawParts = DXDraw::Instance();
			//auto Red = GetColor(255, 0, 0);
			//auto Blue = GetColor(50, 50, 255);
			//auto Green = GetColor(43, 163, 91);
			//auto White = GetColor(212, 255, 239);
			//auto Gray = GetColor(64, 64, 64);
			//auto Black = GetColor(0, 0, 0);
			//unsigned int color = Red;
			//
			{
				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
						a->Draw_Hit_UI(hit_Graph);
					}
					else {
						break;
					}
					loop++;
				}
			}
			{
				auto per = std::clamp(std::abs((float)(Chara->GetHP() - this->m_HPBuf))*0.1f, 0.f, 0.5f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				//OIL_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//UI
			this->m_UIclass.Draw();
			//通信設定
			if (DXDraw::Instance()->IsPause()) {
				//m_NetWorkBrowser.Draw();
			}
			//ポーズ
			if (DXDraw::Instance()->IsPause()) {
				{
					auto per = std::clamp(0.7f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				int xp1, yp1;
				//auto* Fonts = FontPool::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Red75 = GetColor(192, 0, 0);
				//auto White = GetColor(255, 255, 255);
				//auto Gray75 = GetColor(128, 128, 128);
				//auto Gray = GetColor(64, 64, 64);

				xp1 = y_r(1920 - 256 - 54 * 2);
				yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);
				DrawFetteString(xp1, yp1, 1.f, (select == 0), "Retire");

				xp1 = y_r(1920 - 256 - 54 * 1);
				yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);
				DrawFetteString(xp1, yp1, 1.f, (select == 1), "Option");

				xp1 = y_r(1920 - 256);
				yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
				DrawFetteString(xp1, yp1, 1.f, (select == 2), "Return Game");
			}
			//
		}
		void			MAINLOOP::DrawUI_In_Sub(void) noexcept {
			//return;

			//auto* ObjMngr = ObjectManager::Instance();
			//auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

			{
				int xp = y_r(960);
				int yp = y_r(840);
				m_MiniMapScreen.DrawRotaGraph(
					xp,
					yp,
					1.f,
					0.f, true);
			}
			if (Chara->GetLaserActive()) {
				if (!Chara->GetIsADS()) {
					if (0.f < m_Laserpos2D.z() && m_Laserpos2D.z() < 1.f) {
						aim_Graph.DrawRotaGraph((int)m_Laserpos2D.x(), (int)m_Laserpos2D.y(), (float)(y_r(100)) / 100.f*Chara->GetActiveAutoScale(), m_AimRot, true);
					}
				}
			}
			m_AimRot += deg2rad(60.f)*60.f / FPS;
		}
	};
};
