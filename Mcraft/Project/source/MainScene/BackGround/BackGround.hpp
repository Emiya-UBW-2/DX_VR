#pragma once
#pragma warning(disable:4464)
#include "../../Header.hpp"
#include "../../MainScene/BackGround/BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		class BackGroundClass : public SingletonBase<BackGroundClass> {
		private:
			friend class SingletonBase<BackGroundClass>;
		private:
		private:
			MV1							m_ObjSky;
			//MV1							m_ObjGround;
			//MV1							m_ObjGroundCol;

			//頂点データ
			std::vector<VERTEX3D> vert32;
			//頂点インデックスデータ
			std::vector<uint32_t> index32;

			struct CellData {
				int selset = INVALID_ID;
			};

			std::vector<CellData> m_Cell;
			int Xall = 1000;
			int Yall = 200;
			int Zall = 1000;

			const auto& GetCell(int x, int y, int z) const noexcept {
				return m_Cell.at(std::clamp(x + Xall / 2, 0, Xall - 1) + Xall * std::clamp(y + Yall / 2, 0, Yall - 1) + Xall * Yall * std::clamp(z + Zall / 2, 0, Zall - 1));
			}
			auto& SetCell(int x, int y, int z) noexcept {
				return m_Cell.at(std::clamp(x + Zall / 2, 0, Xall - 1) + Xall * std::clamp(y + Yall / 2, 0, Yall - 1) + Xall * Yall * std::clamp(z + Zall / 2, 0, Zall - 1));
			}
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
		private:
			void AddSquare(const Matrix4x4DX& movement, COLOR_U8 color) noexcept;
			void AddCube(const Matrix4x4DX& scaleMat, const Matrix4x4DX& movement, COLOR_U8 color) noexcept;
			void AddCube(int x, int y, int z, float scale, COLOR_U8 color) noexcept;
		public://getter
			//const auto& GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			/*
			auto		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, bool isNearest, Vector3DX* Normal = nullptr) const noexcept {
				bool isHit = false;
				{
					auto col_p = this->m_ObjGroundCol.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (isNearest) {
							*EndPos = col_p.HitPosition;
							if (Normal) { *Normal = col_p.Normal; }
						}
						else {
							return isHit;
						}
					}
				}
				return isHit;
			}
			//*/
		public:
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) const noexcept;
		public://
			void			Load(void) noexcept {
				//MV1::Load("data/model/map/model_DISABLE.mv1", &this->m_ObjGround);
				//MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			}
			//
			void			Init(void) noexcept {
				//地形
				//this->m_ObjGroundCol.SetupCollInfo(1, 1, 1);
				//空
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
					this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
					this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
				}
				/*
				for (int i = 0, num = this->m_ObjGround.GetMaterialNum(); i < num; ++i) {
					this->m_ObjGround.SetMaterialDifColor(i, GetColorF(1.f, 1.f, 1.f, 1.f));
					this->m_ObjGround.SetMaterialAmbColor(i, GetColorF(0.15f, 0.15f, 0.15f, 1.f));
				}
				//*/
				m_Cell.resize(Xall * Yall * Zall);
				for (int x = -Xall / 2; x < Xall / 2; x++) {
					for (int y = -Yall / 2; y < Yall / 2; y++) {
						for (int z = -Zall / 2; z < Zall / 2; z++) {
							int set = INVALID_ID;
							if (y == -1) {
								set = 0;
							}
							SetCell(x, y, z).selset = set;
						}
					}
				}
			}
			//
			void			FirstExecute(void) noexcept {
			}
			//
			void			Execute(void) noexcept {
				vert32.clear();
				index32.clear();
				const COLOR_U8 Colors[] = {
					GetColorU8(255,  0,  0, 255),
					GetColorU8(255,255,  0, 255),
					GetColorU8(0,255,  0, 255),
					GetColorU8(0,255,255, 255),
					GetColorU8(0,  0,255, 255),
					GetColorU8(255,  0,255, 255),
				};

				auto* DrawParts = DXDraw::Instance();
				Vector3DX CamPos = DrawParts->GetMainCamera().GetCamPos();
				Vector3DX CamVec = DrawParts->GetMainCamera().GetCamVec() - CamPos;
				int total = 4;
				float MulPer = 2.f;
				float scale = Scale_Rate / 2.f * 0.5f * powf(MulPer, (float)(total - 1));
				float Max = 15.f;
				for (int i = 0; i < total; i++) {
					bool IsLast = (i == (total - 1));
					Vector3DX center = CamPos / scale;

					for (int x = (int)(center.x - Max); x <= (int)(center.x + Max); x++) {
						for (int y = (int)(center.y - Max); y <= (int)(center.y + Max); y++) {
							for (int z = (int)(center.z - Max); z <= (int)(center.z + Max); z++) {
								if (!IsLast
									&& (((int)(center.x - Max / MulPer) < x - 1) && (x + 1 < (int)(center.x + Max / MulPer)))
									&& (((int)(center.y - Max / MulPer) < y - 1) && (y + 1 < (int)(center.y + Max / MulPer)))
									&& (((int)(center.z - Max / MulPer) < z - 1) && (z + 1 < (int)(center.z + Max / MulPer)))
									) {
									continue;
								}
								const auto& Cell = GetCell(x, y, z);
								if (Cell.selset == INVALID_ID) { continue; }

								Vector3DX Pos = Vector3DX::vget((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f);
								Vector3DX Vec = Pos - center;

								if (Vector3DX::Dot(Vec, CamVec) < 0.f) { continue; }

								AddCube(x, y, z, scale, Colors[i]);
							}
						}
					}
					scale /= MulPer;
				}
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_Far(void) noexcept {
			}
			void			Shadow_Draw(void) noexcept {
				if (vert32.size() > 0 && index32.size() > 0) {
					DrawPolygon32bitIndexed3D(vert32.data(), (int)vert32.size(), index32.data(), (int)index32.size() / 3, DX_NONE_GRAPH, FALSE);
				}
			}
			void			Draw(void) noexcept {
				if (vert32.size() > 0 && index32.size() > 0) {
					DrawPolygon32bitIndexed3D(vert32.data(), (int)vert32.size(), index32.data(), (int)index32.size() / 3, DX_NONE_GRAPH, FALSE);
				}
				//this->m_ObjGround.DrawModel();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				//this->m_ObjGround.Dispose();
				//this->m_ObjGroundCol.Dispose();
			}
		};
	}
}
