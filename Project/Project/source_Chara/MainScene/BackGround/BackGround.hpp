
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
			MV1 Sky;
			MV1 Ground;
			MV1 GroundCol;

			std::shared_ptr<b2World> world;
			struct wallPats {
				b2Pats b2;
				std::array<VECTOR_ref, 2> pos;
			};
			std::vector<wallPats> wall;					//�ǂ��Z�b�g
		public:
			void Load(void) noexcept {
				//�n�`
				MV1::Load("data/model/map/model.mv1", &Ground);
				MV1::Load("data/model/map/col.mv1", &GroundCol);
				GroundCol.SetupCollInfo(64, 16, 64);
				//��
				MV1::Load("data/model/sky/model.mv1", &Sky);
				MV1SetDifColorScale(Sky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));

				this->world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); /* ���̂�ێ�����уV�~�����[�g���郏�[���h�I�u�W�F�N�g���\�z*/
				/*
				MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(mapcol_tank.get(), 0, FALSE);
				for (int i = 0; i < p.PolygonNum; i++) {
					//��
					{
						wall.resize(wall.size() + 1);
						wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
						wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
						if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
							wall.pop_back();
						}

						wall.resize(wall.size() + 1);
						wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
						wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
						if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
							wall.pop_back();
						}

						wall.resize(wall.size() + 1);
						wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
						wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
						if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
							wall.pop_back();
						}
					}
				}
				//*/
				for (auto& w : wall) {
					std::array<b2Vec2, 2> vs;
					vs[0].Set(w.pos[0].x(), w.pos[0].z());
					vs[1].Set(w.pos[1].x(), w.pos[1].z());
					b2ChainShape chain;		// This a chain shape with isolated vertices
					chain.CreateChain(&vs[0], 2);
					b2FixtureDef fixtureDef;			       /*���I�{�f�B�t�B�N�X�`�����`���܂�*/
					fixtureDef.shape = &chain;			       /**/
					fixtureDef.density = 1.0f;			       /*�{�b�N�X���x���[���ȊO�ɐݒ肷��ƁA���I�ɂȂ�܂�*/
					fixtureDef.friction = 0.3f;			       /*�f�t�H���g�̖��C���I�[�o�[���C�h���܂�*/
					b2BodyDef bodyDef;				       /*�_�C�i�~�b�N�{�f�B���`���܂��B���̈ʒu��ݒ肵�A�{�f�B�t�@�N�g�����Ăяo���܂�*/
					bodyDef.type = b2_staticBody;			       /**/
					bodyDef.position.Set(0, 0);			       /**/
					bodyDef.angle = 0.f;				       /**/
					w.b2.Set(this->world->CreateBody(&bodyDef), &chain);
				}
			}
			//
			void Execute(void) noexcept {
			}
			//DrawCall => 100
			void BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				Sky.DrawModel();
				SetUseLighting(TRUE);
			}
			//����
			void DrawCommon(void) noexcept {
				Ground.DrawModel();
			}
			void Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void Shadow_Draw(void) noexcept {
				DrawCommon();
			}
			void Draw(void) noexcept {
				DrawCommon();
			}

			const auto& GetGroundCol(void) noexcept { return GroundCol; }
			auto& GetBox2Dworld(void) noexcept { return world; }
		};
	};
};