#include	"MainGameScene.hpp"

#include	"../sub.hpp"

#include	"../MainScene/Player/Player.hpp"
#include	"../MainScene/BackGround/BackGround.hpp"
#include	"../CommonScene/Object/Object2DManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//ポーズ画面
		void MainGameScene::LoadPause(void) noexcept 
		{
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton("Retire", 48, true, 1920 - 64, 1080 - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Option", 48, true, 1920 - 64, 1080 - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Return Game", 48, true, 1920 - 64, 1080 - 84 - 64 * 0, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
		}
		void MainGameScene::UpdatePause(void) noexcept 
		{
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			if (DrawParts->IsPause()) {
				if (!DrawParts->IsExit() && !DrawParts->IsRestart()) {
					Pad->SetMouseMoveEnable(false);
					Pad->ChangeGuide(
						[&]() {
							auto* KeyGuide = PadControl::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));
							KeyGuide->AddGuide(PADS::RELOAD, LocalizeParts->Get(9991));
							KeyGuide->AddGuide(PADS::MOVE_W, "");
							KeyGuide->AddGuide(PADS::MOVE_S, "");
							KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
						});
					if (!OptionWindowClass::Instance()->IsActive()) {
						ButtonParts->UpdateInput();
						// 選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
							case 0:
								m_IsEnd = true;
								DrawParts->SetPause(false);
								break;
							case 1:
								OptionWindowClass::Instance()->SetActive();
								break;
							case 2:
								DrawParts->SetPause(false);
								break;
							default:
								DrawParts->SetPause(false);
								break;
							}
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetKey(PADS::RELOAD).trigger()) {
							SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
							DrawParts->SetPause(false);
						}
						// 
						ButtonParts->Update();
					}
				}
			}
			else {
				ButtonParts->ResetSel();
			}
		}
		void MainGameScene::DrawPause(void) noexcept 
		{
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				ButtonParts->Draw();
			}
		}
		void MainGameScene::DisposePause(void) noexcept 
		{
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
		}
		//UI
		void MainGameScene::LoadUI(void) noexcept {
			m_Watch = GraphHandle::Load("data/UI/Watch.png");
			m_Caution = GraphHandle::Load("data/UI/Caution.png");
			m_Alert = GraphHandle::Load("data/UI/Alert.png");
			m_ViewHandle = GraphHandle::Make(y_r(1920), y_r(1080), true);
		}
		void MainGameScene::DrawCharaUI_Back(PlayerID value) noexcept 
		{
			auto* PlayerMngr = PlayerManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			if (p.GetChara()) {
				auto DispPos = Convert2DtoDisp(p.GetChara()->GetPos());
				double Deg = (double)p.GetChara()->GetViewRad() / (DX_PI * 2.0) * 100.0;
				double Watch;
				if (value == m_MyPlayerID) {
					SetDrawBright(0, 0, 216);
					Watch = 15.0 / 360.0 * 100.0;
				}
				else {
					if (p.GetAI()->IsAlert()) {
						SetDrawBright(216, 0, 0);// 
					}
					else if (p.GetAI()->IsCaution()) {
						SetDrawBright(216, 216, 0);// 
					}
					else {
						SetDrawBright(0, 216, 0);// 
					}
					Watch = 45.0 / 360.0 * 100.0;
				}
				DrawCircleGauge((int)DispPos.x, (int)DispPos.y, Deg + Watch, m_Watch.get(), Deg - Watch, (double)GetDispSize(5.f) / 64.0);
			}
		}
		void MainGameScene::DrawCharaUI_Front(PlayerID value) noexcept 
		{
			auto* PlayerMngr = PlayerManager::Instance();
			auto& p = PlayerMngr->GetPlayer(value);
			if (p.GetChara() && p.GetChara()->CanLookPlayer0()) {
				if (p.GetAI()->GetGraphSwitch()) {
					p.GetChara()->SetGraphTimer(2.f);
				}
				if (p.GetChara()->GetGraphTimer() <= 0.f) { return; }

				float Per = 1.f;
				float StartTimer = 0.25f;
				if (p.GetChara()->GetGraphTimer() > 2.f - StartTimer) {
					Per = 1.f - (p.GetChara()->GetGraphTimer() - (2.f - StartTimer)) / StartTimer;
				}
				float EndTimer = 0.25f;
				if (p.GetChara()->GetGraphTimer() < 0.25f) {
					Per = (p.GetChara()->GetGraphTimer() - (0.25f - EndTimer)) / EndTimer;
				}

				auto DispPos = Convert2DtoDisp(p.GetChara()->GetPos());
				int ShadowOfset = y_r(3);
				float Scale = (float)y_r(128) / 128.0f * Per;
				if (p.GetAI()->IsAlert()) {
					SetDrawBright(0, 0, 0);// 
					m_Alert.DrawRotaGraph((int)DispPos.x + ShadowOfset, (int)DispPos.y - y_r(32) + ShadowOfset, Scale, 0.f, true);
					SetDrawBright(255, 0, 0);// 
					m_Alert.DrawRotaGraph((int)DispPos.x, (int)DispPos.y - y_r(32), Scale, 0.f, true);
				}
				else if (p.GetAI()->IsCaution()) {
					SetDrawBright(0, 0, 0);// 
					m_Caution.DrawRotaGraph((int)DispPos.x + ShadowOfset, (int)DispPos.y - y_r(32) + ShadowOfset, Scale, 0.f, true);
					SetDrawBright(255, 255, 0);// 
					m_Caution.DrawRotaGraph((int)DispPos.x, (int)DispPos.y - y_r(32), Scale, 0.f, true);
				}
				else {
					SetDrawBright(0, 0, 0);// 
					m_Caution.DrawRotaGraph((int)DispPos.x + ShadowOfset, (int)DispPos.y - y_r(32) + ShadowOfset, Scale, 0.f, true);
					SetDrawBright(0, 255, 0);// 
					m_Caution.DrawRotaGraph((int)DispPos.x, (int)DispPos.y - y_r(32), Scale, 0.f, true);
				}
			}
		}
		void MainGameScene::Dispose_LoadUI(void) noexcept {
			m_ViewHandle.Dispose();
			m_Watch.Dispose();
			m_Caution.Dispose();
			m_Alert.Dispose();
		}
		void MainGameScene::SetupWatchScreen(void) noexcept {
			auto* BackGround = BackGroundClassBase::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* OptionParts = OPTION::Instance();
			m_ViewHandle.SetDraw_Screen(false);
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, true);
				// 視界
				for (int loop = 0; loop < 4; loop++) {
					for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
						auto& p = PlayerMngr->GetPlayer((PlayerID)i);
						if (i == m_MyPlayerID) {
							if (loop == 3) {
								DrawCharaUI_Back((PlayerID)i);
							}
						}
						else {
							if (p.GetAI()->IsAlert()) {
								if (loop == 2) {
									DrawCharaUI_Back((PlayerID)i);
								}
							}
							else if (p.GetAI()->IsCaution()) {
								if (loop == 1) {
									DrawCharaUI_Back((PlayerID)i);
								}
							}
							else {
								if (loop == 0) {
									DrawCharaUI_Back((PlayerID)i);
								}
							}
						}
					}
				}
				SetDrawBright(255, 255, 255);
				if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
					SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
					BackGround->GetShadowGraph().DrawExtendGraph(0, 0, y_r(1920), y_r(1080), false);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
			}
		}
		// ロード
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* ResourceParts = CommonBattleResource::Instance();
			// 共通リソース
			ResourceParts->Load();
			// UI
			LoadUI();
			// ポーズ
			LoadPause();
			//初期マップtodo:セーブデータより
			m_MapName = "map0";
			m_EntryID = 0;
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Cam2D = Cam2DControl::Instance();
			auto* ResourceParts = CommonBattleResource::Instance();
			//リソースセット
			ResourceParts->Set();
			//ステージ
			BackGround->Init(m_MapName);
			// キャラマネ
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
			// 
			m_IsCutScene = false;
			m_CutSceneAlpha = 0.f;
			m_IsGoNext = false;
			m_IsEnd = false;
			m_BlackOutAlpha = 1.f;
			m_MapDrawTime = 5.f;
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG

			auto* PlayerMngr = PlayerManager::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Cam2D = Cam2DControl::Instance();

			auto& Chara = PlayerMngr->GetPlayer(m_MyPlayerID).GetChara();

			UpdatePause();
			if (DrawParts->IsPause()) {
				return true;
			}
			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					if (m_IsPlayable) {
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
					}
					else if (m_IsCutScene) {
						KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9902));
					}
				});
			m_CutSceneAlpha = std::clamp(m_CutSceneAlpha + (m_IsCutScene ? 1.f : -1.f) / DrawParts->GetFps() / 0.5f, 0.f, 1.f);
			m_BlackOutAlpha = std::clamp(m_BlackOutAlpha + ((m_IsEnd || m_IsGoNext) ? 1.f : -1.f) / DrawParts->GetFps() / 0.5f, 0.f, 1.f);
			{
				auto Prev = m_IsPlayable;
				m_IsPlayable = (m_BlackOutAlpha == 0.f && !m_IsCutScene);
				if (Prev != m_IsPlayable) {
					Pad->SetGuideUpdate();
				}
			}
			if ((m_IsEnd|| m_IsGoNext) && m_BlackOutAlpha >= 1.f) {
				return false;
			}
			// 
			BackGround->Execute();
			// 
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
						}
					}
					p.GetChara()->ExecuteInput(&MyInput);
					p.GetChara()->SetViewRad(InputRad);
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("----------");
#endif // DEBUG
			Obj2DParts->ExecuteObject();
#ifdef DEBUG
			DebugParts->SetPoint("----------");
#endif // DEBUG
			// 
			Vector3DX CamPos = Vector3DX::zero();
			if (Pad->GetKey(PADS::AIM).press()) {
				float ViewLimit = Get2DSize(10.f);
				CamPos.Set(std::sin(Chara->GetBodyRad())* ViewLimit, std::cos(Chara->GetBodyRad())* ViewLimit, 0.f);
			}
			Easing(&m_CamAddPos, CamPos, 0.9f, EasingType::OutExpo);
			Cam2D->SetCamAim(Chara->GetPos() + m_CamAddPos);
			// 
			BackGround->SetPointLight(Chara->GetPos());
			BackGround->SetAmbientLight(120.f, deg2rad(30));
			BackGround->SetupShadow([&]() {
				auto* Obj2DParts = Object2DManager::Instance();
				Obj2DParts->DrawShadow();
									});

			m_MapDrawTime = std::max(m_MapDrawTime - 1.f / DrawParts->GetFps(), 0.f);

			if (m_BlackOutAlpha == 0.f) {

				m_MapDrawPer = 1.f;
				float StartTimer = 0.5f;
				if (m_MapDrawTime > 5.f - StartTimer) {
					m_MapDrawPer = 1.f - (m_MapDrawTime - (5.f - StartTimer)) / StartTimer;
				}
				float EndTimer = 0.5f;
				if (m_MapDrawTime < 0.5f) {
					m_MapDrawPer = (m_MapDrawTime - (0.5f - EndTimer)) / EndTimer;
				}
				m_MapDrawPer = std::clamp(m_MapDrawPer, 0.f, 1.f);

				auto MyIndex = BackGround->GetNumToXY(BackGround->GetNearestFloors(Chara->GetPos()));
				//範囲
				auto IsNearIndex = [&](const std::pair<int, int>& a, const std::pair<int, int>& b) {
					return  (abs(a.first - b.first) <= 1 && abs(a.second - b.second) <= 1);
				};

				if (!IsNearIndex(m_PrevXY, MyIndex)) {
					m_PrevXY = MyIndex;
					for (auto& e : BackGround->GetEventChip()) {
						if (IsNearIndex(BackGround->GetNumToXY(e.m_index), MyIndex)) {
							//次マップへの遷移
							if (e.m_EventType == EventType::Entry) {
								m_MapName = e.m_MapName;
								m_EntryID = e.m_EntryID;
								m_IsGoNext = true;
								break;
							}
							//
						}
					}
				}
				//todo:カットシーン
				//m_IsCutScene = true;
			}
			SetupWatchScreen();
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
			SaveDataParts->Save();
			if (m_IsEnd) {//タイトルに戻る
				SetNextSelect(0);
			}
			else {//次のシーンへ
				SetNextSelect(1);
			}
		}
		void			MainGameScene::MainDraw_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* Obj2DParts = Object2DManager::Instance();
			// 
			BackGround->Draw();
			//
			SetDrawBlendMode(DX_BLENDMODE_MULA, 92);
			m_ViewHandle.DrawGraph(0, 0, false);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			// 
			Obj2DParts->Draw();
			// 
			BackGround->DrawFront();
			//
			for (int i = 1;i < PlayerMngr->GetPlayerNum();i++) {
				// !マーク
				DrawCharaUI_Front((PlayerID)i);
				//ID用デバッグ描画
				if (false) {
					SetDrawBright(255, 255, 255);
					auto& p = PlayerMngr->GetPlayer((PlayerID)i);
					p.GetAI()->Draw();
				}
			}
			SetDrawBright(255, 255, 255);
		}
		void			MainGameScene::DrawUI_Base_Sub(void) noexcept {
			auto* Fonts = FontPool::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			// 
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_MapDrawPer), 0, 255));
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(64), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																	y_UI(64), y_UI(128), White, Black, LocalizeParts->Get(8000));
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			// 
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_BlackOutAlpha), 0, 255));
				DrawBox(0, 0, y_UI(1920), y_UI(1080), Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//
			if (m_IsCutScene) {
				auto Color = GetColor(16, 16, 16);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_CutSceneAlpha), 0, 255));
				DrawBox(0, 0, y_UI(1920), y_UI(120), Color, TRUE);
				DrawBox(0, y_UI(1080 - 120), y_UI(1920), y_UI(1080), Color, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}

		void			MainGameScene::DrawUI_In_Sub(void) noexcept {
			// ポーズ
			DrawPause();
		}
		// 使い回しオブジェ系
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto* Obj2DParts = Object2DManager::Instance();
			auto* ResourceParts = CommonBattleResource::Instance();
			Obj2DParts->DeleteAll();
			ResourceParts->Dispose();
			DisposePause();
			Dispose_LoadUI();
		}
	};
};
