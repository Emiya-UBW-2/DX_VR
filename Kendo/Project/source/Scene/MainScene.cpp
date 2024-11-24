#pragma warning(disable:4464)
#include	"MainScene.hpp"
#include	"../MainScene/NetworkBrowser.hpp"

#include	"../MainScene/Object/Judge.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void MainGameScene::SetupStartCommon(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			this->m_IsGameStart = false;
			//初期化
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& c = PlayerMngr->GetPlayer(index)->GetChara();
				{
					Vector3DX pos_t = Vector3DX::vget(0.f, 0.f, (-1.5f * Scale3DRate) * static_cast<float>(index * 2 - 1));
					c->MovePoint(deg2rad(180.f * static_cast<float>(index)), pos_t);
				}
			}
			if (this->m_GameMode != GameMode::Training) {
				for (int index = 0; index < 3; ++index) {
					auto& j = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
					//人の座標設定
					j->SetWin(false, false);
					//人の座標設定
					{
						float Rad = deg2rad(-90.f + 120.f * static_cast<float>(index));
						Vector3DX pos_t = Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 0.f, (3.5f * Scale3DRate)), Matrix3x3DX::RotAxis(Vector3DX::up(), Rad));
						j->MovePoint(Rad, pos_t);
					}
				}
			}
		}
		void MainGameScene::SetupBattleStart(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* FadeInParts = FadeControl::Instance();
			//
			this->m_IsEnd = false;
			this->m_IsWinSound = false;
			this->m_IsGameEnd = false;
			this->m_IsTimeUp = false;
			this->m_IsDrawOneMinute = true;
			this->m_Timer = 180.f;
			this->m_IsPlayable = false;
			PlayerMngr->ResetScore();
			//
			FadeInParts->SetFadeIn(2.f);
			this->m_UIclass.SetGameStart(true);
			//
			SetupStartCommon();
			if (m_GameMode != GameMode::Training) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->Play(DX_PLAYTYPE_LOOP, TRUE);
			}
		}
		void MainGameScene::SetupBattleRestart(void) noexcept {
			auto* FadeInParts = FadeControl::Instance();
			//
			FadeInParts->SetFadeIn(1.f / 2.f);
			if (m_GameMode != GameMode::Training) {
				this->m_UIclass.SetGameStart(false);
			}
			//
			SetupStartCommon();
		}
		void MainGameScene::SetupResult(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good))->StopAll();
			KeyGuideParts->SetGuideFlip();
			m_ResultMenuControl.Set();
			m_GameControlType = GameControlType::Result;
		}
		void MainGameScene::SetupEvent(const char* Path) noexcept {
			m_IsEventSceneFlag = true;
			m_EventSelect = Path;
		}
		void MainGameScene::UpdateEvent(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* FadeInParts = FadeControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			if (!m_IsEventSceneActive) {
				if (m_IsEventSceneFlag) {
					m_IsEventSceneFlag = false;
					m_IsEventSceneActive = true;
					PostPassParts->SetDistortionPer(120.f * 4);
					m_EventScene.Load(m_EventSelect.c_str());
					m_EventScene.Start();
					KeyGuideParts->SetGuideFlip();
				}
			}
			if (m_IsEventSceneActive) {
				m_EventScene.GetDeltaTime();
				if (!SceneParts->IsPause() && Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
					SE->StopAll(SoundType::SE);
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
					m_EventScene.Skip();
				}
				if (m_EventScene.IsEnd()) {
					PostPassParts->SetDistortionPer(120.f);
					FadeInParts->SetFadeIn(1.f / ((m_GameMode == GameMode::Training) ? 0.5f : 2.f));
					m_EventScene.Dispose();
					m_IsEventSceneActive = false;
					KeyGuideParts->SetGuideFlip();
					if (m_GameMode == GameMode::Training) {
						if (m_EventSelect == "data/Cut/Cut2.txt") {
							SetupEnd();
						}
					}
				}
				m_EventScene.Update();
			}
		}
		void MainGameScene::SetupEnd(void) noexcept {
			auto* FadeInParts = FadeControl::Instance();
			if (!this->m_IsEnd) {
				FadeInParts->SetFadeOut(2.f);
				this->m_IsEnd = true;
			}
		}
		void MainGameScene::UpdateInGame(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* SideLogParts = SideLog::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* FadeInParts = FadeControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* CameraParts = Camera3D::Instance();
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
			auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			//FirstDoingv
			if (m_GameMode != GameMode::Training) {
				if (!m_IsPlayable) {
					if (!m_IsGameStart) {
						if (this->m_UIclass.IsGameStartEnd()) {
							m_IsGameStart = true;
							m_IsPlayable = true;
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Start))->Play(DX_PLAYTYPE_BACK, TRUE);
						}
					}
					else {
						if (!m_IsGameEnd) {
							if (this->m_UIclass.IsWinOnceEnd() || m_DivideTimer < 0.f) {
								if (FadeInParts->IsFadeClear()) {
									FadeInParts->SetFadeOut(2.f);
								}
								if (FadeInParts->IsFadeAll()) {
									SetupBattleRestart();
								}
							}
						}
						else {
							if (this->m_UIclass.IsWinOnceEnd()) {
								if (!m_IsWinSound) {
									m_IsWinSound = true;
									this->m_UIclass.SetGameEndStart();

									if (PlayerMngr->GetPlayer(0)->GetScore() != PlayerMngr->GetPlayer(1)->GetScore()) {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Win))->Play(DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Draw))->Play(DX_PLAYTYPE_BACK, TRUE);
									}
								}
								if (this->m_UIclass.IsGameEndEnd()) {
									if (FadeInParts->IsFadeClear()) {
										FadeInParts->SetFadeOut(1.f);
									}
									if (FadeInParts->IsFadeAll()) {
										FadeInParts->SetFadeIn(2.f);
										if (m_GameControlType == GameControlType::InGame) {
											SE->Get(SoundType::BGM, 1)->Play(DX_PLAYTYPE_BACK, TRUE);
											m_Replay.Save();
										}
										SetupResult();
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
						this->m_UIclass.SetWinOnceStart();
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
							for (int index = 0; index < 3; ++index) {
								auto& j = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
								//人の座標設定
								j->SetWin(true, false);
							}
						}
						else {
							for (int index = 0; index < 3; ++index) {
								auto& j = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
								//人の座標設定
								j->SetWin(false, true);
							}
						}
						//どちらかが2点先に取ったら
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
							if (PlayerMngr->GetPlayer(index)->GetScore() >= 2.f) {
								m_IsGameEnd = true;
								break;
							}
						}
					}
					else {
						//タイムアップ
						if (!m_IsTimeUp && (m_Timer < 0.f)) {
							m_IsTimeUp = true;
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::TimeUp))->Play(DX_PLAYTYPE_BACK, TRUE, 255);

							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Stop))->Play(DX_PLAYTYPE_BACK, TRUE);
							m_IsPlayable = false;
							m_IsGameEnd = true;
							this->m_UIclass.SetTimeUpStart();
						}
						if (m_TimerSE != static_cast<int>(m_Timer)) {
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
				m_IsPlayable = FadeInParts->IsFadeClear() && !m_IsEventSceneActive;

				if (!m_IsPlayable) {
					if (FadeInParts->IsFadeAll()) {
						FadeInParts->SetFadeIn(1.f / 2.f);
						m_TutorialResetTimer = 0.f;
						SetupBattleRestart();
					}
				}
				else {
					if (PlayerMngr->PutAddScoreFlag() && (PlayerMngr->GetWinPlayer() == GetMyPlayerID())) {
						if (m_Tutorial.CheckHitOk(PlayerMngr->GetWinHitType())) {
							m_TutorialResetTimer = 2.f;
						}
					}
					if (m_DivideTimer < 0.f && m_TutorialResetTimer == 0.f) {
						m_TutorialResetTimer = 0.001f;
					}

					if (m_TutorialResetTimer > 0.f) {
						m_TutorialResetTimer -= DXLib_refParts->GetDeltaTime();
						if (m_TutorialResetTimer <= 0.f) {
							if (FadeInParts->IsFadeClear()) {
								FadeInParts->SetFadeOut(2.f);
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

							SetupEvent("data/Cut/Cut2.txt");
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
					MyInput.SetAddxRad(Pad->GetLS_Y());
					MyInput.SetAddyRad(Pad->GetLS_X());
					MyInput.SetInputPADS(Controls::PADS::MOVE_W, Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_S, Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_A, Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_D, Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().press());

					MyInput.SetInputPADS(Controls::PADS::SHOT, Pad->GetPadsInfo(Controls::PADS::SHOT).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::AIM, Pad->GetPadsInfo(Controls::PADS::AIM).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::ULT, Pad->GetPadsInfo(Controls::PADS::ULT).GetKey().press());

					MyInput.SetInputPADS(Controls::PADS::WALK, Pad->GetPadsInfo(Controls::PADS::WALK).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::JUMP, Pad->GetPadsInfo(Controls::PADS::JUMP).GetKey().press());
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
				if (m_GameControlType == GameControlType::InGame || m_GameControlType == GameControlType::Network) {
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
					if (m_GameMode != GameMode::Training) {
						m_Replay.Add();
					}
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = p->GetChara();
						c->SetViewID(GetMyPlayerID());
						if ((PlayerID)index == GetMyPlayerID()) {
							if (m_GameMode != GameMode::Training) {
								int32_t FreeData[10]{};
								FreeData[0] = static_cast<int>(c->GetCharaAction());
								m_Replay.SetBack().SetOnce((PlayerID)index).SetMyPlayer(MyInput, c->GetMove(), c->GetDamageEvent(), FreeData);
							}
							c->SetInput(MyInput);
						}
						else {
							InputControl OtherInput;
							if (m_IsPlayable) {
								p->GetAI()->Execute(&OtherInput, m_GameMode == GameMode::Training);
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

							if (m_GameMode != GameMode::Training) {
								int32_t FreeData[10]{};
								FreeData[0] = static_cast<int>(c->GetCharaAction());
								m_Replay.SetBack().SetOnce((PlayerID)index).SetMyPlayer(OtherInput, c->GetMove(), c->GetDamageEvent(), FreeData);
							}
							c->SetInput(OtherInput);
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
						auto& c = p->GetChara();
						NetWork::PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
						c->SetViewID(GetMyPlayerID());
						if (index == GetMyPlayerID() && !IsServerNotPlayer) {
							c->SetInput(Ret.GetInput());//自身が動かすもの
						}
						else {//サーバーからのデータで動くもの
							//サーバーがCPUを動かす場合
							if (!m_NetWorkController->GetClient()) {
								//cpu
								//p->GetAI()->Execute(&MyInput);
							}
							c->SetInput(Ret.GetInput());
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
						auto& c = PlayerMngr->GetPlayer(index)->GetChara();
						c->SetViewID(GetMyPlayerID());
						NetWork::PlayerSendData Ret = m_Replay.GetNow().GetOnce((PlayerID)index);
						//サーバーがCPUを動かす場合
						c->SetInput(Ret.GetInput());
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

				if (m_GameMode != GameMode::Training) {
					Vector3DX pos_c;
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						pos_c += PlayerMngr->GetPlayer(index)->GetChara()->GetMove().GetPos();
					}
					pos_c /= static_cast<float>(PlayerMngr->GetPlayerNum());

					for (int index = 0; index < 3; ++index) {
						auto& j = (std::shared_ptr<Sceneclass::JudgeClass>&) * ObjMngr->GetObj(static_cast<int>(ObjType::Judge), index);
						//人の座標設定
						{
							float Rad = deg2rad(-90.f + 120.f * static_cast<float>(index));
							Vector3DX pos_t = Vector3DX::vget(0.f, 0.f, (3.5f * Scale3DRate));
							pos_t = Matrix3x3DX::Vtrans(pos_t, Matrix3x3DX::RotAxis(Vector3DX::up(), Rad));
							j->SetInput(pos_c + pos_t, pos_c);
						}
					}
				}
				//ダメージイベント
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
					auto& c = PlayerMngr->GetPlayer(index)->GetChara();
					for (int j = 0, Num = static_cast<int>(this->m_DamageEvents.size()); j < Num; ++j) {
						if (c->SetDamageEvent(this->m_DamageEvents[static_cast<size_t>(j)], m_GameMode == GameMode::Training)) {
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
					auto& c = PlayerMngr->GetPlayer(index)->GetChara();
					//場外判定
					if (c->IsOutArea()) {
						IsWaitOutSide = true;
						break;
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
			//竹刀判定
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& c = PlayerMngr->GetPlayer(index)->GetChara();
				if (!CharacterObject::CharaMove::IsAttackAction(c->GetCharaAction())) { continue; }
				if (c->GetGuardOn()) { continue; }
				Vector3DX StartPos = c->GetWeaponPtr()->GetMove().GetPos();
				Vector3DX EndPos = c->GetWeaponPtr()->GetFramePosition(WeaponObject::WeaponFrame::End);
				StartPos = StartPos + (EndPos - StartPos) * 0.5f;

				for (int index2 = 0; index2 < PlayerMngr->GetPlayerNum(); ++index2) {
					if (index == index2) { continue; }
					auto& tgt = PlayerMngr->GetPlayer(index2)->GetChara();
					tgt->CheckDamageRay(
						static_cast<HitPoint>(100.f * c->GetWeaponPtr()->GetMoveSpeed() / 5.f),
						c->GetMyPlayerID(),
						c->GetYaTimerPer(),
						c->GetWazaType(),
						StartPos, &EndPos);
				}
			}
			//コンカッション
			{
				if (Chara->PopConcussionSwitch()) {
					m_Concussion = 1.f;
					CameraParts->SetCamShake(0.5f, 0.01f * Scale3DRate);
				}
				PostPassParts->Set_is_Blackout(m_Concussion > 0.f);
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
			//視点
			{
				auto& ViewChara = PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				//カメラ
				Vector3DX CamPos = ViewChara->GetEyePosition();
				Vector3DX CamVec = CamPos + ViewChara->GetEyeMatrix().zvec() * -1.f;
				CamVec += CameraParts->GetCamShake();
				CamPos += CameraParts->GetCamShake() * 2.f;
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
				CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeMatrix().yvec());
				//info
				CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.3f, Scale3DRate * 20.f);
				//DoF
				PostPassParts->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, Scale3DRate * 20.f);
			}
			BackGround->Execute();
			//UIパラメーター
			{
				//UIパラメーター
				if (GetIsFirstLoop() || FadeInParts->IsFadeAll()) {
					this->m_UIclass.InitGaugeParam(0, 100, 100);
					this->m_UIclass.InitGaugeParam(1, 300000, 300000);
					this->m_UIclass.InitGaugeParam(2, 100, 100);
				}
				//NvsN
				this->m_UIclass.SetIntParam(0, PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore());
				this->m_UIclass.SetIntParam(1, PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetScore());
				//timer
				this->m_UIclass.SetfloatParam(0, m_Timer);
				this->m_UIclass.SetIntParam(3, (m_IsGameEnd || (m_GameMode == GameMode::Training)) ? 1 : 0);
				//場外表示
				this->m_UIclass.SetfloatParam(1, m_IsPlayable ? m_DivideTimer : -1.f);
				//心拍数
				this->m_UIclass.SetIntParam(2, static_cast<int>(Chara->GetHeartRate()));
				this->m_UIclass.SetfloatParam(2, Chara->GetHeartRatePow());
				//ゲージ
				this->m_UIclass.SetGaugeParam(0, static_cast<int>((1.f - Chara->GetYaTimerPer()) * 100.f), 100, 2);
				this->m_UIclass.SetGaugeParam(1, static_cast<int>(Chara->GetStaminaPer() * 300000.f), 300000, 15);
				this->m_UIclass.SetGaugeParam(2, static_cast<int>((1.f - Chara->GetGuardCoolDownTimerPer()) * 100.f), 100, 0);

				this->m_UIclass.Update();
			}
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
		}
		//
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			//
			SE->Add(SoundType::BGM, 0, 1, "data/Sound/BGM/BattleStart.wav", false);
			SE->Add(SoundType::BGM, 1, 1, "data/Sound/BGM/Result.wav", false);
			//BG
			BackGround->Load(m_GameMode == GameMode::Training);
			//
			BattleResourceMngr->Load();
			//
			PlayerMngr->Init(NetWork::Player_num);
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				BattleResourceMngr->LoadChara("Chara", (PlayerID)index);
				BattleResourceMngr->LoadWeapon("Bamboo", (PlayerID)index);
			}
			//
			if (m_GameMode != GameMode::Training) {
				for (int index = 0; index < 3; ++index) {
					BattleResourceMngr->LoadJudge("Suit");
				}
			}
			//UI
			this->m_UIclass.Load();
			//
			if (m_GameMode == GameMode::Training) {
				m_Tutorial.Load();
			}
			//
			m_ResultMenuControl.Load();
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			//
			BattleResourceMngr->Set();
			m_PauseMenuControl.Set();
			BackGround->Init();
			//
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = p->GetChara();
				c->SetViewID(GetMyPlayerID());
				p->GetAI()->Init((PlayerID)index);
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Init();
			//
			SetupBattleStart();
			//
			if (m_GameMode == GameMode::Training) {
				Vector3DX LightVec = Vector3DX::vget(1.f, -0.5f, 0.05f); LightVec = LightVec.normalized();
				PostPassParts->SetAmbientLight(LightVec);

				SetLightEnable(FALSE);

				auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
				SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
				SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));

				auto& SecondLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec * -1.f);
				SetLightDifColorHandle(SecondLight.get(), GetColorF(0.5f, 0.5f, 0.3f, 0.1f));

				PostPassParts->SetGodRayPer(0.5f);
			}
			else {
				Vector3DX LightVec = Vector3DX::vget(0.5f, -0.85f, 0.05f); LightVec = LightVec.normalized();
				PostPassParts->SetAmbientLight(LightVec);

				SetLightEnable(FALSE);

				auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
				SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
				SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));

				PostPassParts->SetGodRayPer(0.25f);
			}
			PostPassParts->SetShadowScale(0.75f);
			//
			if (m_GameMode == GameMode::Replay) {
				m_Replay.Clear();
				m_Replay.Load();
				m_GameControlType = GameControlType::Replay;
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			else {
				m_Replay.Clear();
				m_GameControlType = GameControlType::InGame;
				if (m_GameMode == GameMode::Training) {
					m_Tutorial.Set();
				}
			}
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* FadeInParts = FadeControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			//
			if (GetIsFirstLoop()) {
				if (m_GameMode == GameMode::Training) {
					SetupEvent("data/Cut/Cut1.txt");
				}
				else if (m_GameMode == GameMode::Main) {
					SetupEvent("data/Cut/Cut3.txt");
					SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_BACK, TRUE);
				}
			}
			//
			KeyGuideParts->ChangeGuide(
				[this]() {
					auto* SceneParts = SceneControl::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					if (SceneParts->IsPause()) {
						switch (m_GameControlType) {
						case GameControlType::InGame:
						case GameControlType::Network:
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizeParts->Get(9991));
							KeyGuideParts->AddGuide(Controls::GetIDtoOffset(Controls::GetStrtoID("LMOUSE", Controls::ControlType::PC), Controls::ControlType::PC), LocalizeParts->Get(9919));
							break;
						case GameControlType::Replay:
							break;
						case GameControlType::Result:
							break;
						default:
							break;
						}
					}
					else {
						switch (m_GameControlType) {
						case GameControlType::InGame:
						case GameControlType::Network:
							if (m_IsEventSceneFlag || m_IsEventSceneActive) {
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9914));
							}
							else {
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9900));

								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::SHOT), LocalizeParts->Get(9906));
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::AIM), LocalizeParts->Get(9908));
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ULT), LocalizeParts->Get(9907));

								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::WALK), LocalizeParts->Get(9903));
								KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::JUMP), LocalizeParts->Get(9905));
								if (m_GameMode == GameMode::Training) {
									KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), "");
									KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizeParts->Get(9916));
								}
							}
							break;
						case GameControlType::Replay:
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::JUMP), LocalizeParts->Get(9918));
							break;
						case GameControlType::Result:
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9915));
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::JUMP), LocalizeParts->Get(9917));
							KeyGuideParts->AddGuide(Controls::GetIDtoOffset(Controls::GetStrtoID("LMOUSE", Controls::ControlType::PC), Controls::ControlType::PC), LocalizeParts->Get(9919));
							break;
						default:
							break;
						}
					}
				});
			//
			FadeInParts->Update();
			if (this->m_IsEnd && FadeInParts->IsFadeAll()) {
				return false;
			}
			switch (m_GameControlType) {
			case GameControlType::InGame:
				Pad->SetMouseMoveEnable(!SceneParts->IsPause());
				if (SceneParts->IsPause()) {
					m_PauseMenuControl.UpdatePause();
				}
				else {
					ButtonParts->ResetSel();
				}
				if (m_PauseMenuControl.IsRetire()) {
					SetupEnd();
				}
				//
				UpdateEvent();
				if (m_IsEventSceneActive) {
					return true;
				}
				if (SceneParts->IsPause() && !m_NetWorkController) {
					return true;
				}
				if (m_GameMode == GameMode::Training) {
					m_Tutorial.Update();
				}
				UpdateInGame();
				break;
			case GameControlType::Network:
				Pad->SetMouseMoveEnable(!SceneParts->IsPause());
				if (SceneParts->IsPause() && !m_NetWorkController) {
					return true;
				}
				UpdateInGame();
				break;
			case GameControlType::Replay:
				Pad->SetMouseMoveEnable(false);
				if (SceneParts->IsPause() && !m_NetWorkController) {
					return true;
				}
				UpdateInGame();
				if (Pad->GetPadsInfo(Controls::PADS::JUMP).GetKey().trigger()) {
					SetupResult();
				}
				break;
			case GameControlType::Result:
				Pad->SetMouseMoveEnable(false);
				if (SceneParts->IsPause()) {
					ButtonParts->ResetSel();
				}
				if (SceneParts->IsPause() && !m_NetWorkController) {
					return true;
				}
				if (FadeInParts->IsFadeClear()) {
					this->m_ResultMenuControl.Update();
					if (this->m_ResultMenuControl.IsReplay()) {
						//リプレイ
						m_GameControlType = GameControlType::Replay;
						m_Replay.Start();
						KeyGuideParts->SetGuideFlip();
						SetupBattleStart();
					}
					if (this->m_ResultMenuControl.IsEnd()) {
						SetupEnd();
					}
				}
				break;
			default:
				break;
			}
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good))->StopAll();

			ButtonParts->Dispose();
			BackGround->Dispose();

			if (m_IsEventSceneActive) {
				m_EventScene.Dispose();
			}
			if (m_NetWorkController) {
				m_NetWorkController->Dispose();
				m_NetWorkController.reset();
			}
			if (m_GameMode == GameMode::Training) {
				this->m_Tutorial.Dispose();
			}

			SetNextSelect(0);//タイトルに戻る
			if (m_GameMode == GameMode::Main) {
				if (!m_PauseMenuControl.IsRetire()) {
					PlayerID Now = GetMyPlayerID();
					bool IsWin = (PlayerMngr->GetPlayer(Now)->GetMaxScore() > PlayerMngr->GetPlayer(1 - Now)->GetMaxScore());
					if (IsWin) {
						SaveDataClass::Instance()->SetParam("WinCount", std::max<int64_t>(0, SaveDataClass::Instance()->GetParam("WinCount")) + 1);
						SaveDataClass::Instance()->Save();
					}
				}
			}
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();

			BattleResourceMngr->Dispose();
			PlayerMngr->Dispose();
			ObjMngr->DeleteAll();
			this->m_ResultMenuControl.Dispose_Load();
			this->m_UIclass.Dispose_Load();
			if (m_GameMode == GameMode::Training) {
				this->m_Tutorial.Dispose_Load();
			}
			SE->Delete(SoundType::BGM, 0);
			SE->Delete(SoundType::BGM, 1);
		}
		//
		void			MainGameScene::BG_Draw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (m_GameControlType == GameControlType::Result) { return; }
			if (m_IsEventSceneActive) {
				m_EventScene.BGDraw();
			}
			else {
				BackGround->BG_Draw();
			}
		}
		void			MainGameScene::ShadowDraw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			if (m_GameControlType == GameControlType::Result) { return; }
			if (m_IsEventSceneActive) {
				m_EventScene.ShadowDraw();
			}
			else {
				BackGround->Shadow_Draw();
				ObjMngr->Draw_Shadow();
			}
		}
		void			MainGameScene::SetShadowDraw_Sub(void) const noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			if (m_GameControlType == GameControlType::Result) { return; }
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
			auto* HitMarkParts = HitMark::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* CameraParts = Camera3D::Instance();
			if (m_GameControlType == GameControlType::Result) { return; }
			if (m_IsEventSceneActive) {
				m_EventScene.MainDraw();
			}
			else {
				SetFogStartEnd(CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar() * 2.f);
				BackGround->Draw();
				ObjMngr->Draw();
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					PlayerMngr->GetPlayer(i)->GetAI()->Draw();
				}
				HitMarkParts->Update();
			}
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* FadeInParts = FadeControl::Instance();

			if (m_GameControlType == GameControlType::Result) {
				this->m_ResultMenuControl.Draw();
			}
			else if (m_IsEventSceneActive) {
					m_EventScene.UIDraw();
				}
			else if (!SceneParts->IsPause()) {
				if (m_GameMode == GameMode::Training) {
					this->m_Tutorial.Draw();
				}
				//UI
				if (m_GameControlType == GameControlType::Replay) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, (64), (64), Green, Black, "Replay");
				}
				this->m_UIclass.Draw();
			}
			if (!m_IsEventSceneActive) {
				FadeInParts->Draw();
			}
		}
		void			MainGameScene::DrawUI_In_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// ポーズ
			if (SceneParts->IsPause()) {
				if ((m_GameControlType != GameControlType::Result) && (m_GameControlType != GameControlType::Replay)) {
					ButtonParts->Draw();
				}
			}
			//UI
			if (!m_IsEventSceneActive) {
				//通信設定
				//auto* NetBrowser = NetWorkBrowser::Instance();
				//NetBrowser->Draw();
				if (m_NetWorkController) {
					if (m_NetWorkController->GetPing() >= 0.f) {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (32), White, Black,
							"Ping:%3dms", static_cast<int>(m_NetWorkController->GetPing()));
					}
					else {
						if (m_NetWorkController->GetClient()) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (32), White, Black,
								"Lost Connection");
						}
						else {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (32), White, Black,
								"Ping:---ms");
						}
					}
				}
			}
		}
	}
}
