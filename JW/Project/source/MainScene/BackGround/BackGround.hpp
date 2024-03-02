#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGroundSub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase {
		protected:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
		protected:
			virtual void			Init_Sub(void) noexcept {}
			//
			virtual void			FirstExecute_Sub(void) noexcept {}
			//
			virtual void			Execute_Sub(void) noexcept {}
			//
			virtual void			BG_Draw_Sub(void) noexcept {}
			virtual void			Shadow_Draw_Far_Sub(void) noexcept {}
			virtual void			Shadow_Draw_NearFar_Sub(void) noexcept {}
			virtual void			Shadow_Draw_Sub(void) noexcept {}
			virtual void			Draw_Sub(void) noexcept {}
			//
			virtual void			Dispose_Sub(void) noexcept {}
			virtual bool			CheckLinetoMap_Sub(const VECTOR_ref&, VECTOR_ref*, bool, VECTOR_ref*, MV1_COLL_RESULT_POLY*) const noexcept { return false; }
		public://
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			const auto		CheckLinetoMap(const VECTOR_ref& StartPos, VECTOR_ref* EndPos, bool isNearest, VECTOR_ref* Normal = nullptr, MV1_COLL_RESULT_POLY* Ret = nullptr) const noexcept {
				bool isHit = false;
				if (this->m_ObjGroundCol.IsActive()) {
					auto col_p = this->m_ObjGroundCol.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						*EndPos = col_p.HitPosition;
						if (Ret) { *Ret = col_p; }
						if (Normal) { *Normal = col_p.Normal; }
						if (!isNearest) {
							return isHit;
						}
					}
				}
				isHit |= CheckLinetoMap_Sub(StartPos, EndPos, isNearest, Normal, Ret);
				return isHit;
			}
		protected:
			void			DrawCommon() noexcept {
				if (this->m_ObjGround.IsActive()) {
					SetFogEnable(TRUE);
					auto* DrawParts = DXDraw::Instance();
					auto far_t = DrawParts->GetMainCamera().GetCamFar();
					SetFogStartEnd(far_t / 3.f, far_t);
					SetFogColor(26, 29, 20);

					this->m_ObjGround.DrawModel();

					SetUseBackCulling(TRUE);
					SetFogEnable(FALSE);
				}
			}
		public://
			//
			void			Init(const char* MapPath, const char* SkyPath) noexcept {
				//’nŒ`
				std::string path;
				path = MapPath;
				if (path != "") {
					path += "model.mv1";
					MV1::Load(path, &this->m_ObjGround, DX_LOADMODEL_PHYSICS_DISABLE);
				}
				path = MapPath;
				if (path != "") {
					path += "col.mv1";
					MV1::Load(path, &this->m_ObjGroundCol, DX_LOADMODEL_PHYSICS_DISABLE);
					this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				}
				//‹ó
				path = SkyPath;
				if (path != "") {
					path += "model.mv1";
					MV1::Load(path, &this->m_ObjSky, DX_LOADMODEL_PHYSICS_DISABLE);
					this->m_ObjSky.SetScale(VECTOR_ref::vget(10.f, 10.f, 10.f));
					MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				}
				//
				Init_Sub();
			}
			//
			void			FirstExecute(void) noexcept {
				FirstExecute_Sub();
			}
			//
			void			Execute(void) noexcept {
				Execute_Sub();
			}
			//
			void			BG_Draw(void) noexcept {
				if (this->m_ObjSky.IsActive()) {
					SetUseLighting(FALSE);
					this->m_ObjSky.DrawModel();
					SetUseLighting(TRUE);
				}
				BG_Draw_Sub();
			}
			void			Shadow_Draw_Far(void) noexcept {
				if (this->m_ObjGround.IsActive()) {
					this->m_ObjGround.DrawModel();
				}
				Shadow_Draw_Far_Sub();
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
				Shadow_Draw_NearFar_Sub();
			}
			void			Shadow_Draw(void) noexcept {
				//DrawCommon();
				Shadow_Draw_Sub();
			}
			void			Draw(void) noexcept {
				DrawCommon();
				Draw_Sub();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				Dispose_Sub();
			}
		};
		//
		class BackGroundClassMain : public BackGroundClassBase {
		private:
			BuildControl				m_BuildControl;
		public://
			const auto&		GetNearestLight(int No) noexcept { return this->m_BuildControl.GetNearestLight(No); }

			const auto&		GetBuildData(void) noexcept { return this->m_BuildControl.GetBuildDatas(); }
			const auto&		GetDrumControl(void) noexcept { return this->m_BuildControl.GetDrumControl(); }

			const auto&		GetMapGraph(void) noexcept { return this->m_BuildControl.GetMapGraph(); }
			const auto&		GetMapGraphXSize(void) noexcept { return this->m_BuildControl.GetMapGraphXSize(); }
			const auto&		GetMapGraphYSize(void) noexcept { return this->m_BuildControl.GetMapGraphYSize(); }
			const int		GetNearestBuilds(const VECTOR_ref& NowPosition) noexcept {
				return this->m_BuildControl.GetNearestBuilds(NowPosition);
			}

			bool			HitLightCheck(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept {
				return this->m_BuildControl.HitLightCheck(StartPos, pEndPos);
			}
			bool CheckPolyMoveWidth(VECTOR_ref StartPos, int TargetIndex, float Width) const {
				return this->m_BuildControl.CheckPolyMoveWidth(StartPos, TargetIndex, Width);
			}
		public://
			//
			void			Init_Sub(void) noexcept override {
				this->m_BuildControl.Load();//1.13•b
				this->m_BuildControl.Init();//0.4•b
			}
			//
			void			Execute_Sub(void) noexcept override {
				this->m_BuildControl.Execute();
			}
			//
			void			Shadow_Draw_Far_Sub(void) noexcept override {}
			void			Shadow_Draw_Sub(void) noexcept override {
				this->m_BuildControl.ShadowDraw();
			}
			void			Draw_Sub(void) noexcept override {
				this->m_BuildControl.Draw();
			}
			//
			void			Dispose_Sub(void) noexcept override {
				this->m_BuildControl.Dispose();
			}

			bool			CheckLinetoMap_Sub(const VECTOR_ref& StartPos, VECTOR_ref* EndPos, bool isNearest, VECTOR_ref* Normal, MV1_COLL_RESULT_POLY* Ret) const noexcept override {
				bool isHit = false;
				for (const auto& bu : this->m_BuildControl.GetBuildDatas()) {
					if (bu.GetMeshSel() < 0) { continue; }
					if (GetMinLenSegmentToPoint(StartPos, *EndPos, bu.GetMatrix().pos()) >= 5.f*Scale_Rate) { continue; }
					auto col_p = bu.GetColLine(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						*EndPos = col_p.HitPosition;
						if (Ret) { *Ret = col_p; }
						if (Normal) { *Normal = col_p.Normal; }
						if (!isNearest) {
							return isHit;
						}
					}
				}
				for (const auto& C : this->m_BuildControl.GetDrumControl().GetDrum()) {
					if (GetMinLenSegmentToPoint(StartPos, *EndPos, C.m_Pos) >= 3.f*Scale_Rate) { continue; }
					auto col_p = C.m_Col.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						*EndPos = col_p.HitPosition;
						if (Ret) { *Ret = col_p; }
						if (Normal) { *Normal = col_p.Normal; }
						if (!isNearest) {
							return isHit;
						}
					}
				}
				return isHit;
			}
		};
		//
		class BackGroundClassTutorial : public BackGroundClassBase {
		private:
		public://
			//
			void			Init_Sub(void) noexcept override {
				MV1SetMaterialDrawAlphaTestAll(this->m_ObjGround.get(), TRUE, DX_CMP_GREATER, 216);
			}
			//
			void			Execute_Sub(void) noexcept override {}
			//
			void			Shadow_Draw_Far_Sub(void) noexcept override {}
			void			Shadow_Draw_Sub(void) noexcept override {
				DrawCommon();
			}
			void			Draw_Sub(void) noexcept override {}
			//
			void			Dispose_Sub(void) noexcept override {}
		};
		//
	};
};
