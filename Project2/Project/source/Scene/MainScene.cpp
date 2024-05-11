#include "MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//BG
			this->m_BackGround->Load();
			//
			this->m_hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			this->m_aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
			this->m_scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");
			//
			BattleResourceMngr->Load();
			PlayerMngr->Init(Vehicle_num);
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			//
			BattleResourceMngr->Set();
			//
			this->m_BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(-0.8f, -0.5f, -0.1f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(0.92f, 0.91f, 0.90f, 1.0f));
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 0);
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 1);
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 2);
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//
			{
				auto Hind = std::make_shared<HindDClass>();
				ObjMngr->AddObject(Hind);
				ObjMngr->LoadModel(Hind, Hind, "data/model/hindD/");

				Hind->Init();

				Hind->SetMove(Matrix4x4DX::identity(), Vector3DX::vget(0.f, 30.f*Scale_Rate, 0.f));
				Hind->GetObj().get_anime(0).per = 1.f;
			}
			//player
			for (int i = 0; i < Vehicle_num; i++) {
				PlayerMngr->GetPlayer(i).SetVehicle(std::make_shared<VehicleClass>());
				auto& Vehicle = PlayerMngr->GetPlayer(i).GetVehicle();
				ObjMngr->AddObject(Vehicle);
				Vehicle->SetMapCol(this->m_BackGround);
				Vehicle->ObjectBaseClass::Init();
				//ポジション指定
				int ID = 0;
				for (int i = 0;i < 10;i++) {
					ID = GetRand(this->m_BackGround->GetRoadPointNum() - 1);
					auto* Mat = this->m_BackGround->GetRoadPoint(ID);
					Vector3DX pos_t = Mat->pos(); pos_t.y = 0.f;
					if (abs(pos_t.x) < 150.f*Scale_Rate && abs(pos_t.z) < 150.f*Scale_Rate) {
						break;
					}
				}
				auto* Mat = this->m_BackGround->GetRoadPoint(ID);
				Vector3DX pos_t = Mat->pos(); pos_t.y = 0.f;
				float rad_t = std::atan2f(Mat->zvec().x, -Mat->zvec().z);
				auto pos_t2 = pos_t - Vector3DX::up() * (100.f * Scale_Rate);
				if (this->m_BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * (100.f * Scale_Rate), &pos_t2, true, false)) {
					pos_t = pos_t2;
				}

				auto& vehc_datas = VehDataControl::Instance()->GetVehData();
				int dataID = GetRand((int)vehc_datas.size() - 1);

				ObjMngr->LoadModel(Vehicle, nullptr, ("data/tank/" + vehc_datas[dataID].GetName() + "/").c_str());

				Vehicle->ValueInit(dataID, this->m_BackGround->GetBox2Dworld(), (PlayerID)i);
				Vehicle->MovePoint(deg2rad(0), rad_t, pos_t);
				//インベントリ
				for (int loop = 0; loop < 5; loop++) {
					PlayerMngr->GetPlayer(i).SetInventory(loop, Vehicle->GetData().GetInventoryXSize(loop), Vehicle->GetData().GetInventoryYSize(loop));
				}
				{
					if (Vehicle->Get_Gunsize() >= 2) {
						PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetData()->GetAmmoSpec().at(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
						PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[1].GetData()->GetAmmoSpec().at(0), PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
					}
					else {
						PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetData()->GetAmmoSpec().at(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
					}
					PlayerMngr->GetPlayer(i).FillInventory(1, Vehicle->GetGun()[0].GetData()->GetAmmoSpec().at(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(1), 6);
				}
				{
					PlayerMngr->GetPlayer(i).FillInventory(2, Vehicle->GetData().GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(2), PlayerMngr->GetPlayer(i).GetInventoryYSize(2));
					PlayerMngr->GetPlayer(i).FillInventory(3, Vehicle->GetData().GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(3), PlayerMngr->GetPlayer(i).GetInventoryYSize(3));
				}
				{
					auto& iData = ItemDataControl::Instance()->GetData();
					auto Select = std::find_if(iData.begin(), iData.end(), [&](const std::shared_ptr<ItemData>& d) {return (d->GetPath().find("DieselMiniTank") != std::string::npos); });
					if (Select != iData.end()) {
						for (int x = 0; x < 5; x++) {
							PlayerMngr->GetPlayer(i).PutInventory(4, x * 2, 0, *Select, -1, false);
						}
					}
				}
				//AI
				this->m_AICtrl[i]->Init(this->m_BackGround, (PlayerID)i);
			}
			//UI
			this->m_UIclass.Set();
			this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetVehicle()->GetHP();
			this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
			//
			this->m_DamageEvents.clear();
			this->m_NetWorkBrowser.Init();
			this->m_InventoryClass.Init();
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(!DXDraw::Instance()->IsPause());
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					if (DXDraw::Instance()->IsPause()) {
						KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					}
					else {
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_A, "");
						KeyGuide->AddGuide(PADS::MOVE_D, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9900));
						KeyGuide->AddGuide(PADS::LEAN_L, "");
						KeyGuide->AddGuide(PADS::LEAN_R, LocalizePool::Instance()->Get(9901));
						KeyGuide->AddGuide(PADS::RUN, LocalizePool::Instance()->Get(9902));
						KeyGuide->AddGuide(PADS::WALK, LocalizePool::Instance()->Get(9903));
						KeyGuide->AddGuide(PADS::SQUAT, LocalizePool::Instance()->Get(9904));
						KeyGuide->AddGuide(PADS::JUMP, LocalizePool::Instance()->Get(9905));
						KeyGuide->AddGuide(PADS::SHOT, LocalizePool::Instance()->Get(9906));
						KeyGuide->AddGuide(PADS::ULT, LocalizePool::Instance()->Get(9907));
						KeyGuide->AddGuide(PADS::AIM, LocalizePool::Instance()->Get(9908));
						KeyGuide->AddGuide(PADS::MELEE, LocalizePool::Instance()->Get(9909));
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9910));
						KeyGuide->AddGuide(PADS::CHECK, LocalizePool::Instance()->Get(9911));
						KeyGuide->AddGuide(PADS::THROW, LocalizePool::Instance()->Get(9912));
						KeyGuide->AddGuide(PADS::INVENTORY, LocalizePool::Instance()->Get(9913));
					}
				});
			this->m_InventoryClass.FirstExecute();
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			else {
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->GetDispXSize() / 2, DXDraw::Instance()->GetDispYSize() / 2);
				SE->Get((int)SoundEnum::Environment).Play(0, DX_PLAYTYPE_LOOP, TRUE);
				this->m_fov_base = DrawParts->GetMainCamera().GetCamFov();
			}
			//Input,AI
			{
				float cam_per = ((DrawParts->GetMainCamera().GetCamFov() / deg2rad(75)) / (DrawParts->is_lens() ? DrawParts->zoom_lens() : 1.f)) / 100.f;
				cam_per *= 0.6f;
				float pp_x = 0.f, pp_y = 0.f;
				InputControl MyInput;
				//
				//cam_per /= std::max(1.f, std::hypotf(Pad->GetLS_Y(), Pad->GetLS_X()));

				pp_x = std::clamp(Pad->GetLS_Y() * cam_per*0.5f, -0.2f, 0.2f);
				pp_y = std::clamp(Pad->GetLS_X() * cam_per*0.5f, -0.2f, 0.2f);
				MyInput.SetInputStart(pp_x, pp_y);
				MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
				MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
				MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
				MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());
				MyInput.SetInputPADS(PADS::RUN, Pad->GetKey(PADS::RUN).press());
				MyInput.SetInputPADS(PADS::LEAN_L, Pad->GetKey(PADS::LEAN_L).press());
				MyInput.SetInputPADS(PADS::LEAN_R, Pad->GetKey(PADS::LEAN_R).press());
				MyInput.SetInputPADS(PADS::MELEE, Pad->GetKey(PADS::MELEE).press());
				MyInput.SetInputPADS(PADS::RELOAD, Pad->GetKey(PADS::RELOAD).press());
				MyInput.SetInputPADS(PADS::INTERACT, Pad->GetKey(PADS::INTERACT).press());
				MyInput.SetInputPADS(PADS::SQUAT, Pad->GetKey(PADS::SQUAT).press());
				MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press() && !DXDraw::Instance()->IsPause());
				MyInput.SetInputPADS(PADS::AIM, Pad->GetKey(PADS::AIM).press() && !DXDraw::Instance()->IsPause());
				MyInput.SetInputPADS(PADS::ULT, Pad->GetKey(PADS::ULT).press());
				MyInput.SetInputPADS(PADS::THROW, Pad->GetKey(PADS::THROW).press());
				MyInput.SetInputPADS(PADS::CHECK, Pad->GetKey(PADS::CHECK).press());
				MyInput.SetInputPADS(PADS::WALK, Pad->GetKey(PADS::WALK).press());
				MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());
				//スコープ
				{
					if (!Vehicle->IsAlive()) {
						auto OLD = this->m_TPSLen;
						this->m_zoom = 2.0f;
						this->m_TPSLen = 8;
						this->m_changeview = ((this->m_TPSLen != OLD) && (this->m_TPSLen == 0));
					}
					else {
						this->m_changeview = false;
						if (this->m_TPSLen == 0) {
							this->m_zoom = std::clamp(this->m_zoom + float(GetMouseWheelRotVolWithCheck()) * 2.0f, 0.0f, 30.f);
							if (this->m_zoom == 0.f) {
								this->m_TPSLen = 1;
								this->m_range_r = (float)this->m_TPSLen;
								this->m_changeview = true;
							}
						}
						else {
							this->m_TPSLen = std::clamp(this->m_TPSLen - GetMouseWheelRotVolWithCheck(), 0, 6);
							if (this->m_TPSLen == 0) {
								this->m_zoom = 2.0f;
								this->m_changeview = true;
							}
						}
					}
					Easing(&this->m_range_r, (float)this->m_TPSLen *8.f / 6.f, 0.8f, EasingType::OutExpo);

					bool IsADS = (this->m_TPSLen == 0);
					Vehicle->SetActive(!IsADS);
					{
						auto RadAdd = Vehicle->GetRadAdd() - m_VehRadAdd;
						m_VehRadAdd = Vehicle->GetRadAdd();
						{
							auto Base = Pad->GetLS_X()*1.f + RadAdd.z*1000.f;
							Easing(&this->m_XScope, Base, 0.95f, EasingType::OutExpo);
						}
						{
							auto Base = Pad->GetLS_Y()*1.f + RadAdd.x*1000.f;
							Easing(&this->m_YScope, -Base, 0.9f, EasingType::OutExpo);
						}
						if (m_IsChangeView != IsADS) {
							this->m_ChangeViewPer = 1.f;
						}
						else {
							Easing(&this->m_ChangeViewPer, 0.f, 0.95f, EasingType::OutExpo);
						}
						m_IsChangeView = IsADS;
					}
				}
				/*
				//ネットワーク
				SendInfo ans;
				ans.m_Pos = Vehicle->GetMove().pos;
				ans.m_Vec = Vehicle->GetMove().vec;
				ans.m_rad.y = (Vehicle->Get_body_yrad());
				ans.m_Damage = &Vehicle->GetDamageEvent();
				ans.m_DamageSwitch = (Vehicle->GetDamageSwitch() ? 1 : 0);
				this->m_NetWorkBrowser.FirstExecute(MyInput, ans);
				//*/
				//クライアント
				if (m_NetWorkBrowser.GetClient()) {
					/*
					for (int i = 0; i < Vehicle_num; i++) {
						auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
						if (v->GetMyPlayerID() == GetMyPlayerID()) {
							v->SetCharaType(CharaTypeID::Mine);
						}
						else {
							v->SetCharaType(CharaTypeID::Enemy);
						}
					}
					//*/
				}
				//
				bool isready = true;
				for (int i = 0; i < Vehicle_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					if (this->m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = m_NetWorkBrowser.GetNowServerPlayerData(i, true);
						if (i == GetMyPlayerID()) {
							MyInput.SetKeyInputFlags(tmp.Input);//キーフレームだけサーバーに合わせる
							v->SetInput(MyInput, isready && v->IsAlive());
							this->m_NetWorkBrowser.GetRecvData(i, tmp.GetFrame());
						}
						else {
							if (!m_NetWorkBrowser.GetClient()) {
								m_AICtrl[i]->AI_move(&tmp.Input);
							}
							v->SetInput(tmp.Input, isready && v->IsAlive(), true);
							bool override_true = true;
							for (int i2 = 0; i2 < Vehicle_num; i2++) {
								auto& v2 = PlayerMngr->GetPlayer(i2).GetVehicle();
								if ((v != v2) && ((v->GetMove().pos - v2->GetMove().pos).magnitude() <= 10.f*Scale_Rate)) {
									override_true = false;
									break;
								}
							}
							if (override_true) {
								v->SetPosBufOverRide(tmp.m_move);
							}

						}
						//ダメージイベント処理
						for (auto& e : tmp.m_DamageEvents) {
							this->m_DamageEvents.emplace_back(e);
						}
					}
					else {
						if (i == GetMyPlayerID()) {
							v->SetInput(MyInput, isready && v->IsAlive());
						}
						else {
							InputControl OtherInput;
							m_AICtrl[i]->AI_move(&OtherInput);//めっちゃ重い
							v->SetInput(OtherInput, isready && v->IsAlive());
						}
						//ダメージイベント処理
						for (const auto& e : v->GetDamageEvent()) {
							this->m_DamageEvents.emplace_back(e);
						}
						v->GetDamageEvent().clear();
					}
				}
				this->m_NetWorkBrowser.LateExecute();
				this->m_InventoryClass.LateExecute();
				//ダメージイベント
				for (int i = 0; i < Vehicle_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					for (int j = 0; j < this->m_DamageEvents.size(); j++) {
						if (v->SetDamageEvent(this->m_DamageEvents[j])) {
							std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[j]);
							this->m_DamageEvents.pop_back();
							j--;
						}
					}
				}
			}
			//レーザーサイト
			{
				auto StartPos = Vehicle->GetGunMuzzlePos(0);
				auto EndPos = StartPos + Vehicle->GetGunMuzzleVec(0) * 100.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true, false);
				for (int i = 0; i < Vehicle_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					if (!v->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) { continue; }
					v->GetColNearestInAllMesh(StartPos, &EndPos);
				}
				Vehicle->SetAimingDistance((StartPos - EndPos).magnitude());
			}
			//アイテムのアタッチ
			{
				int loop = 0;
				while (true) {
					auto item = ObjMngr->GetObj((int)ObjType::Item, loop);
					if (item) {
						auto& ip = (std::shared_ptr<ItemClass>&)(*item);
						if (!ip->IsActive()) {
							ip->SetActive(true);
						}
					}
					else {
						break;
					}
					loop++;
				}
			}
			//Execute
			ObjMngr->ExecuteObject();
			//弾の更新
			{
				m_Concussion = std::max(m_Concussion - 1.f / DrawParts->GetFps(), 0.f);

				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj((int)ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);

						if (a->IsActive()) {
							//AmmoClass
							Vector3DX repos_tmp = a->GetMove().repos;
							Vector3DX pos_tmp = a->GetMove().pos;
							if (GetMyPlayerID() != a->GetShootedID()) {
								if (GetMinLenSegmentToPoint(repos_tmp, pos_tmp, DrawParts->GetMainCamera().GetCamPos()) < 5.f*Scale_Rate) {
									m_Concussion = 1.f;
								}
							}

							Vector3DX norm_tmp;
							bool ColRes = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, false, &norm_tmp);
							ColRes |= this->m_BackGround->GetWallCol(repos_tmp, &pos_tmp, &norm_tmp, a->GetCaliberSize());
							bool is_HitAll = false;
							//auto& v = *ObjMngr->GetObj((int)ObjType::Vehicle, a->GetShootedID());
							for (int i = 0; i < Vehicle_num; i++) {
								auto& tgt = PlayerMngr->GetPlayer(i).GetVehicle();
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								auto res = tgt->CheckAmmoHit(a.get());
								is_HitAll |= res.first;
								if (res.second) { break; }
							}
							if (ColRes && !is_HitAll) {
								a->HitGround(pos_tmp);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.1f * Scale_Rate);
								/*
								if (ObjMngr->GetObj((int)ObjType::Vehicle, a->GetShootedID())) {
									((std::shared_ptr<VehicleClass>&)v)->HitGround(pos_tmp, (pos_tmp - repos_tmp).normalized(), a->GetMove().vec);
								}
								//*/
							}
						}
					}
					else {
						break;
					}
					loop++;
				}
				//コンカッション
				DrawParts->Set_is_Blackout(m_Concussion > 0.f);
				if (m_Concussion == 1.f) {
					DrawParts->SetCamShake(0.5f, 0.05f*Scale_Rate);
					SE->Get((int)SoundEnum::Tank_near).Play_3D(0, DrawParts->GetMainCamera().GetCamPos(), 10.f*Scale_Rate, 128);//, DX_PLAYTYPE_LOOP
				}
				if (m_Concussion > 0.9f) {
					Easing(&m_ConcussionPer, 1.f, 0.1f, EasingType::OutExpo);
				}
				else if (m_Concussion > 0.25f) {
					if (m_ConcussionPer > 0.25f) {
						Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
					}
					else {
						m_ConcussionPer = 0.25f;
					}
				}
				else {
					Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
				}
				DrawParts->Set_Per_Blackout(m_ConcussionPer * 1.5f);

				//Set_is_lens(true);
				//Set_xp_lens(y_r(960));
				//Set_yp_lens(y_r(540));
				//Set_size_lens(y_r(300));
				//Set_zoom_lens(3.f);
			}
			//木の更新
			{
				for (int i = 0; i < Vehicle_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					this->m_BackGround->CheckTreetoSquare(
						v->GetObj_const().frame(v->GetData().Get_square(0)),
						v->GetObj_const().frame(v->GetData().Get_square(2)),
						v->GetObj_const().frame(v->GetData().Get_square(3)),
						v->GetObj_const().frame(v->GetData().Get_square(1)),
						v->GetMove().pos,
						(v->GetMove().pos - v->GetMove().repos).magnitude() * 60.f / DrawParts->GetFps());
				}
			}
			//ハインド
			{
				auto& Hind = *ObjMngr->GetObj((int)ObjType::HindD, 0);
				Hind->SetAnimLoop(0, 1.f);
				Hind->GetObj().work_anime();
			}
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			{
				bool IsADS = (this->m_TPSLen == 0);
				Vector3DX CamPos = IsADS ? Vehicle->GetGunMuzzleBase(0) : Vehicle->Get_EyePos_Base();
				//
				Vector3DX eyepos = CamPos;
				if (!IsADS) {
					Vector3DX eyepos2 = eyepos;
					eyepos += Vehicle->GetRadBuf() * (this->m_range_r*Scale_Rate);
					if (this->m_BackGround->CheckLinetoMap(eyepos, &eyepos2, true, false)) { eyepos = eyepos2; }
				}
				eyepos += DrawParts->GetCamShake() * (IsADS ? 0.05f : 1.f);
				//
				Vector3DX eyetgt = CamPos - Vehicle->GetRadBuf() * std::max(this->m_range_r*Scale_Rate, 1.f) + DrawParts->GetCamShake() * (IsADS ? 0.f : 2.f);
				//
				Vector3DX BaseVec = (eyetgt - eyepos).normalized();
				//
				DrawParts->SetMainCamera().SetCamPos(
					eyepos,
					eyetgt,
					Lerp(Vehicle->GetMove().mat.yvec(), Vector3DX::up(), std::clamp(this->m_range_r / 3.f, 0.f, 1.f)));
				//
				auto fov_t = DrawParts->GetMainCamera().GetCamFov();
				auto near_t = DrawParts->GetMainCamera().GetCamNear();
				auto far_t = DrawParts->GetMainCamera().GetCamFar();
				Easing(&fov_t, (IsADS && (this->m_zoom != 0.f)) ? (this->m_fov_base / this->m_zoom) : this->m_fov_base, 0.9f, EasingType::OutExpo);
				Easing(&near_t, 1.f + 2.f*((IsADS) ? this->m_zoom : 2.f), 0.9f, EasingType::OutExpo);
				Easing(&far_t, std::max(Vehicle->GetAimingDistance(), Scale_Rate * 50.f) + Scale_Rate * 50.f, 0.9f, EasingType::OutExpo);
				DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
				PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t / 2, near_t, far_t);
			}
			this->m_BackGround->Execute();
			{
				Vector3DX StartPos = DrawParts->GetMainCamera().GetCamPos();
				for (int index = 0; index < Chara_num; index++) {
					auto& v = PlayerMngr->GetPlayer(index).GetVehicle();
					if (index == 0) {
						v->CanLookTarget = !(this->m_TPSLen == 0);
						continue;
					}
					Vector3DX TgtPos = v->Get_EyePos_Base();
					v->CanLookTarget = !this->m_BackGround->CheckLinetoMap(StartPos, &TgtPos, true, false);
				}
			}
			//UIパラメーター
			{
				this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
				this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / DrawParts->GetFps();

				this->m_UIclass.SetIntParam(2, 1);

				this->m_UIclass.SetStrParam(0, Vehicle->GetData().GetName());
				this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
				this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
				this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
				this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -500.f, 500.f) / DrawParts->GetFps();

				this->m_UIclass.SetIntParam(6, (int)Vehicle->GetFuel());
				this->m_UIclass.SetIntParam(7, (int)Vehicle->GetFuelMax());
				this->m_UIclass.SetIntParam(8, (int)Vehicle->GetFuel());


				this->m_UIclass.SetIntParam(12, (int)0);//現在選択
				this->m_UIclass.SetIntParam(13, (int)Vehicle->Get_Gunsize());//銃の総数


				this->m_UIclass.SetStrParam(1, Vehicle->GetGun()[0].GetData()->GetName());
				this->m_UIclass.SetIntParam(14, (int)1);//現在選択
				this->m_UIclass.SetIntParam(15, (int)1);//銃の総数
				this->m_UIclass.SetItemGraph(0, &m_aim_Graph);
				if (Vehicle->Get_Gunsize() >= 2) {
					this->m_UIclass.SetStrParam(2, Vehicle->GetGun()[1].GetData()->GetName());
					this->m_UIclass.SetIntParam(16, (int)1);//現在選択
					this->m_UIclass.SetIntParam(17, (int)1);//銃の総数
					this->m_UIclass.SetItemGraph(1, &m_aim_Graph);
				}
			}
			EffectControl::Execute();
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();

			BattleResourceMngr->Dispose();

			this->m_NetWorkBrowser.Dispose();
			this->m_InventoryClass.Dispose();
			EffectControl::Dispose();
			PlayerMngr->Dispose();
			ObjMngr->DeleteAll();
			this->m_BackGround->Dispose();
		}
	};
};