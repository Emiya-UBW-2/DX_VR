#include	"../sub.hpp"
#include	"TitleScene.hpp"

#include	"../Header.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			this->m_GameFadeIn = 1.f;
			this->m_GameStart = 0.f;
			this->m_TitleImage.Load("data/UI/Title.png");
			// 
			ButtonParts->ResetSel();
			// 
			ButtonParts->AddStringButton("Let's Go!", 52, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Reset SaveData", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, BaseScreenWidth - 96 - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, BaseScreenWidth - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			// クレジット
			this->m_CreditControl = std::make_unique<CreditControl>();
			this->m_CreditControl->Init();
			// 
			//*
			SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_LOOP, TRUE);
			// */
			m_CloseResetSave = false;
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* SE = SoundPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (SceneParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			auto* KeyGuideParts = KeyGuide::Instance();
			KeyGuideParts->ChangeGuide(
				[]() {
					auto* Pad = PadControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
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
						PopUpParts->Add(LocalizeParts->Get(3100), WindowSizeParts->GetUIY(480), WindowSizeParts->GetUIY(240),
							[&](int xmin, int ymin, int xmax, int ymax, bool) {
								auto* LocalizeParts = LocalizePool::Instance();
								int xp1, yp1;
								//タイトル
								{
									xp1 = xmin + WindowSizeParts->GetUIY(24);
									yp1 = ymin + LineHeight;

									WindowSystem::SetMsg(xp1, yp1 + LineHeight / 2, LineHeight, FontSystem::FontXCenter::LEFT, White, Black, LocalizeParts->Get(3101));
								}
								//
								{
									xp1 = (xmax + xmin) / 2 - WindowSizeParts->GetUIY(54);
									yp1 = ymax - LineHeight * 3;

									auto* Pad = PadControl::Instance();
									bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + WindowSizeParts->GetUIY(108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(3102));
									if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger() || ret) {
										auto* SaveDataParts = SaveDataClass::Instance();
										SaveDataParts->Reset();
										SaveDataParts->Save();
										WindowSizeParts->SetPause(false);
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
						OptionPopup::Instance()->SetActive();
						break;
					case 3:
						PopUpParts->Add(LocalizeParts->Get(120), WindowSizeParts->GetUIY(720), WindowSizeParts->GetUIY(840),
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
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (m_CloseResetSave && PopUpParts->IsActivePop()) {
				m_CloseResetSave = false;
				PopUpParts->EndAll();
			}
			// 
			ButtonParts->Update();
			// 
			this->m_GameFadeIn = std::max(this->m_GameFadeIn - 1.f / DXLib_refParts->GetFps() / 0.5f, 0.f);
			if (this->m_GameStart != 0.f) { this->m_GameStart += 1.f / DXLib_refParts->GetFps() / 0.5f; }
			return (this->m_GameStart <= 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			SE->Get(SoundType::BGM, 0)->Stop();
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
			auto* WindowSizeParts = WindowSizeControl::Instance();
			// 背景
			WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), Gray50, TRUE);
		}
		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
				&this->m_TitleImage, WindowSizeParts->GetUIY(64), WindowSizeParts->GetUIY(64), WindowSizeParts->GetUIY(64 + 369), WindowSizeParts->GetUIY(64 + 207), true);
			// 
			WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, WindowSizeParts->GetUIY(18), 
				FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
				WindowSizeParts->GetUIY(64 + 369), WindowSizeParts->GetUIY(64 + 207), White, Black, "Ver 1.0.1");
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, WindowSizeParts->GetUIY(18), 
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM,
					WindowSizeParts->GetUIY(32), WindowSizeParts->GetUIYMax() - WindowSizeParts->GetUIY(32 + 32), White, Black, LocalizeParts->Get(9020 + ButtonParts->GetSelect()));
			}
			// 
			{
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameFadeIn, 0.f, 1.f)), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), Black, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameStart, 0.f, 1.f)), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), White, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
