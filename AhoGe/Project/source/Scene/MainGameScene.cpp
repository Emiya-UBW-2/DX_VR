#include	"MainGameScene.hpp"

#include	"../sub.hpp"

#include	"../MainScene/Player/Player.hpp"
#include	"../MainScene/BackGround/BackGround.hpp"
#include	"../CommonScene/Object/Object2DManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		// ロード
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* ResourceParts = CommonBattleResource::Instance();
			ResourceParts->Load();
			InGameUIControl::LoadUI();
			PauseMenuControl::LoadPause();
			//初期マップtodo:セーブデータより
			auto* SaveDataParts = SaveDataClass::Instance();
			if (SaveDataParts->GetParam("LastMap") == -1) {
				SaveDataParts->SetParam("LastMap", 0);
			}
			m_MapName = "map"+ std::to_string(SaveDataParts->GetParam("LastMap"));
			if (SaveDataParts->GetParam("LastEntry") == -1) {
				SaveDataParts->SetParam("LastEntry", 0);
			}
			m_EntryID = (int)SaveDataParts->GetParam("LastEntry");
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Cam2D = Cam2DControl::Instance();
			auto* ResourceParts = CommonBattleResource::Instance();
			ResourceParts->Set();
			PauseMenuControl::SetPause();
			BackGround->Init(m_MapName);
			PlayerMngr->Init((int)BackGround->GetPlayerSpawn().size());
			// 全キャラの設定
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
				ResourceParts->AddCharacter((PlayerID)i);

				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				if ((PlayerID)i == m_MyPlayerID) {
					for (auto& e : BackGround->GetEventChip()) {
						if (e.m_EventID == m_EntryID) {
							p.GetChara()->SetPos(BackGround->GetFloorData(e.m_index)->GetPos());
							break;
						}
					}
				}
				else {
					p.GetChara()->SetPos(BackGround->GetFloorData(BackGround->GetPlayerSpawn().at(i).m_index)->GetPos());
				}
			}
			auto& Chara = PlayerMngr->GetPlayer(m_MyPlayerID).GetChara();
			m_PrevXY = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));
			//カメラ
			Cam2D->SetCamPos(Chara->GetPos());

			InGameUIControl::SetMapTextID(BackGround->GetMapTextID());
			// 
			CutSceneControl::SetCut();
			FadeControl::SetFade();
			InGameUIControl::SetUI();
			//
			m_IsCautionBGM = false;
			m_CautionBGM = 0.f;
			m_IsAlertBGM = false;
			m_AlertBGM = 0.f;
			//
			m_IsGoNext = false;
			m_IsEnd = false;
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			printfDx("FPS : %5.2f\n", GetFPS());
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Cam2D = Cam2DControl::Instance();
			auto* BGM = BGMPool::Instance();
			auto* SE = SoundPool::Instance();

			auto& Chara = PlayerMngr->GetPlayer(m_MyPlayerID).GetChara();

			PauseMenuControl::UpdatePause();
			if (DrawParts->IsPause()) {
				return true;
			}
			if ((m_IsEnd || m_IsGoNext) && FadeControl::IsFadeAll()) {
				return false;
			}
			if (m_IsPlayable) {
				Pad->SetMouseMoveEnable(false);
				Pad->ChangeGuide(
					[]() {
						auto* KeyGuide = PadControl::Instance();
						auto* LocalizeParts = LocalizePool::Instance();
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_A, "");
						KeyGuide->AddGuide(PADS::MOVE_D, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9900));
						KeyGuide->AddGuide(PADS::RUN, LocalizeParts->Get(9902));
						KeyGuide->AddGuide(PADS::WALK, LocalizeParts->Get(9903));
						KeyGuide->AddGuide(PADS::JUMP, LocalizeParts->Get(9905));
						KeyGuide->AddGuide(PADS::SHOT, LocalizeParts->Get(9906));
						KeyGuide->AddGuide(PADS::AIM, LocalizeParts->Get(9908));
					});
			}
			//
			CutSceneControl::UpdateCut();
			FadeControl::IsFadeAll();
			FadeControl::UpdateFade();
			//BGM
			{
				bool Prev = m_IsCautionBGM || m_IsAlertBGM;
				m_IsCautionBGM = false;
				m_IsAlertBGM = false;
				for (int i = 0;i < PlayerMngr->GetPlayerNum();i++) {
					auto& p = PlayerMngr->GetPlayer((PlayerID)i);
					if (p.GetAI()->IsAlert()) {
						m_IsAlertBGM = true;
					}
					else if (p.GetAI()->IsCaution()) {
						m_IsCautionBGM = true;
					}
				}
				if (m_IsAlertBGM) {
					m_IsCautionBGM = false;
				}
				if (Prev && Prev != (m_IsCautionBGM || m_IsAlertBGM)) {
					SE->Get((int)SoundEnum::Normal).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				//
				if (m_IsCautionBGM) {
					if (m_CautionBGM < 1.f) {
						m_CautionBGM = 1.f;
						BGM->Get(0).Play(DX_PLAYTYPE_LOOP, TRUE);
						BGM->Get(0).SetVol_Local((int)(255 * m_CautionBGM));
					}
				}
				else {
					bool IsPlay = (m_CautionBGM > 0.f);
					m_CautionBGM = std::max(m_CautionBGM - 1.f / DrawParts->GetFps(), 0.f);
					if (m_CautionBGM > 0.f) {
						BGM->Get(0).SetVol_Local((int)(255 * m_CautionBGM));
					}
					else {
						if (IsPlay) {
							BGM->Get(0).Stop();
						}
					}
				}
				//
				if (m_IsAlertBGM) {
					if (m_AlertBGM < 1.f) {
						m_AlertBGM = 1.f;
						BGM->Get(1).Play(DX_PLAYTYPE_LOOP, TRUE);
						BGM->Get(1).SetVol_Local((int)(255 * m_AlertBGM));
					}
				}
				else {
					bool IsPlay = (m_AlertBGM > 0.f);
					m_AlertBGM = std::max(m_AlertBGM - 1.f / DrawParts->GetFps(), 0.f);
					if (m_AlertBGM > 0.f) {
						BGM->Get(1).SetVol_Local((int)(255 * m_AlertBGM));
					}
					else {
						if (IsPlay) {
							BGM->Get(1).Stop();
						}
					}
				}
			}
			//イベント等制御
			{
				auto Prev = m_IsPlayable;
				m_IsPlayable = FadeControl::IsFadeClear();
				if (CutSceneControl::IsCutScene()) {
					m_IsPlayable = false;
				}
				if (Prev != m_IsPlayable) {
					Pad->SetGuideUpdate();
				}
				if (PauseMenuControl::IsRetire()) {
					this->m_IsEnd = true;
				}
				FadeControl::SetBlackOut(m_IsEnd || m_IsGoNext);
			}
			// 
			BackGround->Execute();
			// 入力制御
			InputControl MyInput;
			float InputRad = 0.f;
			for (int i = 0;i < PlayerMngr->GetPlayerNum();i++) {
				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				if (p.GetChara()) {
					MyInput.ResetAllInput();
					InputRad = 0.f;
					if (m_IsPlayable) {
						if (i == m_MyPlayerID) {
							MyInput.SetInputStart(0.f, 0.f);
							MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
							MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
							MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
							MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());
							MyInput.SetInputPADS(PADS::RUN, Pad->GetKey(PADS::RUN).press());
							MyInput.SetInputPADS(PADS::WALK, Pad->GetKey(PADS::WALK).press());
							MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());
							MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press());
							//視点操作
							if (abs(Pad->GetLS_X()) > 0.1f || abs(Pad->GetLS_Y()) > 0.1f) {
								InputRad = std::atan2f(Pad->GetLS_X(), Pad->GetLS_Y());
							}
							else if (abs(p.GetChara()->GetVec().x) > 0.1f || abs(p.GetChara()->GetVec().y) > 0.1f) {
								InputRad = std::atan2f(p.GetChara()->GetVec().x, p.GetChara()->GetVec().y);
							}
							else {
								InputRad = p.GetChara()->GetViewRad();
							}
						}
						else {
							p.GetAI()->Execute(&MyInput);
							InputRad = p.GetAI()->GetViewRad();
							if (!p.GetChara()->CanLookPlayer0()) {
								p.GetChara()->SetColTarget(ColTarget::None);
							}
							else {
								p.GetChara()->SetColTarget(ColTarget::All);
							}
						}
					}
					p.GetChara()->ExecuteInput(&MyInput);
					p.GetChara()->SetViewRad(InputRad);
				}
			}
			Obj2DParts->ExecuteObject();
			// カメラ制御
			Vector3DX CamPos = Vector3DX::zero();
			if (m_IsPlayable && Pad->GetKey(PADS::AIM).press()) {
				float ViewLimit = Get2DSize(10.f);
				CamPos.Set(std::sin(Chara->GetBodyRad()) * ViewLimit, std::cos(Chara->GetBodyRad()) * ViewLimit, 0.f);
			}
			if (CutSceneControl::IsCutScene()) {
				auto MyIndex = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));

				MyIndex.first += CutSceneControl::GetAddViewPointX();
				MyIndex.second += CutSceneControl::GetAddViewPointY();
				CamPos = BackGround->GetFloorData(BackGround->GetXYToNum(MyIndex.first, MyIndex.second))->GetPos() - Chara->GetPos();
			}
			Easing(&m_CamAddPos, CamPos, 0.9f, EasingType::OutExpo);
			Cam2D->SetCamAim(Chara->GetPos() + m_CamAddPos);
			// 
			BackGround->SetPointLight(Chara->GetPos());
			BackGround->SetAmbientLight(120.f, deg2rad(30));
			BackGround->SetupShadow([]() {
				auto* Obj2DParts = Object2DManager::Instance();
				Obj2DParts->DrawShadow();
									});

			InGameUIControl::UpdateUI();
			if (FadeControl::IsFadeClear()) {
				//範囲に入ったらイベント
				auto MyIndex = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));
				auto IsNearIndex = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
					return  (abs(a.first - b.first) <= 1 && abs(a.second - b.second) <= 1);
				};
				if (!IsNearIndex(m_PrevXY, MyIndex)) {
					m_PrevXY = MyIndex;
					if (m_IsPlayable) {
						for (auto& e : BackGround->GetEventChip()) {
							if (IsNearIndex(BackGround->GetNumToXY(e.m_index), MyIndex)) {//該当のチップに踏み込んだ
								//次マップへの遷移
								if (e.m_EventType == EventType::Entry) {
									m_MapName = e.m_MapName;
									m_EntryID = e.m_EntryID;
									m_IsGoNext = true;
									break;
								}
								//
								if (e.m_EventType == EventType::CutScene) {
									CutSceneControl::StartCutScene(e.m_CutSceneID);
									break;
								}
							}
						}
					}
				}
			}
			InGameUIControl::SetupWatchScreen();
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* ResourceParts = CommonBattleResource::Instance();
			// リソース
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
				ResourceParts->DelCharacter((PlayerID)i);
			}
			PlayerMngr->Dispose();
			BackGround->Dispose();
			// セーブ
			{
				//最後に訪れたマップ
				SaveDataParts->SetParam("LastMap", std::stoi(m_MapName.substr(3)));
				SaveDataParts->SetParam("LastEntry", m_EntryID);
			}
			SaveDataParts->Save();
			if (m_IsEnd) {//タイトルに戻る
				SetNextSelect(0);
			}
			else {//次のシーンへ
				SetNextSelect(1);
			}
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto* Obj2DParts = Object2DManager::Instance();
			auto* ResourceParts = CommonBattleResource::Instance();
			Obj2DParts->DeleteAll();
			ResourceParts->Dispose();
			PauseMenuControl::DisposePause();
			InGameUIControl::Dispose_LoadUI();
		}
		//
		void			MainGameScene::MainDraw_Sub(void) noexcept {
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			BackGround->Draw();
			InGameUIControl::DrawUI_Back();
			Obj2DParts->Draw();
			BackGround->DrawFront();
			InGameUIControl::DrawUI_Front();
		}
		void			MainGameScene::DrawUI_Base_Sub(void) noexcept {
			InGameUIControl::DrawUI_MapName();
			FadeControl::DrawFade();
			CutSceneControl::DrawCut();
		}
		void			MainGameScene::DrawUI_In_Sub(void) noexcept {
			// ポーズ
			PauseMenuControl::DrawPause();
		}
	};
};
