#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		void BackGroundClass::AddCube(const Vector3DX& Pos, float scale, COLOR_U8 color) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Vector3DX PosCamVec = Pos * scale - DrawParts->GetMainCamera().GetCamPos();

			VECTOR Pos1F = ((Pos + Vector3DX::vget(-0.5f, -0.5f, -0.5f)) * scale).get();
			VECTOR Pos2F = ((Pos + Vector3DX::vget(0.5f, 0.5f, 0.5f)) * scale).get();
			//
			//int Prev = (int)vert32.size();
			// 頂点データの作成
			Vector3DX Normal = Vector3DX::back();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize((size_t)(Now + 4));
				index32.resize((size_t)(Nowi + 6));

				index32.at((size_t)(Nowi)) = Now;
				index32.at((size_t)(Nowi + 1)) = Now + 1;
				index32.at((size_t)(Nowi + 2)) = Now + 2;
				index32.at((size_t)(Nowi + 3)) = Now + 3;
				index32.at((size_t)(Nowi + 4)) = index32.at((size_t)(Nowi + 2));
				index32.at((size_t)(Nowi + 5)) = index32.at((size_t)(Nowi + 1));

				vert32.at(index32.at((size_t)(Nowi))).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 1))).pos = Vector3DX::vget(Pos2F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 2))).pos = Pos1F;
				vert32.at(index32.at((size_t)(Nowi + 3))).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos1F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at((size_t)(Now + i)).dif = color;
					vert32.at((size_t)(Now + i)).spc = GetColorU8(255, 255, 255, 255);
					vert32.at((size_t)(Now + i)).u = 0.0f;
					vert32.at((size_t)(Now + i)).v = 0.0f;
					vert32.at((size_t)(Now + i)).su = 0.0f;
					vert32.at((size_t)(Now + i)).sv = 0.0f;
				}
			}

			Normal = Vector3DX::forward();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize((size_t)(Now + 4));
				index32.resize((size_t)(Nowi + 6));

				index32.at((size_t)(Nowi)) = Now;
				index32.at((size_t)(Nowi + 1)) = Now + 1;
				index32.at((size_t)(Nowi + 2)) = Now + 2;
				index32.at((size_t)(Nowi + 3)) = Now + 3;
				index32.at((size_t)(Nowi + 4)) = index32.at((size_t)(Nowi + 2));
				index32.at((size_t)(Nowi + 5)) = index32.at((size_t)(Nowi + 1));

				vert32.at(index32.at((size_t)(Nowi))).pos = Pos2F;
				vert32.at(index32.at((size_t)(Nowi + 1))).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos2F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 2))).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos2F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 3))).pos = Vector3DX::vget(Pos1F.x, Pos1F.y, Pos2F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at((size_t)(Now + i)).norm = Normal.get();
					vert32.at((size_t)(Now + i)).dif = color;
					vert32.at((size_t)(Now + i)).spc = GetColorU8(255, 255, 255, 255);
					vert32.at((size_t)(Now + i)).u = 0.0f;
					vert32.at((size_t)(Now + i)).v = 0.0f;
					vert32.at((size_t)(Now + i)).su = 0.0f;
					vert32.at((size_t)(Now + i)).sv = 0.0f;
				}
			}

			Normal = Vector3DX::left();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize((size_t)(Now + 4));
				index32.resize((size_t)(Nowi + 6));

				index32.at((size_t)(Nowi)) = Now;
				index32.at((size_t)(Nowi + 1)) = Now + 1;
				index32.at((size_t)(Nowi + 2)) = Now + 2;
				index32.at((size_t)(Nowi + 3)) = Now + 3;
				index32.at((size_t)(Nowi + 4)) = index32.at((size_t)(Nowi + 2));
				index32.at((size_t)(Nowi + 5)) = index32.at((size_t)(Nowi + 1));

				vert32.at(index32.at((size_t)(Nowi))).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos2F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 1))).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 2))).pos = Vector3DX::vget(Pos1F.x, Pos1F.y, Pos2F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 3))).pos = Pos1F;
				for (int i = 0; i < 4; i++) {
					vert32.at((size_t)(Now + i)).norm = Normal.get();
					vert32.at((size_t)(Now + i)).dif = color;
					vert32.at((size_t)(Now + i)).spc = GetColorU8(255, 255, 255, 255);
					vert32.at((size_t)(Now + i)).u = 0.0f;
					vert32.at((size_t)(Now + i)).v = 0.0f;
					vert32.at((size_t)(Now + i)).su = 0.0f;
					vert32.at((size_t)(Now + i)).sv = 0.0f;
				}
			}

			Normal = Vector3DX::right();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize((size_t)(Now + 4));
				index32.resize((size_t)(Nowi + 6));

				index32.at((size_t)(Nowi)) = Now;
				index32.at((size_t)(Nowi + 1)) = Now + 1;
				index32.at((size_t)(Nowi + 2)) = Now + 2;
				index32.at((size_t)(Nowi + 3)) = Now + 3;
				index32.at((size_t)(Nowi + 4)) = index32.at((size_t)(Nowi + 2));
				index32.at((size_t)(Nowi + 5)) = index32.at((size_t)(Nowi + 1));

				vert32.at(index32.at((size_t)(Nowi))).pos = Vector3DX::vget(Pos2F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 1))).pos = Pos2F;
				vert32.at(index32.at((size_t)(Nowi + 2))).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 3))).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos2F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at((size_t)(Now + i)).norm = Normal.get();
					vert32.at((size_t)(Now + i)).dif = color;
					vert32.at((size_t)(Now + i)).spc = GetColorU8(255, 255, 255, 255);
					vert32.at((size_t)(Now + i)).u = 0.0f;
					vert32.at((size_t)(Now + i)).v = 0.0f;
					vert32.at((size_t)(Now + i)).su = 0.0f;
					vert32.at((size_t)(Now + i)).sv = 0.0f;
				}
			}

			Normal = Vector3DX::up();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize((size_t)(Now + 4));
				index32.resize((size_t)(Nowi + 6));

				index32.at((size_t)(Nowi)) = Now;
				index32.at((size_t)(Nowi + 1)) = Now + 1;
				index32.at((size_t)(Nowi + 2)) = Now + 2;
				index32.at((size_t)(Nowi + 3)) = Now + 3;
				index32.at((size_t)(Nowi + 4)) = index32.at((size_t)(Nowi + 2));
				index32.at((size_t)(Nowi + 5)) = index32.at((size_t)(Nowi + 1));

				vert32.at(index32.at((size_t)(Nowi))).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos2F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 1))).pos = Pos2F;
				vert32.at(index32.at((size_t)(Nowi + 2))).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 3))).pos = Vector3DX::vget(Pos2F.x, Pos2F.y, Pos1F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at((size_t)(Now + i)).norm = Normal.get();
					vert32.at((size_t)(Now + i)).dif = color;
					vert32.at((size_t)(Now + i)).spc = GetColorU8(255, 255, 255, 255);
					vert32.at((size_t)(Now + i)).u = 0.0f;
					vert32.at((size_t)(Now + i)).v = 0.0f;
					vert32.at((size_t)(Now + i)).su = 0.0f;
					vert32.at((size_t)(Now + i)).sv = 0.0f;
				}
			}

			Normal = Vector3DX::down();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize((size_t)(Now + 4));
				index32.resize((size_t)(Nowi + 6));

				index32.at((size_t)(Nowi)) = Now;
				index32.at((size_t)(Nowi + 1)) = Now + 1;
				index32.at((size_t)(Nowi + 2)) = Now + 2;
				index32.at((size_t)(Nowi + 3)) = Now + 3;
				index32.at((size_t)(Nowi + 4)) = index32.at((size_t)(Nowi + 2));
				index32.at((size_t)(Nowi + 5)) = index32.at((size_t)(Nowi + 1));

				vert32.at(index32.at((size_t)(Nowi))).pos = Pos1F;
				vert32.at(index32.at((size_t)(Nowi + 1))).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos1F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 2))).pos = Vector3DX::vget(Pos1F.x, Pos1F.y, Pos2F.z).get();
				vert32.at(index32.at((size_t)(Nowi + 3))).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos2F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at((size_t)(Now + i)).norm = Normal.get();
					vert32.at((size_t)(Now + i)).dif = color;
					vert32.at((size_t)(Now + i)).spc = GetColorU8(255, 255, 255, 255);
					vert32.at((size_t)(Now + i)).u = 0.0f;
					vert32.at((size_t)(Now + i)).v = 0.0f;
					vert32.at((size_t)(Now + i)).su = 0.0f;
					vert32.at((size_t)(Now + i)).sv = 0.0f;
				}
			}

			/*
			int NowMax = (int)vert32.size();
			for (int i = Prev; i < NowMax; i++) {
				vert32.at(i).spos.x = vert32.at(i).pos.x;
				vert32.at(i).spos.y = vert32.at(i).pos.y;
				vert32.at(i).spos.z = vert32.at(i).pos.z;
				vert32.at(i).spos.x = 1.0f;

				vert32.at(i).norm = Normal.get();
				vert32.at(i).binorm = Normal.get();
				vert32.at(i).tan = Normal.get();
			}
			//*/
		}
		// 当たり判定カプセルの高さ
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		
		bool BackGroundClass::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) const noexcept
		{
			auto MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			auto& cell = m_CellxN.back();
			Vector3DX Start = StartPos / (CellScale * cell.scaleRate);
			Vector3DX End = StartPos / (CellScale * cell.scaleRate);

			for (int xm = -2; xm <= 2; xm++) {
				for (int ym = 2; ym <= 2; ym++) {
					for (int zm = -2; zm <= 2; zm++) {
						int x1 = (int)(Start.x + xm);
						int y1 = (int)(Start.y + ym);
						int z1 = (int)(Start.z + zm);
						int x2 = (int)(End.x + xm);
						int y2 = (int)(End.y + ym);
						int z2 = (int)(End.z + zm);
						Bresenham3D(x1, y1, z1, x2, y2, z2, [&](int x, int y, int z) {
							if (
								((x <= -cell.Xall / 2) || (cell.Xall / 2 < x)) ||
								((y <= -cell.Yall / 2) || (cell.Yall / 2 < y)) ||
								((z <= -cell.Zall / 2) || (cell.Zall / 2 < z))
								) {
								return false;
							}

							const auto& Cell = cell.GetCell(x, y, z);
							if (Cell.selset == INVALID_ID) { return false; }
							if (Cell.inRock) { return false; }
							Vector3DX MinPos = (Vector3DX::vget((float)x, (float)y, (float)z) + (Vector3DX::one() * -0.1f)) * (CellScale * cell.scaleRate);
							Vector3DX MaxPos = (Vector3DX::vget((float)x, (float)y, (float)z) + (Vector3DX::one() * 1.1f)) * (CellScale * cell.scaleRate);

							MV1_COLL_RESULT_POLY tmp{};
							// Left
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							// Right
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							// Back
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							// Forward
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							return true;
							});
					}
				}
			}
			bool HitFlag = false;
			// 壁ポリゴンとの当たり判定処理
			if (kabes.size() > 0) {
				HitFlag = false;
				for (auto& kabe : kabes) {
					if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
						HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
						if (MoveVector.magnitude() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							*EndPos = Vector3DX::Cross(kabe.Normal, Vector3DX::Cross(MoveVector, kabe.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
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
						for (auto& kabe : kabes) {
							if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {// プレイヤーと当たっているかを判定
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale_Rate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
								//EndPos->y(StartPos.y);
								bool j = false;
								for (auto& kabe2 : kabes) {
									if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {// 当たっていたらループを抜ける
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
				kabes.clear();
			}
			return HitFlag;
		}
	}
}
