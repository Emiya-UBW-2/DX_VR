#include	"CustomScene.hpp"

#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		bool			CustomScene::DelSelData(const Slot* SlotPtr) {
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				if (y != SlotPtr) {
					if (y->ParentSlot == SlotPtr) {
						if (DelSelData(y)) { loop--; }
					}
				}
			}
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				if (y == SlotPtr) {
					auto* ModPtr = (ModSlotControl*)y->GetData();
					y->m_sel = (int)ModPtr->GetModData()->GetPartsSlot(y->SlotType)->m_ItemsUniqueID.size();
					y->m_selectSwitch = true;
					SetMods(ModPtr, y->ParentSlot);
					SelData.erase(SelData.begin() + loop);
					if (select >= (int)SelData.size()) {
						select = (int)SelData.size() - 1;
					}
					return true;
				}
			}
			return false;
		}
		void			CustomScene::ChangeSelData(const Slot* SlotPtr, int sel) {
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				if (y != SlotPtr) {
					if (y->ParentSlot == SlotPtr) {
						if (DelSelData(y)) { loop--; }
					}
				}
			}
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				if (y == SlotPtr) {
					y->m_sel = sel;
					y->m_selectSwitch = true;
					SetMods((ModSlotControl*)y->GetData(), y->ParentSlot);
					//子
					auto& ModPtr1 = (std::shared_ptr<ModClass>&)(y->GetData()->GetSlotControl()->GetPartsPtr(y->SlotType));
					AddSelData((std::shared_ptr<ObjectBaseClass>&)ModPtr1, y);
					SetMods(ModPtr1.get(), y);
					//孫以降
					UpdateMods(ModPtr1.get(), y);
					break;
				}
			}
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				auto* ModPtr = (ModSlotControl*)y->GetData();
				if (!ModPtr->GetModData()) {
					int a = 0;
				}
			}
		}
		void			CustomScene::AddSelData(const std::shared_ptr<ObjectBaseClass>& ModPtr, const Slot* SlotPtr) {
			if (!ModPtr) { return; }
			ModSlotControl* ModP = nullptr;
			if (SlotPtr) {
				ModP = ((std::shared_ptr<ModClass>&)ModPtr).get();

			}
			else {
				ModP = ((std::shared_ptr<GunClass>&)ModPtr).get();
			}
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (ModP->GetModData()->GetPartsSlot((GunSlot)loop)) {
					SelData.emplace_back(new Slot);
					SelData.back()->ParentSlot = SlotPtr;
					SelData.back()->Init((GunSlot)loop, ModPtr);
				}
			}
		}
		//自分の下の世代のオブジェをセット
		void			CustomScene::SetMods(ModSlotControl* ModPtr, const Slot* SlotPtr) {
			for (auto& y : SelData) {
				if (y->ParentSlot == SlotPtr) {
					if (y->m_sel != (int)ModPtr->GetModData()->GetPartsSlot(y->SlotType)->m_ItemsUniqueID.size()) {
						if (y->m_selectSwitch) {
							y->m_selectSwitch = false;
							ModPtr->RemoveMod(y->SlotType);
							ModPtr->SetMod(y->SlotType, y->m_sel, m_GunPtr->GetObj());
						}
					}
					else {
						ModPtr->RemoveMod(y->SlotType);
					}
				}
			}
		}
		void			CustomScene::UpdateMods(ModSlotControl* ModPtr, const Slot* SlotPtr) noexcept {
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const Slot* ChildSlot = SelData[loop];
				if (ChildSlot->ParentSlot != SlotPtr) { continue; }
				if (!ModPtr->GetSlotControl()->HasParts(ChildSlot->SlotType)) { continue; }

				auto& ChildPtr = (std::shared_ptr<ModClass>&)(ModPtr->GetSlotControl()->GetPartsPtr(ChildSlot->SlotType));
				AddSelData((std::shared_ptr<ObjectBaseClass>&)ChildPtr, ChildSlot);
				SetMods(ChildPtr.get(), ChildSlot);

				UpdateMods(ChildPtr.get(), ChildSlot);
			}
		}

		void			CustomScene::Set_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			//
			SetAmbientLight(VECTOR_ref::vget(-0.8f, -0.5f, -0.1f), GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			SetFarShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetMiddleShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetNearShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			//
			select = 0;
			//
			{
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadObjectModel((*Ptr).get(), "data/gun/G17Gen3/");
				MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
				m_GunPtr = ((std::shared_ptr<GunClass>&)(*Ptr));
				(*Ptr)->Init();
				//全消し
				for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
					m_GunPtr->RemoveMod((GunSlot)loop);
				}
			}
			m_Yrad = deg2rad(135);
			m_Xrad = 0.f;
			m_SelAlpha = 0.f;
			//
			{
				const Slot* MySlot0 = nullptr;
				//子
				auto& ModPtr0 = m_GunPtr;
				AddSelData((std::shared_ptr<ObjectBaseClass>&)ModPtr0, MySlot0);
				SetMods(ModPtr0.get(), MySlot0);
				//孫以降
				UpdateMods(ModPtr0.get(), MySlot0);
				m_GunPtr->Init_Gun();
			}
			//
		}
		//
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
					auto* KeyGuide = KeyGuideClass::Instance();
					KeyGuide->Reset();
					KeyGuide->AddGuide("ng.png", "決定");
					KeyGuide->AddGuide("ok.png", "戻る");
					KeyGuide->AddGuide("R_stick.png", "上下選択");
				},
				[&]() {
					auto* KeyGuide = KeyGuideClass::Instance();
					KeyGuide->Reset();
					KeyGuide->AddGuide("none.jpg", "決定");
					KeyGuide->AddGuide("X.jpg", "戻る");
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "上下選択");
				}
			);

			{
				if (Pad->GetUpKey().trigger()) {
					--select;
					if (select < 0) { select = (int)SelData.size() - 1; }
					SelData[select]->Yadd = 10.f;
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_SelAlpha = 2.f;
				}
				if (Pad->GetDownKey().trigger()) {
					++select;
					if (select > (int)SelData.size() - 1) { select = 0; }
					SelData[select]->Yadd = -10.f;
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_SelAlpha = 2.f;
				}
				if (SelData.size() > 0) {
					bool IsChange = false;
					auto& y = SelData[select];
					if (Pad->GetLeftKey().trigger()) {
						++y->m_sel;
						const auto& Data = y->GetData()->GetModData()->GetPartsSlot(y->SlotType);
						if (Data->m_IsNeed) {
							if (y->m_sel > (int)Data->m_ItemsUniqueID.size() - 1) { y->m_sel = 0; }
						}
						else {
							if (y->m_sel > (int)Data->m_ItemsUniqueID.size()) { y->m_sel = 0; }
						}
						IsChange = true;
						y->Xadd = 10.f;
						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetRightKey().trigger()) {
						--y->m_sel;
						const auto& Data = y->GetData()->GetModData()->GetPartsSlot(y->SlotType);
						if (Data->m_IsNeed) {
							if (y->m_sel < 0) { y->m_sel = (int)Data->m_ItemsUniqueID.size() - 1; }
						}
						else {
							if (y->m_sel < 0) { y->m_sel = (int)Data->m_ItemsUniqueID.size(); }
						}
						IsChange = true;
						y->Xadd = -10.f;
						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (IsChange) {
						m_SelAlpha = 2.f;
						ChangeSelData(y, y->m_sel);
					}
				}
				for (auto& y : SelData) {
					Easing(&y->Xadd, 0.f, 0.95f, EasingType::OutExpo);
					Easing(&y->Yadd, 0.f, 0.95f, EasingType::OutExpo);
				}
				m_SelAlpha = std::max(m_SelAlpha - 1.f / FPS, 0.f);

				if (Pad->GetOKKey().trigger()) {
					SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}

			m_GunPtr->SetGunMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.05f, 0.f)*Scale_Rate));
			(*m_GunPtr->GetMagazinePtr())->SetMove(MATRIX_ref::RotZ(deg2rad(90)), VECTOR_ref::vget(0.2f, 0.f, 0.f)*Scale_Rate);
			(*m_GunPtr->GetMagazinePtr())->UpdateMove();

			m_Yrad += Pad->GetLS_X() / 1000.f;
			m_Xrad += Pad->GetLS_Y() / 1000.f;
			m_Xrad = std::clamp(m_Xrad, deg2rad(-80), deg2rad(-10));

			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();

			DrawParts->SetMainCamera().SetCamPos(
				MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.f, 1.f)*Scale_Rate, MATRIX_ref::RotX(m_Xrad)*MATRIX_ref::RotY(m_Yrad)),
				VECTOR_ref::vget(0.f, 0.f, 0.f),
				VECTOR_ref::vget(0.f,1.f,0.f));
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(45), 0.1f*Scale_Rate, 10.f*Scale_Rate);
			PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, 0.1f*Scale_Rate / 2, 0.1f*Scale_Rate, 10.f*Scale_Rate);

			if (Pad->GetNGKey().trigger()) {
				return false;
			}
			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			SelData.clear();

			ObjMngr->DelObj((SharedObj*)&m_GunPtr);
			m_GunPtr.reset();
		}
		void			CustomScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject();

			ClearDrawScreenZBuffer();
			{
				auto& y = SelData[select];
				auto& ModPtr1 = (std::shared_ptr<ModClass>&)(y->GetData()->GetSlotControl()->GetPartsPtr(y->SlotType));
				if (ModPtr1) {
					SetUseLighting(FALSE);
					ModPtr1->GetObj().SetOpacityRate(0.5f*std::clamp(m_SelAlpha,0.f,1.f));
					MV1SetMaterialDrawAddColorAll(ModPtr1->GetObj().get(), -255, 255, -255);
					ModPtr1->GetObj().DrawModel();
					MV1SetMaterialDrawAddColorAll(ModPtr1->GetObj().get(), 0, 0, 0);
					ModPtr1->GetObj().SetOpacityRate(1.f);
					SetUseLighting(TRUE);
				}
			}
		}

		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
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
			DrawFetteString(xp1, yp1, 1.33f, false, "Custom");
			{
				xp1 = y_r(960 - 480);
				yp1 = y_r(540 - 270);
				auto& y = SelData[select];
				const auto& Data = y->GetData()->GetModData()->GetPartsSlot(y->SlotType);
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(32), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
					xp1, yp1, White, Gray25, GunSlotName[(int)Data->m_GunSlot]);
			}
			for (int loop = -1; loop <= (int)SelData.size(); loop++) {
				if (!(select == loop + 1 ||select == loop - 1)) { continue; }
				int index = loop;
				if (index < 0) { index = (int)SelData.size() - 1; }
				if (index > (int)SelData.size() - 1) { index = 0; }
				auto& y = SelData[index];
				const auto& Data = y->GetData()->GetModData()->GetPartsSlot(y->SlotType);
				xp1 = y_r(960);
				yp1 = y_r(840 + 64 * (loop - select) + (int)y->Yadd);

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
						int add = y_r(200) + (int)y->Xadd / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT,
							(select == loop + 1) ? FontHandle::FontYCenter::TOP : FontHandle::FontYCenter::BOTTOM,
							xp1 + add, yp1 + y_r(20), Gray50, Gray25, Name);
					}
					if (loop2 == 1) {
						int add = -y_r(200) + (int)y->Xadd / 2;
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
						xp1 + (int)y->Xadd, yp1 + y_r(20), Gray50, Gray25, Name);
				}
			}
			{
				auto& y = SelData[select];
				const auto& Data = y->GetData()->GetModData()->GetPartsSlot(y->SlotType);
				xp1 = y_r(960);
				yp1 = y_r(840 + (int)y->Yadd);
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
						int add = y_r(320) + (int)y->Xadd / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(36), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
							xp1 + add, yp1 + y_r(20), Green50, Green25, Name);
					}
					if (loop2 == 1) {
						int add = -y_r(320) + (int)y->Xadd / 2;
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
							xp1 + add, yp1 + y_r(20), Green50, Green25, Name);
					}
				}
				{
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(54), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
						xp1 + -y_r(280), yp1 + y_r(20), Green, Green25, "<");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(54), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
						xp1 + y_r(280), yp1 + y_r(20), Green, Green25, ">");
				}
				{
					std::string Name = "None";
					if (y->m_sel != (int)Data->m_ItemsUniqueID.size()) {
						Name = (*ModDataManager::Instance()->GetData(Data->m_ItemsUniqueID[y->m_sel]))->GetName();
					}
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(54), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
						xp1 + (int)y->Xadd, yp1 + y_r(20), Green, Green25, Name);
				}
			}
			//
			{
				xp1 = y_r(32);
				yp1 = y_r(1080 - 32 - 32);

				std::string Info = "";
				switch (select) {
				case 0:
					Info = "3分間の間敵を倒し続けてください。累計撃墜数はこちらでのみカウントされます";
					break;
				case 1:
					Info = "時間制限の緩いモードです";
					break;
				case 2:
					Info = "オプションを開きます";
					break;
				default:
					break;
				}
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Black, Info);
			}
		}
	};
};
