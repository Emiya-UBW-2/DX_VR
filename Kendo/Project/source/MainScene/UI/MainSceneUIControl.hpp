#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
			class GaugeParam {
				int m_Now{0};
				int m_Max{100};
				float m_Buffer{0.f};
			public:
				void			InitGaugeParam(int value, int Max) {
					this->m_Now = value;
					this->m_Max = Max;
					this->m_Buffer = (float)this->m_Now;
				}
				void			SetGaugeParam(int value, int Max, int Rate) {
					auto* DrawParts = DXDraw::Instance();
					this->m_Now = value;
					this->m_Max = Max;
					float Limit = (float)(this->m_Max / Rate)*1.5f;
					this->m_Buffer += std::clamp((float)(this->m_Now - this->m_Buffer)*(float)(this->m_Max / Rate), -Limit, Limit) / DrawParts->GetFps();
				}
				void			DrawGauge(int xp1, int yp1, int xp2, int yp2,
										  COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) {
					//return;
					int ParamBuf = (int)(this->m_Buffer + 0.5f);
					DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int length = (xp2 - 1) - (xp1 + 1);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawBox(
						xp1 + 1 + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + 1,
						xp2 - 1, yp2 - 1,
						Black, TRUE);
					DrawBox(
						xp1 + 1, yp1 + 1,
						xp1 + 1 + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - 1,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					DrawBox(
						xp1 + 1 + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + 1,
						xp1 + 1 + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - 1,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeUp(int xp1, int yp1, int xp2, int yp2,
											COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) {
					//return;
					int ParamBuf = (int)(this->m_Buffer + 0.5f);
					DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int length = (yp2 - 1) - (yp1 + 1);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawBox(
						xp1 + 1, yp1 + 1,
						xp2 - 1, yp2 - 1 - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						Black, TRUE);
					DrawBox(
						xp1 + 1, yp2 - 1 - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + 1, yp2 - 1,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					DrawBox(
						xp1 + 1, yp2 - 1 - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + 1, yp2 - 1 - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeCircleLeft(int xp1, int yp1,
													COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
													GraphHandle* CircleObj, float deg, int Add) {
					//return;
					auto* DrawParts = DXDraw::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (this->m_Buffer > this->m_Now) ? ColorSub : ColorAdd;
					float per = std::clamp((float)this->m_Now / this->m_Max, 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / this->m_Max, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ADD, Add);
					SetDrawBright(ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCircleGauge(xp1 + y_r(256), yp1,
									50.0 + ((50.0 - 15.0*2.0)*1.0) + 15.0 + deg,
									CircleObj->get(),
									50.0 + 15.0 + deg,
									((double)(DrawParts->GetDispYSize()) / 1080.0));
					SetDrawBright(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 + y_r(256), yp1,
									50.0 + ((50.0 - 15.0*2.0)*(double)std::max(per, perbuf)) + 15.0 + deg,
									CircleObj->get(),
									50.0 + ((50.0 - 15.0*2.0)*(double)std::min(per, perbuf)) + 15.0 + deg,
									((double)(DrawParts->GetDispYSize()) / 1080.0));
					SetDrawBright(Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 + y_r(256), yp1,
									50.0 + ((50.0 - 15.0*2.0)*(double)std::min(per, perbuf)) + 15.0 + deg,
									CircleObj->get(),
									50.0 + 15.0 + deg,
									((double)(DrawParts->GetDispYSize()) / 1080.0));
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				void			DrawGaugeCircleRight(int xp1, int yp1,
													 COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
													 GraphHandle* CircleObj, float deg, int Add) {
					//return;
					auto* DrawParts = DXDraw::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)this->m_Now / (float)this->m_Max);
					COLOR_U8 ColorAddSub = (this->m_Buffer > this->m_Now) ? ColorSub : ColorAdd;
					float per = std::clamp((float)this->m_Now / this->m_Max, 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / this->m_Max, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ADD, Add);
					SetDrawBright(ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCircleGauge(xp1 - y_r(256), yp1,
						(50.0 - 15.0*2.0) + 15.0 + deg,
									CircleObj->get(),
									(double)((50.0 - 15.0*2.0)*(1.0 - 1.0)) + 15.0 + deg,
									((double)(DrawParts->GetDispYSize()) / 1080.0));
					SetDrawBright(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 - y_r(256), yp1,
						(double)((50.0 - 15.0*2.0)*(1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
									CircleObj->get(),
									(double)((50.0 - 15.0*2.0)*(1.0 - (double)std::max(per, perbuf))) + 15.0 + deg,
									((double)(DrawParts->GetDispYSize()) / 1080.0));
					SetDrawBright(Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 - y_r(256), yp1,
						(50.0 - 15.0*2.0) + 15.0 + deg,
									CircleObj->get(),
									(double)((50.0 - 15.0*2.0)*(1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
									((double)(DrawParts->GetDispYSize()) / 1080.0));
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				const auto		GetGaugeDiff() const noexcept { return (float)this->m_Now - std::min((float)this->m_Max, this->m_Buffer); }
				const auto&		GetGauge() const noexcept { return this->m_Now; }
				const auto&		GetGaugeMax() const noexcept { return this->m_Max; }

			private:
				int Blend3Int(int pInt1, int pInt2, int pInt3, float per) {
					int ans;
					ans = (int)(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * (float)pInt3);
					ans += (int)(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * (float)pInt2);
					ans += (int)(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * (float)pInt1);
					return ans;
				}
				COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, float per) {
					int r, g, b;
					r = Blend3Int(Color1.r, Color2.r, Color3.r, per);
					g = Blend3Int(Color1.g, Color2.g, Color3.g, per);
					b = Blend3Int(Color1.b, Color2.b, Color3.b, per);
					return GetColorU8(r, g, b, 255);
				}
			};

			class GaugeMask {
			private:
				int ultxp{0}, ultyp{0};
				GraphHandle UltGaugeMask;
				GraphHandle UltGauge;
			public:
				const auto&		GetXSize(void) const noexcept { return this->ultxp; }
				const auto&		GetYSize(void) const noexcept { return this->ultyp; }
				const auto&		GetGraph(void) const noexcept { return this->UltGauge; }
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
			std::array<GaugeParam, 2>		m_GaugeParam;
			std::array<int, 2>				intParam{0};
			std::array<float, 6>			floatParam{0};
			std::array<std::string, 1>		strParam;
			//std::array<GaugeMask, 2 + 3>	m_GaugeMask;

			GraphHandle						m_HeartIcon;
			GraphHandle						m_GuardScreen;
			GraphHandle						m_GuardGraph;
			GraphHandle						m_GuardPGraph;
			GraphHandle						m_CursorGraph;
		private:
		public:
			void			Load() noexcept {
				m_HeartIcon = GraphHandle::Load("data/UI/Heart.png");
				m_GuardGraph = GraphHandle::Load("data/UI/Guard.png");
				m_GuardPGraph = GraphHandle::Load("data/UI/GuardP.png");
				m_CursorGraph = GraphHandle::Load("data/UI/Cursor.png");

				m_GuardScreen = GraphHandle::Make(512, 512, false);
			}
			void			Dispose(void) noexcept {
				m_HeartIcon.Dispose();
				m_GuardGraph.Dispose();
				m_GuardPGraph.Dispose();
				m_CursorGraph.Dispose();
			}
			void			Set() noexcept {
			}
			void			Draw(float scale) noexcept {
				auto* Fonts = FontPool::Instance();
				int xp1, yp1;
				//タイム,スコア
				{
					xp1 = y_r(30.f*scale);
					yp1 = y_r(10.f*scale);
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(32.f*scale), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "TIME");
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(32.f*scale), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240.f*scale), yp1, White, Black, "%d:%05.2f",
						(int)(floatParam[0] / 60.f), (float)((int)(floatParam[0]) % 60) + (floatParam[0] - (float)((int)(floatParam[0]))));

					xp1 = y_r((1920 / 2)*scale);
					yp1 = y_r(10.f*scale);
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(32.f*scale), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "%d : %d", intParam[0], intParam[1]);
				}
				//情報
				{
					//心拍数
					xp1 = y_r((24 + 300 + 8 + 32)*scale);
					yp1 = y_r((1080 - 96 + 30 / 2)*scale);
					float per = Lerp(0.4f, 0.6f, floatParam[1]);
					if ((per *255.f) > 1.f) {
						int radius = (int)Lerp((float)y_r(24.f*scale), (float)y_r(32.f*scale), floatParam[1]);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
						SetDrawBright(255, 0, 0);
						m_HeartIcon.DrawExtendGraph(xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
						SetDrawBright(255, 255, 255);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(24.f*scale), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE, xp1, yp1, GetColor(255, 150, 150), Black, "%d", intParam[2]);

					//気合
					xp1 = y_r(24.f*scale);
					yp1 = y_r((1080 - 96)*scale);

					m_GaugeParam[0].DrawGauge(
						xp1, yp1, xp1 + y_r(300.f*scale), yp1 + y_r(12.f*scale),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 128, 0, 255), GetColorU8(255, 255, 0, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);
					//スタミナ
					xp1 = y_r(24.f*scale);
					yp1 = y_r((1080 - 96 + 18)*scale);

					m_GaugeParam[1].DrawGauge(
						xp1, yp1, xp1 + y_r(300.f*scale), yp1 + y_r(12.f*scale),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);
				}
				//ガード
				{
					if ((floatParam[2] * 255.f) > 1.f) {
						auto Prev = GetDrawScreen();
						m_GuardScreen.SetDraw_Screen();
						{
							{
								xp1 = 255;
								yp1 = 255;
								int radius = 128;
								float length = std::hypotf(floatParam[3], floatParam[4]);
								if (length > 0.f) {
									xp1 += (int)((float)(256 - 32) * floatParam[3] / length);
									yp1 += (int)((float)(256 - 32) * floatParam[4] / length);
								}
								float per = std::clamp(length, 0.f, 1.f)*floatParam[2];
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
								SetDrawBright(100, 255, 100);
								m_GuardPGraph.DrawExtendGraph(xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
								SetDrawBright(255, 255, 255);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}
							{
								xp1 = 255;
								yp1 = 255;
								int radius = 258;
								SetDrawBlendMode(DX_BLENDMODE_MUL, 255);
								m_GuardGraph.DrawExtendGraph(xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}

						}
						GraphHandle::SetDraw_Screen(Prev, false);
						{
							{
								xp1 = y_r((1920 / 2)*scale);
								yp1 = y_r((1080 / 2)*scale);
								int radius = (int)Lerp((float)y_r(0.f*scale), (float)y_r(256.f*scale), floatParam[2]);
								SetDrawBlendMode(DX_BLENDMODE_ADD, (int)Lerp(128.f, 32.f, floatParam[5]));
								m_GuardGraph.DrawExtendGraph(xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
								SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
								m_GuardScreen.DrawExtendGraph(xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}
							{
								xp1 = y_r((1920 / 2)*scale);
								yp1 = y_r((1080 / 2)*scale);
								xp1 += (int)((float)y_r(256.f*scale) * floatParam[3]);
								yp1 += (int)((float)y_r(256.f*scale) * floatParam[4]);

								int radius = (int)Lerp((float)y_r(0.f*scale), (float)y_r(32.f*scale), floatParam[2]);
								float length = std::hypotf(floatParam[3], floatParam[4]);
								if (length > 1.f) {
									radius = (int)Lerp((float)radius, (float)y_r(0.f*scale), length - 1.f);
								}
								SetDrawBlendMode(DX_BLENDMODE_ADD, 128);
								m_CursorGraph.DrawExtendGraph(xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}
						}
					}
				}
			}

			void			InitGaugeParam(int ID, int value, int Max) { m_GaugeParam[ID].InitGaugeParam(value, Max); }
			void			SetGaugeParam(int ID, int value, int Max, int Rate) { m_GaugeParam[ID].SetGaugeParam(value, Max, Rate); }

			void			SetIntParam(int ID, int value) { intParam[ID] = value; }
			void			SetfloatParam(int ID, float value) { floatParam[ID] = value; }
			void			SetStrParam(int ID, std::string_view value) { strParam[ID] = value; }
		};
	};
};
