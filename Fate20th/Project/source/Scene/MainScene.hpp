#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = Player_num;
			static const int		Vehicle_num = 0;
			static const int		Weapon_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			SoundHandle				m_Env;
			SoundHandle				m_AimOn;
			//人
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//ポインター別持ち
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			int						m_LookMode{ 0 };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			GraphHandle				autoaimpoint_Graph;
			GraphHandle				Enemyaimpoint_Graph;
			GraphHandle				hit_Graph;
			GraphHandle				aim_Graph;
			std::array<GraphHandle,8>stand_Graph;
			int						stand_sel{ 0 };
			float					stand_selAnim{ 0.f };
			float					stand_selAnimR{ 0.f };
			float					stand_AnimTime{ 0.f };
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			//
			MATRIX_ref				m_FreeLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;

			NetWorkBrowser			m_NetWorkBrowser;
			//共通
			double					m_ClientFrame{ 0.0 };

			float					fov_base{ 0.f };

			float					m_AimRot{ 0.f };
			VECTOR_ref				m_Laserpos2D;

			bool					m_IsDrawAimPoint{ false };
			struct AimPosControl {
				VECTOR_ref	pos;
				VECTOR_ref	pos2D;
				int			type{ 0 };
			};
			std::vector<AimPosControl>	m_AutoAimPos;

			std::array<shaders, 1>	m_Shader;
			RealTimeCubeMap			m_RealTimeCubeMap;
			int						cubeTex{ -1 };

			float					Timer{ 5.f };


			int						WatchSelect{ 0 };
			int						WatchPoint{ 0 };

		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept { }
			void			Set_Sub(void) noexcept override {
				WeaponAnimManager::Create();
				auto* AnimMngr = WeaponAnimManager::Instance();
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				SetAmbientShadow(
					VECTOR_ref::vget(Scale_Rate*-100.f, Scale_Rate*-10.f, Scale_Rate*-100.f),
					VECTOR_ref::vget(Scale_Rate*100.f, Scale_Rate*50.f, Scale_Rate*100.f),
					VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_BackGround->Init();
				//
				ObjMngr->Init(this->m_BackGround);
				for (int i = 0; i < Chara_num / 2; i++) {
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/saber/")));
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
				for (int i = Chara_num / 2; i < Chara_num; i++) {
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/saber/")));
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
				m_Shader[0].Init("CubeMapTestShader_VS.vso", "CubeMapTestShader_PS.pso");
				m_RealTimeCubeMap.Init();
				cubeTex = LoadGraph("data/SkyCube.dds");	// キューブマップテクスチャの読み込み
				for (int i = 0; i < Weapon_num; i++) {
					ObjMngr->AddObject(ObjType::Weapon, "data/Weapon/Excalibur/");
					(*ObjMngr->GetObj(ObjType::Weapon, i))->SetUseShader(&m_Shader[0]);
					(*ObjMngr->GetObj(ObjType::Weapon, i))->SetShaderTexHandle(0, m_RealTimeCubeMap.GetCubeMapTex());
					(*ObjMngr->GetObj(ObjType::Weapon, i))->SetShaderTexHandle(1, cubeTex);
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
				stand_Graph[0] = GraphHandle::Load("data/UI/Stand.bmp");
				stand_Graph[1] = GraphHandle::Load("data/UI/Squat.bmp");
				stand_Graph[2] = GraphHandle::Load("data/UI/StandR.bmp");
				stand_Graph[3] = GraphHandle::Load("data/UI/SquatR.bmp");
				stand_Graph[4] = GraphHandle::Load("data/UI/Stand_2.bmp");
				stand_Graph[5] = GraphHandle::Load("data/UI/Squat_2.bmp");
				stand_Graph[6] = GraphHandle::Load("data/UI/StandR_2.bmp");
				stand_Graph[7] = GraphHandle::Load("data/UI/SquatR_2.bmp");
				stand_AnimTime = 5.f;
				//Set
				//人
				for (int loop = 0; loop < (int)EnumWeaponAnim::Max; loop++) {
					std::string Path = "data/CharaAnime/";
					Path += EnumWeaponAnimName[loop];
					Path += ".txt";
					AnimMngr->LoadAction(Path.c_str(), (EnumWeaponAnim)loop);
				}
				for (auto& c : this->character_Pool) {
					size_t index = &c - &this->character_Pool.front();

					VECTOR_ref pos_t;
					float rad_t = 0.f;
					if (index < Chara_num / 2) {
						pos_t = VECTOR_ref::vget(22.f*Scale_Rate - (float)(index)*2.f*Scale_Rate, 0.f, 22.f*Scale_Rate);
						rad_t = deg2rad(45.f);
					}
					else {
						pos_t = VECTOR_ref::vget(-22.f*Scale_Rate + (float)((index - Chara_num / 2))*2.f*Scale_Rate, 0.f, -22.f*Scale_Rate);
						rad_t = deg2rad(180.f+ 45.f);
					}



					auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					c->ValueSet(deg2rad(0.f), rad_t, false, pos_t, (PlayerID)index);
					c->SetWeaponPtr((std::shared_ptr<WeaponClass>&)(*ObjMngr->GetObj(ObjType::Weapon, (int)(index))));
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

					this->m_AICtrl[i]->Init(&this->character_Pool, this->m_BackGround, PlayerMngr->GetPlayer(i).GetChara());
				}
				this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetChara()->GetHP();
				this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();

				//Cam
				SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
				SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				//サウンド
				auto SE = SoundPool::Instance();
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/Weapon/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking1_0 + i, 3, "data/Sound/SE/Weapon/bolt/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Cocking2_0 + i, 3, "data/Sound/SE/Weapon/autoM16/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Cocking3_0 + i, 3, "data/Sound/SE/Weapon/auto1911/" + std::to_string(i) + ".wav");
				}


				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, Chara_num * 2, "data/Sound/SE/move/heart.wav");
				SE->Add((int)SoundEnum::Switch, Chara_num, "data/Sound/SE/move/standup.wav");
				

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
				SE->Get((int)SoundEnum::Switch).SetVol_Local(255);

				//入力
				this->m_FPSActive.Set(false);
				this->m_MouseActive.Set(true);

				this->m_DamageEvents.clear();

				m_NetWorkBrowser.Init();

				Timer = 1.f;
			}
			//
			bool			Update_Sub(void) noexcept override {
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				DebugParts->SetPoint("update start");
#endif // DEBUG
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
#ifdef DEBUG
				//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//FirstDoingv
				if (GetIsFirstLoop()) {
					SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					Chara->LoadReticle();//プレイヤー変更時注意
					fov_base = GetMainCamera().GetCamFov();
					Timer = 1.f;
				}
				else {
					Timer = std::max(Timer - 1.f / FPS, 0.f);
				}
				//Input,AI
				{
					float cam_per = ((GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = 0.f, pp_y = 0.f;
					bool look_key = false;
					bool eyechange_key = false;
					InputControl MyInput;
					{
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

						if (GetJoypadNum() > 0) {
							DINPUT_JOYSTATE input;
							int padID = DX_INPUT_PAD1;
							GetJoypadInputState(padID);
							switch (GetJoypadType(padID)) {
							case DX_PADTYPE_OTHER:
							case DX_PADTYPE_DUAL_SHOCK_4:
							case DX_PADTYPE_DUAL_SENSE:
							case DX_PADTYPE_SWITCH_JOY_CON_L:
							case DX_PADTYPE_SWITCH_JOY_CON_R:
							case DX_PADTYPE_SWITCH_PRO_CTRL:
							case DX_PADTYPE_SWITCH_HORI_PAD:
								GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
								{
									pp_x = std::clamp(-(float)(input.Rz) / 100.f*0.35f, -9.f, 9.f) * cam_per;
									pp_y = std::clamp((float)(input.Z) / 100.f*0.35f, -9.f, 9.f) * cam_per;
									if (input.Buttons[11] != 0)/*R3*/ {
										pp_x *= 2.f;
										pp_y *= 2.f;
									}
									float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
									bool w_key = false;
									bool s_key = false;
									bool a_key = false;
									bool d_key = false;
									if (!(input.X == 0 && input.Y == 0)) {
										w_key = (-50.f <= deg && deg <= 50.f);
										a_key = (-140.f <= deg && deg <= -40.f);
										s_key = (130.f <= deg || deg <= -130.f);
										d_key = (40.f <= deg && deg <= 140.f);
									}
									//視点切り替え
									look_key = (input.Buttons[11] != 0);
									//十字
									deg = (float)(input.POV[0]) / 100.f;
									bool right_key = (40.f <= deg && deg <= 140.f);
									bool left_key = (220.f <= deg && deg <= 320.f);
									bool up_key = (310.f <= deg || deg <= 50.f);
									bool down_key = (130.f <= deg && deg <= 230.f);
									//ボタン
									//(input.Buttons[0] != 0)/*□*/
									//(input.Buttons[1] != 0)/*×*/
									//(input.Buttons[2] != 0)/*〇*/
									//(input.Buttons[3] != 0)/*△*/
									//(input.Buttons[4] != 0)/*L1*/
									//(input.Buttons[5] != 0)/*R1*/
									//(input.Buttons[6] != 0)/*L2*/
									//(input.Buttons[7] != 0)/*R2*/
									//(input.Buttons[8] != 0)/**/
									//(input.Buttons[9] != 0)/**/
									//(input.Buttons[10] != 0)/*L3*/
									//(input.Buttons[11] != 0)/*R3*/
									MyInput.SetInput(
										pp_x,
										pp_y,
										w_key, s_key, a_key, d_key,
										(input.Buttons[1] != 0)/*×*/,
										(input.Buttons[4] != 0)/*L1*/, (input.Buttons[5] != 0)/*R1*/,
										right_key, left_key, up_key, down_key,
										(input.Buttons[11] != 0)/*R3*/,	//AutoAim
										(input.Buttons[2] != 0)/*〇*/,	//リロード
										(input.Buttons[3] != 0)/*□*/,	//武器切替
										(input.Buttons[10] != 0)/*L3*/,	//しゃがみ
										(input.Buttons[7] != 0)/*R2*/,	//Shot
										(input.Buttons[6] != 0)/*L2*/	//ADS
									);
								}
								break;
							case DX_PADTYPE_XBOX_360:
							case DX_PADTYPE_XBOX_ONE:
								break;
							default:
								break;
							}

							this->m_TPS_Xrad += pp_x;
							this->m_TPS_Yrad += pp_y;
						}
						else {//キーボード
							this->m_MouseActive.Execute(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
							int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
							if (this->m_MouseActive.on()) {
								if (this->m_MouseActive.trigger()) {
									SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
								}
								GetMousePoint(&mx, &my);
								SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
								SetMouseDispFlag(FALSE);
							}
							else {
								SetMouseDispFlag(TRUE);
							}
							bool LockMode = false;
							int WKey = CheckHitKeyWithCheck(KEY_INPUT_W) != 0;
							int AKey = CheckHitKeyWithCheck(KEY_INPUT_A) != 0;
							int SKey = CheckHitKeyWithCheck(KEY_INPUT_S) != 0;
							int DKey = CheckHitKeyWithCheck(KEY_INPUT_D) != 0;

							float mx2 = 0.f, my2 = 0.f;
							int AimYDeg = -1;
							if (DKey) {
								AimYDeg = 90;
							}
							if (AKey) {
								AimYDeg = 270;
							}
							if (WKey) {
								AimYDeg = 0;
								if (AKey) {
									AimYDeg += -45;
								}
								if (DKey) {
									AimYDeg += 45;
								}
							}
							if (SKey) {
								AimYDeg = 180;
								if (AKey) {
									AimYDeg += 45;
								}
								if (DKey) {
									AimYDeg += -45;
								}
							}
							if (Chara->GetSlash()) {
								AimYDeg = 0;
							}
							if (AimYDeg != -1) {
								VECTOR_ref Vec1 = GetMainCamera().GetCamVec() - GetMainCamera().GetCamPos(); Vec1.y(0); Vec1 = Vec1.Norm();

								Vec1 = MATRIX_ref::Vtrans(Vec1, MATRIX_ref::RotY(deg2rad(AimYDeg)));
								VECTOR_ref Vec2 = Chara->GetCharaVector(); Vec2.y(0); Vec2 = Vec2.Norm();
								float dot = Vec1.dot(Vec2);
								float cross = Vec1.cross(Vec2).y();
								float Per = Lerp(18.f, 9.f, Chara->GetRunPer());
								if (Chara->GetSlash()) {
									Per = 24.f;
								}
								if (dot > 0.f) {
									if (std::abs(cross) > sin(deg2rad(30))) {
										mx2 -= Per * (std::powf(cross, 2.f) *(cross > 0.f ? 1.f : -1.f));
									}
									else {
										mx2 -= 12.f*cross;
									}
								}
								else {
									mx2 -= Per * (cross > 0.f ? 1.f : -1.f);
								}
							}

							pp_x = std::clamp((-(float)(my2))*1.f, -180.f, 180.f) * cam_per;
							pp_y = std::clamp(((float)(mx2))*1.f, -180.f, 180.f) * cam_per;


							look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();
							eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0 && false;



							MyInput.SetInput(
								pp_x,
								pp_y,
								LockMode ? (WKey) : (AimYDeg != -1), LockMode ? (SKey) : false, LockMode ? (AKey) : false, LockMode ? (DKey) : false,
								(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0),
								(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0), (CheckHitKeyWithCheck(KEY_INPUT_E) != 0),
								(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0), (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0),

								(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0),
								(CheckHitKeyWithCheck(KEY_INPUT_R) != 0) && false,
								(CheckHitKeyWithCheck(KEY_INPUT_X) != 0) && false,
								(CheckHitKeyWithCheck(KEY_INPUT_C) != 0) && false,
								(((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on()),
								(((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on())
							);

							this->m_TPS_Xrad += std::clamp((-(float)(my - DXDraw::Instance()->m_DispYSize / 2))*0.5f, -9.f, 9.f) * cam_per;
							this->m_TPS_Xrad = deg2rad(-60);
							this->m_TPS_Yrad += std::clamp(((float)(mx - DXDraw::Instance()->m_DispXSize / 2))*0.5f, -9.f, 9.f) * cam_per;
						}
					}
					this->m_FPSActive.Execute(eyechange_key);
					this->m_LookMode = 1;
					Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

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
								if (c->GetMyPlayerID() < Chara_num / 2) {
									c->SetCharaType(CharaTypeID::Team);
								}
								else {
									c->SetCharaType(CharaTypeID::Enemy);
								}
							}
						}
					}
					//
					printfDx("[%f]\n", Timer);
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
				for (int j = 0; j < Weapon_num; j++) {
					auto& Weapon = (std::shared_ptr<WeaponClass>&)(*ObjMngr->GetObj(ObjType::Weapon, j));
					if (Weapon->GetIsShot()) {
					}
				}
				//いらないオブジェクトの除去
				ObjMngr->DeleteCheck();
				//弾の更新
				this->m_BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				{
					auto& Chara = PlayerMngr->GetPlayer(WatchSelect).GetChara();//
					if (WatchPoint == 0) {
						if (Chara->GetSendCamShake()) {
							this->m_CamShake = 0.2f;
						}
						Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake))*2.f, 0.8f, EasingType::OutExpo);
						Easing(&this->m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
						this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

						if (this->m_FPSActive.on()) {
							VECTOR_ref CamPos = Chara->GetEyePosition();
							SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetEyeVecY());
						}
						else {
							{
								MATRIX_ref FreeLook;
								FreeLook = MATRIX_ref::RotX(m_TPS_XradR) * MATRIX_ref::RotY(m_TPS_YradR);
								Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

								VECTOR_ref CamVec = MATRIX_ref::Vtrans(MATRIX_ref::RotY(deg2rad(45)).zvec()*-1.f, m_FreeLookMat);

								MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
								VECTOR_ref CamPos = Chara->GetMatrix().pos() + VECTOR_ref::up() * 18.f;
								//CamPos += Lerp((UpperMat.xvec()*-1.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-1.f + UpperMat.yvec()*4.f), m_EyeRunPer);

								SetMainCamera().SetCamPos(
									CamPos + CamVec * -45.f,
									CamPos + CamVec * 30.f + VECTOR_ref::up() * 16.f,
									VECTOR_ref::up() + this->m_CamShake2 * 0.25f);
							}
							{
								MATRIX_ref FreeLook;
								FreeLook = MATRIX_ref::RotX(std::clamp(m_TPS_XradR, deg2rad(-20), deg2rad(20))) * MATRIX_ref::RotY(m_TPS_YradR);
								VECTOR_ref CamVec = MATRIX_ref::Vtrans(MATRIX_ref::RotY(deg2rad(45)).zvec()*-1.f, FreeLook);

								Chara->SetCamEyeVec(CamVec.Norm());
							}
						}



						Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
						auto fov_t = GetMainCamera().GetCamFov();
						auto near_t = GetMainCamera().GetCamNear();
						auto far_t = GetMainCamera().GetCamFar();
						if (this->m_FPSActive.on()) {
							Easing(&near_t, Scale_Rate * 0.5f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);
						}
						else {
							if (Chara->GetIsRun()) {
								Easing(&near_t, 3.f, 0.9f, EasingType::OutExpo);
								Easing(&far_t, Scale_Rate * 15.f, 0.9f, EasingType::OutExpo);
							}
							else {
								Easing(&near_t, 10.f, 0.9f, EasingType::OutExpo);
								Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
							}
						}
						//fov
						if (this->m_FPSActive.on()) {
							fov_t = Chara->GetFov();
						}
						else {
							fov_t = Chara->GetFov();
						}
						SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
					}
					else {
						switch (WatchPoint) {
						case 1:
						{
							{
								VECTOR_ref CamPos = Chara->GetFrameWorldMat(CharaFrame::Head).pos();
								CamPos += MATRIX_ref::Vtrans(VECTOR_ref::vget(-0.15f, 0.1f, -0.1f)*Scale_Rate, Chara->GetFrameWorldMat(CharaFrame::Head).GetRot());
								SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetFrameWorldMat(CharaFrame::Head).yvec());
							}
							auto fov_t = GetMainCamera().GetCamFov();
							auto near_t = GetMainCamera().GetCamNear();
							auto far_t = GetMainCamera().GetCamFar();
							Easing(&near_t, Scale_Rate * 1.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 40.f, 0.9f, EasingType::OutExpo);
							fov_t = deg2rad(95);
							SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
						}
						break;
						case 2:
						{
							{
								VECTOR_ref CamPos = VECTOR_ref::vget(0.0f, 4.f, -0.5f)*Scale_Rate;
								SetMainCamera().SetCamPos(CamPos, Chara->GetFrameWorldMat(CharaFrame::Head).pos(), VECTOR_ref::up());
							}
							auto fov_t = GetMainCamera().GetCamFov();
							auto near_t = GetMainCamera().GetCamNear();
							auto far_t = GetMainCamera().GetCamFar();
							Easing(&near_t, Scale_Rate * 1.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 40.f, 0.9f, EasingType::OutExpo);
							fov_t = deg2rad(45);
							SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
						}
						break;
						default:
							break;
						}
					}
				}
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
				}
				{
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD0) != 0) { WatchPoint = 0; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD1) != 0) { WatchPoint = 1; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD2) != 0) { WatchPoint = 2; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD3) != 0) { WatchPoint = 3; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD4) != 0) { WatchPoint = 4; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD5) != 0) { WatchPoint = 5; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD6) != 0) { WatchPoint = 6; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD7) != 0) { WatchPoint = 7; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD8) != 0) { WatchPoint = 8; }
					if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD9) != 0) { WatchPoint = 9; }
				}
				this->m_BackGround->Execute();
#ifdef DEBUG
				DebugParts->SetPoint("---");
#endif // DEBUG

				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);


					this->m_UIclass.SetIntParam(12, (int)0);//現在選択
					this->m_UIclass.SetIntParam(13, (int)1);//銃の総数
					{
						this->m_UIclass.SetStrParam(1, Chara->GetWeaponPtrNow()->GetName().c_str());
						this->m_UIclass.SetItemGraph(0, &Chara->GetWeaponPtrNow()->GetWeaponPic());
					}
					{
						auto Next = (int)((Chara->GetWeaponPtrNowID() + 1) % (int)Chara->GetWeaponPtrNum());
						this->m_UIclass.SetStrParam(2, Chara->GetWeaponPtr(Next)->GetName().c_str());
						this->m_UIclass.SetItemGraph(1, &Chara->GetWeaponPtr(Next)->GetWeaponPic());
					}
					/*
					{
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(3, Chara->GetWeaponPtrNow()->GetName().c_str());
						this->m_UIclass.SetItemGraph(2, &Chara->GetWeaponPtrNow()->GetWeaponPic());
					}
					//*/
				}
				EffectControl::Execute();
				//
				for (auto& c : this->character_Pool) {
					VECTOR_ref campos; campos.z(-1.f);
					c->SetCameraPosition(campos);
				}
				//
#ifdef DEBUG
				DebugParts->SetPoint("update end");
#endif // DEBUG
				return true;
			}
			void			Dispose_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				m_NetWorkBrowser.Dispose();
				EffectControl::Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
				this->m_BackGround->Dispose();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();

				this->m_BackGround->Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
				//auto* ObjMngr = ObjectManager::Instance();
				//ObjMngr->DrawObject_Shadow();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				//this->m_BackGround->Shadow_Draw();
				ObjMngr->DrawObject_Shadow();
			}
			void			MainDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				SetFogStartEnd(Scale_Rate * 10.f, Scale_Rate * 50.f);
				this->m_BackGround->Draw();
				ObjMngr->DrawObject();
				//ObjMngr->DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					//
					Set_is_Blackout(true);
					Set_Per_Blackout(0.5f + (1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					//
					Set_is_lens(false);
					Reticle_on = false;
				}
				for (auto& c : this->character_Pool) {
					if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}

				}
				for (auto& a : m_AutoAimPos) {
					auto tmp = ConvWorldPosToScreenPos(a.pos.get());
					if (tmp.z >= 0.f && tmp.z <= 1.f) {
						a.pos2D = tmp;
					}
				}

				for (int i = 0; i < Player_num; i++) {
					m_AICtrl[i]->Draw();
				}
			}
			void			MainDrawbyDepth_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				ObjMngr->DrawDepthObject();
			}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override {
				if (WatchPoint != 0) {
					return;
				}
				//auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//auto* Fonts = FontPool::Instance();
				//auto* DrawParts = DXDraw::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				//auto White = GetColor(212, 255, 239);
				//auto Gray = GetColor(64, 64, 64);
				//auto Black = GetColor(0, 0, 0);
				//unsigned int color = Red;
				//UI
				this->m_UIclass.Draw();
				m_AimRot += 60.f / FPS;
				//立ち
				{
					auto OLD = stand_sel;
					stand_sel = (Chara->GetIsSquatGround() ? 1 : 0);
					stand_sel += (Chara->GetWeaponPtrNowID() * 4);
					if (OLD != stand_sel) {
						stand_selAnim = 1.f;
						stand_AnimTime = 5.f;
					}
					Easing(&stand_selAnimR, stand_selAnim, (stand_selAnimR < stand_selAnim) ? 0.5f : 0.8f, EasingType::OutExpo);

					if (stand_AnimTime > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)std::clamp(255.f*(stand_AnimTime / 1.f), 0.f, 255.f));
						stand_Graph[stand_sel].DrawRotaGraph(y_r(24) + y_r(128 / 2), y_r(1080) - y_r(128) - y_r(128 / 2), (float)(y_r(128 / 2)) / 128.f *(1.f + stand_selAnimR * 0.3f), 0.f, true);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
					}
					stand_AnimTime = std::max(stand_AnimTime - 1.f / FPS, 0.f);
					stand_selAnim = std::max(stand_selAnim - 1.f / FPS /0.03f, 0.f);
				}
				//通信設定
				if (!this->m_MouseActive.on()) {
					m_NetWorkBrowser.Draw();
				}
				//
			}
			void			DrawUI_In_Sub(void) noexcept override {
				if (WatchPoint != 0) {
					return;
				}
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//
				if (m_IsDrawAimPoint) {
					for (auto& p : m_AutoAimPos) {
						if (p.pos2D.z() >= 0.f && p.pos2D.z() <= 1.f) {
							float AutoAimScale = 1.f;
							VECTOR_ref Laserpos = p.pos2D;
							Laserpos.Set(Laserpos.x() - (float)DrawParts->m_DispXSize / 2, Laserpos.y() - (float)DrawParts->m_DispYSize / 2, Laserpos.z());
							Laserpos.z(0.f);
							Laserpos /= ((float)DrawParts->m_DispXSize / 2);

							AutoAimScale = 1.f * (1.f - Laserpos.Length()*2.f);
							AutoAimScale *= std::clamp((1.f - (p.pos - Chara->GetEyePosition()).Length() / (100.f*Scale_Rate)), 0.f, 1.f);
							switch (p.type) {
							case 1:
								Enemyaimpoint_Graph.DrawRotaGraph((int)p.pos2D.x(), (int)p.pos2D.y(), (float)(y_r(100)) / 100.f*AutoAimScale, m_AimRot, true);
								break;
							default:
								autoaimpoint_Graph.DrawRotaGraph((int)p.pos2D.x(), (int)p.pos2D.y(), (float)(y_r(100)) / 100.f*AutoAimScale, m_AimRot, true);
								break;
							}
						}
					}
				}
				//レティクル表示
				if (Reticle_on) {
					int x, y;
					Chara->GetWeaponPtrNow()->GetReticlePic().GetSize(&x, &y);
					Chara->GetWeaponPtrNow()->GetReticlePic().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (y / 3.f), Chara->GetReticleRad(), true);
				}
			}
		};
	};
};
