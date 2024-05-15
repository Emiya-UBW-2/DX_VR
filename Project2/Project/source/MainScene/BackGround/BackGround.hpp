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
			MV1							m_ObjGroundCol_Box2D;

			std::vector<Vector3DX>		m_WayPoint;
			BuildControl				m_BuildControl;
			Box2DWall					m_Box2DWall;
		public://getter
			const auto&		GetWayPoint(void) noexcept { return this->m_WayPoint; }
			const std::shared_ptr<b2World>&	GetBox2Dworld(void) noexcept {
				return this->m_Box2DWall.GetBox2Dworld();
				return nullptr;
			}
			const auto		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, bool isNearest, bool isOnlyGround, Vector3DX* Normal = nullptr) {
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
				if (isOnlyGround) {
					return isHit;
				}
				for (auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() < 0) { continue; }
					if (GetMinLenSegmentToPoint(StartPos, *EndPos, bu.GetMatrix().pos()) >= 20.f*Scale_Rate) { continue; }
					auto col_p = bu.GetCol(StartPos, *EndPos);
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

			const auto		GetRoadPointNum() const noexcept {
				int count = 0;
				for (const auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() == 0) {
						count++;
					}
				}
				return count;
			}
			const Matrix4x4DX*GetRoadPoint(int ID) const noexcept {
				int count = 0;
				for (const auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() == 0) {
						if (ID == count) {
							return &bu.GetMatrix();
						}
						count++;
					}
				}
				return nullptr;
			}
		public://
			void			Load(void) noexcept {
				MV1::Load("data/model/map_old/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map_old/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/map_old/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);

				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);

				this->m_BuildControl.Load();
			}
			//
			void			Init(void) noexcept {
				//地形
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				this->m_BuildControl.Init(&this->m_ObjGroundCol);
				for (auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() == 3 || bu.GetMeshSel() == 0) {
						Vector3DX pos_t = bu.GetMatrix().pos();
						float offset = 5.f*Scale_Rate;
						if (
							(-300.f*Scale_Rate / 2.f + offset < pos_t.x && pos_t.x < 300.f*Scale_Rate / 2.f - offset) &&
							(-300.f*Scale_Rate / 2.f + offset < pos_t.z && pos_t.z < 300.f*Scale_Rate / 2.f - offset)
							) {
							this->m_WayPoint.emplace_back(bu.GetMatrix().pos() + Vector3DX::up()*(1.f*Scale_Rate));
						}
					}
				}
				//空
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//Box2D壁
				{
					this->m_Box2DWall.Add(MV1GetReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE));
					MV1TerminateReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE);
					for (auto& bu : this->m_BuildControl.GetBuildCol()) {
						if (bu.GetMeshSel() >= 5) {
							this->m_Box2DWall.Add(MV1GetReferenceMesh(bu.GetColBox2D().get(), bu.GetMeshSel(), TRUE));
							MV1TerminateReferenceMesh(bu.GetColBox2D().get(), bu.GetMeshSel(), TRUE);
						}
					}
					this->m_Box2DWall.Init();
				}
			}
			//
			void			FirstExecute(void) noexcept {
				this->m_Box2DWall.GetBox2Dworld()->Step(1.f, 1, 1);//物理更新
			}
			//
			void			Execute(void) noexcept {
				//this->m_BreakWall.Execute();
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_Far(void) noexcept {
				this->m_ObjGround.DrawModel();
				this->m_BuildControl.Draw();
			}
			void			Shadow_Draw_NearFar(void) noexcept {
			}
			void			Shadow_Draw(void) noexcept {
			}
			void			Draw(void) noexcept {
				this->m_ObjGround.DrawModel();
				this->m_BuildControl.Draw();
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
