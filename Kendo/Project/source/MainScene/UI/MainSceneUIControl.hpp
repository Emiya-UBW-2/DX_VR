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
						float Limit = static_cast<float>(this->m_Max / Rate);
						this->m_Buffer += std::clamp(static_cast<float>(static_cast<float>(this->m_Now) - this->m_Buffer) * Limit, -Limit * 1.5f, Limit * 1.5f) * DrawParts->GetDeltaTime();
					}
					else {
						this->m_Buffer = static_cast<float>(this->m_Now);
					}
				}
				void			DrawGauge(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) const noexcept {
					//return;
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int Edge = 0;
					int length = (xp2 - Edge) - (xp1 + Edge);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + Edge,
						xp2 - Edge, yp2 - Edge,
						Black, TRUE);
					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp1 + Edge,
						xp1 + Edge + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - Edge,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + Edge,
						xp1 + Edge + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - Edge,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeUp(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int Edge = 0;
					int length = (yp2 - Edge) - (yp1 + Edge);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp1 + Edge,
						xp2 - Edge, yp2 - Edge - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						Black, TRUE);
					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp2 - Edge - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + Edge, yp2 - Edge,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp2 - Edge - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + Edge, yp2 - Edge - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeCircleLeft(int xp1, int yp1,
					COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
					const GraphHandle* CircleObj, float deg, int Add) noexcept {
					//return;
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					auto* DrawParts = DXDraw::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (this->m_Buffer > this->m_Now) ? ColorSub : ColorAdd;
					float per = std::clamp((float)this->m_Now / this->m_Max, 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / this->m_Max, 0.f, 1.f);
					WindowParts->SetAdd(WindowSystem::DrawLayer::Normal, Add);
					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, ColorBase.r, ColorBase.g, ColorBase.b);
					WindowParts->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 + DrawParts->GetUIY(256), yp1,
						50.0f + ((50.0f - 15.0f * 2.0f) * 1.0f) + 15.0f + deg,
						50.0f + 15.0f + deg,
						((float)(DrawParts->GetUIY(1080)) / 1080.0f));
					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					WindowParts->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 + DrawParts->GetUIY(256), yp1,
						50.0f + ((50.0f - 15.0f * 2.0f) * std::max(per, perbuf)) + 15.0f + deg,
						50.0f + ((50.0f - 15.0f * 2.0f) * std::min(per, perbuf)) + 15.0f + deg,
						((float)(DrawParts->GetUIY(1080)) / 1080.0f));
					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, Color.r, Color.g, Color.b);
					WindowParts->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 + DrawParts->GetUIY(256), yp1,
						50.0f + ((50.0f - 15.0f * 2.0f) * std::min(per, perbuf)) + 15.0f + deg,
						50.0f + 15.0f + deg,
						((float)(DrawParts->GetUIY(1080)) / 1080.0f));
					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				}
				void			DrawGaugeCircleRight(int xp1, int yp1,
					COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
					const GraphHandle* CircleObj, float deg, int Add) noexcept {
					//return;
					auto* DrawParts = DXDraw::Instance();
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (this->m_Buffer > this->m_Now) ? ColorSub : ColorAdd;
					float per = std::clamp((float)this->m_Now / this->m_Max, 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / this->m_Max, 0.f, 1.f);
					WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					WindowParts->SetAdd(WindowSystem::DrawLayer::Normal, Add);
					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, ColorBase.r, ColorBase.g, ColorBase.b);

					WindowParts->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 - DrawParts->GetUIY(256), yp1,
						(50.0f - 15.0f * 2.0f) + 15.0f + deg,
						((50.0f - 15.0f * 2.0f) * (1.0f - 1.0f)) + 15.0f + deg,
						((float)(DrawParts->GetUIY(1080)) / 1080.0f));

					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);

					WindowParts->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 - DrawParts->GetUIY(256), yp1,
						((50.0f - 15.0f * 2.0f) * (1.0f - std::min(per, perbuf))) + 15.0f + deg,
						((50.0f - 15.0f * 2.0f) * (1.0f - std::max(per, perbuf))) + 15.0f + deg,
						((float)(DrawParts->GetUIY(1080)) / 1080.0f));

					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, Color.r, Color.g, Color.b);
					WindowParts->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 - DrawParts->GetUIY(256), yp1,
						(50.0f - 15.0f * 2.0f) + 15.0f + deg,
						((50.0f - 15.0f * 2.0f) * (1.0f - std::min(per, perbuf))) + 15.0f + deg,
						((float)(DrawParts->GetUIY(1080)) / 1080.0f));

					WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				}
				const auto	GetGaugeBuff() const noexcept { return static_cast<int>(this->m_Buffer + 0.5f); }
				const auto& GetGauge() const noexcept { return this->m_Now; }
				const auto& GetGaugeMax() const noexcept { return this->m_Max; }

				void			DrawGaugeMask(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int Edge = 0;
					int length = (xp2 - Edge) - (xp1 + Edge);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawBox(
						xp1 + Edge + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + Edge,
						xp2 - Edge, yp2 - Edge,
						Black, TRUE);
					DrawBox(
						xp1 + Edge, yp1 + Edge,
						xp1 + Edge + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - Edge,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					DrawBox(
						xp1 + Edge + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + Edge,
						xp1 + Edge + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - Edge,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeUpMask(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int Edge = 0;
					int length = (yp2 - Edge) - (yp1 + Edge);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawBox(
						xp1 + Edge, yp1 + Edge,
						xp2 - Edge, yp2 - Edge - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						Black, TRUE);
					DrawBox(
						xp1 + Edge, yp2 - Edge - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + Edge, yp2 - Edge,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					DrawBox(
						xp1 + Edge, yp2 - Edge - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + Edge, yp2 - Edge - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
			private:
				int Blend3Int(int pInt1, int pInt2, int pInt3, float per) const noexcept {
					int ans;
					ans = static_cast<int>(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * (float)pInt3);
					ans += static_cast<int>(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * (float)pInt2);
					ans += static_cast<int>(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * (float)pInt1);
					return ans;
				}
				COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 Color4, float per) const noexcept {
					int r, g, b;
					r = Blend3Int(Color1.r, Color2.r, Color3.r, per);
					g = Blend3Int(Color1.g, Color2.g, Color3.g, per);
					b = Blend3Int(Color1.b, Color2.b, Color3.b, per);
					if (per >= 1.f) {
						r = Color4.r;
						g = Color4.g;
						b = Color4.b;
					}
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
			GraphHandle						m_Kiai;
			GraphHandle						m_Mine;
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
				m_Kiai.Load("data/UI/Kiai.png");
				m_Mine.Load("data/UI/Mine.png");
			}
			void			Dispose(void) noexcept {
				m_HeartIcon.Dispose();
				m_Kiai.Dispose();
				m_Mine.Dispose();
			}
			void			Set(void) noexcept {
			}
			void			Draw(void) const noexcept {
				auto* DrawParts = DXDraw::Instance();
				//気合
				{
					float radius = Lerp(static_cast<float>(1.f), static_cast<float>(1.01f), floatParam[2]);
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal,
						255, 150, 155);
					WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255 * (m_GaugeParam[0].GetGaugeMax() - m_GaugeParam[0].GetGaugeBuff()) / m_GaugeParam[0].GetGaugeMax());
					WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Kiai,
						static_cast<int>(DrawParts->GetUIXMax() / 2 - DrawParts->GetUIXMax() / 2 * radius),
						static_cast<int>(DrawParts->GetUIYMax() / 2 - DrawParts->GetUIYMax() / 2 * radius),
						static_cast<int>(DrawParts->GetUIXMax() / 2 + DrawParts->GetUIXMax() / 2 * radius),
						static_cast<int>(DrawParts->GetUIYMax() / 2 + DrawParts->GetUIYMax() / 2 * radius),
						true);
					WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal,
						255, 255, 255);
				}
				int xp1, yp1;
				//タイム
				{
					xp1 = DrawParts->GetUIY(30);
					yp1 = DrawParts->GetUIY(static_cast<int>(Lerp(10.0f, -50.f, std::clamp(floatParam[1] - 0.5f, 0.f, 1.f))));
					float per = std::cos(DX_PI_F * 10.f * floatParam[1]);
					if ((per * 255.f) > 1.f) {
						WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32),
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, (floatParam[0] < 60.f) ? Red : Yellow, Black, "TIME");
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32),
							FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + DrawParts->GetUIY(300), yp1, (floatParam[0] < 60.f) ? Red : Yellow, Black, "%d:%05.2f",
							static_cast<int>(floatParam[0] / 60.f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));
						WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}
				}
				//スコア
				{
					xp1 = DrawParts->GetUIY(1920 / 2);
					yp1 = DrawParts->GetUIY(static_cast<int>(Lerp(28.0f, -100.f, std::clamp(floatParam[1] - 0.5f, 0.f, 1.f))));

					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal,
						92, 0, 0);
					WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Mine,
						xp1 - DrawParts->GetUIY(10) - DrawParts->GetUIY(40) - DrawParts->GetUIY(36) + DrawParts->GetUIY(1),
						yp1 - DrawParts->GetUIY(10) + DrawParts->GetUIY(1),
						xp1 - DrawParts->GetUIY(10) - DrawParts->GetUIY(40) + DrawParts->GetUIY(1),
						yp1 - DrawParts->GetUIY(10) + DrawParts->GetUIY(36) + DrawParts->GetUIY(1),
						true);
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal,
						192, 0, 0);
					WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Mine,
						xp1 - DrawParts->GetUIY(10) - DrawParts->GetUIY(40) - DrawParts->GetUIY(36),
						yp1 - DrawParts->GetUIY(10),
						xp1 - DrawParts->GetUIY(10) - DrawParts->GetUIY(40),
						yp1 - DrawParts->GetUIY(10) + DrawParts->GetUIY(36),
						true);
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal,
						255, 255, 255);
					

					WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 - DrawParts->GetUIY(10) - DrawParts->GetUIY(40), yp1 + DrawParts->GetUIY(20),
						xp1 + DrawParts->GetUIY(10) + DrawParts->GetUIY(40), yp1 + DrawParts->GetUIY(24), Black, true);

					WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 - DrawParts->GetUIY(10) - DrawParts->GetUIY(40 - 20 * intParam[1]), yp1 + DrawParts->GetUIY(20),
						xp1 - DrawParts->GetUIY(10), yp1 + DrawParts->GetUIY(24), Red, true);
					int Y1add = DrawParts->GetUIY(static_cast<int>(Lerp(0.f, -8.f, floatParam[3])));
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32),
						FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1 - DrawParts->GetUIY(20), yp1 + Y1add, Red, Black, "%d", intParam[0]);

					WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 - DrawParts->GetUIY(10), yp1 + DrawParts->GetUIY(20),
						xp1 + DrawParts->GetUIY(10), yp1 + DrawParts->GetUIY(24), Gray75, true);
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(24),
						FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Gray25, Black, ":");

					WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + DrawParts->GetUIY(10), yp1 + DrawParts->GetUIY(20),
						xp1 + DrawParts->GetUIY(10) + DrawParts->GetUIY(40 - 20 * intParam[0]), yp1 + DrawParts->GetUIY(24), White, true);
					int Y2add = DrawParts->GetUIY(static_cast<int>(Lerp(0.f, -8.f, floatParam[4])));
					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32),
						FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1 + DrawParts->GetUIY(20), yp1 + Y2add, White, Black, "%d", intParam[1]);
				}
				//情報
				{
					//心拍数
					xp1 = DrawParts->GetUIY((24 + 300 + 24 + 32));
					yp1 = DrawParts->GetUIY(1080 - 80 - 28 * 1 + 6 / 2);
					float per = Lerp(0.4f, 0.6f, floatParam[2]);
					if ((per * 255.f) > 1.f) {
						int radius = static_cast<int>(Lerp(static_cast<float>(DrawParts->GetUIY(24)), static_cast<float>(DrawParts->GetUIY(32)), floatParam[2]));
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
					xp1 = DrawParts->GetUIY(24 + 9 * 2);
					yp1 = DrawParts->GetUIY(1080 - 80 - 28 * 2);

					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18),
						FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1 - DrawParts->GetUIY(2), White, Black, "気合");
					m_GaugeParam[0].DrawGauge(
						xp1, yp1, xp1 + DrawParts->GetUIY(300), yp1 + DrawParts->GetUIY(6),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(128, 255, 0, 255), GetColorU8(192, 255, 192, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);

					//スタミナ
					xp1 = DrawParts->GetUIY(24 + 9 * 1);
					yp1 = DrawParts->GetUIY(1080 - 80 - 28 * 1);

					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18),
						FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1 - DrawParts->GetUIY(2), White, Black, "スタミナ");
					m_GaugeParam[1].DrawGauge(
						xp1, yp1, xp1 + DrawParts->GetUIY(300), yp1 + DrawParts->GetUIY(6),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(128, 255, 0, 255), GetColorU8(192, 255, 192, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);

					//ガード
					xp1 = DrawParts->GetUIY(24 + 9 * 0);
					yp1 = DrawParts->GetUIY(1080 - 80 - 28 * 0);

					WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18),
						FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1 - DrawParts->GetUIY(2), White, Black, "ガード");
					m_GaugeParam[2].DrawGauge(
						xp1, yp1, xp1 + DrawParts->GetUIY(300), yp1 + DrawParts->GetUIY(6),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(128, 255, 0, 255), GetColorU8(192, 255, 192, 255),
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
