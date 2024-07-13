#include	"BackGround.hpp"

const FPS_n2::Sceneclass::BackGroundClassBase* SingletonBase<FPS_n2::Sceneclass::BackGroundClassBase>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		// 影を描画
		void DrawModi(std::array<Vector3DX, 4>& Position, float Add, const GraphHandle& ShadowChip) noexcept {
			//前後左右に太らせる
			float XMax = -1000000.f;
			float XMin = 1000000.f;
			float YMax = -1000000.f;
			float YMin = 1000000.f;
			int xmin = INVALID_ID;
			int xmax = INVALID_ID;
			int ymin = INVALID_ID;
			int ymax = INVALID_ID;
			for (auto& p : Position) {
				int i = static_cast<int>(&p - &Position.front());
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
			int x2min = INVALID_ID;
			int x2max = INVALID_ID;
			int y2min = INVALID_ID;
			int y2max = INVALID_ID;
			for (auto& p : Position) {
				int i = static_cast<int>(&p - &Position.front());
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
			Position.at(static_cast<size_t>(xmin)).x -= Add;
			Position.at(static_cast<size_t>(ymin)).y -= Add;
			Position.at(static_cast<size_t>(xmax)).x += Add;
			Position.at(static_cast<size_t>(ymax)).y += Add;

			if (x2min != INVALID_ID) { Position.at(static_cast<size_t>(x2min)).x -= Add; }
			if (y2min != INVALID_ID) { Position.at(static_cast<size_t>(y2min)).y -= Add; }
			if (x2max != INVALID_ID) { Position.at(static_cast<size_t>(x2max)).x += Add; }
			if (y2max != INVALID_ID) { Position.at(static_cast<size_t>(y2max)).y += Add; }

			DrawModiGraph(
				static_cast<int>(Position.at(0).x), static_cast<int>(Position.at(0).y),
				static_cast<int>(Position.at(1).x), static_cast<int>(Position.at(1).y),
				static_cast<int>(Position.at(2).x), static_cast<int>(Position.at(2).y),
				static_cast<int>(Position.at(3).x), static_cast<int>(Position.at(3).y),
				ShadowChip.get(), FALSE);
		}
		void BackGroundClassBase::Blick::DrawAmbientShadow(const Vector3DX& AmbientLightVec, float AmbientLightRad, const GraphHandle& ShadowChip) const noexcept {
			std::array<Vector3DX, 4> Position{ };
			for (int i = 0; i < 4; i++) {
				if (GetLinkIndex().at(static_cast<size_t>((i + 1) % 4)) == INVALID_ID) { continue; }
				Vector3DX postmp1, postmp2;
				Convert2DtoDisp(this->GetBoxSide(i),&postmp1);
				Convert2DtoDisp(this->GetBoxSide((i + 1) % 4),&postmp2);
				float radsub = deg2rad(90 * i);
				if (std::cos(AmbientLightRad - radsub) > 0.f) {
					Position.at(0).Set(postmp1.x + AmbientLightVec.x, postmp1.y + AmbientLightVec.y, 0.f);
					Position.at(1).Set(postmp2.x + AmbientLightVec.x, postmp2.y + AmbientLightVec.y, 0.f);
					Position.at(2) = postmp2;
					Position.at(3) = postmp1;
					DrawModi(Position, 3.f, ShadowChip);
				}
			}
		}
		void BackGroundClassBase::Blick::DrawPointShadow(const Vector3DX& PointLightPos, const GraphHandle& ShadowChip) const noexcept {
			std::array<Vector3DX, 4> Position{ };
			for (int i = 0; i < 4; i++) {
				if (GetLinkIndex().at(static_cast<size_t>((i + 1) % 4)) == INVALID_ID) { continue; }
				Vector3DX postmp1, postmp2;
				Convert2DtoDisp(this->GetBoxSide(i), &postmp1);
				Convert2DtoDisp(this->GetBoxSide((i + 1) % 4), &postmp2);
				float radsub = deg2rad(90 * i);
				if (std::cos(GetRadVec2Vec((postmp1 + postmp2) / 2.f, PointLightPos) - radsub) > 0.f) {
					auto length = 1920.f;
					float lightrad1 = GetRadVec2Vec(postmp1, PointLightPos);
					float lightrad2 = GetRadVec2Vec(postmp2, PointLightPos);

					Position.at(0).Set(postmp1.x + std::sin(lightrad1) * length, postmp1.y + std::cos(lightrad1) * length, 0.f);
					Position.at(1).Set(postmp2.x + std::sin(lightrad2) * length, postmp2.y + std::cos(lightrad2) * length, 0.f);
					Position.at(2) = postmp2;
					Position.at(3) = postmp1;

					DrawModi(Position, 3.f, ShadowChip);
				}
			}
		}
		// 線分の間で壁に当たっているか
		float BackGroundClassBase::CheckHideShadow(const Vector3DX& PosA, const Vector3DX& PosB, float Radius) noexcept {
			float Ret = 1.f;
			for (auto& B : this->m_CheckWallBlick) {
				// 範囲外
				Vector3DX DispPos;
				Convert2DtoDisp(B->GetPos(), &DispPos);
				if (!HitPointToRectangle(
					static_cast<int>(DispPos.x), static_cast<int>(DispPos.y),
					static_cast<int>(-Radius), static_cast<int>(-Radius),
					y_r(1920) + static_cast<int>(Radius), y_r(1080) + static_cast<int>(Radius))) { continue; }
				//ヒットするかとヒットした場合の透明度を指定
				float tmp = 1.f;
				if (B->CheckLineHit(PosA, PosB)) {
					tmp = 0.f;
				}
				else if (B->CheckCapsuleHit(PosA, PosB, Radius)) {
					float P = 2.f;
					for (int i = 0; i < 4; i++) {
						if (B->GetLinkIndex().at(static_cast<size_t>((i + 1) % 4)) == INVALID_ID) { continue; }
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
				if (Ret > tmp) {
					Ret = tmp;
				}
				if (Ret <= 0.f) { break; }
			}
			return Ret;
		}
		// 壁判定
		bool BackGroundClassBase::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius, bool IsPhysical) const noexcept {
			std::vector<CheckLines>				WallList;
			WallList.reserve(256);
			Vector3DX Min, Max;
			Min.x = std::min(EndPos->x, StartPos.x) - (Radius * 4);
			Min.y = std::min(EndPos->y, StartPos.y) - (Radius * 4);
			Max.x = std::max(EndPos->x, StartPos.x) + (Radius * 4);
			Max.y = std::max(EndPos->y, StartPos.y) + (Radius * 4);
			// 線のリストを出す
			for (auto& B : this->m_CheckWallBlick) {
				// 範囲外
				if (!B->CheckRectangleHit(Min, Max)) { continue; }
				// 辺を登録
				for (int i = 0; i < 4; i++) {
					if (B->GetLinkIndex().at(static_cast<size_t>((i + 1) % 4)) == INVALID_ID) { continue; }//辺同士で重なっているので未登録に
					WallList.resize(WallList.size() + 1);
					WallList.back().SetUp(B->GetBoxSide(i), B->GetBoxSide((i + 1) % 4), B->GetBoxSide((i + 2) % 4));
				}
			}
			// 90度のへこみの場合はならす
			for (int loop = 0; loop < 5; loop++) {
				for (auto& w : WallList) {
					for (auto& w2 : WallList) {
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
				// 外向きなのでノーヒット
				Vector3DX VecP = w.GetPos(1) - w.GetPos(0);
				if (Vector3DX::Dot(Vector3DX::Cross(VecP, VecPt), Vector3DX::Cross(VecP, w.GetNormal() * -1.f)) < 0.f) {
					w.SetActive(false);
				}
				else {
					WallCount++;//ヒット加算
				}
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
						*EndPos = Result.SegA_MinDist_Pos;
						*EndPos += Result.SegB_MinDist_Pos;
						*EndPos /= 2.f;
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
			std::string CommonPath = "data/map/" + MapPath + "/";
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
			int MapImage = LoadSoftImage((CommonPath + "col.bmp").c_str());
			GetSoftImageSize(MapImage, &this->m_Xsize, &this->m_Ysize);
			int EvtImage = LoadSoftImage((CommonPath + "evt.bmp").c_str());
			int PalImage = LoadSoftImage((CommonPath + "chp.bmp").c_str());
			GetSoftImageSize(PalImage, &ChipNum, nullptr);
			GraphHandle::LoadDiv(CommonPath + "img.bmp", ChipNum, ChipNum, 1, 32, 32, &this->m_MapChip);//床マップチップ
			GraphHandle::LoadDiv(CommonPath + "wal.bmp", 5 * 2, 5, 2, 32, 32, &this->m_WallChip);//壁マップチップ
			// 
			struct ChipInfo {
				std::array<unsigned char,3> m_Color{};
				bool						m_IsWall{ false };
			};
			std::vector<ChipInfo> ChipInfoDatas;
			{
				ChipInfoDatas.resize(static_cast<size_t>(ChipNum));
				for (int x = 0; x < ChipNum; x++) {
					GetPixelSoftImage(PalImage, x, 0, &r, &g, &b, nullptr);
					ChipInfoDatas.at(static_cast<size_t>(x)).m_Color.at(0) = (unsigned char)r;
					ChipInfoDatas.at(static_cast<size_t>(x)).m_Color.at(1) = (unsigned char)g;
					ChipInfoDatas.at(static_cast<size_t>(x)).m_Color.at(2) = (unsigned char)b;
					GetPixelSoftImage(PalImage, x, 1, &r, &g, &b, nullptr);
					ChipInfoDatas.at(static_cast<size_t>(x)).m_IsWall = (r < 128);
				}
			}
			//
			this->m_PlayerSpawn.resize(1);//0はイベントに
			this->m_EventChip.clear();
			// 
			this->m_Blick.resize(static_cast<size_t>(this->m_Xsize* this->m_Ysize));
			for (int x = 0; x < this->m_Xsize; x++) {
				for (int y = 0; y < this->m_Ysize; y++) {
					auto& B = this->m_Blick.at(static_cast<size_t>(GetXYToNum(x, y))) = std::make_shared<Blick>(x, y, GetXYToNum(x, y));
					GetPixelSoftImage(MapImage, x, this->m_Ysize - 1 - y, &r, &g, &b, nullptr);
					for (auto& p : ChipInfoDatas) {
						if ((p.m_Color.at(0) == r) && (p.m_Color.at(1) == g) && (p.m_Color.at(2) == b)) {
							// 使用する素材を設定
							B->SetBasePalette(static_cast<int>(&p - &ChipInfoDatas.front()), p.m_IsWall);
							break;
						}
					}
					GetPixelSoftImage(EvtImage, x, this->m_Ysize - 1 - y, &r, &g, &b, nullptr);
					if (r == 255 && g == 0 && b == 0) {
						PlayerPatrol tmp;tmp.m_index = GetXYToNum(x, y);
						this->m_PlayerSpawn.emplace_back(tmp);
					}
					else if (r == 192 && g == 168) {//イベント マップ移動とか
						EventChip tmp;
						tmp.m_index = GetXYToNum(x, y);
						tmp.m_EventID = b;//青色部分にイベントIDを埋め込む
						tmp.m_CutSceneID = -1;
						tmp.m_ActiveDelaySec = 0;
						tmp.m_WinCutSceneID = -1;
						this->m_EventChip.emplace_back(tmp);
					}
				}
			}
			//巡回ルート
			for (auto& s : this->m_PlayerSpawn) {
				if (static_cast<int>(&s - &this->m_PlayerSpawn.front()) == 0) { continue; }
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
							GetPixelSoftImage(EvtImage, x, this->m_Ysize - 1 - y, &r, &g, &b, nullptr);
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
			this->m_FloorBlick.reserve(this->m_Blick.size());
			this->m_WallBlick.reserve(this->m_Blick.size());
			this->m_CheckWallBlick.reserve(this->m_Blick.size());
			//リンクと壁パレット指定の適用
			for (auto& B : this->m_Blick) {
				auto XY = GetNumToXY(static_cast<int>(&B - &this->m_Blick.front()));
				std::array<bool, 8> LURN = { true,true,true,true,true,true,true,true };
				for (int i = 0; i < 8; i++) {
					int x = XY.first + Dir.at(static_cast<size_t>(i)).first;
					int y = XY.second + Dir.at(static_cast<size_t>(i)).second;
					//上下左右が壁か否か
					if (HitPointToRectangle(x, y, 0, 0, this->m_Xsize - 1, this->m_Ysize - 1)) {
						LURN.at(static_cast<size_t>(i)) = GetFloorData(x, y)->GetIsWall();
					}
					//リンク確認
					if ((i < static_cast<int>(B->GetLinkIndex().size())) && !LURN.at(static_cast<size_t>(i))) {
						B->SetLinkIndex(i, GetXYToNum(x, y));
					}
				}
				//
				B->SetWallPallet(LURN);

				//ループ高速化のための保持変数
				if (!B->GetIsWall()) {
					this->m_FloorBlick.emplace_back(B);
				}
				if (B->GetIsWall()) {
					this->m_WallBlick.emplace_back(B);
				}
				if (B->GetIsCheckWall()) {
					this->m_CheckWallBlick.emplace_back(B);
				}
			}
			//
			DeleteSoftImage(MapImage);
			DeleteSoftImage(EvtImage);
			DeleteSoftImage(PalImage);
			//
			{
				int mdata = FileRead_open((CommonPath + "dat.txt").c_str(), FALSE);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto ALL = getparams::Getstr(mdata);
					if (ALL == "") { continue; }
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					if (LEFT == "Name") {
						this->m_GetMapTextID = std::stoi(RIGHT);
					}
					else if (LEFT == "CamSize") {
						this->m_CamScale = std::stof(RIGHT);
					}
					else {
						int targetID = std::stoi(LEFT.substr(0, 3));
						for (auto& e : this->m_EventChip) {
							if (e.m_EventID == targetID) {
								//イベントタイプ
								if (LEFT.find("EvtType") != std::string::npos) {
									for (int i = 0; i < static_cast<int>(EventType::Max); i++) {
										if (RIGHT.find(g_EventStr[i]) != std::string::npos) {
											e.m_EventType = (EventType)i;
											break;
										}
									}
								}
								else if (LEFT.find("EntryCutScene") != std::string::npos) {
									e.m_CutSceneID = std::stoi(RIGHT);
								}
								//遷移用設定
								else if (LEFT.find("NextStage") != std::string::npos) {
									if (RIGHT == "None") {
										e.m_EntryID = -1;
									}
									else {
										e.m_EntryID = std::stoi(RIGHT.substr(0, 3));
										e.m_MapName = RIGHT.substr(4);
									}
								}
								//カットシーン用設定
								else if (LEFT.find("CutSelect") != std::string::npos) {
									e.m_CutSceneID = std::stoi(RIGHT);
								}
								else if (LEFT.find("ActiveDelaySec") != std::string::npos) {
									e.m_ActiveDelaySec = std::stoi(RIGHT);
								}
								//
								else if (LEFT.find("WinCut") != std::string::npos) {
									e.m_WinCutSceneID = std::stoi(RIGHT);
								}
								break;
							}
						}
					}
				}
				FileRead_close(mdata);
			}
			//
			this->m_PointShadowHandle = GraphHandle::Make(y_r(1920), y_r(1080), true);
			this->m_AmbientShadowHandle = GraphHandle::Make(y_r(1920), y_r(1080), true);
		}
		// 
		void BackGroundClassBase::SetupShadow(std::function<void()> AddAmbShadow) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
				this->m_AmbientShadowHandle.SetDraw_Screen(false);
				{
					DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, true);
					AddAmbShadow();
					int Radius = GetDispSize(0.5f) + y_r(this->m_AmbientShadowLength);
					for (auto& B : this->m_CheckWallBlick) {
						Vector3DX DispPos;
						Convert2DtoDisp(B->GetPos(), &DispPos);
						if (!HitPointToRectangle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
						B->DrawAmbientShadow(this->m_AmbientLightVec, this->m_AmbientLightRad, this->m_MapChip.at(0));
					}
				}
				if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 1 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
					GraphFilter(this->m_AmbientShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
				}
			}
			this->m_PointShadowHandle.SetDraw_Screen(false);
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, true);
				int Radius = GetDispSize(0.5f) + y_r(255.f);
				for (auto& B : this->m_CheckWallBlick) {
					Vector3DX DispPos;
					Convert2DtoDisp(B->GetPos(), &DispPos);
					if (!HitPointToRectangle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
					B->DrawPointShadow(this->m_PointLightPos, this->m_MapChip.at(0));
				}
			}
			GraphFilter(this->m_PointShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
		}
		// 
		void BackGroundClassBase::Draw(void) noexcept {
			int Radius = GetDispSize(0.5f);
			float Size = static_cast<float>(Radius * 2) / (32 - 1);
			// 床
			for (auto& B : this->m_FloorBlick) {
				Vector3DX DispPos;
				Convert2DtoDisp(B->GetPos(), &DispPos);
				if (!HitPointToRectangle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
				this->m_MapChip.at(static_cast<size_t>(B->GetPalette().at(0).GetpaletteNum())).DrawRotaGraph(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), Size, B->GetPalette().at(0).GetZRad(), FALSE);
			}
			// 影
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0 || (GetUseDirect3DVersion() == DX_DIRECT3D_11)) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
				this->m_AmbientShadowHandle.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), false);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
		void BackGroundClassBase::DrawFront(void) noexcept {
			int Radius = GetDispSize(0.5f);
			float Size = static_cast<float>(Radius * 2) / (32 - 1);
			// 
			{
				Vector3DX MinPos, MaxPos;
				Convert2DtoDisp(GetFloorData(0, 0)->GetPos(), &MinPos);
				Convert2DtoDisp(GetFloorData(this->m_Xsize - 1, this->m_Ysize - 1)->GetPos(), &MaxPos);
				int xmin = static_cast<int>(MinPos.x);
				int ymin = static_cast<int>(MaxPos.y);
				int xmax = static_cast<int>(MaxPos.x);
				int ymax = static_cast<int>(MinPos.y);
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
			for (auto& B : this->m_WallBlick) {
				Vector3DX DispPos;
				Convert2DtoDisp(B->GetPos(), &DispPos);
				if (!HitPointToRectangle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), -Radius, -Radius, y_r(1920) + Radius, y_r(1080) + Radius)) { continue; }
				for (int i = 0; i < 5; i++) {
					if (B->GetPalette().at(static_cast<size_t>(i)).IsActive()) {
						this->m_WallChip.at(static_cast<size_t>(B->GetPalette().at(static_cast<size_t>(i)).GetpaletteNum())).DrawRotaGraph(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y),Size, B->GetPalette().at(static_cast<size_t>(i)).GetZRad(), i != 0);
					}
				}
			}
			/*
			for (auto& w : WallList) {
				if (!w.IsActive()) { continue; }
				{
					Vector3DX DispPos1, DispPos2;
					Convert2DtoDisp(w.GetPos(0), &DispPos1);
					Convert2DtoDisp(w.GetPos(1), &DispPos2);
					DrawLine(
						static_cast<int>(DispPos1.x), static_cast<int>(DispPos1.y),
						static_cast<int>(DispPos2.x), static_cast<int>(DispPos2.y),
						Green, 10);
				}
				{
					Vector3DX DispPos1, DispPos2;
					Convert2DtoDisp((w.GetPos(0) + w.GetPos(1)) / 2.f, &DispPos1);
					Convert2DtoDisp((w.GetPos(0) + w.GetPos(1)) / 2.f + w.GetNormal() * 10.f, &DispPos2);
					DrawLine(
						static_cast<int>(DispPos1.x), static_cast<int>(DispPos1.y),
						static_cast<int>(DispPos2.x), static_cast<int>(DispPos2.y),
						Green, 10);
				}
			}
			//*/
		}
		// 
		void BackGroundClassBase::Dispose(void) noexcept {
			for (auto& b : this->m_FloorBlick) {
				b.reset();
			}
			this->m_FloorBlick.clear();
			for (auto& b : this->m_WallBlick) {
				b.reset();
			}
			this->m_WallBlick.clear();
			for (auto& b : this->m_CheckWallBlick) {
				b.reset();
			}
			this->m_CheckWallBlick.clear();
			for (auto& b : this->m_Blick) {
				b.reset();
			}
			this->m_Blick.clear();
			for (auto& g : this->m_MapChip) {
				g.Dispose();
			}
			this->m_MapChip.clear();
			for (auto& g : this->m_WallChip) {
				g.Dispose();
			}
			this->m_WallChip.clear();
		}
	};
};

