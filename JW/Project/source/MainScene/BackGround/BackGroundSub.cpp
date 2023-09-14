#pragma once
#include	"BackGroundSub.hpp"

#include	"../../sub.hpp"

namespace FPS_n2 {
	void			BuildControl::Load(void) noexcept {
		MV1::Load("data/model/map_parts/model.mv1", &this->m_ObjBuildBase, DX_LOADMODEL_PHYSICS_DISABLE);
		MV1::Load("data/model/map_parts/col.mv1", &this->m_ColBuildBase, DX_LOADMODEL_PHYSICS_DISABLE);
		MV1::Load("data/model/map_parts/light.mv1", &this->m_ObjLightBase, DX_LOADMODEL_PHYSICS_DISABLE);
	}
	void			BuildControl::Init() noexcept {
		int Size = 19;
		float tileSize = 30.f;
		/* 迷路の作成 */
		m_MazeControl.createMaze(Size, Size);
		{
			m_MapGraph = GraphHandle::Make(Size * (int)tileSize, Size * (int)tileSize, true);
			m_MapGraph.SetDraw_Screen();
			{
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

							int color = 255;// *count / 4;

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
			m_MapGraph.GetSize(&m_MapGraphXSize, &m_MapGraphYSize);
		}
		{
			int OneSize = 0;
			for (int y = 0; y < Size; y++) {
				for (int x = 0; x < Size; x++) {
					if (m_MazeControl.PosIsPath(x, y)) {
						OneSize++;
					}
				}
			}
			//
			this->m_ObjBuilds.resize(OneSize);
			{
				VECTOR_ref BasePos;
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

							BasePos.Set(tileSize * (float)x, 0.f, tileSize * (float)y);
							BasePos -= VECTOR_ref::vget(tileSize * (float)Size / 2.f, 0.f, tileSize * (float)Size / 2.f);

							this->m_ObjBuilds[loop].Set(this->m_ObjBuildBase, this->m_ColBuildBase, ID);
							this->m_ObjBuilds[loop].SetPosition(BasePos, deg2rad(deg));

							loop++;
						}
					}
				}
			}
		}
		MATERIALPARAM Material_t;
		Material_t.Diffuse = GetLightDifColor();
		Material_t.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material_t.Ambient = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);;
		Material_t.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material_t.Power = 20.0f;
		SetMaterialParam(Material_t);
		{
			Light_Graph = GraphHandle::Load("data/Picture/light.png");
			int OneSize = 0;
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

						OneSize++;
					}
				}
			}
			this->m_LightPoiont.resize(OneSize);
			VECTOR_ref BasePos;
			int loop = 0;
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

						BasePos.Set(tileSize * (float)x, 0.f, tileSize * (float)y);
						BasePos -= VECTOR_ref::vget(tileSize * (float)(Size) / 2.f, 0.f, tileSize * (float)(Size) / 2.f);
						BasePos.yadd(2.65f*Scale_Rate);

						this->m_LightPoiont[loop].m_Pos = BasePos;
						this->m_LightPoiont[loop].m_Obj = this->m_ObjLightBase.Duplicate();
						this->m_LightPoiont[loop].m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(0));
						loop++;
					}
				}
			}
			for (int i = 0; i < 2; i++) {
				m_LightHandle[i] = CreateSpotLightHandle(
					this->m_LightPoiont[i].m_Pos.get(),
					VGet(0.0f, -1.0f, 0.0f),
					DX_PI_F / 4.0f,
					DX_PI_F / 6.0f,
					5.0f*Scale_Rate,
					0.0f,
					0.006f,
					0.0f);
				SetLightDifColorHandle(m_LightHandle[i], GetColorF(1.f, 1.f, 1.f, 1.f));
				SetLightSpcColorHandle(m_LightHandle[i], GetColorF(0.01f, 0.01f, 0.01f, 0.f));
				SetLightAmbColorHandle(m_LightHandle[i], GetColorF(0.1f, 0.1f, 0.1f, 1.f));
			}
			ChangeLightTypeSpot(this->m_LightPoiont[3].m_Pos.get(),
				VGet(0.0f, -1.0f, 0.0f),
				DX_PI_F / 4.0f,
				DX_PI_F / 6.0f,
				5.0f*Scale_Rate,
				0.0f,
				0.006f,
				0.0f);
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.01f, 0.01f, 0.01f, 0.f));
			SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));
		}
	}
	void			BuildControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		for (int i = 0; i < this->m_LightPoiont.size(); i++) {
			auto& o = this->m_LightPoiont[i];

			o.m_SlingZrad.Execute();

			o.m_SlingZrad.AddRad(1.f / FPS * o.m_SlingPower);
			o.m_SlingPower = 0.f;

			if (o.m_isHit) {
				o.m_Pos.yadd(o.m_Yadd);
				o.m_Yadd += M_GR / (FPS * FPS);
				o.m_EraseTimer -= 1.f / FPS;
			}


			o.m_SlingZrad.GetRad();

			auto Vec = (o.m_Pos - DrawParts->GetMainCamera().GetCamPos());
			o.m_Obj.SetMatrix(MATRIX_ref::RotAxis(Vec.Norm(), o.m_SlingZrad.GetRad()) * MATRIX_ref::Mtrans(o.m_Pos));
			o.m_length = Vec.Length();

			if (o.m_EraseTimer <= 0.f) {
				std::swap(o, this->m_LightPoiont.back());
				this->m_LightPoiont.pop_back();
				i--;
			}
		}
		std::sort(this->m_LightPoiont.begin(), this->m_LightPoiont.end(), [&](const Lights& a, const Lights& b) { return a.m_length < b.m_length && !a.m_isHit; });

		int count = 0;
		for (auto& o : this->m_LightPoiont) {
			auto vec1 = (DrawParts->GetMainCamera().GetCamVec() - DrawParts->GetMainCamera().GetCamPos());
			auto vec2 = (o.m_Pos - DrawParts->GetMainCamera().GetCamPos()); vec2.y(0.f);
			float dot = vec1.Norm().dot(vec2.Norm());
			if (dot > std::sin(deg2rad(-60))) {
				auto Vec = MATRIX_ref::Vtrans(VECTOR_ref::up()*-1.f, MATRIX_ref::RotAxis((o.m_Pos - DrawParts->GetMainCamera().GetCamPos()).Norm(), o.m_SlingZrad.GetRad()));
				if (count <= 1) {
					SetLightPositionHandle(m_LightHandle[count], o.m_Pos.get());
					SetLightDirectionHandle(m_LightHandle[count], Vec.get());
				}
				else {
					SetLightPosition(o.m_Pos.get());
					SetLightDirection(Vec.get());
				}
				count++;
				if (count >= 2) { break; }
			}
		}
	}
	void			BuildControl::Draw() noexcept {
		auto* DrawParts = DXDraw::Instance();
		int fog_enable = 0;
		int fog_mode = 0;
		int fog_r = 0, fog_g = 0, fog_b = 0;
		float fog_start = 0.f, fog_end = 0.f;
		float fog_density = 0.f;

		fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
		fog_mode = GetFogMode();														// フォグモードを取得する
		GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
		GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
		fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

		SetFogEnable(TRUE);
		SetFogMode(DX_FOGMODE_EXP2);
		SetFogStartEnd(Scale_Rate*6.f, Scale_Rate*10.f);
		SetFogColor(26, 29, 20);
		SetFogDensity(0.01f);
		for (auto& b : this->m_ObjBuilds) {
			auto Pos = b.GetObj().GetMatrix().pos();
			auto Vec = (Pos - DrawParts->GetMainCamera().GetCamPos()); Vec.y(0.f);
			auto Len = Vec.Length();
			if (Len <= 15.f*Scale_Rate) {
				if (CheckCameraViewClip_Box(
					(Pos + VECTOR_ref::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
					(Pos + VECTOR_ref::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
					) {
					b.GetObj().DrawMesh(b.GetMeshSel());
				}
			}
		}
		for (auto& b : this->m_LightPoiont) {
			auto Pos = b.m_Pos;
			auto Vec = (Pos - DrawParts->GetMainCamera().GetCamPos()); Vec.y(0.f);
			auto Len = Vec.Length();
			if (Len <= 15.f*Scale_Rate) {
				if (CheckCameraViewClip_Box(
					(Pos + VECTOR_ref::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
					(Pos + VECTOR_ref::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
					) {
					b.m_Obj.DrawModel();
				}
			}
		}

		SetFogEnable(fog_enable);
		SetFogMode(fog_mode);
		SetFogColor(fog_r, fog_g, fog_b);
		SetFogStartEnd(fog_start, fog_end);
		SetFogDensity(fog_density);
		//
		SetUseLighting(FALSE);
		float rad = -(DrawParts->GetMainCamera().GetCamVec() - DrawParts->GetMainCamera().GetCamPos()).cross(VECTOR_ref::up()).dot(DrawParts->GetMainCamera().GetCamUp());
		int max = (int)this->m_LightPoiont.size();
		for (int i = max - 1; i >= 0; i--) {
			auto& o = this->m_LightPoiont[i];
			if (!o.m_isHit) {
				auto Pos = o.m_Pos;
				auto Vec = (Pos - DrawParts->GetMainCamera().GetCamPos()); Vec.y(0.f);
				auto Len = Vec.Length();
				if (Len <= 15.f*Scale_Rate) {
					if (CheckCameraViewClip_Box(
						(Pos + VECTOR_ref::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
						(Pos + VECTOR_ref::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
						) {
						float per = 0.5f*std::clamp((Len - 50.f) / 30.f, 0.f, 1.f);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
						DrawBillboard3D(o.m_Pos.get(), 0.5f, 1.f, 2.2f*Scale_Rate, rad + o.m_SlingZrad.GetRad(), Light_Graph.get(), TRUE);
					}
				}
			}
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		SetUseLighting(TRUE);
	}
	void			BuildControl::Dispose(void) noexcept {
		this->m_ObjBuildBase.Dispose();
		this->m_ColBuildBase.Dispose();
		m_MazeControl.Reset();
		for (auto& b : this->m_LightPoiont) {
			b.m_Obj.Dispose();
		}
		m_LightPoiont.clear();
		Light_Graph.Dispose();
	}
};
