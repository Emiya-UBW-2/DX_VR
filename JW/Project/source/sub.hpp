#pragma once
#include	"Header.hpp"

//
namespace FPS_n2 {
	// プレイヤー関係の定義
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			(0.6f * Scale_Rate)		// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			(16)					// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// 一度の壁押し出し処理でスライドさせる距離
	//壁判定ユニバーサル
	static bool col_wall(const Vector3DX& OldPos, Vector3DX* NowPos, const std::vector<std::pair<MV1*, int>>& col_obj_t) noexcept {
		if (col_obj_t.size() == 0) { return false; }
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y = (0);
		// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
		for (const auto& objs : col_obj_t) {
			if ((&objs - &col_obj_t.front()) != 0) {
				if (GetMinLenSegmentToPoint(OldPos, OldPos + Vector3DX::up(), objs.first->GetMatrix().pos()) >=
					(20.f*Scale_Rate + PLAYER_ENUM_DEFAULT_SIZE + MoveVector.magnitude())) {
					continue;
				}
			}
			auto HitDim = objs.first->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.magnitude(), objs.second);
			// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋PLAYER_ENUM_MIN_SIZEより高いポリゴンのみ当たり判定を行う
				if (
					(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
					&& (h_d.Position[0].y > OldPos.y + PLAYER_ENUM_MIN_SIZE || h_d.Position[1].y > OldPos.y + PLAYER_ENUM_MIN_SIZE || h_d.Position[2].y > OldPos.y + PLAYER_ENUM_MIN_SIZE)
					&& (h_d.Position[0].y < OldPos.y + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[1].y < OldPos.y + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[2].y < OldPos.y + PLAYER_ENUM_DEFAULT_SIZE)
					) {
					kabe_.emplace_back(h_d);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
				}
			}
			MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
		}
		bool HitFlag = false;
		// 壁ポリゴンとの当たり判定処理
		if (kabe_.size() > 0) {
			HitFlag = false;
			for (auto& h_d : kabe_) {
				if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					if (MoveVector.magnitude() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
						// 壁に当たったら壁に遮られない移動成分分だけ移動する
						*NowPos = Vector3DX::Cross(h_d.Normal, Vector3DX::Cross(MoveVector, h_d.Normal)) + OldPos;
						//NowPos->y(OldPos.y);
						bool j = false;
						for (auto& h_d2 : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
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
					for (auto& h_d : kabe_) {
						if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// プレイヤーと当たっているかを判定
							*NowPos += Vector3DX(h_d.Normal) * PLAYER_HIT_SLIDE_LENGTH;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							//NowPos->y(OldPos.y);
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// 当たっていたらループを抜ける
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
		return HitFlag;
	}

	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{ 127 };
		PlayerID				DamageID{ 127 };
		HitPoint				Damage{ 0 };
		ArmerPoint				ArmerDamage{ 0 };
		Vector3DX				m_Pos;
	public:
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage, const Vector3DX& pPos) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
			this->ArmerDamage = pArmerDamage;
			this->m_Pos = pPos;
		}
	};
	class DamageEventControl {
		std::array<DamageEvent, 16>							m_Damage;
		uint8_t												m_DamageCount{ 0 };
	public:
		const auto& GetDamageEvent(void) const noexcept { return this->m_Damage; }
		const auto& GetDamageEventCount(void) const noexcept { return this->m_DamageCount; }
	public:
		void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept {
			if (!pRet) { return; }
			for (auto& e : this->m_Damage) {
				if ((&e - &this->m_Damage.front()) >= this->m_DamageCount) { break; }
				pRet->emplace_back(e);
			}
			this->m_DamageCount = 0;
		}
	public:
		void			Reset(void) noexcept {
			this->m_DamageCount = 0;
		}
		void			Add(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage, const Vector3DX& pPos) noexcept {
			this->m_Damage.at(this->m_DamageCount).SetEvent(ShotID_t, DamageID_t, pDamage, pArmerDamage, pPos);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
	};
	//
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		public:
			void			LoadChara(const std::string&FolderName, PlayerID ID) noexcept;
			void			LoadCharaGun(const std::string&FolderName, PlayerID ID, int Sel) noexcept;
			void			LoadOnlyGun(const std::string& FolderName, int Sel) noexcept;
		};
	};
};
