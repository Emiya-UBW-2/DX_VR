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
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				//ダメージ登録
				{
					m_Damage.Add(AttackID, this->m_MyID, *Damage, HitPtr->GetColType(), *pEndPos);
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
			HitBoxControl::InitHitBox();
			this->m_MoveOverRideFlag = false;
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
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
				for (int i = 0, num = GetObj().GetMaterialNum(); i < num; ++i) {
					GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
					GetObj().SetMaterialAmbColor(i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
				}
				//this->GetObj().SetMaterialTypeAll(DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
			}
			//
			ExecuteInput();
			ExecuteAction();
			ExecuteAnim();
			ExecuteSound();
			ExecuteMatrix();
			HitBoxControl::UpdataHitBox(this, 1.f);									//ヒットボックス
#ifdef _USE_EFFEKSEER_
			EffectControl::Execute();				//
#endif
		}
		//
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//auto* SE = SoundPool::Instance();
			//
			bool IsOutArea = false;
			{
				Vector3DX Vec = this->m_move.GetPos() - Vector3DX::zero();
				float Len = 11.f / 2.f * Scale_Rate;
				if ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z)) {
					IsOutArea = true;
				}
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
