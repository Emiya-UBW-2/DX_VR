#include	"TitleScene.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Load_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->LoadModelBefore("data/Charactor/Main_Movie/");
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);

			//
			for (auto& name : Guns::GunPartsDataManager::Instance()->m_GunList) {
				std::string ChildPath = "data/gun/";
				ChildPath += name;
				ChildPath += "/";
				ObjMngr->LoadModelBefore(ChildPath);
			}
			for (auto& Path : Guns::GunPartsDataManager::Instance()->m_ModPathList) {
				ObjMngr->LoadModelBefore(Path);
			}
			this->m_TitleImage.Load("data/UI/Title.png");
		}
		void			TitleScene::LoadEnd_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			//
			this->m_GunPtr.clear();
			this->m_GunPtr.reserve(12);
			for (auto& name : Guns::GunPartsDataManager::Instance()->m_GunList) {
				std::string ChildPath = "data/gun/";
				ChildPath += name;
				ChildPath += "/";
				this->m_GunPtr.emplace_back(std::make_shared<Guns::GunObj>());
				ObjMngr->InitObject(this->m_GunPtr.back(), ChildPath);
				this->m_GunPtr.back()->SetPlayerID(InvalidID);

				std::string PresetPath = "Save/";
				PresetPath += this->m_GunPtr.back()->GetModifySlot()->GetMyData()->GetName();
				PresetPath += ".dat";
				if (IsFileExist(PresetPath.c_str())) {
					this->m_GunPtr.back()->GetGunsModify()->LoadSlots(PresetPath.c_str());
				}
			}
		}
		void			TitleScene::Set_Sub(void) noexcept {
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* SE = SoundPool::Instance();
			auto* SideLogParts = SideLog::Instance();
			auto* SaveDataParts = SaveData::Instance();
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

			int HighScore = static_cast<int>(SaveDataParts->GetParam("HighScore"));

			this->m_GunPoint.resize(this->m_GunPtr.size());
			for (auto& guns : this->m_GunPtr) {
				int index = static_cast<int>(&guns - &this->m_GunPtr.front());
				auto& slot = guns->GetModifySlot();
				auto& slotdata = slot->GetMyData();
				auto& GP = this->m_GunPoint.at(index);
				int NeedScore = slotdata->GetUnlockScore();
				GP.IsActive = ((slotdata->IsPlayableWeapon() != InvalidID) && (HighScore >= NeedScore));
				if (GP.IsActive) {
					if (this->m_PrevHighScore != InvalidID && NeedScore > this->m_PrevHighScore) {
						SideLogParts->Add(5.0f, 0.0f, Green, ("Unlock : " + slotdata->GetName()).c_str());
					}
					GP.GunType = slotdata->IsPlayableWeapon();
					switch (GP.GunType) {
					case 0:
						GP.Xofs = X1;
						X1++;
						break;
					case 1:
						GP.Xofs = X2;
						X2++;
						break;
					case 2:
						GP.Xofs = X3;
						X3++;
						break;
					default:
						break;
					}
					guns->SetGunMat(Matrix3x3DX::identity(), Vector3DX::vget(2.45f + 0.15f * GP.Xofs, 1.f, 1.5f * GP.GunType) * Scale3DRate);
				}
				else {
					guns->SetActive(false);
				}
			}
			for (auto& guns : this->m_GunPtr) {
				guns->SetupGun();
			}

			this->m_PrevHighScore = HighScore;
			// 
			PostPassParts->SetShadowScale(0.5f);
			//
			Vector3DX LightVec = Vector3DX::vget(0.05f, -0.2f, -0.15f); LightVec = LightVec.normalized();
			PostPassParts->SetAmbientLight(LightVec);

			SetLightEnable(false);

			auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
			SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f, 0.9f, 0.8f, 1.0f));
			SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.9f, 0.8f, 1.0f));

			PostPassParts->SetGodRayPer(0.1f);
			//Fog
			SetVerticalFogEnable(false);
			SetFogEnable(false);

			FadeControl::Instance()->Init();
			this->m_IsEnd = false;
			// 
			ButtonParts->ResetSelect();
			// 
			ButtonParts->AddStringButton("Start Game", 52, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 3, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Customize", 48, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Tutorial", 48, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, BaseScreenWidth - 96 - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, BaseScreenWidth - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			// クレジット
			this->m_CreditControl = std::make_unique<UIs::CreditControl>();
			// 
			SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_LOOP, true);
			//
			this->m_MovieCharacter = std::make_shared<Charas::MovieCharacter>();
			ObjectManager::Instance()->InitObject(this->m_MovieCharacter, "data/Charactor/Main_Movie/");

			Vector3DX BaseCamPos = this->m_MovieCharacter->GetMove().GetPos() + Vector3DX::vget(0.5f, 0.8f, 0.f) * Scale3DRate;
			this->m_CamPos = BaseCamPos + Vector3DX::vget(0.8f, -0.25f, 2.f) * Scale3DRate;
			this->m_CamVec = BaseCamPos;
			this->m_CamFov = deg2rad(35);
			//Cam
			CameraParts->SetMainCamera().SetCamPos(this->m_CamPos, this->m_CamVec, Vector3DX::up());
			CameraParts->SetMainCamera().SetCamInfo(this->m_CamFov, Scale3DRate * 0.03f, Scale3DRate * 100.0f);
			//
			for (auto& sel : this->m_GunSelect) {
				int index = static_cast<int>(&sel - &this->m_GunSelect.front());
				for (auto& guns : Guns::GunPartsDataManager::Instance()->m_GunList) {
					int index2 = static_cast<int>(&guns - &Guns::GunPartsDataManager::Instance()->m_GunList.front());
					if (SaveDataParts->GetParam(guns) == (index + 1)) {
						sel = index2;
						break;
					}
				}
			}
			this->m_TitleWindow = TitleWindow::Main;

			//死んだと判断した時(スコアが1以上から0になったら)
			this->m_EndScoreDisp = Player::ResultData::Instance()->m_SetData;
			Player::ResultData::Instance()->m_SetData = false;
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
			auto* KeyGuideParts = KeyGuide::Instance();
			if (SceneParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			KeyGuideParts->ChangeGuide(
				[this]() {
					auto* LocalizeParts = LocalizePool::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					switch (this->m_TitleWindow) {
					case TitleWindow::Main:
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
						break;
					case TitleWindow::Custom:
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), LocalizeParts->Get(9980));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), LocalizeParts->Get(9981));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9982));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizeParts->Get(9991));
						break;
					case TitleWindow::CustomizeGun:
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), LocalizeParts->Get(9983));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), LocalizeParts->Get(9984));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizeParts->Get(9991));
						break;
					default:
						break;
					}
				}
			);

			switch (this->m_TitleWindow) {
			case TitleWindow::Main:
			{
				if (this->m_EndScoreDisp && FadeControl::Instance()->IsClear()) {
					this->m_EndScoreDisp = false;
					PopUpParts->Add(LocalizeParts->Get(130), (720), (720 * 3 / 4),
						[&](int xmin, int ymin, int xmax, int, bool) {
							auto* DrawCtrls = WindowSystem::DrawControl::Instance();
							auto* SaveDataParts = SaveData::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							int yp1 = ymin + LineHeight + LineHeight / 2;


							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, 48,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xmin + 24 + 6, yp1, White, Black, "Result");
							yp1 += 48 + 24;

							int HighRound = static_cast<int>(SaveDataParts->GetParam("HighRound"));
							bool IsRoundHigh = (Player::ResultData::Instance()->m_Round >= HighRound);
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, 32,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xmin + 24 + 6, yp1, White, Black, "Round : ");
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, 32,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xmax - 24 - 6, yp1, IsRoundHigh ? Red : White, Black, "%s %05d",
								IsRoundHigh ? "High Record!" : "",
								Player::ResultData::Instance()->m_Round);
							yp1 += 32;

							int HighScore = static_cast<int>(SaveDataParts->GetParam("HighScore"));
							bool IsScoreHigh = (Player::ResultData::Instance()->m_Score >= HighScore);
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, 32,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xmin + 24 + 6, yp1, White, Black, "Score : ");
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, 32,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xmax - 24 - 6, yp1, IsScoreHigh ? Red : White, Black, "%s %05d",
								IsScoreHigh ? "High Record!" : "",
								Player::ResultData::Instance()->m_Score);
							yp1 += 32;

							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, 32,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xmin + 24 + 6, yp1, White, Black, "Skill : ");

							bool Complete = true;
							for (int loop = 0; loop < static_cast<int>(Player::SkillType::Max); ++loop) {
								int Level = 0;
								for (auto& sk : Player::ResultData::Instance()->m_SkillInfo) {
									if (loop == static_cast<int>(sk.m_SkillType)) {
										Level = sk.m_Level;
										break;
									}
								}
								if (Level != 3) {
									Complete = false;
									break;
								}
							}
							if (Complete) {
								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, 32,
									FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
									xmax - 24 - 6, yp1, Red, Black, "Complete!");
							}
							yp1 += 32;
							for (auto& sk : Player::ResultData::Instance()->m_SkillInfo) {
								std::string Title = LocalizeParts->Get(5000 + static_cast<int>(sk.m_SkillType));
								Title += " Lv.";
								Title += std::to_string(sk.m_Level);

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, 32,
									FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xmax - 24 - 6, yp1, White, Black, Title.c_str());
								yp1 += 32;
							}
						},
						[]() {},
						[]() {},
						true
						);
				}
				//
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
							this->m_TitleWindow = TitleWindow::Custom;
							KeyGuideParts->SetGuideFlip();
							break;
						case 2:
							if (!this->m_IsEnd) {
								FadeControl::Instance()->SetBlackOut(true);
							}
							this->m_IsEnd = true;
							break;
						case 3:
							OptionPopup::Instance()->SetActive();
							break;
						case 4:
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
				Vector3DX BaseCamPos = this->m_MovieCharacter->GetMove().GetPos() + Vector3DX::vget(0.5f, 0.8f, 0.f) * Scale3DRate;
				Easing(&this->m_CamPos, BaseCamPos + Vector3DX::vget(0.8f, -0.25f, 2.f) * Scale3DRate, 0.9f, EasingType::OutExpo);
				Easing(&this->m_CamVec, BaseCamPos, 0.9f, EasingType::OutExpo);
				Easing(&this->m_CamFov, deg2rad(35), 0.9f, EasingType::OutExpo);
				this->m_GunTypeSel = 0;
				this->m_CamYrad = deg2rad(0);
				this->m_CamTimer = 0.f;
				this->m_MovieCharacter->SetActive(true);
			}
			break;
			case TitleWindow::Custom:
				if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					this->m_TitleWindow = TitleWindow::Main;
					KeyGuideParts->SetGuideFlip();
				}
				if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					int Next = this->m_GunPoint.at(GetNowEditWeaponID()).Xofs - 1;
					int Slot = this->m_GunPoint.at(GetNowEditWeaponID()).GunType;
					int Highest = -1;
					int HighestID = InvalidID;
					for (auto& GP : this->m_GunPoint) {
						if (!GP.IsActive) { continue; }
						if (Slot != GP.GunType) { continue; }
						int index = static_cast<int>(&GP - &this->m_GunPoint.front());
						if (Next >= 0) {
							if (Next == GP.Xofs) {
								this->m_GunSelect.at(this->m_GunTypeSel) = index;
								break;
							}
						}
						else {
							//一番IDがでかいやつを保持しておく
							if (Highest < GP.Xofs) {
								Highest = GP.Xofs;
								HighestID = index;
							}
						}
					}
					if (HighestID != InvalidID) {
						this->m_GunSelect.at(this->m_GunTypeSel) = HighestID;
					}
				}
				if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					int Next = this->m_GunPoint.at(GetNowEditWeaponID()).Xofs + 1;
					int Slot = this->m_GunPoint.at(GetNowEditWeaponID()).GunType;
					int Lowest = 10000;
					int LowestID = InvalidID;
					for (auto& GP : this->m_GunPoint) {
						if (!GP.IsActive) { continue; }
						if (Slot != GP.GunType) { continue; }
						int index = static_cast<int>(&GP - &this->m_GunPoint.front());
						if (Next == GP.Xofs) {
							this->m_GunSelect.at(this->m_GunTypeSel) = index;
							LowestID = InvalidID;
							break;
						}
						//一番IDがでかいやつを保持しておく
						if (Lowest > GP.Xofs) {
							Lowest = GP.Xofs;
							LowestID = index;
						}
					}
					if (LowestID != InvalidID) {
						this->m_GunSelect.at(this->m_GunTypeSel) = LowestID;
					}
				}
				if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					this->m_GunTypeSel++;
					if (this->m_GunTypeSel > 3 - 1) {
						this->m_GunTypeSel = 0;
					}
					this->m_CamTimer = 0.f;
				}
				if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					this->m_GunTypeSel--;
					if (this->m_GunTypeSel < 0) {
						this->m_GunTypeSel = 3 - 1;
					}
					this->m_CamTimer = 0.f;
				}
				if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					auto& slot = this->m_GunPtr.at(GetNowEditWeaponID())->GetModifySlot();
					auto& slotdata = slot->GetMyData();
					if (slotdata->GetIsCustomize()) {
						this->m_TitleWindow = TitleWindow::CustomizeGun;
						KeyGuideParts->SetGuideFlip();

						this->m_SlotSel = Guns::GunSlot::Magazine;
						auto& List = slotdata->GetSlotInfo(this->m_SlotSel)->CanAttachItemsUniqueID;
						int Now = slot->GetParts(this->m_SlotSel)->GetModifySlot()->GetMyData()->GetUniqueID();
						for (auto& l : List) {
							int index = static_cast<int>(&l - &List.front());
							if (l == Now) {
								this->m_GunCustomSel = index;
							}
						}
						this->m_SelAlpha = 2.f;
					}
				}
				break;
			case TitleWindow::CustomizeGun:
			{
				auto& guns = this->m_GunPtr.at(GetNowEditWeaponID());
				auto& mod = guns->GetGunsModify();
				auto& slot = guns->GetModifySlot();
				auto& slotdata = slot->GetMyData();

				if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					this->m_TitleWindow = TitleWindow::Custom;
					KeyGuideParts->SetGuideFlip();

					std::string PresetPath = "Save/";
					PresetPath += slotdata->GetName();
					PresetPath += ".dat";
					mod->SaveSlots(PresetPath.c_str());
				}

				if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					switch (this->m_SlotSel) {
					case Guns::GunSlot::Magazine:
						this->m_SlotSel = Guns::GunSlot::Sight;
						break;
					case Guns::GunSlot::Sight:
						this->m_SlotSel = Guns::GunSlot::Magazine;
						break;
					default:
						break;
					}
					this->m_SelAlpha = 2.f;
				}
				if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
					switch (this->m_SlotSel) {
					case Guns::GunSlot::Magazine:
						this->m_SlotSel = Guns::GunSlot::Sight;
						break;
					case Guns::GunSlot::Sight:
						this->m_SlotSel = Guns::GunSlot::Magazine;
						break;
					default:
						break;
					}
					this->m_SelAlpha = 2.f;
				}
				if (slotdata->GetSlotInfo(this->m_SlotSel)) {
					int size = static_cast<int>(slotdata->GetSlotInfo(this->m_SlotSel)->CanAttachItemsUniqueID.size());
					if (!slotdata->GetSlotInfo(this->m_SlotSel)->IsNeed) {
						size++;
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().trigger()) {
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
						--this->m_GunCustomSel;
						if (this->m_GunCustomSel < 0) {
							this->m_GunCustomSel = size - 1;
						}
						mod->ChangeSelectData(*mod->GetSlotData(this->m_SlotSel), this->m_GunCustomSel);
						guns->SetupGun();
						this->m_SelAlpha = 2.f;
					}
					if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().trigger()) {
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
						++this->m_GunCustomSel;
						if (this->m_GunCustomSel > size - 1) {
							this->m_GunCustomSel = 0;
						}
						mod->ChangeSelectData(*mod->GetSlotData(this->m_SlotSel), this->m_GunCustomSel);
						guns->SetupGun();
						this->m_SelAlpha = 2.f;
					}
				}
			}
			break;
			default:
				break;
			}


			if (this->m_TitleWindow != TitleWindow::Main) {
				this->m_SelAlpha = std::max(this->m_SelAlpha - DXLib_refParts->GetDeltaTime(), 0.f);

				this->m_CamTimer = std::clamp(this->m_CamTimer + DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_CamYrad += deg2rad(Pad->GetLS_X());
				//
				Vector3DX BaseCamPos = Vector3DX::vget(2.45f, 2.5f, 0.f) * Scale3DRate;
				Easing(&this->m_CamYradR, this->m_CamYrad, 0.9f, EasingType::OutExpo);

				this->m_CamPos = Lerp(this->m_CamPos,
					BaseCamPos + Matrix3x3DX::Vtrans(Vector3DX::vget(0.0f, 0.1f, -2.f) * Scale3DRate, Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_CamYradR)),
					this->m_CamTimer);
				Easing(&this->m_CamVec, BaseCamPos, 0.9f, EasingType::OutExpo);
				Easing(&this->m_CamFov, deg2rad(25), 0.9f, EasingType::OutExpo);

				this->m_MovieCharacter->SetActive(false);
			}

			for (auto& GP : this->m_GunPoint) {
				int index = static_cast<int>(&GP - &this->m_GunPoint.front());
				auto& guns = this->m_GunPtr.at(index);
				if ((this->m_TitleWindow != TitleWindow::Main) && index == GetNowEditWeaponID()) {
					GP.AnimTimer = std::clamp(GP.AnimTimer + DXLib_refParts->GetDeltaTime() / 0.25f, 0.f, 1.f);
				}
				else {
					GP.AnimTimer = std::clamp(GP.AnimTimer - DXLib_refParts->GetDeltaTime() / 0.25f, 0.f, 1.f);
				}
				guns->SetActive(GP.AnimTimer > 0.5f);

				Matrix3x3DX Rot = Lerp(Matrix3x3DX::identity(), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(90)), GP.AnimTimer);

				Vector3DX Pos = Lerp(
					Vector3DX::vget(2.45f + 0.15f * GP.Xofs, 1.f, 1.5f * GP.GunType) * Scale3DRate,
					Vector3DX::vget(2.45f + 0.125f * (2 - this->m_GunTypeSel), 2.5f, 0.f) * Scale3DRate,
					GP.AnimTimer * GP.AnimTimer
				);
				guns->SetGunMat(Rot, Pos);
			}

			//Update
			if (!GetIsFirstLoop()) {
				ObjMngr->UpdateObject();
				ObjMngr->LateUpdateObject();
			}
			//視点
			CameraParts->SetMainCamera().SetCamPos(this->m_CamPos, this->m_CamVec, Vector3DX::up());
			CameraParts->SetMainCamera().SetCamInfo(this->m_CamFov, CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar());
			// 
			FadeControl::Instance()->Update();
			EffectSingleton::Instance()->Update();
			//
			if (this->m_IsEnd && FadeControl::Instance()->IsAll()) {
				return false;
			}
			return true;
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveData::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* SE = SoundPool::Instance();
			// セーブ
			for (auto& guns : Guns::GunPartsDataManager::Instance()->m_GunList) {
				SaveDataParts->SetParam(guns, 0);
				int index = static_cast<int>(&guns - &Guns::GunPartsDataManager::Instance()->m_GunList.front());
				for (auto& sel : this->m_GunSelect) {
					int index2 = static_cast<int>(&sel - &this->m_GunSelect.front());
					if (sel == index) {
						SaveDataParts->SetParam(guns, index2 + 1);
					}
				}
			}
			SaveDataParts->Save();
			//
			SE->Get(SoundType::BGM, 0)->StopAll();
			ButtonParts->Dispose();
			this->m_MovieCharacter.reset();
			this->m_CreditControl.reset();
			this->m_TitleImage.Dispose();
			// 次シーン決定
			switch (ButtonParts->GetSelect()) {
			case 0:
				SetNextSelect(0);
				break;
			case 2:
				SetNextSelect(1);
				break;
			default:
				break;
			}
		}
		void			TitleScene::Dispose_Load_Sub(void) noexcept {
			this->m_GunPtr.clear();
			ObjectManager::Instance()->DeleteAll();
			this->m_ObjSky.Dispose();
		}
		void			TitleScene::BG_Draw_Sub(void) const noexcept {
			FillGraph(GetDrawScreen(), 0, 0, 0);
			SetUseLighting(false);
			this->m_ObjSky.DrawModel();
			SetUseLighting(true);
		}
		void			TitleScene::MainDraw_Sub(int Range) const noexcept {
			ObjectManager::Instance()->Draw(true, Range);

			if (Range == 1 && this->m_SelAlpha > 0.f) {
				auto& guns = this->m_GunPtr.at(GetNowEditWeaponID());
				auto& slot = guns->GetModifySlot();
				auto& ModPtr1 = slot->GetParts(this->m_SlotSel);
				if (ModPtr1) {
					ClearDrawScreenZBuffer();
					SetUseLighting(FALSE);
					ModPtr1->GetObj().SetOpacityRate(0.5f * std::clamp(this->m_SelAlpha, 0.f, 1.f));
					ModPtr1->GetObj().SetMaterialDrawAddColorAll(-255, 255, -255);
					ModPtr1->GetObj().DrawModel();
					ModPtr1->GetObj().SetMaterialDrawAddColorAll(0, 0, 0);
					ModPtr1->GetObj().SetOpacityRate(1.f);
					SetUseLighting(TRUE);
				}
			}
		}
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
		// 
		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* SaveDataParts = SaveData::Instance();
			// 
			switch (this->m_TitleWindow) {
			case TitleWindow::Main:
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
					&this->m_TitleImage, 64, 64, 64 + 369, 64 + 207, true);
				// 
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
					64 + 369, 64 + 207, White, Black, "Ver 1.0.0");
				// 
				ButtonParts->Draw();
				//
				{
					int xp = 64;
					int yp = 1080 - 92;
					{
						auto prev = OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth);
						OptionParts->SetParamBoolean(EnumSaveParam::FlatEarth, CheckBox(xp, yp, OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)));
						if (prev != OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)) {
							OptionParts->Save();
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp + 64, yp + LineHeight / 2,
							White, Black, LocalizeParts->Get(1139));
						yp -= LineHeight + 12;
					}
					{
						auto prev = OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn);
						OptionParts->SetParamBoolean(EnumSaveParam::ActiveLockOn, CheckBox(xp, yp, OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn)));
						if (prev != OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn)) {
							OptionParts->Save();
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp + 64, yp + LineHeight / 2,
							White, Black, LocalizeParts->Get(1146));
						yp -= LineHeight + 12;
					}
					{
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp + 64, yp + LineHeight / 2,
							White, Black, "Press F1 Display ItemInfo");
						yp -= LineHeight + 12;
					}
				}
				//
				{
					int xp = 1920 - 48;
					int yp = 128;

					bool IsRoundHigh = SaveDataParts->GetParam("round") == SaveDataParts->GetParam("HighRound");
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp - 200, yp, Green, Black, "Round : ");
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp, yp, IsRoundHigh ? Red : Green, Black, "%05d", std::max(static_cast<int>(SaveDataParts->GetParam("round")), 0));
					yp += 32;

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (12),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp, yp, IsRoundHigh ? Red : Green, Black, "/ %05d", std::max(static_cast<int>(SaveDataParts->GetParam("HighRound")), 0));
					yp += 12 + 4;

					bool IsScoreHigh = SaveDataParts->GetParam("score") == SaveDataParts->GetParam("HighScore");
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp - 200, yp, Green, Black, "Score : ");

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp, yp, IsScoreHigh ? Red : Green, Black, "%05d", std::max(static_cast<int>(SaveDataParts->GetParam("score")), 0));
					yp += 32;

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (12),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp, yp, IsScoreHigh ? Red : Green, Black, "/ %05d", std::max(static_cast<int>(SaveDataParts->GetParam("HighScore")), 0));
					yp += 12 + 4;

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
						xp - 200, yp, Green, Black, "Skill : ");
					bool Complete = true;
					for (int loop = 0; loop < static_cast<int>(Player::SkillType::Max); ++loop) {
						int Level = static_cast<int>(SaveDataParts->GetParam("skill" + std::to_string(loop)));
						if (Level != 3) {
							Complete = false;
							break;
						}
					}
					if (Complete) {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
							xp, yp, Red, Black, "Complete!");
					}
					yp += 32;
					for (int LV = 3; LV >= 1; --LV) {
						for (int loop = 0; loop < static_cast<int>(Player::SkillType::Max); ++loop) {
							int Level = static_cast<int>(SaveDataParts->GetParam("skill" + std::to_string(loop)));
							if (Level > 0 && LV == Level) {
								std::string Title = LocalizeParts->Get(5000 + loop);
								Title += " Lv.";
								Title += std::to_string(Level);

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
									FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
									xp, yp, (LV == 3) ? Yellow : Green, Black, Title);
								yp += 32;
							}
						}
					}
				}
				//
				if (CheckHitKey(KEY_INPUT_F1) != 0) {
					int xp1 = 128;
					int yp1 = 128;

					xp1 += 64;
					for (int loop = 0; loop < Objects::ItemObjDataManager::Instance()->GetList().size(); ++loop) {
						auto& item = Objects::ItemObjDataManager::Instance()->Get(loop);
						int PrevY = yp1;
						{
							xp1 -= 64;
							yp1 += 64;
							DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - 64, yp1 - 64, xp1 + 64, yp1 + 64, Black, true);
							DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - 64, yp1 - 64, xp1 + 64, yp1 + 64, Green, false, 3);
							DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &item->GetIconGraph(), xp1, yp1, (128.f / 512.f) * 1.f, 0.f, true);
							xp1 += 64;
							yp1 -= 64;
						}
						//Info
						{
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								xp1, yp1, Green, Black, item->GetName());
							yp1 += 24;
						}
						yp1 += 24;
						for (auto& info : item->GetInfo()) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								xp1, yp1, Green, Black, info);
							yp1 += 18;
						}

						yp1 = PrevY + 144;
					}
				}
				break;
			case TitleWindow::Custom:
				break;
			case TitleWindow::CustomizeGun:
				break;
			default:
				break;
			}
			if (this->m_TitleWindow != TitleWindow::Main) {
				int XPos = 64;
				{
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
						XPos, (48), Green, Black, "Equip Customize");
					int Len = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, (48), 3)->GetStringWidth("Equip Customize");
					if (!(this->m_TitleWindow == TitleWindow::CustomizeGun)) {
						std::string CustomPoint = "";
						switch (this->m_GunTypeSel) {
						case 0:
							CustomPoint = "Main";
							break;
						case 1:
							CustomPoint = "Sub";
							break;
						case 2:
							CustomPoint = "Gadget";
							break;
						default:
							break;
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP,
							XPos + Len / 2, (48) + 48, Green, Black, CustomPoint.c_str());
					}
					XPos += Len;
				}
				if ((this->m_TitleWindow == TitleWindow::CustomizeGun)) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
						XPos, (48), Green, Black, " > Gun Setup");

					int Len = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, (48), 3)->GetStringWidth(" > Gun Setup");
					{
						std::string CustomPoint = "";
						switch (this->m_SlotSel) {
						case Guns::GunSlot::Magazine:
							CustomPoint = "Magazine";
							break;
						case Guns::GunSlot::Sight:
							CustomPoint = "Sight";
							break;
						default:
							break;
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP,
							XPos + Len / 2, (48) + 48, Green, Black, CustomPoint.c_str());
					}
					XPos += Len;
				}
				{
					auto& guns = this->m_GunPtr.at(GetNowEditWeaponID());
					auto& slot = guns->GetModifySlot();
					auto& slotdata = slot->GetMyData();

					int xp1 = 128;
					int yp1 = 128;
					int xp2 = 512;
					//int yp2 = 960;
					const GraphHandle* Ptr = nullptr;
					if (slotdata->GetIconGraph().IsActive()) {
						Ptr = &slotdata->GetIconGraph();
					}
					if (Ptr) {
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
							Ptr, (xp1 + xp2) / 2, yp1 + 120, 0.5f, 0.f, true);
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						if (!(this->m_TitleWindow == TitleWindow::CustomizeGun)) {
							int xp3 = xp1 - 64;
							int yp3 = yp1 + 120;
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3, yp3 - 10, xp3, yp3 + 10, Green, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3 - 5, yp3, xp3, yp3 + 10, Green, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3 - 5, yp3, xp3, yp3 - 10, Green, 2);

							xp3 = xp2 + 64;
							yp3 = yp1 + 120;
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3, yp3 - 10, xp3, yp3 + 10, Green, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3 + 5, yp3, xp3, yp3 + 10, Green, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3 + 5, yp3, xp3, yp3 - 10, Green, 2);
						}
						yp1 += 240;
					}
					int YPrev = yp1;
					//Info
					{
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							xp1, yp1, Green, Black, slotdata->GetName());
						yp1 += 24;
					}
					yp1 += 24;
					for (auto& info : slotdata->GetInfo()) {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							xp1, yp1, Green, Black, info);
						yp1 += 18;
					}
					yp1 += 24;
					{
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							xp1, yp1, Green, Black, "%05.3fkg", static_cast<float>(guns->GetWeight_gram()) / 1000.f);
						yp1 += 32;
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							xp1, yp1, Green, Black, Guns::SHOTTYPEName[static_cast<int>(slotdata->GetShotType())]);
						if (slotdata->GetShotType() == Guns::SHOTTYPE::FULL) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
								xp2, yp1, Green, Black, "%04d RPM", slotdata->GetShotRate());
						}
						yp1 += 32;
						{
							std::string LoadType = Guns::RELOADTYPEName[static_cast<int>(guns->GetReloadType())];
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								xp1, yp1, Green, Black, "%d round " + LoadType, guns->GetAmmoAll());
							yp1 += 32;
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							xp1, yp1, Green, Black, "Aim Speed %5.2f %%", guns->GetAimSpeed());
						yp1 += 32;
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
							xp1, yp1, Green, Black, "Reload Speed %5.2f %%", guns->GetReloadSpeed());
						yp1 += 32;
						if (slotdata->GetIsCustomize()) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								xp1, yp1, Green, Black, "Can Customize");
							yp1 += 32;
						}
					}
					yp1 += 24;

					//パーツInfo
					if ((this->m_TitleWindow == TitleWindow::CustomizeGun) && slot->IsAttachedParts(this->m_SlotSel)) {
						auto& partsslotdata = slot->GetParts(this->m_SlotSel)->GetModifySlot()->GetMyData();
						xp1 = 128 + 512 + 64;
						yp1 = YPrev;
						{
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								xp1, yp1, Green, Black, partsslotdata->GetName());
							yp1 += 24;
						}
						yp1 += 24;
						for (auto& info : partsslotdata->GetInfo()) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
								xp1, yp1, Green, Black, info);
							yp1 += 18;
						}
					}
				}
			}
			FadeControl::Instance()->Draw();
		}
	}
}
