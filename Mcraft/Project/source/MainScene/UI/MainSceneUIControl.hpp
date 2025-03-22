#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;

			int XP1{}, YP1{}, Scale1{};
			float TimePer{};
			bool CanLookTarget = false;
			float LookPer{};
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
			void			Update(void) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();

				TimePer += DXLib_refParts->GetDeltaTime();
				if (TimePer > 3.f / FrameRate) {
					TimePer -= 3.f / FrameRate;
					XP1 = intParam[2];
					YP1 = intParam[3];
					auto* CameraParts = Camera3D::Instance();
					auto fov_t = CameraParts->GetMainCamera().GetCamFov();
					Scale1 = (int)(1080 * floatParam[2] / fov_t);
				}
				auto* PlayerMngr = Player::PlayerManager::Instance();

				CanLookTarget = false;
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
					if (index == 0) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
					CanLookTarget |= c->IsAlive() && c->CanLookTarget;
				}
				Easing(&LookPer, CanLookTarget ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			}
			void			Draw(void) const noexcept {
				int xp1{}, yp1{};
				int xp2{}, yp2{};
				//タイム,スコア
				{
					xp1 = (30);
					yp1 = (10);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32), 
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "TIME");
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32), 
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (240), yp1, White, Black, "%d:%05.2f",
						static_cast<int>(floatParam[0] / 60.f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));

					
					auto* CameraParts = Camera3D::Instance();
					auto fov_t = CameraParts->GetMainCamera().GetCamFov();
					int Scale = (int)(1080 * floatParam[2] / fov_t);
					WindowSystem::DrawControl::Instance()->SetDrawCircle(WindowSystem::DrawLayer::Normal, XP1, YP1, Scale1, Black, false, 2);
					WindowSystem::DrawControl::Instance()->SetDrawCircle(WindowSystem::DrawLayer::Normal, intParam[2], intParam[3], Scale, Red, false, 2);
					WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, LookPer));
					WindowSystem::DrawControl::Instance()->SetDrawCircle(WindowSystem::DrawLayer::Normal, intParam[2], intParam[3], Scale + 4 + Lerp(100, 0, LookPer), Red, false, 2);
					WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);

					xp1 = intParam[2] - (int)(Scale1 * 1.5f * sin(deg2rad(60) + floatParam[3]));
					yp1 = intParam[3] + (int)(Scale1 * 1.5f * cos(deg2rad(60) + floatParam[3]));
					xp2 = intParam[2] - (int)(Scale1 * sin(deg2rad(60) + floatParam[3]));
					yp2 = intParam[3] + (int)(Scale1 * cos(deg2rad(60) + floatParam[3]));
					WindowSystem::DrawControl::Instance()->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 + 1, xp2, yp2 + 1, Black, 2);
					WindowSystem::DrawControl::Instance()->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 100, yp1 + 1, xp1, yp1 + 1, Black, 2);
					WindowSystem::DrawControl::Instance()->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Red, 2);
					WindowSystem::DrawControl::Instance()->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 100, yp1, xp1, yp1, Red, 2);

					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 65, yp1 - 3, Red, Black, "/%d", intParam[1]);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 100, yp1 - 3, Red, Black, "%d", intParam[0]);


					if (floatParam[1] > 0.f) {
						xp1 = 1920 / 2;
						yp1 = (340);
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "%d:%05.2f",
							static_cast<int>(floatParam[1] / 60.f), static_cast<float>(static_cast<int>(floatParam[1]) % 60) + (floatParam[1] - static_cast<float>(static_cast<int>(floatParam[1]))));
					}
				}

				auto* PlayerMngr = Player::PlayerManager::Instance();
				{
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
						if (index == 0) { continue; }
						auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
						if (!c->IsAlive() || !c->CanLookTarget) { continue; }
						if (0.f < c->m_CameraPos.z && c->m_CameraPos.z < 1.f) {
							WindowSystem::DrawControl::Instance()->SetDrawCircle(WindowSystem::DrawLayer::Normal, (int)c->m_CameraPos.x, (int)c->m_CameraPos.y, static_cast<int>(2400 / std::max(0.1f, c->m_Length) + Lerp(100, 0, LookPer)), Green, false, 2);
						}
					}
				}
			}

			void			SetIntParam(int ID, int value) noexcept { intParam[static_cast<size_t>(ID)] = value; }
			void			SetfloatParam(int ID, float value) noexcept { floatParam[static_cast<size_t>(ID)] = value; }
			void			SetStrParam(int ID, std::string_view value) noexcept { strParam[static_cast<size_t>(ID)] = value; }
		};
	}
}
