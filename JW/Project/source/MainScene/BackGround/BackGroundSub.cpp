#pragma once
#include	"BackGroundSub.hpp"

#include	"../../sub.hpp"

namespace FPS_n2 {
	//
	void			BuildControl::MakeMiniMap() noexcept {
		m_MapGraph = GraphHandle::Make(Size * (int)tileSize, Size * (int)tileSize, true);
		m_MapGraph.GetSize(&m_MapGraphXSize, &m_MapGraphYSize);
	}
	void			BuildControl::UpdateMiniMap() noexcept {
		m_MapGraph.SetDraw_Screen();
		{
			//int loop = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						//auto check = this->m_ObjBuilds[loop].IsDraw();
						//loop++;

						int count = 0;

						bool XP = m_MazeControl.PosIsPath(x + 1, y);
						bool XM = m_MazeControl.PosIsPath(x - 1, y);
						bool ZP = m_MazeControl.PosIsPath(x, y + 1);
						bool ZM = m_MazeControl.PosIsPath(x, y - 1);

						if (XP) { count++; }
						if (XM) { count++; }
						if (ZP) { count++; }
						if (ZM) { count++; }

						int color = 255;// *count / 4;
						//if (check) { color -= 128; }
						int Wall = 3;

						DrawBox(
							x * 30 + (!XM ? Wall : 0),
							y * 30 + (!ZM ? Wall : 0),
							(x + 1) * 30 - (!XP ? Wall : 0),
							(y + 1) * 30 - (!ZP ? Wall : 0),
							GetColor(color, color, color), TRUE);
					}
				}
			}
		}
	}

	//
	void			BuildControl::Load(void) noexcept {
		MV1::Load("data/model/map_parts/model.mv1", &this->m_ObjBuildBase, DX_LOADMODEL_PHYSICS_DISABLE);
		MV1::Load("data/model/map_parts/col.mv1", &this->m_ColBuildBase, DX_LOADMODEL_PHYSICS_DISABLE);
		m_LightControl.Load();
		m_DrumControl.Load();
	}
	void			BuildControl::Init() noexcept {
		/* 迷路の作成 */
		m_MazeControl.createMaze(Size, Size);
		//
		this->m_ObjBuilds.resize(m_MazeControl.GetPachCount());
		{
			float deg = 0.f;
			int loop = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						int count = 0;

						bool XP = m_MazeControl.PosIsPath(x + 1, y);
						bool XM = m_MazeControl.PosIsPath(x - 1, y);
						bool ZP = m_MazeControl.PosIsPath(x, y + 1);
						bool ZM = m_MazeControl.PosIsPath(x, y - 1);

						if (XP) { count++; }
						if (XM) { count++; }
						if (ZP) { count++; }
						if (ZM) { count++; }

						int ID = 0;
						switch (count) {
							case 1:
								ID = 0;
								if (ZM) { deg = 0.f; }
								if (XM) { deg = 90.f; }
								if (ZP) { deg = 180.f; }
								if (XP) { deg = 270.f; }
								break;
							case 2:
								if (ZP && ZM) { ID = 1; deg = 0.f; }
								if (XP && XM) { ID = 1; deg = 90.f; }

								if (XP && ZP) { ID = 4; deg = 270.f; }
								if (XM && ZP) { ID = 4; deg = 180.f; }
								if (XM && ZM) { ID = 4; deg = 90.f; }
								if (XP && ZM) { ID = 4; deg = 0.f; }
								break;
							case 3:
								ID = 2;
								if (ZP && XP && ZM) { deg = 0.f; }
								if (ZP && XM && ZM) { deg = 180.f; }
								if (XP && ZP && XM) { deg = 270.f; }
								if (XP && ZM && XM) { deg = 90.f; }
								break;
							case 4:
								ID = 3;
								break;
							default:
								break;
						}
						this->m_ObjBuilds[loop].Set(loop, this->m_ObjBuildBase, this->m_ColBuildBase, ID);
						this->m_ObjBuilds[loop].SetPosition(GetPos(x, y), deg2rad(deg));
						loop++;
					}
				}
			}
		}
		{
			int loop = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						auto& bu = this->m_ObjBuilds[loop];
						if (m_MazeControl.PosIsPath(x + 1, y)) {
							bu.SetLink(0, GetNearestBuilds(GetPos(x + 1, y)));
						}
						if (m_MazeControl.PosIsPath(x, y + 1)) {
							bu.SetLink(1, GetNearestBuilds(GetPos(x, y + 1)));
						}
						if (m_MazeControl.PosIsPath(x - 1, y)) {
							bu.SetLink(2, GetNearestBuilds(GetPos(x - 1, y)));
						}
						if (m_MazeControl.PosIsPath(x, y - 1)) {
							bu.SetLink(3, GetNearestBuilds(GetPos(x, y - 1)));
						}
						loop++;
					}
				}
			}
		}
		//
		this->m_Garbage.Set();
		//
		MakeMiniMap();
		UpdateMiniMap();
		//
		{
			int MaxCount = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						bool isHit = false;
						if ((y % 4) == 0) {
							if ((x % 3) == 0) { isHit = true; }
						}
						if ((y % 4) == 2) {
							if ((x % 3) == 2) { isHit = true; }
						}
						if ((y % 2) == 1) {
							if ((x % 3) == 1) { isHit = true; }
						}
						if (!isHit) { continue; }
						MaxCount++;
					}
				}
			}
			m_LightControl.Init(MaxCount);
			VECTOR_ref BasePos;
			int loop = 0;
			int loopt = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						auto* ptr = &this->m_ObjBuilds[loopt];
						loopt++;

						bool isHit = false;
						if ((y % 4) == 0) {
							if ((x % 3) == 0) { isHit = true; }
						}
						if ((y % 4) == 2) {
							if ((x % 3) == 2) { isHit = true; }
						}
						if ((y % 2) == 1) {
							if ((x % 3) == 1) { isHit = true; }
						}
						if (!isHit) { continue; }

						BasePos = GetPos(x, y);
						BasePos.yadd(2.65f*Scale_Rate);

						m_LightControl.Set(loop, BasePos, ptr);
						loop++;
					}
				}
			}
		}
		//
		{
			int MaxCount = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						bool isHit = false;
						if ((y % 4) == 0) {
							if ((x % 3) == 0) { isHit = true; }
						}
						if ((y % 4) == 2) {
							if ((x % 3) == 2) { isHit = true; }
						}
						if (!isHit) { continue; }
						MaxCount++;
					}
				}
			}
			m_DrumControl.Init(MaxCount);
			VECTOR_ref BasePos;
			int loop = 0;
			int loopt = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						auto* ptr = &this->m_ObjBuilds[loopt];
						loopt++;

						bool isHit = false;
						if ((y % 4) == 0) {
							if ((x % 3) == 0) { isHit = true; }
						}
						if ((y % 4) == 2) {
							if ((x % 3) == 2) { isHit = true; }
						}
						if (!isHit) { continue; }

						bool XP = m_MazeControl.PosIsPath(x + 1, y);
						bool XM = m_MazeControl.PosIsPath(x - 1, y);
						bool ZP = m_MazeControl.PosIsPath(x, y + 1);
						bool ZM = m_MazeControl.PosIsPath(x, y - 1);

						if (!XP) {
							BasePos = GetPos(x, y) + VECTOR_ref::vget(tileSize / 4.f, 0.f, 0.f);
						}
						if (!XM) {
							BasePos = GetPos(x, y) + VECTOR_ref::vget(-tileSize / 4.f, 0.f, 0.f);
						}
						if (!ZP) {
							BasePos = GetPos(x, y) + VECTOR_ref::vget(0.f, 0.f, tileSize / 4.f);
						}
						if (!ZM) {
							BasePos = GetPos(x, y) + VECTOR_ref::vget(0.f, 0.f, -tileSize / 4.f);
						}


						m_DrumControl.Set(loop, BasePos, ptr);
						loop++;
					}
				}
			}
			m_DrumControl.Update();
		}
		//
	}
	void			BuildControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//オクルージョンカリング
		{
			auto MyPos = DrawParts->GetMainCamera().GetCamPos();
			auto MyVec = DrawParts->GetMainCamera().GetCamVec() - MyPos; MyVec.y(0.f); MyVec = MyVec.Norm();
			auto MyMat = MATRIX_ref::RotY(atan2f(MyVec.x(), MyVec.z())).Inverse();

			auto MinMaxRad = [&](const VECTOR_ref& MaxPos, const VECTOR_ref& MinPos) {
				auto MaxPosA = MaxPos - MyPos;
				auto MinPosA = MinPos - MyPos;
				float MinRadA = 10.f;
				float MaxRadA = -10.f;
				{
					auto Pos1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(MaxPosA.x(), 0.f, MaxPosA.z()), MyMat);
					auto Pos2 = MATRIX_ref::Vtrans(VECTOR_ref::vget(MaxPosA.x(), 0.f, MinPosA.z()), MyMat);
					auto Pos3 = MATRIX_ref::Vtrans(VECTOR_ref::vget(MinPosA.x(), 0.f, MaxPosA.z()), MyMat);
					auto Pos4 = MATRIX_ref::Vtrans(VECTOR_ref::vget(MinPosA.x(), 0.f, MinPosA.z()), MyMat);

					auto rad_1 = atan2f(Pos1.x(), Pos1.z());
					auto rad_2 = atan2f(Pos2.x(), Pos2.z());
					auto rad_3 = atan2f(Pos3.x(), Pos3.z());
					auto rad_4 = atan2f(Pos4.x(), Pos4.z());
					if (MinRadA > rad_1) { MinRadA = rad_1; }
					if (MinRadA > rad_2) { MinRadA = rad_2; }
					if (MinRadA > rad_3) { MinRadA = rad_3; }
					if (MinRadA > rad_4) { MinRadA = rad_4; }
					if (MaxRadA < rad_1) { MaxRadA = rad_1; }
					if (MaxRadA < rad_2) { MaxRadA = rad_2; }
					if (MaxRadA < rad_3) { MaxRadA = rad_3; }
					if (MaxRadA < rad_4) { MaxRadA = rad_4; }
				}
				return std::make_pair(MinRadA, MaxRadA);
			};

			for (auto& b : this->m_ObjBuilds) {
				b.EnableChackCam();
				auto VecB = (b.GetMatrix().pos() - MyPos); VecB.y(0);
				auto LenB = VecB.Length();
				if (LenB > 50.f) {
					if ((MyVec.dot(VecB.Norm()) > sin(deg2rad(0.f))) && (LenB < 60.f*Scale_Rate)) {
						auto RadB = MinMaxRad(b.GetMaxPos(), b.GetMinPos());
						//壁との判定チェック
						bool isHit = false;
						for (int y = 0; y < Size; y++) {
							for (int x = 0; x < Size; x++) {
								if (!m_MazeControl.PosIsPath(x, y)) {
									VECTOR_ref BasePos = GetPos(x, y);
									//
									auto VecA = (BasePos - MyPos); VecA.y(0.f);
									auto LenA = VecA.Length();
									if (LenA > 50.f) {
										if (LenA < LenB) {
											if ((MyVec.dot(VecA.Norm()) > sin(deg2rad(-10.f))) && (LenA < 60.f*Scale_Rate)) {
												auto RadA = MinMaxRad(BasePos + VECTOR_ref::vget(17.f, 0.f, 17.f), BasePos - VECTOR_ref::vget(17.f, 0.f, 17.f));

												//範囲比較
												if ((RadA.first >= RadB.first) || (RadB.second >= RadA.second)) {
													//範囲に被っている部分を消す
													if ((RadB.first <= RadA.first) && (RadA.first <= RadB.second)) {
														RadB.second = RadA.first;
													}

													if ((RadB.first <= RadA.second) && (RadA.second <= RadB.second)) {
														RadB.first = RadA.second;
													}
												}
												else {
													isHit = true;
													b.DisableChackCam();
													break;
												}
											}
										}
									}
								}
							}
							if (isHit) {
								break;
							}
						}
					}
					else {
						b.DisableChackCam();
					}
				}
			}
		}
		this->m_Garbage.Execute(DrawParts->GetMainCamera().GetCamPos());
		//
		m_LightControl.Update();
		m_LightControl.SetLight();
		//
		//UpdateMiniMap();
	}
	void			BuildControl::ShadowDraw() noexcept {
		this->m_Garbage.ShadowDraw();
	}
	void			BuildControl::Draw() noexcept {
		int fog_enable = GetFogEnable();
		int fog_mode = GetFogMode();
		float fog_density = GetFogDensity();
		int fog_r = 0, fog_g = 0, fog_b = 0;
		float fog_start = 0.f, fog_end = 0.f;
		GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
		GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )

		SetFogEnable(TRUE);
		SetFogMode(DX_FOGMODE_EXP2);
		SetFogStartEnd(Scale_Rate*6.f, Scale_Rate*10.f);
		SetFogColor(26, 29, 20);
		SetFogDensity(0.01f);

		for (auto& b : this->m_ObjBuilds) {
			if (b.IsDraw()) {
				if (CheckCameraViewClip_Box(b.GetMinPos().get(), b.GetMaxPos().get()) == FALSE) {
					b.GetObj().DrawMesh(b.GetMeshSel());
				}
			}
		}
		this->m_Garbage.Draw();
		m_DrumControl.Draw();
		m_LightControl.Draw();

		SetFogEnable(fog_enable);
		SetFogMode(fog_mode);
		SetFogColor(fog_r, fog_g, fog_b);
		SetFogStartEnd(fog_start, fog_end);
		SetFogDensity(fog_density);
		//
	}
	void			BuildControl::DrawFront() noexcept {
		m_LightControl.DrawFront();
	}
	
	void			BuildControl::Dispose(void) noexcept {
		this->m_ObjBuildBase.Dispose();
		this->m_ColBuildBase.Dispose();
		m_MazeControl.Reset();
		m_DrumControl.Dispose();
		m_LightControl.Dispose();
		this->m_Garbage.Dispose();
	}
};
