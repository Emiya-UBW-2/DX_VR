#include	"TitleScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

#include"../Header.hpp"

#define LineHeight	y_r(18)

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			select = -1;
			m_MouseSelMode = false;
			GameFadeIn = 0.f;
			for (auto& y : GameStart) {
				y = 0.f;
			}
			for (auto& y : SelYadd) {
				y = 0.f;
			}
			m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
			m_SettingImage = GraphHandle::Load("data/UI/setting.png");
			m_CreditImage = GraphHandle::Load("data/UI/credit.png");

			int mdata = FileRead_open("data/Credit.txt", FALSE);
			m_CreditCoulm = 0;
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				FileRead_gets(m_CreditStr.at(m_CreditCoulm), CharMax, mdata);
				m_CreditCoulm++;
			}
			FileRead_close(mdata);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			select = -1;
			GameFadeIn = 0.f;
			for (auto& y : GameStart) {
				y = 0.f;
			}
			for (auto& y : SelYadd) {
				y = 0.f;
			}
			m_SelectBackImage.Dispose();
			m_SettingImage.Dispose();
			m_CreditImage.Dispose();
		}

		//
		bool			TitleScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			int preselect = select;
			GameFadeIn = std::max(GameFadeIn - 1.f / FPS, 0.f);

			Easing(&m_ActivePer, m_CreditActive ? 1.f : 0.f, 0.5f, EasingType::OutExpo);

			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					if (m_CreditActive) {
						KeyGuide->AddGuide(PADS::RELOAD, "戻る");
					}
					else {
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_A, "");
						KeyGuide->AddGuide(PADS::MOVE_D, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, "選択");
						KeyGuide->AddGuide(PADS::INTERACT, "決定");
					}
				}
			);
			if (!OptionWindowClass::Instance()->IsActive() && !m_CreditActive) {
				if (GetGameStartMax() == 0.f) {
					if (Pad->GetKey(PADS::MOVE_W).trigger() || Pad->GetKey(PADS::MOVE_A).trigger()) {
						if (select != -1) {
							--select;
							if (select < 0) { select = (int)SelYadd.size() - 1; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger() || Pad->GetKey(PADS::MOVE_D).trigger()) {
						if (select != -1) {
							++select;
							if (select > (int)SelYadd.size() - 1) { select = 0; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}

					auto* Fonts = FontPool::Instance();
					int xp1, yp1;
					if (m_MouseSelMode) {
						select = -1;
					}
					//
					{
						int fontsize = y_r(64);
						xp1 = y_r(1920 / 2);
						yp1 = y_r(1080 - 96);
						int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, SelectName[0]);
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - width / 2, yp1 - fontsize, xp1 + width / 2, yp1)) {
							m_MouseSelMode = true;
							select = 0;
						}
					}
					//
					{
						int fontsize = y_r(48);
						xp1 = y_r(1920 - 64 - (int)(GameStart[1] * 100.f));
						yp1 = y_r(1080 - 84 - 64 * 1);
						int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, SelectName[1]);
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - width, yp1 - fontsize, xp1, yp1)) {
							m_MouseSelMode = true;
							select = 1;
						}
					}
					//
					{
						int fontsize = y_r(48);
						xp1 = y_r(1920 - 64 - (int)(GameStart[2] * 100.f));
						yp1 = y_r(1080 - 84 - 64 * 0);
						int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, SelectName[2]);
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - width, yp1 - fontsize, xp1, yp1)) {
							m_MouseSelMode = true;
							select = 2;
						}
					}
					//
					{
						xp1 = y_r(1920 - 96 - 64);
						yp1 = y_r(64);
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - y_r(32), yp1 - y_r(32), xp1 + y_r(32), yp1 + y_r(32))) {
							m_MouseSelMode = true;
							select = 3;
						}
					}
					//
					{
						xp1 = y_r(1920 - 64);
						yp1 = y_r(64);
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - y_r(32), yp1 - y_r(32), xp1 + y_r(32), yp1 + y_r(32))) {
							m_MouseSelMode = true;
							select = 4;
						}
					}
					if ((GameFadeIn == 0.f) && (m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger())) {
						switch (select) {
							case 0:
							case 1:
							case 2:
								GameStart[select] += 0.0001f;
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								break;
							case 3:
								OptionWindowClass::Instance()->SetActive();
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								break;
							case 4:
								Pad->SetGuideUpdate();
								m_CreditActive = true;
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
								break;
							default:
								break;
						}
					}
				}
				if (preselect != select && select != -1) {
					SelYadd[select] = 10.f;
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}

			if (m_CreditActive) {
				int xp1, yp1;
				int WinSizeX = y_r(720);
				int WinSizeY = y_r(720);

				xp1 = y_r(960) + WinSizeX / 2 - y_r(140);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4 + LineHeight / 2;
				bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2);
				if (Pad->GetKey(PADS::RELOAD).trigger() || (MouseOver && Pad->GetMouseClick().trigger())) {
					SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_CreditActive = false;
					Pad->SetGuideUpdate();
				}
			}


			for (auto& y : SelYadd) {
				Easing(&y, 0.f, 0.93f, EasingType::OutExpo);
			}
			for (auto& y : GameStart) {
				if (y != 0.f) {
					y += 1.f / FPS / 0.5f;
				}
			}
			if (GetGameStartMax() >= 1.f) {
				return false;
			}
			return true;
		}

		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto* Pad = PadControl::Instance();

			auto White = GetColor(255, 255, 255);
			auto WhiteSel = GetColor(216, 255, 216);
			auto Red = GetColor(255, 0, 0);
			auto Gray25 = GetColor(216, 216, 216);
			auto Gray50 = GetColor(128, 128, 128);
			auto Gray = GetColor(64, 64, 64);
			auto Black = GetColor(0, 0, 0);
			int xp1, yp1;
			//
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);
			//
			{
				int fontsize = y_r(96);
				xp1 = y_r(64);
				yp1 = y_r(64);
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "JW-ProtoType");
			}
			//
			{
				int fontsize = y_r(64);
				xp1 = y_r(1920 / 2);
				yp1 = y_r(1080 - 96 + (int)SelYadd[0]);
				bool IsSelect = (select == 0);
				if (IsSelect) {
					int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, SelectName[0]);
					float per = std::clamp(SelYadd[0] / 5.f, 0.f, 1.f);
					float per2 = 1.f - std::clamp(SelYadd[0] / 10.f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
					m_SelectBackImage.DrawExtendGraph(
						xp1 - (int)((float)(width / 2 + y_r(300))*per2), yp1 - y_r(12) - (int)((float)(fontsize / 6) * per),
						xp1 + (int)((float)(width / 2 + y_r(300))*per2), yp1 - y_r(12) + (int)((float)(fontsize / 6) * per),
						true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				auto rad = deg2rad(90) * std::clamp(GameStart[0] * 2.f, 0.f, 1.f);
				auto Color = GetColor((int)(255.f * sin(rad)), (int)(255.f * cos(rad)), 0);
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? Color : ((select == -1) ? Gray25 : Gray), Black, SelectName[0]);
			}
			//
			{
				int fontsize = y_r(48);
				xp1 = y_r(1920 - 64 - (int)(GameStart[1] * 100.f));
				yp1 = y_r(1080 - 84 - 64 * 1 + (int)SelYadd[1]);
				bool IsSelect = (select == 1);
				if (IsSelect) {
					int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, SelectName[1]);
					float per = std::clamp(SelYadd[1] / 5.f, 0.f, 1.f);
					float per2 = 1.f - std::clamp(SelYadd[1] / 10.f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
					m_SelectBackImage.DrawExtendGraph(
						xp1 - width / 2 - (int)((float)(width / 2 + y_r(300))*per2), yp1 - y_r(12) - (int)((float)(fontsize / 6) * per),
						xp1 - width / 2 + (int)((float)(width / 2 + y_r(300))*per2), yp1 - y_r(12) + (int)((float)(fontsize / 6) * per),
						true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? WhiteSel : ((select == -1) ? Gray25 : Gray), Black, SelectName[1]);
			}
			//
			{
				int fontsize = y_r(48);
				xp1 = y_r(1920 - 64 - (int)(GameStart[2] * 100.f));
				yp1 = y_r(1080 - 84 - 64 * 0 + (int)SelYadd[2]);
				bool IsSelect = (select == 2);
				if (IsSelect) {
					int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, SelectName[2]);
					float per = std::clamp(SelYadd[2] / 5.f, 0.f, 1.f);
					float per2 = 1.f - std::clamp(SelYadd[2] / 10.f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
					m_SelectBackImage.DrawExtendGraph(
						xp1 - width / 2 - (int)((float)(width / 2 + y_r(300))*per2), yp1 - y_r(12) - (int)((float)(fontsize / 6) * per),
						xp1 - width / 2 + (int)((float)(width / 2 + y_r(300))*per2), yp1 - y_r(12) + (int)((float)(fontsize / 6) * per),
						true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? WhiteSel : ((select == -1) ? Gray25 : Gray), Black, SelectName[2]);
			}
			//
			{
				xp1 = y_r(1920 - 96 - 64);
				yp1 = y_r(64);
				bool IsSelect = (select == 3);
				if (IsSelect) {
					float per = std::clamp(SelYadd[3] / 5.f, 0.f, 1.f);
					float per2 = 1.f - std::clamp(SelYadd[3] / 10.f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
					m_SelectBackImage.DrawExtendGraph(
						xp1 - (int)((float)y_r(64)*per2), yp1 - (int)((float)y_r(64)*per2),
						xp1 + (int)((float)y_r(64)*per2), yp1 + (int)((float)y_r(64)*per2),
						true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				if (IsSelect) {
					SetDrawBright(216, 255, 216);
				}
				else if (select == -1) {
					SetDrawBright(216, 216, 216);
				}
				else {
					SetDrawBright(128, 128, 128);
				}
				m_SettingImage.DrawRotaGraph(xp1, yp1, (float)y_r(100) / 100.f*(1.f + SelYadd[3] / 50.f), 0.f, true);
				SetDrawBright(255, 255, 255);
			}
			//
			{
				xp1 = y_r(1920 - 64);
				yp1 = y_r(64);
				bool IsSelect = (select == 4);
				if (IsSelect) {
					float per = std::clamp(SelYadd[4] / 5.f, 0.f, 1.f);
					float per2 = 1.f - std::clamp(SelYadd[4] / 10.f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
					m_SelectBackImage.DrawExtendGraph(
						xp1 - (int)((float)y_r(64)*per2), yp1 - (int)((float)y_r(64)*per2),
						xp1 + (int)((float)y_r(64)*per2), yp1 + (int)((float)y_r(64)*per2),
						true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				if (IsSelect) {
					SetDrawBright(216, 255, 216);
				}
				else if (select == -1) {
					SetDrawBright(216, 216, 216);
				}
				else {
					SetDrawBright(128, 128, 128);
				}
				m_CreditImage.DrawRotaGraph(xp1, yp1, (float)y_r(100) / 100.f*(1.f + SelYadd[4] / 50.f), 0.f, true);
				SetDrawBright(255, 255, 255);
			}
			//
			if ((select != -1) && (!OptionWindowClass::Instance()->IsActive() && !m_CreditActive)) {
				xp1 = y_r(32);
				yp1 = y_r(1080 - 32 - 32);
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Black, SelectInfoName[select]);
			}
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp((GameFadeIn), 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(GetGameStartMax(), 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//クレジット
			{
				int WinSizeX = y_r(720);
				int WinSizeY = y_r(720);
				//背景
				if (m_ActivePer > 0.f) {
					auto per = std::clamp(m_ActivePer * 0.3f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
					DrawBox(y_r(960) - WinSizeX / 2, y_r(540) - WinSizeY / 2, y_r(960) + WinSizeX / 2, y_r(540) + WinSizeY / 2, Gray50, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				if (m_ActivePer > 0.9f) {
					//タイトル
					{
						xp1 = y_r(960) - WinSizeX / 2 + y_r(32);
						yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4;

						int xpos = xp1 + y_r(6);
						int ypos = yp1 + (yp1 + LineHeight * 2 - yp1) / 2;

						Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(LineHeight * 2, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																			   xpos, ypos, White, Black, "Credit");
					}
					//
					{
						xp1 = y_r(960) + WinSizeX / 2 - y_r(140);
						yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4 + LineHeight / 2;

						bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2);
						int xpos = xp1 + (xp1 + y_r(108) - xp1) / 2;
						int ypos = yp1 + (yp1 + LineHeight * 2 - yp1) / 2;

						DrawBox(xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2, MouseOver ? White : Red, true);
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(std::min(LineHeight, yp1 + LineHeight * 2 - yp1), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																			   xpos, ypos, White, Black, "閉じる");
					}
					//背景
					{
						xp1 = y_r(960) - WinSizeX / 2;
						yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3;

						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*0.3), 0, 255));
						DrawBox(xp1 + y_r(24), yp1, xp1 + WinSizeX - y_r(24), y_r(540) + WinSizeY / 2 - LineHeight, Gray50, true);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					//タイトル
					{
						xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
						yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3 + LineHeight;
						int Height = y_r(12);
						for (int i = 0;i < m_CreditCoulm;i++) {
							int xpos = xp1 + y_r(6);
							int ypos = yp1 + (yp1 + Height - yp1) / 2;
							Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																				   xpos, ypos, White, Black, m_CreditStr.at(i));

							yp1 += Height;
						}
					}
					//
				}
			}
		}
	};
};
