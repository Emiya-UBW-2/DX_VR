#include	"TitleScene.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void TitleScene::Load_Sub(void) noexcept {
			ObjectManager::Instance()->LoadModelBefore("data/Charactor/Main_Movie/");
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
		}
		void			TitleScene::Set_Sub(void) noexcept {
			//空
			this->m_ObjSky.SetScale(Vector3DX::vget(10.0f, 10.0f, 10.0f));
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int loop = 0, num = this->m_ObjSky.GetMaterialNum(); loop < num; ++loop) {
				//this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
				this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.7f, 0.7f, 0.7f, 1.0f));
				this->m_ObjSky.SetMaterialAmbColor(loop, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
			}
			//
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			// 
			PostPassParts->SetShadowScale(0.5f);
			//
			Vector3DX LightVec = Vector3DX::vget(0.05f, -0.3f, 0.15f); LightVec = LightVec.normalized();
			PostPassParts->SetAmbientLight(LightVec);

			SetLightEnable(false);

			auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
			SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));
			SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));

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
			ButtonParts->AddStringButton("Start Game", 52, true, BaseScreenWidth - 64 - 48, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, BaseScreenWidth - 96 - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, BaseScreenWidth - 64, 64, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			// クレジット
			this->m_CreditControl = std::make_unique<UIs::CreditControl>();
			// 
			/*
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::BGM, 0)->Play(DX_PLAYTYPE_LOOP, true);
			// */
			m_MovieCharacter = std::make_shared<Charas::MovieCharacter>();
			ObjectManager::Instance()->InitObject(m_MovieCharacter, "data/Charactor/Main_Movie/");
			//Cam
			CameraParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//info
			CameraParts->SetMainCamera().SetCamInfo(deg2rad(35), Scale3DRate * 0.03f, Scale3DRate * 100.0f);
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
			if (SceneParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			auto* KeyGuideParts = KeyGuide::Instance();
			KeyGuideParts->ChangeGuide(
				[]() {
					auto* LocalizeParts = LocalizePool::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
				}
			);
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
						OptionPopup::Instance()->SetActive();
						break;
					case 2:
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
			//Update
			ObjMngr->UpdateObject();
			ObjMngr->LateUpdateObject();
			//視点
			{
				//カメラ
				Vector3DX BaseCamPos = m_MovieCharacter->GetMove().GetPos() + Vector3DX::vget(0.5f, 0.8f, 0.f) * Scale3DRate;
				CameraParts->SetMainCamera().SetCamPos(
					BaseCamPos + Vector3DX::vget(0.8f, -0.25f, 2.f) * Scale3DRate,
					BaseCamPos,
					Vector3DX::up());
				auto fovBuf = CameraParts->GetMainCamera().GetCamFov();
				CameraParts->SetMainCamera().SetCamInfo(fovBuf, CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar());
			}
			// 
			FadeControl::Instance()->Update();
			if (this->m_IsEnd && FadeControl::Instance()->IsAll()) {
				return false;
			}
			return true;
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveData::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			//
			m_MovieCharacter.reset();
			/*
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
			SetNextSelect(static_cast<size_t>(ButtonParts->GetSelect()));
		}
		void TitleScene::Dispose_Load_Sub(void) noexcept {
			ObjectManager::Instance()->DeleteAll();
			this->m_ObjSky.Dispose();
		}
		void TitleScene::BG_Draw_Sub(void) const noexcept {
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
		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			// 背景
			//DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Gray50, true);
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = UIs::ButtonControl::Instance();
			// 
			DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
				&this->m_TitleImage, (64), (64), (64 + 369), (64 + 207), true);
			// 
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18), 
				FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
				(64 + 369), (64 + 207), White, Black, "Ver 0.0.0");
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18), 
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM,
					(32), 1080 - (32 + 32), White, Black, LocalizeParts->Get(9020 + ButtonParts->GetSelect()));
			}
			// 
			FadeControl::Instance()->Draw();
		}
	}
}
