#include	"../sub.hpp"
#include	"TitleScene.hpp"

#include	"../Header.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			auto* BGM = BGMPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			this->m_GameFadeIn = 1.f;
			this->m_GameStart = 0.f;
			this->m_TitleImage.Load("data/UI/Title.png");
			// 
			ButtonParts->ResetSel();
			// 
			ButtonParts->AddStringButton("Let's Go!", 52, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Reset SaveData", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, BaseScreenWidth - 96 - 64, 64, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, BaseScreenWidth - 64, 64, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			// クレジット
			this->m_CreditControl = std::make_unique<CreditControl>();
			this->m_CreditControl->Init();
			// 
			//*
			if (!BGM->Get(0)->Check()) {
				BGM->Get(0)->Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			BGM->Get(0)->SetVol_Local(255);
			// */
			m_CloseResetSave = false;
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			if (DrawParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[]() {
					auto* Pad = PadControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					Pad->AddGuide(PADS::MOVE_W, "");
					Pad->AddGuide(PADS::MOVE_S, "");
					Pad->AddGuide(PADS::MOVE_A, "");
					Pad->AddGuide(PADS::MOVE_D, "");
					Pad->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
					Pad->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));
				}
			);
			if (!PopUpParts->IsActivePop() && (this->m_GameFadeIn == 0.f) && (this->m_GameStart == 0.f)) {
				ButtonParts->UpdateInput();
				// 選択時の挙動
				if (ButtonParts->GetTriggerButton()) {
					switch (ButtonParts->GetSelect()) {
					case 0:
						this->m_GameStart += 0.0001f;
						break;
					case 1:
						PopUpParts->Add(LocalizeParts->Get(3100), DrawParts->GetUIY(480), DrawParts->GetUIY(240),
							[&](int xmin, int ymin, int xmax, int ymax, bool) {
								auto* LocalizeParts = LocalizePool::Instance();
								int xp1, yp1;
								//タイトル
								{
									xp1 = xmin + DrawParts->GetUIY(24);
									yp1 = ymin + LineHeight;

									WindowSystem::SetMsg(xp1, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(3101));
								}
								//
								{
									xp1 = (xmax + xmin) / 2 - DrawParts->GetUIY(54);
									yp1 = ymax - LineHeight * 3;

									auto* Pad = PadControl::Instance();
									bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + DrawParts->GetUIY(108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(3102));
									if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
										auto* SaveDataParts = SaveDataClass::Instance();
										SaveDataParts->Reset();
										SaveDataParts->Save();
										DrawParts->SetPause(false);
										m_CloseResetSave = true;
									}
								}
							},
							[]() {},
							[]() {},
							true
						);
						break;
					case 2:
						OptionWindowClass::Instance()->SetActive();
						break;
					case 3:
						PopUpParts->Add(LocalizeParts->Get(120), DrawParts->GetUIY(720), DrawParts->GetUIY(840),
							[&](int xmin, int ymin, int xmax, int, bool) {
								this->m_CreditControl->Draw(xmin, ymin, xmax);
							},
							[]() {},
							[]() {},
							true
						);
						break;
					default:
						break;
					}
					SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (m_CloseResetSave && PopUpParts->IsActivePop()) {
				m_CloseResetSave = false;
				PopUpParts->EndAll();
			}
			// 
			ButtonParts->Update();
			// 
			this->m_GameFadeIn = std::max(this->m_GameFadeIn - 1.f / DrawParts->GetFps() / 0.5f, 0.f);
			if (this->m_GameStart != 0.f) { this->m_GameStart += 1.f / DrawParts->GetFps() / 0.5f; }
			return (this->m_GameStart <= 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* BGM = BGMPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			BGM->Get(0)->Stop();
			// 
			this->m_CreditControl->Dispose();
			this->m_CreditControl.reset();
			// 
			ButtonParts->Dispose();
			// 
			this->m_TitleImage.Dispose();
			// セーブ
			SaveDataParts->Save();
			// 次シーン決定
			SetNextSelect(static_cast<size_t>(ButtonParts->GetSelect()));
		}
		// 
		void			TitleScene::MainDraw_Sub(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			// 背景
			WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), Gray50, TRUE);
		}
		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
				&this->m_TitleImage, DrawParts->GetUIY(64), DrawParts->GetUIY(64), DrawParts->GetUIY(64 + 369), DrawParts->GetUIY(64 + 207), true);
			// 
			WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), 
				FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
				DrawParts->GetUIY(64 + 369), DrawParts->GetUIY(64 + 207), White, Black, "Ver 1.0.1");
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), 
					FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
					DrawParts->GetUIY(32), DrawParts->GetUIYMax() - DrawParts->GetUIY(32 + 32), White, Black, LocalizeParts->Get(9020 + ButtonParts->GetSelect()));
			}
			// 
			{
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameFadeIn, 0.f, 1.f)), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), Black, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameStart, 0.f, 1.f)), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), White, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
