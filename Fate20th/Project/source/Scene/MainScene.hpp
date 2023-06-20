#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = Player_num;
			static const int		Vehicle_num = 0;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			SoundHandle				m_Env;
			SoundHandle				m_AimOn;
			//人
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//ポインター別持ち
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			float					m_EyePosPer_Prone = 0.f;
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

			GraphHandle				m_MiniMapScreen;

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
				GunAnimManager::Create();
				auto* AnimMngr = GunAnimManager::Instance();
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
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/Soldier1/")));
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
				for (int i = Chara_num / 2; i < Chara_num; i++) {
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/Soldier2/")));
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
				m_Shader[0].Init("CubeMapTestShader_VS.vso", "CubeMapTestShader_PS.pso");
				m_RealTimeCubeMap.Init();
				cubeTex = LoadGraph("data/SkyCube.dds");	// キューブマップテクスチャの読み込み
				for (int i = 0; i < gun_num; i++) {
					ObjMngr->AddObject(ObjType::Gun, "data/gun/M16A4/");
					(*ObjMngr->GetObj(ObjType::Gun, i * 2 + 0))->SetUseShader(&m_Shader[0]);
					(*ObjMngr->GetObj(ObjType::Gun, i * 2 + 0))->SetShaderTexHandle(0, m_RealTimeCubeMap.GetCubeMapTex());
					(*ObjMngr->GetObj(ObjType::Gun, i * 2 + 0))->SetShaderTexHandle(1, cubeTex);
					ObjMngr->AddObject(ObjType::Gun, "data/gun/M1911/");
					(*ObjMngr->GetObj(ObjType::Gun, i * 2 + 1))->SetUseShader(&m_Shader[0]);
					(*ObjMngr->GetObj(ObjType::Gun, i * 2 + 1))->SetShaderTexHandle(0, m_RealTimeCubeMap.GetCubeMapTex());
					(*ObjMngr->GetObj(ObjType::Gun, i * 2 + 1))->SetShaderTexHandle(1, cubeTex);
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
					c->SetGunPtr(
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 0))),
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 1)))
					);
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
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking1_0 + i, 3, "data/Sound/SE/gun/bolt/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Cocking2_0 + i, 3, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Cocking3_0 + i, 3, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
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
				this->m_FPSActive.Set(true);
				this->m_MouseActive.Set(true);

				this->m_DamageEvents.clear();

				m_NetWorkBrowser.Init();

				m_MiniMapScreen = GraphHandle::Make(y_r(128) * 2, y_r(128) * 2);

				Timer = 10.f;
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
					Timer = 10.f;
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
								auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
								MyInput.SetInput(
									pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
									pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
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
						pp_x = std::clamp((-(float)(my - DXDraw::Instance()->m_DispYSize / 2))*0.5f, -9.f, 9.f) * cam_per;
						pp_y = std::clamp(((float)(mx - DXDraw::Instance()->m_DispXSize / 2))*0.5f, -9.f, 9.f) * cam_per;
						look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();
						eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						MyInput.SetInput(
							pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
							pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
							(CheckHitKeyWithCheck(KEY_INPUT_W) != 0), (CheckHitKeyWithCheck(KEY_INPUT_S) != 0), (CheckHitKeyWithCheck(KEY_INPUT_A) != 0), (CheckHitKeyWithCheck(KEY_INPUT_D) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0), (CheckHitKeyWithCheck(KEY_INPUT_E) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0), (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0),

							(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_R) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_X) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_C) != 0),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on()),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on())
						);
					}

					this->m_FPSActive.Execute(eyechange_key);
					if (look_key) {
						this->m_LookMode = 1;
					}
					else {
						this->m_LookMode = 0;
					}
					Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					this->m_TPS_Xrad += pp_x;
					this->m_TPS_Yrad += pp_y;
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
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//エフェクト
						auto mat = Gun->GetMuzzleMatrix();
						//EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 1.f);
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
								MV1_COLL_RESULT_POLY ColResGround = a->ColCheckGround();
								VECTOR_ref pos_tmp = a->GetMove().pos;
								VECTOR_ref norm_tmp;
								bool is_HitAll = false;
								auto& c = *ObjMngr->GetObj(ObjType::Human, a->GetShootedID());//(std::shared_ptr<CharacterClass>&)
								for (auto& tgt : this->character_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), c->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								if ((ColResGround.HitFlag == TRUE) && !is_HitAll) {
									a->HitGround(pos_tmp);
									if (ColResGround.HitFlag == TRUE) {
										EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, ColResGround.HitPosition, ColResGround.Normal, a->GetCaliberSize() / 0.1f * Scale_Rate);
									}
								}
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				this->m_BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				{
					auto& Chara = PlayerMngr->GetPlayer(WatchSelect).GetChara();//
					if (WatchPoint == 0) {
						if (Chara->GetSendCamShake()) {
							this->m_CamShake = 1.f;
						}
						Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
						Easing(&this->m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
						this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

						if (this->m_FPSActive.on()) {
							VECTOR_ref CamPos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), Chara->GetADSPer());
							SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetEyeVecY());
						}
						else {
							MATRIX_ref FreeLook;
							FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), m_TPS_YradR);
							Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

							VECTOR_ref CamVec = Lerp(Chara->GetEyeVector(), MATRIX_ref::Vtrans(Chara->GetEyeVector(), m_FreeLookMat), m_TPS_Per);

							MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
							VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;
							CamPos += Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), m_EyeRunPer);

							SetMainCamera().SetCamPos(
								CamPos + CamVec * Lerp(Lerp(-20.f, -30.f, m_TPS_Per), 2.f, m_EyePosPer_Prone),
								CamPos + CamVec * 100.f,
								Chara->GetEyeVecY() + this->m_CamShake2 * 0.25f);
						}
						Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
						auto fov_t = GetMainCamera().GetCamFov();
						auto near_t = GetMainCamera().GetCamNear();
						auto far_t = GetMainCamera().GetCamFar();
						if (this->m_FPSActive.on()) {
							if (Chara->GetIsADS()) {
								Easing(&near_t, Scale_Rate * 1.f, 0.9f, EasingType::OutExpo);
								Easing(&far_t, Scale_Rate * 40.f, 0.9f, EasingType::OutExpo);
							}
							else {
								Easing(&near_t, Scale_Rate * 0.5f, 0.9f, EasingType::OutExpo);
								Easing(&far_t, Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);
							}
						}
						else {
							if (Chara->GetIsADS()) {
								Easing(&near_t, 10.f, 0.9f, EasingType::OutExpo);
								Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
							}
							else if (Chara->GetIsRun()) {
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
							float fov = 0;
							if (Chara->GetIsADS()) {
								fov = deg2rad(50);
							}
							else if (Chara->GetIsRun()) {
								fov = deg2rad(70);
							}
							else {
								fov = deg2rad(55);
							}
							Easing(&fov_t, fov, 0.9f, EasingType::OutExpo);
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
						case 3:
						{
							{
								VECTOR_ref CamPos = Chara->GetGunPtrNow()->GetMuzzleMatrix().pos();
								CamPos += MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.1f, 0.f)*Scale_Rate, Chara->GetGunPtrNow()->GetMuzzleMatrix().GetRot());
								SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetGunPtrNow()->GetMuzzleMatrix().zvec(), Chara->GetGunPtrNow()->GetMuzzleMatrix().yvec());
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
						case 4:
						{
							{
								VECTOR_ref CamPos = Chara->GetFrameWorldMat(CharaFrame::Head).pos();
								CamPos += MATRIX_ref::Vtrans(VECTOR_ref::vget(2.f, 0.f, 0.f)*Scale_Rate, Chara->GetFrameWorldMat(CharaFrame::Head).GetRot());
								CamPos.y(Chara->GetFrameWorldMat(CharaFrame::Head).pos().y());
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
						this->m_UIclass.SetStrParam(1, Chara->GetGunPtrNow()->GetName().c_str());
						this->m_UIclass.SetIntParam(14, (int)(Chara->GetAmmoNum()));
						this->m_UIclass.SetIntParam(15, (int)(Chara->GetAmmoAll()));
						this->m_UIclass.SetItemGraph(0, &Chara->GetGunPtrNow()->GetGunPic());
					}
					{
						auto Next = (int)((Chara->GetGunPtrNowID() + 1) % (int)Chara->GetGunPtrNum());
						this->m_UIclass.SetStrParam(2, Chara->GetGunPtr(Next)->GetName().c_str());
						this->m_UIclass.SetItemGraph(1, &Chara->GetGunPtr(Next)->GetGunPic());
					}
					/*
					{
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(3, Chara->GetGunPtrNow()->GetName().c_str());
						this->m_UIclass.SetItemGraph(2, &Chara->GetGunPtrNow()->GetGunPic());
					}
					//*/
				}
				EffectControl::Execute();
				//オートエイム
				for (auto& Chara : this->character_Pool) {
					if (Chara->GetMyPlayerID() == GetMyPlayerID()) {
						m_AutoAimPos.clear();
					}
					bool CanAutoAim = false;
					bool IsCutpai = false;
					std::vector<VECTOR_ref> CutPaiVec;
					if (!Chara->GetIsADS() && !Chara->GetIsRun()) {

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
							for (const auto& C : this->m_BackGround->GetBuildCol()) {
								for (const auto& L : C.GetLockPointPos()) {
									auto Pos = C.GetLockPoint().frame(L.first);
									VECTOR_ref PosAdd = VECTOR_ref::zero();

									if (L.second.size() > 0) {
										auto TgtVec = (Pos - Chara->GetMatrix().pos()); TgtVec.y(0.f);
										TgtVec = TgtVec.Norm();

										//全てのベクトルの左/右にいるかどうか
										int Lcnt = 0;
										int Rcnt = 0;
										for (auto& V : L.second) {
											auto Vec = (C.GetLockPoint().frame(V) - Pos);
											Vec.y(0.f); Vec = Vec.Norm();
											if (Vec.cross(TgtVec).y() > 0.f) {
												Lcnt++;
											}
											else {
												Rcnt++;
											}
										}
										if (Rcnt != 0 && Lcnt != 0) {
											continue;//ベクトル同士の中間にいるのでターゲットにしない
										}
									}

									for (const auto& V : L.second) {
										auto Vec = (C.GetLockPoint().frame(V) - Pos).Norm();
										PosAdd += Pos + Vec * (0.2f*Scale_Rate);
										//
									}
									if (L.second.size() > 0) {
										PosAdd /= (float)(L.second.size());
										Pos = PosAdd;
									}
									Pos.y(MyPos.y());

									auto TargetPos = Pos;
									VECTOR_ref Tgtpos2D = GetScreenPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetMatrix().yvec(), Chara->GetFov(), TargetPos);
									if (0.f < Tgtpos2D.z() && Tgtpos2D.z() < 1.f) {
										Tgtpos2D = Tgtpos2D - Chara->GetAimPoint();
										Tgtpos2D.z(0.f);
										if (Tgtpos2D.Length() <= y_r(256)) {
											if (!this->m_BackGround->CheckLinetoMap(MyPos, &TargetPos, false)) {
												auto vecLen = (TargetPos - MyPos).Length();
												if (vecLen >= 6.f*Scale_Rate) { continue; }
												if (vecLen <= 1.8f*Scale_Rate) { continue; }
												if (Chara->GetMyPlayerID() == GetMyPlayerID()) {
													AimPosControl tmp;
													tmp.pos = TargetPos;
													tmp.pos2D = VECTOR_ref::vget(0.f, 0.f, -1.f);
													tmp.type = 0;
													m_AutoAimPos.emplace_back(tmp);
												}
												if (Tgtpos2D.Length() <= y_r(128) * Chara->GetActiveAutoScale()) {
													Buffer tmp;
													PosBuf.emplace_back(tmp);
													PosBuf.back().Len = vecLen;
													PosBuf.back().Pos = Pos;
													for (const auto& V : L.second) {
														auto Vec = (C.GetLockPoint().frame(V) - Pos).Norm();
														PosBuf.back().Vec.emplace_back(Vec);
													}
												}
											}
										}
									}
									//
								}
							}

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
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				
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
				//
				//m_RealTimeCubeMap.ReadyDraw(GetMainCamera().GetCamPos(), [&]() { ObjMngr->DrawObject(); });
				//
				{
					auto& Chara = PlayerMngr->GetPlayer(WatchSelect).GetChara();

					m_MiniMapScreen.SetDraw_Screen();
					{
						this->m_BackGround->DrawWallUI(y_r(128), y_r(128), y_r(128),
							1.f + Chara->GetRunPer()*0.25f - Chara->GetADSPer()*0.1f
							//0.1f
							,
							Chara->GetMatrix().pos(), Chara->GetRad().y());
					}
				}
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
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
					Set_is_lens(Chara->GetIsADS() && Chara->GetReticleSize()>1.f);
					if (true) {
						VECTOR_ref LensPos = ConvWorldPosToScreenPos(Chara->GetLensPos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Set_xp_lens(LensPos.x());
							Set_yp_lens(LensPos.y());
							LensPos = ConvWorldPosToScreenPos(Chara->GetLensPosSize().get());
							if (0.f < LensPos.z() && LensPos.z() < 1.f) {
								Set_size_lens(std::hypotf(xp_lens() - LensPos.x(), yp_lens() - LensPos.y()));
							}
						}
						LensPos = ConvWorldPosToScreenPos(Chara->GetReticlePos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Reticle_xpos = LensPos.x();
							Reticle_ypos = LensPos.y();
							Reticle_on = (size_lens() > std::hypotf(xp_lens() - Reticle_xpos, yp_lens() - Reticle_ypos));
						}
					}
					else {
						Reticle_on = false;
					}
					Set_zoom_lens(Chara->GetReticleSize());
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
				auto* ObjMngr = ObjectManager::Instance();
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
				//UI
				this->m_UIclass.Draw();
				if (Chara->GetLaserActive()) {
					if (!Chara->GetIsADS() && !Chara->GetIsRun()) {
						if (0.f < m_Laserpos2D.z() && m_Laserpos2D.z() < 1.f) {
							aim_Graph.DrawRotaGraph((int)m_Laserpos2D.x(), (int)m_Laserpos2D.y(), (float)(y_r(100)) / 100.f*Chara->GetActiveAutoScale(), m_AimRot, true);
						}
					}
				}
				m_AimRot += 60.f / FPS;
				//立ち
				{
					auto OLD = stand_sel;
					stand_sel = (Chara->GetIsSquatGround() ? 1 : 0) + (!Chara->GetIsFastSwitch() ? 2 : 0);
					stand_sel += (Chara->GetGunPtrNowID() * 4);
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
					Chara->GetGunPtrNow()->GetReticlePic().GetSize(&x, &y);
					Chara->GetGunPtrNow()->GetReticlePic().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (y / 3.f), Chara->GetReticleRad(), true);
				}
				//MiniMap
				this->m_MiniMapScreen.DrawRotaGraph(y_r(128 + 24), y_r(384 + 24), 1.f, 0.f, false);
			}
		};
	};
};
