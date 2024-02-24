#include	"TitleScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			select = 0;
			GameFadeIn = 0.f;
			for (auto& y : GameStart) {
				y = 0.f;
			}
			for (auto& y : SelYadd) {
				y = 0.f;
			}
		}
		//
		bool			TitleScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();

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
				if (GameStart[0] == 0.f && GameStart[1] == 0.f && GameStart[2] == 0.f) {
					if (Pad->GetKey(PADS::MOVE_W).trigger()) {
						--select;
						if (select < 0) { select = 3; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger()) {
						++select;
						if (select > 3) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
				for (int i = 0; i < 3; i++) {
					Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
				}

				if (GameFadeIn == 0.f) {
					if (Pad->GetKey(PADS::INTERACT).trigger()) {
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					switch (select) {
						case 0:
							if ((GameStart[0] != 0.f) || Pad->GetKey(PADS::INTERACT).trigger()) {
								GameStart[0] += 1.f / FPS / 0.5f;
							}
							break;
						case 1:
							if ((GameStart[1] != 0.f) || Pad->GetKey(PADS::INTERACT).trigger()) {
								GameStart[1] += 1.f / FPS / 0.5f;
							}
							break;
						case 2:
							if ((GameStart[2] != 0.f) || Pad->GetKey(PADS::INTERACT).trigger()) {
								GameStart[2] += 1.f / FPS / 0.5f;
							}
							break;
						case 3:
							if (Pad->GetKey(PADS::INTERACT).trigger()) {
								OptionWindowClass::Instance()->SetActive();
							}
							break;
						default:
							break;
					}
				}
			}
			for (auto& y : GameStart) {
				if (y >= 1.f) {
					return false;
				}
			}
			return true;
		}

		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto White = GetColor(255, 255, 255);
			auto Black = GetColor(0, 0, 0);
			int xp1, yp1;
			//
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);
			//
			xp1 = y_r(64 + 600);
			yp1 = y_r(64 + 48 * 2);
			DrawFetteString(xp1, yp1, 1.33f, false, "JW-ProtoType");
			//
			xp1 = y_r(1920 - 256 - 54 * 3 - (int)(GameStart[0] * 100.f));
			yp1 = y_r(1080 - 108 - 108 * 3 + (int)SelYadd[0]);
			{
				auto per = std::clamp((1.f - GameStart[0]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				DrawFetteString(xp1, yp1, 1.f, (select == 0), "Custom");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			xp1 = y_r(1920 - 256 - 54 * 2 - (int)(GameStart[1] * 100.f));
			yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[1]);
			{
				auto per = std::clamp((1.f - GameStart[1]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				DrawFetteString(xp1, yp1, 1.f, (select == 1), "Shoot Range");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			xp1 = y_r(1920 - 256 - 54 * 1 - (int)(GameStart[2] * 100.f));
			yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[2]);
			{
				auto per = std::clamp((1.f - GameStart[2]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				DrawFetteString(xp1, yp1, 1.f, (select == 2), "Go Defense");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			xp1 = y_r(1920 - 256);
			yp1 = y_r(1080 - 108 + (int)SelYadd[3]);
			{
				auto per = std::clamp((1.f - GameStart[3]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				DrawFetteString(xp1, yp1, 1.f, (select == 3), "Option");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			{
				xp1 = y_r(32);
				yp1 = y_r(1080 - 32 - 32);

				std::string Info = "";
				switch (select) {
					case 0:
						Info = "使用する拳銃のカスタマイズをすることができます";
						break;
					case 1:
						Info = "射撃場で装備を試します";
						break;
					case 2:
						Info = "戦闘を行います。(現状ではリタイア、ESCキーで終わる以外で特に目標はありません。無限に甦る敵兵士を倒し続けます)";
						break;
					case 3:
						Info = "オプションを開きます";
						break;
					default:
						break;
				}
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Black, Info);
			}
			//
			{
				auto per = std::clamp((GameFadeIn), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			{
				auto per = std::clamp(std::max(std::max(GameStart[0], GameStart[1]), GameStart[2]), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
