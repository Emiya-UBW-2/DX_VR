#include	"MainGameScene.hpp"

#include	"../sub.hpp"

#include	"../MainScene/Player/Player.hpp"
#include	"../MainScene/BackGround/BackGround.hpp"
#include	"../CommonScene/Object/Object2DManager.hpp"

#include	"../MainScene/Object/Metal.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		// ロード
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* ResourceParts = CommonBattleResource::Instance();
			ResourceParts->Load();
			InGameUIControl::LoadUI();
			PauseMenuControl::LoadPause();
			//初期マップ
			this->m_IsPrologue = false;
			auto* SaveDataParts = SaveDataClass::Instance();
			if (SaveDataParts->GetParam("LastMap") == -1) {//初期データがない場合はここから始める
				SaveDataParts->SetParam("LastMap", 0);
				SaveDataParts->SetParam("LastEntry", 0);
				this->m_IsPrologue = true;
			}
			this->m_MapName = "map" + std::to_string(SaveDataParts->GetParam("LastMap"));
			this->m_EntryID = static_cast<int>(SaveDataParts->GetParam("LastEntry"));
			this->m_CutSceneID = INVALID_ID;
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Cam2D = Cam2DControl::Instance();
			auto* ResourceParts = CommonBattleResource::Instance();
			ResourceParts->Set();
			PauseMenuControl::SetPause();
			BackGround->Init(this->m_MapName);
			PlayerMngr->Init(static_cast<int>(BackGround->GetPlayerSpawn().size()));
			m_BossID = INVALID_ID;
			m_WinCutSceneID = INVALID_ID;
			// 全キャラの設定
			Vector3DX					GoalPos;GoalPos.Set(-1.f, -1.f, -1.f);
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
				ResourceParts->AddCharacter((PlayerID)i);

				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				if ((PlayerID)i == this->m_MyPlayerID) {
					for (auto& e : BackGround->GetEventChip()) {
						if (e.m_EventType == EventType::Entry) {
							if (e.m_EventID == this->m_EntryID) {
								p->GetChara()->SetPos(BackGround->GetFloorData(e.m_index)->GetPos());
							}
							else {
								GoalPos = BackGround->GetFloorData(e.m_index)->GetPos();
							}
						}
						if (e.m_EventType == EventType::Boss) {
							auto* Obj2DParts = Object2DManager::Instance();
							const auto& Obj = std::make_shared<MetalObject>();
							Obj2DParts->AddObject(Obj);
							Obj->SetObjType(static_cast<int>(Object2DType::Metal));
							Obj->SetPlayerID(1);
							Obj->SetPos(BackGround->GetFloorData(e.m_index)->GetPos());
							Obj->SetSize(10.f);
							m_BossID = Obj->GetObjectID();
							m_WinCutSceneID = e.m_WinCutSceneID;
						}
					}
				}
				else {
					p->GetChara()->SetPos(BackGround->GetFloorData(BackGround->GetPlayerSpawn().at(static_cast<size_t>(i)).m_index)->GetPos());
				}
			}
			auto& Chara = PlayerMngr->GetPlayer(this->m_MyPlayerID)->GetChara();
			//カメラ
			Cam2D->SetCamPos(Chara->GetPos());
			Cam2D->SetCamRangePos(BackGround->GetCamScale());
			//
			this->m_PrevXY = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));
			//
			InGameUIControl::SetMap(BackGround->GetMapTextID(), GoalPos);
			// 
			CutSceneControl::SetCut();
			FadeControl::SetFade();
			InGameUIControl::SetUI();
			Effect2DControl::Instance()->Init();
			if (m_CutSceneID != INVALID_ID) {
				CutSceneControl::StartCutScene(m_CutSceneID);
			}
			else if (m_IsPrologue) {
				CutSceneControl::StartCutScene(500);
			}
			//
			this->m_IsCautionBGM = false;
			this->m_CautionBGM = 0.f;
			this->m_IsAlertBGM = false;
			this->m_AlertBGM = 0.f;
			//
			this->m_IsGoNext = false;
			this->m_IsEnd = false;
			this->m_IsGoodEnd = false;
			this->m_IsBadEnd = false;
			this->m_StartTime = 0.f;

			auto* SaveDataParts = SaveDataClass::Instance();
			Chara->SetShootTimer(0.f);
			if (SaveDataParts->GetParam("Cut_10") != -1) {
				Chara->SetShootTimer(1.f);
			}
			if (SaveDataParts->GetParam("Cut_20") != -1) {
				Chara->SetShootTimer(0.1f);
			}
			if (SaveDataParts->GetParam("Cut_30") != -1) {
				Chara->SetShootTimer(5.f);
			}
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Cam2D = Cam2DControl::Instance();
			auto* BGM = BGMPool::Instance();
			auto* SE = SoundPool::Instance();
			auto* SaveDataParts = SaveDataClass::Instance();

			auto& Chara = PlayerMngr->GetPlayer(this->m_MyPlayerID)->GetChara();

			PauseMenuControl::UpdatePause();
			if (DrawParts->IsPause()) {
				return true;
			}
			if ((this->m_IsEnd || this->m_IsGoNext) && FadeControl::IsFadeAll()) {
				return false;
			}
			if (this->m_IsPlayable) {
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

			if (m_BossID != INVALID_ID) {
				auto* Obj = Obj2DParts->GetObj(m_BossID);
				if (Obj) {
					auto& P = (std::shared_ptr<MetalObject>&)(*Obj);
					P->SetCanMove(this->m_IsPlayable);
				}
				else if(m_WinCutSceneID != INVALID_ID){
					CutSceneControl::StartCutScene(m_WinCutSceneID);
					m_WinCutSceneID = INVALID_ID;
				}
			}
			//
			CutSceneControl::UpdateCut();
			FadeControl::IsFadeAll();
			FadeControl::UpdateFade();
			//BGM
			{
				bool Prev = this->m_IsCautionBGM || this->m_IsAlertBGM;
				this->m_IsCautionBGM = false;
				this->m_IsAlertBGM = false;
				for (int i = 0;i < PlayerMngr->GetPlayerNum();i++) {
					auto& p = PlayerMngr->GetPlayer((PlayerID)i);
					if (p->GetChara()) {
						if (p->GetAI()->IsAlert()) {
							this->m_IsAlertBGM = true;
						}
						else if (p->GetAI()->IsCaution()) {
							this->m_IsCautionBGM = true;
						}
					}
				}
				if (this->m_IsAlertBGM) {
					this->m_IsCautionBGM = false;
				}
				this->m_IsNormalBGM = !(this->m_IsCautionBGM || this->m_IsAlertBGM);
				if (Prev && Prev != (this->m_IsCautionBGM || this->m_IsAlertBGM)) {
					SE->Get(static_cast<int>(SoundEnum::Normal)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				//
				if (this->m_IsNormalBGM) {
					if (this->m_NormalBGM < 1.f) {
						this->m_NormalBGM = 1.f;
						BGM->Get(1).Play(DX_PLAYTYPE_LOOP, TRUE);
						BGM->Get(1).SetVol_Local(static_cast<int>(255 * this->m_NormalBGM));
					}
				}
				else {
					bool IsPlay = (this->m_NormalBGM > 0.f);
					this->m_NormalBGM = std::max(this->m_NormalBGM - 1.f / DrawParts->GetFps(), 0.f);
					if (this->m_NormalBGM > 0.f) {
						BGM->Get(1).SetVol_Local(static_cast<int>(255 * this->m_NormalBGM));
					}
					else {
						if (IsPlay) {
							BGM->Get(1).Stop();
						}
					}
				}
				//
				if (this->m_IsCautionBGM) {
					if (this->m_CautionBGM < 1.f) {
						this->m_CautionBGM = 1.f;
						BGM->Get(2).Play(DX_PLAYTYPE_LOOP, TRUE);
						BGM->Get(2).SetVol_Local(static_cast<int>(255 * this->m_CautionBGM));
					}
				}
				else {
					bool IsPlay = (this->m_CautionBGM > 0.f);
					this->m_CautionBGM = std::max(this->m_CautionBGM - 1.f / DrawParts->GetFps(), 0.f);
					if (this->m_CautionBGM > 0.f) {
						BGM->Get(2).SetVol_Local(static_cast<int>(255 * this->m_CautionBGM));
					}
					else {
						if (IsPlay) {
							BGM->Get(2).Stop();
						}
					}
				}
				//
				if (this->m_IsAlertBGM) {
					if (this->m_AlertBGM < 1.f) {
						this->m_AlertBGM = 1.f;
						BGM->Get(3).Play(DX_PLAYTYPE_LOOP, TRUE);
						BGM->Get(3).SetVol_Local(static_cast<int>(255 * this->m_AlertBGM));
					}
				}
				else {
					bool IsPlay = (this->m_AlertBGM > 0.f);
					this->m_AlertBGM = std::max(this->m_AlertBGM - 1.f / DrawParts->GetFps(), 0.f);
					if (this->m_AlertBGM > 0.f) {
						BGM->Get(3).SetVol_Local(static_cast<int>(255 * this->m_AlertBGM));
					}
					else {
						if (IsPlay) {
							BGM->Get(3).Stop();
						}
					}
				}
			}
			//イベント等制御
			{
				auto Prev = this->m_IsPlayable;
				this->m_IsPlayable = FadeControl::IsFadeClear();
				if (CutSceneControl::IsCutScene()) {
					this->m_IsPlayable = false;
					//射撃能力
					if (Chara) {
						Chara->SetShootTimer(0.f);
						if (SaveDataParts->GetParam("Cut_10") != -1) {
							Chara->SetShootTimer(1.f);
						}
						if (SaveDataParts->GetParam("Cut_20") != -1) {
							Chara->SetShootTimer(0.1f);
						}
						if (SaveDataParts->GetParam("Cut_30") != -1) {
							Chara->SetShootTimer(5.f);
						}
					}
				}
				if (Prev != this->m_IsPlayable) {
					Pad->SetGuideUpdate();
					if (this->m_IsPlayable && (m_BossID != INVALID_ID) && (m_WinCutSceneID == INVALID_ID)) {
						this->m_IsEnd = true;
						this->m_IsGoodEnd = true;
						SaveDataParts->Reset();
					}
				}
				if (PauseMenuControl::IsRetire()) {
					this->m_IsEnd = true;
				}
				if (!Chara) {
					if (!m_IsBadEnd) {
						CutSceneControl::StartCutScene(999);
						this->m_IsBadEnd = true;
					}
					else {
						if (!CutSceneControl::IsCutScene()) {
							this->m_IsEnd = true;
						}
					}
				}
				FadeControl::SetBlackOut(this->m_IsEnd || this->m_IsGoNext);
				if (this->m_IsPlayable) {
					m_StartTime += 1.f / DrawParts->GetFps();
					InGameUIControl::SetStartTime(m_StartTime);
				}
			}
			//
			Effect2DControl::Instance()->Update();
			// 
			BackGround->Execute();
			// 入力制御
			InputControl MyInput;
			float InputRad = 0.f;
			for (int i = 0;i < PlayerMngr->GetPlayerNum();i++) {
				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				if (p->GetChara()) {
					MyInput.ResetAllInput();
					InputRad = 0.f;
					if (this->m_IsPlayable) {
						if (i == this->m_MyPlayerID) {
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
							if (std::abs(Pad->GetLS_X()) > 0.1f || std::abs(Pad->GetLS_Y()) > 0.1f) {
								InputRad = std::atan2f(Pad->GetLS_X(), Pad->GetLS_Y());
							}
							else if (std::abs(p->GetChara()->GetVec().x) > 0.1f || std::abs(p->GetChara()->GetVec().y) > 0.1f) {
								InputRad = std::atan2f(p->GetChara()->GetVec().x, p->GetChara()->GetVec().y);
							}
							else {
								InputRad = p->GetChara()->GetViewRad();
							}
							p->GetChara()->SetColTarget(ColTarget::All);
						}
						else {
							p->GetAI()->Execute(&MyInput);
							InputRad = p->GetAI()->GetViewRad();
							if (!p->GetChara()->CanLookPlayer0()) {
								p->GetChara()->SetColTarget(ColTarget::None);
							}
							else {
								p->GetChara()->SetColTarget(ColTarget::All);
							}
						}
					}
					else {
						p->GetChara()->SetColTarget(ColTarget::None);
					}
					p->GetChara()->ExecuteInput(&MyInput);
					p->GetChara()->SetViewRad(InputRad);
				}
			}
			Obj2DParts->ExecuteObject();
			//死亡制御
			{
				auto* ResourceParts = CommonBattleResource::Instance();

				for (int i = 0;i < PlayerMngr->GetPlayerNum();i++) {
					auto& p = PlayerMngr->GetPlayer((PlayerID)i);
					if (p->GetChara()) {
						if (p->GetChara()->GetIsDelete()) {
							ResourceParts->DelCharacter((PlayerID)i);
						}
					}
				}
			}
			// カメラ制御
			this->m_CamAddPos.Set(0.f, 0.f, 0.f);
			if (Chara) {
				if (this->m_IsPlayable && Pad->GetKey(PADS::AIM).press()) {
					float ViewLimit = Get2DSize(10.f);
					this->m_CamAddPos.Set(std::sin(Chara->GetBodyRad()) * ViewLimit, std::cos(Chara->GetBodyRad()) * ViewLimit, 0.f);
				}
				if (CutSceneControl::IsCutScene()) {
					auto MyIndex = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));

					MyIndex.first += CutSceneControl::GetAddViewPointX();
					MyIndex.second += CutSceneControl::GetAddViewPointY();
					this->m_CamAddPos = BackGround->GetFloorData(BackGround->GetXYToNum(MyIndex.first, MyIndex.second))->GetPos() - Chara->GetPos();
				}
				Easing(&this->m_CamAddPosR, this->m_CamAddPos, 0.9f, EasingType::OutExpo);
				Cam2D->SetCamAim(Chara->GetPos() + this->m_CamAddPosR);
			}
			Cam2D->SetCamRangeAim(BackGround->GetCamScale());
			Cam2D->UpdateShake();
			// 
			if (Chara) {
				BackGround->SetPointLight(Chara->GetPos());
			}
			BackGround->SetAmbientLight(120.f, deg2rad(30));
			BackGround->SetupShadow([]() {
				auto* Obj2DParts = Object2DManager::Instance();
				Obj2DParts->DrawShadow();
									});

			InGameUIControl::UpdateUI();
			if (FadeControl::IsFadeClear() && Chara) {
				//範囲に入ったらイベント
				auto MyIndex = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));
				auto IsNearIndex = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
					return  (std::abs(a.first - b.first) <= 3 && std::abs(a.second - b.second) <= 3);
				};
				if (!IsNearIndex(this->m_PrevXY, MyIndex)) {
					this->m_PrevXY = MyIndex;
					if (this->m_IsPlayable) {
						for (auto& e : BackGround->GetEventChip()) {
							if (IsNearIndex(BackGround->GetNumToXY(e.m_index), MyIndex)) {//該当のチップに踏み込んだ
								//次マップへの遷移
								if (e.m_EventType == EventType::Entry) {
									if (e.m_EntryID!= INVALID_ID) {
										this->m_MapName = e.m_MapName;
										this->m_EntryID = e.m_EntryID;
										this->m_CutSceneID = e.m_CutSceneID;
										this->m_IsGoNext = true;
										break;
									}
								}
								//
								if (e.m_EventType == EventType::CutScene) {
									if (static_cast<int>(m_StartTime) > e.m_ActiveDelaySec) {
										auto* SaveDataParts = SaveDataClass::Instance();
										std::string SaveStr = "Cut_" + std::to_string(e.m_CutSceneID);
										if (SaveDataParts->GetParam(SaveStr) == -1) {
											SaveDataParts->SetParam(SaveStr, 0);
											CutSceneControl::StartCutScene(e.m_CutSceneID);
										}
										break;
									}
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
			auto* BGM = BGMPool::Instance();
			BGM->Get(1).Stop();
			BGM->Get(2).Stop();
			BGM->Get(3).Stop();
			// リソース
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				if (p->GetChara()) {
					ResourceParts->DelCharacter((PlayerID)i);
				}
			}
			PlayerMngr->Dispose();
			BackGround->Dispose();
			// セーブ
			if(!this->m_IsGoodEnd){
				//最後に訪れたマップ
				SaveDataParts->SetParam("LastMap", std::stoi(this->m_MapName.substr(3)));
				SaveDataParts->SetParam("LastEntry", this->m_EntryID);
			}
			SaveDataParts->Save();
			if (this->m_IsEnd) {//タイトルに戻る
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
			Effect2DControl::Instance()->Draw();
			BackGround->DrawFront();
			InGameUIControl::DrawUI_Front();

			if (m_BossID != INVALID_ID) {
				auto* Obj = Obj2DParts->GetObj(m_BossID);
				if (Obj) {
					auto& P = (std::shared_ptr<MetalObject>&)(*Obj);
					Vector3DX DispPos;
					Convert2DtoDisp(P->GetPos(), &DispPos);
					int xmin = y_r(-50);
					int ymin = y_r(-50);
					int xmax = y_r(50);

					int xper = xmin + (xmax - xmin) * P->GetHitPoint() / P->GetHitPointMax();
					DrawLine(static_cast<int>(DispPos.x + xmin), static_cast<int>(DispPos.y + ymin), static_cast<int>(DispPos.x + xmax), static_cast<int>(DispPos.y + ymin), Gray75, 10);
					DrawLine(static_cast<int>(DispPos.x + xmin), static_cast<int>(DispPos.y + ymin), static_cast<int>(DispPos.x + xper), static_cast<int>(DispPos.y + ymin), Green, 10);
				}
			}
		}
		void			MainGameScene::DrawUI_Base_Sub(void) noexcept {
			InGameUIControl::DrawUI_MapName();
			FadeControl::DrawFade();
			CutSceneControl::DrawCut();
		}
		void			MainGameScene::DrawUI_In_Sub(void) noexcept {
			PauseMenuControl::DrawPause();
		}
	};
};
