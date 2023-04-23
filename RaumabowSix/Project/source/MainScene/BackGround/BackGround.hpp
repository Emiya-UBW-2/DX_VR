#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
		private:
			void			DrawCommon() noexcept {
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				DebugParts->SetPoint("");
#endif // DEBUG

				SetFogEnable(TRUE);
				SetFogColor(0, 0, 0);
				SetFogDensity(0.5f);

				this->m_ObjGround.DrawModel();

				SetUseBackCulling(TRUE);
				SetFogEnable(FALSE);

#ifdef DEBUG
				DebugParts->SetPoint("");
#endif // DEBUG
			}
		public://
			//
			void			Init(void) noexcept {
				//地形
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
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
			}
		};
	};
};
