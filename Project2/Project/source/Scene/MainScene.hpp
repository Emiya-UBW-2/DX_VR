#pragma once
#include	"../Header.hpp"
#include "../MainScene/Input.hpp"

#include "../MainScene/BackGround/BackGround.hpp"

#include "../MainScene/NetWorks.hpp"

#include "../MainScene/Object/Vehicle.hpp"
#include "../MainScene/Object/HindD.hpp"
#include "../MainScene/Object/Item.hpp"
#include "../MainScene/Object/Ammo.hpp"

#include "../MainScene/Object/ItemData.hpp"
#include "../MainScene/Player/CPU.hpp"

#include "../MainScene/UI/MainSceneUIControl.hpp"
#include "../MainScene/Inventory.hpp"

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
			Matrix4x4DX									m_FreeLookMat;
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			float										m_XScope{ 0.f };
			float										m_YScope{ 0.f };
			Vector3DX									m_VehRadAdd;
			bool										m_IsChangeView{ false };
			float										m_ChangeViewPer{ 0.f };
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
			InventoryClass								m_InventoryClass;
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
				auto EndPos = Vehicle->GetGunMuzzlePos(0) + Vehicle->GetGunMuzzleVec(0) * Vehicle->GetAimingDistance();
				Vector3DX LensPos = ConvWorldPosToScreenPos(EndPos.get());
				if (0.f < LensPos.z && LensPos.z < 1.f) {
					this->m_Reticle_xpos = LensPos.x;
					this->m_Reticle_ypos = LensPos.y;
					this->m_Reticle_on = true;
					//if (!this->m_InputClass->GetMouseActive().on()) {
					//	this->m_Reticle_on = false;
					//}
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
				auto* Pad = PadControl::Instance();
				//UI
				if (Vehicle->is_ADS()) {
					auto RadAdd = Vehicle->GetRadAdd() - m_VehRadAdd;
					{
						auto Base = Pad->GetLS_X()*1.f + RadAdd.z*1000.f;
						Easing(&this->m_XScope, Base, 0.95f, EasingType::OutExpo);
					}
					{
						auto Base = Pad->GetLS_Y()*1.f + RadAdd.x*1000.f;
						Easing(&this->m_YScope, -Base, 0.9f, EasingType::OutExpo);
					}
					int xpos = (int)(this->m_XScope);
					int ypos = (int)(this->m_YScope);
					int xsize = DrawParts->GetDispXSize();
					int ysize = DrawParts->GetDispYSize();
					DrawBox(0, 0, xpos, ypos + ysize, Black, TRUE);
					DrawBox(xpos, 0, DrawParts->GetDispXSize(), ypos, Black, TRUE);
					DrawBox(xpos + xsize, ypos, DrawParts->GetDispXSize(), DrawParts->GetDispYSize(), Black, TRUE);
					DrawBox(0, ypos + ysize, xpos + xsize, DrawParts->GetDispYSize(), Black, TRUE);
					this->m_scope_Graph.DrawExtendGraph(xpos, ypos, xpos + xsize, ypos + ysize, true);
				}
				{
					if (m_IsChangeView != Vehicle->is_ADS()) {
						this->m_ChangeViewPer = 1.f;
					}
					else {
						Easing(&this->m_ChangeViewPer, 0.f, 0.95f, EasingType::OutExpo);
					}
					if ((this->m_ChangeViewPer*255.f) > 1.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_ChangeViewPer), 0, 255));
						DrawBox(0, 0, DrawParts->GetDispXSize(), DrawParts->GetDispYSize(), Black, TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
				m_VehRadAdd = Vehicle->GetRadAdd();
				m_IsChangeView = Vehicle->is_ADS();

				if (Vehicle->Get_alive()) {
					this->m_UIclass.Draw();
					Vehicle->DrawModuleView(y_r(50 + 100), DrawParts->GetDispYSize() - y_r(100 + 150), y_r(200));
				}
				//通信設定
				//if (!this->m_MouseActive.on()) {
				//	this->m_NetWorkBrowser.Draw();
				//}
				//Pause
				this->m_InventoryClass.Draw(Vehicle->GetMove().pos);
				if (Vehicle->Get_alive()) {
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
