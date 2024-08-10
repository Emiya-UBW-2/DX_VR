#pragma once
#pragma warning(disable:4464)
#include "../../Header.hpp"
#include "../../MainScene/BackGround/BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		class BackGroundClass : public SingletonBase<BackGroundClass> {
		private:
			friend class SingletonBase<BackGroundClass>;
		private:
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundFar;
			MV1							m_ObjGroundCol;

			ShaderUseClass::ScreenVertex m_ScreenVertex;
			ShaderUseClass m_Shader;
			ShaderUseClass m_Shader2;
			ShaderUseClass m_Shader3;
			int xsize = 2048;
			int ysize = 2048;

			std::array<GraphHandle, 3> m_PrevBufferHandle{};
			GraphHandle m_WaveNormalHandle{};
			GraphHandle m_WaveObjHandle{};
			int cubeTex{};
			//
			GraphHandle m_ScreenBufferHandle{};
			GraphHandle m_ScreenDepthBufferHandle{};
			//
			GraphHandle m_SeaBufferHandle{};

			float timer = 0.f;

			LONGLONG Time{};
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
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
			void			Load(void) noexcept {
				MV1::Load("data/model/sea/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/sea/modelfar.mv1", &this->m_ObjGroundFar);
				//MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			}
			//
			void			Init(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//地形
				this->m_ObjGroundCol.SetupCollInfo(1, 1, 1);
				//空
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
					this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
					this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
				}
				//

				m_ScreenVertex.SetScreenVertex(xsize, ysize);
				m_Shader.Init("data/Wave/TestVS.vso", "data/Wave/TestPS.pso");
				m_Shader2.Init("data/Wave/TestVS.vso", "data/Wave/NormalMapPS.pso");
				m_Shader3.Init("data/Wave/SeaVS.vso", "data/Wave/SeaPS.pso");
				m_Shader3.CalcGWave();
				cubeTex = LoadGraph("data/Wave/SkyCube.dds");

				for (auto& g : m_PrevBufferHandle) {
					g = GraphHandle::Make(xsize, ysize, TRUE);
				}
				{
					int PresetHandle = LoadGraph("data/Wave/Preset.bmp");
					m_PrevBufferHandle[0].SetDraw_Screen();
					{
						SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
						DrawGraph(0, 0, PresetHandle, FALSE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
					m_PrevBufferHandle[1].SetDraw_Screen();
					{
						SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
						DrawGraph(0, 0, PresetHandle, FALSE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
					m_PrevBufferHandle[2].SetDraw_Screen();
					{
						SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
						DrawGraph(0, 0, PresetHandle, FALSE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
					DeleteGraph(PresetHandle);
				}
				m_WaveNormalHandle = GraphHandle::Make(xsize, ysize, TRUE);

				//
				int DispX = DrawParts->GetScreenX(1920);
				int DispY = DrawParts->GetScreenY(1080);
				m_ScreenBufferHandle = GraphHandle::Make(DispX, DispY, TRUE);
				m_ScreenDepthBufferHandle = GraphHandle::MakeDepth(DispX, DispY);

				Time = GetNowHiPerformanceCount();
			}
			//
			void			FirstExecute(void) noexcept {
			}
			//
			void			Execute(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* OptionParts = OPTION::Instance();
				//波シム
				LONGLONG NowTime = GetNowHiPerformanceCount();
				int SimPer = 30;
				switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)){
				case 0:
					SimPer = 10;
					break;
				case 1:
					SimPer = 24;
					break;
				case 2:
					SimPer = 30;
					break;
				case 3:
					SimPer = 60;
					break;
				default:
					break;
				}
				if (NowTime > Time + (LONGLONG)(1000 / SimPer * 1000)) {
					Time = NowTime;
					{
						//前までの画面を保持
						for (int i = 0; i < 2; i++) {
							m_PrevBufferHandle[i].SetDraw_Screen();
							{
								DrawGraph(0, 0, m_PrevBufferHandle[static_cast<size_t>(i + 1)].get(), TRUE);
							}
						}
						/*
						SetDrawScreen(m_WaveObjHandle.get());
						ClearDrawScreen();
						SetupCamera_Ortho(400.f);
						SetCameraNearFar(1.f, 50.f);
						SetCameraPositionAndTargetAndUpVec(VGet(0.0f, 50.0f, 0.0f), VGet(0.0f, 0.0f, 0.0f), VGet(0.0f, 0.0f, 1.0f));
						{
							SetUseLighting(FALSE);

							float Per = std::clamp(VSize(VSub(PrevPoint, MousPoint)) / 3.f, 0.f, 1.f);
							Per = 1.f;
							DrawCapsule3D(MousPoint, PrevPoint, 0.25f, 8,
								GetColor((int)(255.f * Per), 0, 0), GetColor((int)(255.f * Per), 0, 0), TRUE);
							SetUseLighting(TRUE);
						}
						//*/
						SetDrawScreen(m_PrevBufferHandle[1].get());
						{
							//オブジェクト書き込み
							//SetDrawBright(255, 0, 0);
							//DrawGraph(0, 0, m_WaveObjHandle.get(), TRUE);

							DrawBox(3, 3, xsize - 3, ysize - 3, GetColor(255, 0, 0), FALSE);
						}
						m_PrevBufferHandle[2].SetDraw_Screen();
						{
							SetUseTextureToShader(0, m_PrevBufferHandle[0].get());
							SetUseTextureToShader(1, m_PrevBufferHandle[1].get());
							m_Shader.Draw(m_ScreenVertex);
							SetUseTextureToShader(0, -1);
							SetUseTextureToShader(1, -1);
						}
					}
					GraphFilter(m_PrevBufferHandle[2].get(), DX_GRAPH_FILTER_GAUSS, 8, 16);
					//法線マップへの変換
					SetDrawScreen(m_WaveNormalHandle.get());
					ClearDrawScreen();
					{
						SetUseTextureToShader(0, m_PrevBufferHandle[2].get());
						m_Shader2.Draw(m_ScreenVertex);
						SetUseTextureToShader(0, -1);
					}
					//現段階の波形の保存
					/*
					if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
						SetDrawScreen(m_PrevBufferHandle[2]);
						SaveDrawScreenToBMP(0, 0, xsize, ysize, "data/Wave/Preset.bmp");
					}
					//*/
					timer += 1.f / SimPer;
				}
				{
					//海面以外の描画
					SetDrawScreen(m_ScreenBufferHandle.get());
					SetRenderTargetToShader(0, m_ScreenBufferHandle.get());
					SetRenderTargetToShader(2, m_ScreenDepthBufferHandle.get());
					ClearDrawScreen();
					DrawParts->GetMainCamera().FlipCamInfo();
					SetCameraNearFar(1000.0f, 50000.0f);
					BG_Draw();
					DrawParts->GetMainCamera().FlipCamInfo();
					ClearDrawScreenZBuffer();
					{
						/*
						if (CameraPos.y < 0.f) {
							SetFogEnable(TRUE);
							SetFogMode(DX_FOGMODE_LINEAR);
							SetFogColor(27, 57, 77);
							SetFogStartEnd(10.f*Scale_Rate, 50.f*Scale_Rate);
						}
						//*/
						//SetVerticalFogEnable(TRUE);
						SetVerticalFogMode(DX_FOGMODE_LINEAR);
						SetVerticalFogColor(27, 57, 77);
						SetVerticalFogStartEnd(-25.f*Scale_Rate, -75.f * Scale_Rate);

						ObjectManager::Instance()->Draw_Shadow();

						SetVerticalFogEnable(FALSE);

						SetFogEnable(FALSE);
					}
					SetRenderTargetToShader(0, -1);
					SetRenderTargetToShader(2, -1);
				}
				Vector3DX pos = DrawParts->GetMainCamera().GetCamPos(); pos.y = 0.f;
				this->m_ObjGroundFar.SetPosition(pos);
				this->m_ObjGround.SetPosition(pos);
				this->m_ObjSky.SetPosition(pos);
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_Far(void) noexcept {
				this->m_ObjGroundFar.DrawModel();
				this->m_ObjGround.DrawModel();
			}
			void			Shadow_Draw(void) noexcept {
				this->m_ObjGroundFar.DrawModel();
				this->m_ObjGround.DrawModel();
			}
			void			Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				this->m_ObjGroundFar.DrawModel();
				SetUseTextureToShader(1, m_WaveNormalHandle.get());
				SetUseTextureToShader(3, m_ScreenBufferHandle.get());
				SetUseTextureToShader(4, m_ScreenDepthBufferHandle.get());
				SetUseTextureToShader(5, cubeTex);
				Vector3DX CameraPos = DrawParts->GetMainCamera().GetCamPos();
				m_Shader3.SetVertexParam(4, CameraPos.x, CameraPos.y, CameraPos.z, timer);
				m_Shader3.SetVertexWave();
				m_Shader3.Draw_lamda([&]() {
					this->m_ObjGround.DrawModel();
					});
				SetUseTextureToShader(1, -1);
				SetUseTextureToShader(3, -1);
				SetUseTextureToShader(4, -1);
				SetUseTextureToShader(5, -1);
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGroundFar.Dispose();
				this->m_ObjGround.DrawModel();
				this->m_ObjGroundCol.Dispose();
			}
		};
	}
}
