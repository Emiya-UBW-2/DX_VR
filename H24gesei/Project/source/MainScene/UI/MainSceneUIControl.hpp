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
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawBoxLine_2D(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White);
					int length = (xp2 - 1) - (xp1 + 1);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawBox_2D(
						xp1 + 1 + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + 1,
						xp2 - 1, yp2 - 1,
						Black, true);
					DrawBox_2D(
						xp1 + 1, yp1 + 1,
						xp1 + 1 + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - 1,
						GetColor(Color.r, Color.g, Color.b), true);
					DrawBox_2D(
						xp1 + 1 + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + 1,
						xp1 + 1 + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - 1,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), true);
				}
				void			DrawGaugeUp(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawBoxLine_2D(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White);
					int length = (yp2 - 1) - (yp1 + 1);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawBox_2D(
						xp1 + 1, yp1 + 1,
						xp2 - 1, yp2 - 1 - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						Black, true);
					DrawBox_2D(
						xp1 + 1, yp2 - 1 - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + 1, yp2 - 1,
						GetColor(Color.r, Color.g, Color.b), true);
					DrawBox_2D(
						xp1 + 1, yp2 - 1 - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + 1, yp2 - 1 - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), true);
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
					SetDrawBlendMode(DX_BLENDMODE_ADD, Add);
					SetDrawBright(ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCircleGauge(xp1 + DrawParts->GetScreenY(256), yp1,
						50.0 + ((50.0 - 15.0 * 2.0) * 1.0) + 15.0 + deg,
						CircleObj->get(),
						50.0 + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					SetDrawBright(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 + DrawParts->GetScreenY(256), yp1,
						50.0 + ((50.0 - 15.0 * 2.0) * (double)std::max(per, perbuf)) + 15.0 + deg,
						CircleObj->get(),
						50.0 + ((50.0 - 15.0 * 2.0) * (double)std::min(per, perbuf)) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					SetDrawBright(Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 + DrawParts->GetScreenY(256), yp1,
						50.0 + ((50.0 - 15.0 * 2.0) * (double)std::min(per, perbuf)) + 15.0 + deg,
						CircleObj->get(),
						50.0 + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
					SetDrawBlendMode(DX_BLENDMODE_ADD, Add);
					SetDrawBright(ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCircleGauge(xp1 - DrawParts->GetScreenY(256), yp1,
						(50.0 - 15.0 * 2.0) + 15.0 + deg,
						CircleObj->get(),
						(double)((50.0 - 15.0 * 2.0) * (1.0 - 1.0)) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					SetDrawBright(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 - DrawParts->GetScreenY(256), yp1,
						(double)((50.0 - 15.0 * 2.0) * (1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
						CircleObj->get(),
						(double)((50.0 - 15.0 * 2.0) * (1.0 - (double)std::max(per, perbuf))) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					SetDrawBright(Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 - DrawParts->GetScreenY(256), yp1,
						(50.0 - 15.0 * 2.0) + 15.0 + deg,
						CircleObj->get(),
						(double)((50.0 - 15.0 * 2.0) * (1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
						((double)(DrawParts->GetUIY(1080)) / 1080.0));
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				auto			GetGaugeDiff(void) const noexcept { return static_cast<float>(this->m_Now) - std::min(static_cast<float>(this->m_Max), this->m_Buffer); }
				const auto& GetGauge(void) const noexcept { return this->m_Now; }
				const auto& GetGaugeMax(void) const noexcept { return this->m_Max; }

			private:
				int Blend3Int(int pInt1, int pInt2, int pInt3, float per) noexcept {
					int ans;
					ans = static_cast<int>(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * static_cast<float>(pInt3));
					ans += static_cast<int>(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * static_cast<float>(pInt2));
					ans += static_cast<int>(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * static_cast<float>(pInt1));
					return ans;
				}
				COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, float per) noexcept {
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
					UltGaugeMask = GraphHandle::Load(Path);
					UltGaugeMask.GetSize(&ultxp, &ultyp);
					UltGauge = GraphHandle::Make(ultxp, ultyp, true);
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
			}

			void			InitGaugeParam(int ID, int value, int Max) noexcept { m_GaugeParam[static_cast<size_t>(ID)].InitGaugeParam(value, Max); }
			void			SetGaugeParam(int ID, int value, int Max, int Rate) noexcept { m_GaugeParam[static_cast<size_t>(ID)].SetGaugeParam(value, Max, Rate); }

			void			SetIntParam(int ID, int value) noexcept { intParam[static_cast<size_t>(ID)] = value; }
			void			SetfloatParam(int ID, float value) noexcept { floatParam[static_cast<size_t>(ID)] = value; }
			void			SetStrParam(int ID, std::string_view value) noexcept { strParam[static_cast<size_t>(ID)] = value; }
		};
	}
}
