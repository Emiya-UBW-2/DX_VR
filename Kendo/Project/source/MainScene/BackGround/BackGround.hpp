#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		class BackGroundClass : public SingletonBase<BackGroundClass> {
		private:
			friend class SingletonBase<BackGroundClass>;
		private:
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGroundShadow;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;

		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;
		public://getter
			const auto& GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
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

		public:
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) const noexcept;
		public://
			void			Load(bool isTraining) noexcept {
				if (isTraining) {
					MV1::Load("data/model/map/model_DISABLE.mv1", &this->m_ObjGroundShadow);
					MV1::Load("data/model/map/model_DISABLE.mv1", &this->m_ObjGround);
				}
				else {
					MV1::Load("data/model/budokan/Shadow.mv1", &this->m_ObjGroundShadow);
					MV1::Load("data/model/budokan/model_DISABLE.mv1", &this->m_ObjGround);
				}
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			}
			//
			void			Init(void) noexcept {
				//’nŒ`
				this->m_ObjGroundCol.SetupCollInfo(1, 1, 1);
				//‹ó
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				//this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
					//this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.75f, 0.75f, 0.75f, 1.f));
					//this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
					//this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
				}
				for (int i = 0, num = this->m_ObjGround.GetMaterialNum(); i < num; ++i) {
					//this->m_ObjGround.SetMaterialDifColor(i, GetColorF(1.f, 1.f, 1.f, 1.f));
					//this->m_ObjGround.SetMaterialAmbColor(i, GetColorF(0.15f, 0.15f, 0.15f, 1.f));
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
				this->m_ObjGroundShadow.DrawModel();
			}
			void			Shadow_Draw(void) noexcept {
				this->m_ObjGroundShadow.DrawModel();
			}
			void			Draw(void) noexcept {
				this->m_ObjGround.DrawModel();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGroundShadow.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
			}
		};
	}
}
