
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
			std::vector<std::pair<b2Pats, std::array<VECTOR_ref, 2>>>	m_b2wallParts;	//�ǂ��Z�b�g
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
				//�n�`
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				MV1::Load("data/model/map/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);
				//��
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//��
				this->m_b2world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); // ���̂�ێ�����уV�~�����[�g���郏�[���h�I�u�W�F�N�g���\�z
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
					b2FixtureDef fixtureDef;								//���I�{�f�B�t�B�N�X�`�����`���܂�
					fixtureDef.shape = &chain;								//
					fixtureDef.density = 1.0f;								//�{�b�N�X���x���[���ȊO�ɐݒ肷��ƁA���I�ɂȂ�܂�
					fixtureDef.friction = 0.3f;								//�f�t�H���g�̖��C���I�[�o�[���C�h���܂�
					b2BodyDef bodyDef;										//�_�C�i�~�b�N�{�f�B���`���܂��B���̈ʒu��ݒ肵�A�{�f�B�t�@�N�g�����Ăяo���܂�
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