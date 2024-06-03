#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/BackGround/BackGroundSub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;

		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			const auto		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, bool isNearest, Vector3DX* Normal = nullptr) {
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

		public://
			void			Load(void) noexcept {
				MV1::Load("data/model/map/model_DISABLE.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			}
			//
			void			Init(void) noexcept {
				//’nŒ`
				this->m_ObjGroundCol.SetupCollInfo(1, 1, 1);
				//‹ó
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				{
					int num = MV1GetMaterialNum(this->m_ObjGround.get());
					for (int i = 0; i < num; i++) {
						MV1SetMaterialDifColor(this->m_ObjGround.get(), i, GetColorF(1.f, 1.f, 1.f, 1.f));
						MV1SetMaterialAmbColor(this->m_ObjGround.get(), i, GetColorF(0.15f, 0.15f, 0.15f, 1.f));
					}
				}
				{
					int num = MV1GetMaterialNum(this->m_ObjSky.get());
					for (int i = 0; i < num; i++) {
						MV1SetMaterialDifColor(this->m_ObjSky.get(), i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
						MV1SetMaterialAmbColor(this->m_ObjSky.get(), i, GetColorF(0.f, 0.f, 0.f, 1.f));
					}
				}
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
			void			Shadow_Draw_Far(void) noexcept {
				this->m_ObjGround.DrawModel();
			}
			void			Shadow_Draw(void) noexcept {
			}
			void			Draw(void) noexcept {
				this->m_ObjGround.DrawModel();
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
