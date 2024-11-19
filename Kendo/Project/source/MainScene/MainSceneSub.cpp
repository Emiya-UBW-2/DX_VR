#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void PauseMenuControl::Set(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton(420, 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(421, 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(422, 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 0, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			this->m_IsRetire = false;
		}
		void PauseMenuControl::UpdatePause(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* OptionDrawCtrls = OptionWindowClass::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* PopUpParts = PopUp::Instance();
			if (!PopUpParts->IsActivePop()) {
				KeyGuideParts->ChangeGuide(
					[this]() {
						auto* Pad = PadControl::Instance();
						auto* KeyGuideParts = KeyGuide::Instance();
						auto* LocalizeParts = LocalizePool::Instance();
						KeyGuideParts->AddGuide(KeyGuide::GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9992));
						KeyGuideParts->AddGuide(KeyGuide::GetIDtoOffset(Pad->GetPadsInfo(PADS::RELOAD).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9991));
						KeyGuideParts->AddGuide(KeyGuide::GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_W).GetAssign(), Pad->GetControlType()), "");
						KeyGuideParts->AddGuide(KeyGuide::GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_S).GetAssign(), Pad->GetControlType()), "");
						KeyGuideParts->AddGuide(KeyGuide::GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_STICK).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9993));
					});
				ButtonParts->UpdateInput();
				// ‘I‘ðŽž‚Ì‹““®
				if (ButtonParts->GetTriggerButton()) {
					switch (ButtonParts->GetSelect()) {
					case 0:
						this->m_IsRetire = true;
						SceneParts->ChangePause(false);
						break;
					case 1:
						OptionDrawCtrls->SetActive();
						break;
					case 2:
						SceneParts->ChangePause(false);
						break;
					default:
						SceneParts->ChangePause(false);
						break;
					}
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
				}
				if (Pad->GetPadsInfo(PADS::RELOAD).GetKey().trigger()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_CANCEL))->Play(DX_PLAYTYPE_BACK, TRUE);
					SceneParts->ChangePause(false);
				}
				// 
				ButtonParts->Update();
			}
		}
		void ResultMenuControl::Load(void) noexcept{
			m_Result.Load("data/UI/Result.png");
			m_Result.GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 400);
		}
		void ResultMenuControl::Set(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
			ButtonParts->ResetSel();
			ButtonParts->AddIconButton("CommonData/UI/Right.png", true, (1920 / 2 + 64), (1080 - 256), FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			ButtonParts->AddIconButton("data/UI/Reset.png", true, (1920 / 2 - 64), (1080 - 256), FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE);
			m_IsReplay = false;
			m_IsEnd = false;
		}
		void ResultMenuControl::Update(void) noexcept {
			auto* FadeInParts = FadeControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();

			m_IsReplay = false;
			m_IsEnd = false;
			ButtonParts->UpdateInput();
			// ‘I‘ðŽž‚Ì‹““®
			if (ButtonParts->GetTriggerButton()) {
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
				switch (ButtonParts->GetSelect()) {
				case 0:
				{
					this->m_IsEnd = true;
					FadeInParts->SetFadeOut(1.f);
				}
				break;
				case 1:
					this->m_IsReplay = true;
					break;
				default:
					break;
				}
			}
			if (Pad->GetPadsInfo(PADS::JUMP).GetKey().trigger()) {
				this->m_IsReplay = true;
			}
		}
		void ResultMenuControl::Draw(void) const noexcept{
			auto* ButtonParts = ButtonControl::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();

			PlayerID Now = 0;// GetMyPlayerID();

			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Result, (0), (0), (1920), (1080), false);
			bool IsWin = (PlayerMngr->GetPlayer(Now)->GetMaxScore() > PlayerMngr->GetPlayer(1 - Now)->GetMaxScore());
			bool IsDraw = (PlayerMngr->GetPlayer(Now)->GetMaxScore() == PlayerMngr->GetPlayer(1 - Now)->GetMaxScore());
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (48),
				FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::BOTTOM, (300), (384),
				IsDraw ? Gray25 : (IsWin ? Red : White), Black,
				IsDraw ? LocalizeParts->Get(6002) : (IsWin ? LocalizeParts->Get(6000) : LocalizeParts->Get(6001)));
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
				FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, (300), (386), White, Black, "%d : %d",
				PlayerMngr->GetPlayer(Now)->GetMaxScore(), PlayerMngr->GetPlayer(1 - Now)->GetMaxScore());
			ButtonParts->Draw();
		}
		void ResultMenuControl::Dispose_Load(void) noexcept {
			this->m_Result.Dispose();
		}
	}
}
