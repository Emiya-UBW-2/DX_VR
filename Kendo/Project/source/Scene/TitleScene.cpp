#include	"TitleScene.hpp"
#include	"../sub.hpp"

#include	"../Header.hpp"

#include	"../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void TitleScene::Load_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();

			MV1::Load("data/model/sky/model.mv1", &this->m_TitleBGSky);
			MV1::Load("data/model/map/model_DISABLE.mv1", &this->m_TitleBGMap);
			MV1::Load("data/MovieModel/Chara/model.mv1", &this->m_TitleChara);
			MV1::Load("data/MovieModel/Bamboo/model.mv1", &this->m_TitleBamboo);

			m_Flag.Load("data/UI/Flag.png");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Envi), 1, "data/Sound/SE/envi.wav", false);
		}
		void			TitleScene::Set_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* CameraParts = Camera3D::Instance();
			//
			this->m_TitleBGSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
			MV1::SetAnime(&(this->m_TitleChara), this->m_TitleChara);
			MV1::SetAnime(&(this->m_TitleBamboo), this->m_TitleBamboo);
			//
			this->m_TitleChara.SetMatrix(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.f, 75.f)));
			this->m_TitleChara.SetAnim(0).SetPer(1.0f);
			this->m_TitleChara.SetAnim(0).SetTime(0.0f);
			this->m_TitleChara.UpdateAnimAll();
			this->m_TitleBamboo.SetMatrix(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.f, 75.f)));
			this->m_TitleBamboo.SetAnim(0).SetPer(1.0f);
			this->m_TitleBamboo.SetAnim(0).SetTime(0.0f);
			this->m_TitleBamboo.UpdateAnimAll();
			// 
			this->m_GameFadeIn = 1.f;
			this->m_GameStart = 0.f;
			this->m_TitleImage.Load("data/UI/Title.png");
			// 
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton(400, 52, true, 1920 - 64 - 48, 1080 - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(401, 48, true, 1920 - 64 - 48, 1080 - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(402, 48, IsFileExist("Replay/Rep.txt"), 1920 - 64 - 48, 1080 - 84 - 64 * 0, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddIconButton("CommonData/UI/setting.png", true, (1920 - 96 * 2 - 64), (64), FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("CommonData/UI/credit.png", true, (1920 - 96 * 1 - 64), (64), FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("data/UI/Info.png", true, (1920 - 96 * 0 - 64), (64), FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			int WinCount = static_cast<int>(SaveDataClass::Instance()->GetParam("WinCount"));
			if (2 < WinCount) {
				ButtonParts->AddStringButton(406, 24, true, 1920 - 64 - 48-125, 1080 - 84 - 64 * 3, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::BOTTOM);
			}
			// クレジット
			this->m_CreditControl = std::make_unique<CreditControl>();
			this->m_CreditControl->Init();
			//
			this->m_InfoControl = std::make_unique<InfoControl>();
			this->m_InfoControl->Init();
			// 
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->Play(DX_PLAYTYPE_LOOP, TRUE);
			//Cam
			CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.3f, Scale3DRate * 20.f);
			Vector3DX CamVec = Vector3DX::vget(0, 10, 75);
			Vector3DX CamPos = CamVec + Vector3DX::vget(-20, -5, -20);
			CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, Vector3DX::vget(0, 1, 0));
			{
				Vector3DX LightVec = Vector3DX::vget(1.f, -0.5f, 0.05f); LightVec = LightVec.normalized();
				PostPassParts->SetAmbientLight(LightVec);

				SetLightEnable(FALSE);

				auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
				SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
				SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));

				auto& SecondLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec * -1.f);
				SetLightDifColorHandle(SecondLight.get(), GetColorF(0.5f, 0.5f, 0.3f, 0.1f));

				PostPassParts->SetGodRayPer(0.5f);
				PostPassParts->SetShadowScale(0.75f);
			}
			m_CloseResetSave = false;
			//DoF
			PostPassParts->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, Scale3DRate * 20.f);
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* OptionDrawCtrls = OptionPopup::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			if (SceneParts->IsPause()) {
				return true;
			}

			//カメラ
			Vector3DX CamVec = Vector3DX::vget(0, 10, 75);
			Vector3DX CamPos = CamVec + Vector3DX::vget(-20, -5, -20);

			auto Prev = m_Phase;
			switch (m_Phase) {
			case 0:
				CamVec = Vector3DX::vget(0, 10, 95);
				CamPos = Vector3DX::vget(0, 10, 75) + Lerp(Vector3DX::vget(-5, -5, -30), Vector3DX::vget(-15, -5, -30), m_PhaseTime / 5.f);
				if (m_PhaseTime > 5.f) {
					m_Phase++;
				}
				break;
			case 1:
				CamVec = Lerp(Vector3DX::vget(0, 5, 75 + 10), Vector3DX::vget(0, 5, 75 - 10), m_PhaseTime / 5.f);
				CamPos = Lerp(Vector3DX::vget(-20, 15, 75 + 10), Vector3DX::vget(-20, 15, 75 - 10), m_PhaseTime / 5.f);
				if (m_PhaseTime > 5.f) {
					m_Phase++;
				}
				break;
			case 2:
				CamVec = Vector3DX::vget(0, 15, 0);
				CamPos = Lerp(Vector3DX::vget(-10, 10, 75 + 20), Vector3DX::vget(-5, 10, 75 + 20), m_PhaseTime / 5.f);
				if (m_PhaseTime > 5.f) {
					m_Phase = 0;
				}
				break;
			default:
				break;
			}
			if (Prev != m_Phase) {
				m_PhaseTime = 0.f;
			}
			else {
				m_PhaseTime += DXLib_refParts->GetDeltaTime();
			}

			CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, Vector3DX::vget(0, 1, 0));

			Pad->SetMouseMoveEnable(false);
			KeyGuideParts->ChangeGuide(
				[]() {
					auto* KeyGuideParts = KeyGuide::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
					KeyGuideParts->AddGuide(Controls::GetIDtoOffset(Controls::GetStrtoID("LMOUSE", Controls::ControlType::PC), Controls::ControlType::PC), LocalizeParts->Get(9919));
				}
			);
			if (!PopUpParts->IsActivePop() && (this->m_GameFadeIn == 0.f) && (this->m_GameStart == 0.f)) {
				ButtonParts->UpdateInput();
				// 選択時の挙動
				if (ButtonParts->GetTriggerButton()) {
					switch (ButtonParts->GetSelect()) {
					case 0:
					case 1:
					case 2:
						this->m_GameStart += 0.0001f;
						// 次シーン決定
						SetNextSelect(static_cast<size_t>(ButtonParts->GetSelect()));
						break;
					case 3:
						OptionDrawCtrls->SetActive();
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
					case 5:
						this->m_InfoControl->Start();
						PopUpParts->Add(LocalizeParts->Get(130), (720), (840),
							[&](int xmin, int ymin, int xmax, int ymax, bool) {
								this->m_InfoControl->Draw(xmin, ymin, xmax, ymax);
							},
							[&]() { this->m_InfoControl->End(); },
							[&]() { this->m_InfoControl->Guide(); },
							true
						);
						break;
					case 6:
						this->m_GameStart += 0.0001f;
						// 次シーン決定
						SetNextSelect(3);
						break;
					default:
						break;
					}
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (m_CloseResetSave && PopUpParts->IsActivePop()) {
				m_CloseResetSave = false;
				PopUpParts->EndAll();
			}
			// 
			ButtonParts->Update();
			// 
			this->m_GameFadeIn = std::max(this->m_GameFadeIn - DXLib_refParts->GetDeltaTime() / 0.5f, 0.f);
			if (this->m_GameStart != 0.f) { this->m_GameStart += DXLib_refParts->GetDeltaTime() / 0.5f; }
			return (this->m_GameStart <= 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* SE = SoundPool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			//
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->StopAll();
			// 
			this->m_CreditControl->Dispose();
			this->m_CreditControl.reset();
			// 
			this->m_InfoControl->Dispose();
			this->m_InfoControl.reset();
			// 
			ButtonParts->Dispose();
			// 
			this->m_TitleImage.Dispose();
			// セーブ
			SaveDataParts->Save();
		}
		void TitleScene::Dispose_Load_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			m_TitleBGSky.Dispose();
			m_TitleBGMap.Dispose();
			m_TitleChara.Dispose();
			m_TitleBamboo.Dispose();

			m_Flag.Dispose();

			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Envi));
		}
		//
		void TitleScene::BG_Draw_Sub(void) const noexcept {
			SetUseLighting(FALSE);
			this->m_TitleBGSky.DrawModel();
			SetUseLighting(TRUE);
		}
		void TitleScene::ShadowDraw_Sub(void) const noexcept {
			m_TitleBGMap.DrawModel();
			m_TitleChara.DrawModel();
			m_TitleBamboo.DrawModel();
		}
		void TitleScene::SetShadowDraw_Sub(void) const noexcept {
			m_TitleBGMap.DrawModel();
			m_TitleChara.DrawModel();
			m_TitleBamboo.DrawModel();
		}
		// 
		void			TitleScene::MainDraw_Sub(void) const noexcept {
			m_TitleBGMap.DrawModel();
			m_TitleChara.DrawModel();
			m_TitleBamboo.DrawModel();
		}
		void			TitleScene::DrawUI_Base_Sub(void) const noexcept {
			auto* Pad = PadControl::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			// 
			DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 64, 64, 64);
			DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
				&this->m_TitleImage, (64 + 4), (64 + 4), (64 + 1024 + 4), (64 + 256 + 4), true);
			DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
			DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
				&this->m_TitleImage, (64), (64), (64 + 1024), (64 + 256), true);
			// 
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18), 
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
				(64), (64 + 256), White, Black, "Ver 1.0.0");
			//
			int WinCount = static_cast<int>(SaveDataClass::Instance()->GetParam("WinCount"));
			if(2 >= WinCount){
				int xp1 = 1920 - 64 - 48 - 48 * 3;
				int yp1 = 1080 - 84 - 64 * 3;
				for (int i = 0; i < 3; ++i) {
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 64, 64, 64);
					DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &this->m_Flag, xp1 - 32, yp1 - 32, xp1, yp1, true);
					if (i >= WinCount) {
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 0, 0);
					}
					else {
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
					}
					DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &this->m_Flag, xp1 - 32 - 4, yp1 - 32 - 4, xp1 - 4, yp1 - 4, true);
					xp1 += 48;
				}
				DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
			}
			// 
			ButtonParts->Draw();
			// 
			if ((ButtonParts->GetSelect() != InvalidID) && !PopUpParts->IsActivePop()) {
				if (ButtonParts->IsMouseSelMode()) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM,
						Pad->GetMS_X(), Pad->GetMS_Y(), White, Black, LocalizeParts->Get(410 + ButtonParts->GetSelect()));
				}
				else {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM,
						(32), (1080 - 32 - 32), White, Black, LocalizeParts->Get(410 + ButtonParts->GetSelect()));
				}
			}
			// 
			{
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameFadeIn, 0.f, 1.f)), 0, 255));
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, (1920), (1080), Black, TRUE);

				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * std::clamp(this->m_GameStart, 0.f, 1.f)), 0, 255));
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, (1920), (1080), White, TRUE);

				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
