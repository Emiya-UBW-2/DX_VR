#pragma warning(disable:4464)
#include	"EndScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void EndScene::SetupEvent(const char* Path) noexcept {
			m_IsEventSceneFlag = true;
			m_EventSelect = Path;
		}
		void EndScene::UpdateEvent(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			if (!m_IsEventSceneActive) {
				if (m_IsEventSceneFlag) {
					m_IsEventSceneFlag = false;
					m_IsEventSceneActive = true;
					PostPassParts->SetDistortionPer(120.f * 4);
					m_EventScene.Load(m_EventSelect.c_str());
					m_EventScene.Start();
					KeyGuideParts->SetGuideFlip();
				}
			}
			if (m_IsEventSceneActive) {
				m_EventScene.GetDeltaTime();
				if (!SceneParts->IsPause() && Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
					SE->StopAll(SoundType::SE);
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
					m_EventScene.Skip();
				}
				if (m_EventScene.IsEnd()) {
					m_EventScene.Dispose();
					m_IsEventSceneActive = false;
					KeyGuideParts->SetGuideFlip();

					this->m_IsEnd = true;
				}
				m_EventScene.Update();
			}
		}
		//
		void			EndScene::Load_Sub(void) noexcept {
		}
		void			EndScene::Set_Sub(void) noexcept {
			auto* PostPassParts = PostPassEffect::Instance();
			//
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
			}
			PostPassParts->SetShadowScale(0.75f);
		}
		bool			EndScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* SceneParts = SceneControl::Instance();
			//
			if (GetIsFirstLoop()) {
				SetupEvent("data/Cut/Cut4.txt");
			}
			//
			KeyGuideParts->ChangeGuide(
				[this]() {
					auto* SceneParts = SceneControl::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					if (SceneParts->IsPause()) {
					}
					else {
						if (m_IsEventSceneFlag || m_IsEventSceneActive) {
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9914));
						}
					}
				});
			//
			if (this->m_IsEnd) {
				return false;
			}
			Pad->SetMouseMoveEnable(false);
			//
			UpdateEvent();
			if (m_IsEventSceneActive) {
				return true;
			}
			if (SceneParts->IsPause()) {
				return true;
			}
			return true;
		}
		void			EndScene::Dispose_Sub(void) noexcept {
			if (m_IsEventSceneActive) {
				m_EventScene.Dispose();
			}
			SetNextSelect(0);//タイトルに戻る
		}
		void			EndScene::Dispose_Load_Sub(void) noexcept {}
		//
		void			EndScene::BG_Draw_Sub(void) const noexcept {
			if (m_IsEventSceneActive) {
				m_EventScene.BGDraw();
			}
		}
		void			EndScene::ShadowDraw_Sub(void) const noexcept {
			if (m_IsEventSceneActive) {
				m_EventScene.ShadowDraw();
			}
		}
		void			EndScene::SetShadowDraw_Sub(void) const noexcept {
			if (m_IsEventSceneActive) {
				m_EventScene.SetShadowDraw();
			}
		}
		void			EndScene::MainDraw_Sub(void) const noexcept {
			if (m_IsEventSceneActive) {
				m_EventScene.MainDraw();
			}
		}
		//UI表示
		void			EndScene::DrawUI_Base_Sub(void) const noexcept {
			if (m_IsEventSceneActive) {
				m_EventScene.UIDraw();
			}
		}
		void			EndScene::DrawUI_In_Sub(void) const noexcept {}
	}
}
