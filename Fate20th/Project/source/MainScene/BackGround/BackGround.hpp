#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;

			BuildControl				m_BuildControl;
			WallLines					m_Box2DWall;
		public://getter
			const auto&		GetBuildCol(void) noexcept { return this->m_BuildControl.GetBuildCol(); }
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			const auto		CheckLinetoMap(const VECTOR_ref& StartPos, VECTOR_ref* EndPos, bool isNearest, VECTOR_ref* Normal = nullptr, MV1_COLL_RESULT_POLY* Ret = nullptr) {
				bool isHit = false;
				{
					auto col_p = this->m_ObjGroundCol.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (Ret) { *Ret = col_p; }
						if (isNearest) {
							*EndPos = col_p.HitPosition;
							if (Normal) { *Normal = col_p.Normal; }
						}
						else {
							return isHit;
						}
					}
				}
				//*
				for (auto& bu : this->GetBuildCol()) {
					if (bu.GetMeshSel() < 0) { continue; }
					if (GetMinLenSegmentToPoint(StartPos, *EndPos, bu.GetMatrix().pos()) >= 20.f*Scale_Rate) { continue; }
					auto col_p = bu.GetColLine(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (Ret) { *Ret = col_p; }
						if (isNearest) {
							*EndPos = col_p.HitPosition;
							if (Normal) { *Normal = col_p.Normal; }
						}
						else {
							return isHit;
						}
					}
				}
				//*/
				return isHit;
			}
		private:
			void			DrawCommon() noexcept {
				SetFogEnable(TRUE);
				SetFogColor(124, 120, 100);
				SetFogDensity(0.5f);

				this->m_ObjGround.DrawModel();
				this->m_BuildControl.Draw();

				SetUseBackCulling(TRUE);
				SetFogEnable(FALSE);
			}
		public://
			//
			void			Init(void) noexcept {
				this->m_BuildControl.Load();
				//
				//’nŒ`
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				this->m_BuildControl.Init(&this->m_ObjGroundCol);

				for (auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() >= 0) {
						this->m_Box2DWall.Add(MV1GetReferenceMesh(bu.GetWallLine().get(), bu.GetMeshSel(), TRUE));
						MV1TerminateReferenceMesh(bu.GetWallLine().get(), bu.GetMeshSel(), TRUE);
					}
				}
				this->m_Box2DWall.Init();
				//‹ó
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			}
			//
			void			FirstExecute(void) noexcept {
			}
			//
			void			Execute(void) noexcept {
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
				this->m_BuildControl.Draw();
			}
			void			Shadow_Draw(void) noexcept {
				//DrawCommon();
			}
			void			Draw(void) noexcept {
				DrawCommon();
			}
			//
			void			DrawWallUI(int x, int y, int size, float scale, const VECTOR_ref& Pos, float Yrad) noexcept {
				this->m_Box2DWall.Draw(x, y, size, scale, Pos, Yrad);
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				this->m_BuildControl.Dispose();
				this->m_Box2DWall.Dispose();
			}
		};
	};
};
