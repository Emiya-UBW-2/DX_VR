#include	"TitleScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

#include"../Header.hpp"

#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Load_Sub(void) noexcept {
			//ロード
			if (m_IsFirstLoad) {
				m_IsFirstLoad = false;
				auto* PlayerMngr = PlayerManager::Instance();
				//BG
				GunAnimManager::Instance()->Load("data/CharaAnime/");
				//
				PlayerMngr->Init(1);
			}
		}
		void			TitleScene::Set_Sub(void) noexcept {
			select = 0;
			m_MouseSelMode = false;
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			m_HardModeActive = (SaveDataParts->GetParam("UnlockHardMode") == 1);
			GameFadeIn = 1.f;
			GameStart = 0.f;

			m_TitleImage = GraphHandle::Load("data/UI/Title.png");
			m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
			for (auto& y : ButtonSel) {
				y.LoadCommon(&m_SelectBackImage);
			}

			Vector3DX LightVec = Vector3DX::vget(0.f, -1.f, 0.5f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-10.f, -3.f, -10.f)*Scale_Rate, Vector3DX::vget(10.f, 0.5f, 10.f)*Scale_Rate, 0);
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-10.f, -3.f, -10.f)*Scale_Rate, Vector3DX::vget(10.f, 0.f, 10.f)*Scale_Rate, 1);
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-100.f, 0.f, -100.f)*Scale_Rate, Vector3DX::vget(100.f, 1.f, 100.f)*Scale_Rate, 2);
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(LightVec.get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.1f, 0.1f, 0.1f, 0.f));
			SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));

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
				auto ALL = getparams::Getstr(mdata);
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					sprintfDx(m_CreditStr.at(m_CreditCoulm).first, LEFT.c_str());
					sprintfDx(m_CreditStr.at(m_CreditCoulm).second, RIGHT.c_str());
				}
				else {
					sprintfDx(m_CreditStr.at(m_CreditCoulm).first, ALL.c_str());
					sprintfDx(m_CreditStr.at(m_CreditCoulm).second, "");
				}
				m_CreditCoulm++;
			}
			FileRead_close(mdata);

			m_PopUpDrawClass.Set(LocalizePool::Instance()->Get(120), y_r(720), y_r(840), [&](int WinSizeX, int WinSizeY, bool) {
				auto* Fonts = FontPool::Instance();

				int xp1, yp1;

				xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
				yp1 = y_r(540) - WinSizeY / 2 + y_r(18) * 3 + y_r(18);
				int Height = y_r(12);
				for (int i = 0;i < m_CreditCoulm;i++) {
					int xpos = xp1 + y_r(6);
					int ypos = yp1 + (yp1 + Height - yp1) / 2;
					Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																		   xpos, ypos, White, Black, m_CreditStr.at(i).first);

					xpos = xp1 + WinSizeX - y_r(48 * 2) - y_r(6);
					Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
																		   xpos, ypos, White, Black, m_CreditStr.at(i).second);
					yp1 += Height;
				}
								 });
			//
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();

			BattleResourceMngr->LoadChara("Suit", (PlayerID)(0));
			//
			GunsModify::LoadSlots("Save/gundata.svf");
			{
				BattleResourceMngr->LoadGun("G17Gen3", (PlayerID)(0), 0);
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer((PlayerID)(0)).GetChara();
				auto& g = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer((PlayerID)(0)).GetGun(0);
				c->SetGunPtr(0, g);
				GunsModify::CreateSelData(c->GetGunPtr(0), true);
				c->GetGunPtr(0)->Init_Gun();
			}
			{
				BattleResourceMngr->LoadGun(ULT_GUNName[(int)GunsModify::GetULTSelect()], (PlayerID)(0), 1);
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer((PlayerID)(0)).GetChara();
				auto& g = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer((PlayerID)(0)).GetGun(1);
				c->SetGunPtr(1, g);
				GunsModify::CreateSelData(c->GetGunPtr(1), false);
				c->GetGunPtr(1)->Init_Gun();
			}
			{
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer((PlayerID)(0)).GetChara();
				c->ValueSet((PlayerID)(0), false, CharaTypeID::Team);

				Vector3DX pos_t = Vector3DX::vget(-0.6f, 0.0f, 0.2f)*Scale_Rate;
				float rad_t = 0.f;
				c->MovePoint(deg2rad(0.f), rad_t, pos_t, 0);
				c->Heal(100, true);
			}

			auto* BGM = BGMPool::Instance();
			if (!BGM->Get(0).Check()) {
				BGM->Get(0).Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			BGM->Get(0).SetVol_Local(255);

			m_Yrad = deg2rad(45);
			m_Xrad = 0.f;
			m_Yrad_R = m_Yrad;
			m_Xrad_R = m_Xrad;
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* Pad = PadControl::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();

			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
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

						//
						if (select == 0) {
							Chara->SetAimOrADS();
							Chara->SelectGun(0);
						}
						else if (select == 1 || select == 2) {
							Chara->SetADS();
							//Chara->GetGunPtrNow()->ChangeSightSel();
						}
						else if ((m_HardModeActive && (select == 3))) {
							Chara->SetAimOrADS();
							Chara->SelectGun(1);
						}
						else {
							if (Chara->IsGun0Select()) {
								Chara->SetAimOrADS();
							}
							else {
								Chara->SetReady();
							}
						}
					}
				}
			}
			if (Chara->IsGun0Select()) {
				Chara->SetAimOrADS();
			}
			//
			m_PopUpDrawClass.Update(m_CreditActive);
			if (m_PopUpDrawClass.IsActive()) {
				Pad->ChangeGuide(
					[&]() {
						auto* KeyGuide = PadControl::Instance();
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));
					}
				);
			}
			//
			if (select == 1 || select == 2) {
				Chara->SetADS();
			}
			//
			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();

			Easing(&m_Xrad, deg2rad(0.f + GetRandf(10.f)), 0.95f, EasingType::OutExpo);
			Easing(&m_Yrad, deg2rad(45.f + GetRandf(10.f)), 0.95f, EasingType::OutExpo);
			Easing(&m_Xrad_R, m_Xrad, 0.95f, EasingType::OutExpo);
			Easing(&m_Yrad_R, m_Yrad, 0.95f, EasingType::OutExpo);

			Vector3DX Pos = Vector3DX::vget(0.f, 1.25f, 0.0f)*Scale_Rate;
			DrawParts->SetMainCamera().SetCamPos(
				Pos + Matrix4x4DX::Vtrans(Vector3DX::forward()*(3.f*Scale_Rate), Matrix4x4DX::RotAxis(Vector3DX::right(), m_Xrad_R)*Matrix4x4DX::RotAxis(Vector3DX::up(), m_Yrad_R + DX_PI_F)),
				Pos,
				Vector3DX::vget(0.f, 1.f, 0.f));
			float far_t = 20.f*Scale_Rate;
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(45), 0.5f*Scale_Rate, far_t);
			PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t / 2, 0.5f*Scale_Rate, far_t);

			//
			for (auto& y : ButtonSel) {
				y.Update();
			}
			GameFadeIn = std::max(GameFadeIn - 1.f / FPS / 0.5f, 0.f);
			if (GameStart != 0.f) { GameStart += 1.f / FPS / 0.5f; }
			return  (GameStart < 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			{
				auto* Ptr = &PlayerMngr->GetPlayer((PlayerID)(0)).GetChara();
				ObjMngr->DelObj(Ptr);
			}
			{
				auto* Ptr = &PlayerMngr->GetPlayer((PlayerID)(0)).GetGun(0);
				ObjMngr->DelObj(Ptr);
			}
			PlayerMngr->GetPlayer(0).Dispose();
			GunsModify::DisposeSlots();
			//
			m_SelectBackImage.Dispose();
			for (auto& y : ButtonSel) {
				y.Dispose();
			}
			//
			m_TitleImage.Dispose();
		}
		void			TitleScene::BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Gray65, TRUE);
		}

		void			TitleScene::ShadowDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject();
		}
		void			TitleScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject();
		}
		//
		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();

			//
			m_TitleImage.DrawExtendGraph(y_r(64), y_r(64), y_r(64+369), y_r(64 + 207), true);
			//
			{
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18),
																	  FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
																	  y_r(64 + 369), y_r(64 + 207), White, Black, "Ver 1.0.4");
			}
			//
			for (auto& y : ButtonSel) {
				y.Draw();
			}
			//
			if ((select != -1) && (!OptionWindowClass::Instance()->IsActive() && !m_PopUpDrawClass.IsActive())) {
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18),
																	  FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																	  y_r(32), y_r(1080 - 32 - 32), White, Black, LocalizePool::Instance()->Get(9020 + select));
			}
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(GameFadeIn, 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(GameStart, 0.f, 1.f)), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, White, TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//クレジット
			m_PopUpDrawClass.Draw();
		}
		//使い回しオブジェ系
		void			TitleScene::Dispose_Load_Sub(void) noexcept {
			if (!m_IsFirstLoad) {
				m_IsFirstLoad = true;
				auto* PlayerMngr = PlayerManager::Instance();
				auto* ObjMngr = ObjectManager::Instance();

				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
			}
		}
		//
	};
};
