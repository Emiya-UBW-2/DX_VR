#include	"TitleScene.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void TitleScene::Load_Sub(void) noexcept {
			ObjectManager::Instance()->LoadModelBefore("data/Charactor/Main_Movie/");
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);

			Charas::GunAnimManager::Create();
			Charas::GunAnimManager::Instance()->Load("data/CharaAnime/");
			//
			{
				std::string Path = "data/gun/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
					Guns::GunPartsDataManager::Instance()->Add(ChildPath);
				}
			}
			//
			{
				std::string Path = "data/Mods/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
					Guns::GunPartsDataManager::Instance()->Add(ChildPath);
				}
			}
		}
		void			TitleScene::LoadEnd_Sub(void) noexcept {
			//
			this->m_GunPtr.clear();
			this->m_GunPtr.reserve(12);
			{
				std::string Path = "data/gun/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					this->m_GunPtr.emplace_back(std::make_shared<Guns::GunObj>());
					ObjectManager::Instance()->InitObject(this->m_GunPtr.back(), ChildPath);
					this->m_GunPtr.back()->SetupGun();
					this->m_GunPtr.back()->SetPlayerID(InvalidID);

					auto& mod = this->m_GunPtr.back()->GetModifySlot();
					std::string PresetPath = "Save/";
					PresetPath += mod->GetMyData()->GetName();
					PresetPath += ".dat";
					if (IsFileExist(PresetPath.c_str())) {
						this->m_GunPtr.back()->GetGunsModify()->LoadSlots(PresetPath.c_str());
					}
				}
			}
		}
		void			TitleScene::Set_Sub(void) noexcept {
			EffectSingleton::Create();
			//空
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int loop = 0, num = this->m_ObjSky.GetMaterialNum(); loop < num; ++loop) {
				//this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
				this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.7f, 0.7f, 0.7f, 1.0f));
				this->m_ObjSky.SetMaterialAmbColor(loop, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
			}
			int X1 = 0;
			int X2 = 0;
			int X3 = 0;

			this->m_GunPoint.resize(this->m_GunPtr.size());
			this->m_GunAnimTimer.resize(this->m_GunPtr.size());
			for (auto& g : this->m_GunPtr) {
				int index = static_cast<int>(&g - &this->m_GunPtr.front());
				this->m_GunAnimTimer.at(index) = 0.f;
				if (g->GetModifySlot()->GetMyData()->IsPlayableWeapon() != InvalidID) {
					this->m_GunPoint.at(index).second = g->GetModifySlot()->GetMyData()->IsPlayableWeapon();
					switch (this->m_GunPoint.at(index).second) {
					case 0:
						this->m_GunPoint.at(index).first = X1;
						X1++;
						break;
					case 1:
						this->m_GunPoint.at(index).first = X2;
						X2++;
						break;
					case 2:
						this->m_GunPoint.at(index).first = X3;
						X3++;
						break;
					default:
						break;
					}
					g->SetGunMat(Matrix3x3DX::identity(), Vector3DX::vget(2.45f + 0.15f * this->m_GunPoint.at(index).first, 1.f, 1.5f * this->m_GunPoint.at(index).second) * Scale3DRate);
				}
				else {
					g->SetActive(false);
				}
			}
			//
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			// 
			PostPassParts->SetShadowScale(0.5f);
			//
			Vector3DX LightVec = Vector3DX::vget(0.05f, -0.2f, -0.15f); LightVec = LightVec.normalized();
			PostPassParts->SetAmbientLight(LightVec);

			SetLightEnable(false);

			auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
			SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f, 0.9f, 0.8f, 1.0f));
			SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.9f, 0.8f, 1.0f));

			PostPassParts->SetGodRayPer(0.5f);

			//Fog
			SetVerticalFogEnable(false);
			//Fog
			SetFogEnable(false);

			FadeControl::Instance()->Init();
			this->m_IsEnd = false;
			this->m_TitleImage.Load("data/UI/Title.png");
			// 
			ButtonParts->ResetSelect();
			// 
			ButtonParts->AddStringButton("Start Game", 52, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 3, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Customize", 48, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, BaseScreenWidth - 96 - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, BaseScreenWidth - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			// クレジット
			this->m_CreditControl = std::make_unique<UIs::CreditControl>();
			// 
			//*
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_LOOP, true);
			// */
			m_MovieCharacter = std::make_shared<Charas::MovieCharacter>();
			ObjectManager::Instance()->InitObject(this->m_MovieCharacter, "data/Charactor/Main_Movie/");

			Vector3DX BaseCamPos = m_MovieCharacter->GetMove().GetPos() + Vector3DX::vget(0.5f, 0.8f, 0.f) * Scale3DRate;
			CamPos = BaseCamPos + Vector3DX::vget(0.8f, -0.25f, 2.f) * Scale3DRate;
			CamVec = BaseCamPos;
			CamFov = deg2rad(35);
			//Cam
			CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, Vector3DX::up());
			//info
			CameraParts->SetMainCamera().SetCamInfo(CamFov, Scale3DRate * 0.03f, Scale3DRate * 100.0f);
			//
			{
				int Now = 1;
				for (auto& sel : m_GunSelect) {
					for (auto& guns : FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList) {
						int index = static_cast<int>(&guns - &FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList.front());
						if (SaveData::Instance()->GetParam(guns) == Now) {
							sel = index;
							break;
						}
					}
					++Now;
				}
			}
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* Pad = PadControl::Instance();
			auto* SE = SoundPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (SceneParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			auto* KeyGuideParts = KeyGuide::Instance();
			KeyGuideParts->ChangeGuide(
				[this]() {
					auto* LocalizeParts = LocalizePool::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					if (!m_IsCustomize) {
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
					}
					else {

					}
				}
			);
			if (!m_IsCustomize) {
				if (!PopUpParts->IsActivePop() && FadeControl::Instance()->IsClear()) {
					ButtonParts->UpdateInput();
					// 選択時の挙動
					if (ButtonParts->GetTriggerButton()) {
						switch (ButtonParts->GetSelect()) {
						case 0:
							if (!this->m_IsEnd) {
								FadeControl::Instance()->SetBlackOut(true);
							}
							this->m_IsEnd = true;
							break;
						case 1:
							m_IsCustomize = true;
							KeyGuideParts->SetGuideFlip();
							break;
						case 2:
							OptionPopup::Instance()->SetActive();
							break;
						case 3:
							PopUpParts->Add(LocalizeParts->Get(120), (720), (840),
								[&](int xmin, int ymin, int xmax, int, bool) {
									this->m_CreditControl->Draw(xmin, ymin, xmax);
								},
								[]() {},
								[]() {},
								true
							);
							break;
						default:
							break;
						}
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, true);
					}
				}
				// 
				ButtonParts->Update();

				Vector3DX BaseCamPos = m_MovieCharacter->GetMove().GetPos() + Vector3DX::vget(0.5f, 0.8f, 0.f) * Scale3DRate;
				Easing(&CamPos, BaseCamPos + Vector3DX::vget(0.8f, -0.25f, 2.f) * Scale3DRate, 0.9f, EasingType::OutExpo);
				Easing(&CamVec, BaseCamPos, 0.9f, EasingType::OutExpo);
				Easing(&CamFov, deg2rad(35), 0.9f, EasingType::OutExpo);

				m_GunTypeSel = 0;
				CamYrad = deg2rad(0);
				m_CamTimer = 0.f;

				m_MovieCharacter->SetActive(true);
				m_IsCustomizeGun = false;
				SlotSel = Guns::GunSlot::Magazine;
			}
			else {
				if (!m_IsCustomizeGun) {
					if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
						m_IsCustomize = false;
						KeyGuideParts->SetGuideFlip();
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().trigger()) {
						int Next = this->m_GunPoint.at(m_GunSelect.at(m_GunTypeSel)).first - 1;
						int Slot = this->m_GunPoint.at(m_GunSelect.at(m_GunTypeSel)).second;
						int Highest = -1;
						int HighestID = -1;
						for (auto& gp : this->m_GunPoint) {
							int index = static_cast<int>(&gp - &this->m_GunPoint.front());
							if (Slot == gp.second) {
								if (Next >= 0) {
									if (Next == gp.first) {
										m_GunSelect.at(m_GunTypeSel) = index;
										break;
									}
								}
								else {
									//一番IDがでかいやつを保持しておく
									if (Highest < gp.first) {
										Highest = gp.first;
										HighestID = index;
									}
								}
							}
						}
						if (HighestID != -1) {
							m_GunSelect.at(m_GunTypeSel) = HighestID;
						}
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().trigger()) {
						int Next = this->m_GunPoint.at(m_GunSelect.at(m_GunTypeSel)).first + 1;
						int Slot = this->m_GunPoint.at(m_GunSelect.at(m_GunTypeSel)).second;
						int Lowest = 10000;
						int LowestID = -1;
						for (auto& gp : this->m_GunPoint) {
							int index = static_cast<int>(&gp - &this->m_GunPoint.front());
							if (Slot == gp.second) {
								if (Next == gp.first) {
									m_GunSelect.at(m_GunTypeSel) = index;
									LowestID = -1;
									break;
								}
								//一番IDがでかいやつを保持しておく
								if (Lowest > gp.first) {
									Lowest = gp.first;
									LowestID = index;
								}
							}
						}
						if (LowestID != -1) {
							m_GunSelect.at(m_GunTypeSel) = LowestID;
						}
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().trigger()) {
						m_GunTypeSel++;
						if (m_GunTypeSel > 3 - 1) {
							m_GunTypeSel = 0;
						}
						m_CamTimer = 0.f;
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().trigger()) {
						m_GunTypeSel--;
						if (m_GunTypeSel < 0) {
							m_GunTypeSel = 3 - 1;
						}
						m_CamTimer = 0.f;
					}
					if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
						m_IsCustomizeGun = true;
						KeyGuideParts->SetGuideFlip();

						auto& mod = this->m_GunPtr.at(m_GunSelect.at(m_GunTypeSel))->GetModifySlot();
						auto& List = mod->GetMyData()->GetSlotInfo(Guns::GunSlot::Magazine)->CanAttachItemsUniqueID;
						int Now = mod->GetParts(Guns::GunSlot::Magazine)->GetModifySlot()->GetMyData()->GetUniqueID();
						for (auto& l : List) {
							int index = static_cast<int>(&l - &List.front());
							if (l == Now) {
								m_GunCustomSel = index;
							}
						}
					}
				}
				else {
					auto& guns = this->m_GunPtr.at(m_GunSelect.at(m_GunTypeSel));
					auto& mod = guns->GetGunsModify();
					auto& slot = guns->GetModifySlot();

					if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
						m_IsCustomizeGun = false;
						KeyGuideParts->SetGuideFlip();

						std::string PresetPath = "Save/";
						PresetPath += slot->GetMyData()->GetName();
						PresetPath += ".dat";
						mod->SaveSlots(PresetPath.c_str());
					}

					if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().trigger()) {
						switch (SlotSel)						{
						case FPS_n2::Guns::GunSlot::Magazine:
							SlotSel = Guns::GunSlot::Sight;
							break;
						case FPS_n2::Guns::GunSlot::Sight:
							SlotSel = Guns::GunSlot::Magazine;
							break;
						default:
							break;
						}
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().trigger()) {
						switch (SlotSel) {
						case FPS_n2::Guns::GunSlot::Magazine:
							SlotSel = Guns::GunSlot::Sight;
							break;
						case FPS_n2::Guns::GunSlot::Sight:
							SlotSel = Guns::GunSlot::Magazine;
							break;
						default:
							break;
						}
					}
					if (slot->GetMyData()->GetSlotInfo(SlotSel)) {
						auto& List = slot->GetMyData()->GetSlotInfo(SlotSel)->CanAttachItemsUniqueID;
						int size = static_cast<int>(List.size());
						if (!slot->GetMyData()->GetSlotInfo(SlotSel)->IsNeed) {
							size++;
						}
						if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().trigger()) {
							--m_GunCustomSel;
							if (m_GunCustomSel < 0) {
								m_GunCustomSel = size - 1;
							}
							mod->ChangeSelectData(*mod->GetSlotData(SlotSel), m_GunCustomSel);
							guns->SetupGun();
						}
						if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().trigger()) {
							++m_GunCustomSel;
							if (m_GunCustomSel > size - 1) {
								m_GunCustomSel = 0;
							}
							mod->ChangeSelectData(*mod->GetSlotData(SlotSel), m_GunCustomSel);
							guns->SetupGun();
						}
					}
				}
				m_CamTimer = std::clamp(m_CamTimer + DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				CamYrad += deg2rad(Pad->GetLS_X());
				//
				Vector3DX BaseCamPos = Vector3DX::vget(2.45f, 2.5f, 1.5f * m_GunTypeSel) * Scale3DRate;
				Easing(&CamYradR, CamYrad, 0.9f, EasingType::OutExpo);

				CamPos = Lerp(CamPos,
					BaseCamPos + Matrix3x3DX::Vtrans(Vector3DX::vget(0.0f, 0.1f, -2.f) * Scale3DRate, Matrix3x3DX::RotAxis(Vector3DX::up(), CamYradR)),
					m_CamTimer);
				Easing(&CamVec, BaseCamPos, 0.9f, EasingType::OutExpo);
				Easing(&CamFov, deg2rad(25), 0.9f, EasingType::OutExpo);

				m_MovieCharacter->SetActive(false);
			}
			for (auto& g : this->m_GunAnimTimer) {
				int index = static_cast<int>(&g - &this->m_GunAnimTimer.front());
				if (m_IsCustomize && index == m_GunSelect.at(m_GunTypeSel)) {
					g = std::clamp(g + DXLib_refParts->GetDeltaTime() / 0.25f, 0.f, 1.f);
				}
				else {
					g = std::clamp(g - DXLib_refParts->GetDeltaTime() / 0.25f, 0.f, 1.f);
				}
				auto& guns = this->m_GunPtr.at(index);
				guns->SetActive(g > 0.5f);

				Matrix3x3DX Rot = Lerp(Matrix3x3DX::identity(), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(90)), g);

				float Ofs = 0.f;
				switch (this->m_GunPoint.at(index).second) {
				case 0:
					Ofs = 0.25f;
					break;
				case 1:
					Ofs = 0.125f;
					break;
				case 2:
					Ofs = 0.0f;
					break;
				default:
					break;
				}

				Vector3DX Pos = Lerp(
					Vector3DX::vget(2.45f + 0.15f * this->m_GunPoint.at(index).first, 1.f, 1.5f * this->m_GunPoint.at(index).second) * Scale3DRate,
					Vector3DX::vget(2.45f + Ofs, 2.5f, 1.5f * this->m_GunPoint.at(index).second) * Scale3DRate,
					g * g
				);
				this->m_GunPtr.at(index)->SetGunMat(Rot, Pos);
			}

			//Update
			ObjMngr->UpdateObject();
			ObjMngr->LateUpdateObject();
			//視点
			{
				//カメラ
				CameraParts->SetMainCamera().SetCamPos(
					CamPos,
					CamVec,
					Vector3DX::up());
				CameraParts->SetMainCamera().SetCamInfo(CamFov, CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar());
			}
			// 
			FadeControl::Instance()->Update();
			EffectSingleton::Instance()->Update();

			if (this->m_IsEnd && FadeControl::Instance()->IsAll()) {
				return false;
			}
			return true;
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveData::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			//
			for (auto& guns : FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList) {
				SaveData::Instance()->SetParam(guns, 0);
				int index = static_cast<int>(&guns - &FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList.front());
				int Now = 1;
				for (auto& sel : m_GunSelect) {
					if (sel == index) {
						SaveData::Instance()->SetParam(guns, Now);
					}
					++Now;
				}
			}
			//
			m_MovieCharacter.reset();
			//*
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::BGM, 0)->StopAll();
			//*/
			// 
			this->m_CreditControl.reset();
			// 
			ButtonParts->Dispose();
			// 
			this->m_TitleImage.Dispose();
			// セーブ
			SaveDataParts->Save();
			// 次シーン決定
			switch (ButtonParts->GetSelect()) {
			case 0:
				SetNextSelect(0);
				break;
			default:
				break;
			}
		}
		void TitleScene::Dispose_Load_Sub(void) noexcept {
			m_GunPtr.clear();
			ObjectManager::Instance()->DeleteAll();
			this->m_ObjSky.Dispose();

			EffectSingleton::Release();
			Charas::GunAnimManager::Release();
		}
		void TitleScene::BG_Draw_Sub(void) const noexcept {
			FillGraph(GetDrawScreen(), 0, 0, 0);

			auto Fog = GetFogEnable();
			auto VFog = GetVerticalFogEnable();
			SetFogEnable(false);
			SetVerticalFogEnable(false);
			SetUseLighting(false);
			this->m_ObjSky.DrawModel();
			SetUseLighting(true);
			SetFogEnable(Fog);
			SetVerticalFogEnable(VFog);
		}
		void TitleScene::MainDraw_Sub(int Range) const noexcept {
			ObjectManager::Instance()->Draw(true, Range);
		}
		// 

		// オンオフできるボタン
		static bool CheckBox(int xp1, int yp1, bool switchturn) noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			int xp3 = xp1 + EdgeSize;
			int yp3 = yp1 + EdgeSize;
			int xp4 = xp1 + LineHeight * 2 - EdgeSize;
			int yp4 = yp1 + LineHeight - EdgeSize;

			auto* Pad = PadControl::Instance();
			bool MouseOver = IntoMouse(xp3, yp3, xp4, yp4);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				switchturn ^= 1;
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			int Edge = (5);
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3 + Edge / 2, yp3 + Edge / 2, xp4 - Edge / 2, yp4 - Edge / 2, DarkGreen, true);

			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3 + Edge, yp3 + Edge, xp4 - Edge, yp4 - Edge, Black, true);
			xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3 + Edge, yp3 + Edge, xp4 + Edge, yp4 - Edge, Gray50, true);
			xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
			xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3, yp3, xp4, yp4, Green, true);
			return switchturn;
		}


		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			// 背景
			//DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Gray50, true);
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			// 
			if (!m_IsCustomize) {
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
					&this->m_TitleImage, (64), (64), (64 + 369), (64 + 207), true);
				// 
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
					(64 + 369), (64 + 207), White, Black, "Ver 1.0.0");
				// 
				ButtonParts->Draw();
				// 
				if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM,
						(32), 1080 - (32 + 32), White, Black, LocalizeParts->Get(9020 + ButtonParts->GetSelect()));
				}
				//
				{
					int xp = 64;
					int yp = 1080 - 92;
					auto* OptionParts = OptionManager::Instance();
					{
						auto prev = OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth);
						OptionParts->SetParamBoolean(EnumSaveParam::FlatEarth, CheckBox(xp, yp, OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)));
						if (prev != OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)) {
							OptionParts->Save();
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp + 64, yp + LineHeight / 2,
							White, Black, LocalizeParts->Get(1139));
					}
					{
						yp -= LineHeight + 12;

						auto prev = OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn);
						OptionParts->SetParamBoolean(EnumSaveParam::ActiveLockOn, CheckBox(xp, yp, OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn)));
						if (prev != OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn)) {
							OptionParts->Save();
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp + 64, yp + LineHeight / 2,
							White, Black, LocalizeParts->Get(1146));
					}
				}
			}
			else {
				int XPos = 64;
				{
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
						XPos, (48), Green, Black, "Equip Customize");
					XPos += FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, 64, 3)->GetStringWidth("Equip Customize");
				}
				if (m_IsCustomizeGun) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
						XPos, (48), Green, Black, " > Gun Setup");
					XPos += FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, 64, 3)->GetStringWidth(" > Gun Setup");

					std::string CustomPoint = "";
					switch (SlotSel) {
					case FPS_n2::Guns::GunSlot::Magazine:
						CustomPoint = "Magazine";
						break;
					case FPS_n2::Guns::GunSlot::Sight:
						CustomPoint = "Sight";
						break;
					default:
						break;
					}
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
						XPos, (48), Green, Black, (" > " + CustomPoint).c_str());
					XPos += FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, 64, 3)->GetStringWidth((" > " + CustomPoint).c_str());
				}

			}
			// 
			FadeControl::Instance()->Draw();
		}
	}
}
