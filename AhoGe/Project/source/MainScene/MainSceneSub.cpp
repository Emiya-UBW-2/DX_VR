#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void PauseMenuControl::LoadPause(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton("Retire", 48, true, 1920 - 64, 1080 - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Option", 48, true, 1920 - 64, 1080 - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Return Game", 48, true, 1920 - 64, 1080 - 84 - 64 * 0, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
		}
		void PauseMenuControl::SetPause(void) noexcept {
			this->m_IsRetire = false;
		}
		void PauseMenuControl::UpdatePause(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			if (DrawParts->IsPause()) {
				if (!DrawParts->IsExit() && !DrawParts->IsRestart()) {
					Pad->SetMouseMoveEnable(false);
					Pad->ChangeGuide(
						[]() {
							auto* KeyGuide = PadControl::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));
							KeyGuide->AddGuide(PADS::RELOAD, LocalizeParts->Get(9991));
							KeyGuide->AddGuide(PADS::MOVE_W, "");
							KeyGuide->AddGuide(PADS::MOVE_S, "");
							KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
						});
					if (!OptionWindowClass::Instance()->IsActive()) {
						ButtonParts->UpdateInput();
						// 選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
							case 0:
								this->m_IsRetire = true;
								DrawParts->SetPause(false);
								break;
							case 1:
								OptionWindowClass::Instance()->SetActive();
								break;
							case 2:
								DrawParts->SetPause(false);
								break;
							default:
								DrawParts->SetPause(false);
								break;
							}
							SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetKey(PADS::RELOAD).trigger()) {
							SE->Get(static_cast<int>(SoundEnumCommon::UI_CANCEL)).Play(0, DX_PLAYTYPE_BACK, TRUE);
							DrawParts->SetPause(false);
						}
						// 
						ButtonParts->Update();
					}
				}
			}
			else {
				ButtonParts->ResetSel();
			}
		}
		void PauseMenuControl::DrawPause(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				ButtonParts->Draw();
			}
		}
		void PauseMenuControl::DisposePause(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
		}
		//UI
		void InGameUIControl::DrawCharaUI_Back(PlayerID value) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			if (p->GetChara()) {
				Vector3DX DispPos;
				Convert2DtoDisp(p->GetChara()->GetPos(),&DispPos);
				double Deg = (double)p->GetChara()->GetViewRad() / (DX_PI * 2.0) * 100.0;
				double Watch;
				if (value == 0) {
					SetDrawBright(0, 0, 216);
					Watch = 15.0 / 360.0 * 100.0;
				}
				else {
					if (p->GetAI()->IsAlert()) {
						SetDrawBright(216, 0, 0);// 
					}
					else if (p->GetAI()->IsCaution()) {
						SetDrawBright(216, 216, 0);// 
					}
					else {
						SetDrawBright(0, 216, 0);// 
					}
					Watch = 45.0 / 360.0 * 100.0;
				}
				DrawCircleGauge(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), Deg + Watch, this->m_Watch.get(), Deg - Watch, (double)GetDispSize(10.f) / 64.0);
			}
		}
		void InGameUIControl::DrawCharaUI_Front(PlayerID value) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			if (p->GetChara() && p->GetChara()->CanLookPlayer0()) {
				if (p->GetAI()->GetGraphSwitch()) {
					p->GetChara()->SetGraphTimer(2.f);
				}
				if (p->GetChara()->GetGraphTimer() <= 0.f) { return; }

				float Per = 1.f;
				float StartTimer = 0.25f;
				if (p->GetChara()->GetGraphTimer() > 2.f - StartTimer) {
					Per = 1.f - (p->GetChara()->GetGraphTimer() - (2.f - StartTimer)) / StartTimer;
				}
				float EndTimer = 0.25f;
				if (p->GetChara()->GetGraphTimer() < 0.25f) {
					Per = (p->GetChara()->GetGraphTimer() - (0.25f - EndTimer)) / EndTimer;
				}
				Vector3DX DispPos;
				Convert2DtoDisp(p->GetChara()->GetPos(), &DispPos);
				int ShadowOfset = y_r(3);
				float Scale = static_cast<float>(y_r(128)) / 128.0f * Per;
				if (p->GetAI()->IsAlert()) {
					SetDrawBright(0, 0, 0);// 
					this->m_Alert.DrawRotaGraph(static_cast<int>(DispPos.x) + ShadowOfset, static_cast<int>(DispPos.y) - y_r(32) + ShadowOfset, Scale, 0.f, true);
					SetDrawBright(255, 0, 0);// 
					this->m_Alert.DrawRotaGraph(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y) - y_r(32), Scale, 0.f, true);
				}
				else if (p->GetAI()->IsCaution()) {
					SetDrawBright(0, 0, 0);// 
					this->m_Caution.DrawRotaGraph(static_cast<int>(DispPos.x) + ShadowOfset, static_cast<int>(DispPos.y) - y_r(32) + ShadowOfset, Scale, 0.f, true);
					SetDrawBright(255, 255, 0);// 
					this->m_Caution.DrawRotaGraph(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y) - y_r(32), Scale, 0.f, true);
				}
				else {
					SetDrawBright(0, 0, 0);// 
					this->m_Caution.DrawRotaGraph(static_cast<int>(DispPos.x) + ShadowOfset, static_cast<int>(DispPos.y) - y_r(32) + ShadowOfset, Scale, 0.f, true);
					SetDrawBright(0, 255, 0);// 
					this->m_Caution.DrawRotaGraph(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y) - y_r(32), Scale, 0.f, true);
				}
			}
		}
		void InGameUIControl::LoadUI(void) noexcept {
			this->m_Watch = GraphHandle::Load("data/UI/Watch.png");
			this->m_Caution = GraphHandle::Load("data/UI/Caution.png");
			this->m_Alert = GraphHandle::Load("data/UI/Alert.png");
			this->m_ViewHandle = GraphHandle::Make(y_r(1920), y_r(1080), true);
		}
		void InGameUIControl::SetUI(void) noexcept {
			this->m_MapDrawTime = 5.f;
		}
		void InGameUIControl::UpdateUI(void) noexcept {
			auto* DrawParts = DXDraw::Instance();

			this->m_MapDrawTime = std::max(this->m_MapDrawTime - 1.f / DrawParts->GetFps(), 0.f);
			float Per = 1.f;
			float StartTimer = 0.5f;
			if (this->m_MapDrawTime > 5.f - StartTimer) {
				Per = 1.f - (this->m_MapDrawTime - (5.f - StartTimer)) / StartTimer;
			}
			float EndTimer = 0.5f;
			if (this->m_MapDrawTime < 0.5f) {
				Per = (this->m_MapDrawTime - (0.5f - EndTimer)) / EndTimer;
			}
			this->m_MapDrawPer = std::clamp(Per, 0.f, 1.f);
		}
		void InGameUIControl::Dispose_LoadUI(void) noexcept {
			this->m_ViewHandle.Dispose();
			this->m_Watch.Dispose();
			this->m_Caution.Dispose();
			this->m_Alert.Dispose();
		}
		void InGameUIControl::SetupWatchScreen(void) noexcept {
			auto* BackGround = BackGroundClassBase::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* OptionParts = OPTION::Instance();
			this->m_ViewHandle.SetDraw_Screen(false);
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, true);
				// 視界
				for (int loop = 0; loop < 4; loop++) {
					for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
						auto& p = PlayerMngr->GetPlayer((PlayerID)i);
						if (i == 0) {
							if (loop == 3) {
								DrawCharaUI_Back((PlayerID)i);
							}
						}
						else {
							if (p->GetAI()->IsAlert()) {
								if (loop == 2) {
									DrawCharaUI_Back((PlayerID)i);
								}
							}
							else if (p->GetAI()->IsCaution()) {
								if (loop == 1) {
									DrawCharaUI_Back((PlayerID)i);
								}
							}
							else {
								if (loop == 0) {
									DrawCharaUI_Back((PlayerID)i);
								}
							}
						}
					}
				}
				SetDrawBright(255, 255, 255);
				if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
					SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
					BackGround->GetShadowGraph().DrawExtendGraph(0, 0, y_r(1920), y_r(1080), false);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
			}
		}
		void InGameUIControl::DrawUI_Back(void) noexcept {
			SetDrawBlendMode(DX_BLENDMODE_MULA, 92);
			this->m_ViewHandle.DrawGraph(0, 0, false);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		void InGameUIControl::DrawUI_Front(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			for (int i = 1; i < PlayerMngr->GetPlayerNum(); i++) {
				// !マーク
				DrawCharaUI_Front((PlayerID)i);
				//ID用デバッグ描画
				if (false) {
					SetDrawBright(255, 255, 255);
					auto& p = PlayerMngr->GetPlayer((PlayerID)i);
					p->GetAI()->Draw();
				}
			}
			SetDrawBright(255, 255, 255);
		}
		void InGameUIControl::DrawUI_MapName(void) noexcept {
			if (this->m_MapDrawPer > 1.f / 255.f) {
				auto* Fonts = FontPool::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_MapDrawPer), 0, 255));
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(64), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
					y_UI(64), y_UI(128), White, Black, LocalizeParts->Get(this->m_MapTextID));
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
		//カットシーン制御
		void CutSceneControl::StartCutScene(int ID) noexcept {
			this->m_CutSceneData.clear();
			std::string Path; Path.reserve(24);
			sprintfDx(Path.data(), "data/CutScene/%03d.txt", ID);
			int mdata = FileRead_open(Path.c_str(), FALSE);
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				if (ALL == "") { continue; }
				auto LEFT = getparams::getleft(ALL);
				auto RIGHT = getparams::getright(ALL);

				//int targetID = std::stoi(LEFT.substr(0, 3));
				//イベントタイプ
				if (LEFT.find("Type") != std::string::npos) {
					this->m_CutSceneData.resize(this->m_CutSceneData.size() + 1);
					for (int i = 0; i < static_cast<int>(CutSceneType::Max); i++) {
						if (RIGHT.find(g_CutSceneStr[i]) != std::string::npos) {
							this->m_CutSceneData.back().m_Type = (CutSceneType)i;
							break;
						}
					}
				}
				//メッセージ
				else if (LEFT.find("Name") != std::string::npos) {
					this->m_CutSceneData.back().m_NameID = std::stoi(RIGHT);
				}
				else if (LEFT.find("Msg") != std::string::npos) {
					this->m_CutSceneData.back().m_MsgID = std::stoi(RIGHT);
				}
				//カメラ移動
				else if (LEFT.find("Point") != std::string::npos) {
					auto X = std::stoi(RIGHT.substr(0, RIGHT.find(',')));
					auto Y = std::stoi(RIGHT.substr(RIGHT.find(',') + 1));
					this->m_CutSceneData.back().m_PointX = X;
					this->m_CutSceneData.back().m_PointY = Y;
				}
				//待機
				else if (LEFT.find("WaitMilSec") != std::string::npos) {
					this->m_CutSceneData.back().m_WatiMS = std::stoi(RIGHT);
				}
			}
			FileRead_close(mdata);
			this->m_IsCutScene = true;
			this->m_CutSceneSeek = 0;
		}
		void CutSceneControl::SetCut(void) noexcept {
			this->m_IsCutScene = false;
			this->m_CutSceneAlpha = 0.f;
			this->m_MsgBoxSeek = 0.f;
			this->m_WaitMS = 0.f;
		}
		void CutSceneControl::UpdateCut(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();

			if (this->m_IsCutScene) {
				Pad->SetMouseMoveEnable(false);
				Pad->ChangeGuide(
					[]() {
						auto* KeyGuide = PadControl::Instance();
						auto* LocalizeParts = LocalizePool::Instance();
						KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9902));//todo
					});
			}
			//カットシーン全体
			this->m_CutSceneAlpha = std::clamp(this->m_CutSceneAlpha + (this->m_IsCutScene ? 1.f : -1.f) / DrawParts->GetFps() / 0.5f, 0.f, 1.f);
			//メッセージシーク
			this->m_MsgBoxAlpha = std::clamp(this->m_MsgBoxAlpha + (this->m_IsMsgBox ? 1.f : -1.f) / DrawParts->GetFps() / 0.1f, 0.f, 1.f);
			if (this->m_MsgBoxAlpha >= 1.f) {
				this->m_MsgBoxSeek += 1.f / DrawParts->GetFps() / 0.1f;
			}
			//カットシーン中のボタン制御
			if (this->m_CutSceneAlpha >= 1.f) {
				if (0 <= this->m_CutSceneSeek && this->m_CutSceneSeek < static_cast<int>(this->m_CutSceneData.size())) {
					this->m_IsMsgBox = false;
					bool IsGoNext = false;
					auto& Data = this->m_CutSceneData.at(static_cast<size_t>(this->m_CutSceneSeek));
					switch (Data.m_Type) {
					case CutSceneType::MsgBox:
						this->m_IsMsgBox = true;
						break;
					case CutSceneType::End:
						this->m_IsCutScene = false;//おわり
						break;
					case CutSceneType::ViewPoint:
						this->m_PointX = Data.m_PointX;
						this->m_PointY = Data.m_PointY;
						IsGoNext = true;
						break;
					case CutSceneType::WaitMilSec:
						IsGoNext = static_cast<int>(this->m_WaitMS * 1000.f) > Data.m_WatiMS;
						this->m_WaitMS += 1.f / DrawParts->GetFps();
						break;
					case CutSceneType::Max:
					default:
						break;
					}
					if (Pad->GetKey(PADS::INTERACT).trigger() || (IsGoNext)) {
						for (auto& m : this->m_MsgString) {
							m = "";
						}
						this->m_WaitMS = 0.f;
						this->m_MsgBoxSeek = 0.f;
						this->m_PrevMsgBoxSeek = 0;
						this->m_CutSceneSeek++;
					}
				}
				else {
					this->m_IsCutScene = false;//おわり
				}
			}
			else {
				this->m_PointX = 0;
				this->m_PointY = 0;
			}
		}
		void CutSceneControl::DrawCut(void) noexcept {
			if (this->m_CutSceneAlpha > 1.f / 255.f) {
				auto Color = GetColor(16, 16, 16);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_CutSceneAlpha), 0, 255));
				DrawBox(0, 0, y_UI(1920), y_UI(120), Color, TRUE);
				DrawBox(0, y_UI(1080 - 120), y_UI(1920), y_UI(1080), Color, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			if (this->m_MsgBoxAlpha > 1.f / 255.f) {
				float Per = Lerp(0.5f, 1.f, this->m_MsgBoxAlpha);
				int x1 = y_UI(64);
				int y1 = y_UI((1080 - 64) - static_cast<int>((320 - 64) * Per));
				int x2 = y_UI(1920 - 48);
				int y2 = y_UI(1080 - 64 - static_cast<int>((320 - 64) * (1.f - Per)));

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_MsgBoxAlpha), 0, 255));
				DrawBox(x1, y1, x2, y2, Gray50, TRUE);
				if (this->m_MsgBoxAlpha >= 1.f) {
					int NameID{};
					int MsgID{};
					if (0 <= this->m_CutSceneSeek && this->m_CutSceneSeek < static_cast<int>(this->m_CutSceneData.size())) {
						auto& Data = this->m_CutSceneData.at(static_cast<size_t>(this->m_CutSceneSeek));
						NameID = Data.m_NameID;
						MsgID = Data.m_MsgID;
					}
					auto* Fonts = FontPool::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
						x1 + y_UI(32), y1 + y_UI(32), White, Black, LocalizeParts->Get(NameID));

					int NowC = static_cast<int>(this->m_MsgBoxSeek);
					if(this->m_PrevMsgBoxSeek!= NowC){
						this->m_PrevMsgBoxSeek = NowC;
						std::string Msg = LocalizeParts->Get(MsgID);
						std::string NowMsg; NowMsg.reserve(512);
						strncpy2_sDx(NowMsg.data(), 512, LocalizeParts->Get(MsgID), std::min(512, NowC));
						NowMsg = NowMsg.c_str();
						for (auto& m : this->m_MsgString) {
							m = "";
						}
						for (int i = 0; i < 10; i++) {
							if (NowMsg == "") { break; }
							std::string Tmp; Tmp.reserve(512);

							int Limit = ((x2 - x1) - y_UI(64) * 2);
							int column = Limit / y_UI(32);//超えない範囲
							while (true) {
								if (NowC <= column) {
									column = NowC;
									break;
								}
								strncpy2_sDx(Tmp.data(), 512, NowMsg.c_str(), column); Tmp = Tmp.c_str();
								if (Fonts->Get(FontPool::FontType::Nomal_AA, y_UI(32)).GetStringWidth(-1, Tmp) < Limit) {
									column++;
								}
								else {
									column--;
									break;
								}
							}
							NowC -= column;
							strncpy2_sDx(Tmp.data(), 512, NowMsg.c_str(), column); Tmp = Tmp.c_str();
							this->m_MsgString.at(static_cast<size_t>(i)) = Tmp;
							{
								while (true) {
									if (strchrDx(Tmp.c_str(), '\n')) {
										i++;
										strpncpy2_sDx(Tmp.data(), 512, Tmp.c_str(), strchr2Dx(Tmp.c_str(), '\n') + 1, static_cast<int>(strlen2Dx(Tmp.c_str()))); Tmp = Tmp.c_str();
									}
									else {
										break;
									}
								}
							}
							if (static_cast<int>(strlen2Dx(NowMsg.c_str())) > column) {
								strpncpy2_sDx(NowMsg.data(), 512, NowMsg.c_str(), column + 1, static_cast<int>(strlen2Dx(NowMsg.c_str()))); NowMsg = NowMsg.c_str();
							}
							else {
								NowMsg = "";
							}
						}
					}
					for(auto& m : this->m_MsgString){
						if (m == "") { continue; }
						int i = static_cast<int>(&m - &this->m_MsgString.front());
						Fonts->Get(FontPool::FontType::Nomal_AA, y_UI(32)).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
							x1 + y_UI(64), y1 + y_UI(64) + y_UI(32 * i), White, Black, m);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
		//
		void FadeControl::SetFade(void) noexcept {
			this->m_IsBlackOut = false;
			this->m_BlackOutAlpha = 1.f;
		}
		void FadeControl::UpdateFade(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.f : -1.f) / DrawParts->GetFps() / 0.5f, 0.f, 1.f);
		}
		void FadeControl::DrawFade(void) noexcept {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
			DrawBox(0, 0, y_UI(1920), y_UI(1080), Black, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
};
};
