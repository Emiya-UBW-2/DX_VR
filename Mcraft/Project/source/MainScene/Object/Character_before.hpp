#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../MainScene/NetWork.hpp"
#include "CharaAnimData.hpp"
#include "CharacterEnum.hpp"

#include "Gun.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//キャラのうち特定機能だけ抜き出したもの
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
		//ヒットポイントなどのパラメーター
		template<class Point, int MaxPoint>
		class PointControl {
		private://パラメーター
			const Point											Max = MaxPoint;
		private:
			Point												m_HP{ 0 };							//スコア
		public://ゲッター
			const auto		IsNotZero(void) const noexcept { return this->m_HP != 0; }
			const auto& GetPoint(void) const noexcept { return this->m_HP; }
			const auto& GetMax(void) const noexcept { return Max; }
			void			Sub(Point damage_t) noexcept { this->m_HP = std::clamp<Point>(this->m_HP - damage_t, 0, Max); }
		public:
			void		Init(void) noexcept { Sub(-Max); }
		};
		//キャラ入力
		class LeanControl {
		private://キャラパラメーター
			switchs												m_QKey;
			switchs												m_EKey;
			float												m_Rad{ 0.f };
			int													m_Rate{ 0 };
			bool												m_Switch{ false };
		public://ゲッター
			const auto& GetRad(void) const noexcept { return this->m_Rad; }
			const auto& GetRate(void) const noexcept { return this->m_Rate; }
			const auto& GetSwitch(void) const noexcept { return this->m_Switch; }
		public:
			void			Init(void) noexcept {
				this->m_Rad = 0.f;
			}
			void			Update(bool LeftPress, bool RightPress) {
				//入力
				this->m_QKey.Update(LeftPress);
				this->m_EKey.Update(RightPress);
				auto Prev = this->m_Rate;
				if (true) {//トグル式
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
		private://キャラパラメーター
			float												m_VecPower{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
		public://ゲッター
			const auto& GetVecTotal(void) const noexcept { return this->m_VecTotal; }
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
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (WKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (AKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (SKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (DKey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_VecPower = m_VecTotal.magnitude();
			}
		};
		class RotateControl {
		private://キャラパラメーター
			Vector3DX											m_rad_Buf, m_rad;
			Vector2DX											m_radAdd;
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_UpperChange{ 0.f };
			Vector3DX											m_UpperPrevRad;
			Vector3DX											m_UpperRad;
			Vector3DX											m_UpperPos;
			bool												m_TurnBody{ false };
			Matrix3x3DX											m_GunSwingMat, m_GunSwingMat2;
		public://セッター
			const auto		IsTurnBody(void) const noexcept { return this->m_TurnBody; }
			const auto		GetRad(void) const noexcept { return this->m_rad; }
			const auto		GetYRadUpper(void) const noexcept { return this->m_yrad_Upper; }
			const auto		GetYRadBottom(void) const noexcept { return this->m_yrad_Bottom; }
			const auto		GetYRadBottomChange(void) const noexcept { return GetRad().y - this->m_yrad_Bottom; }
			const auto		GetGunSwingMat(void) const noexcept { return m_GunSwingMat2; }
		public:
			void			Init(float pxRad, float pyRad) {
				this->m_radAdd.Set(0, 0);
				this->m_rad_Buf.x = (pxRad);
				this->m_rad_Buf.y = (pyRad);
				this->m_rad = this->m_rad_Buf;
				this->m_yrad_Upper = GetRad().y;
				this->m_yrad_Bottom = GetRad().y;
				this->m_TurnBody = false;
			}
			void Update(float pxRad, float pyRad, const Vector2DX& pAddRadvec, bool IsMove, float pyRadFront) {
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


				Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);
				this->m_rad_Buf.x = std::clamp(this->m_rad_Buf.x + pxRad, deg2rad(-70.f), deg2rad(24.f)) + this->m_radAdd.x;
				this->m_rad_Buf.y = this->m_rad_Buf.y + pyRad + this->m_radAdd.y;
				Easing(&this->m_rad.x, this->m_rad_Buf.x, 0.5f, EasingType::OutExpo);
				Easing(&this->m_rad.y, this->m_rad_Buf.y, 0.8f, EasingType::OutExpo);
				Easing(&this->m_rad.z, this->m_rad_Buf.z, 0.5f, EasingType::OutExpo);



				if (this->m_TurnBody || IsMove) { Easing(&this->m_yrad_Upper, GetRad().y, 0.85f, EasingType::OutExpo); }
				auto YradChange = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - pyRadFront, 0.85f, EasingType::OutExpo);
				YradChange = this->m_yrad_Bottom - YradChange;
				Easing(&this->m_rad_Buf.z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
				//銃の揺れ
				Easing(&m_UpperRad, (GetRad() - this->m_UpperPrevRad) * -1.f, 0.9f, EasingType::OutExpo);
				m_UpperPrevRad = GetRad();
				Easing(&m_GunSwingMat, Matrix3x3DX::RotAxis(Vector3DX::right(), m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), m_UpperRad.y), 0.8f, EasingType::OutExpo);
				Easing(&m_GunSwingMat2, m_GunSwingMat, 0.8f, EasingType::OutExpo);
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
		//歩く時の揺れ
		class WalkSwingControl {
		private:
			Vector3DX											m_WalkSwingRad;
			Vector3DX											m_WalkSwing;
			Vector3DX											m_WalkSwing_p;
			Vector3DX											m_WalkSwing_t;
			Vector3DX											m_PrevPos;
		public:
			WalkSwingControl(void) noexcept {}
			~WalkSwingControl(void) noexcept {}
		public:
			const auto Calc(const Vector3DX& Pos, float SwingPer)noexcept {
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
				return Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_WalkSwing.z * m_WalkSwingRad.z)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(m_WalkSwing.x * m_WalkSwingRad.x));
			}
		};
		class EyeSwingControl {
		private:
			float												m_MoveEyePosTimer{ 0.f };
			Vector3DX											m_MoveEyePos;
		public:
			EyeSwingControl(void) noexcept {}
			~EyeSwingControl(void) noexcept {}
		public:
			const auto& Calc(const Matrix3x3DX& pCharaMat, float SwingPer, float SwingSpeed) noexcept {
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
				return this->m_MoveEyePos;
			}
		};
		//
		class GunPtrControl {
		private:
		private:
			int													m_GunSelect{ 0 };
			int													m_ReserveGunSelect{ 0 };
			std::array<std::shared_ptr<GunClass>, 3>			m_GunPtr{};			//銃
		public://ゲッター
			auto&				GetGunPtr(int ID) noexcept { return this->m_GunPtr[ID]; }
			const auto&			GetGunPtr(int ID) const noexcept { return this->m_GunPtr[ID]; }
			const auto			GetGunNum(void) const noexcept { return static_cast<int>(m_GunPtr.size()); }
			const auto			GetNowGunSelect(void) const noexcept { return this->m_GunSelect; }
		public://セッター
			//次の武器に切り替え
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
						m_ReserveGunSelect = Next;
						break;
					}
				}
			}
			//投げ武器ではない最初の武器に切り替え
			void				GunChangeThrowWeapon(bool isThrow) noexcept {
				for (int loop = 0; loop < GetGunNum(); ++loop) {
					auto& p = GetGunPtr(loop);
					if (!p) { continue; }

					if (isThrow ? p->GetModSlot().GetModData()->GetIsThrowWeapon() : !p->GetModSlot().GetModData()->GetIsThrowWeapon()) {
						m_ReserveGunSelect = loop;
						break;
					}
				}
			}

			const auto			IsChangeGunSel(void) const noexcept { return GetNowGunSelect() != m_ReserveGunSelect; }
			void				InvokeReserveGunSel(void) noexcept { this->m_GunSelect = m_ReserveGunSelect; }
			void				SelectGun(int ID) noexcept { this->m_GunSelect = this->m_ReserveGunSelect = ID; }
			void				SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_GunPtr[ID] = pGunPtr0; }
		public:
			GunPtrControl(void) noexcept {}
			~GunPtrControl(void) noexcept {}
		public:
			void				Dispose(void) noexcept {
				for (auto& g : m_GunPtr) {
					if (!g) { return; }
					auto* ObjMngr = ObjectManager::Instance();
					ObjMngr->DelObj((SharedObj*)&g);
					g.reset();
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
		public://ゲッター
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
		//ラグドール
		class RagDollControl {
		private:
			//体のフレーム情報
			class frame_body {
			public:
				//頭
				frames head_f;
				//胴体
				frames bodyg_f;
				frames bodyc_f;
				frames bodyb_f;
				frames body_f;
				//右手座標系
				frames RIGHThand2_f;
				frames RIGHThand_f;
				frames RIGHTarm2_f;
				frames RIGHTarm1_f;
				//左手座標系
				frames LEFThand2_f;
				frames LEFThand_f;
				frames LEFTarm2_f;
				frames LEFTarm1_f;

				//右手座標系
				frames RIGHTreg2_f;
				frames RIGHTreg_f;
				frames RIGHTfoot2_f;
				frames RIGHTfoot1_f;
				//左手座標系
				frames LEFTreg2_f;
				frames LEFTreg_f;
				frames LEFTfoot2_f;
				frames LEFTfoot1_f;
			public:
				//
				void Get_frame(const MV1& obj_) noexcept {
					for (int i = 0; i < int(obj_.GetFrameNum()); ++i) {
						std::string p = obj_.GetFrameName(i);
						if (p == std::string("グルーブ")) {
							this->bodyg_f.Set(i, obj_);
						}
						else if (p == std::string("下半身")) {
							this->bodyc_f.Set(i, obj_);
						}

						else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->LEFTfoot1_f.Set(i, obj_);
						}
						else if (p.find("左ひざ") != std::string::npos) {
							this->LEFTfoot2_f.Set(i, obj_);
						}
						else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->LEFTreg_f.Set(i, obj_);
						}
						else if (p.find("左足首先") != std::string::npos) {
							this->LEFTreg2_f.Set(i, obj_);
						}

						else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->RIGHTfoot1_f.Set(i, obj_);
						}
						else if (p.find("右ひざ") != std::string::npos) {
							this->RIGHTfoot2_f.Set(i, obj_);
						}
						else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->RIGHTreg_f.Set(i, obj_);
						}
						else if (p.find("右足首先") != std::string::npos) {
							this->RIGHTreg2_f.Set(i, obj_);
						}
						else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
							this->bodyb_f.Set(i, obj_);
						}
						else if (p.find("上半身2") != std::string::npos) {
							this->body_f.Set(i, obj_);
						}
						else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
							this->head_f.Set(i, obj_);
							//head_hight = obj_.frame(this->head_f.first).y;
						}

						else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->RIGHTarm1_f.Set(i, obj_);
						}
						else if (p.find("右ひじ") != std::string::npos) {
							this->RIGHTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("右手首")) {
							this->RIGHThand_f.Set(i, obj_);
						}
						else if (p == std::string("右手先") || p == std::string("右手首先")) {
							this->RIGHThand2_f.Set(i, obj_);
						}

						else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->LEFTarm1_f.Set(i, obj_);
						}
						else if (p.find("左ひじ") != std::string::npos) {
							this->LEFTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("左手首")) {
							this->LEFThand_f.Set(i, obj_);
						}
						else if (p == std::string("左手先") || p == std::string("左手首先")) {
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
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID()));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID()));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID()));
					//左手座標系
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
			float												m_RagDollTimer{ 0.f };						//ラグドールの物理演算フラグ
		public:
			//体のフレーム情報
			frame_body lagframe_;							//フレーム
			frame_body frame_s;								//フレーム
		public:
			RagDollControl(void) noexcept {}
			~RagDollControl(void) noexcept {}
		public:
			auto& GetRagDoll(void) noexcept { return this->m_RagDoll; }
		public:
			void Init(const MV1& obj_body_t) noexcept {
				//身体
				this->frame_s.Get_frame(obj_body_t);
				//ラグドール
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
