#pragma warning(disable:4464)
#include	"MainScene.hpp"
#include	"../MainScene/NetworkBrowser.hpp"

#include	"../MainScene/Object/Judge.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void MainGameScene::SetupBattleStart(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			FadeControl::SetFadeIn(2.f);
			this->m_IsEnd = false;

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

			PlayerMngr->ResetScore();

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
			if (!m_isTraining) {
				for (int index = 0; index < 3; ++index) {
					auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
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
		void MainGameScene::SetupResult(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good))->StopAll();
			this->m_IsResult = true;
			KeyGuideParts->SetGuideFlip();
			ButtonParts->Dispose();
			ButtonParts->ResetSel();
			ButtonParts->AddIconButton(
				"CommonData/UI/Right.png", true,
				(1920 / 2 + 64), (1080 - 256), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton(
				"data/UI/Reset.png", true,
				(1920 / 2 - 64), (1080 - 256), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
		}
		//
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* HitMarkParts = HitMark::Instance();

			auto* SE = SoundPool::Instance();
			SE->Add(SoundType::BGM, 0, 1, "data/Sound/BGM/BattleStart.wav", false);
			SE->Add(SoundType::BGM, 1, 1, "data/Sound/BGM/Result.wav", false);
			//BG
			BackGround->Load(m_isTraining);
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
			HitMarkParts->Load();
			//
			m_GameStart.Load("data/UI/GameStart.png");
			m_GameRestart.Load("data/UI/GameReStart.png");
			m_Once.Load("data/UI/Once.png");
			m_GameEnd.Load("data/UI/GameEnd.png");
			m_Result.Load("data/UI/Result.png");
			m_Result.GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 400);

			m_Tutorial.Load();
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* HitMarkParts = HitMark::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			//
			BattleResourceMngr->Set();

			SetShadowScale(0.75f);
			//
			m_PauseMenuControl.LoadPause();
			//
			BackGround->Init();
			//
			if (m_isTraining) {
				Vector3DX LightVec = Vector3DX::vget(1.f, -0.5f, 0.05f); LightVec = LightVec.normalized();
				WindowSizeParts->SetAmbientLight(LightVec, GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
				SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する

				auto& SecondLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec * -1.f);
				SetLightDifColorHandle(SecondLight.get(), GetColorF(0.5f, 0.5f, 0.3f, 0.1f));

				PostPassParts->SetGodRayPer(0.5f);
			}
			else {
				Vector3DX LightVec = Vector3DX::vget(0.5f, -0.85f, 0.05f); LightVec = LightVec.normalized();
				WindowSizeParts->SetAmbientLight(LightVec, GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
				SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する

				PostPassParts->SetGodRayPer(0.25f);
			}
			//Cam
			WindowSizeParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			WindowSizeParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));

			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
				c->SetViewID(0);
				p->GetAI()->Init((PlayerID)index);
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Init();

			HitMarkParts->Set();
			if (m_isTraining) {
				m_Tutorial.Set();
			}
			m_PauseMenuControl.SetPause();
			m_Replay.Clear();
			m_GameControlType = GameControlType::InGame;

			SetupBattleStart();
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* SideLogParts = SideLog::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* SceneParts = SceneControl::Instance();
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
			auto* LocalizeParts = LocalizePool::Instance();
			if (m_GameControlType == GameControlType::InGame) {
				if (!m_IsResult) {
					m_PauseMenuControl.UpdatePause();
				}
				if (m_PauseMenuControl.IsRetire()) {
					this->m_IsEnd = true;
				}
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
				//SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Environment))->Play(DX_PLAYTYPE_LOOP, TRUE);
				if (m_isTraining) {
					m_IsEventSceneFlag = true;
					m_EventSelect = "data/Cut/Cut1.txt";
				}
				else {
					m_IsEventSceneFlag = true;
					m_EventSelect = "data/Cut/Cut3.txt";
					SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_BACK, TRUE);
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

			Pad->SetMouseMoveEnable(true);
			KeyGuideParts->ChangeGuide(
				[this]() {
					auto* Pad = PadControl::Instance();
					auto* SceneParts = SceneControl::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					if (m_IsEventSceneFlag) {
						return;
					}
					if (SceneParts->IsPause()) {
						if (m_IsResult) {
							return;
						}
						if (m_GameControlType == GameControlType::Replay) {
							return;
						}
						KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9992));
						KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::RELOAD).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9991));
						KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_W).GetAssign(), Pad->GetControlType()), "");
						KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_S).GetAssign(), Pad->GetControlType()), "");
						KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_STICK).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9993));
					}
					else {
						if (m_IsResult) {
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9915));
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::JUMP).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9917));
							return;
						}
						if (m_GameControlType == GameControlType::Replay) {
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::JUMP).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9918));
							return;
						}
						if (m_IsEventSceneActive) {
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9914));
						}
						else {
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_W).GetAssign(), Pad->GetControlType()), "");
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_S).GetAssign(), Pad->GetControlType()), "");
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_A).GetAssign(), Pad->GetControlType()), "");
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_D).GetAssign(), Pad->GetControlType()), "");
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_STICK).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9900));

							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::SHOT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9906));
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::AIM).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9908));
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::ULT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9907));

							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::WALK).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9903));
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::JUMP).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9905));
							if (m_isTraining) {
								KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()), "");
								KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::RELOAD).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9916));
							}
						}
					}
				});
			if (!m_IsResult) {
				if (m_IsEventSceneFlag) {
					m_IsEventSceneFlag = false;
					if (!m_IsEventSceneActive) {
						PostPassParts->SetDistortionPer(120.f * 4);
						m_EventScene.Load(m_EventSelect.c_str());
						m_EventScene.Start();
						m_IsEventSceneActive = true;
						KeyGuideParts->SetGuideFlip();
					}
				}

				if (m_IsEventSceneActive) {
					m_EventScene.GetDeltaTime();
					if (!SceneParts->IsPause() && Pad->GetPadsInfo(PADS::INTERACT).GetKey().trigger()) {
						SE->StopAll(SoundType::SE);
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
						m_EventScene.Skip();
					}
					if (m_EventScene.IsEnd()) {
						PostPassParts->SetDistortionPer(120.f);
						FadeControl::SetFadeIn(1.f / (m_isTraining ? 0.5f : 2.f));
						m_EventScene.Dispose();
						m_IsEventSceneActive = false;
						KeyGuideParts->SetGuideFlip();
						if (!m_isTraining) {
							m_IsGameStart = false;
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->Play(DX_PLAYTYPE_LOOP, TRUE);
						}
						else {
							if (m_EventSelect == "data/Cut/Cut2.txt") {
								m_IsEnd = true;
							}
						}
					}
					m_EventScene.Update();
					return true;
				}
				if (SceneParts->IsPause()) {
					Pad->SetMouseMoveEnable(false);
					if (!m_NetWorkController) {
						return true;
					}
				}	  
				if (!m_isTraining) {
					if (m_GameControlType == GameControlType::Replay) {
						//リザルトに移動
						if (Pad->GetPadsInfo(PADS::JUMP).GetKey().trigger()) {
							SetupResult();
							//PlayerMngr->ResetScore();
						}
					}
				}
			}
			else {
				Pad->SetMouseMoveEnable(false);
				if (SceneParts->IsPause()) {
					ButtonParts->ResetSel();
					if (!m_NetWorkController) {
						return true;
					}
				}
				if (FadeControl::IsFadeClear()) {
					ButtonParts->UpdateInput();
					// 選択時の挙動
					if (ButtonParts->GetTriggerButton()) {
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
						switch (ButtonParts->GetSelect()) {
						case 0:
						{
							this->m_IsEnd = true;
							FadeControl::SetFadeOut(1.f);
						}
						break;
						case 1:
						{
							//リプレイ
							m_GameControlType = GameControlType::Replay;
							m_Replay.Start();
							KeyGuideParts->SetGuideFlip();
							SetupBattleStart();
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->Play(DX_PLAYTYPE_LOOP, TRUE);
						}
						break;
						default:
							break;
						}
					}
					if (Pad->GetPadsInfo(PADS::JUMP).GetKey().trigger()) {
						//リプレイ
						m_GameControlType = GameControlType::Replay;
						m_Replay.Start();
						KeyGuideParts->SetGuideFlip();
						SetupBattleStart();
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->Play(DX_PLAYTYPE_LOOP, TRUE);
					}
				}
			}
			if (m_isTraining) {
				m_Tutorial.Update();
			}
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
						m_GameStartTimer = std::max(m_GameStartTimer - DXLib_refParts->GetDeltaTime(), -1.f);
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
						m_WinOnceTimer = std::max(m_WinOnceTimer - DXLib_refParts->GetDeltaTime(), -1.f);
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
						m_GameEndTimer = std::max(m_GameEndTimer - DXLib_refParts->GetDeltaTime(), -1.f);
					}
					if (!m_IsPlayable) {
						if (!m_IsGameStart) {
							if (m_GameStartTimer <= 0.f) {
								m_IsGameStart = true;
								m_IsPlayable = true;
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Start))->Play(DX_PLAYTYPE_BACK, TRUE);
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
												auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
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
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Win))->Play(DX_PLAYTYPE_BACK, TRUE);
										}
										else {
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Draw))->Play(DX_PLAYTYPE_BACK, TRUE);
										}
										//
									}
									if (m_GameEndTimer <= 0.f) {
										if (FadeControl::IsFadeClear()) {
											FadeControl::SetFadeOut(1.f);
										}
										if (FadeControl::IsFadeAll()) {
											FadeControl::SetFadeIn(2.f);
											SetupResult();
											if (m_GameControlType == GameControlType::InGame) {
												SE->Get(SoundType::BGM, 1)->Play(DX_PLAYTYPE_BACK, TRUE);
												m_Replay.Save();
											}
										}
									}
								}
							}
						}
					}
					else {
						m_Timer -= DXLib_refParts->GetDeltaTime();
						//どちらかが勝利したので停止
						if (PlayerMngr->PutAddScoreFlag()) {
							m_WinOnceTimer = 1.5f;
							m_IsPlayable = false;

							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good))->Play(DX_PLAYTYPE_BACK, TRUE);

							{
								switch (PlayerMngr->GetWinHitType()) {
								case HitType::Head://面
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Men))->Play(DX_PLAYTYPE_BACK, TRUE);
									break;
								case HitType::Body://胴
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Dou))->Play(DX_PLAYTYPE_BACK, TRUE);
									break;
								case HitType::Arm://小手
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Kote))->Play(DX_PLAYTYPE_BACK, TRUE);
									break;
								case HitType::Leg:
								default:
									break;
								}
							}

							if (PlayerMngr->GetWinPlayer() == GetMyPlayerID()) {
								m_ScoreUp0 = 1.f;
								for (int index = 0; index < 3; ++index) {
									auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
									//人の座標設定
									c->SetWin(true, false);
								}
							}
							else {
								m_ScoreUp1 = 1.f;
								for (int index = 0; index < 3; ++index) {
									auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
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
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::TimeUp))->Play(DX_PLAYTYPE_BACK, TRUE, 255);

								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Stop))->Play(DX_PLAYTYPE_BACK, TRUE);
								m_IsPlayable = false;
								m_IsGameEnd = true;
								m_GameEndTimer = 2.5f;
							}
							if (m_TimerSE!= static_cast<int>(m_Timer)) {
								m_TimerSE = static_cast<int>(m_Timer);
								if (m_Timer <= 60.f) {
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::CountDown))->Play(DX_PLAYTYPE_BACK, TRUE);
								}
							}
							//エリア外
							if (m_DivideTimer < 0.f) {
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Stop))->Play(DX_PLAYTYPE_BACK, TRUE);
								m_IsPlayable = false;
							}
							//残り1分
							if (m_IsDrawOneMinute && (m_Timer < 60.f)) {
								m_IsDrawOneMinute = false;
								SideLogParts->Add(10.f, 0.f, Red, LocalizeParts->Get(200));
							}
						}
					}
				}
				else {
					m_IsPlayable = FadeControl::IsFadeClear();

					if (!m_IsPlayable) {
						if (FadeControl::IsFadeAll()) {
							FadeControl::SetFadeIn(1.f / 2.f);
							m_TutorialResetTimer = 0.f;
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
						}
					}
					else {
						if (PlayerMngr->PutAddScoreFlag()) {
							if (PlayerMngr->GetWinPlayer() == GetMyPlayerID()) {
								if (m_Tutorial.CheckHitOk(PlayerMngr->GetWinHitType())) {
									m_TutorialResetTimer = 2.f;
								}
							}
						}

						if (m_TutorialResetTimer > 0.f) {
							m_TutorialResetTimer -= DXLib_refParts->GetDeltaTime();
							if (m_TutorialResetTimer <= 0.f) {
								if (FadeControl::IsFadeClear()) {
									FadeControl::SetFadeOut(2.f);
								}
							}
						}
						else {
							m_TutorialResetTimer = 0.f;
						}

						if (m_Tutorial.IsEndTutorial()) {
							m_TutorialTimer += DXLib_refParts->GetDeltaTime();

							if (m_TutorialTimer > 10.f) {
								m_TutorialTimer = 0.f;

								m_IsPlayable = false;

								m_IsEventSceneFlag = true;
								m_EventSelect = "data/Cut/Cut2.txt";
							}
						}
						else {
							m_TutorialTimer = 0.f;
						}
					}
				}
				//Input,AI
				{
					InputControl MyInput;
					if (m_IsPlayable) {
						MyInput.ResetAllInput();
						MyInput.SetInputStart(Pad->GetLS_Y(), Pad->GetLS_X());
						MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetPadsInfo(PADS::MOVE_W).GetKey().press());
						MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetPadsInfo(PADS::MOVE_S).GetKey().press());
						MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetPadsInfo(PADS::MOVE_A).GetKey().press());
						MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetPadsInfo(PADS::MOVE_D).GetKey().press());

						MyInput.SetInputPADS(PADS::SHOT, Pad->GetPadsInfo(PADS::SHOT).GetKey().press());
						MyInput.SetInputPADS(PADS::AIM, Pad->GetPadsInfo(PADS::AIM).GetKey().press());
						MyInput.SetInputPADS(PADS::ULT, Pad->GetPadsInfo(PADS::ULT).GetKey().press());

						MyInput.SetInputPADS(PADS::WALK, Pad->GetPadsInfo(PADS::WALK).GetKey().press());
						MyInput.SetInputPADS(PADS::JUMP, Pad->GetPadsInfo(PADS::JUMP).GetKey().press());
						{
							Vector2DX MSVec = Chara->GetBambooVec();
							MSVec.Set(
								std::clamp(Chara->GetBambooVec().x + MyInput.GetAddxRad() * deg2rad(0.1f) * DXLib_refParts->GetFps() / FrameRate, deg2rad(-10), deg2rad(10)),
								std::clamp(Chara->GetBambooVec().y + MyInput.GetAddyRad() * deg2rad(0.1f) * DXLib_refParts->GetFps() / FrameRate, deg2rad(-30), deg2rad(30))
							);
							MyInput.SetxRad(MSVec.x);
							MyInput.SetyRad(MSVec.y);
						}
					}
					//ネットワーク
					{
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
						if (m_NetWorkController && m_NetWorkController->IsInGame()) {
							m_GameControlType = GameControlType::Network;
						}
					}
					switch (m_GameControlType) {
					case GameControlType::InGame:
					{
						if (!m_isTraining) {
							m_Replay.Add();
						}
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							auto& p = PlayerMngr->GetPlayer(index);
							auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
							c->SetViewID(GetMyPlayerID());
							if ((PlayerID)index == GetMyPlayerID()) {
								if (!m_isTraining) {
									int32_t FreeData[10]{};
									FreeData[0] = static_cast<int>(c->GetCharaAction());
									m_Replay.SetBack().SetOnce((PlayerID)index).SetMyPlayer(MyInput, c->GetMove(), c->GetDamageEvent(), FreeData);
								}
								c->SetInput(MyInput, true);
							}
							else {
								InputControl OtherInput;
								if (m_IsPlayable) {
									p->GetAI()->Execute(&OtherInput, m_isTraining);
									{
										Vector2DX MSVec;
										MSVec.Set(
											std::clamp(c->GetBambooVec().x + OtherInput.GetAddxRad() * deg2rad(0.1f) * DXLib_refParts->GetFps() / FrameRate, deg2rad(-10), deg2rad(10)),
											std::clamp(c->GetBambooVec().y + OtherInput.GetAddyRad() * deg2rad(0.1f) * DXLib_refParts->GetFps() / FrameRate, deg2rad(-30), deg2rad(30))
										);
										OtherInput.SetxRad(MSVec.x);
										OtherInput.SetyRad(MSVec.y);
									}
								}

								if (!m_isTraining) {
									int32_t FreeData[10]{};
									FreeData[0] = static_cast<int>(c->GetCharaAction());
									m_Replay.SetBack().SetOnce((PlayerID)index).SetMyPlayer(OtherInput, c->GetMove(), c->GetDamageEvent(), FreeData);
								}
								c->SetInput(OtherInput, true);
							}
							//ダメージイベント処理
							c->AddDamageEvent(&this->m_DamageEvents);
						}
					}
						break;
					case GameControlType::Network:
					{
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
					break;
					case GameControlType::Replay:
					{
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							auto& p = PlayerMngr->GetPlayer(index);
							auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
							c->SetViewID(GetMyPlayerID());
							NetWork::PlayerSendData Ret = m_Replay.GetNow().GetOnce((PlayerID)index);
							//サーバーがCPUを動かす場合
							c->SetInput(Ret.GetInput(), true);
							c->SetMoveOverRide(Ret.GetMove());
							//アクションが違う場合には上書き
							if (Ret.GetFreeData()[0] != static_cast<int>(c->GetCharaAction())) {
								c->SetActionOverRide(static_cast<EnumArmAnimType>(Ret.GetFreeData()[0]));
							}
							//ダメージイベント処理
							Ret.GetDamageEvent().PutDamageEvent(&this->m_DamageEvents);
						}
						m_Replay.Seek();
					}
						break;
					default:
						break;
					}

					if (!m_isTraining) {
						Vector3DX pos_c;
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							auto& p = PlayerMngr->GetPlayer(index);
							pos_c += p->GetChara()->GetMove().GetPos();
						}
						pos_c /= static_cast<float>(PlayerMngr->GetPlayerNum());

						for (int index = 0; index < 3; ++index) {
							auto& c = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
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
							if (c->SetDamageEvent(this->m_DamageEvents[static_cast<size_t>(j)], m_isTraining)) {
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
						if (m_IsPlayable) {
							m_DivideTimer -= DXLib_refParts->GetDeltaTime();
						}
					}
					else {
						m_DivideTimer = 3.f;
					}
				}
				//Execute
				ObjMngr->ExecuteObject();
				BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				{
					auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
					//カメラ
					Vector3DX CamPos = ViewChara->GetEyePosition();
					Vector3DX CamVec = CamPos + ViewChara->GetEyeMatrix().zvec() * -1.f;
					CamVec += CameraShake::Instance()->GetCamShake();
					CamPos += CameraShake::Instance()->GetCamShake() * 2.f;
#ifdef DEBUG
					if (CheckHitKey(KEY_INPUT_F1) != 0) {
						DBG_CamSel = -1;
					}
					if (CheckHitKey(KEY_INPUT_F2) != 0) {
						DBG_CamSel = 0;
					}
					if (CheckHitKey(KEY_INPUT_F3) != 0) {
						DBG_CamSel = 1;
					}
					if (CheckHitKey(KEY_INPUT_F4) != 0) {
						DBG_CamSel = 2;
					}
					if (CheckHitKey(KEY_INPUT_F5) != 0) {
						DBG_CamSel = 3;
					}
					if (CheckHitKey(KEY_INPUT_F6) != 0) {
						DBG_CamSel = 4;
					}
					if (CheckHitKey(KEY_INPUT_F7) != 0) {
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
					WindowSizeParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeMatrix().yvec());
					//info
					WindowSizeParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.3f, Scale3DRate * 20.f);
					//DoF
					PostPassParts->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, Scale3DRate * 20.f);
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
					PostPassParts->Set_is_Blackout(m_Concussion > 0.f);
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
					PostPassParts->Set_Per_Blackout(m_ConcussionPer * 2.f);
					m_Concussion = std::max(m_Concussion - DXLib_refParts->GetDeltaTime(), 0.f);
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
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good))->StopAll();
			m_PauseMenuControl.DisposePause();
			ButtonParts->Dispose();
			if (m_IsEventSceneActive) {
				m_EventScene.Dispose();
			}
			//使い回しオブジェ系
			BackGround->Dispose();
			//
			if (m_NetWorkController) {
				m_NetWorkController->Dispose();
				m_NetWorkController.reset();
			}
			{
				PostPassParts->SetLevelFilter(0, 255, 1.f);
				PostPassParts->SetAberrationPower(1.f);
				PostPassParts->Set_is_Blackout(false);
				PostPassParts->Set_Per_Blackout(0.f);
				PostPassParts->Set_is_lens(false);
				PostPassParts->Set_zoom_lens(1.f);
			}

			this->m_Tutorial.Dispose();

			if (this->m_IsEnd) {//タイトルに戻る
				SetNextSelect(0);
			}
			else {//次のシーンへ
				SetNextSelect(1);
			}

			LightPool::Instance()->Dispose();
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* SE = SoundPool::Instance();
			auto* HitMarkParts = HitMark::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			BattleResourceMngr->Dispose();
			this->m_UIclass.Dispose();
			PlayerMngr->Dispose();
			ObjMngr->DeleteAll();
			HitMarkParts->Dispose();
			m_GameStart.Dispose();
			m_GameRestart.Dispose();
			m_Once.Dispose();
			m_GameEnd.Dispose();
			m_Result.Dispose();

			this->m_Tutorial.Dispose_Load();
			SE->Delete(SoundType::BGM, 0);
			SE->Delete(SoundType::BGM, 1);
		}

		//
		void			MainGameScene::BG_Draw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.BGDraw();
			}
			else {
				BackGround->BG_Draw();
			}
		}
		void			MainGameScene::ShadowDraw_Far_Sub(void) const noexcept {
			//auto* BackGround = BackGround::BackGroundClass::Instance();
			if (m_IsResult) {
				return;
			}
			if (!m_IsEventSceneActive) {
				//BackGround->Shadow_Draw_Far();
			}
		}
		void			MainGameScene::ShadowDraw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.ShadowDraw();
			}
			else {
				BackGround->Shadow_Draw();
				ObjMngr->Draw_Shadow();
			}
		}
		void			MainGameScene::CubeMap_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.BGDraw();
			}
			else {
				BackGround->Draw();
			}
		}

		void			MainGameScene::SetShadowDraw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.SetShadowDraw();
			}
			else {
				BackGround->Draw();
				ObjMngr->Draw();
			}
		}

		void			MainGameScene::MainDraw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* HitMarkParts = HitMark::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			if (m_IsResult) {
				return;
			}
			if (m_IsEventSceneActive) {
				m_EventScene.MainDraw();
			}
			else {
				SetFogStartEnd(WindowSizeParts->GetMainCamera().GetCamNear(), WindowSizeParts->GetMainCamera().GetCamFar() * 2.f);
				BackGround->Draw();
				ObjMngr->Draw();
				//ObjMngr->Draw_Depth();
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					PlayerMngr->GetPlayer(i)->GetAI()->Draw();
				}
				HitMarkParts->Update();
			}
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* HitMarkParts = HitMark::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			if (!m_IsEventSceneActive) {
				if (m_isTraining) {
					this->m_Tutorial.Draw();
				}
			}

			if (m_GameControlType == GameControlType::Replay) {
				WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (32),
					FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, (64), (64), Green, Black, "Replay");
			}

			if (m_IsResult) {
				WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Result,
					(0), (0), (1920), (1080), false);

				bool IsWin = (PlayerMngr->GetPlayer(GetMyPlayerID())->GetMaxScore() > PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetMaxScore());
				bool IsDraw = (PlayerMngr->GetPlayer(GetMyPlayerID())->GetMaxScore() == PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetMaxScore());

				WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (48),
					FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, (300), (384),
					IsDraw ? Gray25 : (IsWin ? Red : White), Black,
					IsDraw ? LocalizeParts->Get(6002) : (IsWin ? LocalizeParts->Get(6000) : LocalizeParts->Get(6001)));
				WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
					FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, (300), (386), White, Black, "%d : %d",
					PlayerMngr->GetPlayer(GetMyPlayerID())->GetMaxScore(), PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetMaxScore());

				ButtonParts->Draw();


				FadeControl::DrawFade();
				return;
			}
			if (m_IsEventSceneActive) {
				FadeControl::DrawFade();
				m_EventScene.UIDraw();
			}
			else {
				HitMarkParts->Draw();
				//UI
				if (!SceneParts->IsPause()) {
					this->m_UIclass.Draw();
					if (!m_isTraining) {
						int xp1 = (1920 / 2);
						int yp1 = (240);

						if ((m_GameStartAlpha * 255.f) > 1.f) {
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_GameStartAlpha), 0, 255));
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 0);
							WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
								m_pStart, xp1, yp1, m_GameStartScale,0.f, true);
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}

						if ((m_WinOnceAlpha * 255.f) > 1.f) {
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_WinOnceAlpha), 0, 255));
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
							WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
								&m_Once, xp1, yp1, m_WinOnceScale, 0.f, true);
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}

						if ((m_GameEndAlpha * 255.f) > 1.f) {
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_GameEndAlpha), 0, 255));
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
							WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
								&m_GameEnd, xp1, yp1, m_GameEndScale, 0.f, true);
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
						if (m_IsPlayable) {
							if (0 <= m_DivideTimer && m_DivideTimer < 3.f) {
								if (static_cast<int>(m_DivideTimer * 100) % 30 < 15) {
									WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
										FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, (32), (384),
										Yellow, Black,
										"場外まであと%3.1f秒", m_DivideTimer);
								}
							}
						}
					}
				}
				FadeControl::DrawFade();
			}
		}
		void			MainGameScene::DrawUI_In_Sub(void) const noexcept {
			auto* SceneParts = SceneControl::Instance();
			if (m_IsResult) {
				return;
			}
			if (m_GameControlType == GameControlType::Replay) {
				return;
			}
			//UI
			if (SceneParts->IsPause()) {
				m_PauseMenuControl.DrawPause();
			}
			if (!m_IsEventSceneActive) {
				//通信設定
				//auto* NetBrowser = NetWorkBrowser::Instance();
				//NetBrowser->Draw();
				if (m_NetWorkController) {
					if (m_NetWorkController->GetPing() >= 0.f) {
						WindowSystem::SetMsg((1920), (32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:%3dms", static_cast<int>(m_NetWorkController->GetPing()));
					}
					else {
						if (m_NetWorkController->GetClient()) {
							WindowSystem::SetMsg((1920), (32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, Red, Black, "Lost Connection");
						}
						else {
							WindowSystem::SetMsg((1920), (32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:---ms");
						}
					}
				}
			}
		}
	}
}
