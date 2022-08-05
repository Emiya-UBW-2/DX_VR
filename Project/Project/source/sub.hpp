#pragma once
#include"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
	//入力
	class InputControl {
	public:
		float m_AddxRad{ 0.f };
		float m_AddyRad{ 0.f };
		bool m_GoFrontPress{ false };
		bool m_GoBackPress{ false };
		bool m_GoLeftPress{ false };
		bool m_GoRightPress{ false };
		bool m_RunPress{ false };
		bool m_QPress{ false };
		bool m_EPress{ false };
		bool m_GoFlight{ false };
		bool m_Press_Accel{ false };
		bool m_Press_Brake{ false };
		bool m_RightPress{ false };
		bool m_LeftPress{ false };
		bool m_UseItemPress{ false };
		bool m_UpPress{ false };
		bool m_DownPress{ false };
		bool m_UseMagicPress{ false };
	public:
		void			SetInput(
			float pAddxRad, float pAddyRad,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,

			bool pRunPress,
			bool pQPress,
			bool pEPress,
			bool pGoFlight,
			bool pPress_Accel,
			bool pPress_Brake,
			bool pRightPress,
			bool pLeftPress,
			bool pUseItemPress,
			bool pUpPress,
			bool pDownPress,
			bool pUseMagicPress
		) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;
			this->m_GoFrontPress = pGoFrontPress;
			this->m_GoBackPress = pGoBackPress;
			this->m_GoLeftPress = pGoLeftPress;
			this->m_GoRightPress = pGoRightPress;
			this->m_RunPress = pRunPress;
			this->m_QPress = pQPress;
			this->m_EPress = pEPress;
			this->m_GoFlight = pGoFlight;
			this->m_Press_Accel = pPress_Accel;
			this->m_Press_Brake = pPress_Brake;
			this->m_RightPress = pRightPress;
			this->m_LeftPress = pLeftPress;
			this->m_UseItemPress = pUseItemPress;
			this->m_UpPress = pUpPress;
			this->m_DownPress = pDownPress;
			this->m_UseMagicPress = pUseMagicPress;
		}
	};
	//キャラ入力
	class CharacterMoveGroundControl {
	private:
		std::array<float, 4>								m_Vec{ 0,0,0,0 };
		bool												m_Press_GoFront{ false };
		bool												m_Press_GoRear{ false };
		bool												m_Press_GoLeft{ false };
		bool												m_Press_GoRight{ false };
		switchs												m_QKey;
		switchs												m_EKey;
		bool												m_IsRun{ false };
		bool												m_IsSprint{ false };
		float												m_RunPer{ 0.f };
		float												m_RunTimer{ 0.f };
		float												m_SprintPer{ 0.f };
		VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
		int													m_TurnRate{ 0 };
		float												m_TurnRatePer{ 0.f };
	private: //内部
		void			SetVec(int pDir, bool Press) {
			this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
			this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
		}
	public:
		const auto		GetTurnRatePer() const noexcept { return  this->m_TurnRatePer; }
		const auto		GetRad() const noexcept { return  this->m_rad; }
		const auto		GetVecFront() const noexcept { return  this->m_Vec[0] || m_IsSprint; }
		const auto		GetVecRear() const noexcept { return this->m_Vec[2]; }
		const auto		GetVecLeft() const noexcept { return this->m_Vec[1]; }
		const auto		GetVecRight() const noexcept { return this->m_Vec[3]; }
		const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
		const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
		const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
		const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }
		const auto		GetRun() const noexcept { return this->m_IsRun; }
		const auto		GetRunPer() const noexcept { return  this->m_RunPer; }
		const auto		GetSprint() const noexcept { return this->m_IsSprint; }
		const auto		GetSprintPer() const noexcept { return  this->m_SprintPer; }
		const auto		GetVec() const noexcept { return VECTOR_ref::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
		const auto		GetFrontP() const noexcept {
			auto FrontP = ((GetPressFront() && !GetPressRear())) ? (atan2f(GetVec().x(), -GetVec().z()) * GetVecFront()) : 0.f;
			FrontP += (!GetPressFront() && GetPressRear()) ? (atan2f(-GetVec().x(), GetVec().z()) * GetVecRear()) : 0.f;
			return FrontP;
		}

		void			SetSprintPer(float value) noexcept { this->m_SprintPer = value; }
		void			SetRadBufX(float x) noexcept {
			auto xbuf = this->m_rad_Buf.x();
			Easing(&xbuf, x, 0.9f, EasingType::OutExpo);
			this->m_rad_Buf.x(xbuf);
		}
		void			SetRadBufY(float y) noexcept {
			this->m_rad_Buf.y(y);
			this->m_rad.y(y);
		}
		void			SetRadBufZ(float z) noexcept {
			auto zbuf = this->m_rad_Buf.z();
			Easing(&zbuf, z, 0.9f, EasingType::OutExpo);
			this->m_rad_Buf.z(zbuf);
		}
	public:
		void			ValueSet(float pxRad, float pyRad) {
			for (int i = 0; i < 4; i++) {
				this->m_Vec[i] = 0.f;
			}
			this->m_Press_GoFront = false;
			this->m_Press_GoRear = false;
			this->m_Press_GoLeft = false;
			this->m_Press_GoRight = false;
			this->m_radAdd.clear();
			this->m_IsRun = false;
			this->m_IsSprint = false;
			this->m_RunPer = 0.f;
			this->m_RunTimer = 0.f;
			this->m_SprintPer = 0.f;
			//動作にかかわる操作
			this->m_rad_Buf.x(pxRad);
			this->m_rad_Buf.y(pyRad);
			//上記を反映するもの
			this->m_rad = this->m_rad_Buf;
		}
		void			SetInput(
			float pAddxRad, float pAddyRad,
			const VECTOR_ref& pAddRadvec,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,
			bool pRunPress,
			bool pFlightMode,
			bool pQPress,
			bool pEPress
		) {
			this->m_Press_GoFront = pGoFrontPress && !pFlightMode;
			this->m_Press_GoRear = pGoBackPress && !pFlightMode;
			this->m_Press_GoLeft = pGoLeftPress && !pFlightMode;
			this->m_Press_GoRight = pGoRightPress && !pFlightMode;

			if (!this->m_IsRun && (pRunPress && !pFlightMode)) {
				this->m_RunTimer = 1.f;
			}
			if (this->m_RunTimer > 0.f) {
				this->m_RunTimer -= 1.f / FPS;
				this->m_Press_GoFront = true;
				this->m_IsRun = true;
			}
			else {
				this->m_RunTimer = 0.f;
				this->m_IsRun = pRunPress && !pFlightMode;
			}
			if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
				this->m_IsRun = false;
			}
			this->m_IsSprint = this->m_IsRun && (!GetPressFront() && !GetPressRear());
			{
				m_QKey.GetInput(pQPress && !pFlightMode);
				m_EKey.GetInput(pEPress && !pFlightMode);
				if (m_EKey.trigger()) {
					if (this->m_TurnRate > -1) {
						this->m_TurnRate--;
					}
					else {
						this->m_TurnRate++;
					}
				}
				if (m_QKey.trigger()) {
					if (this->m_TurnRate < 1) {
						this->m_TurnRate++;
					}
					else {
						this->m_TurnRate--;
					}
				}
				if (!GetRun()) {
					this->m_TurnRate = 0;
				}

				this->m_TurnRate = std::clamp(this->m_TurnRate, -1, 1);
				float xadd = 0.f;
				if (this->m_IsSprint) {
					xadd = 0.315f*(-this->m_TurnRate);//スプリント
				}
				else if (GetRun()) {
					xadd = 0.2f*(-this->m_TurnRate);//走り
				}
				Easing(&this->m_TurnRatePer, xadd, 0.9f, EasingType::OutExpo);
			}
			//回転
			{
				auto limchange = Lerp(1.f, powf(1.f - this->GetVecFront(), 0.5f), this->m_RunPer * 0.8f);
				auto tmp = 1.f;
				Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

				this->m_rad_Buf.x(
					std::clamp(this->m_rad_Buf.x() + pAddxRad * tmp, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange)
					+ this->m_radAdd.x()
				);
				this->m_rad_Buf.y(
					this->m_rad_Buf.y() + (pAddyRad + this->m_TurnRatePer / 100.f)*tmp
					+ this->m_radAdd.y()
				);

				Easing(&this->m_rad, this->m_rad_Buf, 0.5f, EasingType::OutExpo);
			}
		}
		void			Execute(void) noexcept {
			//移動ベクトル取得
			{
				SetVec(0, GetPressFront());
				SetVec(1, GetPressLeft());
				SetVec(2, GetPressRear());
				SetVec(3, GetPressRight());
			}
			//
			Easing(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f, EasingType::OutExpo);
			Easing(&this->m_SprintPer, this->m_IsSprint ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
		}
	};
	class CharacterMoveFlightControl {
	private:
		std::array<float, 7>								m_Vec{ 0,0,0,0,0,0,0 };
		bool												m_Flightmode{ false };
		bool												m_Press_Flight{ false };
		bool												m_Press_GoFront{ false };
		bool												m_Press_GoRear{ false };
		bool												m_Press_GoLeft{ false };
		bool												m_Press_GoRight{ false };
		switchs												m_QKey;
		switchs												m_EKey;
		bool												m_Press_Accel{ false };
		bool												m_Press_Brake{ false };
		float												m_FlightPer{ 0.f };
		VECTOR_ref											m_FradAdd_Buf, m_FradAdd;
		float												m_PressFlightTime{ 0.f };
	private: //内部
		void			SetVec(int pDir, bool Press) {
			this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
			this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
		}
	public:
		const auto		GetVecFront() const noexcept { return  this->m_Vec[0]; }
		const auto		GetVecRear() const noexcept { return this->m_Vec[2]; }
		const auto		GetVecLeft() const noexcept { return this->m_Vec[1]; }
		const auto		GetVecRight() const noexcept { return this->m_Vec[3]; }
		const auto		GetVecLeftYaw() const noexcept { return this->m_Vec[4]; }
		const auto		GetVecRightYaw() const noexcept { return this->m_Vec[5]; }
		const auto		GetGoFlight() const noexcept { return this->m_Vec[6]; }
		const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
		const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
		const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
		const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }
		const auto		GetFlightPer() const noexcept { return this->m_FlightPer; }
		const auto		GetFradAdd() const noexcept { return this->m_FradAdd; }

		const auto		GetPressAccel() const noexcept { return this->m_Press_Accel; }
		const auto		GetPressBrake() const noexcept { return this->m_Press_Brake; }

		const auto		GetIsFlightMode() const noexcept { return this->m_Flightmode; }
		void			SetIsFlightMode(bool value) noexcept { this->m_Flightmode = value; }

	public:
		void			ValueSet() {
			this->m_FradAdd_Buf.clear();
			this->m_FradAdd.clear();
		}
		void			SetInput(
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,
			bool pFlightMode,
			bool pQPress,
			bool pEPress,
			bool pPress_Accel,
			bool pPress_Brake
		) {
			if (this->m_PressFlightTime == 0.f && pFlightMode) {
				this->m_PressFlightTime = 5.f;
			}
			this->m_Press_Flight = (this->m_PressFlightTime > (5.f - 1.f));
			this->m_Flightmode |= this->m_Press_Flight;

			this->m_Press_GoFront = pGoFrontPress && this->m_Flightmode;
			this->m_Press_GoRear = pGoBackPress && this->m_Flightmode;
			this->m_Press_GoLeft = pGoLeftPress && this->m_Flightmode;
			this->m_Press_GoRight = pGoRightPress && this->m_Flightmode;
			m_QKey.GetInput(pQPress && this->m_Flightmode);
			m_EKey.GetInput(pEPress && this->m_Flightmode);
			this->m_Press_Accel = pPress_Accel && this->m_Flightmode;
			this->m_Press_Brake = pPress_Brake && this->m_Flightmode;
			if (m_EKey.trigger()) {
			}
			if (m_QKey.trigger()) {
			}
		}
		void			Execute(void) noexcept {
			//移動ベクトル取得
			{
				SetVec(0, GetPressFront());
				SetVec(1, GetPressLeft());
				SetVec(2, GetPressRear());
				SetVec(3, GetPressRight());

				SetVec(4, m_QKey.press());
				SetVec(5, m_EKey.press());

				SetVec(6, this->m_Press_Flight);
			}
			//
			this->m_PressFlightTime = std::max(this->m_PressFlightTime - 1.f / FPS, 0.f);
			//
			Easing(&this->m_FlightPer, this->m_Flightmode ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
			//回転
			this->m_FradAdd_Buf.x((GetVecRear() - GetVecFront())*2.f / 100.f);
			this->m_FradAdd_Buf.y((GetVecRight() - GetVecLeft())*4.f / 100.f);
			this->m_FradAdd_Buf.z((GetVecRightYaw() - GetVecLeftYaw())*0.6f / 100.f);
			Easing(&this->m_FradAdd, this->m_FradAdd_Buf, 0.95f, EasingType::OutExpo);
		}
	};

	EffectControl	effectControl;	//エフェクトリソース
	//エフェクト利用コントロール
	class Effect_UseControl {
		std::array<EffectS, int(Effect::effects)> effcs;	/*エフェクト*/
		std::array<EffectS, 256> effcs_G;					/*エフェクト*/
		int G_cnt = 0;
	public:
		const auto CheckPlayEffect(Effect ef_) const noexcept { return this->effcs[(int)ef_].GetIsPlaying(); }
		void Set_FootEffect(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs_G[this->G_cnt].Stop();
			this->effcs_G[this->G_cnt].Set(pos_t, nomal_t, scale);
			++this->G_cnt %= 256;
		}
		const auto Check_FootEffectCnt(void) noexcept {
			int cnt = 0;
			for (auto& t : this->effcs_G) {
				if (t.GetStart()) { cnt++; }
			}
			return cnt;
		}

		void Set_LoopEffect(Effect ef_, const VECTOR_ref& pos_t) noexcept {
			this->effcs[(int)ef_].Stop();
			this->effcs[(int)ef_].pos = pos_t;
			this->effcs[(int)ef_].set_loop(effectControl.effsorce[(int)ef_]);
		}
		void Update_LoopEffect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs[(int)ef_].put_loop(pos_t, nomal_t, scale);
		}

		void Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		void Stop_Effect(Effect ef_) noexcept { this->effcs[(int)ef_].Stop(); }

		void SetSpeed_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Speed(value); }
		void SetScale_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Scale(value); }
		//エフェクトの更新
		void Update_Effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (
					index != (int)Effect::ef_smoke
					&& index != (int)Effect::ef_FireBallLoop
					&& index != (int)Effect::ef_ThunderStart	//雷はじめ
					&& index != (int)Effect::ef_ThunderLoop		//雷ループ
					) {
					t.put(effectControl.effsorce[index]);
				}
			}
			for (auto& t : this->effcs_G) {
				t.put(effectControl.effsorce[(int)Effect::ef_gndsmoke]);
			}
		}
		/*おわり*/
		void Dispose_Effect(void) noexcept {
			for (auto& t : this->effcs) { t.handle.Dispose(); }
		}
	};


	// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	(1.8f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			(0.4f * Scale_Rate)		// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			(1.8f * Scale_Rate)		// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			(16)					// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// 一度の壁押し出し処理でスライドさせる距離
	//壁判定ユニバーサル
	static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const MV1& col_obj_t) noexcept {
		auto MoveVector = *NowPos - OldPos;
		// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
		auto HitDim = col_obj_t.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
		std::vector<MV1_COLL_RESULT_POLY*> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
		// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
		for (int i = 0; i < HitDim.HitNum; ++i) {
			auto& h_d = HitDim.Dim[i];
			//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋0.1fより高いポリゴンのみ当たり判定を行う
			if (
				(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
				&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
				&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y < OldPos.y() + 1.6f)
				) {
				kabe_.emplace_back(&h_d);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
			}
		}
		bool HitFlag = false;
		// 壁ポリゴンとの当たり判定処理
		if (kabe_.size() > 0) {
			HitFlag = false;
			for (auto& KeyBind : kabe_) {
				if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					if (MoveVector.size() >= 0.0001f) {	// x軸かy軸方向に 0.0001f 以上移動した場合は移動したと判定
						// 壁に当たったら壁に遮られない移動成分分だけ移動する
						*NowPos = VECTOR_ref(KeyBind->Normal).cross(MoveVector.cross(KeyBind->Normal)) + OldPos;
						bool j = false;
						for (auto& b_ : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
								j = true;
								break;// 当たっていたらループから抜ける
							}
						}
						if (!j) {
							HitFlag = false;
							break;//どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
						}
					}
					else {
						break;
					}
				}
			}
			//*
			if (
				HitFlag
				) {		// 壁に当たっていたら壁から押し出す処理を行う
				for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
					bool HitF = false;
					for (auto& KeyBind : kabe_) {
						if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {// プレイヤーと当たっているかを判定
							*NowPos += VECTOR_ref(KeyBind->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							bool j = false;
							for (auto& b_ : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// 当たっていたらループを抜ける
									j = true;
									break;
								}
							}
							if (!j) {// 全てのポリゴンと当たっていなかったらここでループ終了
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
						break;
					}
				}
			}
			//*/
			kabe_.clear();
		}
		MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
		return HitFlag;
	}
};