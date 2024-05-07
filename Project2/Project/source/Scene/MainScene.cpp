#include "MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Load_Sub(void) noexcept {
			//BG
			this->m_BackGround->Load();
			//
			this->m_hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			this->m_aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
			this->m_scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");
			//
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Load();
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();

			this->m_BackGround->Init();
			PlayerMngr->Init(Player_num);

			Vector3DX LightVec = Vector3DX::vget(-0.8f, -0.5f, -0.1f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(0.92f, 0.91f, 0.90f, 1.0f));
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 0);
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 1);
			DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 2);

			for (int i = 0; i < Vehicle_num; i++) {
				PlayerMngr->GetPlayer(i).SetVehicle(std::make_shared<VehicleClass>());
				ObjMngr->AddObject(PlayerMngr->GetPlayer(i).GetVehicle());
				PlayerMngr->GetPlayer(i).GetVehicle()->SetMapCol(this->m_BackGround);
				PlayerMngr->GetPlayer(i).GetVehicle()->ObjectBaseClass::Init();

			}
			{
				auto Hind = std::make_shared<HindDClass>();
				ObjMngr->AddObject(Hind);
				ObjMngr->LoadModel(Hind, Hind, "data/model/hindD/");

				Hind->Init();

				Hind->SetMove(Matrix4x4DX::identity(), Vector3DX::vget(0.f, 30.f*Scale_Rate, 0.f));
				Hind->GetObj().get_anime(0).per = 1.f;
			}
			VehDataControl::Instance()->Set();
			//UI
			this->m_UIclass.Set();
			//Set
			//戦車
			{
				Vector3DX BasePos;

				std::vector<int> OtherSelect;
				for (int i = 0; i < Vehicle_num; i++) {
					int ID = 0;
					while (true) {
						ID = GetRand(this->m_BackGround->GetRoadPointNum() - 1);
						bool Hit = (std::find_if(OtherSelect.begin(), OtherSelect.end(), [&](int tmp) { return tmp == ID; }) != OtherSelect.end());

						if (i != 0) {
							auto LEN = (BasePos - this->m_BackGround->GetRoadPoint(ID)->pos()); LEN.y = (0.f);
							if (LEN.magnitude() <= 100.f*Scale_Rate) {
								Hit = true;
							}
						}
						if (!Hit) {
							auto Mat = *this->m_BackGround->GetRoadPoint(ID);
							Vector3DX pos_t = Mat.pos();
							if (
								(-280.f*Scale_Rate / 2.f < pos_t.x && pos_t.x < 290.f*Scale_Rate / 2.f) &&
								(-280.f*Scale_Rate / 2.f < pos_t.z && pos_t.z < 280.f*Scale_Rate / 2.f)
								) {
								OtherSelect.emplace_back(ID);
								break;
							}
						}
					}

					auto Mat = *this->m_BackGround->GetRoadPoint(ID);
					Vector3DX pos_t = Mat.pos();
					float rad_t = std::atan2f(Mat.zvec().x, -Mat.zvec().z);
					auto pos_t1 = pos_t + Vector3DX::up() * 1250.f;
					auto pos_t2 = pos_t + Vector3DX::up() * -1250.f;
					if (this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, false)) {
						pos_t = pos_t2;
					}
					if (i == 0) {
						BasePos = pos_t;
					}

					auto& vehc_datas = VehDataControl::Instance()->GetVehData();
					auto* vehc_data = &vehc_datas[
						GetRand((int)vehc_datas.size() - 1)
							//i != 0 ? GetRand((int)vehc_datas.size() - 1) : 0
					];

					ObjMngr->LoadModel(PlayerMngr->GetPlayer(i).GetVehicle(), nullptr, ("data/tank/" + vehc_data->GetName() + "/").c_str());

					PlayerMngr->GetPlayer(i).GetVehicle()->ValueInit(vehc_data, this->m_BackGround->GetBox2Dworld(), (PlayerID)i);
					PlayerMngr->GetPlayer(i).GetVehicle()->ValueSet(deg2rad(0), rad_t, pos_t);
				}
			}
			//player
			for (int i = 0; i < Player_num; i++) {
				auto& Vehicle = PlayerMngr->GetPlayer(i).GetVehicle();
				for (int loop = 0; loop < 5; loop++) {
					PlayerMngr->GetPlayer(i).SetInventory(loop, Vehicle->GetInventoryXSize(loop), Vehicle->GetInventoryYSize(loop));
				}
				{
					if (Vehicle->Get_Gunsize() >= 2) {
						PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
						PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[1].GetAmmoSpec(0), PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
					}
					else {
						PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
					}
					PlayerMngr->GetPlayer(i).FillInventory(1, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(1), 6);
				}
				{
					PlayerMngr->GetPlayer(i).FillInventory(2, Vehicle->GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(2), PlayerMngr->GetPlayer(i).GetInventoryYSize(2));
					PlayerMngr->GetPlayer(i).FillInventory(3, Vehicle->GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(3), PlayerMngr->GetPlayer(i).GetInventoryYSize(3));
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
				this->m_AICtrl[i]->Init(this->m_BackGround, (PlayerID)i);
			}
			this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetVehicle()->GetHP();
			this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//サウンド
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Set();
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
				//Vehicle->Get_alive()
				//ネットワーク
				SendInfo ans;
				if (PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					ans.m_Pos = Vehicle->GetMove().pos;
					ans.m_Vec = Vehicle->GetMove().vec;
					ans.m_rad.y = (Vehicle->Get_body_yrad());
					ans.m_Damage = &Vehicle->GetDamageEvent();
					ans.m_DamageSwitch = (Vehicle->GetDamageSwitch() ? 1 : 0);
				}

				this->m_NetWorkBrowser.FirstExecute(MyInput, ans);
				//クライアント
				if (this->m_NetWorkBrowser.GetIsClient()) {
					for (int i = 0; i < Vehicle_num; i++) {
						auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
						if (v->GetMyPlayerID() == GetMyPlayerID()) {
							v->SetCharaType(CharaTypeID::Mine);
						}
						else {
							v->SetCharaType(CharaTypeID::Enemy);
						}
					}
				}
				//
				bool isready = true;
				for (int i = 0; i < Player_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					if (this->m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = m_NetWorkBrowser.GetNowServerPlayerData(i, true);
						if (i == GetMyPlayerID()) {
							MyInput.SetKeyInputFlags(tmp.Input);//キーフレームだけサーバーに合わせる
							v->SetInput(MyInput, isready, false);
							this->m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
						}
						else {
							if (!m_NetWorkBrowser.GetIsClient()) {
								m_AICtrl[i]->AI_move(&tmp.Input);
							}
							v->SetInput(tmp.Input, isready, true);
							bool override_true = true;
							for (int i2 = 0; i2 < Player_num; i2++) {
								auto& v2 = PlayerMngr->GetPlayer(i2).GetVehicle();
								if ((v != v2) && ((v->GetMove().pos - v2->GetMove().pos).magnitude() <= 10.f*Scale_Rate)) {
									override_true = false;
									break;
								}
							}
							if (override_true) {
								v->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
							}

						}
						//ダメージイベント処理
						if (v.get() != nullptr) {
							if (tmp.DamageSwitch != v->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(tmp.Damage);
								v->SetDamageSwitchRec(tmp.DamageSwitch);
							}
						}
					}
					else {
						if (i == GetMyPlayerID()) {
							v->SetInput(MyInput, isready, false);
						}
						else {
							InputControl OtherInput;
							m_AICtrl[i]->AI_move(&OtherInput);//めっちゃ重い
							v->SetInput(OtherInput, isready, false);
						}
						//ダメージイベント処理
						if (v.get() != nullptr) {
							if (v->GetDamageSwitch() != v->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(v->GetDamageEvent());
								v->SetDamageSwitchRec(v->GetDamageSwitch());
							}
						}
					}
				}
				this->m_NetWorkBrowser.LateExecute();
				this->m_InventoryClass.LateExecute();
				//ダメージイベント
				for (int i = 0; i < Player_num; i++) {
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
				for (int i = 0; i < Player_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					if (v->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) {
						v->GetColNearestInAllMesh(StartPos, &EndPos);
					}
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
							auto& v = *ObjMngr->GetObj((int)ObjType::Vehicle, a->GetShootedID());
							for (int i = 0; i < Player_num; i++) {
								auto& tgt = PlayerMngr->GetPlayer(i).GetVehicle();
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								auto res = tgt->CheckAmmoHit(a.get(), v->GetMove().pos);
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
					Vehicle->ShakePer(0.05f);
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
				for (int i = 0; i < Player_num; i++) {
					auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
					this->m_BackGround->CheckTreetoSquare(v->GetSquarePos(0), v->GetSquarePos(2), v->GetSquarePos(3), v->GetSquarePos(1), v->GetMove().pos,
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
				auto& VehicleView = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				VehicleView->Setcamera(DrawParts->SetMainCamera(), this->m_fov_base);
				Vector3DX BaseVec = (DrawParts->GetMainCamera().GetCamVec() - DrawParts->GetMainCamera().GetCamPos()).normalized();
				Vector3DX BaseUp = DrawParts->GetMainCamera().GetCamUp();
				Matrix4x4DX FreeLook = Matrix4x4DX::RotAxis(Vector3DX::Cross(BaseVec, BaseUp), Pad->GetLS_X()) * Matrix4x4DX::RotAxis(BaseUp, Pad->GetLS_Y());
				Easing_Matrix(&this->m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);
				Vector3DX CamVec = Lerp(BaseVec, Matrix4x4DX::Vtrans(BaseVec, this->m_FreeLookMat), 0.f);
				Vector3DX CamPos = VehicleView->Get_EyePos_Base();

				DrawParts->SetMainCamera().SetCamPos(
					Lerp(DrawParts->GetMainCamera().GetCamPos(), (CamPos + CamVec * -100.f), 0.f),
					CamPos + CamVec * 100.f,
					BaseUp);

				auto near_t = DrawParts->GetMainCamera().GetCamNear();
				auto far_t = DrawParts->GetMainCamera().GetCamFar();
				PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t / 2, near_t, far_t);
			}
			this->m_BackGround->Execute();

			//UIパラメーター
			{
				this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
				this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / DrawParts->GetFps();

				this->m_UIclass.SetIntParam(2, 1);

				this->m_UIclass.SetStrParam(0, Vehicle->GetName());
				this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
				this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
				this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
				this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -500.f, 500.f) / DrawParts->GetFps();

				this->m_UIclass.SetIntParam(6, (int)Vehicle->GetFuel());
				this->m_UIclass.SetIntParam(7, (int)Vehicle->GetFuelMax());
				this->m_UIclass.SetIntParam(8, (int)Vehicle->GetFuel());


				this->m_UIclass.SetIntParam(12, (int)0);//現在選択
				this->m_UIclass.SetIntParam(13, (int)Vehicle->Get_Gunsize());//銃の総数


				this->m_UIclass.SetStrParam(1, Vehicle->GetGun()[0].GetGunSpec()->GetName());
				this->m_UIclass.SetIntParam(14, (int)1);//現在選択
				this->m_UIclass.SetIntParam(15, (int)1);//銃の総数
				this->m_UIclass.SetItemGraph(0, &m_aim_Graph);
				if (Vehicle->Get_Gunsize() >= 2) {
					this->m_UIclass.SetStrParam(2, Vehicle->GetGun()[1].GetGunSpec()->GetName());
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