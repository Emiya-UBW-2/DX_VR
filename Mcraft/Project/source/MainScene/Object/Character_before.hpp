#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../MainScene/NetWork.hpp"
#include "CharaAnimData.hpp"
#include "CharacterEnum.hpp"

#include "Gun.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//�L�����̂�������@�\���������o��������
		//
		enum class HitType {
			Head,
			Body,
			Arm,
			Leg,
		};
		enum class EnumGunReadySeq {
			LowReady,
			Aim,
			ADS,
		};
		//
		class HitBox {
			Vector3DX	m_pos;
			float		m_radius{ 0.f };
			HitType		m_HitType{ HitType::Body };
		public:
			void	Update(const Vector3DX& pos, float radius, HitType pHitType) {
				this->m_pos = pos;
				this->m_radius = radius;
				this->m_HitType = pHitType;
			}
			void	Draw(void) const noexcept {
				unsigned int color;
				switch (this->m_HitType) {
				case HitType::Head:
					color = Red;
					break;
				case HitType::Body:
					color = Green;
					break;
				case HitType::Arm:
					color = Blue;
					break;
				case HitType::Leg:
					color = Blue;
					break;
				default:
					break;
				}
				DrawSphere_3D(this->m_pos, this->m_radius, color, color);
			}

			bool	Colcheck(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				if (HitCheck_Sphere_Capsule(
					this->m_pos.get(), this->m_radius,
					StartPos.get(), pEndPos->get(), 0.001f * Scale3DRate
				) == TRUE) {
					VECTOR pos1 = StartPos.get();
					VECTOR pos2 = pEndPos->get();
					VECTOR pos3 = this->m_pos.get();
					SEGMENT_POINT_RESULT Res;
					Segment_Point_Analyse(&pos1, &pos2, &pos3, &Res);

					*pEndPos = Res.Seg_MinDist_Pos;

					return TRUE;
				}
				return FALSE;
			}
			const auto GetColType()const noexcept { return this->m_HitType; }
			const auto GetPos()const noexcept { return this->m_pos; }
		};
		//�q�b�g�|�C���g�Ȃǂ̃p�����[�^�[
		template<class Point, int MaxPoint>
		class PointControl {
		private://�p�����[�^�[
			const Point											Max = MaxPoint;
		private:
			Point												m_HP{ 0 };							//�X�R�A
		public://�Q�b�^�[
			const auto		IsNotZero(void) const noexcept { return this->m_HP != 0; }
			const auto& GetPoint(void) const noexcept { return this->m_HP; }
			const auto& GetMax(void) const noexcept { return Max; }
			void			Sub(Point damage_t) noexcept { this->m_HP = std::clamp<Point>(this->m_HP - damage_t, 0, Max); }
		public:
			void		Init(void) noexcept { Sub(-Max); }
		};
		//�L��������
		class LeanControl {
		private:
			float												m_Rad{ 0.f };
			int													m_Rate{ 0 };
		public://�Q�b�^�[
			const auto& GetRad(void) const noexcept { return this->m_Rad; }
		public:
			void			Init(void) noexcept {
				this->m_Rad = 0.f;
			}
			bool			Update(bool LeftTrigger, bool RightTrigger) {
				//����
				auto Prev = this->m_Rate;
				if (LeftTrigger) {
					if (this->m_Rate != -1) {
						this->m_Rate = -1;
					}
					else {
						this->m_Rate = 0;
					}
				}
				if (RightTrigger) {
					if (this->m_Rate != 1) {
						this->m_Rate = 1;
					}
					else {
						this->m_Rate = 0;
					}
				}
				Easing(&this->m_Rad, static_cast<float>(this->m_Rate) * deg2rad(25), 0.9f, EasingType::OutExpo);
				return (Prev != this->m_Rate);
			}
		};
		class MoveControl {
		private:
			float												m_VecPower{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
		public://�Q�b�^�[
			const auto& GetVecTotal(void) const noexcept { return this->m_VecTotal; }
			const auto GetVecMove(void) const noexcept {
				if (this->m_VecPower > 0.f) {
					return this->m_VecTotal.normalized() * std::clamp(this->m_VecPower, 0.f, 1.f);
				}
				return Vector3DX::zero();
			}
			const auto GoFrontRad(void) const noexcept {
				return atan2f(this->m_VecTotal.x, -this->m_VecTotal.z) * -this->m_VecTotal.z;
			}
			const auto GoBackRad(void) const noexcept {
				return atan2f(-this->m_VecTotal.x, this->m_VecTotal.z) * this->m_VecTotal.z;
			}

			const auto& GetVecPower(void) const noexcept { return this->m_VecPower; }
			const auto& GetVecFront(void) const noexcept { return this->m_Vec[0]; }
			const auto& GetVecRear(void) const noexcept { return this->m_Vec[2]; }
			const auto& GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
			const auto& GetVecRight(void) const noexcept { return this->m_Vec[3]; }
		public:
			void			Init(void) noexcept {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
			}
			void			Update(bool WKey, bool AKey, bool SKey, bool DKey) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (WKey ? 3.f : -7.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (AKey ? 3.f : -7.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (SKey ? 3.f : -7.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (DKey ? 3.f : -7.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				this->m_VecPower = this->m_VecTotal.magnitude();
			}
		};
		class RotateControl {
		private:
			Vector3DX											m_rad_Buf, m_rad;
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			bool												m_TurnBody{ false };
		public://�Z�b�^�[
			const auto&		IsTurnBody(void) const noexcept { return this->m_TurnBody; }
			const auto&		GetRad(void) const noexcept { return this->m_rad; }
			const auto&		GetYRadUpper(void) const noexcept { return this->m_yrad_Upper; }
			const auto&		GetYRadBottom(void) const noexcept { return this->m_yrad_Bottom; }
			const auto		GetYRadBottomChange(void) const noexcept { return GetRad().y - this->m_yrad_Bottom; }
		public:
			void			Init(float pxRad, float pyRad) {
				this->m_rad_Buf.x = (pxRad);
				this->m_rad_Buf.y = (pyRad);
				this->m_rad = this->m_rad_Buf;
				this->m_yrad_Upper = GetRad().y;
				this->m_yrad_Bottom = GetRad().y;
				this->m_TurnBody = false;
			}
			void Update(float pxRad, float pyRad, bool IsMove, float pyRadFront) {
				if (!IsMove) {
					float Change = abs(GetRad().y - this->m_yrad_Upper);
					if (deg2rad(50.f) < Change) {
						this->m_TurnBody = true;
					}
					if (Change < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}


				this->m_rad_Buf.x = std::clamp(this->m_rad_Buf.x + pxRad, deg2rad(-70.f), deg2rad(24.f));
				this->m_rad_Buf.y = this->m_rad_Buf.y + pyRad;
				Easing(&this->m_rad.x, this->m_rad_Buf.x, 0.5f, EasingType::OutExpo);
				Easing(&this->m_rad.y, this->m_rad_Buf.y, 0.8f, EasingType::OutExpo);
				Easing(&this->m_rad.z, this->m_rad_Buf.z, 0.5f, EasingType::OutExpo);



				if (this->m_TurnBody || IsMove) { Easing(&this->m_yrad_Upper, GetRad().y, 0.85f, EasingType::OutExpo); }
				auto PrevYradBottom = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - pyRadFront, 0.85f, EasingType::OutExpo);
				auto YradChange = this->m_yrad_Bottom - PrevYradBottom;
				Easing(&this->m_rad_Buf.z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
			}
		};
		//
		class HitBoxControl {
		private:
			std::vector<HitBox>									m_HitBox;
		public:
			const HitBox* GetLineHit(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(StartPos, pEndPos)) {
						return &h;
					}
				}
				return nullptr;
			}
		public:
			void		CheckLineHitNearest(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				for (auto& h : this->m_HitBox) {
					h.Colcheck(StartPos, pEndPos);
				}
			}
			const auto& GetHitBoxPointList() const { return this->m_HitBox; }
		public:
			HitBoxControl(void) noexcept {}
			~HitBoxControl(void) noexcept {}
		public:
			void Init(void) noexcept {
				this->m_HitBox.resize(27);
			}
			void Update(const ObjectBaseClass* ptr, float SizeRate) noexcept;
			void Draw(void) noexcept {
				//GetObj().SetOpacityRate(0.5f);
				SetFogEnable(FALSE);
				SetUseLighting(FALSE);
				//SetUseZBuffer3D(FALSE);

				for (auto& h : this->m_HitBox) {
					h.Draw();
				}

				//SetUseZBuffer3D(TRUE);
				SetUseLighting(TRUE);
			}
		};
		//�������̗h��
		class WalkSwingControl {
		private:
			Vector3DX											m_WalkSwing{};
			Vector3DX											m_WalkSwing_p{};
			Vector3DX											m_WalkSwing_t{};
			float												m_PrevY{};
			float												m_MoveEyePosTimer{ 0.f };
			Vector3DX											m_MoveEyePosLocal{};
			Vector3DX											m_MoveEyePos{};
		public:
			WalkSwingControl(void) noexcept {}
			~WalkSwingControl(void) noexcept {}
		public:
			const auto& GetMoveEyePos(void) const noexcept { return m_MoveEyePosLocal; }
		public:
			const auto CalcWalk(const Vector3DX& Pos, float SwingPer)noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				//X
				{
					if (this->m_PrevY > Pos.y) {
						this->m_WalkSwing_t.x = 1.f;
					}
					else {
						this->m_WalkSwing_t.x = std::max(this->m_WalkSwing_t.x - 15.f * DXLib_refParts->GetDeltaTime(), 0.f);
					}
					this->m_PrevY = Pos.y;
				}
				//Z
				{
					if (this->m_WalkSwing_t.x == 1.f) {
						if (this->m_WalkSwing_t.z >= 0.f) {
							this->m_WalkSwing_t.z = -1.f;
						}
						else {
							this->m_WalkSwing_t.z = 1.f;
						}
					}
				}
				Easing(&this->m_WalkSwing_p.x, this->m_WalkSwing_t.x * SwingPer, (this->m_WalkSwing_p.x > this->m_WalkSwing_t.x * SwingPer) ? 0.6f : 0.9f, EasingType::OutExpo);
				Easing(&this->m_WalkSwing_p.z, this->m_WalkSwing_t.z * SwingPer, 0.95f, EasingType::OutExpo);
				Easing(&this->m_WalkSwing, this->m_WalkSwing_p, 0.5f, EasingType::OutExpo);
				Vector3DX WalkSwingRad = Vector3DX::vget(5.f, 0.f, 10.f);
				return Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(this->m_WalkSwing.z * WalkSwingRad.z)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(this->m_WalkSwing.x * WalkSwingRad.x));
			}
			const auto& CalcEye(const Matrix3x3DX& pCharaMat, float SwingPer, float SwingSpeed) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (SwingPer > 0.f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(SwingSpeed) * 60.f * DXLib_refParts->GetDeltaTime();
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = Matrix3x3DX::Vtrans(Vector3DX::up() * (0.25f * SwingPer), Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
				EyePos.y = -std::abs(EyePos.y);
				Easing(&this->m_MoveEyePosLocal, EyePos, 0.9f, EasingType::OutExpo);
				Easing(&this->m_MoveEyePos, Matrix3x3DX::Vtrans(EyePos, pCharaMat), 0.9f, EasingType::OutExpo);
				return this->m_MoveEyePos;
			}
		};
		//
		class GunPtrControl {
		private:
		private:
			int													m_GunSelect{ 0 };
			int													m_ReserveGunSelect{ 0 };
			std::array<std::shared_ptr<GunClass>, 3>			m_GunPtr{};			//�e
		public://�Q�b�^�[
			auto&				GetGunPtr(int ID) noexcept { return this->m_GunPtr[ID]; }
			const auto&			GetGunPtr(int ID) const noexcept { return this->m_GunPtr[ID]; }
			const auto			GetGunNum(void) const noexcept { return static_cast<int>(this->m_GunPtr.size()); }
			const auto			GetNowGunSelect(void) const noexcept { return this->m_GunSelect; }
		public://�Z�b�^�[
			//���̕���ɐ؂�ւ�
			void				GunChangeNext(bool IsUpDown) noexcept {
				int Next = GetNowGunSelect();
				while (true) {
					Next += (IsUpDown ? 1 : -1);
					if (Next >= GetGunNum()) {
						Next -= GetGunNum();
					}
					else if (Next < 0) {
						Next += GetGunNum();
					}
					if (GetGunPtr(Next)) {
						this->m_ReserveGunSelect = Next;
						break;
					}
				}
			}
			//��������ł͂Ȃ��ŏ��̕���ɐ؂�ւ�
			void				GunChangeThrowWeapon(bool isThrow) noexcept {
				for (int loop = 0; loop < GetGunNum(); ++loop) {
					auto& p = GetGunPtr(loop);
					if (!p) { continue; }

					if (isThrow ? p->GetModSlot().GetModData()->GetIsThrowWeapon() : !p->GetModSlot().GetModData()->GetIsThrowWeapon()) {
						this->m_ReserveGunSelect = loop;
						break;
					}
				}
			}

			const auto			IsChangeGunSel(void) const noexcept { return GetNowGunSelect() != this->m_ReserveGunSelect; }
			void				InvokeReserveGunSel(void) noexcept { this->m_GunSelect = this->m_ReserveGunSelect; }
			void				SelectGun(int ID) noexcept { this->m_GunSelect = this->m_ReserveGunSelect = ID; }
			void				SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_GunPtr[ID] = pGunPtr0; }
		public:
			GunPtrControl(void) noexcept {}
			~GunPtrControl(void) noexcept {}
		public:
			void				Dispose(void) noexcept {
				for (auto& g : this->m_GunPtr) {
					if (!g) { return; }
					auto* ObjMngr = ObjectManager::Instance();
					ObjMngr->DelObj((SharedObj*)&g);
					g.reset();
				}
			}
		};
		//
		class HitReactionControl {
		private:
			Vector3DX											m_HitAxis{ Vector3DX::forward() };
			float												m_HitPower{ 0.f };
			float												m_HitPowerR{ 0.f };
		private:
		public:
			HitReactionControl(void) noexcept {}
			~HitReactionControl(void) noexcept {}
		public:
			const auto GetHitReactionMat(void) const noexcept { return Matrix3x3DX::RotAxis(this->m_HitAxis, this->m_HitPowerR * deg2rad(90.f)); }
			const auto IsDamaging(void) const noexcept { return this->m_HitPower > 0.f; }
		public:
			void SetHit(const Vector3DX& Axis) noexcept {
				this->m_HitAxis = Axis;
				this->m_HitPower = 1.f;
			}
			void Update(void) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				Easing(&this->m_HitPowerR, this->m_HitPower, 0.8f, EasingType::OutExpo);
				this->m_HitPower = std::max(this->m_HitPower - DXLib_refParts->GetDeltaTime() / 0.3f, 0.f);
			}
		};
		//���O�h�[��
		enum class RagFrame {
			//��
			head,
			//����
			bodyg,
			bodyc,
			bodyb,
			body,
			//�E����W�n
			RIGHThand2,
			RIGHThand,
			RIGHTarm2,
			RIGHTarm1,
			//������W�n
			LEFThand2,
			LEFThand,
			LEFTarm2,
			LEFTarm1,

			//�E����W�n
			RIGHTreg2,
			RIGHTreg,
			RIGHTfoot2,
			RIGHTfoot1,
			//������W�n
			LEFTreg2,
			LEFTreg,
			LEFTfoot2,
			LEFTfoot1,

			Max,
		};
		static const char* RagFrameName[static_cast<int>(RagFrame::Max)] = {
			"��",
			"�O���[�u",
			"�����g",
			"�㔼�g",
			"�㔼�g2",
			"�E����",
			"�E���",
			"�E�Ђ�",
			"�E�r",
			"������",
			"�����",
			"���Ђ�",
			"���r",
			"�E�����",
			"�E����",
			"�E�Ђ�",
			"�E��",
			"�������",
			"������",
			"���Ђ�",
			"����",
		};
		class RagDollControl {
		private:
			//�̂̃t���[�����
			class frame_body {
			public:
				std::array<frames, static_cast<int>(RagFrame::Max)> m_Frames;
			public:
				//
				void SetupFrameInfo(const MV1& obj_) noexcept {
					for (int i = 0; i < int(obj_.GetFrameNum()); ++i) {
						std::string p = obj_.GetFrameName(i);
						for (auto& f : this->m_Frames) {
							int index = static_cast<int>(&f - &this->m_Frames.front());
							if (p == RagFrameName[index]) {
								f.Set(i, obj_);
								break;
							}
						}
					}
				}
				//
				void CopyFrame(const MV1& mine, const frame_body& frame_tgt_, MV1* tgt) noexcept {
					tgt->SetMatrix(mine.GetMatrix());
					for (const auto& f : frame_tgt_.m_Frames) {
						tgt->SetFrameLocalMatrix(f.GetFrameID(), mine.GetFrameLocalMatrix(f.GetFrameID()));
					}
					for (int i = 0, max = static_cast<int>(tgt->GetAnimNum()); i < max; ++i) {
						tgt->SetAnim(i).SetPer(mine.GetAnim(i).GetPer());
						tgt->SetAnim(i).SetTime(mine.GetAnim(i).GetTime());
					}
					tgt->UpdateAnimAll();
				}
				//
			};
		private:
			const MV1*											m_pBaseObj{nullptr};
			MV1													m_RagDoll;
			float												m_Timer{ 0.f };						//���O�h�[���̕������Z�^�C�}�[
			frame_body											m_RagObjFrame;						//�t���[��
			frame_body											m_BaseObjFrame;						//�t���[��
		public:
		public:
			RagDollControl(void) noexcept {}
			~RagDollControl(void) noexcept {}
		public:
			auto& SetRagDoll(void) noexcept { return this->m_RagDoll; }
			const auto& GetRagDoll(void) const noexcept { return this->m_RagDoll; }

			const auto GetFrameMat(RagFrame select) const noexcept {
				auto& frame = this->m_RagObjFrame.m_Frames.at(static_cast<int>(select));
				return frame.GetFrameWorldPosition().rotation().inverse() * this->m_RagDoll.GetFrameLocalWorldMatrix(frame.GetFrameID());
			}

		public:
			void Init(const MV1& obj_body_t) noexcept {
				this->m_pBaseObj = &obj_body_t;
				if (!this->m_pBaseObj) { return; }
				this->m_BaseObjFrame.SetupFrameInfo(*this->m_pBaseObj);				//�g��
				this->m_RagObjFrame.SetupFrameInfo(this->m_RagDoll);			//���O�h�[��
			}
			void Update(bool isAlive) noexcept {
				if (!this->m_pBaseObj) { return; }
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (isAlive) {
					this->m_Timer = 0.f;
				}
				else {
					if (this->m_Timer == 0.f) {
						this->m_RagDoll.SetPrioritizePhysicsOverAnimFlag(true);
						this->m_BaseObjFrame.CopyFrame(*this->m_pBaseObj, this->m_RagObjFrame, &this->m_RagDoll);
						this->m_RagDoll.PhysicsResetState();
					}
					if (this->m_Timer < 3.f) {
						this->m_RagDoll.SetPrioritizePhysicsOverAnimFlag(true);
						this->m_BaseObjFrame.CopyFrame(*this->m_pBaseObj, this->m_RagObjFrame, &this->m_RagDoll);
						this->m_RagDoll.PhysicsCalculation(1000.f * DXLib_refParts->GetDeltaTime());
					}
					this->m_Timer += DXLib_refParts->GetDeltaTime();
				}
			}
			void Dispose(void) noexcept {
				if (!this->m_pBaseObj) { return; }
				this->m_pBaseObj = nullptr;
				this->m_RagDoll.Dispose();
			}
		};
	};
};
