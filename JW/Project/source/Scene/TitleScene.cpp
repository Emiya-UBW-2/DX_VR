#include	"TitleScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			select = 0;
			GameFadeIn = 0.f;
			GameStart = 0.f;
			GameStart2 = 0.f;
			GameStart3 = 0.f;

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
				if (GameStart == 0.f && GameStart2 == 0.f) {
					if (Pad->GetKey(PADS::MOVE_W).trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
				for (int i = 0; i < 3; i++) {
					Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
				}

				if (GameFadeIn == 0.f) {
					switch (select) {
					case 0:
						if (Pad->GetKey(PADS::INTERACT).trigger()) {
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (!(GameStart == 0.f && !Pad->GetKey(PADS::INTERACT).trigger())) {
							GameStart += 1.f / FPS / 0.5f;
						}
						break;
					case 1:
						if (Pad->GetKey(PADS::INTERACT).trigger()) {
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (!(GameStart2 == 0.f && !Pad->GetKey(PADS::INTERACT).trigger())) {
							GameStart2 += 1.f / FPS / 0.5f;
						}
						break;
					case 2:
						if (Pad->GetKey(PADS::INTERACT).trigger()) {
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetKey(PADS::INTERACT).trigger()) {
							OptionWindowClass::Instance()->SetActive();
						}
						break;
					default:
						break;
					}
				}
			}

			if (GameStart >= 1.f || GameStart2 >= 1.f) {
				return false;
			}
			return true;
		}

		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto Red = GetColor(255, 0, 0);
			auto Red75 = GetColor(192, 0, 0);
			auto White = GetColor(255, 255, 255);
			auto Gray75 = GetColor(128, 128, 128);
			auto Gray = GetColor(64, 64, 64);
			auto Black = GetColor(0, 0, 0);
			int xp1, yp1;
			//
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);
			//
			xp1 = y_r(64 + 600);
			yp1 = y_r(64 + 48 * 2);
			DrawFetteString(xp1, yp1, 1.33f, false, "JW-ProtoType");
			//
			xp1 = y_r(1920 - 256 - 54 * 2 - (int)(GameStart*100.f));
			yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);
			{
				auto per = std::clamp((1.f - GameStart), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 9 / 8), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 0) ? Red75 : Gray75, Gray, "Custom");
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 6 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 0) ? Red : White, Gray, "Custom");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			xp1 = y_r(1920 - 256 - 54 * 1 - (int)(GameStart2*100.f));
			yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);
			{
				auto per = std::clamp((1.f - GameStart2), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				DrawFetteString(xp1, yp1, 1.f, (select == 1), "Go Defense");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			xp1 = y_r(1920 - 256);
			yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
			DrawFetteString(xp1, yp1, 1.f, (select == 2), "Option");
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
					Info = "戦闘を行います。(現状ではリタイア、ESCキーで終わる以外で特に目標はありません。無限に甦る敵兵士を倒し続けます)";
					break;
				case 2:
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
				auto per = std::clamp(std::max(GameStart, GameStart2), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
