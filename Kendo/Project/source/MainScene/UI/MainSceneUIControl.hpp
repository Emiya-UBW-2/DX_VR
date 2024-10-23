#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
			class GaugeParam {
				int m_Now{ 0 };
				int m_Max{ 100 };
				float m_Buffer{ 0.f };
			public:
				void			InitGaugeParam(int value, int Max) noexcept {
					this->m_Now = value;
					this->m_Max = Max;
					this->m_Buffer = static_cast<float>(this->m_Now);
				}
				void			SetGaugeParam(int value, int Max, int Rate) noexcept {
					auto* DrawParts = DXDraw::Instance();
					this->m_Now = value;
					this->m_Max = Max;
					if (Rate > 0) {
						float Limit = static_cast<float>(this->m_Max / Rate) * 1.5f;
						this->m_Buffer += std::clamp(static_cast<float>(static_cast<float>(this->m_Now) - this->m_Buffer) * static_cast<float>(this->m_Max / Rate), -Limit, Limit) / DrawParts->GetFps();
					}
					else {
						this->m_Buffer = static_cast<float>(this->m_Now);
					}
				}
				void			DrawGauge(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) const noexcept {
					//return;
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawBoxLine_2D(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White);
					int length = (xp2 - 1) - (xp1 + 1);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					WindowSystem::SetBox(
						xp1 + 1 + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + 1,
						xp2 - 1, yp2 - 1,
						Black);
					WindowSystem::SetBox(
						xp1 + 1, yp1 + 1,
						xp1 + 1 + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - 1,
						GetColor(Color.r, Color.g, Color.b));
					WindowSystem::SetBox(
						xp1 + 1 + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + 1,
						xp1 + 1 + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - 1,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b));
				}
				void			DrawGaugeUp(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawBoxLine_2D(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White);
					int length = (yp2 - 1) - (yp1 + 1);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					WindowSystem::SetBox(
						xp1 + 1, yp1 + 1,
						xp2 - 1, yp2 - 1 - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						Black);
					WindowSystem::SetBox(
						xp1 + 1, yp2 - 1 - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + 1, yp2 - 1,
						GetColor(Color.r, Color.g, Color.b));
					WindowSystem::SetBox(
						xp1 + 1, yp2 - 1 - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + 1, yp2 - 1 - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b));
				}
				void			DrawGaugeCircleLeft(int xp1, int yp1,
					COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
					GraphHandle* CircleObj, float deg, int Add) noexcept {
					//return;
					auto* DrawParts = DXDraw::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (this->m_Buffer > static_cast<float>(this->m_Now)) ? ColorSub : ColorAdd;
					float per = std::clamp(static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max), 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / static_cast<float>(this->m_Max), 0.f, 1.f);
					//SetDrawBlendMode(DX_BLENDMODE_ADD, Add);
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCircleGauge(xp1 + DrawParts->GetScreenY(256), yp1,
						50.0 + ((50.0 - 15.0 * 2.0) * 1.0) + 15.0 + deg,
						CircleObj->get(),
						50.0 + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 + DrawParts->GetScreenY(256), yp1,
						50.0 + ((50.0 - 15.0 * 2.0) * (double)std::max(per, perbuf)) + 15.0 + deg,
						CircleObj->get(),
						50.0 + ((50.0 - 15.0 * 2.0) * (double)std::min(per, perbuf)) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 + DrawParts->GetScreenY(256), yp1,
						50.0 + ((50.0 - 15.0 * 2.0) * (double)std::min(per, perbuf)) + 15.0 + deg,
						CircleObj->get(),
						50.0 + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				}
				void			DrawGaugeCircleRight(int xp1, int yp1,
					COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
					GraphHandle* CircleObj, float deg, int Add) noexcept {
					//return;
					auto* DrawParts = DXDraw::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (this->m_Buffer > static_cast<float>(this->m_Now)) ? ColorSub : ColorAdd;
					float per = std::clamp(static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max), 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / static_cast<float>(this->m_Max), 0.f, 1.f);
					//SetDrawBlendMode(DX_BLENDMODE_ADD, Add);
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCircleGauge(xp1 - DrawParts->GetScreenY(256), yp1,
						(50.0 - 15.0 * 2.0) + 15.0 + deg,
						CircleObj->get(),
						(double)((50.0 - 15.0 * 2.0) * (1.0 - 1.0)) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 - DrawParts->GetScreenY(256), yp1,
						(double)((50.0 - 15.0 * 2.0) * (1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
						CircleObj->get(),
						(double)((50.0 - 15.0 * 2.0) * (1.0 - (double)std::max(per, perbuf))) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 - DrawParts->GetScreenY(256), yp1,
						(50.0 - 15.0 * 2.0) + 15.0 + deg,
						CircleObj->get(),
						(double)((50.0 - 15.0 * 2.0) * (1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				}
				auto			GetGaugeDiff(void) const noexcept { return static_cast<float>(this->m_Now) - std::min(static_cast<float>(this->m_Max), this->m_Buffer); }
				const auto& GetGauge(void) const noexcept { return this->m_Now; }
				const auto& GetGaugeMax(void) const noexcept { return this->m_Max; }

			private:
				int Blend3Int(int pInt1, int pInt2, int pInt3, float per) const noexcept {
					int ans;
					ans = static_cast<int>(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * static_cast<float>(pInt3));
					ans += static_cast<int>(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * static_cast<float>(pInt2));
					ans += static_cast<int>(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * static_cast<float>(pInt1));
					return ans;
				}
				COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, float per) const noexcept {
					int r, g, b;
					r = Blend3Int(Color1.r, Color2.r, Color3.r, per);
					g = Blend3Int(Color1.g, Color2.g, Color3.g, per);
					b = Blend3Int(Color1.b, Color2.b, Color3.b, per);
					return GetColorU8(r, g, b, 255);
				}
			};

			class GaugeMask {
			private:
				int ultxp{ 0 }, ultyp{ 0 };
				GraphHandle UltGaugeMask;
				GraphHandle UltGauge;
			public:
				const auto& GetXSize(void) const noexcept { return this->ultxp; }
				const auto& GetYSize(void) const noexcept { return this->ultyp; }
				const auto& GetGraph(void) const noexcept { return this->UltGauge; }
			public:
				GaugeMask(void) noexcept {}
				GaugeMask(const GaugeMask&) = delete;
				GaugeMask(GaugeMask&& o) = delete;
				GaugeMask& operator=(const GaugeMask&) = delete;
				GaugeMask& operator=(GaugeMask&& o) = delete;

				~GaugeMask(void) noexcept {}
			public:
				void			Load(const char* Path) noexcept {
					UltGaugeMask.Load(Path);
					UltGaugeMask.GetSize(&ultxp, &ultyp);
					UltGauge.Make(ultxp, ultyp, true);
				}
				void			Dispose(void) noexcept {
					UltGaugeMask.Dispose();
					UltGauge.Dispose();
				}

				void SetDraw(std::function<void()> Doing) noexcept {
					int Prev = GetDrawScreen();
					UltGauge.SetDraw_Screen();
					{
						Doing();
					}
					SetDrawScreen(Prev);
					GraphBlend(UltGauge.get(), UltGaugeMask.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
						DX_RGBA_SELECT_SRC_R,
						DX_RGBA_SELECT_SRC_G,
						DX_RGBA_SELECT_SRC_B,
						DX_RGBA_SELECT_BLEND_R
					);
				}
			};
		private:
			std::array<GaugeParam, 3>		m_GaugeParam;
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;
			//std::array<std::unique_ptr<GaugeMask>, 2 + 3>	m_GaugeMask;

			GraphHandle						m_HeartIcon;
		public:
			UIClass(void) noexcept {}
			UIClass(const UIClass&) = delete;
			UIClass(UIClass&& o) = delete;
			UIClass& operator=(const UIClass&) = delete;
			UIClass& operator=(UIClass&& o) = delete;

			virtual ~UIClass(void) noexcept {}
		public:
			void			Load(void) noexcept {
				m_HeartIcon.Load("data/UI/Heart.png");
			}
			void			Dispose(void) noexcept {
				m_HeartIcon.Dispose();
			}
			void			Set(void) noexcept {
			}
			void			Draw(void) const noexcept {
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

					xp1 = DrawParts->GetUIY(1920 / 2);
					yp1 = DrawParts->GetUIY(10);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32), 
						FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "%d : %d", intParam[0], intParam[1]);
				}
				//情報
				{
					//心拍数
					xp1 = DrawParts->GetUIY((24 + 300 + 8 + 32));
					yp1 = DrawParts->GetUIY((1080 - 96 + 30 / 2));
					float per = Lerp(0.4f, 0.6f, floatParam[1]);
					if ((per * 255.f) > 1.f) {
						int radius = static_cast<int>(Lerp(static_cast<float>(DrawParts->GetUIY(24)), static_cast<float>(DrawParts->GetUIY(32)), floatParam[1]));
						WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
						WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal,
							255, 0, 0);
						WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
							&m_HeartIcon, xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
						WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 
							255, 255, 255);
						WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(24), 
						FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE, xp1, yp1, GetColor(255, 150, 150), Black, "%d", intParam[2]);

					//気合
					xp1 = DrawParts->GetUIY(24);
					yp1 = DrawParts->GetUIY(1080 - 96);

					m_GaugeParam[0].DrawGauge(
						xp1, yp1, xp1 + DrawParts->GetUIY(300), yp1 + DrawParts->GetUIY(12),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);
					//スタミナ
					xp1 = DrawParts->GetUIY(24);
					yp1 = DrawParts->GetUIY(1080 - 96 + 18);

					m_GaugeParam[1].DrawGauge(
						xp1, yp1, xp1 + DrawParts->GetUIY(300), yp1 + DrawParts->GetUIY(12),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);

					//スタミナ
					xp1 = DrawParts->GetUIY(24);
					yp1 = DrawParts->GetUIY(1080 - 96 + 18*2);

					m_GaugeParam[2].DrawGauge(
						xp1, yp1, xp1 + DrawParts->GetUIY(300), yp1 + DrawParts->GetUIY(12),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);
				}
			}

			void			InitGaugeParam(int ID, int value, int Max) noexcept { m_GaugeParam[static_cast<size_t>(ID)].InitGaugeParam(value, Max); }
			void			SetGaugeParam(int ID, int value, int Max, int Rate) noexcept { m_GaugeParam[static_cast<size_t>(ID)].SetGaugeParam(value, Max, Rate); }

			void			SetIntParam(int ID, int value) noexcept { intParam[static_cast<size_t>(ID)] = value; }
			void			SetfloatParam(int ID, float value) noexcept { floatParam[static_cast<size_t>(ID)] = value; }
			void			SetStrParam(int ID, std::string_view value) noexcept { strParam[static_cast<size_t>(ID)] = value; }
		};
	}
}
