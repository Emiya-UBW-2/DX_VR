#pragma once
#include	"../Header.hpp"
#include "../MainScene/Input.hpp"

#include "../MainScene/BackGround/BackGround.hpp"


#include "../MainScene/NetWork.hpp"
#include "../MainScene/NetworkBrowser.hpp"

#include "../MainScene/Object/Character.hpp"

#include "../MainScene/Player/CPU.hpp"

#include "../MainScene/UI/MainSceneUIControl.hpp"

#include "../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = 0;
			static const int		Vehicle_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;					//BG
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			//操作関連
			int											m_TPSLen{6};
			float										m_range_r{(float)this->m_TPSLen};
			float										m_zoom{1.f};
			bool										m_changeview{false};
			bool										m_IsChangeView{false};
			float										m_ChangeViewPer{0.f};
			//UI関連
			UIClass										m_UIclass;
			float m_GuardStart{0.f};

			NetWorkBrowser								m_NetWorkBrowser;
			//
			std::vector<DamageEvent>					m_DamageEvents;
			float										m_fov_base{ deg2rad(45.f) };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept {
				this->m_BackGround = std::make_shared<BackGroundClass>();
				for (int i = 0; i < Player_num; i++) {
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
			}
			~MAINLOOP(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_Far();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw();
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			CubeMap_Sub(void) noexcept override {
				this->m_BackGround->Draw();
			}
			
			void			MainDraw_Sub(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar()*2.f);
				this->m_BackGround->Draw();
				ObjectManager::Instance()->Draw();
				//ObjectManager::Instance()->Draw_Depth();
				for (int i = 0; i < Player_num; i++) {
					m_AICtrl[i]->Draw();
				}

			}
			//UI表示
			void			DrawUI_Base_Sub(float) noexcept override {
			}
			void			DrawUI_In_Sub(float scale) noexcept override {
				//UI
				if ((this->m_ChangeViewPer*255.f) > 1.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_ChangeViewPer), 0, 255));
					DrawBox(0, 0, y_r(1920.f*scale), y_r(1080.f*scale), Black, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				if (!DXDraw::Instance()->IsPause()) {
					this->m_UIclass.Draw(scale);
				}
				//通信設定
				//if (!this->m_MouseActive.on()) {
				//	this->m_NetWorkBrowser.Draw();
				//}
			}
		};
	};
};
