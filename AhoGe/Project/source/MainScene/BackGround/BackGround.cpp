#include	"BackGround.hpp"

const FPS_n2::Sceneclass::BackGroundClassBase* SingletonBase<FPS_n2::Sceneclass::BackGroundClassBase>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		// 影を描画
		void DrawModi(std::array<Vector3DX, 4>& Position, int Add, const GraphHandle& ShadowChip) noexcept {
			//前後左右に太らせる
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
			Position.at(xmin).x -= Add;
			Position.at(ymin).y -= Add;
			Position.at(xmax).x += Add;
			Position.at(ymax).y += Add;

			if (x2min != -1) { Position.at(x2min).x -= Add; }
			if (y2min != -1) { Position.at(y2min).y -= Add; }
			if (x2max != -1) { Position.at(x2max).x += Add; }
			if (y2max != -1) { Position.at(y2max).y += Add; }

			DrawModiGraph(
				(int)Position.at(0).x, (int)Position.at(0).y,
				(int)Position.at(1).x, (int)Position.at(1).y,
				(int)Position.at(2).x, (int)Position.at(2).y,
				(int)Position.at(3).x, (int)Position.at(3).y,
				ShadowChip.get(), FALSE);
		}
		void BackGroundClassBase::Blick::DrawAmbientShadow(const Vector3DX& AmbientLightVec, float AmbientLightRad, const GraphHandle& ShadowChip) const noexcept {
			std::array<Vector3DX, 4> Position{ };
			for (int i = 0; i < 4; i++) {
				Vector3DX postmp1 = Convert2DtoDisp(this->GetBoxSide(i));
				Vector3DX postmp2 = Convert2DtoDisp(this->GetBoxSide((i + 1) % 4));
				float radsub = deg2rad(90 * i);
				if (std::cos(AmbientLightRad - radsub) > 0.f) {
					Position.at(0).Set(postmp1.x + AmbientLightVec.x, postmp1.y + AmbientLightVec.y, 0.f);
					Position.at(1).Set(postmp2.x + AmbientLightVec.x, postmp2.y + AmbientLightVec.y, 0.f);
					Position.at(2) = postmp2;
					Position.at(3) = postmp1;
					DrawModi(Position, 3, ShadowChip);
				}
			}
		}
		void BackGroundClassBase::Blick::DrawPointShadow(const Vector3DX& PointLightPos, const GraphHandle& ShadowChip) noexcept {
			std::array<Vector3DX, 4> Position{ };
			for (int i = 0; i < 4; i++) {
				Vector3DX postmp1 = Convert2DtoDisp(this->GetBoxSide(i));
				Vector3DX postmp2 = Convert2DtoDisp(this->GetBoxSide((i + 1) % 4));
				float radsub = deg2rad(90 * i);
				if (std::cos(GetRadVec2Vec((postmp1 + postmp2) / 2.f, PointLightPos) - radsub) > 0.f) {
					auto length = 1920.f;
					float lightrad1 = GetRadVec2Vec(postmp1, PointLightPos);
					float lightrad2 = GetRadVec2Vec(postmp2, PointLightPos);

					Position.at(0).Set(postmp1.x + std::sin(lightrad1) * length, postmp1.y + std::cos(lightrad1) * length, 0.f);
					Position.at(1).Set(postmp2.x + std::sin(lightrad2) * length, postmp2.y + std::cos(lightrad2) * length, 0.f);
					Position.at(2) = postmp2;
					Position.at(3) = postmp1;

					DrawModi(Position, 3, ShadowChip);
				}
			}
		}
		// 線分の間で壁に当たっているか
		const float BackGroundClassBase::CheckHideShadow(const Vector3DX& PosA, const Vector3DX& PosB, float Radius) noexcept {
			float Ret = 1.f;
			for (auto& B : m_Blick) {
				// 壁ではない
				if (!B->GetIsCheckWall()) { continue; }
				// 範囲外
				auto DispPos = Convert2DtoDisp(B->GetPos());
				if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, (int)-Radius, (int)-Radius, (int)(y_r(1920) + Radius), (int)(y_r(1080) + Radius))) { continue; }
				//ヒットするかとヒットした場合の透明度を指定
				float tmp = 1.f;
				if (B->CheckCapsuleHit(PosA, PosB, Radius)) {
					tmp = 0.f;
					if (!B->CheckLineHit(PosA, PosB)) {
						float P = 2.f;
						for (int i = 0; i < 4; i++) {
							float ret = (GetMinLenSegmentToSegment(B->GetBoxSide(i), B->GetBoxSide((i + 1) % 4), PosA, PosB) / Radius);
							if (ret < 1.f) {
								if (P > ret) {
									P = ret;
								}
							}
						}
						if (P <= 1.f) {
							tmp = P;
						}
					}
				}
				if (Ret > tmp) {
					Ret = tmp;
				}
				if (Ret <= 0.f) { break; }
			}
			return Ret;
		}
		// 壁判定
		const bool BackGroundClassBase::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius, bool IsPhysical) noexcept {
			WallList.clear();
			Vector3DX Min, Max;
			Min.x = std::min(EndPos->x, StartPos.x) - (Radius * 4);
			Min.y = std::min(EndPos->y, StartPos.y) - (Radius * 4);
			Max.x = std::max(EndPos->x, StartPos.x) + (Radius * 4);
			Max.y = std::max(EndPos->y, StartPos.y) + (Radius * 4);
			// 線のリストを出す
			for (auto& B : m_Blick) {
				// 壁ではない
				if (!B->GetIsCheckWall()) { continue; }
				// 範囲外
				if (!B->CheckRectangleHit(Min, Max)) { continue; }
				// 辺を登録
				for (int i = 0; i < 4; i++) {
					Vector3DX StartP = B->GetBoxSide(i);
					Vector3DX EndP = B->GetBoxSide((i + 1) % 4);
					// 完全に重なっているものは消す
					bool IsSame = false;
					for (auto& w : WallList) {
						if (
							(((w.GetPos(0) - StartP).sqrMagnitude() <= 0.00001f) && ((w.GetPos(1) - EndP).sqrMagnitude() <= 0.00001f)) ||
							(((w.GetPos(1) - StartP).sqrMagnitude() <= 0.00001f) && ((w.GetPos(0) - EndP).sqrMagnitude() <= 0.00001f))
							) {
							IsSame = true;
							w.SetActive(false);
						}
					}
					if (!IsSame) {
						WallList.resize(WallList.size() + 1);
						WallList.back().SetUp(StartP, EndP, B->GetBoxSide((i + 2) % 4));
					}
				}
			}
			// 90度のへこみの場合はならす
			for (int loop = 0; loop < 5; loop++) {
				for (auto& w : WallList) {
					if (!w.IsActive()) { continue; }
					for (auto& w2 : WallList) {
						if (!w2.IsActive()) { continue; }
						if (&w == &w2) { continue; }
						const Vector3DX* pCenter1 = nullptr;
						const Vector3DX* pCenter2 = nullptr;
						const Vector3DX* pOther1 = nullptr;
						const Vector3DX* pOther2 = nullptr;
						if ((w.GetPos(0) - w2.GetPos(0)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.GetPos(0);
							pCenter2 = &w2.GetPos(0);
							pOther1 = &w.GetPos(1);
							pOther2 = &w2.GetPos(1);
						}
						else if ((w.GetPos(1) - w2.GetPos(0)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.GetPos(1);
							pCenter2 = &w2.GetPos(0);
							pOther1 = &w.GetPos(0);
							pOther2 = &w2.GetPos(1);
						}
						else if ((w.GetPos(0) - w2.GetPos(1)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.GetPos(0);
							pCenter2 = &w2.GetPos(1);
							pOther1 = &w.GetPos(1);
							pOther2 = &w2.GetPos(0);
						}
						else if ((w.GetPos(1) - w2.GetPos(1)).sqrMagnitude() <= 0.00001f) {
							pCenter1 = &w.GetPos(1);
							pCenter2 = &w2.GetPos(1);
							pOther1 = &w.GetPos(0);
							pOther2 = &w2.GetPos(0);
						}
						else { continue; }

						Vector3DX Ct = (*pOther1 + *pOther2) / 2.f;

						if (Vector3DX::Dot(*pOther1 - *pCenter1, *pOther2 - *pCenter2) < 0.f) { continue; }// 鋭角でない

						if (
							(Vector3DX::Dot(w.GetNormal(), *pOther2 - *pCenter2) < 0.f) &&
							(Vector3DX::Dot(w2.GetNormal(), *pOther1 - *pCenter1) < 0.f)
							) {
							continue;
						}// 法線が内向きでない

									// へこみを減らす
						*(Vector3DX*)pCenter1 = Ct;
						*(Vector3DX*)pCenter2 = Ct;
						//*pCenter1 = (*pCenter1 + Ct) / 2.f;
						//*pCenter2 = (*pCenter2 + Ct) / 2.f;

						// ノーマル再演算
						w.CalcNormal();
						w2.CalcNormal();
					}
				}
			}

			Vector3DX VecPt = *EndPos - StartPos;
			int WallCount = 0;
			for (auto& w : WallList) {
				if (!w.IsActive()) { continue; }
				// 外向きなのでノーヒット
				Vector3DX VecP = w.GetPos(1) - w.GetPos(0);
				if (Vector3DX::Dot(Vector3DX::Cross(VecP, VecPt), Vector3DX::Cross(VecP, w.GetNormal() * -1.f)) < 0.f) {
					w.SetActive(false);
				}
				//ヒット加算
				if (w.IsActive()) { WallCount++; }
			}
			if (WallCount == 0) { return false; }
			// 壁との当たり判定処理
			bool HitFlag = false;
			for (auto& w : WallList) {
				if (!w.IsActive()) { continue; }
				// 壁とプレイヤーが当たっていなかったら次のカウントへ
				if (GetHitCheckToCapsule(StartPos, *EndPos, Radius, w.GetPos(0), w.GetPos(1), 0.f)) {
					if (!IsPhysical) {
						SEGMENT_SEGMENT_RESULT Result;
						GetSegmenttoSegment(StartPos, *EndPos, w.GetPos(0), w.GetPos(1), &Result);
						Vector3DX pos;
						pos += Result.SegA_MinDist_Pos;
						pos += Result.SegB_MinDist_Pos;
						pos /= 2.f;
						*EndPos = pos;
						return true;
					}
					else {
						HitFlag = true;// ここにきたら壁とプレイヤーが当たっているということなので、壁に当たったフラグを立てる
						if (VecPt.sqrMagnitude() >= 0.00001f) {
							// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定 壁に当たったら壁に遮られない移動成分分だけ移動する
							*EndPos = StartPos + Vector3DX::Cross(w.GetNormal(), Vector3DX::Cross(VecPt, w.GetNormal()));
							bool IsHit = false;
							for (auto& w2 : WallList) {
								if (!w2.IsActive()) { continue; }
								if (GetHitCheckToCapsule(StartPos, *EndPos, Radius, w2.GetPos(0), w2.GetPos(1), 0.f)) {
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
			}
			if (!IsPhysical) {
				return false;
			}
			// 上記を経ても壁に当たっていたら壁から押し出す処理を行う
			if (HitFlag) {
				for (int i = 0; i < 8; ++i) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
					bool HitF = false;
					for (auto& w : WallList) {
						if (!w.IsActive()) { continue; }
						if (!GetHitCheckToCapsule(StartPos, *EndPos, Radius, w.GetPos(0), w.GetPos(1), 0.f)) { continue; }
						*EndPos += w.GetNormal() * 0.0015f;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						bool IsHit = false;
						for (auto& w2 : WallList) {
							if (!w2.IsActive()) { continue; }
							if (GetHitCheckToCapsule(StartPos, *EndPos, Radius, w2.GetPos(0), w2.GetPos(1), 0.f)) {// 当たっていたらループを抜ける
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
			EndPos->z = 0.f;
			return HitFlag;
		}

		void BackGroundClassBase::Init(const std::string& MapPath) noexcept {
			std::array<std::pair<int, int>, 8> Dir;
			Dir.at(0) = std::make_pair(-1, 0);
			Dir.at(1) = std::make_pair(0, -1);
			Dir.at(2) = std::make_pair(1, 0);
			Dir.at(3) = std::make_pair(0, 1);

			Dir.at(4) = std::make_pair(-1, -1);
			Dir.at(5) = std::make_pair(1, -1);
			Dir.at(6) = std::make_pair(1, 1);
			Dir.at(7) = std::make_pair(-1, 1);
			//
			int ChipNum = 12;
			int r, g, b;
			// マップ
			int MapImage = LoadSoftImage(("data/map/" + MapPath + "/col.bmp").c_str());
			GetSoftImageSize(MapImage, &m_Xsize, &m_Ysize);
			int EvtImage = LoadSoftImage(("data/map/" + MapPath + "/evt.bmp").c_str());
			int PalImage = LoadSoftImage(("data/map/" + MapPath + "/chp.bmp").c_str());
			GetSoftImageSize(PalImage, &ChipNum, NULL);
			GraphHandle::LoadDiv("data/map/" + MapPath + "/img.bmp", ChipNum, ChipNum, 1, 32, 32, &m_MapChip);//床マップチップ
			GraphHandle::LoadDiv("data/map/" + MapPath + "/wal.bmp", 5 * 2, 5, 2, 32, 32, &m_WallChip);//壁マップチップ
			// 
			struct ChipInfo {
				std::array<unsigned char,3> Color{};
				bool m_IsWall{ false };
			};
			std::vector<ChipInfo> ChipInfoDatas;
			{
				ChipInfoDatas.resize(ChipNum);
				for (int x = 0; x < ChipNum; x++) {
					GetPixelSoftImage(PalImage, x, 0, &r, &g, &b, NULL);
					ChipInfoDatas.at(x).Color.at(0) = (unsigned char)r;
					ChipInfoDatas.at(x).Color.at(1) = (unsigned char)g;
					ChipInfoDatas.at(x).Color.at(2) = (unsigned char)b;
					GetPixelSoftImage(PalImage, x, 1, &r, &g, &b, NULL);
					ChipInfoDatas.at(x).m_IsWall = (r < 128);
				}
			}
			//
			m_PlayerSpawn.resize(1);//0はイベントに
			m_EventChip.clear();
			// 
			m_Blick.resize(this->m_Xsize* this->m_Ysize);
			for (int x = 0; x < this->m_Xsize; x++) {
				for (int y = 0; y < this->m_Ysize; y++) {
					auto& B = m_Blick.at(GetXYToNum(x, y)) = std::make_shared<Blick>(x - this->m_Xsize / 2, y - this->m_Ysize / 2);
					GetPixelSoftImage(MapImage, x, this->m_Ysize - 1 - y, &r, &g, &b, NULL);
					for (auto& p : ChipInfoDatas) {
						if ((p.Color.at(0) == r) && (p.Color.at(1) == g) && (p.Color.at(2) == b)) {
							// 使用する素材を設定
							B->SetBasePalette((int)(&p - &ChipInfoDatas.front()), p.m_IsWall);
							break;
						}
					}
					GetPixelSoftImage(EvtImage, x, this->m_Ysize - 1 - y, &r, &g, &b, NULL);
					if (r == 255 && g == 0 && b == 0) {
						PlayerPatrol tmp;tmp.m_index = GetXYToNum(x, y);
						m_PlayerSpawn.emplace_back(tmp);
					}
					else if (r == 192 && g == 168) {//イベント マップ移動とか
						EventChip tmp;
						tmp.m_index = GetXYToNum(x, y);
						tmp.m_EventID = b;//青色部分にイベントIDを埋め込む
						m_EventChip.emplace_back(tmp);
					}
				}
			}
			//巡回ルート
			for (auto& s : m_PlayerSpawn) {
				if ((int)(&s - &m_PlayerSpawn.front()) == 0) { continue; }
				s.m_Patrol.clear();
				s.m_Patrol.emplace_back(s.m_index);
				while (true) {
					auto XY = GetNumToXY(s.m_Patrol.back());
					bool isHitNext = false;
					for (auto& d : Dir) {
						int x = XY.first + d.first;
						int y = XY.second + d.second;
						if (HitPointToRectangle(x, y, 0, 0, this->m_Xsize - 1, this->m_Ysize - 1)) {
							int Num = GetXYToNum(x, y);
							GetPixelSoftImage(EvtImage, x, this->m_Ysize - 1 - y, &r, &g, &b, NULL);
							if (r == 255 && g == 192 && b == 192) {
								bool isHit = false;
								for (auto& p : s.m_Patrol) {
									if (p == Num) {
										isHit = true;
										break;
									}
								}
								if (!isHit) {
									s.m_Patrol.emplace_back(Num);
									isHitNext = true;
									break;
								}
							}
						}
					}
					if (!isHitNext) {
						break;
					}
				}
			}
			//リンクと壁パレット指定の適用
			for (auto& B : m_Blick) {
				auto XY = GetNumToXY((int)(&B - &m_Blick.front()));
				std::array<bool, 8> LURN = { true,true,true,true,true,true,true,true };
				for (int i = 0; i < 8; i++) {
					int x = XY.first + Dir.at(i).first;
					int y = XY.second + Dir.at(i).second;
					//上下左右が壁か否か
					if (HitPointToRectangle(x, y, 0, 0, this->m_Xsize - 1, this->m_Ysize - 1)) {
						LURN.at(i) = GetFloorData(x, y)->GetIsWall();
					}
					//リンク確認
					if ((i < (int)B->GetLinkIndex().size()) && !LURN.at(i)) {
						B->SetLinkIndex(i, GetXYToNum(x, y));
					}
				}
				//
				B->SetWallPallet(LURN);
			}
			//
			DeleteSoftImage(MapImage);
			DeleteSoftImage(EvtImage);
			DeleteSoftImage(PalImage);
			//
			{
				int mdata = FileRead_open(("data/map/" + MapPath + "/dat.txt").c_str(), FALSE);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto ALL = getparams::Getstr(mdata);
					if (ALL == "") { continue; }
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					int targetID = std::atoi(LEFT.substr(0, 3).c_str());
					for (auto& e : m_EventChip) {
						if (e.m_EventID == targetID) {
							//イベントタイプ
							if (LEFT.find("EvtType") != std::string::npos) {
								for (int i = 0;i < (int)EventType::Max;i++) {
									if (RIGHT.find(g_EventStr[i]) != std::string::npos) {
										e.m_EventType = (EventType)i;
										break;
									}
								}
							}
							//イベントタイプ
							else if (LEFT.find("NextStage") != std::string::npos) {
								e.m_EntryID = std::atoi(RIGHT.substr(0, 3).c_str());
								e.m_MapName = RIGHT.substr(4);
							}
							break;
						}
					}
				}
				FileRead_close(mdata);
			}
			//
			m_PointShadowHandle = GraphHandle::Make(y_r(1920), y_r(1080), true);
			m_AmbientShadowHandle = GraphHandle::Make(y_r(1920), y_r(1080), true);
		}
		// 
		void BackGroundClassBase::SetupShadow(std::function<void()> AddAmbShadow) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
				m_AmbientShadowHandle.SetDraw_Screen(false);
				{
					DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, true);
					AddAmbShadow();
					int Radius = GetDispSize(0.5f) + y_r(m_AmbientShadowLength);
					for (auto& B : m_Blick) {
						if (!B->GetIsCheckWall()) { continue; }
						auto DispPos = Convert2DtoDisp(B->GetPos());
						if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
						B->DrawAmbientShadow(m_AmbientLightVec, m_AmbientLightRad, m_MapChip.at(0));
					}
				}
				if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 1 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
					GraphFilter(m_AmbientShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
				}
			}
			m_PointShadowHandle.SetDraw_Screen(false);
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, true);
				int Radius = GetDispSize(0.5f) + y_r(255.f);
				for (auto& B : m_Blick) {
					if (!B->GetIsCheckWall()) { continue; }
					auto DispPos = Convert2DtoDisp(B->GetPos());
					if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
					B->DrawPointShadow(m_PointLightPos, m_MapChip.at(0));
				}
			}
			GraphFilter(m_PointShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
		}
		// 
		void BackGroundClassBase::Draw(void) noexcept {
			int Radius = GetDispSize(0.5f);
			float Size = (float)(Radius * 2.f) / (32 - 1);
			// 床
			for (auto& B : m_Blick) {
				if (B->GetIsWall()) { continue; }
				auto DispPos = Convert2DtoDisp(B->GetPos());
				if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
				m_MapChip.at(B->GetPalette().at(0).GetpaletteNum()).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, B->GetPalette().at(0).GetZRad(), FALSE);
			}
			// 影
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
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
				auto MinPos = Convert2DtoDisp(GetFloorData(0, 0)->GetPos());
				auto MaxPos = Convert2DtoDisp(GetFloorData(this->m_Xsize - 1, this->m_Ysize - 1)->GetPos());
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
			for (auto& B : m_Blick) {
				if (!B->GetIsWall()) { continue; }
				auto DispPos = Convert2DtoDisp(B->GetPos());
				if (!HitPointToRectangle((int)DispPos.x, (int)DispPos.y, -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
				m_WallChip.at(B->GetPalette().at(0).GetpaletteNum()).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, B->GetPalette().at(0).GetZRad(), FALSE);
				for (int i = 0; i < 4; i++) {
					if (B->GetPalette().at(i+1).IsActive()) {
						m_WallChip.at(B->GetPalette().at(i + 1).GetpaletteNum()).DrawRotaGraph((int)DispPos.x, (int)DispPos.y, Size, B->GetPalette().at(i + 1).GetZRad(), TRUE);
					}
				}
			}
			// デバッグ描画
			/*
			for (auto& w : WallList) {
				if (!w.IsActive()) { continue; }
				{
					auto DispPos1 = Convert2DtoDisp(w.GetPos(0));
					auto DispPos2 = Convert2DtoDisp(w.GetPos(1));
					DrawLine(
						(int)(DispPos1.x), (int)(DispPos1.y),
						(int)(DispPos2.x), (int)(DispPos2.y),
						Green, 10);
				}
				{
					auto DispPos1 = Convert2DtoDisp((w.GetPos(0) + w.GetPos(1)) / 2.f);
					auto DispPos2 = Convert2DtoDisp((w.GetPos(0) + w.GetPos(1)) / 2.f + w.GetNormal() * 10.f);
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
				b.reset();
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

