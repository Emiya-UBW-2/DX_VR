#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

#include "../../MainScene/Object/Ammo.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		//
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				auto* SE = SoundPool::Instance();
				//
				HitType type = HitType::Body;
				if (value.DamageID == this->m_ViewID) {//自分にヒット
					SE->Get(static_cast<int>(SoundEnum::Damage)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					m_HP = std::clamp<HitPoint>(m_HP - value.Damage, 0, GetHPMax());
					SE->Get(static_cast<int>(SoundEnum::Hit)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				if (value.ShotID == this->m_ViewID) {//自分にヒット
					if (m_HP == 0) {
						type = HitType::Head;
					}
					HitMark::Instance()->Add(value.m_Pos, type,value.Damage, static_cast<float>(value.Damage) / 100.f);
				}
				//ダメージ
				return true;
			}
			return false;
		}
		//
		bool		CharacterClass::CheckDamageRay(HitPoint* Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!(GetMinLenSegmentToSegment(StartPos, *pEndPos, 
				GetMove().GetPos() + Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 10.0f * Scale_Rate, -100.0f * Scale_Rate), GetBaseRotMatrix()),
				GetMove().GetPos() + Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 10.0f * Scale_Rate, 100.0f * Scale_Rate), GetBaseRotMatrix())
			) <= 10.0f * Scale_Rate)) { return false; }
			//被弾処理
			if (true) {
				SEGMENT_SEGMENT_RESULT Ret;
				GetSegmenttoSegment(StartPos, *pEndPos,
					GetMove().GetPos() + Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 10.0f * Scale_Rate, -100.0f * Scale_Rate), GetBaseRotMatrix()),
					GetMove().GetPos() + Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 10.0f * Scale_Rate, 100.0f * Scale_Rate), GetBaseRotMatrix()),
					&Ret);
				*pEndPos = Ret.SegB_MinDist_Pos;
				//ダメージ登録
				{
					m_Damage.Add(AttackID, this->m_MyID, *Damage, HitType::Head, *pEndPos);
				}
				return true;
			}
			return false;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept {
			CharaMove::InitKey(pxRad, pyRad);
			Matrix3x3DX Mat; Mat.SetRadian(CharaMove::GetRadBuf().x, CharaMove::GetRadBuf().y, 0.f);
			ResetMove(Mat, pPos);
			this->m_CharaSound = -1;
		}
		void			CharacterClass::Init_Sub(void) noexcept {
			m_BambooVec.Set(0.f, 0.f);
			m_BambooVecBase.Set(0.f, 0.f);
			this->m_MoveOverRideFlag = false;
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
			m_TurretData.clear();
			int Max = GetObj().GetFrameNum();
			for (int frameNum = 0; frameNum < Max; frameNum++) {
				if ((GetObj().GetFrameName(frameNum).find("旋回") != std::string::npos) && (GetObj().GetFrameChildNum(frameNum) > 0)) {
					m_TurretData.resize(m_TurretData.size() + 1);
					m_TurretData.back().Rotate.Set(frameNum, GetObj());
					m_TurretData.back().Elevate.Set(GetObj().GetFrameChild(frameNum, 0), GetObj());
					m_TurretData.back().NowRad = 0.f;
					m_TurretData.back().NowRadR = 0.f;
					m_TurretData.back().NowLoadTime = 0.f;
				}
			}
			int mdata = FileRead_open((this->GetFilePath() + "data.txt").c_str(), FALSE);
			{
				auto ALL = getparams::Getstr(mdata);
				auto RIGHT = getparams::getright(ALL);
				m_WaveFront = std::stof(RIGHT);
			}
			size_t index = 0;
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				if (ALL.find('//') != std::string::npos) {
					auto div = ALL.find('//');
					ALL = ALL.substr(0, div);
				}
				if (ALL.find('=') != std::string::npos) {
					auto RIGHT = getparams::getright(ALL);
					if (RIGHT.find(',') != std::string::npos) {
						{
							auto div = RIGHT.find(',');
							m_TurretData.at(index).MinusLimit = deg2rad(std::stof(RIGHT.substr(0, div)));
							RIGHT = RIGHT.substr(div + 1);
						}
						{
							auto div = RIGHT.find(',');
							m_TurretData.at(index).Rad = deg2rad(std::stof(RIGHT.substr(0, div)));
							RIGHT = RIGHT.substr(div + 1);
						}
						{
							auto div = RIGHT.find(',');
							m_TurretData.at(index).PlusLimit = deg2rad(std::stof(RIGHT.substr(0, div)));
							RIGHT = RIGHT.substr(div + 1);
						}
						{
							auto div = RIGHT.find(',');
							m_TurretData.at(index).RotateRate = std::stof(RIGHT.substr(0, div));
							RIGHT = RIGHT.substr(div + 1);
						}
						{
							auto div = RIGHT.find(',');
							m_TurretData.at(index).AmmoSize = std::stof(RIGHT.substr(0, div));
							RIGHT = RIGHT.substr(div + 1);
						}
						{
							m_TurretData.at(index).LoadTime = std::stof(RIGHT);
						}
						index++;
					}
				}
			}
			FileRead_close(mdata);
			if (index < m_TurretData.size()) {
				m_TurretData.resize(index);
			}

			MV1::Load("data/model/Wave/model.mv1", &m_WaveObj);
		}
		//
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputControl Input = pInput;
			//ガード入力を反映
			m_GuardVec.x = std::clamp(m_GuardVec.x + Input.GetAddyRad() / 200.f, -1.5f, 1.5f);
			m_GuardVec.y = std::clamp(m_GuardVec.y + Input.GetAddxRad() / 200.f, -1.5f, 1.5f);
			Easing(&m_GuardVecR, m_GuardVec, 0.9f, EasingType::OutExpo);
			//竹刀の回転を反映
			Easing(&m_BambooVecBase, Vector2DX::vget(Input.GetxRad(), Input.GetyRad()), 0.7f, EasingType::OutExpo);
			//
			if (!pReady) {
				Input.ResetKeyInput();
			}
			CharaMove::InputKey(Input, m_MyID == m_ViewID);
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
			}
			//
			ExecuteInput();
			ExecuteAction();
			ExecuteAnim();
			ExecuteSound();
			ExecuteMatrix();
#ifdef _USE_EFFEKSEER_
			EffectControl::Execute();				//
#endif
		}
		//
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//auto* SE = SoundPool::Instance();
			//
			Vector3DX TargetPos = (GetEyePosition() - CharaMove::GetGunMatrix().zvec() * (1000.f * Scale_Rate));
			for (auto& t: m_TurretData) {
				{
					Vector3DX Vec = Matrix3x3DX::Vtrans(this->m_move.GetMat().zvec() * -1.f, Matrix3x3DX::RotAxis(Vector3DX::up(), t.Rad)); Vec.y = 0.f;
					Vector3DX vec_a = (TargetPos - GetObj().GetFramePosition(t.Elevate.GetFrameID())).normalized(); vec_a.y = 0.f;
					float cost = Vector3DX::Cross(vec_a, Vec).y;
					float sint = Vector3DX::Dot(vec_a, Vec);

					t.NowRad = -std::atan2f(cost, sint);

					t.CanShot = (abs(t.NowRadR - t.NowRad) < deg2rad(5.f));//

					t.NowRad = std::clamp(t.NowRad, t.MinusLimit, t.PlusLimit);
					if (!t.CanShot) {
						t.HitLimit = (abs(t.NowRadR - t.NowRad) < deg2rad(5.f));//
					}
				}
				if (abs(t.NowRadR - t.NowRad)<deg2rad(10.f)) {
					Easing(&t.NowRadR, t.NowRad, 0.9f, EasingType::OutExpo);
				}
				else {
					if (t.NowRadR > t.NowRad) {
						t.NowRadR -= deg2rad(60) / DrawParts->GetFps() * t.RotateRate;
					}
					else {
						t.NowRadR += deg2rad(60) / DrawParts->GetFps() * t.RotateRate;
					}
				}

				//旋回
				this->GetObj().SetFrameLocalMatrix(t.Rotate.GetFrameID(),
					Matrix4x4DX::RotAxis(Vector3DX::up(), t.NowRadR)*
					t.Rotate.GetFrameLocalPosition());

				//仰俯角
				//this->GetObj().SetFrameLocalMatrix(t.Elevate.GetFrameID(),
				//	Matrix4x4DX::RotAxis(Matrix3x3DX::RotAxis(Vector3DX::up(), t.Rad).xvec(), deg2rad(45)) *
				//	t.Elevate.GetFrameLocalPosition());

				if (t.NowLoadTime == 0.f && m_ShotCoolDown==0.f) {
					if (t.CanShot && CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
						m_ShotCoolDown = 0.05f + GetRandf(0.04f);
						t.NowLoadTime = t.LoadTime;

						Vector3DX Vec = GetObj().GetFrameLocalWorldMatrix(t.Rotate.GetFrameID()).zvec() * -1.f;
						Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::up(), t.Rad));
						Vector3DX Pos = (Vec * (3.f * Scale_Rate)) + this->GetObj().GetFramePosition(t.Elevate.GetFrameID());
						//
						EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire,
							Pos,
							Vec,
							t.AmmoSize / 6.3f,
							(t.AmmoSize<5.f) ? 3.f : 1.f
							);
						if (m_MyID == m_ViewID) {
							CameraShake::Instance()->SetCamShake(std::clamp(t.AmmoSize / 12.7f, 0.f, 1.f), t.AmmoSize);
							auto* SE = SoundPool::Instance();
							if (t.AmmoSize < 5.f) {
								SE->Get(static_cast<int>(SoundEnum::Shot)).Play_3D(0, this->GetObj().GetFramePosition(t.Elevate.GetFrameID()),
									Scale_Rate * 300.f);
							}
							else {
								SE->Get(static_cast<int>(SoundEnum::ShotBig)).Play_3D(0, this->GetObj().GetFramePosition(t.Elevate.GetFrameID()),
									Scale_Rate * 500.f);
							}
						}
						auto* ObjMngr = ObjectManager::Instance();

						auto LastAmmo = std::make_shared<AmmoClass>();
						ObjMngr->AddObject(LastAmmo);
						LastAmmo->Init();
						LastAmmo->Put((HitPoint)(t.AmmoSize*10), t.AmmoSize/50.f, Pos, Vec, m_MyID);

					}
				}
				else {
					t.NowLoadTime = std::max(0.f, t.NowLoadTime - 1.f / DrawParts->GetFps());
				}
			}
			m_ShotCoolDown = std::max(m_ShotCoolDown - 1.f / DrawParts->GetFps(), 0.f);
			//
			CharaMove::UpdateKeyRad(this->m_move);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//アニメ演算
			//CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Upper_Ready) = 1.f;
			//ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Turn), 0.5f);
		}
		void			CharacterClass::ExecuteSound(void) noexcept {
			//auto* DrawParts = DXDraw::Instance();
			//auto* SE = SoundPool::Instance();
			//しゃがみ音
			//SE->Get(static_cast<int>(SoundEnum::StandupFoot)).Play_3D(0, GetFramePosition(CharaFrame::Upper), Scale_Rate * 3.f);
		}
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			Vector3DX pos = this->m_move.GetPosBuf();
			//vector
			bool IsHitGround = false;//(pos.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			if(false){
				Vector3DX EndPos = pos - Vector3DX::up() * 5.f;
				IsHitGround = BackGround->CheckLinetoMap(pos + Vector3DX::up() * 20.f, &EndPos, true);
				groundYpos = EndPos.y;
			}
			else {
				IsHitGround = true;
				groundYpos = 0.f;
			}
			Vector3DX vec = CharaMove::GetVec();
			if (IsHitGround) {
				auto yPos = pos.y;
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				pos.y = (yPos);
				vec.y = (0.f);
			}
			else {
				vec.y = (this->m_move.GetVec().y + (M_GR / (DrawParts->GetFps() * DrawParts->GetFps())));
			}
			this->m_move.SetVec(vec);
			pos += this->m_move.GetVec();
			//壁判定
			//BackGround->CheckMapWall(this->m_move.GetRePos(), &pos, 0.6f * Scale_Rate);
			//ほかプレイヤーとの判定
			{
				/*
				float Radius = 0.5f * Scale_Rate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					if (i == this->m_MyID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					//自分が当たったら押し戻す
					Vector3DX Vec = (c->GetMove().GetPos() - this->GetMove().GetPos()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						pos += Vec.normalized() * (Len - Radius);
					}
				}
				for (int i = 0; i < PlayerMngr->GetNPCNum(); ++i) {
					if (i == this->m_MyID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetNPC(i)->GetChara();
					//自分が当たったら押し戻す
					Vector3DX Vec = (c->GetMove().GetPos() - this->GetMove().GetPos()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						pos += Vec.normalized() * (Len - Radius);
					}
				}
				//*/
			}
			//座標オーバーライド
			if (this->m_MoveOverRideFlag) {
				this->m_MoveOverRideFlag = false;
				this->m_move = this->m_OverRideInfo;
			}

			this->m_move.SetPos(pos);
			this->m_move.SetMat(CharaMove::GetBaseRotMatrix());
			this->m_move.Update(0.9f, 0.f);
			UpdateObjMatrix(CharaMove::GetShipSwingMat() * this->m_move.GetMat(), this->m_move.GetPos());
			m_EyePosition = GetMove().GetPos()
				+ Vector3DX::up() * (Lerp(25.f, 50.f, GetEyeRad().x / deg2rad(-15.f)) * Scale_Rate);

			this->m_WaveObj.SetMatrix(Matrix4x4DX::GetScale(Vector3DX::vget(0.75f, 0.5f, 0.5f))
				*Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.f, -m_WaveFront))
				*(this->m_move.GetMat().Get44DX() * Matrix4x4DX::Mtrans(this->m_move.GetPos()))
			);
			this->m_WaveScr += m_WaveAlpha * CharaMove::GetSpeedrPer()  * -0.001f;
			for (int i = 0; i < this->m_WaveObj.GetMeshNum(); i++) {
				MV1SetFrameTextureAddressTransform(this->m_WaveObj.GetHandle(), i, this->m_WaveScr, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
			}
			if (CharaMove::GetSpeedrPer() > 0.f) {
				m_WaveAlpha = std::min(m_WaveAlpha + 0.5f / DrawParts->GetFps(), 1.f);
			}
			else {
				m_WaveAlpha = std::max(m_WaveAlpha - 0.1f / DrawParts->GetFps(), 0.f);
			}
			this->m_WaveObj.SetOpacityRate(m_WaveAlpha * CharaMove::GetMoverPer());

			float per = ((float)m_HP / (float)GetHPMax());
			for (int i = 0, num = this->GetObj().GetMaterialNum(); i < num; ++i) {
				this->GetObj().SetMaterialDifColor(i, GetColorF(1.0f, per, per, 1.f));
			}
		}
	}
}
