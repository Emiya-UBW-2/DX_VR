
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
			MV1 Sky;
			MV1 Ground;
			MV1 GroundCol;
			MV1 GroundPos;

			std::vector<MV1> Tower;
			VECTOR_ref minTower, maxTower;
			MV1 Build;
			VECTOR_ref minBuild, maxBuild;

			MV1 sea;						//ŠC
			shaders Sea;
			float g_fTime{ 0.f };
			int OldTime{ 0 };
		public:
			void Load(void) noexcept {
				MV1::Load("data/model/sky/model.mv1", &Sky);
				MV1::Load("data/model/ground/model.mv1", &Ground);
				MV1::Load("data/model/ground/col.mv1", &GroundCol);
				MV1::Load("data/model/ground/pos.mv1", &GroundPos);

				int TowerFrame = -1;
				for (int i = 0; i < GroundPos.frame_num(); i++) {
					if (GroundPos.frame_name(i) == "Tower") {
						TowerFrame = i;
					}
				}

				MV1::Load("data/model/ground/Build.mv1", &Build);
				minBuild = Build.mesh_minpos(0);
				maxBuild = Build.mesh_maxpos(0);
				for (int i = 1; i < Build.mesh_num(); i++) {
					if (minBuild.x() > Build.mesh_minpos(i).x()) { minBuild.x(Build.mesh_minpos(i).x()); }
					if (minBuild.y() > Build.mesh_minpos(i).y()) { minBuild.y(Build.mesh_minpos(i).y()); }
					if (minBuild.z() > Build.mesh_minpos(i).z()) { minBuild.z(Build.mesh_minpos(i).z()); }

					if (maxBuild.x() < Build.mesh_maxpos(i).x()) { maxBuild.x(Build.mesh_maxpos(i).x()); }
					if (maxBuild.y() < Build.mesh_maxpos(i).y()) { maxBuild.y(Build.mesh_maxpos(i).y()); }
					if (maxBuild.z() < Build.mesh_maxpos(i).z()) { maxBuild.z(Build.mesh_maxpos(i).z()); }
				}


				for (int i = 0; i < GroundPos.frame_child_num(TowerFrame); i++) {
					Tower.resize(Tower.size() + 1);
					if (Tower.size() == 1) {
						MV1::Load("data/model/ground/Tower.mv1", &Tower.back());
					}
					else {
						Tower.back() = Tower[0].Duplicate();
					}
				}
				minTower = Tower[0].mesh_minpos(0);
				maxTower = Tower[0].mesh_maxpos(0);

				GroundCol.SetupCollInfo(64, 16, 64);
				{
					auto Base = GroundPos.frame(TowerFrame);
					for (int i = 0; i < GroundPos.frame_child_num(TowerFrame); i++) {
						auto Pos = GroundPos.frame((int)GroundPos.frame_child(TowerFrame, i));
						auto buf = Pos - Base;
						Tower[i].SetMatrix(
							MATRIX_ref::RotY(std::atan2f(buf.x(), buf.z()))*
							MATRIX_ref::Mtrans(Pos));
					}
				}
				//ŠC
				{
					MV1::Load("data/model/sea/model.mv1", &this->sea, true);	//ŠC
					this->sea.SetPosition(VECTOR_ref::vget(0, -Scale_Rate *3.f, 0));
					this->sea.SetScale(VECTOR_ref::vget(20, 20, 20));

					Sea.Init("NormalMesh_DirLight_PhongVS.vso", "NormalMesh_DirLight_PhongPS.pso");
					this->g_fTime = 0.f;
					this->OldTime = GetNowCount();
				}

				MV1SetDifColorScale(Sky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			}
			//
			void Execute(void) noexcept {
				//ŠC
				{
					auto Time = GetNowCount();	// Œ»Ý‚ÌŽžŠÔ‚ð“¾‚é
					this->g_fTime += float(Time - this->OldTime) / 1000.0f*0.5f;
					this->OldTime = Time;				// Œ»Ý‚ÌŽžŠÔ‚ð•Û‘¶
				}
			}
			//DrawCall => 100
			void BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				Sky.DrawModel();
				SetUseLighting(TRUE);
			}
			//‹¤’Ê
			void DrawCommon(void) noexcept {
				Ground.DrawModel();
				for (auto& m : Tower) {
					if (CheckCameraViewClip_Box(
						(m.GetMatrix().pos() + minTower).get(),
						(m.GetMatrix().pos() + maxTower).get()) == FALSE
						) {
						m.DrawModel();
					}
				}
				if (CheckCameraViewClip_Box(
					(minBuild).get(),
					(maxBuild).get()) == FALSE
					) {
					Build.DrawModel();
				}
			}
			void Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void Shadow_Draw(void) noexcept {
				DrawCommon();
			}
			void Draw(void) noexcept {
				{
					Sea.Set_paramV(this->g_fTime, 0, 0, 0);
					Sea.Set_param(this->g_fTime, 0, 0, 0);

					Sea.Draw_lamda([&]() {
						SetFogEnable(TRUE);
						SetFogStartEnd(12500.0f, 50000.f);
						SetFogColor(126, 168, 193);
						SetUseTextureToShader(0, MV1GetTextureGraphHandle(sea.get(), MV1GetMaterialDifMapTexture(sea.get(), 0)));
						//SetUseTextureToShader(1, MV1GetTextureGraphHandle(sea.get(), MV1GetMaterialNormalMapTexture(sea.get(), 0)));
						this->sea.DrawModel();
						SetUseTextureToShader(0, -1);
						//SetUseTextureToShader(1, -1);
						SetFogEnable(FALSE);
					});
				}
				DrawCommon();
			}

			const auto& GetGroundCol(void) noexcept { return GroundCol; }
		};
	};
};