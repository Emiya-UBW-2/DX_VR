#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		//
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				if (m_DamageCoolTime == 0.f && (GetRand(100) <= 30)) {
					auto* SE = SoundPool::Instance();
					//SE->Get(static_cast<int>(SoundEnum::Kendo_Hit)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 15.f);
					m_DamageCoolTime = static_cast<float>(SE->Get(static_cast<int>(SoundEnum::Kendo_Hit)).GetTotalTIme(0, 0)) / 1000.f;
				}
				//コンカッション
				if (this->m_MyID == this->m_ViewID) {
					switch (value.GetHitType()) {
					case HitType::Head://面
						m_ConcussionSwitch = true;
						break;
					case HitType::Body://胴
						m_ConcussionSwitch = true;
						break;
					case HitType::Arm://小手
						m_ConcussionSwitch = true;
						break;
					case HitType::Leg:
					default:
						break;
					}
				}
				else {
					HitMark::Instance()->Add(value.m_Pos, value.GetHitType(), static_cast<float>(value.Damage) / 100.f);
				}
				//ダメージ
				return true;
			}
			return false;
		}
		//
		bool		CharacterClass::CheckDamageRay(HitPoint* Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale_Rate)) { return false; }
			//被弾処理
			if (false) {
				//ダメージ登録
				{
					//m_Damage.Add(AttackID, this->m_MyID, *Damage, HitPtr->GetColType(), *pEndPos);
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
				}
			}
			int mdata = FileRead_open((this->GetFilePath() + "data.txt").c_str(), FALSE);
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
						m_TurretData.at(index).AmmoSize = std::stof(RIGHT.substr(0, div));
						RIGHT = RIGHT.substr(div + 1);
					}
					{
						m_TurretData.at(index).LoadTime = std::stof(RIGHT);
					}
					index++;
				}
			}
			FileRead_close(mdata);
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
			CharaMove::InputKey(Input);
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
			Vector3DX TargetVec = CharaMove::GetEyeMatrix().zvec() * -1.f;
			for (auto& t: m_TurretData) {
				{
					Vector3DX Vec = Matrix3x3DX::Vtrans(CharaMove::GetBaseRotMatrix().zvec() * -1.f, Matrix3x3DX::RotAxis(Vector3DX::up(), t.Rad)); Vec.y = 0.f;
					Vector3DX vec_a = TargetVec; vec_a.y = 0.f;
					float cost = Vector3DX::Cross(vec_a, Vec).y;
					float sint = Vector3DX::Dot(vec_a, Vec);

					t.NowRad = -std::atan2f(cost, sint);
					t.NowRad = std::clamp(t.NowRad, t.MinusLimit, t.PlusLimit);
				}
				if (abs(t.NowRadR - t.NowRad)<deg2rad(10.f)) {
					Easing(&t.NowRadR, t.NowRad, 0.9f, EasingType::OutExpo);
				}
				else {
					if (t.NowRadR > t.NowRad) {
						t.NowRadR -= deg2rad(60) / DrawParts->GetFps();
					}
					else {
						t.NowRadR += deg2rad(60) / DrawParts->GetFps();
					}
				}

				//旋回
				this->GetObj().SetFrameLocalMatrix(t.Rotate.GetFrameID(),
					Matrix4x4DX::RotAxis(Vector3DX::up(), t.NowRadR)*
					t.Rotate.GetFrameLocalPosition());

				//仰俯角
				this->GetObj().SetFrameLocalMatrix(t.Elevate.GetFrameID(),
					Matrix4x4DX::RotAxis(Matrix3x3DX::RotAxis(Vector3DX::up(), t.Rad).xvec(), deg2rad(45)) *
					t.Elevate.GetFrameLocalPosition());
			}
			//掛け声
			m_YaTimer = std::max(m_YaTimer - 1.f / DrawParts->GetFps(), 0.f);
			m_DamageCoolTime = std::max(m_DamageCoolTime - 1.f / DrawParts->GetFps(), 0.f);
			m_TsubaSoundCoolTime = std::max(m_TsubaSoundCoolTime - 1.f / DrawParts->GetFps(), 0.f);
			m_GuardHit = std::max(m_GuardHit - 1.f / DrawParts->GetFps(), 0.f);
			//
			CharaMove::UpdateKeyRad(this->m_move);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//アニメ演算
			//CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Upper_Ready) = 1.f;
			//ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Turn), 0.5f);
			//アニメ反映
			for (int i = 0, Num = static_cast<int>(GetObj().GetAnimNum()); i < Num; ++i) {
				this->GetObj().SetAnim(i).SetPer(CharaMove::GetCharaAnimeBufID((CharaObjAnimeID)i));
			}
			GetObj().UpdateAnimAll();
		}
		void			CharacterClass::ExecuteSound(void) noexcept {
			//auto* DrawParts = DXDraw::Instance();
			//auto* SE = SoundPool::Instance();
			//しゃがみ音
			if (CharaMove::GetSquatSwitch()) {
				//SE->Get(static_cast<int>(SoundEnum::StandupFoot)).Play_3D(0, GetFramePosition(CharaFrame::Upper), Scale_Rate * 3.f);
			}
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
			this->m_move.Update(0.9f, 0.f);
			UpdateObjMatrix(CharaMove::GetBaseRotMatrix(), this->m_move.GetPos());
			{
				Matrix3x3DX Mat; Mat.SetRadian(CharaMove::GetRadBuf().x, CharaMove::GetRadBuf().y, 0.f);
				this->m_move.SetMat(Mat);
			}
			m_EyePosition = GetMove().GetPos()+Vector3DX::up()*(25.f*Scale_Rate);
		}
	}
}
