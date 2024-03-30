#include	"TitleScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

#include"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			select = 0;
			m_MouseSelMode = false;
			auto* SaveDataParts = SaveDataClass::Instance();
			m_HardModeActive = (SaveDataParts->GetParam("UnlockHardMode") == 1);
			GameFadeIn = 1.f;
			GameStart = 0.f;

			m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
			for (auto& y : ButtonSel) {
				y.LoadCommon(&m_SelectBackImage);
			}

			ButtonSel.at(0).Load_String(SelectName[0], y_r(64), true);
			ButtonSel.at(0).Set(y_r(1920 / 2), y_r(1080 - 96), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM);
			ButtonSel.at(1).Load_String(SelectName[1], y_r(48), true);
			ButtonSel.at(1).Set(y_r(1920 - 64), y_r(1080 - 84 - 64 * 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonSel.at(2).Load_String(SelectName[2], y_r(48), true);
			ButtonSel.at(2).Set(y_r(1920 - 64), y_r(1080 - 84 - 64 * 1), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);

			ButtonSel.at(3).Load_String(SelectName[3], y_r(48), m_HardModeActive);
			ButtonSel.at(3).Set(y_r(1920 - 64), y_r(1080 - 84 - 64 * 0), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);

			ButtonSel.at(4).Load_Icon("data/UI/setting.png", true);
			ButtonSel.at(4).Set(y_r(1920 - 96 - 64), y_r(64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(5).Load_Icon("data/UI/credit.png", true);
			ButtonSel.at(5).Set(y_r(1920 - 64), y_r(64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			//クレジット
			int mdata = FileRead_open("data/Credit.txt", FALSE);
			m_CreditCoulm = 0;
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				FileRead_gets(m_CreditStr.at(m_CreditCoulm), CharMax, mdata);
				m_CreditCoulm++;
			}
			FileRead_close(mdata);

			m_PopUpDrawClass.Set("Exit", y_r(720), y_r(840), [&](int WinSizeX, int WinSizeY, bool) {
				auto* Fonts = FontPool::Instance();

				auto White = GetColor(255, 255, 255);
				auto Black = GetColor(0, 0, 0);
				int xp1, yp1;

				xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
				yp1 = y_r(540) - WinSizeY / 2 + y_r(18) * 3 + y_r(18);
				int Height = y_r(12);
				for (int i = 0;i < m_CreditCoulm;i++) {
					int xpos = xp1 + y_r(6);
					int ypos = yp1 + (yp1 + Height - yp1) / 2;
					Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																		   xpos, ypos, White, Black, m_CreditStr.at(i));
					yp1 += Height;
				}
								 });

			auto* BGM = BGMPool::Instance();
			if (!BGM->Get(0).Check()) {
				BGM->Get(0).Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			BGM->Get(0).SetVol_Local(255);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			m_SelectBackImage.Dispose();
			for (auto& y : ButtonSel) {
				y.Dispose();
			}
		}

		//
		bool			TitleScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();

			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, "選択");
					KeyGuide->AddGuide(PADS::INTERACT, "決定");
				}
			);
			m_CreditActive = false;
			if (!OptionWindowClass::Instance()->IsActive() && !m_PopUpDrawClass.IsActive()) {
				if ((GameFadeIn == 0.f) && (GameStart == 0.f)) {
					int preselect = select;
					bool preMouseSel = m_MouseSelMode;
					if (Pad->GetKey(PADS::MOVE_W).trigger() || Pad->GetKey(PADS::MOVE_A).trigger()) {
						if (select != -1) {
							--select;
							if (select < 0) { select = (int)ButtonSel.size() - 1; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger() || Pad->GetKey(PADS::MOVE_D).trigger()) {
						if (select != -1) {
							++select;
							if (select > (int)ButtonSel.size() - 1) { select = 0; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}

					if (m_MouseSelMode) {
						select = -1;
					}
					//
					for (auto& y : ButtonSel) {
						if (y.GetInto()) {
							m_MouseSelMode = true;
							select = (int)(&y - &ButtonSel.front());
						}
					}
					if ((select != -1) && (m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger())) {
						switch (select) {
							case 0:
							case 1:
							case 2:
								GameStart += 0.0001f;
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								break;
							case 3:
								if (m_HardModeActive) {
									GameStart += 0.0001f;
									SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								}
								else {
									SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
								}
								break;
							case 4:
								OptionWindowClass::Instance()->SetActive();
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								break;
							case 5:
								m_CreditActive = true;
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								break;
							default:
								break;
						}
					}
					if (preselect != select || preMouseSel != m_MouseSelMode) {
						if (select != -1) {
							for (auto& y : ButtonSel) {
								y.SetNone();
							}
							ButtonSel.at(select).SetFocus();
							SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						else {
							for (auto& y : ButtonSel) {
								y.SetReady();
							}
						}
					}
				}
			}
			//
			m_PopUpDrawClass.Update(m_CreditActive);
			if (m_PopUpDrawClass.IsActive()) {
				Pad->ChangeGuide(
					[&]() {
						auto* KeyGuide = PadControl::Instance();
						KeyGuide->AddGuide(PADS::RELOAD, "戻る");
					}
				);
			}
			//
			for (auto& y : ButtonSel) {
				y.Update();
			}
			GameFadeIn = std::max(GameFadeIn - 1.f / FPS / 0.5f, 0.f);
			if (GameStart != 0.f) { GameStart += 1.f / FPS / 0.5f; }
			return  (GameStart < 1.f);
		}

		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();

			auto White = GetColor(255, 255, 255);
			auto Black = GetColor(0, 0, 0);
			//
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(8, 8, 8), TRUE);
			//
			Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(96),
																FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																y_r(64), y_r(64), White, Black, "Phantom of the Bunker");
			//
			for (auto& y : ButtonSel) {
				y.Draw();
			}
			//
			if ((select != -1) && (!OptionWindowClass::Instance()->IsActive() && !m_PopUpDrawClass.IsActive())) {
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18),
																	  FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																	  y_r(32), y_r(1080 - 32 - 32), White, Black, SelectInfoName[select]);
			}
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(GameFadeIn, 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(GameStart, 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//クレジット
			m_PopUpDrawClass.Draw();
		}
	};
};
