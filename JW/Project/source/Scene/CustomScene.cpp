#include	"CustomScene.hpp"

#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CustomScene::Set_Sub(void) noexcept {
			//auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* OptionParts = OPTION::Instance();
			//
			SetAmbientLight(VECTOR_ref::vget(0.4f, -0.5f, 0.1f), GetColorF(1.f, 1.f, 1.f, 0.0f));
			SetNearShadow(VECTOR_ref::vget(Scale_Rate*-6.f, Scale_Rate*-6.f, Scale_Rate*-6.f), VECTOR_ref::vget(Scale_Rate*6.f, Scale_Rate*0.f, Scale_Rate*6.f));
			SetShadowDir(GetLightVec(), 0);
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(GetLightVec().get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.01f, 0.01f, 0.01f, 0.f));
			SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.f));
			//
			select = 0;
			//
			{
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadObjectModel((*Ptr).get(), "data/gun/G17Gen3/");
				MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
				m_GunPtr = ((std::shared_ptr<GunClass>&)(*Ptr));
				(*Ptr)->Init();
			}
			{
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadObjectModel((*Ptr).get(), "data/gun/M16-4/");
				MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
				m_UltPtr[0] = ((std::shared_ptr<GunClass>&)(*Ptr));
				(*Ptr)->Init();
			}
			{
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadObjectModel((*Ptr).get(), "data/gun/Mod870/");
				MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
				m_UltPtr[1] = ((std::shared_ptr<GunClass>&)(*Ptr));
				(*Ptr)->Init();
			}
			{
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadObjectModel((*Ptr).get(), "data/gun/PCC_4/");
				MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
				m_UltPtr[2] = ((std::shared_ptr<GunClass>&)(*Ptr));
				(*Ptr)->Init();
			}
			{
				auto* Ptr = ObjMngr->MakeObject(ObjType::MovieObj);
				ObjMngr->LoadObjectModel((*Ptr).get(), "data/model/table/");
				(*Ptr)->Init();
			}
			m_Yrad = deg2rad(-45);
			m_Xrad = 0.f;
			m_Yrad_R = m_Yrad;
			m_Xrad_R = m_Xrad;
			m_Range = 100.f;
			m_SelAlpha = 0.f;
			//
			GunsModify::LoadSlots("data/bokuzyo.ok");
			//
			GunsModify::CreateSelData(m_GunPtr, true);
			UpdateSlotMove();
			m_GunPtr->Init_Gun();

			for (auto&u : m_UltPtr) {
				GunsModify::CreateSelData(u, false);
				u->Init_Gun();
			}
			m_IsEnd = false;
			m_Alpha = 1.f;

			m_PrevShadow = OptionParts->Get_Shadow();
			OptionParts->Set_Shadow(true);
		}
		bool			CustomScene::Update_Sub(void) noexcept {
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
				KeyGuide->AddGuide(PADS::MOVE_W, "");
				KeyGuide->AddGuide(PADS::MOVE_S, "");
				KeyGuide->AddGuide(PADS::MOVE_A, "");
				KeyGuide->AddGuide(PADS::MOVE_D, "");
				KeyGuide->AddGuide(PADS::MOVE_STICK, "スロット選択,パーツ変更");
				KeyGuide->AddGuide(PADS::AIM, "離して見る");
				KeyGuide->AddGuide(PADS::RELOAD, "セーブして終了");
				KeyGuide->AddGuide(PADS::MELEE, "編集開始時に戻す");
			}
			);

			{
				if (Pad->GetKey(PADS::MOVE_W).trigger()) {
					--select;
					if (select < 0) { select = (int)SelMoveClass.size() - 1; }
					SelMoveClass[select].Yadd = 10.f;
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_SelAlpha = 2.f;
				}
				if (Pad->GetKey(PADS::MOVE_S).trigger()) {
					++select;
					if (select > (int)SelMoveClass.size() - 1) { select = 0; }
					SelMoveClass[select].Yadd = -10.f;
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_SelAlpha = 2.f;
				}
				if (GetSelData().size() > 0) {
					bool IsChange = false;
					auto& y = GetSelData()[SelMoveClass[select].index];
					if (Pad->GetKey(PADS::MOVE_A).trigger()) {
						++y->m_sel;
						const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
						if (Data->m_IsNeed) {
							if (y->m_sel > (int)Data->m_ItemsUniqueID.size() - 1) { y->m_sel = 0; }
						}
						else {
							if (y->m_sel > (int)Data->m_ItemsUniqueID.size()) { y->m_sel = 0; }
						}
						IsChange = true;
						SelMoveClass[select].Xadd = -1.f;
						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetKey(PADS::MOVE_D).trigger()) {
						--y->m_sel;
						const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
						if (Data->m_IsNeed) {
							if (y->m_sel < 0) { y->m_sel = (int)Data->m_ItemsUniqueID.size() - 1; }
						}
						else {
							if (y->m_sel < 0) { y->m_sel = (int)Data->m_ItemsUniqueID.size(); }
						}
						IsChange = true;
						SelMoveClass[select].Xadd = 1.f;
						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (IsChange) {
						m_SelAlpha = 2.f;
						GunsModify::ChangeSelData(y, y->m_sel, false);
						UpdateSlotMove();
					}
				}
				for (auto& y : SelMoveClass) {
					Easing(&y.Xadd, 0.f, 0.9f, EasingType::OutExpo);
					Easing(&y.Yadd, 0.f, 0.95f, EasingType::OutExpo);
				}
				m_SelAlpha = std::max(m_SelAlpha - 1.f / FPS, 0.f);

				if (Pad->GetKey(PADS::MELEE).trigger()) {
					SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
					//
					GunsModify::DisposeSlots();
					GunsModify::CreateSelData(m_GunPtr, true);
					UpdateSlotMove();
					m_GunPtr->Init_Gun();
				}
			}

			auto Per = 1.f - std::clamp(m_Range - 1.f, 0.f, 1.f);

			m_UltPtr[0]->SetGunMatrix(
				MATRIX_ref::RotZ(deg2rad(90)) * MATRIX_ref::RotY(deg2rad(210)) *
				MATRIX_ref::Mtrans(VECTOR_ref::vget(-0.8f, 0.95f, 0.f)*Scale_Rate));
			m_UltPtr[1]->SetGunMatrix(
				MATRIX_ref::RotZ(deg2rad(90)) * MATRIX_ref::RotY(deg2rad(190)) *
				MATRIX_ref::Mtrans(VECTOR_ref::vget(0.7f, 0.95f, -0.3f)*Scale_Rate));
			m_UltPtr[2]->SetGunMatrix(
				MATRIX_ref::RotZ(deg2rad(90)) * MATRIX_ref::RotY(deg2rad(0)) *
				MATRIX_ref::Mtrans(VECTOR_ref::vget(1.1f, 0.95f, 0.f)*Scale_Rate));

			m_GunPtr->SetGunMatrix(
				Lerp_Matrix(MATRIX_ref::RotY(deg2rad(90)) * MATRIX_ref::RotX(deg2rad(30)), MATRIX_ref::RotY(deg2rad(0)), std::clamp(Per*2.f, 0.f, 1.f))*
				MATRIX_ref::Mtrans(
					Lerp(
						Lerp(VECTOR_ref::vget(0.1f, 1.05f, 0.f)*Scale_Rate, VECTOR_ref::vget(0.f, 1.2f, 0.f)*Scale_Rate, std::clamp(Per*2.f, 0.f, 1.f))
						, VECTOR_ref::vget(0.f, 1.05f, 0.f)*Scale_Rate, std::clamp(Per*2.f - 1.f, 0.f, 1.f))
				)
			);
			auto Mat = m_GunPtr->GetFrameWorldMat(GunFrame::Magpos);
			(*m_GunPtr->GetMagazinePtr())->SetMove(
				Lerp_Matrix(Mat.GetRot(), MATRIX_ref::RotZ(deg2rad(90)), std::clamp(Per*2.f - 1.f, 0.f, 1.f)),
				Lerp(
					Lerp(Mat.pos(), Mat.pos() + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, -0.15f, 0.05f)*Scale_Rate, Mat.GetRot()), std::clamp(Per*2.f, 0.f, 1.f))
					, VECTOR_ref::vget(0.2f, 1.f, 0.f)*Scale_Rate, std::clamp(Per*2.f - 1.f, 0.f, 1.f))

			);
			(*m_GunPtr->GetMagazinePtr())->UpdateMove();

			m_Yrad += Pad->GetLS_X() / 1000.f;
			m_Yrad = std::clamp(m_Yrad, deg2rad(-100), deg2rad(100));
			m_Xrad += Pad->GetLS_Y() / 1000.f;
			m_Xrad = std::clamp(m_Xrad, deg2rad(-20), deg2rad(-20));
			Easing(&m_Xrad_R, m_Xrad, 0.95f, EasingType::OutExpo);
			Easing(&m_Yrad_R, m_Yrad, 0.95f, EasingType::OutExpo);
			Easing(&m_Range, Pad->GetKey(PADS::AIM).press() ? 2.f : 1.f, 0.95f, EasingType::OutExpo);
			Pad->SetMouseMoveEnable(Pad->GetKey(PADS::AIM).press());

			if (Pad->GetKey(PADS::AIM).release_trigger()) {
				m_Yrad = deg2rad(-45);
			}
			if (Pad->GetKey(PADS::AIM).trigger()) {
				m_Yrad = deg2rad(15);
			}

			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();

			VECTOR_ref Pos = VECTOR_ref::vget(0.f, 1.f, 0.f)*Scale_Rate;
			DrawParts->SetMainCamera().SetCamPos(
				Pos + MATRIX_ref::Vtrans(VECTOR_ref::front()*(m_Range*Scale_Rate), MATRIX_ref::RotX(m_Xrad_R)*MATRIX_ref::RotY(m_Yrad_R + DX_PI_F)),
				Pos,
				VECTOR_ref::vget(0.f, 1.f, 0.f));
			float far_t = 20.f*Scale_Rate;
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(45), 0.5f*Scale_Rate, far_t);
			PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t / 2, 0.5f*Scale_Rate, far_t);

			m_IsEnd |= Pad->GetKey(PADS::RELOAD).trigger();
			m_Alpha = std::clamp(m_Alpha + (m_IsEnd ? 1.f : -1.f) / 60, 0.f, 1.f);//一旦FPS使わない
			if (m_IsEnd && m_Alpha == 1.f) {
				return false;
			}
			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* OptionParts = OPTION::Instance();

			GunsModify::SaveSlots("data/bokuzyo.ok");

			GunsModify::DisposeSlots();

			ObjMngr->DelObj((SharedObj*)&m_GunPtr);
			m_GunPtr.reset();
			for (auto&u : m_UltPtr) {
				ObjMngr->DelObj((SharedObj*)&u);
				u.reset();
			}
			ObjMngr->DisposeObject();
			OptionParts->Set_Shadow(m_PrevShadow);
		}
		void			CustomScene::ShadowDraw_Sub(void) noexcept {
			//this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			CustomScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject();

			ClearDrawScreenZBuffer();
			{
				auto& y = GetSelData()[SelMoveClass[select].index];
				auto& ModPtr1 = (std::shared_ptr<ModClass>&)(y->m_Data->GetSlotControl()->GetPartsPtr(y->SlotType));
				if (ModPtr1) {
					SetUseLighting(FALSE);
					ModPtr1->GetObj().SetOpacityRate(0.5f*std::clamp(m_SelAlpha, 0.f, 1.f));
					MV1SetMaterialDrawAddColorAll(ModPtr1->GetObj().get(), -255, 255, -255);
					ModPtr1->GetObj().DrawModel();
					MV1SetMaterialDrawAddColorAll(ModPtr1->GetObj().get(), 0, 0, 0);
					ModPtr1->GetObj().SetOpacityRate(1.f);
					SetUseLighting(TRUE);
				}
			}
		}

		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto Green = GetColor(0, 255, 0);
			auto Green50 = GetColor(0, 128, 0);
			auto Green25 = GetColor(0, 64, 0);
			auto White = GetColor(255, 255, 255);
			auto Gray50 = GetColor(128, 128, 128);
			auto Gray25 = GetColor(64, 64, 64);
			auto Black = GetColor(0, 0, 0);
			int xp1, yp1;
			//
			xp1 = y_r(64 + 300);
			yp1 = y_r(64 + 48 * 2);

			auto Per = 1.f - std::clamp(m_Range - 1.f, 0.f, 1.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*std::clamp(Per*2.f - 1.f, 0.f, 1.f)), 0, 255));

			DrawFetteString(xp1, yp1, 1.33f, false, "Custom");
			{
				xp1 = y_r(960 - 480);
				yp1 = y_r(540 - 270);
				auto& y = GetSelData()[SelMoveClass[select].index];
				const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(32), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
					xp1, yp1, White, Gray25, GunSlotName[(int)Data->m_GunSlot]);
			}
			for (int loop = -1; loop <= (int)SelMoveClass.size(); loop++) {
				if (!(select == loop + 1 || select == loop - 1)) { continue; }
				int index = loop;
				if (index < 0) { index = (int)SelMoveClass.size() - 1; }
				if (index > (int)SelMoveClass.size() - 1) { index = 0; }
				auto& y = GetSelData()[SelMoveClass[index].index];
				const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
				xp1 = y_r(960);
				yp1 = y_r(840 + 64 * (loop - select) + (int)SelMoveClass[index].Yadd);

				for (int loop2 = -1; loop2 <= 1; loop2++) {
					int sel = y->m_sel + loop2;
					if (Data->m_IsNeed) {
						if (sel < 0) { sel = (int)Data->m_ItemsUniqueID.size() - 1; }
						if (sel > (int)Data->m_ItemsUniqueID.size() - 1) { sel = 0; }
					}
					else {
						if (sel < 0) { sel = (int)Data->m_ItemsUniqueID.size(); }
						if (sel > (int)Data->m_ItemsUniqueID.size()) { sel = 0; }
					}
					std::string Name = "None";
					if (sel != (int)Data->m_ItemsUniqueID.size()) {
						Name = (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[sel]))->GetName();
					}
					if (loop2 == -1) {
						int add = y_r(200) + y_r(SelMoveClass[index].Xadd*300.f) / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT,
							(select == loop + 1) ? FontHandle::FontYCenter::TOP : FontHandle::FontYCenter::BOTTOM,
							xp1 + add, yp1 + y_r(20), Gray50, Gray25, Name);
					}
					if (loop2 == 1) {
						int add = -y_r(200) + +y_r(SelMoveClass[index].Xadd*300.f) / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT,
							(select == loop + 1) ? FontHandle::FontYCenter::TOP : FontHandle::FontYCenter::BOTTOM,
							xp1 + add, yp1 + y_r(20), Gray50, Gray25, Name);
					}
				}
				{
					std::string Name = "None";
					if (y->m_sel != (int)Data->m_ItemsUniqueID.size()) {
						Name = (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[y->m_sel]))->GetName();
					}
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(32), FontHandle::FontXCenter::MIDDLE,
						(select == loop + 1) ? FontHandle::FontYCenter::TOP : FontHandle::FontYCenter::BOTTOM,
						xp1 + +y_r(SelMoveClass[index].Xadd*300.f), yp1 + y_r(20), Gray50, Gray25, Name);
				}
			}
			{
				auto& y = GetSelData()[SelMoveClass[select].index];
				const auto& Data = y->m_Data->GetModData()->GetPartsSlot(y->SlotType);
				xp1 = y_r(960);
				yp1 = y_r(840 + (int)SelMoveClass[select].Yadd);
				for (int loop2 = -1; loop2 <= 1; loop2++) {
					int sel = y->m_sel + loop2;
					if (Data->m_IsNeed) {
						if (sel < 0) { sel = (int)Data->m_ItemsUniqueID.size() - 1; }
						if (sel > (int)Data->m_ItemsUniqueID.size() - 1) { sel = 0; }
					}
					else {
						if (sel < 0) { sel = (int)Data->m_ItemsUniqueID.size(); }
						if (sel > (int)Data->m_ItemsUniqueID.size()) { sel = 0; }
					}
					std::string Name = "None";
					if (sel != (int)Data->m_ItemsUniqueID.size()) {
						Name = (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[sel]))->GetName();
					}
					if (loop2 == -1) {
						int add = y_r(380) + y_r(SelMoveClass[select].Xadd*300.f) / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(36), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
							xp1 + add, yp1 + y_r(20), Green50, Green25, Name);
					}
					if (loop2 == 1) {
						int add = -y_r(380) + y_r(SelMoveClass[select].Xadd*300.f) / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
							xp1 + add, yp1 + y_r(20), Green50, Green25, Name);
					}
				}
				{
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(54), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
						xp1 + -y_r(350), yp1 + y_r(20), Green, Green25, "<");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(54), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
						xp1 + y_r(350), yp1 + y_r(20), Green, Green25, ">");
				}
				{
					std::string Name = "None";
					if (y->m_sel != (int)Data->m_ItemsUniqueID.size()) {
						Name = (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[y->m_sel]))->GetName();
					}
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(54), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
						xp1 + y_r(SelMoveClass[select].Xadd*300.f), yp1 + y_r(20), Green, Green25, Name);
				}
			}
			//
			{
				xp1 = y_r(32);
				yp1 = y_r(1080 - 32 - 32);

				std::string Info = "";
				Info = "3分間の間敵を倒し続けてください。累計撃墜数はこちらでのみカウントされます";
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Black, Info);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*m_Alpha), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
