#include	"TitleScene.hpp"
#include	"../sub.hpp"

#include	"../Header.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			auto* BGM = BGMPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			// 
			this->m_GameFadeIn = 1.f;
			this->m_GameStart = 0.f;
			this->m_TitleImage.Load("data/UI/Title.png");
			// 
			ButtonParts->ResetSel();
			// 
			ButtonParts->AddStringButton(
				LocalizeParts->Get(400), 52, true,
				1920 - 64 - 48, 1080 - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(
				LocalizeParts->Get(401), 48, true,
				1920 - 64 - 48, 1080 - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton(
				"CommonData/UI/setting.png", true,
				(1920 - 96 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton(
				"CommonData/UI/credit.png", true,
				(1920 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
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
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* OptionWindowParts = OptionWindowClass::Instance();
			if (DrawParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[]() {
					auto* KeyGuide = PadControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
					KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));
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
						this->m_GameStart += 0.0001f;
						break;
					case 2:
						OptionWindowParts->SetActive();
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
			this->m_GameFadeIn = std::max(this->m_GameFadeIn - DXLib_refParts->GetDeltaTime() / 0.5f, 0.f);
			if (this->m_GameStart != 0.f) { this->m_GameStart += DXLib_refParts->GetDeltaTime() / 0.5f; }
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
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			// 背景
			WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Gray65, TRUE);
			// 描画
		}
		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			// 
			WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
				&this->m_TitleImage, DrawParts->GetUIY(64), DrawParts->GetUIY(64), DrawParts->GetUIY(64 + 1024), DrawParts->GetUIY(64 + 256), true);
			// 
			WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), 
				FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
				DrawParts->GetUIY(64), DrawParts->GetUIY(64 + 256), White, Black, "Ver 1.0.0");
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
				WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), 
					FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
					DrawParts->GetUIY(32), DrawParts->GetUIY(1080 - 32 - 32), White, Black, LocalizeParts->Get(410 + ButtonParts->GetSelect()));
			}
			// 
			{
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameFadeIn, 0.f, 1.f)), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Black, TRUE);

				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameStart, 0.f, 1.f)), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), White, TRUE);

				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
