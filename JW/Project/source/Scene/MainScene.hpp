#pragma once
#include	"../Header.hpp"
#include "../NetWork.hpp"
#include "../MainScene/UI/MainSceneUIControl.hpp"
#include "../MainScene/BackGround/BackGround.hpp"
#include "../MainScene/NetworkBrowser.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/Player/CPU.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClassMain>			m_BackGround;				//BG
			SoundHandle				m_Env;
			SoundHandle				m_AimOn;
			//人
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//ポインター別持ち
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			float					m_EyePosPer_Prone = 0.f;
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			int						m_LookMode{ 0 };
			//UI関連
			UIClass					m_UIclass;
			int						m_HPBuf{ 0 };
			int						m_ScoreBuf{ 0 };
			GraphHandle				autoaimpoint_Graph;
			GraphHandle				Enemyaimpoint_Graph;
			GraphHandle				Gauge_Graph;
			GraphHandle				hit_Graph;
			GraphHandle				aim_Graph;
			int						stand_sel{ 0 };
			float					stand_selAnim{ 0.f };
			float					stand_selAnimR{ 0.f };
			float					stand_AnimTime{ 0.f };
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;

			float					SpeedUpdateTime{ 0.f };
			float					m_DrawSpeed{ 0.f };
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			//
			MATRIX_ref				m_FreeLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;

			NetWorkBrowser			m_NetWorkBrowser;
			//共通
			double					m_ClientFrame{ 0.0 };

			float					fov_base{ 0.f };

			float					m_AimRot{ 0.f };
			VECTOR_ref				m_Laserpos2D;

			bool					m_IsDrawAimPoint{ false };
			struct AimPosControl {
				VECTOR_ref	pos;
				VECTOR_ref	pos2D;
				int			type{ 0 };
			};
			std::vector<AimPosControl>	m_AutoAimPos;

			GraphHandle				m_MiniMapScreen;

			std::array<ShaderUseClass, 1>	m_Shader;
			RealTimeCubeMap			m_RealTimeCubeMap;
			int						cubeTex{ -1 };

			float					Timer{ 5.f };


			int						WatchSelect{ 0 };
			int						WatchPoint{ 0 };
		private:
			int select{ 0 };
			std::array<float, 3> SelYadd{};
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept { }
			void			Load_Sub(void) noexcept override {
				auto* SE = SoundPool::Instance();

				SetCreate3DSoundFlag(FALSE);
				//this->m_AimOn = SoundHandle::Load("data/Sound/SE/aim_on.wav");
				SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
				SE->Add((int)SoundEnum::Env2, 1, "data/Sound/SE/envi2.wav", false);
				SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
				SetCreate3DSoundFlag(FALSE);
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					//SE->Add((int)SoundEnum::Cocking1_0 + i, 3, "data/Sound/SE/gun/bolt/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Cocking2_0 + i, 3, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Cocking3_0 + i, 3, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
					if (i == 2) {
						SOUND3D_REVERB_PARAM ReverbParam;
						ReverbParam.WetDryMix = 100.0f;
						ReverbParam.ReflectionsDelay = 20;
						ReverbParam.ReverbDelay = 29;
						ReverbParam.RearDelay = 5;
						ReverbParam.PositionLeft = 6;
						ReverbParam.PositionRight = 6;
						ReverbParam.PositionMatrixLeft = 27;
						ReverbParam.PositionMatrixRight = 27;
						ReverbParam.EarlyDiffusion = 15;
						ReverbParam.LateDiffusion = 15;
						ReverbParam.LowEQGain = 8;
						ReverbParam.LowEQCutoff = 4;
						ReverbParam.HighEQGain = 8;
						ReverbParam.HighEQCutoff = 6;
						ReverbParam.RoomFilterFreq = 5000.0f;
						ReverbParam.RoomFilterMain = -10.0f;
						ReverbParam.RoomFilterHF = -5.0f;
						ReverbParam.ReflectionsGain = -12.3f;
						ReverbParam.ReverbGain = -0.02f;
						ReverbParam.DecayTime = 3.9200001f;
						ReverbParam.Density = 100.0f;
						ReverbParam.RoomSize = 100.0f;

						auto& handles = SE->Get((int)SoundEnum::Cocking3_0 + i).GetHandles();
						for (auto& hs : handles) {
							for (auto& h : hs.handle) {
								Set3DReverbParamSoundMem(&ReverbParam, h.get());
							}
						}
					}
				}


				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 6, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 6, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, Chara_num * 2, "data/Sound/SE/move/heart.wav");

				autoaimpoint_Graph = GraphHandle::Load("data/UI/battle_autoaimpoint.bmp");
				Enemyaimpoint_Graph = GraphHandle::Load("data/UI/battle_enemyaimpoint.bmp");
				Gauge_Graph = GraphHandle::Load("data/UI/Gauge.png");
				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");

				//BG
				this->m_BackGround = std::make_shared<BackGroundClassMain>();
				this->m_BackGround->Init("data/model/map/", "");//("data/model/map/", "data/model/sky/");

				for (auto& y : SelYadd) {
					y = 0.f;
				}
			}
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();

				this->m_BackGround->Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_NearFar_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
