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
				m_pos = pos;
				m_radius = radius;
				m_HitType = pHitType;
			}
			void	Draw(void) const noexcept {
				unsigned int color;
				switch (m_HitType) {
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
				DrawSphere_3D(m_pos, m_radius, color, color);
			}

			bool	Colcheck(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				if (HitCheck_Sphere_Capsule(
					m_pos.get(), m_radius,
					StartPos.get(), pEndPos->get(), 0.001f * Scale3DRate
				) == TRUE) {
					VECTOR pos1 = StartPos.get();
					VECTOR pos2 = pEndPos->get();
					VECTOR pos3 = m_pos.get();
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
			void		Init() { Sub(-Max); }
		};
		//�L��������
		class LeanControl {
		private://�L�����p�����[�^�[
			switchs												m_QKey;
			switchs												m_EKey;
			float												m_Rad{ 0.f };
			int													m_Rate{ 0 };
			bool												m_Switch{ false };
		public://�Q�b�^�[
			const auto& GetRad(void) const noexcept { return this->m_Rad; }
			const auto& GetRate(void) const noexcept { return this->m_Rate; }
			const auto& GetSwitch(void) const noexcept { return this->m_Switch; }
		public:
			void			Init() {
				this->m_Rad = 0.f;
			}
			void			Update(bool LeftPress, bool RightPress) {
				//����
				this->m_QKey.Update(LeftPress);
				this->m_EKey.Update(RightPress);
				auto Prev = this->m_Rate;
				if (true) {//�g�O����
					if (this->m_QKey.trigger()) {
						if (this->m_Rate != 1) {
							this->m_Rate = 1;
						}
						else {
							this->m_Rate = 0;
						}
					}
					if (this->m_EKey.trigger()) {
						if (this->m_Rate != -1) {
							this->m_Rate = -1;
						}
						else {
							this->m_Rate = 0;
						}
					}
				}
				else {
					this->m_Rate = 0;
					if (this->m_QKey.press()) {
						this->m_Rate = 1;
					}
					if (this->m_EKey.press()) {
						this->m_Rate = -1;
					}
				}
				this->m_Switch = (Prev != this->m_Rate);
				Easing(&this->m_Rad, static_cast<float>(-this->m_Rate) * deg2rad(25), 0.9f, EasingType::OutExpo);
			}
		};
		class MoveControl {
		private://�L�����p�����[�^�[
			float												m_VecPower{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
		public://�Q�b�^�[
			const auto& GetVecTotal(void) const noexcept { return this->m_VecTotal; }
			const auto& GetVecPower(void) const noexcept { return this->m_VecPower; }
			const auto& GetVecFront(void) const noexcept { return this->m_Vec[0]; }
			const auto& GetVecRear(void) const noexcept { return this->m_Vec[2]; }
			const auto& GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
			const auto& GetVecRight(void) const noexcept { return this->m_Vec[3]; }
		public:
			void			Init() {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
			}
			void			Update(bool WKey, bool AKey, bool SKey, bool DKey) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (WKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (AKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (SKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (DKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_VecPower = m_VecTotal.magnitude();
			}
		};
		class RotateControl {
		private://�L�����p�����[�^�[
			Vector3DX											m_rad_Buf, m_rad;
			Vector2DX											m_radAdd;
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_UpperChange{ 0.f }, m_yrad_BottomChange{ 0.f };
			Vector3DX											m_UpperPrevRad;
			Vector3DX											m_UpperRad;
			Vector3DX											m_UpperyVecNormal, m_UpperzVecNormal;
			Vector3DX											m_UpperyVec, m_UpperzVec, m_UpperPos;
			bool												m_TurnBody{ false };
		public://�Z�b�^�[
			const auto		IsTurnBody(void) const noexcept { return this->m_TurnBody; }
			const auto		GetRad(void) const noexcept { return this->m_rad; }
			const auto		GetYRadUpper(void) const noexcept { return this->m_yrad_Upper; }
			const auto		GetYRadBottom(void) const noexcept { return this->m_yrad_Bottom; }
			const auto		GetYRadBottomChange(void) const noexcept { return this->m_yrad_BottomChange; }
			const auto		GetGunSwingMat(void) const noexcept { return Matrix3x3DX::Axis1(m_UpperyVec.normalized(), m_UpperzVec.normalized()); }
		public:
			void			Init(float pxRad, float pyRad) {
				this->m_radAdd.Set(0, 0);
				this->m_rad_Buf.x = (pxRad);
				this->m_rad_Buf.y = (pyRad);
				this->m_rad = this->m_rad_Buf;
				this->m_yrad_Upper = GetRad().y;
				this->m_yrad_Bottom = GetRad().y;
				this->m_yrad_BottomChange = 0.f;
				this->m_TurnBody = false;
			}
			void Update(float pxRad, float pyRad, const Vector2DX& pAddRadvec, bool IsMove, float pyRadFront) {
				Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);
				this->m_rad_Buf.x = std::clamp(this->m_rad_Buf.x + pxRad, deg2rad(-70.f), deg2rad(24.f)) + this->m_radAdd.x;
				this->m_rad_Buf.y = this->m_rad_Buf.y + pyRad + this->m_radAdd.y;
				Easing(&this->m_rad.x, m_rad_Buf.x, 0.5f, EasingType::OutExpo);
				Easing(&this->m_rad.y, m_rad_Buf.y, 0.8f, EasingType::OutExpo);
				Easing(&this->m_rad.z, m_rad_Buf.z, 0.5f, EasingType::OutExpo);


				if (!IsMove) {
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

				if (this->m_TurnBody || IsMove) { Easing(&this->m_yrad_Upper, GetRad().y, 0.85f, EasingType::OutExpo); }
				auto YradChange = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - pyRadFront, 0.85f, EasingType::OutExpo);
				YradChange = this->m_yrad_Bottom - YradChange;
				float Z = this->m_rad_Buf.z;
				Easing(&Z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.z = (Z);
				this->m_yrad_UpperChange = GetRad().y - this->m_yrad_Upper;
				this->m_yrad_BottomChange = GetRad().y - this->m_yrad_Bottom;
				//�e�̗h��
				Easing(&m_UpperRad, (GetRad() - this->m_UpperPrevRad) * -1.f, 0.9f, EasingType::OutExpo);
				m_UpperPrevRad = GetRad();
				auto mat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), m_UpperRad.y);
				Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);
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
			const auto& GetHitBoxPointList() const { return m_HitBox; }
		public:
			HitBoxControl(void) noexcept {}
			~HitBoxControl(void) noexcept {}
		public:
			void Init(void) noexcept {
				m_HitBox.resize(27);
			}
			void Update(const ObjectBaseClass* ptr, float SizeRate) noexcept;
			void Draw(void) noexcept {
				//this->GetObj().SetOpacityRate(0.5f);
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
			Vector3DX											m_WalkSwingRad;
			Vector3DX											m_WalkSwing;
			Vector3DX											m_WalkSwing_p;
			Vector3DX											m_WalkSwing_t;
			Vector3DX											m_PrevPos;
		public://�Q�b�^�[
			const auto		GetWalkSwingMat(void) const noexcept {
				return Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_WalkSwing.z * m_WalkSwingRad.z)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(m_WalkSwing.x * m_WalkSwingRad.x));
			}
		public:
			WalkSwingControl(void) noexcept {}
			~WalkSwingControl(void) noexcept {}
		public:
			void Update(const Vector3DX& Pos, float SwingPer)noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				m_WalkSwingRad.Set(5.f, 0.f, 10.f);
				//X
				{
					if (m_PrevPos.y > Pos.y) {
						m_WalkSwing_t.x = (1.f);
					}
					else {
						m_WalkSwing_t.x = (std::max(m_WalkSwing_t.x - 15.f * DXLib_refParts->GetDeltaTime(), 0.f));
					}
				}
				//Z
				{
					if (m_WalkSwing_t.x == 1.f) {
						if (m_WalkSwing_t.z >= 0.f) {
							m_WalkSwing_t.z = (-1.f);
						}
						else {
							m_WalkSwing_t.z = (1.f);
						}
					}
				}
				auto WS_tmp = m_WalkSwing_t * SwingPer;
				//X
				{
					auto tmp = m_WalkSwing_p.x;
					Easing(&tmp, WS_tmp.x, (m_WalkSwing_p.x > WS_tmp.x) ? 0.6f : 0.9f, EasingType::OutExpo);
					m_WalkSwing_p.x = (tmp);
				}
				//Z
				{
					auto tmp = m_WalkSwing_p.z;
					Easing(&tmp, WS_tmp.z, 0.95f, EasingType::OutExpo);
					m_WalkSwing_p.z = (tmp);
				}
				//
				m_PrevPos = Pos;
				//
				Easing(&m_WalkSwing, m_WalkSwing_p, 0.5f, EasingType::OutExpo);
			}
		};
		class EyeSwingControl {
		private:
			float												m_MoveEyePosTimer{ 0.f };
			Vector3DX											m_MoveEyePos;
		public://�Q�b�^�[
			const auto& GetEyeSwingPos(void) const noexcept { return this->m_MoveEyePos; }
		public:
			EyeSwingControl(void) noexcept {}
			~EyeSwingControl(void) noexcept {}
		public:
			void Init(void) noexcept {
				this->m_MoveEyePosTimer = 0.f;
			}
			void Update(const Matrix3x3DX& pCharaMat, float SwingPer, float SwingSpeed)noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (SwingPer > 0.f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(SwingSpeed) * 60.f * DXLib_refParts->GetDeltaTime();
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = Matrix3x3DX::Vtrans(Vector3DX::up() * (0.25f * SwingPer), Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
				EyePos.y = (-std::abs(EyePos.y));
				Easing(&this->m_MoveEyePos, Matrix3x3DX::Vtrans(EyePos, pCharaMat), 0.9f, EasingType::OutExpo);
			}
		};
		//
		class GunPtrControl {
		private:
			struct GunParam {
			private:
				Matrix3x3DX							m_SlingRot{};
				Vector3DX							m_SlingPos{};
			public:
				std::shared_ptr<GunClass>			m_Gun_Ptr{ nullptr };
				float								m_SlingPer{};
			public:
				void				SetGunMountMat(const Matrix3x3DX& Rot, const Vector3DX& Pos) noexcept {
					m_SlingRot = Rot;
					m_SlingPos = Pos;
				}
				const Matrix3x3DX	GetGunRotMat(const Matrix3x3DX& AimGunRot) const noexcept { return Lerp(m_SlingRot, AimGunRot, m_SlingPer); }
				const Vector3DX		GetGunPos(const Vector3DX& AimGunPos) const noexcept { return Lerp(m_SlingPos, AimGunPos, m_SlingPer); }
				void				Update(bool IsSelGun) noexcept {
					Easing(&m_SlingPer, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
					if (m_SlingPer <= 0.001f) { m_SlingPer = 0.f; }
					if (m_SlingPer >= 0.999f) { m_SlingPer = 1.f; }
				}
				void				Dispose() {
					if (!m_Gun_Ptr) { return; }
					auto* ObjMngr = ObjectManager::Instance();
					ObjMngr->DelObj((SharedObj*)&m_Gun_Ptr);
					m_Gun_Ptr.reset();
				}
			};
		private:
			int													m_GunSelect{ 0 };
			int													m_ReserveGunSelect{ 0 };
			std::array<GunParam, 3>								m_GunParam{};			//�e
		public://�Q�b�^�[
			auto&				GetParam(int ID) noexcept { return this->m_GunParam[ID]; }
			const auto&			GetParam(int ID) const noexcept { return this->m_GunParam[ID]; }
			const auto			GetGunNum(void) const noexcept { return static_cast<int>(sizeof(m_GunParam) / sizeof(m_GunParam[0])); }
			const auto			GetNowGunSelect(void) const noexcept { return this->m_GunSelect; }
		public://�Z�b�^�[
			void				GunChangeStart(int Gunselect) noexcept {
				if (GetParam(GetNowGunSelect()).m_Gun_Ptr->GetGunAnime() != GunAnimeID::LowReady) {
					GetParam(GetNowGunSelect()).m_Gun_Ptr->SetGunAnime(GunAnimeID::LowReady);
					SelectReserveGun(Gunselect);
				}
			}
			void				GunChangeNext(bool IsUpDown) noexcept {
				int Next = GetNowGunSelect();
				while (true) {
					Next = (Next + IsUpDown ? 1 : -1);
					if (Next >= GetGunNum()) {
						Next -= GetGunNum();
					}
					if (Next < 0) {
						Next += GetGunNum();
					}
					if (GetParam(Next).m_Gun_Ptr) {
						GunChangeStart(Next);
						break;
					}
				}
			}
			//��������ł͂Ȃ��ŏ��̕���ɐ؂�ւ�
			void				GunChangeThrowWeapon(bool isThrow) noexcept {
				for (int loop = 0; loop < GetGunNum(); ++loop) {
					auto& p = GetParam(loop).m_Gun_Ptr;
					if (!p) { continue; }

					if (isThrow ? p->IsThrowWeapon() : !p->IsThrowWeapon()) {
						GunChangeStart(loop);
						break;
					}
				}
			}

			void				SelectReserveGun(int ID) noexcept { m_ReserveGunSelect = ID; }
			const auto			IsChangeGunSel(void) const noexcept { return GetNowGunSelect() != m_ReserveGunSelect; }
			void				InvokeReserveGunSel(void) noexcept { SelectGun(m_ReserveGunSelect, false); }
			void				SelectGun(int ID, bool IsForce) noexcept {
				this->m_GunSelect = ID;
				this->m_ReserveGunSelect = ID;
				if (IsForce) {
					for (int loop = 0; loop < GetGunNum(); ++loop) {
						this->m_GunParam[loop].m_SlingPer = (GetNowGunSelect() == loop) ? 0.f : 1.f;
					}
				}
				GetParam(GetNowGunSelect()).m_Gun_Ptr->SetGunAnime(GunAnimeID::None);
			}
			void				SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_GunParam[ID].m_Gun_Ptr = pGunPtr0; }
		public:
			GunPtrControl(void) noexcept {}
			~GunPtrControl(void) noexcept {}
		public:
			void				Dispose(void) noexcept {
				for (auto& g : m_GunParam) {
					g.Dispose();
				}
			}
		};
		//
		class AutoAimControl {
		private:
			int													m_AutoAim{ -1 };
			int													m_AutoAimPoint{ -1 };
			float												m_AutoAimTimer{ 0.f };
			Vector3DX											m_AutoAimVec{};
			bool												m_AutoAimActive{};
			float												m_AutoAimPer{ 0.f };
		public:
			AutoAimControl(void) noexcept {}
			~AutoAimControl(void) noexcept {}
		public://�Q�b�^�[
			const auto& GetAutoAimID(void) const noexcept { return this->m_AutoAim; }
			const auto		GetAutoAimActive(void) const noexcept { return this->m_AutoAimActive; }
		public:
			void CalcAutoAimMat(Matrix3x3DX* ptmp_gunmat) const noexcept {
				*ptmp_gunmat = Lerp(*ptmp_gunmat, (*ptmp_gunmat) * Matrix3x3DX::RotVec2(ptmp_gunmat->zvec() * -1.f, m_AutoAimVec), m_AutoAimPer);
			}
		public:
			void Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept;
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
			const auto GetHitReactionMat(void) const noexcept { return Matrix3x3DX::RotAxis(m_HitAxis, m_HitPowerR * deg2rad(90.f)); }
			const auto IsDamaging(void) const noexcept { return m_HitPower > 0.f; }
		public:
			void SetHit(const Vector3DX& Axis) noexcept {
				m_HitAxis = Axis;
				m_HitPower = 1.f;
			}
			void Update(void) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				Easing(&this->m_HitPowerR, this->m_HitPower, 0.8f, EasingType::OutExpo);
				this->m_HitPower = std::max(this->m_HitPower - DXLib_refParts->GetDeltaTime() / 0.3f, 0.f);
			}
		};
		//���O�h�[��
		class RagDollControl {
		private:
			//�̂̃t���[�����
			class frame_body {
			public:
				//��
				frames head_f;
				//����
				frames bodyg_f;
				frames bodyc_f;
				frames bodyb_f;
				frames body_f;
				//�E����W�n
				frames RIGHThand2_f;
				frames RIGHThand_f;
				frames RIGHTarm2_f;
				frames RIGHTarm1_f;
				//������W�n
				frames LEFThand2_f;
				frames LEFThand_f;
				frames LEFTarm2_f;
				frames LEFTarm1_f;

				//�E����W�n
				frames RIGHTreg2_f;
				frames RIGHTreg_f;
				frames RIGHTfoot2_f;
				frames RIGHTfoot1_f;
				//������W�n
				frames LEFTreg2_f;
				frames LEFTreg_f;
				frames LEFTfoot2_f;
				frames LEFTfoot1_f;
			public:
				//
				void Get_frame(MV1& obj_) noexcept {
					for (int i = 0; i < int(obj_.GetFrameNum()); ++i) {
						std::string p = obj_.GetFrameName(i);
						if (p == std::string("�O���[�u")) {
							this->bodyg_f.Set(i, obj_);
						}
						else if (p == std::string("�����g")) {
							this->bodyc_f.Set(i, obj_);
						}

						else if (p.find("����") != std::string::npos && p.find("��") == std::string::npos && p.find("��") == std::string::npos && p.find("�h�j") == std::string::npos) {
							this->LEFTfoot1_f.Set(i, obj_);
						}
						else if (p.find("���Ђ�") != std::string::npos) {
							this->LEFTfoot2_f.Set(i, obj_);
						}
						else if (p.find("������") != std::string::npos && p.find("��") == std::string::npos) {
							this->LEFTreg_f.Set(i, obj_);
						}
						else if (p.find("�������") != std::string::npos) {
							this->LEFTreg2_f.Set(i, obj_);
						}

						else if (p.find("�E��") != std::string::npos && p.find("��") == std::string::npos && p.find("��") == std::string::npos && p.find("�h�j") == std::string::npos) {
							this->RIGHTfoot1_f.Set(i, obj_);
						}
						else if (p.find("�E�Ђ�") != std::string::npos) {
							this->RIGHTfoot2_f.Set(i, obj_);
						}
						else if (p.find("�E����") != std::string::npos && p.find("��") == std::string::npos) {
							this->RIGHTreg_f.Set(i, obj_);
						}
						else if (p.find("�E�����") != std::string::npos) {
							this->RIGHTreg2_f.Set(i, obj_);
						}
						else if (p.find("�㔼�g") != std::string::npos && p.find("�㔼�g2") == std::string::npos) {
							this->bodyb_f.Set(i, obj_);
						}
						else if (p.find("�㔼�g2") != std::string::npos) {
							this->body_f.Set(i, obj_);
						}
						else if (p.find("��") != std::string::npos && p.find("��") == std::string::npos) {
							this->head_f.Set(i, obj_);
							//head_hight = obj_.frame(this->head_f.first).y;
						}

						else if (p.find("�E�r") != std::string::npos && p.find("��") == std::string::npos) {
							this->RIGHTarm1_f.Set(i, obj_);
						}
						else if (p.find("�E�Ђ�") != std::string::npos) {
							this->RIGHTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("�E���")) {
							this->RIGHThand_f.Set(i, obj_);
						}
						else if (p == std::string("�E���") || p == std::string("�E����")) {
							this->RIGHThand2_f.Set(i, obj_);
						}

						else if (p.find("���r") != std::string::npos && p.find("��") == std::string::npos) {
							this->LEFTarm1_f.Set(i, obj_);
						}
						else if (p.find("���Ђ�") != std::string::npos) {
							this->LEFTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("�����")) {
							this->LEFThand_f.Set(i, obj_);
						}
						else if (p == std::string("�����") || p == std::string("������")) {
							this->LEFThand2_f.Set(i, obj_);
						}
					}
				}
				//
				void copy_frame(MV1& mine, frame_body& frame_tgt_, MV1* tgt) noexcept {
					tgt->SetMatrix(mine.GetMatrix());
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.head_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.head_f.GetFrameID()));
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyc_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyb_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.body_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.body_f.GetFrameID()));
					//�E����W�n
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID()));
					//������W�n
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID()));
					//�E����W�n
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID()));
					//������W�n
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.GetFrameID()));
					for (int i = 0; i < tgt->GetAnimNum(); ++i) {
						tgt->SetAnim(i).SetPer(mine.SetAnim(i).GetPer());
						tgt->SetAnim(i).SetTime(mine.SetAnim(i).GetTime());
					}
				}
				//
			};
		private:
			MV1													m_RagDoll;
			float												m_RagDollTimer{ 0.f };						//���O�h�[���̕������Z�t���O
		public:
			//�̂̃t���[�����
			frame_body lagframe_;							//�t���[��
			frame_body frame_s;								//�t���[��
		public:
			RagDollControl(void) noexcept {}
			~RagDollControl(void) noexcept {}
		public:
			auto& GetRagDoll(void) noexcept { return this->m_RagDoll; }
		public:
			void Init(MV1& obj_body_t) noexcept {
				//�g��
				this->frame_s.Get_frame(obj_body_t);
				//���O�h�[��
				this->lagframe_.Get_frame(m_RagDoll);
			}
			void Update(MV1& obj_body_t, bool isAlive) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (isAlive) {
					this->m_RagDollTimer = 0.f;
				}
				else {
					this->m_RagDollTimer = std::min(this->m_RagDollTimer + DXLib_refParts->GetDeltaTime(), 3.f);
				}
				if (this->m_RagDollTimer < 3.f) {
					this->m_RagDoll.SetPrioritizePhysicsOverAnimFlag(true);
					this->frame_s.copy_frame(obj_body_t, this->lagframe_, &m_RagDoll);
					this->m_RagDoll.UpdateAnimAll();
					if (this->m_RagDollTimer == 0.f) {
						this->m_RagDoll.PhysicsResetState();
					}
					else {
						this->m_RagDoll.PhysicsCalculation(1000.f * DXLib_refParts->GetDeltaTime());
					}
				}
			}
		};
	};
};
