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
			GunsModify::LoadSlots("Save/gundata.svf");
			//
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();

			{
				BattleResourceMngr->LoadGun("G17Gen3", (PlayerID)(0), 0);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(0).GetGun(0);
				GunsModify::CreateSelData(GunPtr, true);
				UpdateSlotMove();
				GunPtr->Init_Gun();
			}

			auto* SaveDataParts = SaveDataClass::Instance();
			int max = std::clamp(static_cast<int>(SaveDataParts->GetParam("ULT Unlock")), 1, (int)ULT_GUN::Max);
			for (int i = 0;i < max;i++) {
				BattleResourceMngr->LoadGun(ULT_GUNName[i], (PlayerID)(1 + i), 0);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(1 + i).GetGun(0);
				GunsModify::CreateSelData(GunPtr, false);
				GunPtr->Init_Gun();
			}
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

			for (auto& up : m_UltPer) {
				up = 0.f;
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
			auto* PlayerMngr = PlayerManager::Instance();

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
					if (Pad->GetKey(PADS::MOVE_W).trigger()) {
						ChangeModSel(-1);
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger()) {
						ChangeModSel(1);
					}
					if (Pad->GetKey(PADS::MOVE_A).trigger()) {
						ChangeMod(-1);
					}
					if (Pad->GetKey(PADS::MOVE_D).trigger()) {
						ChangeMod(1);
					}
					if (Pad->GetKey(PADS::MELEE).trigger()) {
						ResetMod();
					}
					break;
				case LookSelect::ULTSet:
					if (Pad->GetKey(PADS::MOVE_A).trigger()) {
						ChangeULT(1);
					}
					if (Pad->GetKey(PADS::MOVE_D).trigger()) {
						ChangeULT(-1);
					}
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

			m_UltMat[0] = Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(90)) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(0)) *
				Matrix4x4DX::Mtrans(Vector3DX::vget(1.1f, 0.95f, 0.f)*Scale_Rate);
			m_UltMat[1] = Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(90)) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(190)) *
				Matrix4x4DX::Mtrans(Vector3DX::vget(0.7f, 0.95f, -0.3f)*Scale_Rate);
			m_UltMat[2] = Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(90)) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(-10)) *
				Matrix4x4DX::Mtrans(Vector3DX::vget(-0.5f, 0.95f, 0.3f)*Scale_Rate);
			m_UltMat[3] = Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(80)) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(190)) *
				Matrix4x4DX::Mtrans(Vector3DX::vget(-0.9f, 0.95f, 0.f)*Scale_Rate);
			m_UltMat[4] = Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(-5)) * Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(100)) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(90)) *
				Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 1.01f, -0.4f)*Scale_Rate);
			{
				for (int i = 0;i < (int)ULT_GUN::Max;i++) {
					auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(1 + i).GetGun(0);
					auto Per = std::clamp(m_UltPer[i], 0.f, 1.f);
					if (GunPtr) {
						GunPtr->SetGunMatrix(
							Lerp(Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(90)) * Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(30)), m_UltMat[i].rotation(), Per)*
							Matrix4x4DX::Mtrans(Lerp(Vector3DX::vget(-0.05f, 1.2f, -0.6f)*Scale_Rate, m_UltMat[i].pos(), Per)));
					}
				}
			}
			{
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(0).GetGun(0);
				auto Per = 1.f - std::clamp(m_Range - 1.f, 0.f, 1.f);
				GunPtr->SetGunMatrix(
					Lerp(Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(90)) * Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(30)), Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(0)), std::clamp(Per*2.f, 0.f, 1.f))*
					Matrix4x4DX::Mtrans(
						Lerp(
							Lerp(Vector3DX::vget(0.1f, 1.05f, 0.f)*Scale_Rate, Vector3DX::vget(0.f, 1.2f, 0.f)*Scale_Rate, std::clamp(Per*2.f, 0.f, 1.f))
							, Vector3DX::vget(0.f, 1.05f, 0.f)*Scale_Rate, std::clamp(Per*2.f - 1.f, 0.f, 1.f))
					)
				);
				auto Mat = GunPtr->GetFrameWorldMat_P(GunFrame::Magpos);
				(*GunPtr->GetMagazinePtr())->SetMove().SetMat(
					Lerp(Matrix3x3DX::Get33DX(Mat.rotation()), Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(90)), std::clamp(Per * 2.f - 1.f, 0.f, 1.f)));
				(*GunPtr->GetMagazinePtr())->SetMove().SetPos(
					Lerp(
						Lerp(Mat.pos(), Mat.pos() + Matrix4x4DX::Vtrans(Vector3DX::vget(0.f, -0.15f, 0.05f)*Scale_Rate, Mat.rotation()), std::clamp(Per*2.f, 0.f, 1.f))
						, Vector3DX::vget(0.2f, 1.f, 0.f)*Scale_Rate, std::clamp(Per*2.f - 1.f, 0.f, 1.f))

				);
				(*GunPtr->GetMagazinePtr())->SetMove().Update(0.f, 0.f);
				(*GunPtr->GetMagazinePtr())->UpdateObjMatrix((*GunPtr->GetMagazinePtr())->GetMove().GetMat(), (*GunPtr->GetMagazinePtr())->GetMove().GetPos());
			}
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
								ChangeMod(-1);
								break;
							case LookSelect::ULTSet:
								ChangeULT(1);
								break;
							default:
								break;
						}
						break;
					case 4:
						switch (m_LookSel) {
							case LookSelect::ModSet:
								ChangeMod(1);
								break;
							case LookSelect::ULTSet:
								ChangeULT(-1);
								break;
							default:
								break;
						}
						break;
					case 5:
						ChangeModSel(-1);
						break;
					case 6:
						ChangeModSel(1);
						break;
					case 7:
						ResetMod();
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
			for (int i = 0;i < (int)ULT_GUN::Max;i++) {
				Easing(&m_UltPer[i], ((i == (int)GunsModify::GetULTSelect()) && (m_LookSel == LookSelect::ULTSet)) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
			}
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

			GunsModify::SaveSlots("Save/gundata.svf");

			for (int i = 0;i < (int)ULT_GUN::Max;i++) {
				ObjMngr->DelObj(&PlayerMngr->GetPlayer(1 + i).GetGun(0));
			}
			ObjMngr->DelObj(&PlayerMngr->GetPlayer(0).GetGun(0));
			PlayerMngr->GetPlayer(0).Dispose();
			for (int i = 0;i < (int)ULT_GUN::Max;i++) {
				PlayerMngr->GetPlayer(1 + i).Dispose();
			}
			GunsModify::DisposeSlots();
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

			ClearDrawScreenZBuffer();
			switch (m_LookSel) {
				case LookSelect::ModSet:
					{
						auto& y = GunsModify::GetSelData()[SelMoveClass[select].index];
						auto& ModPtr1 = (std::shared_ptr<ModClass>&)(y->m_Data->GetPartsPtr(y->SlotType));
						if (ModPtr1) {
							SetUseLighting(FALSE);
							ModPtr1->GetObj().SetOpacityRate(0.5f*std::clamp(m_SelAlpha, 0.f, 1.f));
							MV1SetMaterialDrawAddColorAll(ModPtr1->GetObj().GetHandle(), -255, 255, -255);
							ModPtr1->GetObj().DrawModel();
							MV1SetMaterialDrawAddColorAll(ModPtr1->GetObj().GetHandle(), 0, 0, 0);
							ModPtr1->GetObj().SetOpacityRate(1.f);
							SetUseLighting(TRUE);
						}
					}
					break;
				case LookSelect::ULTSet:
					break;
				case LookSelect::FreeLook:
					break;
				default:
					break;
			}

		}

		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//
			WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
				DrawParts->GetUIY(96),
																FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																DrawParts->GetUIY(64), DrawParts->GetUIY(64), White, Black, "Customize");
			//
			WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp((int)(255.f*std::clamp(ButtonAlpha[(int)LookSelect::ModSet], 0.f, 1.f)), 0, 255));
			DrawCustomUI();
			WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp((int)(255.f*std::clamp(ButtonAlpha[(int)LookSelect::ULTSet], 0.f, 1.f)), 0, 255));
			DrawULTUI();
			WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
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
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(18),
																	  FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																	  DrawParts->GetUIY(32), DrawParts->GetUIY(1080 - 32 - 32), White, Black, LocalizePool::Instance()->Get(9010 + bselect));
			}
			//
			{
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp((int)(255.f*m_Alpha), 0, 255));

				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Black, TRUE);

				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}

		const char*		CustomScene::ItemSelName(int SlotSel, int sel) {
			auto& y = GunsModify::GetSelData()[SelMoveClass[SlotSel].index];
			const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
			if (sel != (int)Data->m_ItemsUniqueID.size()) {
				return (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[sel]))->GetName().c_str();
			}
			return "None";
		}

		void			CustomScene::ChangeModSel(int add) noexcept {
			if (add == 0) { return; }
			if (GunsModify::GetSelData().size() > 0) {
				auto* SE = SoundPool::Instance();
				bool IsChange = false;
				if (add > 0) {
					++select;
					IsChange = true;
				}
				if (add < 0) {
					--select;
					IsChange = true;
				}
				if (IsChange) {
					if (select < 0) { select = (int)SelMoveClass.size() - 1; }
					if (select > (int)SelMoveClass.size() - 1) { select = 0; }

					if (add > 0) {
						SelMoveClass[select].Yadd = 0.4f;
					}
					else {
						SelMoveClass[select].Yadd = -0.4f;
					}
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_SelAlpha = 2.f;
				}
			}
		}

		void			CustomScene::ResetMod() noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(0).GetGun(0);
			GunsModify::DisposeSlots();
			GunsModify::CreateSelData(GunPtr, true);
			UpdateSlotMove();
			GunPtr->Init_Gun();
			SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
		}
		void			CustomScene::ChangeMod(int add) noexcept {
			if (add == 0) { return; }
			if (GunsModify::GetSelData().size() > 0) {
				auto* SE = SoundPool::Instance();
				auto* SaveDataParts = SaveDataClass::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				auto& y = GunsModify::GetSelData()[SelMoveClass[select].index];
				const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);


				auto prev = y->m_sel;
				while (true) {
					y->m_sel += add;
					if (y->m_sel > Data->ItemMaxCount()) { y->m_sel = 0; }
					if (y->m_sel < 0) { y->m_sel = Data->ItemMaxCount(); }
					//無選択
					if (y->m_sel == (int)Data->m_ItemsUniqueID.size()) {
						break;
					}
					//開放済
					if (SaveDataParts->GetParam((*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[y->m_sel]))->GetName().c_str()) == 1) {
						break;
					}
				}
				if (prev != y->m_sel) {
					SelMoveClass[select].Xadd = (add > 0) ? 1.f : -1.f;
					auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(0).GetGun(0);
					GunsModify::ChangeSelData(y.get(), y->m_sel, false);
					UpdateSlotMove();
					GunPtr->Init_Gun();
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				m_SelAlpha = 2.f;
			}
		}

		void			CustomScene::ChangeULT(int add) noexcept {
			if (add == 0) { return; }
			auto* SE = SoundPool::Instance();
			auto* SaveDataParts = SaveDataClass::Instance();
			bool IsChange = false;
			int tmp = (int)GunsModify::GetULTSelect();
			if (add > 0) {
				tmp++;
				IsChange = true;
			}
			if (add < 0) {
				tmp--;
				IsChange = true;
			}

			if (IsChange) {
				int max = std::clamp(static_cast<int>(SaveDataParts->GetParam("ULT Unlock")), 1, (int)ULT_GUN::Max);
				if (tmp > max - 1) { tmp = 0; }
				if (tmp < 0) { tmp = max - 1; }
				if (tmp != (int)GunsModify::GetULTSelect()) {
					GunsModify::SetULTSelect((ULT_GUN)tmp);
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}

		void			CustomScene::DrawCustomUI(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			int xp1, yp1;

			const auto& y = GunsModify::GetSelData()[SelMoveClass[select].index];
			const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
			//
			{
				xp1 = DrawParts->GetUIY(960 - 400 + (int)(SelMoveClass[select].Yadd*64.f));
				yp1 = DrawParts->GetUIY(540 - 270);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(32), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																	  xp1, yp1, White, Gray75, GunSlotName[(int)Data->m_GunSlot]);
			}
			{
				xp1 = DrawParts->GetUIY(960 - 400);
				yp1 = DrawParts->GetUIY(540 - 270 + 48);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(18), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																	   xp1, yp1, White, Gray75, "%d/%d", select + 1, (int)SelMoveClass.size());
			}
			//
			{
				auto* OptionParts = OPTION::Instance();
				xp1 = DrawParts->GetUIY(1900);
				yp1 = DrawParts->GetUIY(1080 - 12 - 24 * 3);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(0).GetGun(0);
				if (GunPtr) {
					auto& STR = OptionParts->GetParamInt(EnumSaveParam::Language) ? GunPtr->GetModData()->GetInfoEng() : GunPtr->GetModData()->GetInfo();
					for (auto& s : STR) {
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
																			  xp1, yp1, Green, DarkGreen, s.c_str());
						yp1 += DrawParts->GetUIY(24);
					}
				}
			}
			//
			{
				xp1 = DrawParts->GetUIY(160);
				yp1 = DrawParts->GetUIY(540);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(0).GetGun(0);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																	  xp1, yp1, Green, DarkGreen, "ShotMode : %s", SHOTTYPEName[(int)GunPtr->GetShotType()]);
				yp1 += DrawParts->GetUIY(24 + 4);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																	  xp1, yp1, Green, DarkGreen, "ShotSound : %s", GunShootSoundName[(int)GunPtr->GetGunShootSound()]);
				yp1 += DrawParts->GetUIY(24 + 4);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																	  xp1, yp1, Green, DarkGreen, "RecoilPower : %d", (int)(GunPtr->GetRecoilPower()*GunPtr->GetRecoilReturn()));
				yp1 += DrawParts->GetUIY(24 + 4);
			}
			//
			{
				xp1 = DrawParts->GetUIY(960);
				yp1 = DrawParts->GetUIY(840);

				if (y->m_sel != (int)Data->m_ItemsUniqueID.size()) {
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																		xp1 + DrawParts->GetUIY((int)(SelMoveClass[select].Xadd*100.f)), yp1 + DrawParts->GetUIY((int)(SelMoveClass[select].Yadd*64.f)), Green, DarkGreen,
																		(*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[y->m_sel]))->GetName().c_str()
					);
				}
				else {
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																		xp1 + DrawParts->GetUIY((int)(SelMoveClass[select].Xadd*100.f)), yp1 + DrawParts->GetUIY((int)(SelMoveClass[select].Yadd*64.f)), Green, DarkGreen,
																		"None"
					);
				}
			}
			//
			{
				xp1 = DrawParts->GetUIY(960);
				yp1 = DrawParts->GetUIY(920);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(32), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																	   xp1, yp1, Green, DarkGreen, "%d / %d", y->m_sel + 1, Data->ItemMaxCount() + 1);
			}
			//
			{
				xp1 = DrawParts->GetUIY(160);
				yp1 = DrawParts->GetUIY(840) - (DrawParts->GetUIY(28)*Data->ItemMaxCount()) / 2;

				auto* SaveDataParts = SaveDataClass::Instance();
				auto* Pad = PadControl::Instance();

				int mouseover = -1;
				for (int sel = 0;sel <= Data->ItemMaxCount();sel++) {
					unsigned int Color = (y->m_sel == sel) ? Green : Gray50;
					if (sel != (int)Data->m_ItemsUniqueID.size()) {
						const char* Name = (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[sel]))->GetName().c_str();
						if (SaveDataParts->GetParam(Name) == 1) {
							WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
								DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																				  xp1, yp1, Color, DarkGreen, Name);
						}
						else {
							WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
								DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																				  xp1, yp1, GetColor(216, 143, 143), DarkGreen, "????");
							int xsize = Fonts->Get(FontPool::FontType::MS_Gothic, DrawParts->GetUIY(24), 3)->GetStringWidth(DrawParts->GetUIY(24), "????");
							int ysize = DrawParts->GetUIY(24);
							if (IntoMouse(xp1, yp1, xp1 + xsize, yp1 + ysize)) {
								mouseover = sel;
							}
						}
					}
					else {
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																			  xp1, yp1, Color, DarkGreen, "None");
					}
					yp1 += DrawParts->GetUIY(28);
				}
				if (mouseover != -1) {
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																		  Pad->GetMS_X(), Pad->GetMS_Y(), White, Gray75, LocalizePool::Instance()->Get(9000));

				}
			}
		}
		void			CustomScene::DrawULTUI(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();

			int xp1, yp1;
			{
				xp1 = DrawParts->GetUIY(960 - 400);
				yp1 = DrawParts->GetUIY(540 - 270);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(32), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																	  xp1, yp1, White, Gray75, LocalizePool::Instance()->Get(9001));
			}
			//
			{
				xp1 = DrawParts->GetUIY(960);
				yp1 = DrawParts->GetUIY(840);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(1 + (int)GunsModify::GetULTSelect()).GetGun(0);
				if (GunPtr) {
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																		xp1, yp1, Green, DarkGreen, GunPtr->GetModData()->GetName());
				}
			}
			//
			{
				xp1 = DrawParts->GetUIY(1900);
				yp1 = DrawParts->GetUIY(1080 - 12 - 24 * 3);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(1 + (int)GunsModify::GetULTSelect()).GetGun(0);
				if (GunPtr) {
					auto& STR = GunPtr->GetModData()->GetInfo();
					for (auto& s : STR) {
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
																			  xp1, yp1, Green, DarkGreen, s.c_str());
						yp1 += DrawParts->GetUIY(24);
					}
				}
			}
			//
			{
				xp1 = DrawParts->GetUIY(160);
				yp1 = DrawParts->GetUIY(540);
				auto& GunPtr = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(1 + (int)GunsModify::GetULTSelect()).GetGun(0);
				if (GunPtr) {
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																		  xp1, yp1, Green, DarkGreen, "ShotMode : %s", SHOTTYPEName[(int)GunPtr->GetShotType()]);
					yp1 += DrawParts->GetUIY(24 + 4);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																		  xp1, yp1, Green, DarkGreen, "ShotSound : %s", GunShootSoundName[(int)GunPtr->GetGunShootSound()]);
					yp1 += DrawParts->GetUIY(24 + 4);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																		  xp1, yp1, Green, DarkGreen, "RecoilPower : %d", (int)(GunPtr->GetRecoilPower()*GunPtr->GetRecoilReturn()));
					yp1 += DrawParts->GetUIY(24 + 4);
				}
			}
			//
			{
				xp1 = DrawParts->GetUIY(960);
				yp1 = DrawParts->GetUIY(920);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(32), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																	   xp1, yp1, Green, DarkGreen, "%d / %d", (int)GunsModify::GetULTSelect() + 1, (int)ULT_GUN::Max);
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
