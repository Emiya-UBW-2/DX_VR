#include	"TitleScene.hpp"
#include	"../sub.hpp"

#include	"../Header.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			// auto* BGM = BGMPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			GameFadeIn = 1.f;
			GameStart = 0.f;
			m_TitleImage = GraphHandle::Load("data/UI/Title.png");
			// 
			ButtonParts->ResetSel();
			// 
			ButtonParts->AddStringButton(
				"Let's Go!", 52, true,
				1920 - 64 - 48, 1080 - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(
				"Test Stage", 48, true,
				1920 - 64, 1080 - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton(
				"data/UI/setting.png", true,
				(1920 - 96 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton(
				"data/UI/credit.png", true,
				(1920 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			// クレジット
			m_CreditControl.Init();
			// 
			/*
			if (!BGM->Get(0).Check()) {
				BGM->Get(0).Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			BGM->Get(0).SetVol_Local(255);
			// */
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
				[&]() {
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
			if (!PopUpParts->IsActivePop() && (GameFadeIn == 0.f) && (GameStart == 0.f)) {
				ButtonParts->UpdateInput();
				// 選択時の挙動
				if (ButtonParts->GetTriggerButton()) {
					switch (ButtonParts->GetSelect()) {
						case 0:
						case 1:
							GameStart += 0.0001f;
							break;
						case 2:
							OptionWindowClass::Instance()->SetActive();
							break;
						case 3:
							PopUpParts->Add(LocalizeParts->Get(120), y_UI(720), y_UI(840),
											[&](int xmin, int ymin, int xmax, int, bool) {
												m_CreditControl.Draw(xmin, ymin, xmax);
											},
											[&]() {},
												[&]() {},
												true
												);
							break;
						default:
							break;
					}
					SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}
			// 
			ButtonParts->Update();
			// 
			GameFadeIn = std::max(GameFadeIn - 1.f / DrawParts->GetFps() / 0.5f, 0.f);
			if (GameStart != 0.f) { GameStart += 1.f / DrawParts->GetFps() / 0.5f; }
			return (GameStart <= 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			// auto* BGM = BGMPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			// BGM->Get(0).Stop();
			// 
			m_CreditControl.Dispose();
			// 
			ButtonParts->Dispose();
			// 
			m_TitleImage.Dispose();
			// セーブ
			SaveDataParts->Save();
			// 次シーン決定
			SetNextSelect(ButtonParts->GetSelect());
		}
		// 
		void			TitleScene::MainDraw_Sub(void) noexcept {
			// 背景
			DrawBox(0, 0, y_r(1920), y_r(1080), Gray75, TRUE);
			// 描画
		}
		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* Fonts = FontPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// 
			m_TitleImage.DrawExtendGraph(y_UI(64), y_UI(64), y_UI(64 + 369), y_UI(64 + 207), true);
			// 
			Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_UI(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
																 y_UI(64 + 369), y_UI(64 + 207), White, Black, "Ver 1.0.0");
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != -1) && !PopUpParts->IsActivePop()) {
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_UI(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																	 y_UI(32), y_UI(1080 - 32 - 32), White, Black, LocalizeParts->Get(9020 + ButtonParts->GetSelect()));
			}
			// 
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * std::clamp(GameFadeIn, 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, y_r(1920), y_r(1080), Black, TRUE);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * std::clamp(GameStart, 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, y_r(1920), y_r(1080), White, TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
