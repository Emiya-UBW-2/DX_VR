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

				for (auto& bu : this->m_BuildControl.GetBuildCol()) {
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
				return isHit;
			}
		private:
			void			DrawCommon() noexcept {
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				DebugParts->SetPoint("");
#endif // DEBUG

				SetFogEnable(TRUE);
				SetFogColor(24, 20, 0);
				SetFogDensity(0.5f);

				this->m_ObjGround.DrawModel();
				this->m_BuildControl.Draw();

				SetUseBackCulling(TRUE);
				SetFogEnable(FALSE);

#ifdef DEBUG
				DebugParts->SetPoint("");
#endif // DEBUG
			}
		public://
			//
			void			Init(void) noexcept {
				this->m_BuildControl.Load();
				//
				//地形
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				this->m_BuildControl.Init(&this->m_ObjGroundCol);
				//空
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
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				this->m_BuildControl.Dispose();
			}
		};
	};
};
