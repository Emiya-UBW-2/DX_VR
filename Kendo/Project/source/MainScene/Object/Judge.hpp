#pragma once
#include	"../../Header.hpp"
#include "JudgeEnum.hpp"

#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class JudgeClass : public ObjectBaseClass {
		private:
			int													m_ID{ InvalidID };
			float												m_MoverPer{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
			InputControl										m_Input;
			Vector3DX											m_rad_Buf, m_rad;
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_UpperChange{ 0.f }, m_yrad_BottomChange{ 0.f };
			Vector3DX											m_UpperPrevRad;
			Vector3DX											m_UpperRad;
			Vector3DX											m_UpperyVecNormal, m_UpperzVecNormal;
			Vector3DX											m_UpperyVec, m_UpperzVec;
			std::array<float, (int)JudgeAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			bool												m_TurnBody{ false };
			JudgeAnimeID										m_BottomAnimSelect{};
		public: //コンストラクタ、デストラクタ
			JudgeClass(void) noexcept {
				this->m_objType = (int)ObjType::Judge;
				this->m_IsDraw = true;
			}
			~JudgeClass(void) noexcept {}
		private:
			int	GetFrameNum() noexcept override { return (int)JudgeFrame::Max; }
			const char* GetFrameStr(int id) noexcept override { return JudgeFrameName[id]; }
		private:
			const auto		IsMove(void) const noexcept { return m_MoverPer > 0.1f; }
			const auto		GetFrontP(void) const noexcept {
				auto wkey = this->m_Input.GetPADSPress(PADS::MOVE_W);
				auto skey = this->m_Input.GetPADSPress(PADS::MOVE_S);
				auto FrontP = (wkey && !skey) ? (atan2f(m_VecTotal.x, -m_VecTotal.z) * -m_VecTotal.z) : 0.f;
				FrontP += (!wkey && skey) ? (atan2f(-m_VecTotal.x, m_VecTotal.z) * m_VecTotal.z) : 0.f;
				return FrontP;
			}
			const auto		GetSpeedPer(void) const noexcept { return 0.65f; }
			const auto		GetVec(void) const noexcept {
				auto* DrawParts = DXDraw::Instance();
				Vector3DX vecBuf = m_VecTotal;
				if (m_MoverPer > 0.f) {
					vecBuf = vecBuf.normalized() * (GetSpeedPer() * 60.f / DrawParts->GetFps());
				}
				vecBuf = Matrix4x4DX::Vtrans(vecBuf, Matrix4x4DX::RotAxis(Vector3DX::up(), this->m_yrad_Upper));
				return vecBuf;
			}
			const auto		GetVecFront(void) const noexcept { return 1.15f * this->m_Vec[0] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
			const auto		GetVecRear(void) const noexcept { return 1.15f * this->m_Vec[2] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
			const auto		GetVecLeft(void) const noexcept { return 1.15f * this->m_Vec[1] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
			const auto		GetVecRight(void) const noexcept { return 1.15f * this->m_Vec[3] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
		public:
			void			SetID(int ID) noexcept { this->m_ID = ID; }
			void			MovePoint(float pyRad, const Vector3DX& pPos) noexcept {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				this->m_Input.ResetAllInput();
				this->m_rad_Buf.y = (pyRad);
				this->m_rad = this->m_rad_Buf;

				this->m_yrad_Upper = this->m_rad.y;
				this->m_yrad_Bottom = this->m_rad.y;
				this->m_yrad_BottomChange = 0.f;
				this->m_TurnBody = false;
				//
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
				SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_rad.y), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_rad.y));
			}
			void			SetInput(const InputControl& pInput, bool pReady) noexcept {
				auto* DrawParts = DXDraw::Instance();
				this->m_Input = pInput;
				if (!pReady) {
					this->m_Input.ResetKeyInput();
				}
				//回転
				{
					this->m_rad_Buf.y = this->m_rad_Buf.y + this->m_Input.GetAddyRad();
					Easing(&this->m_rad.y, m_rad_Buf.y, 0.8f, EasingType::OutExpo);
					Easing(&this->m_rad.z, m_rad_Buf.z, 0.5f, EasingType::OutExpo);
				}
				//移動
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (this->m_Input.GetPADSPress(PADS::MOVE_W) ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (this->m_Input.GetPADSPress(PADS::MOVE_A) ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (this->m_Input.GetPADSPress(PADS::MOVE_S) ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (this->m_Input.GetPADSPress(PADS::MOVE_D) ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_MoverPer = m_VecTotal.magnitude();
			}
		private: //継承
			void			Init_Sub(void) noexcept override {}
			void			FirstExecute(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				auto* BackGround = BackGround::BackGroundClass::Instance();
				//
				{
					//
					if (!IsMove()) {
						if (deg2rad(50.f) < abs(this->m_yrad_UpperChange)) {
							this->m_TurnBody = true;
						}
						if (abs(this->m_yrad_UpperChange) < deg2rad(0.5f)) {
							this->m_TurnBody = false;
						}
					}
					else {
						this->m_TurnBody = false;
					}

					if (this->m_TurnBody || IsMove()) { Easing(&this->m_yrad_Upper, this->m_rad.y, 0.85f, EasingType::OutExpo); }
					auto YradChange = this->m_yrad_Bottom;
					Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - GetFrontP(), 0.85f, EasingType::OutExpo);
					YradChange = this->m_yrad_Bottom - YradChange;
					float Z = this->m_rad_Buf.z;
					Easing(&Z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
					this->m_rad_Buf.z = (Z);
					this->m_yrad_UpperChange = this->m_rad.y - this->m_yrad_Upper;
					this->m_yrad_BottomChange = this->m_rad.y - this->m_yrad_Bottom;
					//銃の揺れ
					Easing(&m_UpperRad, (this->m_rad - this->m_UpperPrevRad) * -1.f, 0.9f, EasingType::OutExpo);
					m_UpperPrevRad = this->m_rad;
					auto mat = Matrix4x4DX::RotAxis(Vector3DX::right(), m_UpperRad.x) * Matrix4x4DX::RotAxis(Vector3DX::up(), m_UpperRad.y);
					Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);

					this->m_BottomAnimSelect = JudgeAnimeID::Bottom_Stand;
					if (this->m_Input.GetPADSPress(PADS::MOVE_A) && !this->m_Input.GetPADSPress(PADS::MOVE_D)) { this->m_BottomAnimSelect = JudgeAnimeID::Bottom_Stand_LeftStep; }
					if (this->m_Input.GetPADSPress(PADS::MOVE_D) && !this->m_Input.GetPADSPress(PADS::MOVE_A)) { this->m_BottomAnimSelect = JudgeAnimeID::Bottom_Stand_RightStep; }
					if (this->m_Input.GetPADSPress(PADS::MOVE_S) && !this->m_Input.GetPADSPress(PADS::MOVE_W)) { this->m_BottomAnimSelect = JudgeAnimeID::Bottom_Stand_WalkBack; }
					if (this->m_Input.GetPADSPress(PADS::MOVE_W) && !this->m_Input.GetPADSPress(PADS::MOVE_S)) { this->m_BottomAnimSelect = JudgeAnimeID::Bottom_Stand_Walk; }
					//下半身
					Easing(&this->m_AnimPerBuf.at((int)JudgeAnimeID::Bottom_Stand_Turn), (this->m_TurnBody) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
					for (int i = 0; i < (int)JudgeAnimeID::AnimeIDMax; i++) {
						if (
							i == (int)JudgeAnimeID::Bottom_Stand ||
							i == (int)JudgeAnimeID::Bottom_Stand_Walk ||
							i == (int)JudgeAnimeID::Bottom_Stand_LeftStep ||
							i == (int)JudgeAnimeID::Bottom_Stand_RightStep ||
							i == (int)JudgeAnimeID::Bottom_Stand_WalkBack) {
							this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((i == (int)this->m_BottomAnimSelect) ? 6.f : -2.f) / DrawParts->GetFps(), 0.f, 1.f);
						}
					}
				}
				//
				GetObj().ResetFrameUserLocalMatrix(GetFrame((int)JudgeFrame::Upper));
				GetObj().SetFrameLocalMatrix(GetFrame((int)JudgeFrame::Upper),
					GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(JudgeFrame::Upper))).rotation()
					* (
						(Matrix4x4DX::RotAxis(Vector3DX::up(), this->m_yrad_BottomChange) * this->GetMove().GetMat().Get44DX())
						* this->GetMove().GetMat().Get44DX().inverse()
						).rotation()
					* GetFrameBaseLocalMat((int)JudgeFrame::Upper));
				//上半身アニメ演算
				this->m_AnimPerBuf.at((int)JudgeAnimeID::Upper_Ready) = 1.f;
				//下半身アニメ演算
				ObjectBaseClass::SetAnimLoop((int)JudgeAnimeID::Bottom_Stand_Turn, 0.5f);
				ObjectBaseClass::SetAnimLoop((int)JudgeAnimeID::Bottom_Stand_Walk, GetVecFront());
				ObjectBaseClass::SetAnimLoop((int)JudgeAnimeID::Bottom_Stand_LeftStep, GetVecLeft());
				ObjectBaseClass::SetAnimLoop((int)JudgeAnimeID::Bottom_Stand_WalkBack, GetVecRear());
				ObjectBaseClass::SetAnimLoop((int)JudgeAnimeID::Bottom_Stand_RightStep, GetVecRight());
				//アニメ反映
				for (int i = 0; i < GetObj().GetAnimNum(); i++) {
					this->GetObj().SetAnim(i).SetPer(this->m_AnimPerBuf.at((int)(JudgeAnimeID)i));
				}
				GetObj().UpdateAnimAll();
				//vector
				Vector3DX PosBuf = this->GetMove().GetPos();
				bool IsHitGround = (PosBuf.y <= 0.f); //高度0なら止まる
				float groundYpos = 0.f;
				if (IsHitGround) {
					auto yPos = PosBuf.y;
					Easing(&yPos, groundYpos, 0.6f, EasingType::OutExpo);
					PosBuf.y = (yPos);

					Vector3DX vec = GetVec(); vec.y = 0.f;
					this->SetMove().SetVec(vec);
					this->SetMove().Update(0.f, 0.f);
					UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				}
				else {
					float Y = this->GetMove().GetVec().y;
					Vector3DX vec = GetVec(); vec.y = (Y + (GravityRate / (DrawParts->GetFps() * DrawParts->GetFps())));
					this->SetMove().SetVec(vec);
				}
				PosBuf += this->GetMove().GetVec();
				//壁判定
				BackGround->CheckMapWall(this->m_move.GetRePos(), &PosBuf, 0.6f * Scale3DRate);

				this->SetMove().SetPos(PosBuf);
				this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_Bottom));
				this->SetMove().Update(0.9f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			void			CheckDraw(void) noexcept override {
				this->m_IsDraw = false;
				this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.f * Scale3DRate, -2.5f * Scale3DRate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.5f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
						if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
			void			Dispose_Sub(void) noexcept override {}
		};
	};
};
