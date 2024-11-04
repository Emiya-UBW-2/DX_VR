#pragma warning(disable:4464)
#include "MainScene.hpp"
#include "../MainScene/NetworkBrowser.hpp"

#include "../MainScene/Object/Judge.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();

			auto* BGM = BGMPool::Instance();
			auto* OptionParts = OPTION::Instance();
			BGM->Add(0, "data/Sound/BGM/BattleStart.wav", false);
			BGM->Add(1, "data/Sound/BGM/Result.wav", false);
			BGM->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
			//BG
			BackGround->Load();
			//
			BattleResourceMngr->Load();
			PlayerMngr->Init(NetWork::Player_num);
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				BattleResourceMngr->LoadChara("Chara", (PlayerID)index);
				BattleResourceMngr->LoadWeapon("Bamboo", (PlayerID)index);
			}
			//
			if (!m_isTraining) {
				BattleResourceMngr->LoadJudge("Suit");
				BattleResourceMngr->LoadJudge("Suit");
				BattleResourceMngr->LoadJudge("Suit");
			}
			//UI
			this->m_UIclass.Load();
			HitMark::Instance()->Load();
			//
			m_GameStart.Load("data/UI/GameStart.png");
			m_GameRestart.Load("data/UI/GameReStart.png");
			m_Once.Load("data/UI/Once.png");
			m_GameEnd.Load("data/UI/GameEnd.png");
			m_Result.Load("data/UI/Result.png");
			m_Result.GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 400);
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			//
			BattleResourceMngr->Set();

			SetShadowScale(1.5f);
			//
			m_PauseMenuControl.LoadPause();
			//
			BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(1.f, -0.5f, 0.05f); LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する

			auto& SecondLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec * -1.f);
			SetLightDifColorHandle(SecondLight.get(), GetColorF(0.5f, 0.5f, 0.3f, 0.1f));
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));

			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
				c->SetViewID(0);
				//人の座標設定
				{
					Vector3DX pos_t;
					pos_t = Vector3DX::vget(0.f, 0.f, (-1.5f * Scale3DRate) * static_cast<float>(index * 2 - 1));

					Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f * Scale3DRate;
					if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f * Scale3DRate, &EndPos, true)) {
						pos_t = EndPos;
					}
					c->ValueSet((PlayerID)index, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(180.f * static_cast<float>(index)), pos_t);
				}
				p->GetAI()->Init((PlayerID)index);
			}
			if (!m_isTraining) {
				for (int index = 0; index < 3; ++index) {
					auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&)*ObjMngr->GetObj((int)ObjType::Judge, index);
					//人の座標設定
					c->SetWin(false, false);
					//人の座標設定
					{
						float Rad = deg2rad(-90.f + 120.f * static_cast<float>(index));
						Vector3DX pos_t = Vector3DX::vget(0.f, 0.f, (3.5f * Scale3DRate));
						pos_t = Matrix3x3DX::Vtrans(pos_t, Matrix3x3DX::RotAxis(Vector3DX::up(), Rad));
						c->MovePoint(Rad, pos_t);
					}
				}
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Init();
			m_PauseMenuControl.SetPause();
			FadeControl::SetFadeIn(2.f);
			this->m_IsEnd = false;
			HitMark::Instance()->Set();

			m_GameStartAlpha = 0.f;
			m_GameStartScale = 0.f;
			m_GameStartTimer = 2.f;

			m_WinOnceAlpha = 0.f;
			m_WinOnceScale = 0.f;
			m_WinOnceTimer = -1.f;

			m_GameEndAlpha = 0.f;
			m_GameEndScale = 0.f;
			m_GameEndTimer = -1.f;
			m_IsWinSound = false;

			m_IsGameStart = false;
			m_IsGameEnd = false;
			m_IsTimeUp = true;
			m_IsDrawOneMinute = true;
			m_Timer = 180.f;
			m_IsPlayable = false;

			m_IsResult = false;

			m_pStart = &m_GameStart;
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* BGM = BGMPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
#ifdef DEBUG
			/*
			{
				auto* DrawParts = DXDraw::Instance();
				if (CheckHitKeyWithCheck(KEY_INPUT_1) != 0) {
					m_D1 = std::clamp(m_D1 - 0.1f * DrawParts->GetDeltaTime(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_2) != 0) {
					m_D1 = std::clamp(m_D1 + 0.1f * DrawParts->GetDeltaTime(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_3) != 0) {
					m_D2 = std::clamp(m_D2 - 0.1f * DrawParts->GetDeltaTime(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_4) != 0) {
					m_D2 = std::clamp(m_D2 + 0.1f * DrawParts->GetDeltaTime(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_5) != 0) {
					m_D3 = std::clamp(m_D3 - 0.1f * DrawParts->GetDeltaTime(), 1.f, 10.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_6) != 0) {
					m_D3 = std::clamp(m_D3 + 0.1f * DrawParts->GetDeltaTime(), 1.f, 10.f);
				}
				printfDx("Dif[%5.2f]\n", m_D1*255.f);
				printfDx("Spc[%5.2f]\n", m_D2*255.f);
				printfDx("Amb[%5.2f]\n", m_D3);
				printfDx("\n");
			}
			PostPassParts->SetLevelFilter(m_D1*255.f, m_D2*255.f, m_D3);
			//*/
			//PostPassParts->SetLevelFilter(38, 154, 1.f);
#else
			//PostPassParts->SetLevelFilter(38, 154, 1.f);
#endif
			if (!m_IsResult) {
				m_PauseMenuControl.UpdatePause();
			}
			if (m_PauseMenuControl.IsRetire()) {
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd) {
				if (FadeControl::IsFadeClear()) {
					FadeControl::SetFadeOut(2.f);
				}
				if (FadeControl::IsFadeAll()) {
					return false;
				}
			}
			if (!GetIsFirstLoop()) {
				FadeControl::UpdateFade();
			}

			if (GetIsFirstLoop()) {
				//SE->Get(static_cast<int>(SoundEnum::Environment)).Play(0, DX_PLAYTYPE_LOOP, TRUE);
				if (m_isTraining) {
					m_IsEventSceneFlag = true;
					m_EventSelect = "data/Cut/Cut1.txt";
				}
				else {
					m_IsEventSceneFlag = true;
					m_EventSelect = "data/Cut/Cut3.txt";
					BGM->Get(0)->Play(DX_PLAYTYPE_BACK, TRUE);
				}
			}

			auto& Chara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			//UIパラメーター
			if (GetIsFirstLoop() || FadeControl::IsFadeAll()) {
				this->m_UIclass.InitGaugeParam(0, static_cast<int>(Chara->GetYaTimerMax() * 10.f), static_cast<int>(Chara->GetYaTimerMax() * 10.f));
				this->m_UIclass.InitGaugeParam(1, static_cast<int>(Chara->GetStaminaMax() * 10000.f), static_cast<int>(Chara->GetStaminaMax() * 10000.f));
				this->m_UIclass.InitGaugeParam(2, static_cast<int>(Chara->GetGuardCoolDownTimerMax() * 100.f), static_cast<int>(Chara->GetGuardCoolDownTimerMax() * 100.f));
				this->m_UIclass.SetfloatParam(1, std::max(m_GameStartTimer, 0.f));
			}

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(true);
			Pad->ChangeGuide(
				[this]() {
					auto* KeyGuide = PadControl::Instance();
					if (m_IsEventSceneFlag) {
						return;
					}
					if (DXDraw::Instance()->IsPause()) {
						if (m_IsResult) {
							return;
						}
						KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					}
					else {
						if (m_IsResult) {
							KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9915));
							return;
						}
						if (m_IsEventSceneActive) {
							KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9914));
						}
						else {
							KeyGuide->AddGuide(PADS::MOVE_W, "");
							KeyGuide->AddGuide(PADS::MOVE_S, "");
							KeyGuide->AddGuide(PADS::MOVE_A, "");
							KeyGuide->AddGuide(PADS::MOVE_D, "");
							KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9900));

							KeyGuide->AddGuide(PADS::SHOT, LocalizePool::Instance()->Get(9906));
							KeyGuide->AddGuide(PADS::AIM, LocalizePool::Instance()->Get(9908));
							KeyGuide->AddGuide(PADS::ULT, LocalizePool::Instance()->Get(9907));

							KeyGuide->AddGuide(PADS::WALK, LocalizePool::Instance()->Get(9903));
							KeyGuide->AddGuide(PADS::JUMP, LocalizePool::Instance()->Get(9905));
						}
					}
				});
			if (!m_IsResult) {
				if (m_IsEventSceneFlag) {
					m_IsEventSceneFlag = false;
					if (!m_IsEventSceneActive) {
						DrawParts->SetDistortionPer(120.f * 4);
						m_EventScene.Load(m_EventSelect.c_str());
						m_EventScene.Start();
						m_IsEventSceneActive = true;
						Pad->SetGuideUpdate();
					}
				}

				if (m_IsEventSceneActive) {
					m_EventScene.GetDeltaTime();
					if (Pad->GetKey(PADS::INTERACT).trigger()) {
						SE->StopAll();
						SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);
						m_EventScene.Skip();
					}
					if (m_EventScene.IsEnd()) {
						DrawParts->SetDistortionPer(120.f);
						FadeControl::SetFadeIn(1.f / (m_isTraining ? 0.5f : 2.f));
						m_EventScene.Dispose();
						m_IsEventSceneActive = false;
						Pad->SetGuideUpdate();
						if (!m_isTraining) {
							m_GameStartTimer = 2.f;
							m_IsGameStart = false;
							m_Timer = 180.f;
						}
					}
					m_EventScene.Update();
					return true;
				}
				if (DXDraw::Instance()->IsPause()) {
					Pad->SetMouseMoveEnable(false);
					if (!m_NetWorkController) {
						return true;
					}
				}
			}
			else {
				Pad->SetMouseMoveEnable(false);
				if (DXDraw::Instance()->IsPause()) {
					ButtonParts->ResetSel();
					if (!m_NetWorkController) {
						return true;
					}
				}
				if (FadeControl::IsFadeClear()) {
					ButtonParts->UpdateInput();
					// 選択時の挙動
					if (ButtonParts->GetTriggerButton()) {
						if (ButtonParts->GetSelect() == 0) {
							SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);
							this->m_IsEnd = true;
							FadeControl::SetFadeOut(1.f);
						}
					}
				}
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			//FirstDoingv
			if (!m_IsResult) {
				if (!m_isTraining) {
					{
						if (m_GameStartTimer > 1.f) {
							float Per = std::clamp((1.4f - m_GameStartTimer) / 0.2f, 0.f, 1.f);//0to1
							m_GameStartAlpha = Lerp(0.f, 1.f, Per);
							m_GameStartScale = Lerp(0.f, 1.0f, Per);
						}
						else if (m_GameStartTimer > 0.f) {
							float Per = std::clamp((1.f - m_GameStartTimer), 0.f, 1.f);//0to1
							m_GameStartAlpha = Lerp(1.f, 1.f, Per);
							m_GameStartScale = Lerp(1.0f, 1.1f, Per);
						}
						else if (m_GameStartTimer > -0.2f) {
							float Per = std::clamp((-m_GameStartTimer) / 0.2f, 0.f, 1.f);//0to1
							m_GameStartAlpha = Lerp(1.f, 0.f, Per);
							m_GameStartScale = Lerp(1.1f, 5.f, Per);
						}
						else {
							m_GameStartAlpha = 0.f;
							m_GameStartScale = 0.f;
						}
						m_GameStartTimer = std::max(m_GameStartTimer - DrawParts->GetDeltaTime(), -1.f);
					}
					{
						if (m_WinOnceTimer > 1.f) {
							float Per = std::clamp((1.4f - m_WinOnceTimer) / 0.2f, 0.f, 1.f);//0to1
							m_WinOnceAlpha = Lerp(0.f, 1.f, Per);
							m_WinOnceScale = Lerp(0.f, 1.0f, Per);
						}
						else if (m_WinOnceTimer > 0.f) {
							float Per = std::clamp((1.f - m_WinOnceTimer), 0.f, 1.f);//0to1
							m_WinOnceAlpha = Lerp(1.f, 1.f, Per);
							m_WinOnceScale = Lerp(1.0f, 1.1f, Per);
						}
						else if (m_WinOnceTimer > -0.2f) {
							float Per = std::clamp((-m_WinOnceTimer) / 0.2f, 0.f, 1.f);//0to1
							m_WinOnceAlpha = Lerp(1.f, 0.f, Per);
							m_WinOnceScale = Lerp(1.1f, 5.f, Per);
						}
						else {
							m_WinOnceAlpha = 0.f;
							m_WinOnceScale = 0.f;
						}
						m_WinOnceTimer = std::max(m_WinOnceTimer - DrawParts->GetDeltaTime(), -1.f);
					}
					{
						if (m_GameEndTimer > 1.f) {
							float Per = std::clamp((1.4f - m_GameEndTimer) / 0.2f, 0.f, 1.f);//0to1
							m_GameEndAlpha = Lerp(0.f, 1.f, Per);
							m_GameEndScale = Lerp(0.f, 1.0f, Per);
						}
						else if (m_GameEndTimer > 0.f) {
							float Per = std::clamp((1.f - m_GameEndTimer), 0.f, 1.f);//0to1
							m_GameEndAlpha = Lerp(1.f, 1.f, Per);
							m_GameEndScale = Lerp(1.0f, 1.1f, Per);
						}
						else if (m_GameEndTimer > -0.2f) {
							float Per = std::clamp((-m_GameEndTimer) / 0.2f, 0.f, 1.f);//0to1
							m_GameEndAlpha = Lerp(1.f, 0.f, Per);
							m_GameEndScale = Lerp(1.1f, 5.f, Per);
						}
						else {
							m_GameEndAlpha = 0.f;
							m_GameEndScale = 0.f;
						}
						m_GameEndTimer = std::max(m_GameEndTimer - DrawParts->GetDeltaTime(), -1.f);
					}
					if (!m_IsPlayable) {
						if (!m_IsGameStart) {
							if (m_GameStartTimer <= 0.f) {
								m_IsGameStart = true;
								m_IsPlayable = true;
								SE->Get(static_cast<int>(SoundEnum::JudgeVoice_Start)).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
						}
						else if (m_IsGameStart) {
							if (!m_IsGameEnd) {
								if (m_WinOnceTimer <= 0.f || m_DivideTimer < 0.f) {
									if (FadeControl::IsFadeClear()) {
										FadeControl::SetFadeOut(2.f);
									}
									if (FadeControl::IsFadeAll()) {
										FadeControl::SetFadeIn(1.f / 2.f);
										if (!m_IsGameEnd) {
											m_pStart = &m_GameRestart;
											m_GameStartTimer = 2.f;
											m_IsGameStart = false;
											//初期化
											for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
												auto& p = PlayerMngr->GetPlayer(index);
												auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
												{
													Vector3DX pos_t;
													pos_t = Vector3DX::vget(0.f, 0.f, (-1.5f * Scale3DRate) * static_cast<float>(index * 2 - 1));

													Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f * Scale3DRate;
													if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f * Scale3DRate, &EndPos, true)) {
														pos_t = EndPos;
													}
													c->MovePoint(deg2rad(0.f), deg2rad(180.f * static_cast<float>(index)), pos_t);
												}
											}
											//
											for (int index = 0; index < 3; ++index) {
												auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj((int)ObjType::Judge, index);
												//人の座標設定
												c->SetWin(false, false);
												//人の座標設定
												{
													float Rad = deg2rad(-90.f + 120.f * static_cast<float>(index));
													Vector3DX pos_t = Vector3DX::vget(0.f, 0.f, (3.5f * Scale3DRate));
													pos_t = Matrix3x3DX::Vtrans(pos_t, Matrix3x3DX::RotAxis(Vector3DX::up(), Rad));
													c->MovePoint(Rad, pos_t);
												}
											}
										}
									}
								}
							}
							else {
								if (m_WinOnceTimer <= 0.f) {
									if (m_GameEndTimer == -1.f) {
										m_GameEndTimer = 1.5f;
									}
									if (m_GameEndTimer <= 1.5f && !m_IsWinSound) {
										m_IsWinSound = true;

										if (PlayerMngr->GetPlayer(0)->GetScore() != PlayerMngr->GetPlayer(1)->GetScore()) {
											SE->Get(static_cast<int>(SoundEnum::JudgeVoice_Win)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										}
										else {
											SE->Get(static_cast<int>(SoundEnum::JudgeVoice_Draw)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										}
										//
									}
									if (m_GameEndTimer <= 0.f) {
										if (FadeControl::IsFadeClear()) {
											FadeControl::SetFadeOut(1.f);
										}
										if (FadeControl::IsFadeAll()) {
											FadeControl::SetFadeIn(2.f);
											this->m_IsResult = true;
											Pad->SetGuideUpdate();
											ButtonParts->Dispose();
											ButtonParts->ResetSel();
											ButtonParts->AddIconButton(
												"CommonData/UI/Right.png", true,
												(1920 / 2), (1080 - 256), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
											BGM->Get(1)->Play(DX_PLAYTYPE_BACK, TRUE);
										}
									}
								}
							}
						}
					}
					else {
						m_Timer -= DrawParts->GetDeltaTime();
						//どちらかが勝利したので停止
						if (PlayerMngr->PutAddScoreFlag()) {
							m_WinOnceTimer = 1.5f;
							m_IsPlayable = false;
							if (PlayerMngr->GetWinPlayer() == GetMyPlayerID()) {
								m_ScoreUp0 = 1.f;
								for (int index = 0; index < 3; ++index) {
									auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj((int)ObjType::Judge, index);
									//人の座標設定
									c->SetWin(true, false);
								}
							}
							else {
								m_ScoreUp1 = 1.f;
								for (int index = 0; index < 3; ++index) {
									auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj((int)ObjType::Judge, index);
									//人の座標設定
									c->SetWin(false, true);
								}
							}
							//どちらかが2点先に取ったら
							for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
								auto& p = PlayerMngr->GetPlayer(index);
								if (p->GetScore() >= 2.f) {
									m_IsGameEnd = true;
									break;
								}
							}
						}
						else {
							//タイムアップ
							if (m_IsTimeUp && (m_Timer < 0.f)) {
								m_IsTimeUp = false;
								SE->Get(static_cast<int>(SoundEnum::TimeUp)).Play(0, DX_PLAYTYPE_BACK, TRUE, 255);

								SE->Get(static_cast<int>(SoundEnum::JudgeVoice_Stop)).Play(0, DX_PLAYTYPE_BACK, TRUE);
								m_IsPlayable = false;
								m_IsGameEnd = true;
								m_GameEndTimer = 2.5f;
							}
							if (m_TimerSE!= (int)m_Timer) {
								m_TimerSE = (int)m_Timer;
								if (m_Timer <= 60.f) {
									SE->Get(static_cast<int>(SoundEnum::CountDown)).Play(0, DX_PLAYTYPE_BACK, TRUE);
								}
							}
							//エリア外
							if (m_DivideTimer < 0.f) {
								SE->Get(static_cast<int>(SoundEnum::JudgeVoice_Stop)).Play(0, DX_PLAYTYPE_BACK, TRUE);
								m_IsPlayable = false;
							}
							//残り1分
							if (m_IsDrawOneMinute && (m_Timer < 60.f)) {
								m_IsDrawOneMinute = false;
								SideLog::Instance()->Add(10.f, 0.f, Red, "残り1分!");
							}
						}
					}
				}
				else {
					m_IsPlayable = FadeControl::IsFadeClear();
				}
				//Input,AI
				{
					InputControl MyInput;
					if (m_IsPlayable) {
						MyInput.ResetAllInput();
						MyInput.SetInputStart(Pad->GetLS_Y(), Pad->GetLS_X());
						MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
						MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
						MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
						MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());

						MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press());
						MyInput.SetInputPADS(PADS::AIM, Pad->GetKey(PADS::AIM).press());
						MyInput.SetInputPADS(PADS::ULT, Pad->GetKey(PADS::ULT).press());

						MyInput.SetInputPADS(PADS::WALK, Pad->GetKey(PADS::WALK).press());
						MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());
						{
							Vector2DX MSVec = Chara->GetBambooVec();
							MSVec.Set(
								std::clamp(Chara->GetBambooVec().x + MyInput.GetAddxRad() * deg2rad(0.1f) * DrawParts->GetFps() / FrameRate, deg2rad(-10), deg2rad(10)),
								std::clamp(Chara->GetBambooVec().y + MyInput.GetAddyRad() * deg2rad(0.1f) * DrawParts->GetFps() / FrameRate, deg2rad(-30), deg2rad(30))
							);
							MyInput.SetxRad(MSVec.x);
							MyInput.SetyRad(MSVec.y);
						}
					}
					//ネットワーク
					auto* NetBrowser = NetWorkBrowser::Instance();
					if (NetBrowser->IsDataReady() && !m_NetWorkController) {
						m_NetWorkController = std::make_unique<NetWork::NetWorkController>();
						this->m_NetWorkController->Init(NetBrowser->GetClient(), NetBrowser->GetNetSetting().UsePort, NetBrowser->GetNetSetting().IP, NetBrowser->GetServerPlayer());
					}
					if (m_NetWorkController) {
						int32_t FreeData[10]{};
						FreeData[0] = static_cast<int>(Chara->GetCharaAction());
						this->m_NetWorkController->SetLocalData().SetMyPlayer(MyInput, Chara->GetMove(), Chara->GetDamageEvent(), FreeData);
						this->m_NetWorkController->Update();
					}
					//
					if (m_NetWorkController && m_NetWorkController->IsInGame()) {
						bool IsServerNotPlayer = !m_NetWorkController->GetClient() && !m_NetWorkController->GetServerPlayer();
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							auto& p = PlayerMngr->GetPlayer(index);
							auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
							NetWork::PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
							c->SetViewID(GetMyPlayerID());
							if (index == GetMyPlayerID() && !IsServerNotPlayer) {
								c->SetInput(Ret.GetInput(), true);//自身が動かすもの
							}
							else {//サーバーからのデータで動くもの
								//サーバーがCPUを動かす場合
								if (!m_NetWorkController->GetClient()) {
									//cpu
									//p->GetAI()->Execute(&MyInput);
								}
								c->SetInput(Ret.GetInput(), true);
								bool override_true = true;
								//override_true = Ret.GetIsActive();
								if (override_true) {
									c->SetMoveOverRide(Ret.GetMove());
									//アクションが違う場合には上書き
									if (Ret.GetFreeData()[0] != static_cast<int>(c->GetCharaAction())) {
										c->SetActionOverRide(static_cast<EnumArmAnimType>(Ret.GetFreeData()[0]));
									}
								}

							}
							//ダメージイベント処理
							Ret.AddDamageEvent(&this->m_DamageEvents);
						}
					}
					else {//オフライン
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							auto& p = PlayerMngr->GetPlayer(index);
							auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
							c->SetViewID(GetMyPlayerID());
							if (index == GetMyPlayerID()) {
								c->SetInput(MyInput, true);
							}
							else {
								InputControl OtherInput;
								if (m_IsPlayable) {
									p->GetAI()->Execute(&OtherInput, m_isTraining);
									{
										Vector2DX MSVec;
										MSVec.Set(
											std::clamp(c->GetBambooVec().x + OtherInput.GetAddxRad() * deg2rad(0.1f) * DrawParts->GetFps() / FrameRate, deg2rad(-10), deg2rad(10)),
											std::clamp(c->GetBambooVec().y + OtherInput.GetAddyRad() * deg2rad(0.1f) * DrawParts->GetFps() / FrameRate, deg2rad(-30), deg2rad(30))
										);
										OtherInput.SetxRad(MSVec.x);
										OtherInput.SetyRad(MSVec.y);
									}
								}
								c->SetInput(OtherInput, true);
							}
							//ダメージイベント処理
							c->AddDamageEvent(&this->m_DamageEvents);
						}
					}

					if (!m_isTraining) {
						Vector3DX pos_c;
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							auto& p = PlayerMngr->GetPlayer(index);
							pos_c += p->GetChara()->GetMove().GetPos();
						}
						pos_c /= (float)PlayerMngr->GetPlayerNum();

						for (int index = 0; index < 3; ++index) {
							auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj((int)ObjType::Judge, index);
							//人の座標設定
							{
								float Rad = deg2rad(-90.f + 120.f * static_cast<float>(index));
								Vector3DX pos_t = Vector3DX::vget(0.f, 0.f, (3.5f * Scale3DRate));
								pos_t = Matrix3x3DX::Vtrans(pos_t, Matrix3x3DX::RotAxis(Vector3DX::up(), Rad));
								c->SetInput(pos_c + pos_t, pos_c);
							}
						}
					}
					//ダメージイベント
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						for (int j = 0, Num = static_cast<int>(this->m_DamageEvents.size()); j < Num; ++j) {
							if (c->SetDamageEvent(this->m_DamageEvents[static_cast<size_t>(j)])) {
								std::swap(this->m_DamageEvents.back(), m_DamageEvents[static_cast<size_t>(j)]);
								this->m_DamageEvents.pop_back();
								--Num;
								--j;
							}
						}
					}
				}
				//別れタイマー
				{
					bool IsWaitOutSide = false;
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						//場外判定
						{
							Vector3DX Vec = p->GetChara()->GetMove().GetPos() - Vector3DX::zero();
							float Len = 11.f / 2.f * Scale3DRate;
							if ((Vec.x < -Len || Len < Vec.x) ||
								(Vec.z < -Len || Len < Vec.z)) {
								IsWaitOutSide = true;
								break;
							}
						}
					}
					if (IsWaitOutSide) {
						m_DivideTimer -= DrawParts->GetDeltaTime();
					}
					else {
						m_DivideTimer = 5.f;
					}
				}
				//Execute
				ObjMngr->ExecuteObject();
				BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				{
					auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
					auto* OptionParts = OPTION::Instance();
					//カメラ
					Vector3DX CamPos = ViewChara->GetEyePosition();
					Vector3DX CamVec = CamPos + ViewChara->GetEyeMatrix().zvec() * -1.f;
					CamVec += CameraShake::Instance()->GetCamShake();
					CamPos += CameraShake::Instance()->GetCamShake() * 2.f;
#ifdef DEBUG
					if (CheckHitKeyWithCheck(KEY_INPUT_F1) != 0) {
						DBG_CamSel = -1;
					}
					if (CheckHitKeyWithCheck(KEY_INPUT_F2) != 0) {
						DBG_CamSel = 0;
					}
					if (CheckHitKeyWithCheck(KEY_INPUT_F3) != 0) {
						DBG_CamSel = 1;
					}
					if (CheckHitKeyWithCheck(KEY_INPUT_F4) != 0) {
						DBG_CamSel = 2;
					}
					if (CheckHitKeyWithCheck(KEY_INPUT_F5) != 0) {
						DBG_CamSel = 3;
					}
					if (CheckHitKeyWithCheck(KEY_INPUT_F6) != 0) {
						DBG_CamSel = 4;
					}
					if (CheckHitKeyWithCheck(KEY_INPUT_F7) != 0) {
						DBG_CamSel = 5;
					}
					switch (DBG_CamSel) {
					case 0:
					case 3:
						CamVec = CamPos;
						CamPos += ViewChara->GetEyeMatrix().xvec() * (2.f * Scale3DRate);
						break;
					case 1:
					case 4:
						CamVec = CamPos;
						CamPos += ViewChara->GetEyeMatrix().yvec() * (2.f * Scale3DRate) + ViewChara->GetEyeMatrix().zvec() * 0.1f;
						break;
					case 2:
					case 5:
						CamVec = CamPos;
						CamPos += ViewChara->GetEyeMatrix().zvec() * (-2.f * Scale3DRate);
						break;
					default:
						break;
					}
#endif
					DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeMatrix().yvec());
					//info
					DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.3f, Scale3DRate * 20.f);
					//DoF
					PostPassEffect::Instance()->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, Scale3DRate * 20.f);
				}
				//竹刀判定
				{
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						if (!c->IsAttacking()) { continue; }
						if (c->GetGuardOn()) { continue; }
#ifdef DEBUG
						//printfDx("[%f]\n", c->GetWeaponPtr()->GetMoveSpeed());
#endif
						Vector3DX StartPos = c->GetWeaponPtr()->GetMove().GetPos();
						Vector3DX EndPos = c->GetWeaponPtr()->GetFramePosition(WeaponObject::WeaponFrame::End);
						StartPos = StartPos + (EndPos - StartPos) * 0.5f;

						for (int index2 = 0; index2 < PlayerMngr->GetPlayerNum(); ++index2) {
							if (index == index2) { continue; }
							auto& p2 = PlayerMngr->GetPlayer(index2);
							auto& tgt = (std::shared_ptr<CharacterObject::CharacterClass>&)p2->GetChara();
							HitPoint Damage = static_cast<HitPoint>(100.f * c->GetWeaponPtr()->GetMoveSpeed() / 5.f);
							tgt->CheckDamageRay(&Damage, c->GetMyPlayerID(), c->GetYaTimer() / c->GetYaTimerMax(), c->GetWazaType(), StartPos, &EndPos);
						}
					}
				}
				//コンカッション
				{
					if (Chara->PopConcussionSwitch()) {
						m_Concussion = 1.f;
					}
					DrawParts->Set_is_Blackout(m_Concussion > 0.f);
					if (m_Concussion == 1.f) {
						CameraShake::Instance()->SetCamShake(0.5f, 0.01f * Scale3DRate);
					}
					if (m_Concussion > 0.9f) {
						Easing(&m_ConcussionPer, 1.f, 0.1f, EasingType::OutExpo);
					}
					else if (m_Concussion > 0.25f) {
						if (m_ConcussionPer > 0.25f) {
							Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
						}
						else {
							m_ConcussionPer = 0.25f;
						}
					}
					else {
						Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
					}
					DrawParts->Set_Per_Blackout(m_ConcussionPer * 2.f);
					m_Concussion = std::max(m_Concussion - DrawParts->GetDeltaTime(), 0.f);
				}
				BackGround->Execute();
				//UIパラメーター
				{
					//NvsN
					this->m_UIclass.SetIntParam(0, PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore());
					this->m_UIclass.SetIntParam(1, PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetScore());
					this->m_UIclass.SetfloatParam(3, m_ScoreUp0);
					this->m_UIclass.SetfloatParam(4, m_ScoreUp1);
					Easing(&m_ScoreUp0, 0.f, 0.95f, EasingType::OutExpo);
					Easing(&m_ScoreUp1, 0.f, 0.95f, EasingType::OutExpo);
					//timer
					this->m_UIclass.SetfloatParam(0, m_Timer);
					this->m_UIclass.SetfloatParam(1, m_IsGameEnd ? 2.f : std::max(m_GameStartTimer, 0.f));
					//心拍数
					this->m_UIclass.SetIntParam(2, static_cast<int>(Chara->GetHeartRate()));
					this->m_UIclass.SetfloatParam(2, Chara->GetHeartRatePow());
					//ゲージ
					this->m_UIclass.SetGaugeParam(0, static_cast<int>((Chara->GetYaTimerMax() - Chara->GetYaTimer()) * 10.f), static_cast<int>(Chara->GetYaTimerMax() * 10.f), 2);
					this->m_UIclass.SetGaugeParam(1, static_cast<int>(Chara->GetStamina() * 10000.f), static_cast<int>(Chara->GetStaminaMax() * 10000.f), 15);
					this->m_UIclass.SetGaugeParam(2, static_cast<int>((Chara->GetGuardCoolDownTimerMax() - Chara->GetGuardCoolDownTimer()) * 100.f), static_cast<int>(Chara->GetGuardCoolDownTimerMax() * 100.f), 0);
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			m_PauseMenuControl.DisposePause();
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
			if (m_IsEventSceneActive) {
				m_EventScene.Dispose();
			}
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//使い回しオブジェ系
			BackGround->Dispose();
			//
			if (m_NetWorkController) {
				m_NetWorkController->Dispose();
				m_NetWorkController.reset();
			}
			{
				auto* DrawParts = DXDraw::Instance();
				PostPassEffect::Instance()->SetLevelFilter(0, 255, 1.f);
				DrawParts->SetAberrationPower(1.f);
				DrawParts->Set_is_Blackout(false);
				DrawParts->Set_Per_Blackout(0.f);
				DrawParts->Set_is_lens(false);
				DrawParts->Set_zoom_lens(1.f);
			}
			if (this->m_IsEnd) {//タイトルに戻る
				SetNextSelect(0);
			}
			else {//次のシーンへ
				SetNextSelect(1);
			}
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Dispose();
			this->m_UIclass.Dispose();
			PlayerMngr->Dispose();
			ObjectManager::Instance()->DeleteAll();
			HitMark::Instance()->Dispose();
			m_GameStart.Dispose();
			m_GameRestart.Dispose();
			m_Once.Dispose();
			m_GameEnd.Dispose();
			m_Result.Dispose();
			auto* BGM = BGMPool::Instance();
			BGM->Delete(0);
			BGM->Delete(1);
		}

		//
		void			MainGameScene::BG_Draw_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.BGDraw();
			}
			else {
				auto* BackGround = BackGround::BackGroundClass::Instance();
				BackGround->BG_Draw();
			}
		}
		void			MainGameScene::ShadowDraw_Far_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			if (!m_IsEventSceneActive) {
				//auto* BackGround = BackGround::BackGroundClass::Instance();
				//BackGround->Shadow_Draw_Far();
			}
		}
		void			MainGameScene::ShadowDraw_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.ShadowDraw();
			}
			else {
				auto* BackGround = BackGround::BackGroundClass::Instance();
				BackGround->Shadow_Draw();
				ObjectManager::Instance()->Draw_Shadow();
			}
		}
		void			MainGameScene::CubeMap_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.BGDraw();
			}
			else {
				auto* BackGround = BackGround::BackGroundClass::Instance();
				BackGround->Draw();
			}
		}

		void			MainGameScene::SetShadowDraw_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.SetShadowDraw();
			}
			else {
				auto* BackGround = BackGround::BackGroundClass::Instance();
				BackGround->Draw();
				ObjectManager::Instance()->Draw();
			}
		}

		void			MainGameScene::MainDraw_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.MainDraw();
			}
			else {
				auto* BackGround = BackGround::BackGroundClass::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto* DrawParts = DXDraw::Instance();
				SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar() * 2.f);
				BackGround->Draw();
				ObjectManager::Instance()->Draw();
				//ObjectManager::Instance()->Draw_Depth();
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					PlayerMngr->GetPlayer(i)->GetAI()->Draw();
				}
				HitMark::Instance()->Update();
			}
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			if (m_IsResult) {
				auto* DrawParts = DXDraw::Instance();
				auto* ButtonParts = ButtonControl::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Result,
					DrawParts->GetUIY(0), DrawParts->GetUIY(0), DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), false);

				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32),
					FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, DrawParts->GetUIY(160), DrawParts->GetUIY(256), Yellow, Black, "Result");

				bool IsWin = (PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore() > PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetScore());
				bool IsDraw = (PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore() == PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetScore());

				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(48),
					FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, DrawParts->GetUIY(300), DrawParts->GetUIY(384),
					IsDraw ? Gray25 : (IsWin ? Red : White), Black,
					IsDraw ? "引き分け" : (IsWin ? "勝利" : "敗退"));
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(24),
					FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, DrawParts->GetUIY(300), DrawParts->GetUIY(386), White, Black, "%d : %d",
					PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore(), PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetScore());

				ButtonParts->Draw();


				FadeControl::DrawFade();
				return;
			}
			if (m_IsEventSceneActive) {
				FadeControl::DrawFade();
				m_EventScene.UIDraw();
			}
			else {
				HitMark::Instance()->Draw();
				//UI
				if (!DXDraw::Instance()->IsPause()) {
					this->m_UIclass.Draw();
					if (!m_isTraining) {
						auto* DrawParts = DXDraw::Instance();
						int xp1 = DrawParts->GetUIY(1920 / 2);
						int yp1 = DrawParts->GetUIY(240);

						if ((m_GameStartAlpha * 255.f) > 1.f) {
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_GameStartAlpha), 0, 255));
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 0);
							WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
								m_pStart, xp1, yp1, m_GameStartScale,0.f, true);
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}

						if ((m_WinOnceAlpha * 255.f) > 1.f) {
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_WinOnceAlpha), 0, 255));
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
							WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
								&m_Once, xp1, yp1, m_WinOnceScale, 0.f, true);
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}

						if ((m_GameEndAlpha * 255.f) > 1.f) {
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_GameEndAlpha), 0, 255));
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
							WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
								&m_GameEnd, xp1, yp1, m_GameEndScale, 0.f, true);
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
					
						if (0 <= m_DivideTimer && m_DivideTimer < 5.f) {
							if ((int)(m_DivideTimer * 100) % 30 < 15) {
								WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(24),
									FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, DrawParts->GetUIY(32), DrawParts->GetUIY(384),
									Yellow, Black,
									"場外まであと%3.1f秒", m_DivideTimer);
							}
						}
					}
				}
				FadeControl::DrawFade();
			}
		}
		void			MainGameScene::DrawUI_In_Sub(void) const noexcept {
			if (m_IsResult) {
				return;
			}
			//UI
			if (DXDraw::Instance()->IsPause()) {
				m_PauseMenuControl.DrawPause();
			}
			if (!m_IsEventSceneActive) {
				//通信設定
				//auto* NetBrowser = NetWorkBrowser::Instance();
				//NetBrowser->Draw();
				if (m_NetWorkController) {
					auto* DrawParts = DXDraw::Instance();
					if (m_NetWorkController->GetPing() >= 0.f) {
						WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:%3dms", static_cast<int>(m_NetWorkController->GetPing()));
					}
					else {
						if (m_NetWorkController->GetClient()) {
							WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, Red, Black, "Lost Connection");
						}
						else {
							WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:---ms");
						}
					}
				}
			}
		}
	}
}
