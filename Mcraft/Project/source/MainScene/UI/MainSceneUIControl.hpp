#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;
		public:
			UIClass(void) noexcept {}
			UIClass(const UIClass&) = delete;
			UIClass(UIClass&& o) = delete;
			UIClass& operator=(const UIClass&) = delete;
			UIClass& operator=(UIClass&& o) = delete;

			virtual ~UIClass(void) noexcept {}
		public:
			void			Load(void) noexcept {
			}
			void			Dispose(void) noexcept {
			}
			void			Set(void) noexcept {
			}
			void			Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				int xp1, yp1;
				//タイム,スコア
				{
					xp1 = DrawParts->GetUIY(30);
					yp1 = DrawParts->GetUIY(10);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32), 
						FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "TIME");
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32), 
						FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + DrawParts->GetUIY(240), yp1, White, Black, "%d:%05.2f",
						static_cast<int>(floatParam[0] / 60.f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));

					xp1 = UIWidth / 2;
					yp1 = DrawParts->GetUIY(10);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32), 
						FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "%d : %d", intParam[0], intParam[1]);
				}
			}

			void			SetIntParam(int ID, int value) noexcept { intParam[static_cast<size_t>(ID)] = value; }
			void			SetfloatParam(int ID, float value) noexcept { floatParam[static_cast<size_t>(ID)] = value; }
			void			SetStrParam(int ID, std::string_view value) noexcept { strParam[static_cast<size_t>(ID)] = value; }
		};
	}
}
