#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include	"../sub.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"

#include	"../MainScene/Object/MovieCharacter.hpp"
#include	"../MainScene/Object/Gun.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
			bool								m_IsEnd{ false };//シーン全体の終了フラグ
			GraphHandle							m_TitleImage;
			std::unique_ptr<UIs::CreditControl>		m_CreditControl;

			std::shared_ptr<Charas::MovieCharacter>	m_MovieCharacter{};
			MV1										m_ObjSky;

			std::vector<std::shared_ptr<Guns::GunObj>>		m_GunPtr{};			//銃

			bool									m_IsCustomize = false;
			Vector3DX CamPos;
			Vector3DX CamVec = Vector3DX::forward();
			float CamFov = deg2rad(45);
			float CamYrad = deg2rad(0);
			float CamYradR = deg2rad(0);
			std::vector<std::pair<int, int>>		m_GunPoint;
			std::array<int,3>						m_GunSelect;
			int										m_GunTypeSel = 0;
			int										m_GunSel = 0;

			float									m_CamTimer = 0.f;

			std::vector<float>						m_GunAnimTimer;
		public:
			TitleScene(void) noexcept {}
			TitleScene(const TitleScene&) = delete;
			TitleScene(TitleScene&&) = delete;
			TitleScene& operator=(const TitleScene&) = delete;
			TitleScene& operator=(TitleScene&&) = delete;

			virtual ~TitleScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			LoadEnd_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;

			void			BG_Draw_Sub(void) const noexcept override;
			void			ShadowDraw_Far_Sub(void) const noexcept override {}
			void			ShadowDraw_Sub(void) const noexcept override {
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			CubeMap_Sub(void) const noexcept override {}
			void			SetShadowDraw_Rigid_Sub(void) const noexcept override {}
			void			MainDraw_Sub(int) const noexcept override;
			void			SetShadowDraw_Sub(void) const noexcept override {
				ObjectManager::Instance()->Draw(false, InvalidID);
			}
			void			DrawUI_Base_Sub(void) const noexcept override;
		};
	}
}
