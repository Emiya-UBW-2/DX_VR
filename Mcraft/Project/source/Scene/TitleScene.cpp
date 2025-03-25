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
			ButtonParts->AddStringButton("Start Game", 52, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, BaseScreenWidth - 96 - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, BaseScreenWidth - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			// クレジット
			this->m_CreditControl = std::make_unique<CreditControl>();
			this->m_CreditControl->Init();
			// 
			//*
			//SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_LOOP, TRUE);
			// */
			m_CloseResetSave = false;
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* SE = SoundPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SceneParts = SceneControl::Instance();
			if (SceneParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			auto* KeyGuideParts = KeyGuide::Instance();
			KeyGuideParts->ChangeGuide(
				[]() {
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
						OptionPopup::Instance()->SetActive();
						break;
					case 2:
						PopUpParts->Add(LocalizeParts->Get(120), (720), (840),
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
			this->m_GameFadeIn = std::max(this->m_GameFadeIn - DXLib_refParts->GetDeltaTime() / 0.5f, 0.f);
			if (this->m_GameStart != 0.f) { this->m_GameStart += DXLib_refParts->GetDeltaTime() / 0.5f; }
			return (this->m_GameStart <= 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			//SE->Get(SoundType::BGM, 0)->StopAll();
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
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
				&this->m_TitleImage, (64), (64), (64 + 369), (64 + 207), true);
			// 
			WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18), 
				FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
				(64 + 369), (64 + 207), White, Black, "Ver 0.0.0");
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18), 
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM,
					(32), 1080 - (32 + 32), White, Black, LocalizeParts->Get(9020 + ButtonParts->GetSelect()));
			}
			// 
			{
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameFadeIn, 0.f, 1.f)), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Black, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameStart, 0.f, 1.f)), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, White, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
