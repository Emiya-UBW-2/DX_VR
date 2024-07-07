#include	"BackGround.hpp"

const FPS_n2::Sceneclass::BackGroundClassBase* SingletonBase<FPS_n2::Sceneclass::BackGroundClassBase>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		const float BackGroundClassBase::CheckHideShadow(const Vector3DX & PosA, const Vector3DX & PosB, float Radius) noexcept {
			float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));

			float Ret = 1.f;
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					//壁ではない
					if (!y->IsWall) { continue; }
					//辺を登録
					Vector3DX Box[4];
					Box[0].Set(y->m_Pos.x - HalfLen, y->m_Pos.y - HalfLen, 0.f);
					Box[1].Set(y->m_Pos.x + HalfLen, y->m_Pos.y - HalfLen, 0.f);
					Box[2].Set(y->m_Pos.x + HalfLen, y->m_Pos.y + HalfLen, 0.f);
					Box[3].Set(y->m_Pos.x - HalfLen, y->m_Pos.y + HalfLen, 0.f);
					auto GetPer = [&]() {
						if (GetHitCapsuleToTriangle(PosA, PosB, Radius, Box[0], Box[1], Box[3]) ||
							GetHitCapsuleToTriangle(PosA, PosB, Radius, Box[1], Box[2], Box[3])) {

							if (GetHitCapsuleToTriangle(PosA, PosB, 1.f, Box[0], Box[1], Box[3]) ||
								GetHitCapsuleToTriangle(PosA, PosB, 1.f, Box[1], Box[2], Box[3])) {
								return 0.0f;
							}

							SEGMENT_SEGMENT_RESULT Result;
							float P = 2.f;
							for (int i = 0;i < 4;i++) {
								VECTOR Pos1t = Box[i].get();
								VECTOR Pos2t = Box[(i + 1) % 4].get();
								VECTOR PosAt = PosA.get();
								VECTOR PosBt = PosB.get();
								Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, &Result);
								float ret = (std::sqrt(Result.SegA_SegB_MinDist_Square) / Radius);
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
		//壁判定ユニバーサル
		const bool BackGroundClassBase::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) noexcept {
			Vector3DX EndPt = *EndPos; EndPt.z = 0.f;
			Vector3DX StartPt = StartPos; StartPt.z = 0.f;
			Vector3DX VecPt = EndPt - StartPt;
			float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));

			WallList.clear();// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			Vector3DX Min, Max;
			Min.x = std::min(EndPt.x, StartPt.x) - (Radius * 6);
			Min.y = std::min(EndPt.y, StartPt.y) - (Radius * 6);
			Max.x = std::max(EndPt.x, StartPt.x) + (Radius * 6);
			Max.y = std::max(EndPt.y, StartPt.y) + (Radius * 6);
			//線のリストを出す
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					//壁ではない
					if (!y->IsWall) { continue; }
					//範囲外
					Vector3DX bMin, bMax;
					bMin.x = y->m_Pos.x - HalfLen;
					bMin.y = y->m_Pos.y - HalfLen;
					bMax.x = y->m_Pos.x + HalfLen;
					bMax.y = y->m_Pos.y + HalfLen;
					if (!((Min.x < bMin.x && bMax.x < Max.x) && (Min.y < bMin.y && bMax.y < Max.y))) { continue; }
					//辺を登録
					Vector3DX Box[4];
					Box[0].Set(bMin.x, bMin.y, 0.f);
					Box[1].Set(bMax.x, bMin.y, 0.f);
					Box[2].Set(bMax.x, bMax.y, 0.f);
					Box[3].Set(bMin.x, bMax.y, 0.f);

					for (int i = 0; i < 4; i++) {
						Vector3DX StartP = Box[i];
						Vector3DX EndP = Box[(i + 1) % 4];
						//完全に重なっているものは消す
						bool IsSame = false;
						for (auto& w : WallList) {
							if (
								(((w.Position[0] - StartP).sqrMagnitude() <= 0.00001f) && ((w.Position[1] - EndP).sqrMagnitude() <= 0.00001f)) ||
								(((w.Position[1] - StartP).sqrMagnitude() <= 0.00001f) && ((w.Position[0] - EndP).sqrMagnitude() <= 0.00001f))
								) {
								IsSame = true;
								w.canuse = false;
							}
						}
						if (!IsSame) {
							WallList.resize(WallList.size() + 1);
							WallList.back().Position[0] = StartP;
							WallList.back().Position[1] = EndP;
							WallList.back().Position[2] = Box[(i + 2) % 4];
							WallList.back().CalcNormal();
							WallList.back().canuse = true;
						}
					}
				}
			}
			//90度のへこみの場合はならす
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
						if ((w.Position[0] - w2.Position[0]).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position[0];
							pCenter2 = &w2.Position[0];
							pOther1 = &w.Position[1];
							pOther2 = &w2.Position[1];
						}
						else if ((w.Position[1] - w2.Position[0]).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position[1];
							pCenter2 = &w2.Position[0];
							pOther1 = &w.Position[0];
							pOther2 = &w2.Position[1];
						}
						else if ((w.Position[0] - w2.Position[1]).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position[0];
							pCenter2 = &w2.Position[1];
							pOther1 = &w.Position[1];
							pOther2 = &w2.Position[0];
						}
						else if ((w.Position[1] - w2.Position[1]).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.Position[1];
							pCenter2 = &w2.Position[1];
							pOther1 = &w.Position[0];
							pOther2 = &w2.Position[0];
						}
						else { continue; }

						Vector3DX Ct = (*pOther1 + *pOther2) / 2.f;

						if (Vector3DX::Dot(*pOther1 - *pCenter1, *pOther2 - *pCenter2) < 0.f) { continue; }//鋭角でない

						if (
							(Vector3DX::Dot(w.Normal, *pOther2 - *pCenter2) < 0.f) &&
							(Vector3DX::Dot(w2.Normal, *pOther1 - *pCenter1) < 0.f)
							) {
							continue;
						}//法線が内向きでない

									//へこみを減らす
						*pCenter1 = Ct;
						*pCenter2 = Ct;
						//*pCenter1 = (*pCenter1 + Ct) / 2.f;
						//*pCenter2 = (*pCenter2 + Ct) / 2.f;

						//ノーマル再演算
						w.CalcNormal();
						w2.CalcNormal();
					}
				}
			}
			//外向きなのでノーヒット
			for (auto& w : WallList) {
				if (!w.canuse) { continue; }
				Vector3DX StartP = w.Position[0];
				Vector3DX EndP = w.Position[1];
				if (Vector3DX::Dot(Vector3DX::Cross(EndP - StartP, VecPt), Vector3DX::Cross(EndP - StartP, w.Normal * -1.f)) < 0.f) {
					w.canuse = false;
				}
			}
			//使わないものを削除
			for (int i = 0; i < WallList.size(); i++) {
				if (!WallList[i].canuse) {
					std::swap(WallList[i], WallList.back());
					WallList.pop_back();
					i--;
				}
			}
			bool HitFlag = false;
			// 壁ポリゴンとの当たり判定処理
			if (WallList.size() > 0) {
				for (auto& w : WallList) {
					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (GetHitCheckToCapsule(StartPt, EndPt, Radius, w.Position[0], w.Position[1], 0.f)) {
						HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
						if (VecPt.magnitude() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							EndPt = StartPt + Vector3DX::Cross(w.Normal, Vector3DX::Cross(VecPt, w.Normal));
							bool IsHit = false;
							for (auto& w2 : WallList) {
								if (GetHitCheckToCapsule(StartPt, EndPt, Radius, w2.Position[0], w2.Position[1], 0.f)) {
									IsHit = true;
									break;
								}
							}
							if (!IsHit) {
								HitFlag = false;
								break;//どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
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
							if (!GetHitCheckToCapsule(StartPt, EndPt, Radius, w.Position[0], w.Position[1], 0.f)) { continue; }
							EndPt += w.Normal * 0.0015f;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							bool IsHit = false;
							for (auto& w2 : WallList) {
								if (GetHitCheckToCapsule(StartPt, EndPt, Radius, w2.Position[0], w2.Position[1], 0.f)) {// 当たっていたらループを抜ける
									IsHit = true;
									break;
								}
							}
							if (!IsHit) {// 全てのポリゴンと当たっていなかったらここでループ終了
								break;
							}
							HitF = true;
						}
						if (!HitF) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
							break;
						}
					}
				}
				//WallList.clear();
			}
			*EndPos = EndPt;
			return HitFlag;
		}

		void BackGroundClassBase::Init(const char* MapPath) noexcept {
			int xsize = 64;
			int ysize = 64;
			int r, g, b;
			std::string Path;
			//マップ
			Path = "data/map/";
			Path += MapPath;
			Path += "_col.bmp";
			int MapImage = LoadSoftImage(Path.c_str());
			GetSoftImageSize(MapImage, &xsize, &ysize);
			//パレット
			Path = "data/map/";
			Path += MapPath;
			Path += "_chp.bmp";
			int PalImage = LoadSoftImage(Path.c_str());
			int ChipNum = 12;
			GetSoftImageSize(PalImage, &ChipNum, NULL);
			//イメージ
			Path = "data/map/";
			Path += MapPath;
			Path += "_img.bmp";
			GraphHandle::LoadDiv(Path, ChipNum, ChipNum, 1, 32, 32, &m_MapChip);
			//イメージ
			Path = "data/map/";
			Path += MapPath;
			Path += "_wal.bmp";
			GraphHandle::LoadDiv(Path, 5 * 2, 5, 2, 32, 32, &m_WallChip);
			//
			struct ChipInfo {
				unsigned char Color[3]{};
				int IsWall{0};
			};
			std::vector<ChipInfo> ChipInfoDatas;
			{
				ChipInfoDatas.resize(ChipNum);
				for (int x = 0; x < ChipNum; x++) {
					GetPixelSoftImage(PalImage, x, 0, &r, &g, &b, NULL);
					ChipInfoDatas.at(x).Color[0] = (unsigned char)r;
					ChipInfoDatas.at(x).Color[1] = (unsigned char)g;
					ChipInfoDatas.at(x).Color[2] = (unsigned char)b;
					GetPixelSoftImage(PalImage, x, 1, &r, &g, &b, NULL);
					ChipInfoDatas.at(x).IsWall = (r < 128);
				}
			}
			//
			m_Blick.resize(xsize);
			{
				int x = 0;
				for (auto& B : m_Blick) {
					for (int y = 0; y < ysize; y++) {
						B.emplace_back(std::make_shared<Blick>(x - xsize / 2, y - ysize / 2));
						//
						GetPixelSoftImage(MapImage, x, ysize - 1 - y, &r, &g, &b, NULL);
						for (auto& p : ChipInfoDatas) {
							if ((p.Color[0] == r) && (p.Color[1] == g) && (p.Color[2] == b)) {
								//使用する素材を設定
								B.back()->palletNum = (int)(&p - &ChipInfoDatas.front());
								B.back()->ZRad = 0.f;
								for (int i = 0;i < 4;i++) {
									B.back()->AddpalletNum[i] = -1;
									B.back()->AddZRad[i] = 0.f;
								}
								B.back()->IsWall = p.IsWall;
								break;
							}
						}
						//
						B.back()->m_Pos = B.back()->m_Pos * ((64.f * 64.f) / (1080 / 2));
					}
					x++;
				}
			}
			for (int x = 0; x < xsize; x++) {
				for (int y = 0; y < ysize; y++) {
					auto& B = m_Blick.at(x).at(y);
					if (B->IsWall) {
						bool LURN[4 + 4] = {true,true,true,true,true,true,true,true};
						if (x != 0) { LURN[0] = m_Blick.at(x - 1).at(y)->IsWall; }
						if (y != 0) { LURN[1] = m_Blick.at(x).at(y - 1)->IsWall; }
						if (x != xsize - 1) { LURN[2] = m_Blick.at(x + 1).at(y)->IsWall; }
						if (y != ysize - 1) { LURN[3] = m_Blick.at(x).at(y + 1)->IsWall; }

						if (x != 0 && y != 0) { LURN[4] = m_Blick.at(x - 1).at(y - 1)->IsWall; }
						if (x != xsize - 1 && y != 0) { LURN[5] = m_Blick.at(x + 1).at(y - 1)->IsWall; }
						if (x != xsize - 1 && y != ysize - 1) { LURN[6] = m_Blick.at(x + 1).at(y + 1)->IsWall; }
						if (x != 0 && y != ysize - 1) { LURN[7] = m_Blick.at(x - 1).at(y + 1)->IsWall; }

						int count = 0;
						for (int i = 0;i < 4;i++) { if (LURN[i]) { count++; } }
						switch (count) {
							case 0:
								B->palletNum = 0;
								B->ZRad = 0.f;
								break;
							case 1:
								B->palletNum = 1;
								for (int i = 0;i < 4;i++) {
									if (LURN[i]) {
										B->ZRad = deg2rad(-90 * i + 90);
									}
								}
								break;
							case 2:
								{
									bool IsHit = false;
									for (int i = 0;i < 4;i++) {
										if (!LURN[i]) {
											if (!LURN[(i + 1) % 4]) {
												B->palletNum = 2 + 5;
												B->ZRad = deg2rad(-90 * i);
												IsHit = true;
												break;
											}
										}
									}
									if (!IsHit) {
										for (int i = 0;i < 4;i++) {
											if (!LURN[i]) {
												B->palletNum = 2;
												B->ZRad = deg2rad(90 * i - 90);
											}
										}
									}
								}
								break;
							case 3:
								B->palletNum = 3;
								for (int i = 0;i < 4;i++) {
									if (!LURN[i]) {
										B->ZRad = deg2rad(-90 * i + 90);
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

						for (int i = 0;i < 4;i++) {
							if (LURN[i] && LURN[(i + 1) % 4] && !LURN[(i + 0 + 4) % 8]) {
								B->AddpalletNum[i] = 9;
								B->AddZRad[i] = deg2rad(-90 * i);
							}
						}
					}
				}
			}
			DeleteSoftImage(MapImage);
			DeleteSoftImage(PalImage);

			m_ShadowHandle = GraphHandle::Make(y_r(1920.f * ShadowPer), y_r(1080.f * ShadowPer), false);
		}
		//
		void BackGroundClassBase::Execute(void) noexcept {}
		//
		void BackGroundClassBase::SetupShadow(void) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) { return; }
			m_ShadowHandle.SetDraw_Screen(false);
			{
				DrawBox_2D(0, 0, y_r(1920.f * ShadowPer), y_r(1080.f * ShadowPer), White, true);

				auto& CamPos = Cam2DControl::Instance()->GetCamPos();
				float Radius = (float)y_r(64.f * 64.f / CamPos.z) / 2;

				for (int x = 0; x < m_Blick.size(); x++) {
					for (int y = 0; y < m_Blick.back().size(); y++) {
						auto DispPos = Convert2DtoDisp(m_Blick.at(x).at(y)->m_Pos);
						if (
							(DispPos.x < -(Radius + m_AmbientShadowLength) || y_r(1920) + (Radius + m_AmbientShadowLength) < DispPos.x) ||
							(DispPos.y < -(Radius + m_AmbientShadowLength) || y_r(1080) + (Radius + m_AmbientShadowLength) < DispPos.y)
							) {
							continue;
						}
						if (m_Blick.at(x).at(y)->IsWall) {
							DrawAmbientShadow(x, y);
						}
					}
				}
				for (int x = 0; x < m_Blick.size(); x++) {
					for (int y = 0; y < m_Blick.back().size(); y++) {
						auto DispPos = Convert2DtoDisp(m_Blick.at(x).at(y)->m_Pos);
						if (
							(DispPos.x < -Radius || y_r(1920) + Radius < DispPos.x) ||
							(DispPos.y < -Radius || y_r(1080) + Radius < DispPos.y)
							) {
							continue;
						}
						if (m_Blick.at(x).at(y)->IsWall) {
							DrawPointShadow(x, y);
						}
					}
				}
			}
			GraphFilter(m_ShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
		}
		//
		//座標変換
		const void		BackGroundClassBase::Get2DPos(int x, int y, Vector3DX* Min, Vector3DX* Center, Vector3DX* Max) const noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			float Radius = (float)y_r(64.f * 64.f / CamPos.z) / 2;
			auto DispPos = Convert2DtoDisp(m_Blick.at(x).at(y)->m_Pos);
			Min->x = DispPos.x - Radius;
			Min->y = DispPos.y + Radius;
			*Center = DispPos;
			Max->x = DispPos.x + Radius;
			Max->y = DispPos.y - Radius;
		}
		//環境影を描画
		void BackGroundClassBase::DrawAmbientShadow(int x, int y) const noexcept {
			auto Draw = [&](const Vector3DX& postmp1, const Vector3DX& postmp2, float radsub) {
				if (std::cos(m_AmbientLightRad - radsub) > 0.f) {
					Vector3DX Position[4]{ };

					Position[0].Set(postmp1.x + std::sin(m_AmbientLightRad) * m_AmbientShadowLength, postmp1.y + std::cos(m_AmbientLightRad) * m_AmbientShadowLength, 0.f);
					Position[1].Set(postmp2.x + std::sin(m_AmbientLightRad) * m_AmbientShadowLength, postmp2.y + std::cos(m_AmbientLightRad) * m_AmbientShadowLength, 0.f);
					Position[2] = postmp2;
					Position[3] = postmp1;

					float XMax = -1000000.f;
					float XMin = 1000000.f;
					float YMax = -1000000.f;
					float YMin = 1000000.f;
					int xmin = -1;
					int xmax = -1;
					int ymin = -1;
					int ymax = -1;
					for (int i = 0;i < 4;i++) {
						auto& p = Position[i];
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
					for (int i = 0;i < 4;i++) {
						auto& p = Position[i];
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
					Position[xmin].x -= 3;
					Position[ymin].y -= 3;
					Position[xmax].x += 3;
					Position[ymax].y += 3;

					if (x2min != -1) { Position[x2min].x -= 3; }
					if (y2min != -1) { Position[y2min].y -= 3; }
					if (x2max != -1) { Position[x2max].x += 3; }
					if (y2max != -1) { Position[y2max].y += 3; }

					DrawModiGraph(
						(int)(Position[0].x * ShadowPer), (int)(Position[0].y * ShadowPer),
						(int)(Position[1].x * ShadowPer), (int)(Position[1].y * ShadowPer),
						(int)(Position[2].x * ShadowPer), (int)(Position[2].y * ShadowPer),
						(int)(Position[3].x * ShadowPer), (int)(Position[3].y * ShadowPer),
						m_MapChip.at(0).get(), FALSE);
				}
			};
			Vector3DX Min, Center, Max;
			//左
			if (x != 0) {
				Get2DPos(x - 1, y, &Min, &Center, &Max);
				Draw(VGet(Max.x, Min.y, 0.f), VGet(Max.x, Max.y, 0.f), deg2rad(270.f));
			}
			//上
			if (y != 0) {
				Get2DPos(x, y - 1, &Min, &Center, &Max);
				Draw(VGet(Max.x, Max.y, 0.f), VGet(Min.x, Max.y, 0.f), deg2rad(0.f));
			}
			//右
			if (x != m_Blick.size() - 1) {
				Get2DPos(x + 1, y, &Min, &Center, &Max);
				Draw(VGet(Min.x, Max.y, 0.f), VGet(Min.x, Min.y, 0.f), deg2rad(90.f));
			}
			//下
			if (y != m_Blick.back().size() - 1) {
				Get2DPos(x, y + 1, &Min, &Center, &Max);
				Draw(VGet(Min.x, Min.y, 0.f), VGet(Max.x, Min.y, 0.f), deg2rad(180.f));
			}
		}
		//ポイント影を描画
		void BackGroundClassBase::DrawPointShadow(int x, int y) noexcept {
			auto DispPos = Convert2DtoDisp(m_Blick.at(x).at(y)->m_Pos);
			auto Draw = [&](const Vector3DX& sidePos, const Vector3DX& postmp1, const Vector3DX& postmp2, float radsub) {
				if (std::cos(GetRadVec2Vec(sidePos, m_PointLightPos) - radsub) > 0.f) {
					auto length = 1920.f;
					float lightrad1 = GetRadVec2Vec(postmp1, m_PointLightPos);
					float lightrad2 = GetRadVec2Vec(postmp2, m_PointLightPos);
					Vector3DX Position[4]{ };

					Position[0].Set(postmp1.x + std::sin(lightrad1) * length, postmp1.y + std::cos(lightrad1) * length, 0.f);
					Position[1].Set(postmp2.x + std::sin(lightrad2) * length, postmp2.y + std::cos(lightrad2) * length, 0.f);
					Position[2] = postmp2;
					Position[3] = postmp1;

					float XMax = -1000000.f;
					float XMin = 1000000.f;
					float YMax = -1000000.f;
					float YMin = 1000000.f;
					int xmin = -1;
					int xmax = -1;
					int ymin = -1;
					int ymax = -1;
					for (int i = 0;i < 4;i++) {
						auto& p = Position[i];
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
					Position[xmin].x -= 3;
					Position[ymin].y -= 3;
					Position[xmax].x += 3;
					Position[ymax].y += 3;

					DrawModiGraph(
						(int)(Position[0].x * ShadowPer), (int)(Position[0].y * ShadowPer),
						(int)(Position[1].x * ShadowPer), (int)(Position[1].y * ShadowPer),
						(int)(Position[2].x * ShadowPer), (int)(Position[2].y * ShadowPer),
						(int)(Position[3].x * ShadowPer), (int)(Position[3].y * ShadowPer),
						m_MapChip.at(0).get(), FALSE);
				}
			};
			Vector3DX Min, Center, Max;
			//左
			if (x != 0) {
				Get2DPos(x - 1, y, &Min, &Center, &Max);
				Draw(Center, VGet(Max.x, Min.y, 0.f), VGet(Max.x, Max.y, 0.f), deg2rad(270.f));
			}
			//上
			if (y != 0) {
				Get2DPos(x, y - 1, &Min, &Center, &Max);
				Draw(Center, VGet(Max.x, Max.y, 0.f), VGet(Min.x, Max.y, 0.f), deg2rad(0.f));
			}
			//右
			if (x != m_Blick.size() - 1) {
				Get2DPos(x + 1, y, &Min, &Center, &Max);
				Draw(Center, VGet(Min.x, Max.y, 0.f), VGet(Min.x, Min.y, 0.f), deg2rad(90.f));
			}
			//下
			if (y != m_Blick.back().size() - 1) {
				Get2DPos(x, y + 1, &Min, &Center, &Max);
				Draw(Center, VGet(Min.x, Min.y, 0.f), VGet(Max.x, Min.y, 0.f), deg2rad(180.f));
			}
		}
		//
		void BackGroundClassBase::Draw(void) noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			float Radius = (float)y_r(64.f * 64.f / CamPos.z) / 2;
			float Size = (float)(Radius * 2.f) / (32 - 1);
			//床
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					auto DispPos = Convert2DtoDisp(y->m_Pos);
					if (
						(DispPos.x < -Radius || y_r(1920) + Radius < DispPos.x) ||
						(DispPos.y < -Radius || y_r(1080) + Radius < DispPos.y)
						) {
						continue;
					}
					if (!y->IsWall) {
						m_MapChip.at(y->palletNum).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, y->ZRad, FALSE);
					}
				}
			}
			//影
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
				m_ShadowHandle.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), false);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
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
			//壁
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					auto DispPos = Convert2DtoDisp(y->m_Pos);
					if (
						(DispPos.x < -Radius || y_r(1920) + Radius < DispPos.x) ||
						(DispPos.y < -Radius || y_r(1080) + Radius < DispPos.y)
						) {
						continue;
					}
					if (y->IsWall) {
						m_WallChip.at(y->palletNum).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, y->ZRad, FALSE);
						for (int i = 0;i < 4;i++) {
							if (y->AddpalletNum[i] != -1) {
								m_WallChip.at(y->AddpalletNum[i]).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, y->AddZRad[i], TRUE);
							}
						}
					}
				}
			}
			//デバッグ描画
			/*
			for (auto& w : WallList) {
				{
					auto DispPos1 = Convert2DtoDisp(w.Position[0]);
					auto DispPos2 = Convert2DtoDisp(w.Position[1]);
					DrawLine(
						(int)(DispPos1.x), (int)(DispPos1.y),
						(int)(DispPos2.x), (int)(DispPos2.y),
						Green, 10);
				}
				{
					auto DispPos1 = Convert2DtoDisp((w.Position[0] + w.Position[1]) / 2.f);
					auto DispPos2 = Convert2DtoDisp((w.Position[0] + w.Position[1]) / 2.f + w.Normal * 10.f);
					DrawLine(
						(int)(DispPos1.x), (int)(DispPos1.y),
						(int)(DispPos2.x), (int)(DispPos2.y),
						Green, 10);
				}
			}
			//*/
		}
		void BackGroundClassBase::DrawFront(void) noexcept {}
		//
		void BackGroundClassBase::Dispose(void) noexcept {
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					y.reset();
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

