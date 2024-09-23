#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		//
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
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
			StaminaControl::InitStamina();
			HitBoxControl::InitHitBox();
			this->m_MoveOverRideFlag = false;
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
		}
		//
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputControl Input = pInput;
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
			//
			CharaMove::UpdateKeyRad(this->m_move);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//上半身アニメ演算
			CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Upper_Ready) = 1.f;
			CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Turn), 0.5f);
			ObjectBaseClass::SetAnimOnce(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Attack), CharaMove::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Run), CharaMove::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaMove::GetBottomWalkAnimSel()), CharaMove::GetVecFront());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_LeftStep), CharaMove::GetVecLeft());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_WalkBack), CharaMove::GetVecRear());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_RightStep), CharaMove::GetVecRight());
			//アニメ反映
			for (int i = 0, Num = static_cast<int>(GetObj().GetAnimNum()); i < Num; ++i) {
				this->GetObj().SetAnim(i).SetPer(CharaMove::GetCharaAnimeBufID((CharaObjAnimeID)i));
			}
			GetObj().UpdateAnimAll();
		}
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
				if (!GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {

								SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time && Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {

								SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 5.f);
							}
						}
					}
				}
				else {
					if (CharaMove::GetCharaAnimeBufID(CharaObjAnimeID::Bottom_Stand_Attack) > 0.5f) {
						if ((9.f < Time && Time < 10.f)) {
							if (this->m_CharaSound != 7) {
								this->m_CharaSound = 7;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::Kendo_Foot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 15.f);
								}
							}
						}
					}
					else {
						//L
						if ((3.f < Time && Time < 4.f)) {
							if (this->m_CharaSound != 5) {
								this->m_CharaSound = 5;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 15.f);
								}
							}
						}
						//R
						if ((10.f < Time && Time < 11.f)							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 15.f);
								}
							}
						}
					}
				}
			}
			else {
				this->m_CharaSound = -1;
			}
			//しゃがみ音
			if (CharaMove::GetSquatSwitch()) {
				SE->Get(static_cast<int>(SoundEnum::StandupFoot)).Play_3D(0, GetFramePosition(CharaFrame::Upper), Scale_Rate * 3.f);
			}
			if (StaminaControl::ExcuteStamina(0.0f / DrawParts->GetFps())) {
				if (this->m_MyID == this->m_ViewID) {
					SE->Get(static_cast<int>(SoundEnum::Heart)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			float XRad = 0.f;
			this->m_move.GetMat().GetRadian(&XRad, nullptr, nullptr);
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				(Matrix3x3DX::RotAxis(Vector3DX::right(), -XRad / 2.f) * CharaMove::GetUpperRotMatrix()).Get44DX() *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				Matrix3x3DX::RotAxis(Vector3DX::right(), XRad / 2.f).Get44DX() *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper2)));
			//
			Vector3DX pos = this->m_move.GetPosBuf();
			//vector
			bool IsHitGround = false;//(pos.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			if(true){
				Vector3DX EndPos = pos - Vector3DX::up() * (0.5f * Scale_Rate);
				IsHitGround = BackGround->CheckLinetoMap(pos + Vector3DX::up() * (0.f * Scale_Rate), &EndPos);
				groundYpos = EndPos.y;
			}
			else {
				IsHitGround = (pos.y <= groundYpos);
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
			BackGround->CheckMapWall(this->m_move.GetRePos(), &pos, 0.6f * Scale_Rate);
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
			m_EyePosition = (GetFramePosition(CharaFrame::LeftEye) + GetFramePosition(CharaFrame::RightEye)) / 2.f + (CharaMove::GetEyeRotMatrix() * CharaMove::GetBaseRotMatrix()).zvec() * (-0.04f * Scale_Rate);
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != this->m_ViewID) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				m_EyePosition += CharaMove::GetEyeSwingPos();
			}
		}
	}
}
