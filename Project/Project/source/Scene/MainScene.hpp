#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			static const int		team_num = 3;
			static const int		enemy_num = 3;
			static const int		item_num = 9;
		private:
			//リソース関連
			ObjectManager			m_Obj;						//モデル
			BackGroundClass			m_BackGround;				//BG
			SoundHandle				m_Env;
			//ルール
			float					m_ReadyTime{ 0.f };
			bool					m_StartSwitch{ false };
			bool					m_RemoveSwitch{ false };
			//操作関連
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			switchs					m_Lockon;
			switchs					m_LookModeChange;
			int						m_LookMode{ 0 };
			float					m_LookReturnTime{ 0.f };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_MPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			float					m_AddItemBuf{ 1.f };
			float					m_SubItemBuf{ 1.f };
			int						m_PrevPointSel{ 0 };
			float					m_LockonBuf{ 0.f };
			float					m_LockoffBuf{ 0.f };
			int						m_LockoffXposBuf{ 0 };
			int						m_LockoffYposBuf{ 0 };
			int						m_LockoffXsizeBuf{ 0 };
			int						m_LockoffYsizeBuf{ 0 };
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//
			MATRIX_ref				m_FreeLookMat;
			//
			float					m_Rader{ 1.f };
			float					m_Rader_r{ 1.f };
			//
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				SoundPool::Create();
				Set_EnvLight(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->m_BackGround.Load();
				//
				this->m_Obj.Init(&this->m_BackGround.GetGroundCol());
				for (int i = 0; i < team_num; i++) {
					this->m_Obj.AddObject(ObjType::Human, "data/Charactor/Marisa/");
					this->m_Obj.AddObject(ObjType::Houki, "data/Houki/Houki01/");
				}
				for (int i = 0; i < enemy_num; i++) {
					this->m_Obj.AddObject(ObjType::Human, "data/Charactor/Marisa/");
					this->m_Obj.AddObject(ObjType::Houki, "data/Houki/Houki01/");
				}
				for (int i = 0; i < item_num / 3; i++) {
					this->m_Obj.AddObject(ObjType::Item, "data/Item/BluePotion/");
					this->m_Obj.AddObject(ObjType::Item, "data/Item/YellowPotion/");
					this->m_Obj.AddObject(ObjType::Item, "data/Item/RedPotion/");
				}
				//ロード
				SetCreate3DSoundFlag(FALSE);
				this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Env.vol(64);
				//UI
				this->m_UIclass.Set();
				//
				TEMPSCENE::Set();
				//Set
				//人
				for (int i = 0; i < team_num + enemy_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					c->SetHoukiPtr((std::shared_ptr<HoukiClass>&)(*this->m_Obj.GetObj(ObjType::Houki, i)));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), VECTOR_ref::vget(0.f, 0.f, -52.5f + (float)(i - 1)*20.f));
					if (i == 0) {
						c->SetUseRealTimePhysics(true);
					}
					else {
						c->SetUseRealTimePhysics(false);
					}
					if (i < team_num) {
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetCharaType(CharaTypeID::Enemy);
					}
				}
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 0));//自分
					this->m_HPBuf = Chara->GetHP();
					this->m_MPBuf = Chara->GetMP();
					this->m_ScoreBuf = Chara->GetScore();
				}
				//アイテム
				{
					for (int i = 0; i < item_num; i++) {
						auto& c = *this->m_Obj.GetObj(ObjType::Item, i);
						c->SetMove(MATRIX_ref::RotY(deg2rad(0)), VECTOR_ref::vget(20.f * (i / (item_num / 3)), 0.f, 20.f + 20.f * (i % (item_num / 3))));
					}
				}
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
				//サウンド
				auto SE = SoundPool::Instance();
				SE->Add((int)SoundEnum::Shot_Gun, 3, "data/Sound/SE/gun/shot.wav");
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking0 + i, 3, "data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, 9, "data/Sound/SE/move/heart.wav");
				SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);
				//
				this->m_ReadyTime = 15.f;
				this->m_StartSwitch = false;
				this->m_RemoveSwitch = false;
				//入力
				this->m_FPSActive.Init(false);
				this->m_MouseActive.Init(true);
				this->m_LookMode = 0;
				this->m_LookReturnTime = 0.f;
			}
			//
			bool Update(void) noexcept override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 0));//自分
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
					this->m_ReadyTime = 1.f;
					this->m_PrevPointSel = Chara->GetItemSel();
				}
				//
				{
					auto prev = this->m_ReadyTime;
					this->m_ReadyTime -= 1.f / FPS;
					auto timep = 0.f;

					timep = 0.f;
					this->m_StartSwitch = (prev >= timep && this->m_ReadyTime < timep);
					timep = -10.f;
					this->m_RemoveSwitch = (prev >= timep && this->m_ReadyTime < timep);
					//printfDx("%f \n", this->m_ReadyTime);
				}
				//Input,AI
				{
					this->m_MouseActive.GetInput(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
					int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
					if (this->m_MouseActive.on()) {
						if (this->m_MouseActive.trigger()) {
							SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
						}
						GetMousePoint(&mx, &my);
						SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
						SetMouseDispFlag(FALSE);
					}
					else {
						SetMouseDispFlag(TRUE);
					}

					float cam_per = ((camera_main.fov / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per;
					float pp_y = std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per;
					bool w_key = (CheckHitKeyWithCheck(KEY_INPUT_W) != 0);
					bool s_key = (CheckHitKeyWithCheck(KEY_INPUT_S) != 0);
					bool a_key = (CheckHitKeyWithCheck(KEY_INPUT_A) != 0);
					bool d_key = (CheckHitKeyWithCheck(KEY_INPUT_D) != 0);
					bool Lshift_key = (CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0);
					bool q_key = (CheckHitKeyWithCheck(KEY_INPUT_Q) != 0);
					bool e_key = (CheckHitKeyWithCheck(KEY_INPUT_E) != 0);
					bool space_key = (CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
					bool r_key = (CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
					bool f_key = (CheckHitKeyWithCheck(KEY_INPUT_F) != 0);
					bool right_key = (CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
					bool left_key = (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
					bool useItem_key = (CheckHitKeyWithCheck(KEY_INPUT_J) != 0);
					bool up_key = (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
					bool down_key = (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);
					auto wheel = GetMouseWheelRotVol();
					up_key |= (wheel > 0);
					down_key |= (wheel < 0);
					bool useMagic_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
					bool look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
					bool eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;

					bool Lockon_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);

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
								pp_x = std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f) * cam_per;
								pp_y = std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f) * cam_per;

								float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
								if (!(input.X == 0 && input.Y == 0)) {
									w_key = (-50.f <= deg && deg <= 50.f);
									a_key = (-140.f <= deg && deg <= -40.f);
									s_key = (130.f <= deg || deg <= -130.f);
									d_key = (40.f <= deg && deg <= 140.f);
								}
								else {
									w_key = false;
									a_key = false;
									s_key = false;
									d_key = false;
								}
								//走り(未定)
								Lshift_key = (input.Buttons[10] != 0);
								space_key = (input.Buttons[10] != 0);
								//視点切り替え
								look_key = (input.Buttons[11] != 0);
								//eyechange_key = (input.Buttons[11]!=0);
								//ヨー
								q_key = (input.Buttons[6] != 0);
								e_key = (input.Buttons[7] != 0);
								//加減速
								f_key = (input.Buttons[4] != 0);
								r_key = (input.Buttons[5] != 0);
								//十字
								deg = (float)(input.POV[0]) / 100.f;
								up_key = (310.f <= deg || deg <= 50.f);
								left_key = (220.f <= deg && deg <= 320.f);
								down_key = (130.f <= deg && deg <= 230.f);
								right_key = (40.f <= deg && deg <= 140.f);
								//ボタン
								useItem_key = (input.Buttons[3] != 0);//□
								Lockon_key = (input.Buttons[0] != 0);//△
								useMagic_key = (input.Buttons[1] != 0);//〇
								//_key = (input.Buttons[2] != 0);//×
							}
							break;
						case DX_PADTYPE_XBOX_360:
						case DX_PADTYPE_XBOX_ONE:
							break;
						default:
							break;
						}
					}

					this->m_FPSActive.GetInput(eyechange_key);
					this->m_LookModeChange.GetInput(look_key);
					this->m_Lockon.GetInput(Lockon_key);

					if (this->m_LookModeChange.trigger()) {
						++this->m_LookMode %= 2;
					}
					if (this->m_LookMode == 1) {
						if (this->m_LookModeChange.press()) {
							this->m_LookReturnTime = std::min(this->m_LookReturnTime + 1.f / FPS, 1.f);
						}
						else {
							if (this->m_LookReturnTime >= 1.f) {
								this->m_LookMode = 0;
							}
							this->m_LookReturnTime = 0.f;
						}
					}
					if (this->m_Lockon.trigger()) {
						if (Chara->GetLockOn() == nullptr) {
							for (int i = 1; i < team_num + enemy_num; i++) {
								auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));

								if (Chara->GetCharaType() == c->GetCharaType()) { continue; }
								auto cPos = c->GetFrameWorldMat(CharaFrame::Upper).pos();

								auto minLength = GetMinLenSegmentToPoint(
									Chara->GetEyePosition(),
									Chara->GetEyePosition() + Lerp(Chara->GetEyeVecMat().zvec() * -1.f, MATRIX_ref::Vtrans(Chara->GetEyeVecMat().zvec() * -1.f, m_FreeLookMat), this->m_TPS_Per) * Scale_Rate * 3000.f,
									cPos);
								if (minLength <= (100.f / (50 * Scale_Rate)*(cPos - Chara->GetEyePosition()).size())) {
									Chara->SetLockOn(c);
									this->m_LockonBuf = 1.f;
									break;
								}
							}
						}
						else {
							Chara->SetLockOn(nullptr);
							this->m_LockoffBuf = 1.f;
						}
					}

					Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					this->m_TPS_Xrad += pp_x;
					this->m_TPS_Yrad += pp_y;
					this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-80), deg2rad(80));
					if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
					if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

					this->m_TPS_Xrad *= this->m_TPS_Per;
					this->m_TPS_Yrad *= this->m_TPS_Per;

					Easing(&this->m_TPS_XradR, this->m_TPS_Xrad, 0.5f, EasingType::OutExpo);

					this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;

					Chara->SetEyeVec((camera_main.camvec - camera_main.campos).Norm());

					InputControl Input;
					bool isready = this->m_ReadyTime < 0.f;


					for (int i = 0; i < team_num + enemy_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
						//拾い判定
						for (int j = 0; j < item_num; j++) {
							if (this->m_Obj.GetObj(ObjType::Item, j) == nullptr) { break; }
							auto& item = (std::shared_ptr<ItemClass>&)(*this->m_Obj.GetObj(ObjType::Item, j));
							if (!item->GetHaveChara() && !item->GetUsed()) {
								if ((item->GetMatrix().pos() - c->GetMatrix().pos()).size() < Scale_Rate*1.f) {
									c->AddItem(item);
								}
							}
						}

						//操作
						if (i == 0
							//&& false
							) {
							Input.SetInput(
								pp_x*(1.f - this->m_TPS_Per),
								pp_y*(1.f - this->m_TPS_Per),
								w_key && isready,
								s_key && isready,
								a_key && isready,
								d_key && isready,
								Lshift_key && isready,
								q_key && isready,
								e_key && isready,
								space_key && isready,
								r_key && isready,
								f_key && isready,
								right_key && isready,
								left_key && isready,
								useItem_key && isready,
								up_key && isready,
								down_key && isready,
								useMagic_key && isready
							);
							Chara->SetInput(Input, isready);
							continue;
						}
						Input.SetInput(
							0.f,
							0.f,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(GetRand(2) == 0) && isready
						);
						c->SetInput(Input, isready);
						c->SetLockOn((std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 0)));
					}
				}
				//Execute
				this->m_Obj.ExecuteObject();
				//アイテム削除
				for (int i = 0; i < item_num; i++) {
					if (this->m_Obj.GetObj(ObjType::Item, i) == nullptr) { break; }
					auto& item = (std::shared_ptr<ItemClass>&)(*this->m_Obj.GetObj(ObjType::Item, i));
					if (item->GetUsed()) {
						this->m_Obj.DelObj(ObjType::Item, i);
						i = 0;
					}
				}
				//col

				//視点
				{
					if (Chara->GetSendCamShake()) {
						this->m_CamShake = 1.f;
					}
					Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
					Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
					this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

					if (this->m_FPSActive.on()) {
						camera_main.campos = Chara->GetEyePosition();
						camera_main.camvec = camera_main.campos + Chara->GetEyeVecMat().zvec() * -1.f;
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;

						MATRIX_ref FreeLook;
						auto charalock = Chara->GetLockOn();
						if (charalock != nullptr && (this->m_LookMode == 1)) {
							FreeLook = MATRIX_ref::RotVec2(Chara->GetMatrix().zvec()*-1.f, charalock->GetFrameWorldMat(CharaFrame::Upper).pos() - Chara->GetEyePosition());
						}
						else {
							FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), this->m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), this->m_TPS_YradR);
						}
						Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = Lerp(Chara->GetEyeVecMat().zvec() * -1.f, MATRIX_ref::Vtrans(Chara->GetEyeVecMat().zvec() * -1.f, m_FreeLookMat), this->m_TPS_Per);

						CamPos +=
							Lerp(
								Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), this->m_EyeRunPer),
								Lerp(UpperMat.yvec()*8.f, UpperMat.yvec()*14.f, this->m_TPS_Per),
								Chara->GetFlightPer()
							);

						camera_main.campos = (CamPos + CamVec * Lerp(Lerp(-20.f, -30.f - (Chara->GetFlightSpeed() / 25.f), Chara->GetFlightPer()), -50.f, this->m_TPS_Per)) + this->m_CamShake2 * 5.f;
						camera_main.camvec = CamPos + CamVec * 100.f;

						camera_main.camup = Chara->GetEyeVecMat().yvec() + this->m_CamShake2 * 0.25f;
					}
					Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);

					if (Chara->GetIsRun()) {
						Easing(&camera_main.fov, deg2rad(90), 0.9f, EasingType::OutExpo);
						Easing(&camera_main.near_, 3.f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, Scale_Rate * 150.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&camera_main.fov, deg2rad(75), 0.9f, EasingType::OutExpo);
						Easing(&camera_main.near_, 10.f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, Scale_Rate * 300.f, 0.9f, EasingType::OutExpo);
					}
				}

				this->m_BackGround.Execute();
				//UIパラメーター
				{
					int minute = (int)((-this->m_ReadyTime) / 60.f);
					this->m_UIclass.SetIntParam(0, minute);
					this->m_UIclass.SetfloatParam(0, (-this->m_ReadyTime) - (float)minute*60.f);

					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((Chara->GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, Chara->GetFlightMode() ? 1 : 0);
					this->m_UIclass.SetfloatParam(1, Chara->GetFlightSpeed());
					this->m_UIclass.SetfloatParam(2, Chara->GetMatrix().pos().y() / Scale_Rate);

					this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)Chara->GetMP());
					this->m_UIclass.SetIntParam(7, (int)Chara->GetMPMax());
					this->m_UIclass.SetIntParam(8, (int)(this->m_MPBuf + 0.5f));
					this->m_MPBuf += std::clamp((Chara->GetMP() - this->m_MPBuf)*100.f, -5.f, 5.f) / FPS;
					//アイテム
					{
						int i = 0;
						int ID = -1;
						int ID1 = -1;
						int ID2 = -1;
						//
						i = 1;
						ID = Chara->GetItemSel();
						if (Chara->GetItemNum() <= ID) { ID = -1; }
						ID1 = ID;

						this->m_UIclass.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						this->m_UIclass.SetIntParam(9 + i, (ID != -1) ? (int)(Chara->GetItemCnt(ID)) : 0);
						//
						i = 0;
						ID = (Chara->GetItemSel() - 1);
						if (ID <= -1) { ID = (int)(Chara->GetItemNum()) - 1; }
						ID2 = ID;

						if (Chara->GetItemNum() <= ID) { ID = -1; }
						if (ID1 == ID) { ID = -1; }

						this->m_UIclass.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						this->m_UIclass.SetIntParam(9 + i, (ID != -1) ? (int)(Chara->GetItemCnt(ID)) : 0);
						//
						i = 2;
						ID = (Chara->GetItemSel() + 1);
						if (ID >= Chara->GetItemNum()) { ID = 0; }

						if (Chara->GetItemNum() <= ID) { ID = -1; }
						if (ID1 == ID) { ID = -1; }
						//if (ID2 == ID) { ID = -1; }
						this->m_UIclass.SetItemGraph(i, (ID != -1) ? (&(Chara->GetItem(ID)->GetItemGraph())) : nullptr);
						this->m_UIclass.SetIntParam(9 + i, (ID != -1) ? (int)(Chara->GetItemCnt(ID)) : 0);

						if (this->m_PrevPointSel > Chara->GetItemSel()) {
							if (Chara->GetItemSel() == 0 && this->m_PrevPointSel == Chara->GetItemNum() - 1) {
								this->m_AddItemBuf = 0.f;
							}
							else {
								this->m_SubItemBuf = 0.f;
							}
						}
						else if (this->m_PrevPointSel < Chara->GetItemSel()) {
							if (Chara->GetItemSel() == Chara->GetItemNum() - 1 && this->m_PrevPointSel == 0) {
								this->m_SubItemBuf = 0.f;
							}
							else {
								this->m_AddItemBuf = 0.f;
							}
						}
						this->m_PrevPointSel = Chara->GetItemSel();

						this->m_UIclass.SetfloatParam(3, this->m_AddItemBuf);
						this->m_UIclass.SetfloatParam(4, this->m_SubItemBuf);
						Easing(&this->m_AddItemBuf, 1.f, 0.9f, EasingType::OutExpo);
						Easing(&this->m_SubItemBuf, 1.f, 0.9f, EasingType::OutExpo);
					}
					//
					{
						this->m_UIclass.SetStrParam(0, Chara->GetName());
					}
					//魔法
					{
						this->m_UIclass.SetIntParam(12, Chara->GetMagicSel());
						this->m_UIclass.SetIntParam(13, (int)Chara->GetMagicNum());
						for (int i = 0; i < 4; i++) {
							this->m_UIclass.SetStrParam(1 + i, "");
						}
						for (int i = 0; i < std::min<int>((int)(Chara->GetMagicNum()), 4); i++) {
							this->m_UIclass.SetStrParam(1 + i, Chara->GetMagicName(i));
							this->m_UIclass.SetIntParam(14 + 2 * i, (int)((Chara->GetMagicCoolDown(i) - Chara->GetMagicCoolFrame(i))*100.f));
							this->m_UIclass.SetIntParam(15 + 2 * i, (int)(Chara->GetMagicCoolDown(i)*100.f));
						}
					}
					//メッセージ
					{
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 1));
						this->m_UIclass.SetIntParam(22, (int)c->GetCharaType());
						this->m_UIclass.SetStrParam(5, c->GetName());
						this->m_UIclass.SetStrParam(6, "あいうえおかきくけこさしすせそ");
					}
					//
				}
				TEMPSCENE::Update();
				Effect_UseControl::Update_Effect();
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				this->m_Obj.DisposeObject();
			}
			//
			void Depth_Draw(void) noexcept override {
				this->m_BackGround.Draw();
				//this->m_Obj.DrawDepthObject();
			}
			void BG_Draw(void) noexcept override {
				this->m_BackGround.BG_Draw();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				this->m_BackGround.Shadow_Draw_NearFar();
				//this->m_Obj.DrawObject_Shadow();
			}
			void Shadow_Draw(void) noexcept override {
				this->m_BackGround.Shadow_Draw();
				this->m_Obj.DrawObject_Shadow();
			}

			void Main_Draw(void) noexcept override {
				this->m_BackGround.Draw();
				this->m_Obj.DrawObject();
				//this->m_Obj.DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					Set_is_Blackout(false);
					Set_is_lens(false);
				}
				for (int i = 1; i < team_num + enemy_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(
							std::max(
								20.f / ((pos - GetCameraPosition()).size() / 2.f),
								0.2f
							)
						);
					}
				}
			}
			void Main_Draw2(void) noexcept override {
				this->m_Obj.DrawDepthObject();
			}
			void LAST_Draw(void) noexcept override {
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 0));//自分

				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				auto Blue = GetColor(50, 50, 255);
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(212, 255, 239);
				unsigned int color = Red;
				//キャラ
				for (int i = 1; i < team_num + enemy_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					auto campos = c->GetCameraPosition();
					if (0.f < campos.z() && campos.z() < 1.f) {
						switch (c->GetCharaType()) {
						case CharaTypeID::Team:
							color = Blue;
							break;
						case CharaTypeID::Enemy:
							color = Red;
							break;
						default:
							break;
						}
						int xp, yp, xs, ys;
						xp = (int)(campos.x());
						yp = (int)(campos.y());
						xs = (int)(100.f*c->GetCameraSize());
						ys = (int)(100.f*c->GetCameraSize());
						int siz = y_r(std::max((int)(20.f*c->GetCameraSize()), 10));
						int p = 0;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						p = 1;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, color, FALSE);
						p = 2;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);

						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat_MID(xp, yp - ys - siz, color, White, "%s", c->GetName().c_str());

						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat(xp + xs, yp + ys, color, White, "%dm", (int)((c->GetMatrix().pos() - Chara->GetMatrix().pos()).size() / Scale_Rate));

						if (Chara->GetLockOn() == c) {
							Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat_MID(xp, yp - siz / 2, color, White, "%s", (c->GetCameraSize() > 0.25f) ? "TARGET" : "TGT");

							if (this->m_LockonBuf > 0.f) {
								xs = (int)((100.f + 100.f * this->m_LockonBuf)*c->GetCameraSize());
								ys = (int)((100.f + 100.f * this->m_LockonBuf)*c->GetCameraSize());
								p = 0;
								DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
								p = 1;
								DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, color, FALSE);
								p = 2;
								DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);

								this->m_LockonBuf -= 1.f / FPS / 0.5f;

							}
							this->m_LockoffXposBuf = xp;
							this->m_LockoffYposBuf = yp;
							this->m_LockoffXsizeBuf = xs;
							this->m_LockoffYsizeBuf = ys;
						}
						//リセット
						campos.z(-1.f);
						c->SetCameraPosition(campos);
					}
					if (this->m_LockoffBuf > 0.f) {
						int xp, yp, xs, ys;
						xp = this->m_LockoffXposBuf;
						yp = this->m_LockoffYposBuf;
						xs = this->m_LockoffXsizeBuf;
						ys = this->m_LockoffYsizeBuf;

						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->m_LockoffBuf));
						int p = 0;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						p = 1;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, Red, FALSE);
						p = 2;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

						this->m_LockoffBuf -= 1.f / FPS / 2.f;
					}
				}
				//UI
				this->m_UIclass.Draw();
				//レーダー
				{
					Easing(&this->m_Rader_r, this->m_Rader, 0.8f, EasingType::OutExpo);

					int xp1, yp1;
					int xs1, ys1, xs2, ys2;

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					xp1 = DrawParts->disp_x - y_r(80) - xs1;
					yp1 = DrawParts->disp_y - y_r(300) - ys1;

					Fonts->Get(y_r(20), FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat((int)(xp1 - xs1), (int)(yp1 - ys1) - y_r(20), Green, White, "x%4.2f", this->m_Rader_r);
					DrawLine_2D((int)(xp1 - xs1), (int)(yp1), (int)(xp1 + xs2), (int)(yp1), Green, 3);
					DrawLine_2D((int)(xp1), (int)(yp1 - ys1), (int)(xp1), (int)(yp1 + ys2), Green, 3);
					DrawLine_2D((int)(xp1 - xs1), (int)(yp1), (int)(xp1 + xs2), (int)(yp1), White);
					DrawLine_2D((int)(xp1), (int)(yp1 - ys1), (int)(xp1), (int)(yp1 + ys2), White);

					int p = 0;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);
					p = 1;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, White, FALSE);
					p = 2;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);

					xs1 = y_r((int)(256.f * 0.5f*std::min(1.f, this->m_Rader_r)));
					ys1 = y_r((int)(256.f * 0.8f*std::min(1.f, this->m_Rader_r)));
					xs2 = y_r((int)(256.f * 0.5f*std::min(1.f, this->m_Rader_r)));
					ys2 = y_r((int)(256.f * 0.2f*std::min(1.f, this->m_Rader_r)));
					p = 0;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);
					p = 1;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, White, FALSE);
					p = 2;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);

					auto BaseBos = Chara->GetMatrix().pos();

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					auto base = Chara->GetCharaDir().zvec()*-1.f;
					base.y(0.f);
					base = base.Norm();
					auto vec = VECTOR_ref::front();
					auto rad = std::atan2f(base.cross(vec).y(), base.dot(vec));
					{
						float BaseVPer = 2.f;

						this->m_Rader = BaseVPer;
						bool isout = true;
						auto tmpRader = 1.f;
						int div = 5;
						for (int i = 1; i < team_num + enemy_num; i++) {
							auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
							tmpRader = BaseVPer;
							for (int j = 0; j < div; j++) {
								auto pos = MATRIX_ref::Vtrans(c->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / Scale_Rate) * tmpRader);
								if (((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2))) {
									if (this->m_Rader >= tmpRader) {
										this->m_Rader = tmpRader;
									}
									isout = false;
									break;
								}
								tmpRader -= BaseVPer / div;
							}
						}
						if (isout) {
							this->m_Rader = tmpRader + BaseVPer / div;
						}
					}

					for (int i = 1; i < team_num + enemy_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
						auto pos = MATRIX_ref::Vtrans(c->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / Scale_Rate) * this->m_Rader_r);
						if ((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2)) {
							switch (c->GetCharaType()) {
							case CharaTypeID::Team:
								color = Blue;
								break;
							case CharaTypeID::Enemy:
								color = Red;
								break;
							default:
								break;
							}
							int xp, yp;
							xp = xp1 + (int)(pos.x());
							yp = yp1 + (int)(-pos.z());
							DrawCircle(xp, yp, (int)(5.f * std::min(1.f, this->m_Rader_r)), color, TRUE);
						}
					}
				}
				//
			}
		};
	};
};
