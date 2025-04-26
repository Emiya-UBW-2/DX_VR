#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class LoadScene : public TEMPSCENE {
		private:
			int ASyncLoadNum = 0;
		public:
			LoadScene(void) noexcept {}
			LoadScene(const LoadScene&) = delete;
			LoadScene(LoadScene&&) = delete;
			LoadScene& operator=(const LoadScene&) = delete;
			LoadScene& operator=(LoadScene&&) = delete;

			virtual ~LoadScene(void) noexcept {}
		public:
			void Set_Sub(void) noexcept override {
				SetUseASyncLoadFlag(true);
				Get_Next()->Load();//次シーンのロード物
				SetUseASyncLoadFlag(false);
				ASyncLoadNum = GetASyncLoadNum();
			}
			bool Update_Sub(void) noexcept override {
				return (GetASyncLoadNum() > 0);
			}
			void Dispose_Sub(void) noexcept override {
				Get_Next()->LoadEnd();
			}
		public:
			//UI表示
			void DrawUI_Base_Sub(void) const noexcept override {
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				// 背景
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Gray50, true);
				// 
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM,
					(1920-32), (1080 - 32), Green, Black, "Loading...%d / %d", ASyncLoadNum - GetASyncLoadNum(), ASyncLoadNum);

			}
		};
	}
}