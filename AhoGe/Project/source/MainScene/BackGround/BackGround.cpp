#include	"BackGround.hpp"

const FPS_n2::Sceneclass::BackGroundClassBase* SingletonBase<FPS_n2::Sceneclass::BackGroundClassBase>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		const float BackGroundClassBase::CheckHideShadow(const Vector3DX& PosA, const Vector3DX& PosB, float Radius) noexcept {
			float Ret = 1.f;
			for (auto& b : m_Blick) {
				for (auto& B : b) {
					// 壁ではない
					if (!B->IsWall) { continue; }
					auto DispPos = Convert2DtoDisp(B->m_Pos);
					if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
					// 辺を登録
					auto GetPer = [&]() {
						if (GetHitCapsuleToTriangle(PosA, PosB, Radius, B->BoxSide.at(0), B->BoxSide.at(1), B->BoxSide.at(3)) ||
							GetHitCapsuleToTriangle(PosA, PosB, Radius, B->BoxSide.at(1), B->BoxSide.at(2), B->BoxSide.at(3))) {

							if (GetHitCapsuleToTriangle(PosA, PosB, 1.f, B->BoxSide.at(0), B->BoxSide.at(1), B->BoxSide.at(3)) ||
								GetHitCapsuleToTriangle(PosA, PosB, 1.f, B->BoxSide.at(1), B->BoxSide.at(2), B->BoxSide.at(3))) {
								return 0.0f;
							}

							float P = 2.f;
							for (int i = 0;i < 4;i++) {
								float ret = (GetMinLenSegmentToSegment(B->BoxSide.at(i), B->BoxSide.at((i + 1) % 4), PosA, PosB) / Radius);
								if (ret < 1.f) {
									if (P > ret) {
										P = ret;
									}
								}
							}
							if (P <= 1.f) {
								return P;
							}
							return 0.0f;
						}
						return 1.0f;
					};
					float tmp = GetPer();
					if (Ret > tmp) {
						Ret = tmp;
					}
					if (Ret <= 0.f) { break; }
				}
				if (Ret <= 0.f) { break; }
			}
			return Ret;
		}
		// 壁判定ユニバーサル
		const bool BackGroundClassBase::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) noexcept {
			Vector3DX EndPt = *EndPos; EndPt.z = 0.f;
			Vector3DX StartPt = StartPos; StartPt.z = 0.f;
			Vector3DX VecPt = EndPt - StartPt;
			WallList.clear();
			Vector3DX Min, Max;
			Min.x = std::min(EndPt.x, StartPt.x) - (Radius * 6);
			Min.y = std::min(EndPt.y, StartPt.y) - (Radius * 6);
			Max.x = std::max(EndPt.x, StartPt.x) + (Radius * 6);
			Max.y = std::max(EndPt.y, StartPt.y) + (Radius * 6);
			// 線のリストを出す
			for (auto& b : m_Blick) {
				for (auto& B : b) {
					// 壁ではない
					if (!B->IsWall) { continue; }
					// 範囲外
					if (!B->CheckRectangleHit(Min, Max)) { continue; }
					// 辺を登録
					for (int i = 0; i < 4; i++) {
						Vector3DX StartP = B->BoxSide.at(i);
						Vector3DX EndP = B->BoxSide.at((i + 1) % 4);
						// 完全に重なっているものは消す
						bool IsSame = false;
						for (auto& w : WallList) {
							if (
								(((w.Position.at(0) - StartP).sqrMagnitude() <= 0.00001f) && ((w.Position.at(1) - EndP).sqrMagnitude() <= 0.00001f)) ||
								(((w.Position.at(1) - StartP).sqrMagnitude() <= 0.00001f) && ((w.Position.at(0) - EndP).sqrMagnitude() <= 0.00001f))
								) {
								IsSame = true;
								w.canuse = false;
							}
						}
						if (!IsSame) {
							WallList.resize(WallList.size() + 1);
							WallList.back().Position.at(0) = StartP;
							WallList.back().Position.at(1) = EndP;
							WallList.back().Position.at(2) = B->BoxSide.at((i + 2) % 4);
							WallList.back().CalcNormal();
							WallList.back().canuse = true;
						}
					}
				}
			}
			// 90度のへこみの場合はならす
			for (int loop = 0;loop < 5;loop++) {
				for (auto& w : WallList) {
					if (!w.canuse) { continue; }
					for (auto& w2 : WallList) {
						if (!w2.canuse) { continue; }
						if (&w == &w2) { continue; }
						Vector3DX* pCenter1 = nullptr;
						Vector3DX* pCenter2 = nullptr;
						Vector3DX* pOther1 = nullptr;
						Vector3DX* pOther2 = nullptr;
						if ((w.Position.at(0) - w2.Position.at(0)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position.at(0);
							pCenter2 = &w2.Position.at(0);
							pOther1 = &w.Position.at(1);
							pOther2 = &w2.Position.at(1);
						}
						else if ((w.Position.at(1) - w2.Position.at(0)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position.at(1);
							pCenter2 = &w2.Position.at(0);
							pOther1 = &w.Position.at(0);
							pOther2 = &w2.Position.at(1);
						}
						else if ((w.Position.at(0) - w2.Position.at(1)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position.at(0);
							pCenter2 = &w2.Position.at(1);
							pOther1 = &w.Position.at(1);
							pOther2 = &w2.Position.at(0);
						}
						else if ((w.Position.at(1) - w2.Position.at(1)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position.at(1);
							pCenter2 = &w2.Position.at(1);
							pOther1 = &w.Position.at(0);
							pOther2 = &w2.Position.at(0);
						}
						else { continue; }

						Vector3DX Ct = (*pOther1 + *pOther2) / 2.f;

						if (Vector3DX::Dot(*pOther1 - *pCenter1, *pOther2 - *pCenter2) < 0.f) { continue; }// 鋭角でない

						if (
							(Vector3DX::Dot(w.Normal, *pOther2 - *pCenter2) < 0.f) &&
							(Vector3DX::Dot(w2.Normal, *pOther1 - *pCenter1) < 0.f)
							) {
							continue;
						}// 法線が内向きでない

									// へこみを減らす
						*pCenter1 = Ct;
						*pCenter2 = Ct;
						//*pCenter1 = (*pCenter1 + Ct) / 2.f;
						//*pCenter2 = (*pCenter2 + Ct) / 2.f;

						// ノーマル再演算
						w.CalcNormal();
						w2.CalcNormal();
					}
				}
			}
			// 外向きなのでノーヒット
			for (auto& w : WallList) {
				if (!w.canuse) { continue; }
				Vector3DX StartP = w.Position.at(0);
				Vector3DX EndP = w.Position.at(1);
				if (Vector3DX::Dot(Vector3DX::Cross(EndP - StartP, VecPt), Vector3DX::Cross(EndP - StartP, w.Normal * -1.f)) < 0.f) {
					w.canuse = false;
				}
			}
			// 使わないものを削除
			for (int i = 0; i < WallList.size(); i++) {
				if (!WallList.at(i).canuse) {
					std::swap(WallList.at(i), WallList.back());
					WallList.pop_back();
					i--;
				}
			}
			bool HitFlag = false;
			// 壁との当たり判定処理
			if (WallList.size() > 0) {
				for (auto& w : WallList) {
					// 壁とプレイヤーが当たっていなかったら次のカウントへ
					if (GetHitCheckToCapsule(StartPt, EndPt, Radius, w.Position.at(0), w.Position.at(1), 0.f)) {
						HitFlag = true;// ここにきたら壁とプレイヤーが当たっているということなので、壁に当たったフラグを立てる
						if (VecPt.sqrMagnitude() >= 0.00001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							EndPt = StartPt + Vector3DX::Cross(w.Normal, Vector3DX::Cross(VecPt, w.Normal));
							bool IsHit = false;
							for (auto& w2 : WallList) {
								if (GetHitCheckToCapsule(StartPt, EndPt, Radius, w2.Position.at(0), w2.Position.at(1), 0.f)) {
									IsHit = true;
									break;
								}
							}
							if (!IsHit) {
								HitFlag = false;
								break;// どの壁とも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
							}
						}
						else {
							break;
						}
					}
				}
				// 上記を経ても壁に当たっていたら壁から押し出す処理を行う
				if (HitFlag) {
					for (int i = 0; i < 16; ++i) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
						bool HitF = false;
						for (auto& w : WallList) {
							if (!GetHitCheckToCapsule(StartPt, EndPt, Radius, w.Position.at(0), w.Position.at(1), 0.f)) { continue; }
							EndPt += w.Normal * 0.0015f;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							bool IsHit = false;
							for (auto& w2 : WallList) {
								if (GetHitCheckToCapsule(StartPt, EndPt, Radius, w2.Position.at(0), w2.Position.at(1), 0.f)) {// 当たっていたらループを抜ける
									IsHit = true;
									break;
								}
							}
							if (!IsHit) {// 全ての壁と当たっていなかったらここでループ終了
								break;
							}
							HitF = true;
						}
						if (!HitF) {// 全部の壁で押し出しを試みる前に全ての壁壁と接触しなくなったということなのでループから抜ける
							break;
						}
					}
				}
				// WallList.clear();
			}
			*EndPos = EndPt;
			return HitFlag;
		}

		void BackGroundClassBase::Init(const char* MapPath) noexcept {
			int xsize = 64;
			int ysize = 64;
			std::string Path;
			// マップ
			Path = "data/map/";
			Path += MapPath;
			Path += "_col.bmp";
			int MapImage = LoadSoftImage(Path.c_str());
			GetSoftImageSize(MapImage, &xsize, &ysize);
			// パレット
			Path = "data/map/";
			Path += MapPath;
			Path += "_chp.bmp";
			int PalImage = LoadSoftImage(Path.c_str());
			int ChipNum = 12;
			GetSoftImageSize(PalImage, &ChipNum, NULL);
			// イメージ
			Path = "data/map/";
			Path += MapPath;
			Path += "_img.bmp";
			GraphHandle::LoadDiv(Path, ChipNum, ChipNum, 1, 32, 32, &m_MapChip);
			// イメージ
			Path = "data/map/";
			Path += MapPath;
			Path += "_wal.bmp";
			GraphHandle::LoadDiv(Path, 5 * 2, 5, 2, 32, 32, &m_WallChip);
			// 
			struct ChipInfo {
				std::array<unsigned char,3> Color{};
				bool IsWall{ false };
			};
			std::vector<ChipInfo> ChipInfoDatas;
			{
				ChipInfoDatas.resize(ChipNum);
				for (int x = 0; x < ChipNum; x++) {
					int r, g, b;
					GetPixelSoftImage(PalImage, x, 0, &r, &g, &b, NULL);
					ChipInfoDatas.at(x).Color.at(0) = (unsigned char)r;
					ChipInfoDatas.at(x).Color.at(1) = (unsigned char)g;
					ChipInfoDatas.at(x).Color.at(2) = (unsigned char)b;
					GetPixelSoftImage(PalImage, x, 1, &r, &g, &b, NULL);
					ChipInfoDatas.at(x).IsWall = (r < 128);
				}
			}
			// 
			m_Blick.resize(xsize);
			for (int x = 0; x < xsize; x++) {
				for (int y = 0; y < ysize; y++) {
					m_Blick.at(x).emplace_back(std::make_shared<Blick>(x - xsize / 2, y - ysize / 2));
					auto& B = GetFloorData(x,y);
					// 
					int r, g, b;
					GetPixelSoftImage(MapImage, x, ysize - 1 - y, &r, &g, &b, NULL);
					for (auto& p : ChipInfoDatas) {
						if ((p.Color.at(0) == r) && (p.Color.at(1) == g) && (p.Color.at(2) == b)) {
							// 使用する素材を設定
							B->palletNum = (int)(&p - &ChipInfoDatas.front());
							B->ZRad = 0.f;
							for (int i = 0;i < 4;i++) {
								B->AddpalletNum.at(i) = -1;
								B->AddZRad.at(i) = 0.f;
							}
							B->IsWall = p.IsWall;
							break;
						}
					}
				}
			}
			for (auto& b : m_Blick) {
				for (auto& B : b) {
					int x = (int)(&b - &m_Blick.front());
					int y = (int)(&B - &b.front());

					std::array<bool, 4 + 4> LURN = { true,true,true,true,true,true,true,true };
					if (x != 0) { LURN.at(0) = GetFloorData(x - 1, y)->IsWall; }
					if (y != 0) { LURN.at(1) = GetFloorData(x, y - 1)->IsWall; }
					if (x != GetXSize() - 1) { LURN.at(2) = GetFloorData(x + 1, y)->IsWall; }
					if (y != GetYSize() - 1) { LURN.at(3) = GetFloorData(x, y + 1)->IsWall; }

					if (x != 0 && y != 0) { LURN.at(4) = GetFloorData(x - 1, y - 1)->IsWall; }
					if (x != GetXSize() - 1 && y != 0) { LURN.at(5) = GetFloorData(x + 1, y - 1)->IsWall; }
					if (x != GetXSize() - 1 && y != GetYSize() - 1) { LURN.at(6) = GetFloorData(x + 1, y + 1)->IsWall; }
					if (x != 0 && y != GetYSize() - 1) { LURN.at(7) = GetFloorData(x - 1, y + 1)->IsWall; }

					if (!LURN.at(0)) {
						B->LinkIndex.at(0) = GetXYToNum(x - 1, y);
					}
					if (!LURN.at(1)) {
						B->LinkIndex.at(1) = GetXYToNum(x, y - 1);
					}
					if (!LURN.at(2)) {
						B->LinkIndex.at(2) = GetXYToNum(x + 1, y);
					}
					if (!LURN.at(3)) {
						B->LinkIndex.at(3) = GetXYToNum(x, y + 1);
					}
					if (!B->IsWall) { continue; }
					int WayCount = 0;
					for (int i = 0; i < 4; i++) { if (LURN.at(i)) { WayCount++; } }
					switch (WayCount) {
					case 0:
						B->palletNum = 0;
						B->ZRad = 0.f;
						break;
					case 1:
						B->palletNum = 1;
						for (int i = 0; i < 4; i++) {
							if (LURN.at(i)) {
								B->ZRad = deg2rad(-90 * i + 90);
								break;
							}
						}
						break;
					case 2:
					{
						bool IsHit = false;
						for (int i = 0; i < 4; i++) {
							if (!LURN.at(i) && !LURN.at((i + 1) % 4)) {
								B->palletNum = 2 + 5;
								B->ZRad = deg2rad(-90 * i);
								IsHit = true;
								break;
							}
						}
						if (!IsHit) {
							for (int i = 0; i < 4; i++) {
								if (!LURN.at(i)) {
									B->palletNum = 2;
									B->ZRad = deg2rad(90 * i - 90);
									break;
								}
							}
						}
					}
					break;
					case 3:
						B->palletNum = 3;
						for (int i = 0; i < 4; i++) {
							if (!LURN.at(i)) {
								B->ZRad = deg2rad(-90 * i + 90);
								break;
							}
						}
						break;
					case 4:
						B->palletNum = 4;
						B->ZRad = 0.f;
						break;
					default:
						break;
					}

					for (int i = 0; i < 4; i++) {
						if (LURN.at(i) && LURN.at((i + 1) % 4) && !LURN.at((i + 4) % 8)) {
							B->AddpalletNum.at(i) = 9;
							B->AddZRad.at(i) = deg2rad(-90 * i);
						}
					}
				}
			}
			DeleteSoftImage(MapImage);
			DeleteSoftImage(PalImage);

			m_PointShadowHandle = GraphHandle::Make(y_r(1920.f * ShadowPer), y_r(1080.f * ShadowPer), true);
			m_AmbientShadowHandle = GraphHandle::Make(y_r(1920.f * ShadowPer), y_r(1080.f * ShadowPer), true);
		}
		// 
		void BackGroundClassBase::Execute(void) noexcept {}
		// 
		void BackGroundClassBase::SetupShadow(void) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				m_AmbientShadowHandle.SetDraw_Screen(false);
				{
					DrawBox_2D(0, 0, y_r(1920.f * ShadowPer), y_r(1080.f * ShadowPer), White, true);
					int Radius = GetDispSize(0.5f) + y_r(m_AmbientShadowLength);
					for (auto& b : m_Blick) {
						for (auto& B : b) {
							if (!B->IsWall) { continue; }
							auto DispPos = Convert2DtoDisp(B->m_Pos);
							if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
							int x = (int)(&b - &m_Blick.front());
							int y = (int)(&B - &b.front());
							DrawAmbientShadow(x, y);
						}
					}
				}
				GraphFilter(m_AmbientShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
			}
			m_PointShadowHandle.SetDraw_Screen(false);
			{
				DrawBox_2D(0, 0, y_r(1920.f * ShadowPer), y_r(1080.f * ShadowPer), White, true);
				int Radius = GetDispSize(0.5f) + y_r(255.f);
				for (auto& b : m_Blick) {
					for (auto& B : b) {
						if (!B->IsWall) { continue; }
						auto DispPos = Convert2DtoDisp(B->m_Pos);
						if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
						int x = (int)(&b - &m_Blick.front());
						int y = (int)(&B - &b.front());
						DrawPointShadow(x, y);
					}
				}
			}
			GraphFilter(m_PointShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
		}
		// 環境影を描画
		void BackGroundClassBase::DrawAmbientShadow(int x, int y) const noexcept {
			auto Draw = [&](const Vector3DX& postmp1, const Vector3DX& postmp2, float radsub) {
				if (std::cos(m_AmbientLightRad - radsub) > 0.f) {
					std::array<Vector3DX, 4> Position{ };
					float Radius = (float)y_r(m_AmbientShadowLength);
					Position.at(0).Set(postmp1.x + std::sin(m_AmbientLightRad) * Radius, postmp1.y + std::cos(m_AmbientLightRad) * Radius, 0.f);
					Position.at(1).Set(postmp2.x + std::sin(m_AmbientLightRad) * Radius, postmp2.y + std::cos(m_AmbientLightRad) * Radius, 0.f);
					Position.at(2) = postmp2;
					Position.at(3) = postmp1;

					float XMax = -1000000.f;
					float XMin = 1000000.f;
					float YMax = -1000000.f;
					float YMin = 1000000.f;
					int xmin = -1;
					int xmax = -1;
					int ymin = -1;
					int ymax = -1;
					for (int i = 0; i < 4; i++) {
						auto& p = Position.at(i);
						if (XMax <= p.x) {
							XMax = p.x;
							xmax = i;
						}
						if (XMin >= p.x) {
							XMin = p.x;
							xmin = i;
						}

						if (YMax <= p.y) {
							YMax = p.y;
							ymax = i;
						}
						if (YMin >= p.y) {
							YMin = p.y;
							ymin = i;
						}
					}
					// 
					XMax = -1000000.f;
					XMin = 1000000.f;
					YMax = -1000000.f;
					YMin = 1000000.f;
					int x2min = -1;
					int x2max = -1;
					int y2min = -1;
					int y2max = -1;
					for (int i = 0; i < 4; i++) {
						auto& p = Position.at(i);
						if (XMax <= p.x) {
							if (xmax != i) {
								XMax = p.x;
								x2max = i;
							}
						}
						if (XMin >= p.x) {
							if (xmin != i) {
								XMin = p.x;
								x2min = i;
							}
						}

						if (YMax <= p.y) {
							if (ymax != i) {
								YMax = p.y;
								y2max = i;
							}
						}
						if (YMin >= p.y) {
							if (ymin != i) {
								YMin = p.y;
								y2min = i;
							}
						}
					}
					Position.at(xmin).x -= 3;
					Position.at(ymin).y -= 3;
					Position.at(xmax).x += 3;
					Position.at(ymax).y += 3;

					if (x2min != -1) { Position.at(x2min).x -= 3; }
					if (y2min != -1) { Position.at(y2min).y -= 3; }
					if (x2max != -1) { Position.at(x2max).x += 3; }
					if (y2max != -1) { Position.at(y2max).y += 3; }

					Position.at(0) *= ShadowPer;
					Position.at(1) *= ShadowPer;
					Position.at(2) *= ShadowPer;
					Position.at(3) *= ShadowPer;
					DrawModiGraph(
						(int)Position.at(0).x, (int)Position.at(0).y,
						(int)Position.at(1).x, (int)Position.at(1).y,
						(int)Position.at(2).x, (int)Position.at(2).y,
						(int)Position.at(3).x, (int)Position.at(3).y,
						m_MapChip.at(0).get(), FALSE);
				}
				};
			for (int i = 0; i < 4; i++) {
				Draw(
					Convert2DtoDisp(GetFloorData(x, y)->BoxSide.at(i)),
					Convert2DtoDisp(GetFloorData(x, y)->BoxSide.at((i + 1) % 4)),
					deg2rad(90 * i));
			}
		}
		// ポイント影を描画
		void BackGroundClassBase::DrawPointShadow(int x, int y) noexcept {
			auto Draw = [&](const Vector3DX& postmp1, const Vector3DX& postmp2, float radsub) {
				if (std::cos(GetRadVec2Vec((postmp1 + postmp2) / 2.f, m_PointLightPos) - radsub) > 0.f) {
					auto length = 1920.f;
					float lightrad1 = GetRadVec2Vec(postmp1, m_PointLightPos);
					float lightrad2 = GetRadVec2Vec(postmp2, m_PointLightPos);
					std::array<Vector3DX,4> Position{ };

					Position.at(0).Set(postmp1.x + std::sin(lightrad1) * length, postmp1.y + std::cos(lightrad1) * length, 0.f);
					Position.at(1).Set(postmp2.x + std::sin(lightrad2) * length, postmp2.y + std::cos(lightrad2) * length, 0.f);
					Position.at(2) = postmp2;
					Position.at(3) = postmp1;

					float XMax = -1000000.f;
					float XMin = 1000000.f;
					float YMax = -1000000.f;
					float YMin = 1000000.f;
					int xmin = -1;
					int xmax = -1;
					int ymin = -1;
					int ymax = -1;
					for (int i = 0;i < 4;i++) {
						auto& p = Position.at(i);
						if (XMax <= p.x) {
							XMax = p.x;
							xmax = i;
						}
						if (XMin >= p.x) {
							XMin = p.x;
							xmin = i;
						}

						if (YMax <= p.y) {
							YMax = p.y;
							ymax = i;
						}
						if (YMin >= p.y) {
							YMin = p.y;
							ymin = i;
						}
					}
					Position.at(xmin).x -= 3;
					Position.at(ymin).y -= 3;
					Position.at(xmax).x += 3;
					Position.at(ymax).y += 3;

					Position.at(0) *= ShadowPer;
					Position.at(1) *= ShadowPer;
					Position.at(2) *= ShadowPer;
					Position.at(3) *= ShadowPer;
					DrawModiGraph(
						(int)Position.at(0).x, (int)Position.at(0).y,
						(int)Position.at(1).x, (int)Position.at(1).y,
						(int)Position.at(2).x, (int)Position.at(2).y,
						(int)Position.at(3).x, (int)Position.at(3).y,
						m_MapChip.at(0).get(), FALSE);
				}
			};
			for (int i = 0; i < 4; i++) {
				Draw(
					Convert2DtoDisp(GetFloorData(x, y)->BoxSide.at(i)),
					Convert2DtoDisp(GetFloorData(x, y)->BoxSide.at((i + 1) % 4)),
					deg2rad(90 * i));
			}
		}
		// 
		void BackGroundClassBase::Draw(void) noexcept {
			int Radius = GetDispSize(0.5f);
			float Size = (float)(Radius * 2.f) / (32 - 1);
			// 床
			for (auto& b : m_Blick) {
				for (auto& B : b) {
					if (B->IsWall) { continue; }
					auto DispPos = Convert2DtoDisp(B->m_Pos);
					if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
					m_MapChip.at(B->palletNum).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, B->ZRad, FALSE);
				}
			}
			// 影
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
				m_AmbientShadowHandle.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), false);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
		void BackGroundClassBase::DrawFront(void) noexcept {
			int Radius = GetDispSize(0.5f);
			float Size = (float)(Radius * 2.f) / (32 - 1);
			// 
			{
				auto MinPos = Convert2DtoDisp(m_Blick.front().front()->m_Pos);
				auto MaxPos = Convert2DtoDisp(m_Blick.back().back()->m_Pos);
				int xmin = (int)MinPos.x;
				int ymin = (int)MaxPos.y;
				int xmax = (int)MaxPos.x;
				int ymax = (int)MinPos.y;
				auto DrawBE = [](int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int IsFill) {
					if (p2x < p1x || p2y < p1y) { return; }
					DrawBox(p1x, p1y, p2x, p2y, color, IsFill);
				};
				DrawBE(0, 0, xmin, ymax, Black, TRUE);
				DrawBE(xmin, 0, y_r(1920), ymin, Black, TRUE);
				DrawBE(xmax, ymin, y_r(1920), y_r(1080), Black, TRUE);
				DrawBE(0, ymax, xmax, y_r(1080), Black, TRUE);
			}
			// 壁
			for (auto& b : m_Blick) {
				for (auto& B : b) {
					if (!B->IsWall) { continue; }
					auto DispPos = Convert2DtoDisp(B->m_Pos);
					if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
					m_WallChip.at(B->palletNum).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, B->ZRad, FALSE);
					for (int i = 0; i < 4; i++) {
						if (B->AddpalletNum.at(i) != -1) {
							m_WallChip.at(B->AddpalletNum.at(i)).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, B->AddZRad.at(i), TRUE);
						}
					}
				}
			}
			// デバッグ描画
			/*
			for (auto& w : WallList) {
				{
					auto DispPos1 = Convert2DtoDisp(w.Position.at(0));
					auto DispPos2 = Convert2DtoDisp(w.Position.at(1));
					DrawLine(
						(int)(DispPos1.x), (int)(DispPos1.y),
						(int)(DispPos2.x), (int)(DispPos2.y),
						Green, 10);
				}
				{
					auto DispPos1 = Convert2DtoDisp((w.Position.at(0) + w.Position.at(1)) / 2.f);
					auto DispPos2 = Convert2DtoDisp((w.Position.at(0) + w.Position.at(1)) / 2.f + w.Normal * 10.f);
					DrawLine(
						(int)(DispPos1.x), (int)(DispPos1.y),
						(int)(DispPos2.x), (int)(DispPos2.y),
						Green, 10);
				}
			}
			//*/
		}
		// 
		void BackGroundClassBase::Dispose(void) noexcept {
			for (auto& b : m_Blick) {
				for (auto& B : b) {
					B.reset();
				}
				b.clear();
			}
			m_Blick.clear();
			for (auto& g : m_MapChip) {
				g.Dispose();
			}
			m_MapChip.clear();
			for (auto& g : m_WallChip) {
				g.Dispose();
			}
			m_WallChip.clear();
		}
	};
};

