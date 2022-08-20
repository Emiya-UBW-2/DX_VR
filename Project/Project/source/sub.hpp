#pragma once
#include"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void		operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
	//
	static auto* CreateB2Body(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) noexcept {
		b2BodyDef f_bodyDef;
		f_bodyDef.type = type;
		f_bodyDef.position.Set(x_, y_);
		f_bodyDef.angle = angle;
		return world->CreateBody(&f_bodyDef);
	}
	//入力
	class InputControl {
	private:
		float			m_AddxRad{ 0.f };
		float			m_AddyRad{ 0.f };
		float			m_xRad{ 0.f };
		float			m_yRad{ 0.f };
		unsigned int	m_Flags{ 0 };
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

			bool pRightPress,
			bool pLeftPress,
			bool pUpPress,
			bool pDownPress,

			bool pAction1,
			bool pAction2,
			bool pAction3,
			bool pAction4,
			bool pAction5
		) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;
			this->m_Flags = 0;
			if (pGoFrontPress) { this->m_Flags |= (1 << 0); }
			if (pGoBackPress) { this->m_Flags |= (1 << 1); }
			if (pGoLeftPress) { this->m_Flags |= (1 << 2); }
			if (pGoRightPress) { this->m_Flags |= (1 << 3); }
			if (pRunPress) { this->m_Flags |= (1 << 4); }
			if (pQPress) { this->m_Flags |= (1 << 5); }
			if (pEPress) { this->m_Flags |= (1 << 6); }
			if (pRightPress) { this->m_Flags |= (1 << 7); }
			if (pLeftPress) { this->m_Flags |= (1 << 8); }
			if (pUpPress) { this->m_Flags |= (1 << 9); }
			if (pDownPress) { this->m_Flags |= (1 << 10); }
			if (pAction1) { this->m_Flags |= (1 << 11); }
			if (pAction2) { this->m_Flags |= (1 << 12); }
			if (pAction3) { this->m_Flags |= (1 << 13); }
			if (pAction4) { this->m_Flags |= (1 << 14); }
			if (pAction5) { this->m_Flags |= (1 << 15); }
		}
		void			SetRadBuf(float pxRad, float pyRad) {
			this->m_xRad = pxRad;
			this->m_yRad = pyRad;
		}
		void			SetKeyInput(unsigned int pFlags) { this->m_Flags = pFlags; }
		const auto&		GetKeyInput(void) const noexcept { return this->m_Flags; }


		const auto&		GetAddxRad(void) const noexcept { return this->m_AddxRad; }
		const auto&		GetAddyRad(void) const noexcept { return this->m_AddyRad; }
		const auto&		GetxRad(void) const noexcept { return this->m_xRad; }
		const auto&		GetyRad(void) const noexcept { return this->m_yRad; }
		const auto	GetGoFrontPress(void) const noexcept { return (this->m_Flags & (1 << 0)) != 0; }
		const auto	GetGoBackPress(void) const noexcept { return (this->m_Flags & (1 << 1)) != 0; }
		const auto	GetGoLeftPress(void) const noexcept { return (this->m_Flags & (1 << 2)) != 0; }
		const auto	GetGoRightPress(void) const noexcept { return (this->m_Flags & (1 << 3)) != 0; }
		const auto	GetRunPress(void) const noexcept { return (this->m_Flags & (1 << 4)) != 0; }
		const auto	GetQPress(void) const noexcept { return (this->m_Flags & (1 << 5)) != 0; }
		const auto	GetEPress(void) const noexcept { return (this->m_Flags & (1 << 6)) != 0; }
		const auto	GetRightPress(void) const noexcept { return (this->m_Flags & (1 << 7)) != 0; }
		const auto	GetLeftPress(void) const noexcept { return (this->m_Flags & (1 << 8)) != 0; }
		const auto	GetUpPress(void) const noexcept { return (this->m_Flags & (1 << 9)) != 0; }
		const auto	GetDownPress(void) const noexcept { return (this->m_Flags & (1 << 10)) != 0; }
		const auto	GetAction1(void) const noexcept { return (this->m_Flags & (1 << 11)) != 0; }
		const auto	GetAction2(void) const noexcept { return (this->m_Flags & (1 << 12)) != 0; }
		const auto	GetAction3(void) const noexcept { return (this->m_Flags & (1 << 13)) != 0; }
		const auto	GetAction4(void) const noexcept { return (this->m_Flags & (1 << 14)) != 0; }
		const auto	GetAction5(void) const noexcept { return (this->m_Flags & (1 << 15)) != 0; }

		const InputControl operator+(const InputControl& o) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad + o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad + o.m_AddyRad;
			tmp.m_xRad = this->m_xRad + o.m_xRad;
			tmp.m_yRad = this->m_yRad + o.m_yRad;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}
		const InputControl operator-(const InputControl& o) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad - o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad - o.m_AddyRad;
			tmp.m_xRad = this->m_xRad - o.m_xRad;
			tmp.m_yRad = this->m_yRad - o.m_yRad;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}
		const InputControl operator*(float per) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad *per;
			tmp.m_AddyRad = this->m_AddyRad *per;
			tmp.m_xRad = this->m_xRad *per;
			tmp.m_yRad = this->m_yRad *per;
			tmp.m_Flags = this->m_Flags;

			return tmp;
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
		const auto		GetRadBuf(void) const noexcept { return  this->m_rad_Buf; }
		const auto		GetTurnRatePer(void) const noexcept { return  this->m_TurnRatePer; }
		const auto		GetRad(void) const noexcept { return  this->m_rad; }
		const auto		GetVecFront(void) const noexcept { return  this->m_Vec[0] || this->m_IsSprint; }
		const auto		GetVecRear(void) const noexcept { return this->m_Vec[2]; }
		const auto		GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
		const auto		GetVecRight(void) const noexcept { return this->m_Vec[3]; }
		const auto		GetPressFront(void) const noexcept { return this->m_Press_GoFront; }
		const auto		GetPressRear(void) const noexcept { return this->m_Press_GoRear; }
		const auto		GetPressLeft(void) const noexcept { return this->m_Press_GoLeft; }
		const auto		GetPressRight(void) const noexcept { return this->m_Press_GoRight; }
		const auto		GetRun(void) const noexcept { return this->m_IsRun; }
		const auto		GetRunPer(void) const noexcept { return  this->m_RunPer; }
		const auto		GetSprint(void) const noexcept { return this->m_IsSprint; }
		const auto		GetSprintPer(void) const noexcept { return  this->m_SprintPer; }
		const auto		GetVec(void) const noexcept { return VECTOR_ref::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
		const auto		GetFrontP(void) const noexcept {
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
			float pxRad, float pyRad,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,
			bool pRunPress,
			bool pIsNotActive,
			bool pQPress,
			bool pEPress
		) {
			this->m_Press_GoFront = pGoFrontPress && !pIsNotActive;
			this->m_Press_GoRear = pGoBackPress && !pIsNotActive;
			this->m_Press_GoLeft = pGoLeftPress && !pIsNotActive;
			this->m_Press_GoRight = pGoRightPress && !pIsNotActive;

			if (!this->m_IsRun && (pRunPress && !pIsNotActive)) {
				this->m_RunTimer = 1.f;
			}
			if (this->m_RunTimer > 0.f) {
				this->m_RunTimer -= 1.f / FPS;
				this->m_Press_GoFront = true;
				this->m_IsRun = true;
			}
			else {
				this->m_RunTimer = 0.f;
				this->m_IsRun = pRunPress && !pIsNotActive;
			}
			if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
				this->m_IsRun = false;
			}
			this->m_IsSprint = this->m_IsRun && (!GetPressFront() && !GetPressRear());
			{
				m_QKey.GetInput(pQPress && !pIsNotActive);
				m_EKey.GetInput(pEPress && !pIsNotActive);
				if (this->m_EKey.trigger()) {
					if (this->m_TurnRate > -1) {
						this->m_TurnRate--;
					}
					else {
						this->m_TurnRate++;
					}
				}
				if (this->m_QKey.trigger()) {
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
				if (pxRad != -1.f || pyRad != -1.f) {
					this->m_rad_Buf.x(pxRad);
					this->m_rad_Buf.y(pyRad);
				}

				Easing(&this->m_rad, this->m_rad_Buf, 0.5f, EasingType::OutExpo);
			}
		}
		void			ExecuteRadBuf(float pAddxRad, float pAddyRad, const VECTOR_ref& pAddRadvec) noexcept {
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

	//エフェクト利用コントロール
	class Effect_UseControl {
		std::array<EffectS, int(Effect::effects)> effcs;	//エフェクト
		std::array<EffectS, 256> effcs_G;					//エフェクト
		int G_cnt = 0;
	public:
		const auto	CheckPlayEffect(Effect ef_) const noexcept { return this->effcs[(int)ef_].GetIsPlaying(); }
		void		Set_FootEffect(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs_G[this->G_cnt].Stop();
			this->effcs_G[this->G_cnt].Set(pos_t, nomal_t, scale);
			++this->G_cnt %= 256;
		}
		const auto	Check_FootEffectCnt(void) noexcept {
			int cnt = 0;
			for (auto& t : this->effcs_G) {
				if (t.GetStart()) { cnt++; }
			}
			return cnt;
		}

		void		Set_LoopEffect(Effect ef_, const VECTOR_ref& pos_t) noexcept {
			auto* EffectUseControl = EffectControl::Instance();
			this->effcs[(int)ef_].Stop();
			this->effcs[(int)ef_].pos = pos_t;
			this->effcs[(int)ef_].set_loop(EffectUseControl->effsorce[(int)ef_]);
		}
		void		Update_LoopEffect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs[(int)ef_].put_loop(pos_t, nomal_t, scale);
		}

		void		Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		void		Stop_Effect(Effect ef_) noexcept { this->effcs[(int)ef_].Stop(); }

		void		SetSpeed_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Speed(value); }
		void		SetScale_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Scale(value); }

		void		Update_Effect(void) noexcept {
			auto* EffectUseControl = EffectControl::Instance();
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (
					index != (int)Effect::ef_smoke
					) {
					t.put(EffectUseControl->effsorce[index]);
				}
			}
			for (auto& t : this->effcs_G) {
				t.put(EffectUseControl->effsorce[(int)Effect::ef_gndsmoke]);
			}
		}
		void		Dispose_Effect(void) noexcept {
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
			if (HitFlag) {		// 壁に当たっていたら壁から押し出す処理を行う
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
			kabe_.clear();
		}
		MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
		return HitFlag;
	}









	//BOX2D
	class b2Pats {
		b2FixtureDef fixtureDef;			//動的ボディフィクスチャを定義します
		std::unique_ptr<b2Body> body;		//
		b2Fixture* playerfix{ nullptr };	//
	public:
		VECTOR_ref pos;//仮

		void		Set(b2Body* body_ptr, b2Shape* dynamicBox) {
			fixtureDef.shape = dynamicBox;								//
			fixtureDef.density = 1.0f;									//ボックス密度をゼロ以外に設定すると、動的になる
			fixtureDef.friction = 0.3f;									//デフォルトの摩擦をオーバーライド
			this->body.reset(body_ptr);									//
			this->playerfix = this->body->CreateFixture(&fixtureDef);	//シェイプをボディに追加
		}

		void		SetLinearVelocity(const b2Vec2& position) {
			this->body->SetLinearVelocity(position);
		}

		void		Update(const VECTOR_ref& add, float yradadd) {
			this->body->SetLinearVelocity(b2Vec2(add.x(), add.z()));
			this->body->SetAngularVelocity(yradadd);
		}


		void		Dispose(void) noexcept {
			if (this->playerfix != nullptr) {
				delete this->playerfix->GetUserData();
				this->playerfix->SetUserData(nullptr);
			}
		}

		const auto	Get(void) noexcept { return body.get(); }
		const auto	Pos(void) noexcept { return body->GetPosition(); }
		const auto	Rad(void) noexcept { return body->GetAngle(); }
		const auto	Speed(void) noexcept { return std::hypot(this->body->GetLinearVelocity().x, this->body->GetLinearVelocity().y); }
		void		SetTransform(const b2Vec2& position, float32 angle) {
			body->SetTransform(position, angle);
		}
	};


	//ダメージイベント
	struct DamageEvent {
		PlayerID ID{ 127 };
		HitPoint Damage{ 0 };
		float rad{ 0.f };
		void SetEvent(PlayerID pID, HitPoint pDamage, float pRad) {
			this->ID = pID;
			this->Damage = pDamage;
			this->rad = pRad;
		}
	};


	//インスタシング
	class Model_Instance {
	public:
		int hitss = 0;					//hitsの数
		std::vector<VERTEX3D> hitsver;	//hits
		std::vector<DWORD> hitsind;	    //hits
		int VerBuf = -1, IndexBuf = -1;	//hits
		MV1 hits;						//hitsモデル
		GraphHandle hits_pic;			//画像ハンドル
		int IndexNum = -1, VerNum = -1;	//hits
		int vnum = -1, pnum = -1;		//hits
		MV1_REF_POLYGONLIST RefMesh{};	//hits
		//初期化
		void			Init(std::string pngpath, std::string mv1path) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->hits_pic = GraphHandle::Load(pngpath);		 //grass
			MV1::Load(mv1path, &this->hits);	//弾痕
			Init_one();
		}
		void			Init_one(void) noexcept {
			MV1RefreshReferenceMesh(this->hits.get(), -1, TRUE);			//参照用メッシュの更新
			this->RefMesh = MV1GetReferenceMesh(this->hits.get(), -1, TRUE);	//参照用メッシュの取得
		}
		//毎回のリセット
		void			Clear(void) noexcept {
			this->hitss = 0;
			this->vnum = 0;
			this->pnum = 0;
			this->hitsver.clear();								//頂点データとインデックスデータを格納するメモリ領域の確保
			this->hitsind.clear();								//頂点データとインデックスデータを格納するメモリ領域の確保
			this->hitsver.reserve(2000);							//頂点データとインデックスデータを格納するメモリ領域の確保
			this->hitsind.reserve(2000);							//頂点データとインデックスデータを格納するメモリ領域の確保
		}

		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->hitss++;
			Set_start();
			{
				float asize = 200.f * caliber;
				const auto& y_vec = Normal;
				auto z_vec = y_vec.cross(Zvec).Norm();
				auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
				auto pos = Position + y_vec * 0.02f;
				MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec);

				this->hits.SetMatrix(mat * MATRIX_ref::Mtrans(pos));
			}
			Set_one();
		}
		void			Set_start(void) noexcept {
			this->IndexNum = this->RefMesh.PolygonNum * 3 * this->hitss;				//インデックスの数を取得
			this->VerNum = this->RefMesh.VertexNum * this->hitss;						//頂点の数を取得
			this->hitsver.resize(this->VerNum);									//頂点データとインデックスデータを格納するメモリ領域の確保
			this->hitsind.resize(this->IndexNum);								//頂点データとインデックスデータを格納するメモリ領域の確保
		}
		void			Set_one(void) noexcept {
			Init_one();
			for (size_t j = 0; j < size_t(this->RefMesh.VertexNum); ++j) {
				auto& g = this->hitsver[j + this->vnum];
				g.pos = this->RefMesh.Vertexs[j].Position;
				g.norm = this->RefMesh.Vertexs[j].Normal;
				g.dif = this->RefMesh.Vertexs[j].DiffuseColor;
				g.spc = this->RefMesh.Vertexs[j].SpecularColor;
				g.u = this->RefMesh.Vertexs[j].TexCoord[0].u;
				g.v = this->RefMesh.Vertexs[j].TexCoord[0].v;
				g.su = this->RefMesh.Vertexs[j].TexCoord[1].u;
				g.sv = this->RefMesh.Vertexs[j].TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(this->RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(this->RefMesh.Polygons[j].VIndex); ++k)
					this->hitsind[j * 3 + k + this->pnum] = WORD(this->RefMesh.Polygons[j].VIndex[k] + this->vnum);
			}
			this->vnum += this->RefMesh.VertexNum;
			this->pnum += this->RefMesh.PolygonNum * 3;
		}

		void			Update(void) noexcept {
			this->VerBuf = CreateVertexBuffer(this->VerNum, DX_VERTEX_TYPE_NORMAL_3D);
			this->IndexBuf = CreateIndexBuffer(this->IndexNum, DX_INDEX_TYPE_32BIT);
			SetVertexBufferData(0, this->hitsver.data(), this->VerNum, this->VerBuf);
			SetIndexBufferData(0, this->hitsind.data(), this->IndexNum, this->IndexBuf);
		}
		void			Draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			{
				DrawPolygonIndexed3D_UseVertexBuffer(this->VerBuf, this->IndexBuf, this->hits_pic.get(), TRUE);
			}
			//SetDrawAlphaTest(-1, 0);
		}
	};
	//命中根
	class HIT_PASSIVE {
		Model_Instance inst;
		bool isUpdate{ true };
	public:
		//初期化
		void			Init(void) noexcept {
			inst.Init("data/m_obj/hit/hit.png", "data/m_obj/hit/m_obj.mv1");
		}
		//毎回のリセット
		void			Clear(void) noexcept {
			inst.Clear();
		}

		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			inst.Set(caliber, Position, Normal, Zvec);
			isUpdate = true;
		}
		void			Update(void) noexcept {
			if (isUpdate) {
				isUpdate = false;
				inst.Update();
			}
		}
		void			Draw(void) noexcept {
			inst.Draw();
		}
	};
	//kusa
	class Grass {
		class grass_t {
		public:
			bool canlook = true;
			Model_Instance inst;
		public:
			void Init(int total, int sel) {
				switch (sel) {
				case 0:
					this->inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1");
					break;
				case 1:
					this->inst.Init("data/model/grass/grass2.png", "data/model/grass/model.mv1");
					break;
				case 2:
					this->inst.Init("data/model/grass/grass3.png", "data/model/grass/model.mv1");
					break;
				}
				this->inst.Clear();
				this->inst.hitss = total;

				this->inst.Set_start();
			}
			void Set_one(void) noexcept {
				this->inst.Set_one();
			}
			void put(void) noexcept {
				canlook = true;
				this->inst.Update();
			}
			void Dispose(void) noexcept {
				this->inst.hitsver.clear();
				this->inst.hitsind.clear();

				this->inst.hits.Dispose();
				this->inst.hits_pic.Dispose();
			}
			/*視界外か否かを判断*/
			void Check_CameraViewClip(const VECTOR_ref& min, const VECTOR_ref& max) noexcept {
				this->canlook = true;
				if (CheckCameraViewClip_Box(min.get(), max.get())) {
					this->canlook = false;
					return;
				}
			}
			void Draw(void) noexcept {
				if (this->canlook) {
					this->inst.Draw();
				}
			}
		};
		struct GrassPos {
			int X_PosMin = 0;
			int Y_PosMin = 0;
			int X_PosMax = 0;
			int Y_PosMax = 0;
		};
	public:
		static const int grassDiv{ 12 };//6;
		const float size{ 70.f };
	private:
		const int grasss = 50 * 50;						/*grassの数*/
		std::array<grass_t, grassDiv>grass__;
		std::array<VECTOR_ref, grassDiv>grassPosMin;
		std::array<VECTOR_ref, grassDiv>grassPosMax;
		int grasss2 = 30 * 30;							/*grassの数*/
		std::array<grass_t, grassDiv>grass2__;
		int grasss3 = 12 * 12;							/*grassの数*/
		std::array<grass_t, grassDiv>grass3__;
		int Flag = 0;
		std::array<GrassPos, grassDiv> grassPos;
	private:
		int GetColorSoftImage(int softimage, int x_, int y_) {
			int _r_, _g_, _b_;
			int CCC = 0;
			GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
			if (_r_ <= 64) {}
			else if (_r_ <= 192) { CCC |= (1 << 1); }
			else if (_r_ <= 256) { CCC |= (1 << 2); }
			if (_g_ <= 64) {}
			else if (_g_ <= 192) { CCC |= (1 << 4); }
			else if (_g_ <= 256) { CCC |= (1 << 5); }
			if (_b_ <= 64) {}
			else if (_b_ <= 192) { CCC |= (1 << 7); }
			else if (_b_ <= 256) { CCC |= (1 << 8); }
			return CCC;
		}
		//y方向に操作する前提
		void SetMinMax(int CCC, int ID, int softimage, int x_t, int y_t, int sizex, int sizey) {
			int BufC = -1;
			if ((Flag & (1 << ID)) == 0) {
				Flag |= (1 << ID);
				//xmin
				grassPos[ID].X_PosMin = x_t;
				y_t;
				//ymin
				BufC = -1;
				for (int y_ = 0; y_ < sizey; y_++) {
					for (int x_ = grassPos[ID].X_PosMin; x_ < sizex; x_++) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMin = y_;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//xmax
				BufC = -1;
				for (int x_ = sizex - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
					for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].X_PosMax = x_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ymax
				BufC = -1;
				for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
					for (int x_ = grassPos[ID].X_PosMax - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMax = y_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ok
			}
		}
	public:
		void Init(const MV1* MapCol) noexcept {
			float MAPX = 300.f*Scale_Rate;
			float MAPZ = 300.f*Scale_Rate;
			float PosX = 0.f;
			float PosZ = 0.f;

			float MINX = -MAPX / 2.f + PosX;
			float MINZ = -MAPZ / 2.f + PosZ;

			float MAXX = MAPX / 2.f + PosX;
			float MAXZ = MAPZ / 2.f + PosZ;

			auto softimage = LoadSoftImage("data/grass.png");

			int sizex = 0, sizey = 0;
			GetSoftImageSize(softimage, &sizex, &sizey);

			Flag = 0;
			for (int x_ = 0; x_ < sizex; x_++) {
				for (int y_ = 0; y_ < sizey; y_++) {
					int CCC = GetColorSoftImage(softimage, x_, y_);
					//255,0,0
					if (CCC == (1 << 2)) {
						SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
					}
					//255,128,0
					else if (CCC == ((1 << 2) | (1 << 4))) {
						SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
					}
					//255,255,0
					else if (CCC == ((1 << 2) | (1 << 5))) {
						SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
					}
					//128,255,0
					else if (CCC == ((1 << 1) | (1 << 5))) {
						SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
					}
					//0,255,0
					else if (CCC == (1 << 5)) {
						SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
					}
					//0,255,128
					else if (CCC == ((1 << 5) | (1 << 7))) {
						SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
					}
					//0,255,255
					else if (CCC == ((1 << 5) | (1 << 8))) {
						SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
					}
					//0,128,255
					else if (CCC == ((1 << 4) | (1 << 8))) {
						SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
					}
					//0,0,255
					else if (CCC == (1 << 8)) {
						SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
					}
					//128,0,255
					else if (CCC == ((1 << 1) | (1 << 8))) {
						SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
					}
					//255,0,255
					else if (CCC == ((1 << 2) | (1 << 8))) {
						SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
					}
					//255,0,128
					else if (CCC == ((1 << 2) | (1 << 7))) {
						SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
					}

					//MINX + (MAXX - MINX) * x_ / sizex = x_t 
					//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
				}
			}
			for (int ID = 0; ID < grassDiv; ID++) {
				//ポジション決定
				float xp = MINX + (MAXX - MINX) * grassPos[ID].X_PosMin / sizex;
				float zp = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMin / sizey;
				float xp2 = MINX + (MAXX - MINX) * grassPos[ID].X_PosMax / sizex;
				float zp2 = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMax / sizey;
				float xsize = xp2 - xp;
				float zsize = zp2 - zp;
				//
				{
					grassPosMin[ID] = VECTOR_ref::vget(xp, 0.2f, zp);
					grassPosMax[ID] = grassPosMin[ID] + VECTOR_ref::vget(xsize, 1.f, zsize);
					float xmid = xsize / 2.f;
					float zmid = zsize / 2.f;
					if (grasss != 0) {
						auto& tgt_g = grass__[ID];
						tgt_g.Init(grasss, 0);
						for (int i = 0; i < grasss; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}

							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.2f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.f, 4.0f + GetRandf(3.5f), size*1.f);
							auto tmpMat = MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale);
							auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
							if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
							tmpMat = tmpMat * MATRIX_ref::RotVec2(VECTOR_ref::up(), ((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect);
							tgt_g.inst.hits.SetMatrix(tmpMat);
							tgt_g.Set_one();
						}
						tgt_g.put();
					}
					if (grasss2 != 0) {
						auto& tgt_g = grass2__[ID];
						tgt_g.Init(grasss2, 1);
						for (int i = 0; i < grasss2; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}
							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.f, 4.0f + GetRandf(3.5f), size*1.f);
							auto tmpMat = MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale);
							auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
							if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
							tmpMat = tmpMat * MATRIX_ref::RotVec2(VECTOR_ref::up(),((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect);
							tgt_g.inst.hits.SetMatrix(tmpMat);
							tgt_g.Set_one();
						}
						tgt_g.put();
					}
					if (grasss3 != 0) {
						auto& tgt_g = grass3__[ID];
						tgt_g.Init(grasss3, 2);
						for (int i = 0; i < grasss3; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}
							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.f, 4.0f + GetRandf(3.5f), size*1.f);
							auto tmpMat = MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale);
							auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
							if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
							tmpMat = tmpMat * MATRIX_ref::RotVec2(VECTOR_ref::up(), ((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect);
							tgt_g.inst.hits.SetMatrix(tmpMat);
							tgt_g.Set_one();
						}
						tgt_g.put();
					}

					{
						auto res = MapCol->CollCheck_Line(grassPosMin[ID] + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), grassPosMin[ID] + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
						if (res.HitFlag == TRUE) { grassPosMin[ID] = res.HitPosition; }
						res = MapCol->CollCheck_Line(grassPosMax[ID] + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), grassPosMax[ID] + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
						if (res.HitFlag == TRUE) { grassPosMax[ID] = res.HitPosition; }
					}
				}
				//
			}
			DeleteSoftImage(softimage);
		}
		void Dispose(void) noexcept {
			for (int ID = 0; ID < grassDiv; ID++) {
				if (grasss != 0) {
					grass__[ID].Dispose();
				}
				if (grasss2 != 0) {
					grass2__[ID].Dispose();
				}
				if (grasss3 != 0) {
					grass3__[ID].Dispose();
				}
			}
		}
		void Draw(cam_info camera_buf) noexcept {
			SetFogEnable(TRUE);
			SetFogStartEnd(camera_buf.near_, camera_buf.far_*3.f);
			SetFogColor(184, 187, 118);
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			//SetUseLighting(FALSE);
			SetUseLightAngleAttenuation(FALSE);
			//auto dir=GetLightDirection();
			//VECTOR_ref vec = (VECTOR_ref)GetCameraPosition() - GetCameraTarget();
			//SetLightDirection(vec.Norm().get());

			for (int ID = 0; ID < grassDiv; ID++) {
#ifdef DEBUG
				DrawCube3D(grassPosMin[ID].get(), grassPosMax[ID].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
				if (grasss != 0) {
					this->grass__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass__[ID].Draw();
				}
				if (grasss2 != 0) {
					this->grass2__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass2__[ID].Draw();
				}
				if (grasss3 != 0) {
					this->grass3__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass3__[ID].Draw();
				}
			}
			//SetLightDirection(dir);

			SetUseLightAngleAttenuation(TRUE);
			//SetUseLighting(TRUE);
			SetDrawAlphaTest(-1, 0);
			SetFogEnable(FALSE);
		}
	};
};