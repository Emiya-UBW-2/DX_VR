#include	"CustomScene.hpp"

#include "../sub.hpp"

#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Object/MovieObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CustomScene::Load_Sub(void) noexcept {
			//ロード
				auto* PlayerMngr = PlayerManager::Instance();
				PlayerMngr->Init(1 + (int)ULT_GUN::Max);
		}
		void			CustomScene::Set_Sub(void) noexcept {
			//
			bselect = 0;
			m_MouseSelMode = false;
			m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
			for (auto& y : ButtonSel) {
				y.LoadCommon(&m_SelectBackImage);
			}
			ButtonSel.at(0).Load_Icon("data/UI/Customs.png", true);
			ButtonSel.at(0).Set((1920 - 96 * 2 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(1).Load_Icon("data/UI/WeaponChange.png", true);
			ButtonSel.at(1).Set((1920 - 96 * 1 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(2).Load_Icon("data/UI/FreeLook.png", true);
			ButtonSel.at(2).Set((1920 - 96 * 0 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);

			ButtonSel.at(3).Load_Icon("data/UI/Left.png", true);
			ButtonSel.at(3).Set((960 - 100), (920), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(4).Load_Icon("data/UI/Right.png", true);
			ButtonSel.at(4).Set((960 + 100), (920), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);

			ButtonSel.at(5).Load_Icon("data/UI/Left.png", true);
			ButtonSel.at(5).Set((960 - 400 - 48), (540 - 270 + 48), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(6).Load_Icon("data/UI/Right.png", true);
			ButtonSel.at(6).Set((960 - 400 + 48), (540 - 270 + 48), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(7).Load_Icon("data/UI/Reset.png", true);
			ButtonSel.at(7).Set((1920 - 96 * 2 - 64), (64 + 64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			//
			auto* DrawParts = DXDraw::Instance();

			Vector3DX LightVec = Vector3DX::vget(-0.4f, -0.5f, 0.1f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(LightVec.get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.01f, 0.01f, 0.01f, 0.f));
			SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.f));
			//
			select = 0;
			//
			auto* ObjMngr = ObjectManager::Instance();
			{
				auto Obj = std::make_shared<MovieObjClass>();
				ObjMngr->AddObject(Obj);
				ObjMngr->LoadModel(Obj, nullptr, "data/model/table/");
				Obj->Init();
			}
			m_Yrad = deg2rad(-45);
			m_Xrad = 0.f;
			m_Yrad_R = m_Yrad;
			m_Xrad_R = m_Xrad;
			m_Range = 10.f;
			m_SelAlpha = 0.f;
			//
			m_IsEnd = false;
			m_Alpha = 1.f;

			m_LookSel = LookSelect::ModSet;
			for (auto& y : ButtonSel) {
				if ((int)(&y - &ButtonSel.front()) < 3) {
					if ((int)m_LookSel == (int)(&y - &ButtonSel.front())) {
						y.SetReady();
					}
					else {
						y.SetNone();
					}
				}
				else {
					y.SetReady();
				}
			}

		}
		bool			CustomScene::Update_Sub(void) noexcept {
			if (GetIsFirstLoop()) {
				m_Yrad = deg2rad(-45);
				m_Range = 10.f;
				m_SelAlpha = 2.f;
			}
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();

			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					if (m_LookSel == LookSelect::FreeLook) {
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9930));
					}
					else {
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_A, "");
						KeyGuide->AddGuide(PADS::MOVE_D, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9931));
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9932));
						KeyGuide->AddGuide(PADS::MELEE, LocalizePool::Instance()->Get(9933));
					}
				}
			);

			switch (m_LookSel) {
				case LookSelect::ModSet:
					break;
				case LookSelect::ULTSet:
					break;
				case LookSelect::FreeLook:
					break;
				default:
					break;
			}

			for (auto& y : SelMoveClass) {
				Easing(&y.Xadd, 0.f, 0.9f, EasingType::OutExpo);
				Easing(&y.Yadd, 0.f, 0.95f, EasingType::OutExpo);
			}
			m_SelAlpha = std::max(m_SelAlpha - 1.f / DrawParts->GetFps(), 0.f);

			int preselect = bselect;
			bool preMouseSel = m_MouseSelMode;
			auto preLookSel = m_LookSel;
			/*
			if (Pad->GetKey(PADS::MOVE_W).trigger()) {
				if (bselect != -1) {
					--bselect;
					if (bselect < 0) { bselect = (int)ButtonSel.size() - 1; }
				}
				else {
					bselect = 0;
				}
				m_MouseSelMode = false;
			}
			if (Pad->GetKey(PADS::MOVE_S).trigger()) {
				if (bselect != -1) {
					++bselect;
					if (bselect > (int)ButtonSel.size() - 1) { bselect = 0; }
				}
				else {
					bselect = 0;
				}
				m_MouseSelMode = false;
			}
			//*/

			if (m_MouseSelMode) {
				bselect = -1;
			}
			//
			for (auto& y : ButtonSel) {
				if (y.GetInto()) {
					m_MouseSelMode = true;
					bselect = (int)(&y - &ButtonSel.front());
				}
			}

			if (Pad->GetKey(PADS::RELOAD).trigger()) {
				if (m_LookSel == LookSelect::FreeLook) {
					m_LookSel = LookSelect::ModSet;
					m_Yrad = deg2rad(-45);
					bselect = 0;
					Pad->SetGuideUpdate();
				}
				else {
					m_IsEnd = true;
				}
				SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}

			if ((bselect != -1) && (m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger())) {
				switch (bselect) {
					case 0:
						m_LookSel = LookSelect::ModSet;
						m_Yrad = deg2rad(-45);
						break;
					case 1:
						m_LookSel = LookSelect::ULTSet;
						m_Yrad = deg2rad(15);
						break;
					case 2:
						m_LookSel = LookSelect::FreeLook;
						bselect = -1;
						Pad->SetGuideUpdate();
						break;
					case 3:
						switch (m_LookSel) {
							case LookSelect::ModSet:
								break;
							case LookSelect::ULTSet:
								break;
							default:
								break;
						}
						break;
					case 4:
						switch (m_LookSel) {
							case LookSelect::ModSet:
								break;
							case LookSelect::ULTSet:
								break;
							default:
								break;
						}
						break;
					case 5:
						break;
					case 6:
						break;
					case 7:
						break;
					default:
						break;
				}
				m_SelAlpha = 2.f;
				SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}

			if (preselect != bselect || preMouseSel != m_MouseSelMode || preLookSel != m_LookSel) {
				for (auto& y : ButtonSel) {
					if ((int)(&y - &ButtonSel.front()) < 3) {
						if ((int)m_LookSel == (int)(&y - &ButtonSel.front())) {
							y.SetReady();
						}
						else {
							y.SetNone();
						}
					}
					else {
						y.SetReady();
					}
				}
				if (m_LookSel != LookSelect::FreeLook) {
					if (bselect != -1) {
						ButtonSel.at(bselect).SetFocus();
						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}

			for (auto& y : ButtonSel) {
				y.Update();
			}
			for (auto& y : ButtonAlpha) {
				Easing(&y, ((int)m_LookSel == (int)(&y - &ButtonAlpha.front())) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
			}
			if (m_LookSel == LookSelect::FreeLook) {
				m_Yrad += Pad->GetLS_X() / 1000.f;
				m_Xrad += Pad->GetLS_Y() / 1000.f;
			}
			m_Yrad = std::clamp(m_Yrad, deg2rad(-100), deg2rad(100));
			m_Xrad = std::clamp(m_Xrad, deg2rad(-20), deg2rad(-20));
			Easing(&m_Xrad_R, m_Xrad, 0.95f, EasingType::OutExpo);
			Easing(&m_Yrad_R, m_Yrad, 0.95f, EasingType::OutExpo);
			Easing(&m_Range, (m_LookSel != LookSelect::ModSet) ? 2.f : 1.f, 0.95f, EasingType::OutExpo);
			Pad->SetMouseMoveEnable(m_LookSel == LookSelect::FreeLook);

			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();

			Vector3DX Pos = Vector3DX::vget(0.f, 1.f, 0.f)*Scale_Rate;
			DrawParts->SetMainCamera().SetCamPos(
				Pos + Matrix4x4DX::Vtrans(Vector3DX::forward()*(m_Range*Scale_Rate), Matrix4x4DX::RotAxis(Vector3DX::right(), m_Xrad_R)*Matrix4x4DX::RotAxis(Vector3DX::up(), m_Yrad_R + DX_PI_F)),
				Pos,
				Vector3DX::vget(0.f, 1.f, 0.f));
			float far_t = 20.f*Scale_Rate;
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(45), 0.5f*Scale_Rate, far_t);
			PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t / 2, 0.5f*Scale_Rate, far_t);

			m_Alpha = std::clamp(m_Alpha + (m_IsEnd ? 1.f : -1.f) / 60, 0.f, 1.f);//一旦DrawParts->GetFps()使わない
			if (m_IsEnd && m_Alpha == 1.f) {
				return false;
			}
			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			for (int i = 0;i < (int)ULT_GUN::Max;i++) {
				ObjMngr->DelObj(&PlayerMngr->GetPlayer(1 + i).GetGun(0));
			}
			ObjMngr->DelObj(&PlayerMngr->GetPlayer(0).GetGun(0));
			PlayerMngr->GetPlayer(0).Dispose();
			for (int i = 0;i < (int)ULT_GUN::Max;i++) {
				PlayerMngr->GetPlayer(1 + i).Dispose();
			}
			//
			m_SelectBackImage.Dispose();
			for (auto& y : ButtonSel) {
				y.Dispose();
			}
			auto* SaveDataParts = SaveDataClass::Instance();
			SaveDataParts->Save();//セーブ
		}
		void			CustomScene::ShadowDraw_Sub(void) noexcept {
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			CustomScene::MainDraw_Sub(void) noexcept {
			ObjectManager::Instance()->Draw();
		}

		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
			auto* Fonts = FontPool::Instance();
			//
			Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(96),
																FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																y_UI(64), y_UI(64), White, Black, "Customize");
			//
			for (auto& y : ButtonSel) {
				if (m_LookSel == LookSelect::FreeLook) {
					if ((int)(&y - &ButtonSel.front()) >= 3) {
						break;
					}
				}
				if (m_LookSel != LookSelect::ModSet) {
					if ((int)(&y - &ButtonSel.front()) >= 5) {
						break;
					}
				}
				y.Draw();
			}
			//
			if (bselect != -1) {
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_UI(18),
																	  FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																	  y_UI(32), y_UI(1080 - 32 - 32), White, Black, LocalizePool::Instance()->Get(9010 + bselect));
			}
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*m_Alpha), 0, 255));

				DrawBox(0, 0, y_r(1920), y_r(1080), Black, TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}

		//使い回しオブジェ系
		void			CustomScene::Dispose_Load_Sub(void) noexcept {
				PlayerManager::Instance()->Dispose();
				ObjectManager::Instance()->DeleteAll();
		}
		//
	};
};
