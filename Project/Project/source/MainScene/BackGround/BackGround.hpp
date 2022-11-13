#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGround2;
			MV1							m_ObjGroundCol;
			//Grass						m_grass;
			//Box2DWall					m_Box2DWall;
			BreakWall					m_BreakWall;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }

			const std::shared_ptr<b2World>&	GetBox2Dworld(void) noexcept {
				return nullptr;
				//return this->m_Box2DWall.GetBox2Dworld();
			}

			const auto&		GetWallGroundCol(int id) noexcept { return this->m_BreakWall.GetWallGroundCol(id); }
			const auto		GetWallGroundColNum() const noexcept { return this->m_BreakWall.GetWallGroundColNum(); }
			const auto		GetWallCol(const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius) noexcept { return this->m_BreakWall.GetWallCol(repos, pos, norm, radius); }
		private:
			void			DrawCommon() noexcept {
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				DebugParts->end_way();
#endif // DEBUG

				this->m_ObjGround2.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-318.f, -12.5f, 897.f)));
				this->m_ObjGround2.DrawModel();

				SetFogEnable(TRUE);
				SetFogColor(0, 0, 0);
				SetFogDensity(0.5f);

				this->m_ObjGround.DrawModel();
				this->m_BreakWall.DrawCommon();

				SetUseBackCulling(TRUE);
				SetFogEnable(FALSE);

#ifdef DEBUG
				DebugParts->end_way();
#endif // DEBUG
			}
		public://
			//
			void			Init(void) noexcept {
				//地形
				MV1::Load("data/model/ground/model.mv1", &this->m_ObjGround2);
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				//空
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));

				//Box2D壁
				//this->m_Box2DWall.Init();
				//草
				//this->m_grass.Init(&this->m_ObjGroundCol);
				//壁
				this->m_BreakWall.Init();
			}
			//
			void			FirstExecute(void) noexcept {
				//this->m_Box2DWall.GetBox2Dworld()->Step(1.f, 1, 1);//物理更新
			}
			//
			void			Execute(void) noexcept {
				this->m_BreakWall.Execute();
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void			Shadow_Draw(void) noexcept {
				//DrawCommon();
			}
			void			Draw(void) noexcept {
				DrawCommon();
				//this->m_grass.Draw();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGround2.Dispose();
				this->m_ObjGroundCol.Dispose();
				//this->m_grass.Dispose();
				//this->m_Box2DWall.Dispose();
				this->m_BreakWall.Dispose();
			}
		};
	};
};
