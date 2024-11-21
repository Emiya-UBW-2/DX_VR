#include	"Judge.hpp"

#include	"Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void JudgeClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			Vector3DX VecTotal;
			bool IsMove = false;
			//回転
			{
				Vector3DX VecToCenter = m_CenterPos - this->GetMove().GetPos();
				this->m_TargetRad += GetRadRad2Rad(this->m_TargetRad, std::atan2f(-VecToCenter.x, -VecToCenter.z)) * GetEasingRatio(EasingType::OutExpo, 0.8f);
			}
			//移動
			{
				Vector3DX Vec = Matrix4x4DX::RotAxis(Vector3DX::up(), this->m_TargetRad).zvec();
				Vector3DX VecToTarget = m_TargetPos - this->GetMove().GetPos();

				float Len = VecToTarget.magnitude();
				float Cross = Vector3DX::Cross(Vec, VecToTarget.normalized()).y;
				float Dot = Vector3DX::Dot(Vec, VecToTarget.normalized());
				bool												m_WKey{ false };
				bool												m_AKey{ false };
				bool												m_SKey{ false };
				bool												m_DKey{ false };
				if (Len > 1.f * Scale3DRate) {
					m_WKey = Dot < 0.f;
					m_AKey = Cross > 0.f;
					m_SKey = Dot > 0.f;
					m_DKey = Cross < 0.f;
				}

				if (Len > 5.f * Scale3DRate) {
					Easing(&m_Speed, 0.8f, 0.9f, EasingType::OutExpo);
				}
				else {
					Easing(&m_Speed, 0.45f, 0.9f, EasingType::OutExpo);
				}

				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (m_WKey ? 5.f : -1.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (m_AKey ? 5.f : -1.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (m_SKey ? 5.f : -1.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (m_DKey ? 5.f : -1.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0.f, this->m_Vec[2] - this->m_Vec[0]);
				IsMove = (VecTotal.magnitude() > 0.01f);
			}
			//下半身の回転を身体の向きに少しずつ寄せる
			{
				float Rad = GetRadRad2Rad(this->m_BodyRad, this->m_TargetRad);
				if (!this->m_TurnBody) {
					//動いたか、向くべき角度が50度を超えたらtrueに
					if (IsMove || (deg2rad(50.f) < abs(Rad))) {
						this->m_TurnBody = true;
					}
				}
				else {
					//動かず、尚且つ目標の角度まで回転したらfalse
					if (!IsMove && (abs(Rad) < deg2rad(0.5f))) {
						this->m_TurnBody = false;
					}
					this->m_BodyRad += Rad * GetEasingRatio(EasingType::OutExpo, 0.85f);
				}
				this->m_BottomRad += GetRadRad2Rad(this->m_BottomRad, this->m_BodyRad) * GetEasingRatio(EasingType::OutExpo, 0.95f);
			}
			//
			if (!m_RedWin && !m_WhiteWin) {
				Vector3DX Pos0 = PlayerMngr->GetPlayer(0)->GetChara()->GetMove().GetPos(); Pos0.y = 0.f;
				Vector3DX Pos1 = PlayerMngr->GetPlayer(1)->GetChara()->GetMove().GetPos(); Pos1.y = 0.f;
				if (Vector3DX::Cross(Vector3DX::right(), Pos0).y < Vector3DX::Cross(Vector3DX::right(), Pos1).y) {
					m_XScale = 1.f;
				}
				else {
					m_XScale = -1.f;
				}
				if (this->GetMove().GetPos().x < m_CenterPos.x) {
					m_XScale *= -1.f;
				}
			}
			//
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(JudgeFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(JudgeFrame::Upper)),
				GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(JudgeFrame::Upper))).rotation()
				* Matrix4x4DX::RotAxis(Vector3DX::up(), (this->m_TargetRad - this->m_BottomRad) * m_XScale)
				* GetFrameBaseLocalMat(static_cast<int>(JudgeFrame::Upper)));
			//アニメ演算
			{
				JudgeAnimeID BottomAnimSelect = JudgeAnimeID::Bottom_Stand;
				if (IsMove) {
					float Rad = std::atan2f(VecTotal.x, VecTotal.y);
					if (deg2rad(45) < Rad && Rad < deg2rad(135)) { BottomAnimSelect = JudgeAnimeID::Bottom_Stand_RightStep; }
					if (Rad < deg2rad(-45) || Rad < deg2rad(45)) { BottomAnimSelect = JudgeAnimeID::Bottom_Stand_Walk; }
					if (deg2rad(-135) < Rad && Rad < deg2rad(-45)) { BottomAnimSelect = JudgeAnimeID::Bottom_Stand_LeftStep; }
					if (Rad < deg2rad(-135) || deg2rad(135) < Rad) { BottomAnimSelect = JudgeAnimeID::Bottom_Stand_WalkBack; }
				}
				for (int i = 0; i < static_cast<int>(JudgeAnimeID::AnimeIDMax); i++) {
					JudgeAnimeID Sel = (JudgeAnimeID)i;
					if (
						Sel == JudgeAnimeID::Bottom_Stand_Walk ||
						Sel == JudgeAnimeID::Bottom_Stand_LeftStep ||
						Sel == JudgeAnimeID::Bottom_Stand_RightStep ||
						Sel == JudgeAnimeID::Bottom_Stand_WalkBack) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((Sel == BottomAnimSelect) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 0.5f);
					}
					if (Sel == JudgeAnimeID::Bottom_Stand) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((Sel == BottomAnimSelect) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
					if (Sel == JudgeAnimeID::Bottom_Stand_Turn) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((this->m_TurnBody && !IsMove) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
					if (Sel == JudgeAnimeID::Upper_Ready) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((!m_RedWin && !m_WhiteWin) ? 6.f : -6.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
					if (Sel == JudgeAnimeID::Upper_RedWin) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((m_RedWin && !m_WhiteWin) ? 6.f : -6.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
					if (Sel == JudgeAnimeID::Upper_WhiteWin) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((!m_RedWin && m_WhiteWin) ? 6.f : -6.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
					this->GetObj().SetAnim(i).SetPer(this->m_AnimPerBuf.at(i));
				}
				ObjectBaseClass::SetAnimLoop(static_cast<int>(JudgeAnimeID::Bottom_Stand_Turn), 0.72f * (m_Speed / 0.45f));
				ObjectBaseClass::SetAnimLoop(static_cast<int>(JudgeAnimeID::Bottom_Stand_Walk), 0.72f * (m_Speed / 0.45f));
				ObjectBaseClass::SetAnimLoop(static_cast<int>(JudgeAnimeID::Bottom_Stand_LeftStep), 0.72f * (m_Speed / 0.45f));
				ObjectBaseClass::SetAnimLoop(static_cast<int>(JudgeAnimeID::Bottom_Stand_WalkBack), 0.72f * (m_Speed / 0.45f));
				ObjectBaseClass::SetAnimLoop(static_cast<int>(JudgeAnimeID::Bottom_Stand_RightStep), 0.72f * (m_Speed / 0.45f));
				GetObj().UpdateAnimAll();
			}
			//壁判定
			Vector3DX PosBuf = this->GetMove().GetPos()
				+ Matrix4x4DX::Vtrans(VecTotal * (m_Speed * 60.f * DXLib_refParts->GetDeltaTime()), Matrix4x4DX::RotAxis(Vector3DX::up(), this->m_TargetRad));

			//ほかプレイヤーとの判定
			{
				float Radius = 0.5f * Scale3DRate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					auto& c = PlayerMngr->GetPlayer(i)->GetChara();
					//自分が当たったら押し戻す
					Vector3DX Vec = (c->GetMove().GetPos() - PosBuf); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						PosBuf += Vec.normalized() * (Len - Radius);
					}
				}
			}


			BackGround->CheckMapWall(this->m_move.GetRePos(), &PosBuf, 0.6f * Scale3DRate);
			this->SetMove().SetPos(PosBuf);
			this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_BottomRad));
			this->SetMove().Update(0.9f, 0.f);
			this->GetObj().SetMatrix(
				Matrix4x4DX::GetScale(Vector3DX::vget(m_XScale, 1.f, 1.f))
				* GetMove().GetMat().Get44DX() * Matrix4x4DX::Mtrans(GetMove().GetPos()));
		}
		void JudgeClass::CheckDraw(void) noexcept {
			this->m_IsDraw = false;
			this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.f * Scale3DRate, -2.5f * Scale3DRate)).get(),
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.5f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}
		}
		void JudgeClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
					if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
						this->GetObj().DrawMesh(i);
					}
				}
			}
		}
		void JudgeClass::DrawShadow(void) noexcept {
			if (this->m_IsActive) {
				this->GetObj().DrawModel();
			}
		}
	};
};
