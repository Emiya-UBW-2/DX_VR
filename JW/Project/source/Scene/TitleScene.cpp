#include	"TitleScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

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
			m_SettingImage = GraphHandle::Load("data/UI/setting.png");
			m_CreditImage = GraphHandle::Load("data/UI/credit.png");
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

			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, "選択");
					KeyGuide->AddGuide(PADS::INTERACT, "決定");
				}
			);
			if (!OptionWindowClass::Instance()->IsActive()) {
				if (GetGameStartMax() == 0.f) {
					if (Pad->GetKey(PADS::MOVE_W).trigger()) {
						if (select != -1) {
							--select;
							if (select < 0) { select = (int)SelYadd.size() - 1; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger()) {
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
						int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, "Go Defense");
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - width / 2, yp1 - fontsize, xp1 + width / 2, yp1)) {
							m_MouseSelMode = true;
							select = 0;
						}
					}
					//
					{
						int fontsize = y_r(64);
						xp1 = y_r(1920 - 320 - (int)(GameStart[1] * 100.f));
						yp1 = y_r(1080 - 108 - 108 * 2);
						int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, "Custom");
						if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - width, yp1 - fontsize, xp1, yp1)) {
							m_MouseSelMode = true;
							select = 1;
						}
					}
					//
					{
						int fontsize = y_r(64);
						xp1 = y_r(1920 - 320 - (int)(GameStart[2] * 100.f));
						yp1 = y_r(1080 - 108 - 108 * 1);
						int width = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, "Shoot Range");
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
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						switch (select) {
							case 0:
							case 1:
							case 2:
								GameStart[select] += 0.0001f;
								break;
							case 3:
								OptionWindowClass::Instance()->SetActive();
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
				for (auto& y : SelYadd) {
					Easing(&y, 0.f, 0.95f, EasingType::OutExpo);
				}
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

			auto White = GetColor(255, 255, 255);
			auto Gray25 = GetColor(216, 216, 216);
			auto Gray = GetColor(64, 64, 64);
			auto Black = GetColor(0, 0, 0);
			int xp1, yp1;
			//
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);
			//
			xp1 = y_r(64 + 600);
			yp1 = y_r(64 + 48 * 2);
			DrawFetteString(xp1, yp1, 1.33f, false, FontHandle::FontXCenter::RIGHT, "JW-ProtoType");

			//
			{
				int fontsize = y_r(64);
				xp1 = y_r(1920 / 2);
				yp1 = y_r(1080 - 96 + (int)SelYadd[0]);
				auto per = std::clamp((1.f - GameStart[0]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				bool IsSelect = (select == 0);
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? White : ((select == -1) ? Gray25 : Gray), Black, "Go Defense");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			{
				int fontsize = y_r(64);
				xp1 = y_r(1920 - 320 - (int)(GameStart[1] * 100.f));
				yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[1]);
				auto per = std::clamp((1.f - GameStart[1]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				bool IsSelect = (select == 1);
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? White : ((select == -1) ? Gray25 : Gray), Black, "Custom");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			{
				int fontsize = y_r(64);
				xp1 = y_r(1920 - 320 - (int)(GameStart[2] * 100.f));
				yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[2]);
				auto per = std::clamp((1.f - GameStart[2]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				bool IsSelect = (select == 2);
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(fontsize,
																	FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? White : ((select == -1) ? Gray25 : Gray), Black, "Shoot Range");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			{
				xp1 = y_r(1920 - 96 - 64);
				yp1 = y_r(64);
				bool IsSelect = (select == 3);
				if (IsSelect) {
					SetDrawBright(255, 255, 255);
				}
				else if ((select == -1)) {
					SetDrawBright(216, 216, 216);
				}
				else {
					SetDrawBright(128, 128, 128);
				}
				m_SettingImage.DrawRotaGraph(xp1, yp1, (float)y_r(100) / 100.f, 0.f, true);
				SetDrawBright(255, 255, 255);
			}
			//
			{
				xp1 = y_r(1920 - 64);
				yp1 = y_r(64);
				bool IsSelect = (select == 4);
				if (IsSelect) {
					SetDrawBright(255, 255, 255);
				}
				else if ((select == -1)) {
					SetDrawBright(216, 216, 216);
				}
				else {
					SetDrawBright(128, 128, 128);
				}
				m_CreditImage.DrawRotaGraph(xp1, yp1, (float)y_r(100) / 100.f, 0.f, true);
				SetDrawBright(255, 255, 255);
			}
			//
			{
				xp1 = y_r(32);
				yp1 = y_r(1080 - 32 - 32);

				std::string Info = "";
				switch (select) {
					case 0:
						Info = "戦闘を行います。(現状ではリタイア、ESCキーで終わる以外で特に目標はありません。無限に甦る敵兵士を倒し続けます)";
						break;
					case 1:
						Info = "使用する拳銃のカスタマイズをすることができます";
						break;
					case 2:
						Info = "射撃場で装備を試します";
						break;
					case 3:
						Info = "オプションを開きます";
						break;
					case 4:
						Info = "クレジットを開きます";
						break;
					default:
						break;
				}
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Black, Info);
			}
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp((GameFadeIn), 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(GetGameStartMax(), 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
