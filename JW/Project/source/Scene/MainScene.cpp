#pragma once
#include	"MainScene.hpp"
#include "../sub.hpp"
#include "../ObjectManager.hpp"
#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "../MainScene/Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Load_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
			SE->Add((int)SoundEnum::Env2, 1, "data/Sound/SE/envi2.wav", false);
			SE->Add((int)SoundEnum::LaserSwitch, 1, "data/Sound/SE/aim_on.wav", false);

			SE->Add((int)SoundEnum::CartFall, 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add((int)SoundEnum::MagFall, 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::Cocking2_0 + i, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
			}
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::Cocking3_0 + i, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
			}
			SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
			SE->Add((int)SoundEnum::RunFoot, 3, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::SlideFoot, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Heart, 3, "data/Sound/SE/move/heart.wav");
			//
			this->Gauge_Graph = GraphHandle::Load("data/UI/Gauge.png");
			this->hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			this->aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
			this->m_MiniMapScreen = GraphHandle::Make(y_r(128) * 2, y_r(128) * 2, true);
			//BG
			this->m_BackGround = std::make_shared<BackGroundClassMain>();
			this->m_BackGround->Init("", "");
			//
			GunAnimManager::Instance()->Load("data/CharaAnime/");
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			SetAmbientLight(VECTOR_ref::vget(-0.8f, -0.5f, -0.1f), GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			SetFarShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetMiddleShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetNearShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			//
			for (int i = 0; i < 1; i++) {
				auto* Ptr = ObjMngr->MakeObject(ObjType::Human);
				ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), "data/Charactor/Suit/");
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*Ptr));
				character_Pool.back()->SetMapCol(this->m_BackGround);
				(*Ptr)->Init();
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}
			for (int i = 1; i < Chara_num; i++) {
				auto* Ptr = ObjMngr->MakeObject(ObjType::Human);
				ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), "data/Charactor/Soldier/");
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*Ptr));
				character_Pool.back()->SetMapCol(this->m_BackGround);
				(*Ptr)->Init();
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}

			for (int i = 0; i < 1; i++) {
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), "data/gun/G17Gen3/");
				auto& Gun = ((std::shared_ptr<GunClass>&)(*Ptr));
				Gun->SetMapCol(this->m_BackGround);
				(*Ptr)->Init();
			}
			for (int i = 1; i < gun_num; i++) {
				auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
				ObjMngr->LoadModel(PHYSICS_SETUP::DISABLE, false, (*Ptr).get(), "data/gun/AR15_90/");
				auto& Gun = ((std::shared_ptr<GunClass>&)(*Ptr));
				Gun->SetMapCol(this->m_BackGround);
				(*Ptr)->Init();
			}
			//ロード
			//人
			for (auto& c : this->character_Pool) {
				size_t index = &c - &this->character_Pool.front();

				VECTOR_ref pos_t;
				float rad_t = 0.f;
				if (index == 0) {
					pos_t = VECTOR_ref::vget(0.f, 0.f, 0.f);
					rad_t = deg2rad(GetRandf(180.f));
				}
				else {
					pos_t = VECTOR_ref::vget(
						GetRandf(30 * 19 / 2),
						0.f,
						GetRandf(30 * 19 / 2));
					rad_t = deg2rad(GetRandf(180.f));
				}



				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
				if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
				c->ValueSet(deg2rad(0.f), rad_t, pos_t, (PlayerID)index);
				c->SetGunPtr((std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)index)));
				c->GunSetUp();
				if (index < 1) {
					c->SetCharaType(CharaTypeID::Team);
				}
				else {
					c->SetCharaType(CharaTypeID::Enemy);
				}
			}
			//player
			PlayerMngr->Init(Player_num);
			for (int i = 0; i < Player_num; i++) {
				PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i)));
				//PlayerMngr->GetPlayer(i).SetChara(nullptr);

				this->m_AICtrl[i]->Init(this->m_BackGround, (PlayerID)i);
			}
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			this->m_HPBuf = Chara->GetHP();
			this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();

			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			SE->Get((int)SoundEnum::CartFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::MagFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::Trigger).SetVol_Local(48);
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(216);
			SE->Get((int)SoundEnum::Shot3).SetVol_Local(216);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
			//UI
			this->m_UIclass.Set();
			//入力
			this->m_DamageEvents.clear();
			m_NetWorkBrowser.Init();
			Timer = 3.f;
			//SetLightEnable(FALSE);
			for (auto& y : SelYadd) {
				y = 0.f;
			}

			m_Retire = false;
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			Pad->ChangeGuide(
				[&]() {
				auto* KeyGuide = KeyGuideClass::Instance();
				//KeyGuide->Reset();
				//return;
				if (DXDraw::Instance()->IsPause()) {
					KeyGuide->Reset();
					KeyGuide->AddGuide("ng.png", "決定");
					KeyGuide->AddGuide("ok.png", "戻る");
					KeyGuide->AddGuide("R_stick.png", "上下選択");
				}
				else {
					KeyGuide->Reset();
					KeyGuide->AddGuide("R_stick.png", "移動");
					KeyGuide->AddGuide("L1.png", "");
					KeyGuide->AddGuide("R1.png", "左右傾け(リーン)");
					KeyGuide->AddGuide("R_stick.png", "押し込みで走り");
					KeyGuide->AddGuide("ng.png", "しゃがみ");
					KeyGuide->AddGuide("square.png", "リロード");
					KeyGuide->AddGuide("R2.png", "射撃");
					KeyGuide->AddGuide("triangle.png", "レーザー点灯/消灯");
					KeyGuide->AddGuide("L2.png", "エイム");
					KeyGuide->AddGuide("option.png", "ポーズ");
				}
			},
				[&]() {
				auto* KeyGuide = KeyGuideClass::Instance();
				//KeyGuide->Reset();
				//return;
				if (DXDraw::Instance()->IsPause()) {
					KeyGuide->Reset();
					KeyGuide->AddGuide("none.jpg", "決定");
					KeyGuide->AddGuide("X.jpg", "戻る");
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "上下選択");
				}
				else {
					KeyGuide->Reset();
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "");
					KeyGuide->AddGuide("A.jpg", "");
					KeyGuide->AddGuide("D.jpg", "移動");
					KeyGuide->AddGuide("Q.jpg", "");
					KeyGuide->AddGuide("E.jpg", "左右傾け(リーン)");
					KeyGuide->AddGuide("Shift.jpg", "走る");
					KeyGuide->AddGuide("R.jpg", "リロード");
					KeyGuide->AddGuide("C.jpg", "");
					KeyGuide->AddGuide("X.jpg", "しゃがみ");
					KeyGuide->AddGuide("LM.jpg", "射撃");
					KeyGuide->AddGuide("F.jpg", "レーザー点灯/消灯");
					KeyGuide->AddGuide("RM.jpg", "エイム");
					KeyGuide->AddGuide("Tab.jpg", "ポーズ");
				}
			});

			if (DXDraw::Instance()->IsPause()) {
				auto* SE = SoundPool::Instance();
				if (!OptionWindowClass::Instance()->IsActive()) {
					if (Pad->GetUpKey().trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetDownKey().trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					for (int i = 0; i < 3; i++) {
						Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
					}
					if (Pad->GetOKKey().trigger()) {
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						switch (select) {
						case 0:
							m_Retire = true;
							DXDraw::Instance()->PauseExit();
							break;
						case 1:
							OptionWindowClass::Instance()->SetActive();
							break;
						case 2:
							DXDraw::Instance()->PauseExit();
							break;
						default:
							DXDraw::Instance()->PauseExit();
							break;
						}
					}
					if (Pad->GetNGKey().trigger()) {
						SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
						DXDraw::Instance()->PauseExit();
					}
				}
				return true;
			}
			else {
				select = 0;
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
				Timer = 3.f;
				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP);
				SE->Get((int)SoundEnum::Env2).Play(0, DX_PLAYTYPE_LOOP);
			}
			else {
				Timer = std::max(Timer - 1.f / FPS, 0.f);
			}
			{
				/*
				m_GameEnd |= (!(Chara->GetHP() != 0 && (Timer <= TotalTime)));
				if (m_GameEnd) {
					EndTimer = std::max(EndTimer - 1.f / FPS, 0.f);
				}
				//*/
			}
			//Input,AI
			{
				float cam_per = ((DrawParts->GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
				cam_per *= 0.6f;
				float pp_x = 0.f, pp_y = 0.f;
				InputControl MyInput;
				//
				pp_x = Pad->GetLS_Y() * cam_per*0.25f;
				pp_y = Pad->GetLS_X() * cam_per*0.25f;
				if (Pad->GetFreeLook().press()) {
					pp_x /= 2.f;
					pp_y /= 2.f;
				}

				MyInput.SetInput(
					pp_x - Chara->GetRecoilRadAdd().y(),
					pp_y - Chara->GetRecoilRadAdd().x(),
					Pad->GetUpKey().press(), Pad->GetDownKey().press(), Pad->GetLeftKey().press(), Pad->GetRightKey().press(),
					Pad->GetRunKey().press(),
					Pad->GetQKey().press(), Pad->GetEKey().press(),
					false, false, false, false,
					Pad->GetOKKey().press(),
					Pad->GetAccelKey().press(),
					Pad->GetBrakeKey().press(),
					Pad->GetNGKey().press(),
					(Pad->GetShotKey().press() && !DXDraw::Instance()->IsPause()),
					(Pad->GetLookKey().press() && !DXDraw::Instance()->IsPause())
				);
				//
				MyInput.SetRadBuf(PlayerMngr->GetPlayer(GetMyPlayerID()).GetRadBuf());
				//ネットワーク
				m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
				//クライアント
				if (m_NetWorkBrowser.GetClient()) {
					for (auto& c : this->character_Pool) {
						if (c->GetMyPlayerID() == GetMyPlayerID()) {
							c->SetCharaType(CharaTypeID::Team);
						}
						else {
							if (c->GetMyPlayerID() < 1) {
								c->SetCharaType(CharaTypeID::Team);
							}
							else {
								c->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
				}
				//
				bool isready = (Timer == 0.f);
				for (int i = 0; i < Player_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
					if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = this->m_NetWorkBrowser.GetNowServerPlayerData(i, false);
						if (i == GetMyPlayerID()) {
							MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
							c->SetInput(MyInput, isready);
							m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
						}
						else {
							if (!m_NetWorkBrowser.GetClient()) {
								m_AICtrl[i]->Execute(&tmp.Input);
							}
							c->SetInput(tmp.Input, isready);
							bool override_true = true;
							override_true = (tmp.CalcCheckSum() != 0);
							if (override_true) {
								c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
							}

						}
						//ダメージイベント処理
						if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
							if (tmp.DamageSwitch != c->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(tmp.Damage);
								c->SetDamageSwitchRec(tmp.DamageSwitch);
							}
						}
					}
					else {
						if (i == GetMyPlayerID()) {
							c->SetInput(MyInput, isready);
						}
						else {
							InputControl OtherInput;
							m_AICtrl[i]->Execute(&OtherInput);//めっちゃ重い
							c->SetInput(OtherInput, isready);
						}
						//ダメージイベント処理
						if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
							if (c->GetDamageSwitch() != c->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(c->GetDamageEvent());
								c->SetDamageSwitchRec(c->GetDamageSwitch());
							}
						}
					}
				}
				m_NetWorkBrowser.LateExecute();
				//ダメージイベント
				for (auto& c : this->character_Pool) {
					for (int j = 0; j < this->m_DamageEvents.size(); j++) {
						if (c->SetDamageEvent(this->m_DamageEvents[j])) {
							std::swap(this->m_DamageEvents.back(), m_DamageEvents[j]);
							this->m_DamageEvents.pop_back();
							j--;
						}
					}
				}
			}
			//Execute
			ObjMngr->ExecuteObject();
			//弾の更新
			{
				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
						if (a->IsActive()) {
							//AmmoClass
							VECTOR_ref pos_tmp = a->GetMove().pos;
							VECTOR_ref norm_tmp;
							auto ColResGround = this->m_BackGround->CheckLinetoMap(a->GetMove().repos, &pos_tmp, true, &norm_tmp);
							bool is_HitAll = false;
							for (auto& tgt : this->character_Pool) {
								if (tgt->GetHP() == 0) { continue; }
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								is_HitAll |= tgt->CheckAmmoHit(a.get(), a->GetMove().repos, &pos_tmp, tgt->GetMove().pos);
							}
							if (this->m_BackGround->HitLightCheck(a->GetMove().repos, &pos_tmp)) {
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
							}
							if (ColResGround && !is_HitAll) {
								a->HitGround(pos_tmp);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
							}
						}
					}
					else {
						break;
					}
					loop++;
				}
			}
			//背景更新
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//首の向き
			{
				MATRIX_ref FreeLook = MATRIX_ref::RotX(std::clamp(0.f, deg2rad(-20), deg2rad(20))) * MATRIX_ref::RotY(0.f);
				Chara->SetCamEyeVec(FreeLook.zvec()*-1.f);
			}
			//視点
			{
				float ShakeTime = 0.1f;
				if (Chara->GetSendCamShake()) {
					this->m_CamShake = ShakeTime;
				}
				Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake / ShakeTime), GetRandf(this->m_CamShake / ShakeTime), GetRandf(this->m_CamShake / ShakeTime)), 0.8f, EasingType::OutExpo);
				Easing(&this->m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
				this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

				{
					//FPSカメラ
					VECTOR_ref CamPos = Chara->GetEyePosition() + this->m_CamShake2;
					DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetEyeVecY());
					//info
					auto fov_t = DrawParts->GetMainCamera().GetCamFov();
					auto near_t = DrawParts->GetMainCamera().GetCamNear();
					auto far_t = DrawParts->GetMainCamera().GetCamFar();
					if (Chara->GetIsADS()) {
						Easing(&near_t, Scale_Rate * 0.01f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 60.f, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&near_t, Scale_Rate * 0.01f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
					}
					//fov
					{
						float fov = deg2rad(OptionParts->Get_Fov());
						if (Chara->GetIsADS()) {
							fov -= deg2rad(15);
						}
						else if (Chara->GetRun()) {
							fov += deg2rad(5);
						}
						if (Chara->GetGunPtrNow()->GetShotSwitch()) {
							fov -= deg2rad(8);
							Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
						}
						else {
							Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
						}
					}
					DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
					//DoF
					if (Chara->GetIsADS()) {
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t - 0.1f, Scale_Rate * 0.3f, far_t);
					}
					else {
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t / 2, near_t, far_t);
					}
				}
			}
			{
				auto Len = (DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Length();

				Easing(&Min, Lerp(0.f, 25.f, std::clamp(Len / (5.f*Scale_Rate), 0.f, 1.f)), 0.95f, EasingType::OutExpo);
				Easing(&Gamma, Lerp(1.f, 1.15f, std::clamp(Len / (5.f*Scale_Rate), 0.f, 1.f)), 0.95f, EasingType::OutExpo);

				PostPassEffect::Instance()->SetLevelFilter((int)Min, 192, Gamma);
			}
			//
			this->m_BackGround->Execute();
			//射撃光
			if (Chara->GetGunPtrNow()->GetShotSwitch()) {
				auto mat = Chara->GetGunPtrNow()->GetMuzzleMatrix();
				SetLightEnable(TRUE);
				ChangeLightTypePoint(mat.pos().get(),
					2.0f*Scale_Rate,
					0.1f,
					0.6f,
					0.0f);
			}
			{
				SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Norm(), 0);
				SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(1)).Norm(), 1);
				SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(2)).Norm(), 2);
				SetisUpdateFarShadow(true);
			}
			//レーザーサイト
			for (auto& c : this->character_Pool) {
				//
				auto LaserStartPos = c->GetLaserStartPos();
				auto LaserEndPos = LaserStartPos + c->GetGunPtrNow()->GetMuzzleMatrix().zvec()*-1.f * 15.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(LaserStartPos, &LaserEndPos, true);
				for (const auto& c2 : this->character_Pool) {
					if (c2->GetMyPlayerID() == c->GetMyPlayerID()) { continue; }
					c2->CheckLineHit(LaserStartPos, &LaserEndPos);
				}
				auto Vec = (LaserEndPos - LaserStartPos);
				LaserEndPos = LaserStartPos + Vec.Norm()*std::max(Vec.Length() - 0.3f*Scale_Rate, 0.f);
				c->SetLaserPos(LaserEndPos);
				//
				VECTOR_ref CamPos = c->GetEyePosition();
				c->SetLaser2D(GetScreenPos(CamPos, CamPos + c->GetEyeVector(), c->GetEyeVecY(), DrawParts->GetMainCamera().GetCamFov(), LaserEndPos));
			}
#ifdef DEBUG
			DebugParts->SetPoint("---");
#endif // DEBUG
			//UIパラメーター
			{
				//Time,Score
				this->m_UIclass.SetfloatParam(0, Timer);
				this->m_UIclass.SetIntParam(6, PlayerMngr->GetPlayer(0).GetScore());
				//HP
				this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
				this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
				this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
				this->m_HPBuf += (int)(std::clamp((float)(Chara->GetHP() - this->m_HPBuf)*20.f, -15.f, 15.f) / FPS);
				//SPeed,ALT

				this->m_UIclass.SetIntParam(0, (int)(this->m_CamShake2.x()*100.f));
				this->m_UIclass.SetIntParam(1, (int)(this->m_CamShake2.y()*100.f));
				this->m_UIclass.SetIntParam(2, (int)(0.f*100.f));

				this->m_UIclass.SetItemGraph(0, &Gauge_Graph);
				this->m_UIclass.SetItemGraph(1, &Gauge_Graph);
			}
			EffectControl::Execute();
			//
			for (auto& c : this->character_Pool) {
				VECTOR_ref campos; campos.z(-1.f);
				c->SetCameraPosition(campos);
			}
			//m_AimPoint.z(-1.f);
			//ミニマップ
			m_MiniMapScreen.SetDraw_Screen();
			{
				float size = 0.7f;
				int xp = y_r(128);
				int yp = y_r(128);

				int xp1 = 0;
				int yp1 = 0;

				xp1 -= (int)(15.f + Chara->GetMove().pos.x());
				yp1 -= -(int)(15.f + Chara->GetMove().pos.z());

				float rad = std::atan2f(
					Chara->GetEyeVector().cross(VECTOR_ref::front()).y(),
					Chara->GetEyeVector().dot(VECTOR_ref::front())
				);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 216);
				this->m_BackGround->GetMapGraph().DrawRotaGraph3(
					xp + (int)(((float)xp1*std::cos(rad) - (float)yp1 * std::sin(rad))*size),
					yp + (int)(((float)yp1*std::cos(rad) + (float)xp1 * std::sin(rad))*size),
					this->m_BackGround->GetMapGraphXSize() / 2,
					this->m_BackGround->GetMapGraphYSize() / 2,
					size, -size,
					rad, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

				for (auto& c : this->character_Pool) {
					int xp2 = xp1 + (int)(15.f + c->GetMove().pos.x());
					int yp2 = yp1 - (int)(15.f + c->GetMove().pos.z());
					DrawCircle(
						xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, GetColor(255, 0, 0), TRUE);
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			if (m_Retire) {
				return false;
			}
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			SE->Get((int)SoundEnum::Env).StopAll(0);
			SE->Get((int)SoundEnum::Env2).StopAll(0);

			SE->Delete((int)SoundEnum::LaserSwitch);
			SE->Delete((int)SoundEnum::CartFall);
			SE->Delete((int)SoundEnum::MagFall);
			SE->Delete((int)SoundEnum::Env);
			SE->Delete((int)SoundEnum::Env2);
			SE->Delete((int)SoundEnum::StandUp);
			SE->Delete((int)SoundEnum::Trigger);
			for (int i = 0; i < 5; i++) {
				SE->Delete((int)SoundEnum::Cocking2_0 + i);
			}
			for (int i = 0; i < 5; i++) {
				SE->Delete((int)SoundEnum::Cocking3_0 + i);
			}
			SE->Delete((int)SoundEnum::RunFoot);
			SE->Delete((int)SoundEnum::SlideFoot);
			SE->Delete((int)SoundEnum::StandupFoot);
			SE->Delete((int)SoundEnum::Heart);
			//
			this->Gauge_Graph.Dispose();
			this->hit_Graph.Dispose();
			this->aim_Graph.Dispose();
			this->m_MiniMapScreen.Dispose();
			//
			for (auto& c : character_Pool) {
				c.reset();
			}
			character_Pool.clear();

			m_AICtrl.clear();
			m_NetWorkBrowser.Dispose();
			EffectControl::Dispose();
			PlayerMngr->Dispose();
			ObjMngr->DisposeObject();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
		}

		void			MAINLOOP::BG_Draw_Sub(void) noexcept {
			this->m_BackGround->BG_Draw();
		}
		void			MAINLOOP::ShadowDraw_Far_Sub(void) noexcept {
			this->m_BackGround->Shadow_Draw_Far();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::ShadowDraw_NearFar_Sub(void) noexcept {
			this->m_BackGround->Shadow_Draw_NearFar();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept {
			//this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);

			ObjMngr->DrawObject();
			this->m_BackGround->Draw();
			//レーザー
			for (auto& c : this->character_Pool) {
				if (c->GetLaserActive()) {
					c->DrawLaser();
				}
			}

			//ObjMngr->DrawDepthObject();
			//シェーダー描画用パラメーターセット
			{
				//
				Set_is_Blackout(true);
				Set_Per_Blackout(0.5f + (1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
				//
				Set_is_lens(false);
			}
			for (auto& c : this->character_Pool) {
				if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
				//auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
				auto pos = c->GetMove().pos + c->GetMove().mat.zvec()*-1.f * 5.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					c->SetCameraPosition(campos);
					c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
				}
			}
			if (Chara->GetHP() != 0) {
				auto pos = Chara->GetMatrix().pos() + Chara->GetMatrix().zvec()*-1.f*100.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					//m_AimPoint = campos;
				}
			}
			if (false)
			{
				for (int i = 0; i < Chara_num; i++) {
					m_AICtrl[i]->Draw();
				}
			}
			//
		}
		void			MAINLOOP::MainDrawbyDepth_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawDepthObject();
		}
		//UI表示
		void			MAINLOOP::DrawUI_Base_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			//auto* Fonts = FontPool::Instance();
			//auto* DrawParts = DXDraw::Instance();
			//auto Red = GetColor(255, 0, 0);
			//auto Blue = GetColor(50, 50, 255);
			//auto Green = GetColor(43, 163, 91);
			//auto White = GetColor(212, 255, 239);
			//auto Gray = GetColor(64, 64, 64);
			//auto Black = GetColor(0, 0, 0);
			//unsigned int color = Red;
			//
			{
				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
						a->Draw_Hit_UI(hit_Graph);
					}
					else {
						break;
					}
					loop++;
				}
			}
			{
				auto per = std::clamp(std::abs((float)(Chara->GetHP() - this->m_HPBuf))*0.1f, 0.f, 0.5f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				//OIL_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//UI
			this->m_UIclass.Draw();
			//通信設定
			if (DXDraw::Instance()->IsPause()) {
				//m_NetWorkBrowser.Draw();
			}
			//
		}
		void			MAINLOOP::DrawUI_In_Sub(void) noexcept {
			//ポーズ
			if (DXDraw::Instance()->IsPause()) {
				int xp1, yp1;
				//auto* Fonts = FontPool::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Red75 = GetColor(192, 0, 0);
				//auto White = GetColor(255, 255, 255);
				//auto Gray75 = GetColor(128, 128, 128);
				//auto Gray = GetColor(64, 64, 64);

				xp1 = y_r(1920 - 256 - 54 * 2);
				yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);
				DrawFetteString(xp1, yp1, 1.f, (select == 0), "Retire");

				xp1 = y_r(1920 - 256 - 54 * 1);
				yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);
				DrawFetteString(xp1, yp1, 1.f, (select == 1), "Option");

				xp1 = y_r(1920 - 256);
				yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
				DrawFetteString(xp1, yp1, 1.f, (select == 2), "Return Game");
			}
			else {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//
				m_MiniMapScreen.DrawRotaGraph(y_r(960), y_r(840), 1.f, 0.f, true);
				//
				if (Chara->GetLaserActive() && !Chara->GetIsADS()) {
					VECTOR_ref Laserpos2D = Chara->GetAimPoint();
					if (0.f < Laserpos2D.z() && Laserpos2D.z() < 1.f) {
						aim_Graph.DrawRotaGraph((int)Laserpos2D.x(), (int)Laserpos2D.y(), (float)(y_r(100)) / 100.f, 0.f, true);
					}
				}
			}
		}
	};
};
