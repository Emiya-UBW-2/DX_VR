#pragma once
#include	"../../Header.hpp"

#include	"../../sub.hpp"
#include	"../../MainScene/NetWork.hpp"
#include	"CharaAnimData.hpp"
#include	"CharacterEnum.hpp"

#include	"Gun.hpp"

namespace FPS_n2 {
	namespace Charas {
		//キャラのうち特定機能だけ抜き出したもの
		//
		enum class HitType {
			Head,
			Body,
			Arm,
			Leg,
			//ヒール限定
			Armor,
			Helmet,
		};
		//
		class HitBox {
			Vector3DX	m_pos;
			float		m_radius{ 0.0f };
			HitType		m_HitType{ HitType::Body };
		public:
			const auto&		GetColType(void) const noexcept { return this->m_HitType; }
			const auto&		GetPos(void) const noexcept { return this->m_pos; }
		public:
			void	Update(const Vector3DX& pos, float radius, HitType pHitType) {
				this->m_pos = pos;
				this->m_radius = radius;
				this->m_HitType = pHitType;
			}
#if defined(DEBUG) && DRAW_HITBOX
			void	Draw(void) const noexcept {
				unsigned int color{};
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
#endif
			bool	Colcheck(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				VECTOR pos1 = StartPos.get();
				VECTOR pos2 = pEndPos->get();
				VECTOR posA = this->m_pos.get();
				SEGMENT_POINT_RESULT Res;
				Segment_Point_Analyse(&pos1, &pos2, &posA, &Res);
				if (Res.Seg_Point_MinDist_Square <= this->m_radius * this->m_radius) {
					*pEndPos = Res.Seg_MinDist_Pos;
					return true;
				}
				return false;
			}
		};
		//ヒットポイントなどのパラメーター
		template<class Point, int MaxPoint>
		class PointControl {
		private://パラメーター
			const Point											Max = MaxPoint;
		private:
			Point												m_Point{ 0 };
		public://ゲッター
			const auto		IsNotZero(void) const noexcept { return this->m_Point != 0; }
			const auto&		GetPoint(void) const noexcept { return this->m_Point; }
			const auto&		GetMax(void) const noexcept { return Max; }
			void			Sub(Point point) noexcept { this->m_Point = std::clamp<Point>(this->m_Point - point, 0, Max); }
		public:
			void			Init(void) noexcept { Sub(-Max); }
		};
		//キャラ入力
		class LeanControl {
		private:
			float												m_Rad{ 0.0f };
			int													m_Rate{ 0 };
			float												m_HeadShotPer{ 0.f };
		public://ゲッター
			const auto& GetRad(void) const noexcept { return this->m_Rad; }
			const auto& GetRate(void) const noexcept { return this->m_Rate; }
		public:
			void			SetHeadShot(void) noexcept { m_HeadShotPer = 1.f; }
		public:
			void			Init(void) noexcept {
				this->m_Rad = 0.0f;
				this->m_Rate = 0;
			}
			bool			Update(bool LeftTrigger, bool RightTrigger) {
				//入力
				auto Prev = this->m_Rate;
				//*
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
				//*/
				/*
				this->m_Rate = 0;
				if (LeftTrigger) {
					this->m_Rate = -1;
				}
				if (RightTrigger) {
					this->m_Rate = 1;
				}
				//*/
				Easing(&this->m_Rad, (static_cast<float>(this->m_Rate) + this->m_HeadShotPer) * deg2rad(25), 0.9f, EasingType::OutExpo);
				Easing(&this->m_HeadShotPer, 0.f, 0.9f, EasingType::OutExpo);
				return (Prev != this->m_Rate);
			}
		};
		class MoveControl {
		private:
			float												m_VecPower{ 0.0f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
		public://ゲッター
			const auto& GetVecTotal(void) const noexcept { return this->m_VecTotal; }
			const auto GetVecMove(void) const noexcept {
				if (this->m_VecPower > 0.0f) {
					return this->m_VecTotal.normalized() * std::clamp(this->m_VecPower, 0.0f, 1.0f);
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
				for (int loop = 0; loop < 4; ++loop) {
					this->m_Vec[loop] = 0.0f;
				}
			}
			void			Update(bool WKey, bool AKey, bool SKey, bool DKey) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (WKey ? 3.0f : -7.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 1.0f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (AKey ? 3.0f : -7.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 1.0f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (SKey ? 3.0f : -7.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 1.0f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (DKey ? 3.0f : -7.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 1.0f);
				this->m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				this->m_VecPower = this->m_VecTotal.magnitude();
			}
		};
		class RotateControl {
		private:
			Vector3DX											m_rad_Buf, m_rad;
			float												m_yrad_Upper{ 0.0f }, m_yrad_Bottom{ 0.0f };
			bool												m_TurnBody{ false };
		public://セッター
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
			void			SetRad(const Vector3DX& Rotate) noexcept {
				this->m_rad_Buf = Rotate;
			}
			void Update(float pxRad, float pyRad, bool IsMove, float pyRadFront) {
				if (!IsMove) {
					float Change = abs(GetRad().y - this->m_yrad_Upper);
					if (deg2rad(50.0f) < Change) {
						this->m_TurnBody = true;
					}
					if (Change < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}


				this->m_rad_Buf.x = std::clamp(this->m_rad_Buf.x + pxRad, deg2rad(-70.0f), deg2rad(35.0f));
				this->m_rad_Buf.y = this->m_rad_Buf.y + pyRad;
				Easing(&this->m_rad.x, this->m_rad_Buf.x, 0.5f, EasingType::OutExpo);
				Easing(&this->m_rad.y, this->m_rad_Buf.y, 0.8f, EasingType::OutExpo);
				Easing(&this->m_rad.z, this->m_rad_Buf.z, 0.5f, EasingType::OutExpo);



				if (this->m_TurnBody || IsMove) { Easing(&this->m_yrad_Upper, GetRad().y, 0.85f, EasingType::OutExpo); }
				auto PrevYradBottom = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - pyRadFront, 0.85f, EasingType::OutExpo);
				auto YradChange = this->m_yrad_Bottom - PrevYradBottom;
				Easing(&this->m_rad_Buf.z, (abs(YradChange) > deg2rad(10)) ? 0.0f : std::clamp(YradChange * 3.0f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
			}
		};
		//
		class HitBoxControl {
		private:
			std::vector<HitBox>									m_HitBox;
		public:
			const HitBox* GetLineHit(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				for (auto& hitbox : this->m_HitBox) {
					if (hitbox.Colcheck(StartPos, pEndPos)) {
						return &hitbox;
					}
				}
				return nullptr;
			}
		public:
			void		CheckLineHitNearest(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				for (auto& hitbox : this->m_HitBox) {
					hitbox.Colcheck(StartPos, pEndPos);
				}
			}
			const auto& GetHitBoxPointList() const { return this->m_HitBox; }
		public:
			HitBoxControl(void) noexcept {}
			virtual ~HitBoxControl(void) noexcept {}
		public:
			void Init(void) noexcept {
				this->m_HitBox.resize(27);
			}
			void Update(const BaseObject* ptr, float SizeRate) noexcept;
#if defined(DEBUG) && DRAW_HITBOX
			void Draw(void) noexcept {
				SetUseLighting(false);
				//SetUseZBuffer3D(false);

				for (auto& hitbox : this->m_HitBox) {
					hitbox.Draw();
				}

				//SetUseZBuffer3D(true);
				SetUseLighting(true);
			}
#endif
		};
		//歩く時の揺れ
		class WalkSwingControl {
		private:
			Vector3DX											m_WalkSwing{};
			Vector3DX											m_Buffer{};
			Vector3DX											m_Base{};
			float												m_PrevY{};
			float												m_MoveEyePosTimer{ 0.0f };
			Vector3DX											m_MoveEyePosLocal{};
			Vector3DX											m_MoveEyePos{};
			float												m_BreathEyePosTimer{ 0.0f };
		public:
			WalkSwingControl(void) noexcept {}
			virtual ~WalkSwingControl(void) noexcept {}
		public:
			const auto& GetMoveEyePos(void) const noexcept { return this->m_MoveEyePosLocal; }
		public:
			const auto CalcWalk(const Vector3DX& Pos, float SwingPer)noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				//X
				{
					if (this->m_PrevY > Pos.y) {
						this->m_Base.x = 1.0f;
					}
					else {
						this->m_Base.x = std::max(this->m_Base.x - 15.0f * DXLib_refParts->GetDeltaTime(), 0.0f);
					}
					this->m_PrevY = Pos.y;
				}
				//Z
				{
					if (this->m_Base.x == 1.0f) {
						if (this->m_Base.z >= 0.0f) {
							this->m_Base.z = -1.0f;
						}
						else {
							this->m_Base.z = 1.0f;
						}
					}
				}
				Easing(&this->m_Buffer.x, this->m_Base.x * SwingPer, (this->m_Buffer.x > this->m_Base.x * SwingPer) ? 0.6f : 0.9f, EasingType::OutExpo);
				Easing(&this->m_Buffer.z, this->m_Base.z * SwingPer, 0.95f, EasingType::OutExpo);
				Easing(&this->m_WalkSwing, this->m_Buffer, 0.5f, EasingType::OutExpo);
				Vector3DX WalkSwingRad = Vector3DX::vget(5.0f, 0.0f, 10.0f);
				return Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(this->m_WalkSwing.z * WalkSwingRad.z)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(this->m_WalkSwing.x * WalkSwingRad.x));
			}
			const auto& CalcEye(const Matrix3x3DX& pCharaMat, float SwingPer, float SwingSpeed) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (SwingPer > 0.0f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(SwingSpeed) * 60.0f * DXLib_refParts->GetDeltaTime();
				}
				else {
					this->m_MoveEyePosTimer = 0.0f;
				}
				auto EyePos = Matrix3x3DX::Vtrans(Vector3DX::up() * (0.25f * SwingPer), Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
				EyePos.y = -std::abs(EyePos.y);

				this->m_BreathEyePosTimer += DXLib_refParts->GetDeltaTime() * 60 / 120;

				Vector3DX EyePosBreath = Vector3DX::up() * (std::sin(this->m_BreathEyePosTimer * deg2rad(360)) * 0.005f * Scale3DRate);

				Easing(&this->m_MoveEyePosLocal, EyePos + EyePosBreath, 0.9f, EasingType::OutExpo);
				Easing(&this->m_MoveEyePos, Matrix3x3DX::Vtrans(EyePos + EyePosBreath, pCharaMat), 0.9f, EasingType::OutExpo);
				return this->m_MoveEyePos;
			}
		};
		//
		class GunPtrControl {
		private:
		private:
			int													m_GunSelect{ 0 };
			int													m_ReserveGunSelect{ 0 };
			std::array<std::shared_ptr<Guns::GunObj>, 3>		m_GunPtr{};			//銃
		public://ゲッター
			const auto			GetGunNum(void) const noexcept { return static_cast<int>(this->m_GunPtr.size()); }
			std::shared_ptr<Guns::GunObj> GetGunPtr(int ID) noexcept {
				if (0 <= ID && ID < GetGunNum()) {
					return this->m_GunPtr[ID];
				}
				return nullptr;
			}
			const std::shared_ptr<Guns::GunObj> GetGunPtr(int ID) const noexcept {
				if (0 <= ID && ID < GetGunNum()) {
					return this->m_GunPtr[ID];
				}
				return nullptr;
			}
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
						this->m_ReserveGunSelect = Next;
						break;
					}
				}
			}
			//投げ武器ではない最初の武器に切り替え
			void				GunChangeThrowWeapon(bool isThrow) noexcept {
				for (int loop = 0, max = GetGunNum(); loop < max; ++loop) {
					auto pGun = GetGunPtr(loop);
					if (!pGun) { continue; }

					if (isThrow ? pGun->GetModifySlot()->GetMyData()->GetIsThrowWeapon() : !pGun->GetModifySlot()->GetMyData()->GetIsThrowWeapon()) {
						this->m_ReserveGunSelect = loop;
						break;
					}
				}
			}
			//武器を外す
			void				SetOnOff(bool IsOn) noexcept {
				if (this->m_GunSelect != this->m_ReserveGunSelect) { return; }
				if (!IsOn) {
					this->m_ReserveGunSelect = InvalidID;
				}
				else {
					this->m_ReserveGunSelect = 0;
				}
			}

			const auto			IsChangeGunSelect(void) const noexcept { return GetNowGunSelect() != this->m_ReserveGunSelect; }
			void				InvokeReserveGunSelect(void) noexcept { this->m_GunSelect = this->m_ReserveGunSelect; }
			void				SelectGun(int ID) noexcept { this->m_GunSelect = this->m_ReserveGunSelect = ID; }
			void				SetGunPtr(int ID, const std::shared_ptr<Guns::GunObj>& pGunPtr0) noexcept { this->m_GunPtr[ID] = pGunPtr0; }
		public:
			GunPtrControl(void) noexcept {}
			virtual ~GunPtrControl(void) noexcept {}
		public:
			void				Dispose(void) noexcept {
				for (auto& gun : this->m_GunPtr) {
					if (!gun) { return; }
					ObjectManager::Instance()->DelObj(gun);
					gun.reset();
				}
			}
		};
		//
		class HitReactionControl {
		private:
			Vector3DX											m_HitAxis{ Vector3DX::forward() };
			float												m_HitPower{ 0.0f };
			float												m_HitPowerR{ 0.0f };
		private:
		public:
			HitReactionControl(void) noexcept {}
			virtual ~HitReactionControl(void) noexcept {}
		public:
			const auto GetHitReactionMat(void) const noexcept { return Matrix3x3DX::RotAxis(this->m_HitAxis, this->m_HitPowerR * deg2rad(90.0f)); }
			const auto IsDamaging(void) const noexcept { return this->m_HitPower > 0.0f; }
		public:
			void SetHit(const Vector3DX& Axis) noexcept {
				this->m_HitAxis = Axis;
				this->m_HitPower = 1.0f;
			}
			void Update(void) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				Easing(&this->m_HitPowerR, this->m_HitPower, 0.8f, EasingType::OutExpo);
				this->m_HitPower = std::max(this->m_HitPower - DXLib_refParts->GetDeltaTime() / 0.3f, 0.0f);
			}
		};
		//ラグドール
		enum class RagFrame {
			//頭
			head,
			LeftEye,
			RightEye,
			//胴体
			center,
			bodyg,
			bodyc,
			bodyb,
			body,
			//右手座標系
			RIGHThand2,
			RIGHThand,
			RIGHTarm2,
			RIGHTarm1,
			RIGHTShoulder,
			//左手座標系
			LEFThand2,
			LEFThand,
			LEFTarm2,
			LEFTarm1,
			LEFTShoulder,

			//右手座標系
			RIGHTreg2,
			RIGHTreg,
			RIGHTfoot2,
			RIGHTfoot1,
			//左手座標系
			LEFTreg2,
			LEFTreg,
			LEFTfoot2,
			LEFTfoot1,

			Max,
		};
		static const char* RagFrameName[] = {
			"頭",
			"左目",
			"右目",

			"センター",
			"グルーブ",
			"下半身",
			"上半身",
			"上半身2",
			"右手首先",
			"右手首",
			"右ひじ",
			"右腕",
			"右肩",

			"左手首先",
			"左手首",
			"左ひじ",
			"左腕",
			"左肩",

			"右足首先",
			"右足首",
			"右ひざ",
			"右足",
			"左足首先",
			"左足首",
			"左ひざ",
			"左足",
		};
		class RagDollControl {
		private:
			//体のフレーム情報
			class frame_body {
				std::array<frames, static_cast<int>(sizeof(RagFrameName)/sizeof(RagFrameName[0]))>	m_Frames;
			public:
				const auto& GetFrame(RagFrame select) const noexcept { return this->m_Frames[static_cast<int>(select)]; }
				//
				void SetupFrameInfo(const MV1& obj_) noexcept {
					for (int loop = 0, max = int(obj_.GetFrameNum()); loop < max; ++loop) {
						std::string FrameName = obj_.GetFrameName(loop);
						for (auto& frame : this->m_Frames) {
							int index = static_cast<int>(&frame - &this->m_Frames.front());
							if (FrameName == RagFrameName[index]) {
								frame.Set(loop, obj_);
								break;
							}
						}
					}
				}
				//
				void CopyFrame(const MV1& mine, const frame_body& targetFrame, MV1* tgt, const Vector3DX& groundPos) noexcept {
					tgt->SetMatrix(mine.GetMatrix().rotation() * Matrix4x4DX::Mtrans(groundPos));
					for (const auto& frame : targetFrame.m_Frames) {
						tgt->SetFrameLocalMatrix(frame.GetFrameID(), mine.GetFrameLocalMatrix(frame.GetFrameID()));
					}
					{
						int Frame = 2;
						tgt->SetFrameLocalMatrix(Frame, mine.GetFrameLocalMatrix(Frame) * Matrix4x4DX::Mtrans(Vector3DX::vget(0.0f, (mine.GetMatrix().pos() - groundPos).y, 0.0f)));
					}
					for (int loop = 0, max = static_cast<int>(tgt->GetAnimNum()); loop < max; ++loop) {
						tgt->SetAnim(loop).SetPer(mine.GetAnim(loop).GetPer());
						tgt->SetAnim(loop).SetTime(mine.GetAnim(loop).GetTime());
					}
					tgt->UpdateAnimAll();
				}
				//
			};
		private:
			const MV1*											m_pBaseObj{nullptr};
			MV1													m_RagDoll;
			float												m_Timer{ 0.0f };						//ラグドールの物理演算タイマー
			frame_body											m_RagObjFrame;						//フレーム
			frame_body											m_BaseObjFrame;						//フレーム
			float												m_PhysicsTime{ 0.0f };				//ラグドールの物理演算タイマー
		public:
		public:
			RagDollControl(void) noexcept {}
			virtual ~RagDollControl(void) noexcept {}
		public:
			auto& SetRagDoll(void) noexcept { return this->m_RagDoll; }
			const auto& GetRagDoll(void) const noexcept { return this->m_RagDoll; }

			const auto GetFrameMat(RagFrame select) const noexcept {
				auto& frame = this->m_RagObjFrame.GetFrame(select);
				return frame.GetFrameWorldPosition().rotation().inverse() * this->m_RagDoll.GetFrameLocalWorldMatrix(frame.GetFrameID());
			}

		public:
			void Init(const MV1& baseObj) noexcept {
				this->m_pBaseObj = &baseObj;
				if (!this->m_pBaseObj) { return; }
				this->m_BaseObjFrame.SetupFrameInfo(*this->m_pBaseObj);				//身体
				this->m_RagObjFrame.SetupFrameInfo(this->m_RagDoll);			//ラグドール
			}
			void Update(bool isAlive, const Vector3DX& groundPos) noexcept {
				if (!this->m_pBaseObj) { return; }
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (isAlive) {
					this->m_Timer = 0.0f;
				}
				else {
					if (this->m_Timer < 3.0f) {
						this->m_RagDoll.SetPrioritizePhysicsOverAnimFlag(true);
						this->m_BaseObjFrame.CopyFrame(*this->m_pBaseObj, this->m_RagObjFrame, &this->m_RagDoll, groundPos);
						//物理演算
						if (this->m_Timer == 0.0f) {
							this->m_RagDoll.PhysicsResetState();
							this->m_PhysicsTime = 0.0f;
						}
						this->m_PhysicsTime += DXLib_refParts->GetDeltaTime();

						bool isCalc = false;
						while (true) {
							if (this->m_PhysicsTime >= 1.0f / 60.0f) {
								this->m_PhysicsTime -= 1.0f / 60.0f;
								this->m_RagDoll.PhysicsCalculation(1000.0f / 60.0f);
								isCalc = true;
							}
							else {
								break;
							}
						}
						if(!isCalc) {
							this->m_RagDoll.PhysicsCalculation(0.0f);
						}
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
	}
}
