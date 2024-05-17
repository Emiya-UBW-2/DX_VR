#pragma once
#include	"../Header.hpp"
#include "../MainScene/Input.hpp"

#include "../MainScene/BackGround/BackGround.hpp"


#include "../MainScene/NetWork.hpp"
#include "../MainScene/NetworkBrowser.hpp"

#include "../MainScene/Object/Vehicle.hpp"
#include "../MainScene/Object/Ammo.hpp"

#include "../MainScene/Object/ItemData.hpp"
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
			float										m_XScope{0.f};
			float										m_YScope{0.f};
			Vector3DX									m_VehRadAdd;
			float										m_ChangeViewPer{0.f};
			//UI関連
			UIClass										m_UIclass;
			float										m_HPBuf{ 0.f };
			float										m_ScoreBuf{ 0.f };
			GraphHandle									m_hit_Graph;
			GraphHandle									m_aim_Graph;
			GraphHandle									m_scope_Graph;
			bool										m_Reticle_on = false;
			float										m_Reticle_xpos = 0;
			float										m_Reticle_ypos = 0;
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
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_Far();
			}

			void			ShadowDraw_NearFar_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw();
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			MainDraw_Sub(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar()*2.f);
				this->m_BackGround->Draw();
				ObjectManager::Instance()->Draw();
				//ObjectManager::Instance()->Draw_Depth();
				Vector3DX LensPos = ConvWorldPosToScreenPos(Vehicle->GetAimPoint().get());
				if (0.f < LensPos.z && LensPos.z < 1.f) {
					this->m_Reticle_xpos = LensPos.x;
					this->m_Reticle_ypos = LensPos.y;
					this->m_Reticle_on = true;
				}
				for (int i = 0; i < Player_num; i++) {
					m_AICtrl[i]->Draw();
				}

			}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj((int)ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (GetMyPlayerID() == a->GetMyPlayerID()) {
								a->Draw_Hit_UI(this->m_hit_Graph);
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
			}
			void			DrawUI_In_Sub(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				//UI
				if (this->m_TPSLen == 0) {
					int xpos = (int)(this->m_XScope);
					int ypos = (int)(this->m_YScope);
					int xsize = DrawParts->GetDispXSize();
					int ysize = DrawParts->GetDispYSize();
					DrawBox(0, 0, xpos, ypos + ysize, Black, TRUE);
					DrawBox(xpos, 0, DrawParts->GetDispXSize(), ypos, Black, TRUE);
					DrawBox(xpos + xsize, ypos, DrawParts->GetDispXSize(), DrawParts->GetDispYSize(), Black, TRUE);
					DrawBox(0, ypos + ysize, xpos + xsize, DrawParts->GetDispYSize(), Black, TRUE);
					//this->m_scope_Graph.DrawExtendGraph(xpos, ypos, xpos + xsize, ypos + ysize, true);
				}
				if ((this->m_ChangeViewPer*255.f) > 1.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_ChangeViewPer), 0, 255));
					DrawBox(0, 0, DrawParts->GetDispXSize(), DrawParts->GetDispYSize(), Black, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				if (Vehicle->IsAlive()) {
					this->m_UIclass.Draw();
					Vehicle->DrawModuleView(y_r(50 + 100), DrawParts->GetDispYSize() - y_r(100 + 150), y_r(200));
				}
				//通信設定
				//if (!this->m_MouseActive.on()) {
				//	this->m_NetWorkBrowser.Draw();
				//}
				//Pause
				if (Vehicle->IsAlive()) {
					if (this->m_Reticle_on) {
						this->m_aim_Graph.DrawRotaGraph((int)this->m_Reticle_xpos, (int)this->m_Reticle_ypos, (float)(y_r(100)) / 100.f, 0.f, true);

						unsigned int color = GetColor(0, 255, 0);
						auto Time = Vehicle->GetTotalloadtime(0);
						if (Vehicle->Gunloadtime(0) != 0.f) {
							color = GetColor(255, 0, 0);
							Time = Vehicle->Gunloadtime(0);
						}
						Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, (int)this->m_Reticle_xpos + y_r(50), (int)this->m_Reticle_ypos, color, GetColor(0, 0, 0), "%05.2f s", Time);
					}
				}
			}
		};
	};
};
