
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
			MV1							m_ObjGroundCol_Box2D;
			std::shared_ptr<b2World>	m_b2world;
			std::vector<std::pair<b2Pats, std::array<VECTOR_ref, 2>>>	m_b2wallParts;	//壁をセット
			Grass						m_grass;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			auto&			GetBox2Dworld(void) noexcept { return this->m_b2world; }
		private:
			void			DrawCommon(void) noexcept {
				this->m_ObjGround.DrawModel();
			}
		public://
			//
			void			Init(void) noexcept {
				//地形
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				MV1::Load("data/model/map/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);
				//空
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//壁
				this->m_b2world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); // 剛体を保持およびシミュレートするワールドオブジェクトを構築
				MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE);
				for (int i = 0; i < p.PolygonNum; i++) {
					this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
					this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
						this->m_b2wallParts.pop_back();
					}
					this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
					this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
					if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
						this->m_b2wallParts.pop_back();
					}
					this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
					this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
					this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
						this->m_b2wallParts.pop_back();
					}
				}
				for (auto& w : this->m_b2wallParts) {
					std::array<b2Vec2, 2> vs;								//
					vs[0].Set(w.second[0].x(), w.second[0].z());			//
					vs[1].Set(w.second[1].x(), w.second[1].z());			//
					b2ChainShape chain;										// This a chain shape with isolated vertices
					chain.CreateChain(&vs[0], 2);							//
					b2FixtureDef fixtureDef;								//動的ボディフィクスチャを定義します
					fixtureDef.shape = &chain;								//
					fixtureDef.density = 1.0f;								//ボックス密度をゼロ以外に設定すると、動的になります
					fixtureDef.friction = 0.3f;								//デフォルトの摩擦をオーバーライドします
					b2BodyDef bodyDef;										//ダイナミックボディを定義します。その位置を設定し、ボディファクトリを呼び出します
					bodyDef.type = b2_staticBody;							//
					bodyDef.position.Set(0, 0);								//
					bodyDef.angle = 0.f;									//
					w.first.Set(this->m_b2world->CreateBody(&bodyDef), &chain);	//
				}
				//
				this->m_grass.Init(&this->m_ObjGroundCol);
			}
			//
			void			Execute(void) noexcept {}
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
				DrawCommon();
			}
			void			Draw(void) noexcept {
				DrawCommon();
				this->m_grass.Draw();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				this->m_ObjGroundCol_Box2D.Dispose();
				this->m_grass.Dispose();
				for (auto& w : this->m_b2wallParts) {
					w.first.Dispose();
					w.second[0].clear();
					w.second[1].clear();
				}
				this->m_b2wallParts.clear();
				this->m_b2world.reset();
			}
		};
	};
};