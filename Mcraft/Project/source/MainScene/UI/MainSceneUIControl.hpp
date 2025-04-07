#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;

			bool CanLookTarget = false;
			float LookPer{};
			float IsDrawAimUIPer{};
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
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& ViewChara = PlayerMngr->GetPlayer(PlayerMngr->GetWatchPlayer())->GetChara();

				CanLookTarget = false;
				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); loop++) {
					if (loop == PlayerMngr->GetWatchPlayer()) { continue; }
					auto& c = PlayerMngr->GetPlayer(loop)->GetChara();
					CanLookTarget |= c->IsAlive() && c->GetCanLookByPlayer();
				}
				Easing(&LookPer, CanLookTarget ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				bool IsDrawAimUI = false;
				if (ViewChara->GetGunPtrNow()) {
					IsDrawAimUI = ViewChara->GetGunPtrNow()->GetCanShot();
				}
				Easing(&IsDrawAimUIPer, IsDrawAimUI ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				if (IsDrawAimUIPer < 0.1f && !IsDrawAimUI) { IsDrawAimUIPer = 0.f; }
			}
			void			Draw(void) const noexcept {
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto* CameraParts = Camera3D::Instance();
				auto& ViewChara = PlayerMngr->GetPlayer(PlayerMngr->GetWatchPlayer())->GetChara();

				int xp1{}, yp1{};
				int xp2{}, yp2{};
				int xp3{}, yp3{};
				//タイム,スコア
				{
					xp1 = (30);
					yp1 = (10);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32), 
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "TIME");
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (240), yp1, White, Black, "%d:%05.2f",
							static_cast<int>(floatParam[0] / 60.f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));



						for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); loop++) {
							if (loop == PlayerMngr->GetWatchPlayer()) { continue; }
							auto& c = PlayerMngr->GetPlayer(loop)->GetChara();
							if (!c->IsAlive() || !c->GetCanLookByPlayer()) { continue; }
							if (c->GetIsActiveCameraPosToPlayer()) {
								if (ViewChara->GetGunPtrNow() && ViewChara->GetGunPtrNow()->GetAutoAimActive() && loop == ViewChara->GetGunPtrNow()->GetAutoAimID()) {
									DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, static_cast<int>(c->GetCameraPosToPlayer().XScreenPos()), static_cast<int>(c->GetCameraPosToPlayer().YScreenPos()), static_cast<int>(2400 / std::max(0.1f, c->GetLengthToPlayer()) + Lerp(100, 0, LookPer)), Red, false, 5);
								}
								else {
									DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, static_cast<int>(c->GetCameraPosToPlayer().XScreenPos()), static_cast<int>(c->GetCameraPosToPlayer().YScreenPos()), static_cast<int>(2400 / std::max(0.1f, c->GetLengthToPlayer()) + Lerp(100, 0, LookPer)), Red50, false, 2);
								}
							}
						}

						if (IsDrawAimUIPer > 0.1f && ViewChara->GetGunPtrNow()) {
							int RetX = ViewChara->GetGunPtrNow()->GetAimXPos() + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.f);
							int RetY = ViewChara->GetGunPtrNow()->GetAimYPos() + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.f);
							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, IsDrawAimUIPer));
							auto fov_t = CameraParts->GetMainCamera().GetCamFov();
							int Scale = static_cast<int>(1080 * ViewChara->GetGunPtrNow()->GetAutoAimRadian() / fov_t);
							DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, RetX, RetY, Scale, Green, false, 2);
							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, LookPer * IsDrawAimUIPer));
							DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, RetX, RetY, Scale + 4 + Lerp(100, 0, LookPer), Green, false, 2);
							//DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, IsDrawAimUIPer));

							int Scale2 = Scale + 4 + Lerp(100, 0, LookPer);

							Vector3DX StartPos = CameraParts->GetMainCamera().GetCamPos();
							Vector3DX Vec1 = CameraParts->GetMainCamera().GetCamVec() - StartPos; Vec1.y = 0.f; Vec1 = Vec1.normalized();
							{
								for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); loop++) {
									if (loop == PlayerMngr->GetWatchPlayer()) { continue; }
									auto& c = PlayerMngr->GetPlayer(loop)->GetChara();
									if (!c->IsAlive() || !c->GetCanLookByPlayer()) { continue; }
									Vector3DX Vec2 = (c->GetEyePositionCache() - StartPos); Vec2.y = 0.f; Vec2 = Vec2.normalized();
									float Angle = DX_PI_F + Vector3DX::Angle(Vec1, Vec2) * (Vector3DX::Cross(Vec1, Vec2).y > 0.f ? 1.f : -1.f);

									xp1 = RetX - static_cast<int>(Scale2 * 1.5f * sin(Angle + deg2rad(5)));
									yp1 = RetY + static_cast<int>(Scale2 * 1.5f * cos(Angle + deg2rad(5)));
									xp2 = RetX - static_cast<int>(Scale2 * sin(Angle));
									yp2 = RetY + static_cast<int>(Scale2 * cos(Angle));
									xp3 = RetX - static_cast<int>(Scale2 * 1.5f * sin(Angle - deg2rad(5)));
									yp3 = RetY + static_cast<int>(Scale2 * 1.5f * cos(Angle - deg2rad(5)));
									unsigned int Color = (loop == ViewChara->GetGunPtrNow()->GetAutoAimID()) ? Green : DarkGreen;
									DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Color, 2);
									DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp2, yp2, xp3, yp3, Color, 2);
									DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3, yp3, xp1, yp1, Color, 2);
								}
							}
							float BaseRad = ViewChara->GetLeanRad();
							xp1 = RetX - static_cast<int>(Scale * 1.0f * sin(deg2rad(90) + BaseRad));
							yp1 = RetY + static_cast<int>(Scale * 1.0f * cos(deg2rad(90) + BaseRad));
							xp2 = RetX - static_cast<int>(Scale * 0.85f * sin(deg2rad(90) + BaseRad));
							yp2 = RetY + static_cast<int>(Scale * 0.85f * cos(deg2rad(90) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

							xp1 = RetX - static_cast<int>(Scale * 1.0f * sin(deg2rad(-90) + BaseRad));
							yp1 = RetY + static_cast<int>(Scale * 1.0f * cos(deg2rad(-90) + BaseRad));
							xp2 = RetX - static_cast<int>(Scale * 0.85f * sin(deg2rad(-90) + BaseRad));
							yp2 = RetY + static_cast<int>(Scale * 0.85f * cos(deg2rad(-90) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

							xp1 = RetX - static_cast<int>(Scale * 1.0f * sin(deg2rad(0) + BaseRad));
							yp1 = RetY + static_cast<int>(Scale * 1.0f * cos(deg2rad(0) + BaseRad));
							xp2 = RetX - static_cast<int>(Scale * 0.5f * sin(deg2rad(0) + BaseRad));
							yp2 = RetY + static_cast<int>(Scale * 0.5f * cos(deg2rad(0) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

							xp1 = RetX - static_cast<int>(Scale * 1.5f * sin(deg2rad(60) + BaseRad));
							yp1 = RetY + static_cast<int>(Scale * 1.5f * cos(deg2rad(60) + BaseRad));
							xp2 = RetX - static_cast<int>(Scale * sin(deg2rad(60) + BaseRad));
							yp2 = RetY + static_cast<int>(Scale * cos(deg2rad(60) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 100, yp1, xp1, yp1, Green, 2);

							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 65, yp1 - 3, Green, Black, "/%d", ViewChara->GetGunPtrNow()->GetAmmoAll());
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 100, yp1 - 3, Green, Black, "%d", ViewChara->GetGunPtrNow()->GetAmmoNumTotal());

							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}

						//スタートタイマー
						if (floatParam[1] > 0.f) {
							xp1 = 1920 / 2;
							yp1 = (340);
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
								FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "%d:%05.2f",
								static_cast<int>(floatParam[1] / 60.f), static_cast<float>(static_cast<int>(floatParam[1]) % 60) + (floatParam[1] - static_cast<float>(static_cast<int>(floatParam[1]))));
						}

				}
				//方位磁針
				{
					Vector3DX Vec = ViewChara->GetEyeRotationCache().zvec() * -1.f; Vec.y = 0.f; Vec = Vec.normalized();
					float radian = std::atan2f(Vec.x, Vec.z);
					float degreeBase = rad2deg(radian);


					for (int loop = -25; loop <= 25; ++loop) {
						int degree = (360 + static_cast<int>(degreeBase) + loop) % 360;
						xp1 = 960 + static_cast<int>((static_cast<int>(degreeBase)- degreeBase + loop) * 10) + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.f);
						yp1 = 1080 * 2 / 10 + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.f);
						if (degree % 90 == 0) {
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 20, Green, 4);
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
								FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1 + 24, Green, Black, "%03d", degree);
						}
						else if (degree % 10 == 0) {
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 10, Green, 4);
						}
						else {
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 6, xp1, yp1 + 6, Green, 2);
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
