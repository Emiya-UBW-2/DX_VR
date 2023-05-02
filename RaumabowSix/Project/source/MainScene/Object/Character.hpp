#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public ShapeControl
		{
		private://キャラパラメーター
			const float											SpeedLimit{ 1.75f };
			const float											UpperTimerLimit = 10.f;
		private:
			CharaTypeID											m_CharaType;
			std::vector<CharaAnimeSet>							m_CharaAnimeSet;
			std::vector<GunAnimeSet>							m_GunAnimeSet;
			int													m_CharaAnimeSel{ 0 };
			int													m_AimAnimeSel{ 0 };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			bool												m_PosBufOverRideFlag{ false };
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			VECTOR_ref											m_RadOverRide;									//

			MATRIX_ref											m_UpperMatrix;
			float												m_LeanRad{ 0.f };
			float												m_LateLeanRad{ 0.f };
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_RunReady{ false };
			bool												m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			//銃
			float												m_ReadyTimer{ 0.f };
			float												m_ReadyPer{ 0.f };
			float												m_ReloadPer{ 0.f };

			int													m_ShotPhase{ 0 };

			float												m_MagHansPer{ 0.f };
			float												m_MoveEyePosTimer{ 0.f };
			VECTOR_ref											m_MoveEyePos;
			bool												m_IsStuckGun{ false };
			//入力
			bool												m_Press_Shot{ false };
			bool												m_Press_Reload{ false };
			bool												m_Press_Aim{ false };
			//体力
			std::vector<HitBox>									m_HitBox;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//
			float												m_LeftHandPer{ 0.f };
			Pendulum2D											m_SlingZrad;
			Pendulum2D											m_SlingYrad;

			VECTOR_ref											m_RecoilRadAdd;
			//サウンド
			int													m_CharaSound{ -1 };
			std::array<std::shared_ptr<GunClass>,2>				m_Gun_Ptr{ nullptr };			//銃
			std::array<float, 2>								m_SlingPer;
			std::array<MATRIX_ref, 2>							m_SlingMat;
			int													m_GunSelect{ 0 };
			//
			bool												m_SendCamShake{ false };

			VECTOR_ref											m_UpperPrevRad;
			VECTOR_ref											m_UpperRad;
			VECTOR_ref											m_UpperyVec, m_UpperzVec, m_UpperPos;
			float												m_UpperAnim{ 0.f };

			float												m_AutoAimScale{ 1.f };
			bool												m_IsActiveAutoAim{ false };
			float												m_AutoAimPer{ 0.f };
			VECTOR_ref											m_AutoAimVec;
			VECTOR_ref											m_AutoAimPos;

			VECTOR_ref											LaserStartPos;
			VECTOR_ref											LaserEndPos;
			VECTOR_ref											LaserEndPos2D;

			float												m_ADSPer = 0.f;
			float												m_Fov = 0.f;
		public://ゲッター
			//const auto		GetShotAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_ADS; }
			//const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			//const auto		GetReadyPer(void) const noexcept { return this->m_ReadyPer; }
			//const auto&		GetPosBuf(void) const noexcept { return this->m_PosBuf; }
			//const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			//void				SetAnimOnce(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			//const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
		public://ゲッター(ラッパー)
			const auto		GetBottomStandAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetBottomRunAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Run; }
			const auto		GetCharaFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
		public://ゲッター
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			auto&			GetGunPtr(int ID) noexcept { return this->m_Gun_Ptr[ID]; }
			auto&			GetGunPtrNow(void) noexcept { return this->m_Gun_Ptr[this->m_GunSelect]; }
			const auto&		GetGunPtrNow_Const(void) const noexcept { return this->m_Gun_Ptr[this->m_GunSelect]; }
			const auto&		GetGunPtrNowID() const noexcept { return this->m_GunSelect; }
			const auto&		GetReticleRad(void) const noexcept { return this->m_LeanRad; }
			const auto&		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
			const auto&		GetIsActiveAutoAim(void) const noexcept { return this->m_IsActiveAutoAim; }
			const auto&		GetActiveAutoScale(void) const noexcept { return this->m_AutoAimScale; }
		public://セッター
			void			SetAutoAim(const VECTOR_ref* value = nullptr) noexcept {
				m_IsActiveAutoAim = (value) && !GetIsRun();
				if (value) {
					m_AutoAimPos = *value;
				}
			}

			void			SetDamageSwitchRec(unsigned long long value) noexcept { this->m_DamageSwitchRec = value; }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetCharaFrame(frame)); }
			void			LoadReticle(void) noexcept {
				for (auto& p : this->m_Gun_Ptr) {
					if (p != nullptr) {
						p->LoadReticle();
					}
				}
			}
			bool			SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID && this->m_objType == value.CharaType) {
					SubHP(value.Damage, value.rad);
					return true;
				}
				return false;
			}
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetCharaFrame(frame), value * this->m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, const VECTOR_ref& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}
			void			SetGunPtr(std::shared_ptr<GunClass>& pGunPtr0, std::shared_ptr<GunClass>& pGunPtr1) noexcept {
				this->m_Gun_Ptr[0] = pGunPtr0;
				this->m_SlingPer[0] = 0.f;
				this->m_Gun_Ptr[1] = pGunPtr1;
				this->m_SlingPer[1] = 1.f;
				if (this->GetGunPtr(0) != nullptr) {
					this->m_CharaAnimeSel = this->GetGunPtr(0)->GetHumanAnimType();
				}
				for (auto& p : this->m_Gun_Ptr) {
					p->SetPlayerID(this->m_MyID);
				}
				this->m_GunSelect = 0;
			}
		public://ゲッター
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(GetCharaFrame(frame)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(GetCharaFrame(frame)); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
			const auto		GetGunPtrNum() const noexcept { return this->m_Gun_Ptr.size(); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetCharaVecX(void) const noexcept { return GetCharaDir().xvec(); }
			const auto		GetCharaVecY(void) const noexcept { return GetCharaDir().yvec(); }
			const auto		GetCharaVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }
			const auto		GetLensPos(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetLensPos() : VECTOR_ref::zero(); }
			const auto		GetReticleSize(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetReticleSize() : 1.f; }
			const auto		GetReticlePos(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetReticlePos() : VECTOR_ref::zero(); }
			const auto		GetLensPosSize(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetLensPosSize() : VECTOR_ref::zero(); }
			const auto		GetCanshot(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? (GetGunPtrNow_Const()->GetCanShot() && (this->m_ShotPhase <= 1)) : false; }
			const auto		GetAmmoNum(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoNum() : 0; }
			const auto		GetAmmoAll(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoAll() : 0; }
			const auto		GetShotSwitch(void) const noexcept { return this->m_ShotPhase == 1; }
			const auto		GetDownAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Down; }
			const auto		GetAimAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Ready; }
			const auto		GetCockingAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Cocking; }
			const auto		GetReloadStartAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Reload; }
			const auto		GetReloadOneAnimSel(void) const noexcept { return  (CharaAnimeID)((int)GetReloadStartAnimSel() + 1); }
			const auto		GetReloadEndAnimSel(void) const noexcept { return  (CharaAnimeID)((int)GetReloadStartAnimSel() + 2); }

			const auto		GetRunGunAnimSel(void) const noexcept { return this->m_GunAnimeSet[this->m_CharaAnimeSel].m_Run; }
			const auto		GetReadyGunAnimSel(void) const noexcept { return this->m_GunAnimeSet[this->m_CharaAnimeSel].m_Ready; }
			const auto&		GetAimGunAnimSel(void) const noexcept { return this->m_GunAnimeSet[this->m_CharaAnimeSel].m_Aim; }
			const auto		GetADSGunAnimSel(void) const noexcept { return this->m_GunAnimeSet[this->m_CharaAnimeSel].m_ADS; }
			const auto		GetReloadGunAnimSel(void) const noexcept { return this->m_GunAnimeSet[this->m_CharaAnimeSel].m_Reload; }

			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto&		GetADSPer(void) const noexcept { return this->m_ADSPer; }

			const auto		GetEyeVecMat(void) const noexcept {
				auto tmpUpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);
				return tmpUpperMatrix * this->m_move.mat;
			}
			const auto		GetEyeVecX(void) const noexcept { return GetEyeVecMat().xvec(); }
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ this->GetEyeVecY() * 0.f
					+ this->GetEyeVector() * 0.5f
					+ this->m_MoveEyePos;
			}
			const auto		GetScopePos(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetScopePos() : GetEyePosition(); }
		private:
			//被弾チェック
			const auto		CheckAmmoHited(const AmmoClass& pAmmo) noexcept {
				bool is_Hit = false;
				for (auto& h : this->m_HitBox) {
					is_Hit |= h.Colcheck(pAmmo);
				}
				return is_Hit;
			}
			const auto		CheckLineHited(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				bool is_Hit = false;
				for (auto& h : this->m_HitBox) {
					is_Hit |= h.Colcheck(StartPos, pEndPos);
				}
				return is_Hit;
			}
			//被弾処理
			const auto		CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
				//auto SE = SoundPool::Instance();
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(*pAmmo)) {
						h.GetColType();
						pAmmo->Penetrate();	//貫通
						//ダメージ計算
						auto v1 = GetCharaVector();
						auto v2 = (pShooterPos - this->m_move.pos).Norm(); v2.y(0);
						this->m_DamageEvent.SetEvent(this->m_MyID, m_objType, pAmmo->GetDamage(), atan2f(v1.cross(v2).y(), v1.dot(v2)));
						++this->m_DamageSwitch;// %= 255;//
						//エフェクトセット
						return true;
					}
				}
				return false;
			}
		public:
			const auto&		GetFov(void) const noexcept { return this->m_Fov; }
			const auto		CalcFov() noexcept {
			float fov = 0;
			if (this->GetIsADS()) {
				fov = deg2rad(70);
			}
			else if (this->GetIsRun()) {
				fov = deg2rad(90);
			}
			else {
				fov = deg2rad(85);
			}

			if (this->GetShotSwitch()) {
				if (this->GetIsADS()) {
					fov -= deg2rad(5);
				}
				Easing(&this->m_Fov, fov, 0.5f, EasingType::OutExpo);
			}
			else {
				Easing(&this->m_Fov, fov, 0.9f, EasingType::OutExpo);
			}
		}
			const auto		CheckLineHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				if (GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate) {
					if (this->CheckLineHited(StartPos, pEndPos)) {									//とりあえず当たったかどうか探す
						return true;
					}
				}
				return false;
			}
			const auto		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
				std::pair<bool, bool> isDamaged{ false,false };
				if (GetMinLenSegmentToPoint(pAmmo->GetMove().repos, pAmmo->GetMove().pos, m_move.pos) <= 2.0f*Scale_Rate) {
					if (this->CheckAmmoHited(*pAmmo)) {									//とりあえず当たったかどうか探す
						isDamaged.first = true;
						if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
							isDamaged.second = true;
						}
					}
				}
				return isDamaged;
			}
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			//
			void			ValueSet(float pxRad, float pyRad, bool SquatOn, const VECTOR_ref& pPos, PlayerID pID) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept;
		public://レーザーサイト
			const auto&		GetLaser(void) const noexcept { return this->LaserEndPos; }
			const auto&		GetAimPoint(void) const noexcept { return this->LaserEndPos2D; }
			void			SetLaser2D(const VECTOR_ref& value) noexcept {
				this->LaserEndPos2D = value;
				if (!(0.f < this->LaserEndPos2D.z() && this->LaserEndPos2D.z() < 1.f)) {
					auto* DrawParts = DXDraw::Instance();
					this->LaserEndPos2D.Set((float)DrawParts->m_DispXSize / 2, (float)DrawParts->m_DispYSize / 2, 0.f);
				}
			}
			void			SetLaser(const std::vector<std::shared_ptr<CharacterClass>>* CharaPool) noexcept {
				LaserStartPos = this->GetGunPtrNow()->GetMuzzleMatrix().pos() + this->GetGunPtrNow()->GetMuzzleMatrix().yvec()*-0.05f*Scale_Rate;
				LaserEndPos = LaserStartPos + this->GetGunPtrNow()->GetMuzzleMatrix().zvec()*-1.f * 50.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(LaserStartPos, &LaserEndPos, true);
				for (const auto& c : *CharaPool) {
					if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					c->CheckLineHit(LaserStartPos, &LaserEndPos);
				}
				auto Vec = (LaserEndPos - LaserStartPos);
				LaserEndPos = LaserStartPos + Vec.Norm()*std::max(Vec.Length() - 0.3f*Scale_Rate, 0.f);
			}
			void			DrawLaser() noexcept {
				SetUseLighting(FALSE);
				DrawSphere_3D(LaserEndPos, 0.01f*Scale_Rate, GetColor(255, 24, 24), GetColor(0, 0, 0));
				DrawCapsule_3D(LaserStartPos, LaserEndPos, 0.0015f*Scale_Rate, GetColor(255, 24, 24), GetColor(0, 0, 0));
				SetUseLighting(TRUE);
			}
		private: //更新関連
			void			ExecuteSavePrev(void) noexcept;			//以前の状態保持														//
			void			ExecuteInput(void) noexcept;			//操作																	//0.01ms
			void			ExecuteUpperMatrix(void) noexcept;		//上半身回転															//0.06ms
			void			ExecuteAnim(void) noexcept;				//SetMat指示															//0.03ms
			void			ExecuteMatrix(void) noexcept;			//SetMat指示更新														//0.03ms
			void			ExecuteShape(void) noexcept;			//顔																	//0.01ms
			void			ExecuteHeartRate(void) noexcept;		//心拍数																//0.00ms
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			//
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(GetBottomStandAnimSel()).per = 1.f;
				//
				m_CharaAnimeSet.clear();
				//M4
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().m_Down = CharaAnimeID::Upper_Down1;
				m_CharaAnimeSet.back().m_Ready = CharaAnimeID::Upper_Ready1;
				m_CharaAnimeSet.back().m_ADS = CharaAnimeID::Upper_ADS1;
				m_CharaAnimeSet.back().m_Cocking = CharaAnimeID::Upper_Cocking1;
				m_CharaAnimeSet.back().m_Reload = CharaAnimeID::Upper_Reload1Start;
				//ハンドガン
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().m_Down = CharaAnimeID::Upper_Down2;
				m_CharaAnimeSet.back().m_Ready = CharaAnimeID::Upper_Ready2;
				m_CharaAnimeSet.back().m_ADS = CharaAnimeID::Upper_ADS2;
				m_CharaAnimeSet.back().m_Cocking = CharaAnimeID::Upper_Cocking2;
				m_CharaAnimeSet.back().m_Reload = CharaAnimeID::Upper_Reload2Start;
				//
				m_GunAnimeSet.clear();
				//M4
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				m_GunAnimeSet.back().m_Run = EnumGunAnim::M16_run;
				m_GunAnimeSet.back().m_Ready = EnumGunAnim::M16_ready;
				m_GunAnimeSet.back().m_Aim.emplace_back(EnumGunAnim::M16_aim);
				m_GunAnimeSet.back().m_ADS = EnumGunAnim::M16_ads;
				m_GunAnimeSet.back().m_Reload = EnumGunAnim::M16_reload;
				//ハンドガン
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				m_GunAnimeSet.back().m_Run = EnumGunAnim::M1911_run;
				m_GunAnimeSet.back().m_Ready = EnumGunAnim::M1911_ready;
				m_GunAnimeSet.back().m_Aim.emplace_back(EnumGunAnim::M1911_aim1);
				m_GunAnimeSet.back().m_Aim.emplace_back(EnumGunAnim::M1911_aim2);
				m_GunAnimeSet.back().m_ADS = EnumGunAnim::M1911_ads;
				m_GunAnimeSet.back().m_Reload = EnumGunAnim::M1911_reload;
				//
				m_HitBox.resize(27);
			}
			void			FirstExecute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < GetObj().get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					GetObj().work_anime();
				}
				if (GetGunPtrNow() != nullptr) {
					auto prev = this->m_CharaAnimeSel;
					auto newtmp = GetGunPtrNow()->GetHumanAnimType();
					bool ischange = true;
					if (prev != newtmp) {
						if (this->m_UpperAnimSelect == GetDownAnimSel()) {
							ischange = false;
						}
						if (ischange) {
							GetAnimeBuf(GetAimAnimSel()) = 0.f;
						}
						this->m_ReadyTimer = 0.1f;
						this->m_ReadyPer = 0.f;
					}
					this->m_CharaAnimeSel = newtmp;
					if (prev != newtmp) {
						if (ischange) {
							GetAnimeBuf(GetDownAnimSel()) = 1.f;
						}
					}
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteUpperMatrix();		//上半身回転//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
				ExecuteHeartRate();			//心拍数//0.00ms
				CalcFov();
			}
			void			Draw(void) noexcept override {
				int fog_enable;
				int fog_mode;
				int fog_r, fog_g, fog_b;
				float fog_start, fog_end;
				float fog_density;

				fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
				fog_mode = GetFogMode();														// フォグモードを取得する
				GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
				GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
				fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

				SetFogEnable(TRUE);
				SetFogColor(0, 0, 0);
				SetFogStartEnd(Scale_Rate*1.f, Scale_Rate*20.f);
				SetFogEnable(FALSE);

				//
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-30, 0, -30)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(30, 30, 30)).get()) == FALSE
						) {
						auto* DrawParts = DXDraw::Instance();
						DrawParts->SetUseFarShadowDraw(false);
						this->GetObj().DrawModel();
						DrawParts->SetUseFarShadowDraw(true);
					}
				}
				//hitbox描画
				if (false) {
					//this->GetObj().SetOpacityRate(0.5f);
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					SetUseZBuffer3D(FALSE);

					for (auto& h : this->m_HitBox) {
						h.Draw();
					}

					SetUseZBuffer3D(TRUE);
					SetUseLighting(TRUE);
				}

				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
		};
	};
};
