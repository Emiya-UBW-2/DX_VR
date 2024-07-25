#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// 当たり判定カプセルの高さ
		bool BackGroundClass::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) const noexcept
		{
			std::vector<std::pair<const MV1*, int>> cols;
			cols.emplace_back(std::make_pair<const MV1*, int>(&this->m_ObjGroundCol, -1));
			if (cols.size() == 0) { return false; }
			auto MoveVector = *EndPos - StartPos;
			//MoveVector.y = (0);
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			for (const auto& objs : cols) {
				if ((&objs - &cols.front()) != 0) {
					if (GetMinLenSegmentToPoint(StartPos, StartPos + Vector3DX::up(), objs.first->GetMatrix().pos()) >=
						(20.f * Scale_Rate + (Radius * 3.f) + MoveVector.magnitude())) {
						continue;
					}
				}
				auto HitDim = objs.first->CollCheck_Sphere(StartPos, (Radius * 3.f) + MoveVector.magnitude(), objs.second);
				// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
				for (int i = 0; i < HitDim.HitNum; ++i) {
					auto& h_d = HitDim.Dim[i];
					//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋(Radius/3.f)より高いポリゴンのみ当たり判定を行う
					if (
						(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
						&& (h_d.Position[0].y > StartPos.y + (Radius / 3.f) || h_d.Position[1].y > StartPos.y + (Radius / 3.f) || h_d.Position[2].y > StartPos.y + (Radius / 3.f))
						&& (h_d.Position[0].y < StartPos.y + (Radius * 3.f) || h_d.Position[1].y < StartPos.y + (Radius * 3.f) || h_d.Position[2].y < StartPos.y + (Radius * 3.f))
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
					if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
						HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
						if (MoveVector.magnitude() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							*EndPos = Vector3DX::Cross(h_d.Normal, Vector3DX::Cross(MoveVector, h_d.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
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
					for (int k = 0; k < 16; ++k) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
						bool HitF = false;
						for (auto& h_d : kabe_) {
							if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// プレイヤーと当たっているかを判定
								*EndPos += Vector3DX(h_d.Normal) * (0.015f * Scale_Rate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
								//EndPos->y(StartPos.y);
								bool j = false;
								for (auto& h_d2 : kabe_) {
									if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// 当たっていたらループを抜ける
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
	}
}
