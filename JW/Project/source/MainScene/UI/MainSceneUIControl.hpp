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
				void			SetGaugeParam(int value, int Max) {
					this->m_Now = value;
					this->m_Max = Max;
					float Limit = (float)(this->m_Max / 5)*1.5f;
					this->m_Buffer += std::clamp((float)(this->m_Now - this->m_Buffer)*(float)(this->m_Max / 5), -Limit, Limit) / FPS;
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
									((double)(DrawParts->m_DispYSize) / 1080.0));
					SetDrawBright(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 + y_r(256), yp1,
									50.0 + ((50.0 - 15.0*2.0)*(double)std::max(per, perbuf)) + 15.0 + deg,
									CircleObj->get(),
									50.0 + ((50.0 - 15.0*2.0)*(double)std::min(per, perbuf)) + 15.0 + deg,
									((double)(DrawParts->m_DispYSize) / 1080.0));
					SetDrawBright(Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 + y_r(256), yp1,
									50.0 + ((50.0 - 15.0*2.0)*(double)std::min(per, perbuf)) + 15.0 + deg,
									CircleObj->get(),
									50.0 + 15.0 + deg,
									((double)(DrawParts->m_DispYSize) / 1080.0));
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
									((double)(DrawParts->m_DispYSize) / 1080.0));
					SetDrawBright(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCircleGauge(xp1 - y_r(256), yp1,
						(double)((50.0 - 15.0*2.0)*(1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
									CircleObj->get(),
									(double)((50.0 - 15.0*2.0)*(1.0 - (double)std::max(per, perbuf))) + 15.0 + deg,
									((double)(DrawParts->m_DispYSize) / 1080.0));
					SetDrawBright(Color.r, Color.g, Color.b);
					DrawCircleGauge(xp1 - y_r(256), yp1,
						(50.0 - 15.0*2.0) + 15.0 + deg,
									CircleObj->get(),
									(double)((50.0 - 15.0*2.0)*(1.0 - (double)std::min(per, perbuf))) + 15.0 + deg,
									((double)(DrawParts->m_DispYSize) / 1080.0));
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
			std::array<GaugeParam, 5 + 3 + 2>	m_GaugeParam;
			std::array<int, 23>				intParam{0};
			std::array<float, 5>			floatParam{0};
			std::array<std::string, 7>		strParam;
			GraphHandle						Gauge_Graph;
			GraphHandle						Gauge_Aim_Graph;
			GraphHandle						OIL_Graph;
			GraphHandle						ULT_Graph;

			float							ULTTimer{0.f};

			std::array<std::pair<int, float>, 6> ScoreAdd{};
			int m_ScoreAddSel{0};

			std::array<GaugeMask, 2 + 3>	m_GaugeMask;
		private:
		public:
			void			Load() noexcept {
				this->Gauge_Graph = GraphHandle::Load("data/UI/Gauge.png");
				this->Gauge_Aim_Graph = GraphHandle::Load("data/UI/Gauge_Aim.png");
				this->OIL_Graph = GraphHandle::Load("data/UI/back.png");

			}
			void			Dispose(void) noexcept {
				this->Gauge_Graph.Dispose();
				this->Gauge_Aim_Graph.Dispose();
				this->OIL_Graph.Dispose();
				for (auto& m : m_GaugeMask) {
					m.Dispose();
				}
			}
			void			Set(const char* GunName, const char* ULTName, bool isHardmode) noexcept {
				m_ScoreAddSel = 0;
				{
					std::string Path = "data/gun/";
					Path += GunName;
					Path += "/Gauge.png";
					m_GaugeMask.at(1).Dispose();
					m_GaugeMask.at(1).Load(Path.c_str());
				}
				{
					std::string Path = "data/gun/";
					Path += ULTName;
					Path += "/Gauge.png";
					m_GaugeMask.at(0).Dispose();
					m_GaugeMask.at(0).Load(Path.c_str());
					ULT_Graph = GraphHandle::Load(Path);
				}
				//
				for (int i = 0; i < 3; i++) {
					m_GaugeMask.at((size_t)i + 2).Dispose();
				}
				if (isHardmode) {
					for (int i = 0; i < 3; i++) {
						m_GaugeMask.at((size_t)i + 2).Load("data/UI/MagM4.png");
					}
				}
				else {
					for (int i = 0; i < 3; i++) {
						m_GaugeMask.at((size_t)i + 2).Load("data/UI/Mag.png");
					}
				}
			}
			void			Draw(void) noexcept {
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				//ダメージ表示
				{
					auto per = std::clamp(-(m_GaugeParam[0].GetGaugeDiff())*1.f, 0.f, 1.f);
					if (per > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(192.f*per), 0, 96));

						OIL_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);

						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
				}
				//タイム,スコア
				{
					int xp1, yp1;
					xp1 = y_r(30);
					yp1 = y_r(10);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Gray75, "TIME");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240), yp1, White, Gray75, "%d:%05.2f",
						(int)(floatParam[0] / 60.f),
																		 (float)((int)(floatParam[0]) % 60) + (floatParam[0] - (float)((int)(floatParam[0])))
					);

					yp1 += y_r(25);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Gray75, "KILL");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240), yp1, White, Gray75, "%d", intParam[6]);
				}
				//ポイント増加
				{
					int xp1, yp1;
					xp1 = DrawParts->m_DispXSize / 2;
					yp1 = DrawParts->m_DispYSize / 2;

					if (intParam[5] > 0) {
						ScoreAdd.at(m_ScoreAddSel) = std::make_pair(intParam[5], 2.f);
						++m_ScoreAddSel %= (int)ScoreAdd.size();
					}
					for (int i = 0; i < (int)ScoreAdd.size(); i++) {
						auto& s = ScoreAdd[i];
						if (s.second > 0.f) {
							float per = std::powf(2.f - s.second, 2.f);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*(1.f - per)), 0, 255));
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, xp1, yp1 - y_r(per*96.f), GetColor(206, 0, 0), Gray75, "+%d", s.first);
							s.second = std::max(s.second - 1.f / FPS, 0.f);
						}
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				//情報
				{
					int xp1, yp1;
					//体力
					xp1 = y_r(24);
					yp1 = DrawParts->m_DispYSize - y_r(96);

					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(36), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1 + y_r(300) + y_r(18), yp1 + y_r(6), White, Gray75, "HP");

					m_GaugeParam[0].DrawGauge(
						xp1, yp1, xp1 + y_r(300), yp1 + y_r(18),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
						GetColorU8(0, 0, 255, 255), GetColorU8(255, 0, 0, 255)
					);
					yp1 += y_r(10);
				}
				//高度、速度
				{
					float deg = (float)(intParam[2]) / 60.f;
					float rad = deg2rad(deg);
					int xp1, yp1;
					{
						xp1 = DrawParts->m_DispXSize / 2 + intParam[0] - y_r(300.f*std::cos(rad));
						yp1 = DrawParts->m_DispYSize / 2 + intParam[1] - y_r(300.f*std::sin(rad)) - y_r(18) / 2;
						m_GaugeParam[1].DrawGaugeCircleLeft(xp1, yp1,
															GetColorU8(255, 255, 255, 255),
															GetColorU8(255, 128, 128, 255), GetColorU8(255, 255, 128, 255), GetColorU8(64, 64, 255, 255),
															GetColorU8(0, 255, 0, 255), GetColorU8(255, 0, 0, 255),
															&this->Gauge_Graph, deg, 32);
					}
					{
						xp1 = DrawParts->m_DispXSize / 2 + intParam[0] + y_r(300.f*std::cos(rad));
						yp1 = DrawParts->m_DispYSize / 2 + intParam[1] + y_r(300.f*std::sin(rad)) - y_r(18) / 2;
						m_GaugeParam[2].DrawGaugeCircleRight(xp1, yp1,
															 GetColorU8(255, 255, 255, 255),
															 GetColorU8(255, 128, 128, 255), GetColorU8(255, 255, 128, 255), GetColorU8(64, 64, 255, 255),
															 GetColorU8(0, 255, 0, 255), GetColorU8(255, 0, 0, 255),
															 &this->Gauge_Graph, deg, 32);
					}
				}
				//オートエイム
				{
					int ID = intParam[4];
					int Per = (int)(floatParam[2] * 96.f);
					if (ID > 0 || Per > 0) {
						float deg = (float)(intParam[2]) / 60.f;
						float rad = deg2rad(deg);
						int xp1, yp1;
						{
							xp1 = DrawParts->m_DispXSize / 2 + intParam[0] - y_r(300.f*std::cos(rad));
							yp1 = DrawParts->m_DispYSize / 2 + intParam[1] - y_r(300.f*std::sin(rad)) - y_r(18) / 2;
							m_GaugeParam[5 + 3].DrawGaugeCircleLeft(xp1, yp1,
																	GetColorU8(255, 255, 255, 64),
																	GetColorU8(255, 0, 0, 255), GetColorU8(255, 0, 0, 255), GetColorU8(255, 0, 0, 255),
																	GetColorU8(255, 0, 0, 255), GetColorU8(0, 0, 0, 0),
																	&this->Gauge_Aim_Graph, deg, Per);
						}
						{
							xp1 = DrawParts->m_DispXSize / 2 + intParam[0] + y_r(300.f*std::cos(rad));
							yp1 = DrawParts->m_DispYSize / 2 + intParam[1] + y_r(300.f*std::sin(rad)) - y_r(18) / 2;
							m_GaugeParam[5 + 3 + 1].DrawGaugeCircleRight(xp1, yp1,
																		 GetColorU8(255, 255, 255, 64),
																		 GetColorU8(255, 0, 0, 255), GetColorU8(255, 0, 0, 255), GetColorU8(255, 0, 0, 255),
																		 GetColorU8(255, 0, 0, 255), GetColorU8(0, 0, 0, 0),
																		 &this->Gauge_Aim_Graph, deg, Per);
						}
					}
				}
				//リロード表示
				{
					if (m_GaugeParam[2].GetGauge() < m_GaugeParam[2].GetGaugeMax() * 3 / 10) {
						int xp1, yp1;
						//体力
						xp1 = DrawParts->m_DispXSize / 2;
						yp1 = DrawParts->m_DispYSize / 2 + y_r(100);

						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, xp1, yp1, Red, Gray75,
																			 "残弾が少ない");

					}
				}
				//タイム
				if (floatParam[1] > 0) {
					int xp1, yp1;
					xp1 = DrawParts->m_DispXSize / 2;
					yp1 = DrawParts->m_DispYSize / 2 - y_r(64);
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(48), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE, xp1, yp1, White, Gray75, "%d:%05.2f",
						(int)(floatParam[1] / 60.f),
																		  (float)((int)(floatParam[1]) % 60) + (floatParam[1] - (float)((int)(floatParam[1])))
					);
				}
				float UltPer = 1.f - floatParam[3];
				//ゲージ
				{
					m_GaugeMask.at(0).SetDraw([&]() {
						m_GaugeParam[3].DrawGaugeUp(
							-1, -1, 1 + m_GaugeMask.at(0).GetXSize(), 1 + m_GaugeMask.at(0).GetYSize(),
							GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColorU8(0, 0, 255, 255), GetColorU8(255, 0, 0, 255)
						);
											 });
					//
					int xp1, yp1;
					float rad = deg2rad(-90)*UltPer;
					xp1 = y_r(1920 - 20 - m_GaugeMask.at(0).GetXSize() * 3 / 10);
					yp1 = y_r(555 + m_GaugeMask.at(0).GetYSize() * 3 / 10);
					xp1 = (int)Lerp((float)xp1, (float)y_r(1920 - 20 - m_GaugeMask.at(0).GetYSize() * 3 / 10), UltPer);
					yp1 = (int)Lerp((float)yp1, (float)y_r(960 + m_GaugeMask.at(0).GetXSize() * 3 / 10), UltPer);
					if (intParam[7] == -1) {
						xp1 = y_r(1920 - 20 - m_GaugeMask.at(0).GetXSize() * 3 / 10);
						yp1 = y_r(800 + m_GaugeMask.at(0).GetYSize() * 3 / 10) + y_r(150.f* UltPer);
						rad = 0.f;
					}
					m_GaugeMask.at(0).GetGraph().DrawRotaGraph(xp1, yp1, 0.6f *(float)y_r(100) / 100.f, rad, true);
					if (intParam[7] == 1) {
						SetDrawBlendMode(DX_BLENDMODE_ADD, std::clamp((int)(255.f*sin(deg2rad(ULTTimer))), 0, 255));
						ULT_Graph.DrawRotaGraph(xp1, yp1, 0.6f *(float)y_r(100) / 100.f, rad, true);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						ULTTimer += 15.f * 60.f / FPS;
					}
				}
				//ゲージ
				{
					m_GaugeMask.at(1).SetDraw([&]() {
						m_GaugeParam[4].DrawGaugeUp(
							-1, -1, 1 + m_GaugeMask.at(1).GetXSize(), 1 + m_GaugeMask.at(1).GetYSize(),
							GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColorU8(0, 0, 255, 255), GetColorU8(255, 0, 0, 255)
						);
											  });
					//
					int xp1, yp1;
					xp1 = y_r(1500 - 20 - m_GaugeMask.at(1).GetXSize() * 3 / 10);
					yp1 = y_r(960 + m_GaugeMask.at(1).GetYSize() * 3 / 10) + y_r(300.f* UltPer);
					if (intParam[7] == -1) {
						xp1 += y_r(280);
						yp1 -= y_r(40);
					}
					m_GaugeMask.at(1).GetGraph().DrawRotaGraph(xp1, yp1, 0.6f *(float)y_r(100) / 100.f, 0.f, true);

					if (UltPer > 0.f && (intParam[7] != -1)) {
						xp1 += y_r(150);
						yp1 = y_r(980);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*UltPer), 0, 255));
						//弾ストック表示
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																			  xp1, yp1 + y_r(80), White, Gray75,
																			  "sec");
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																			  xp1, yp1 + y_r(80), White, Gray75,
																			  "%04.2f",
																			  floatParam[4]
						);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
				}
				//ゲージ
				{
					int xp1, yp1;
					xp1 = y_r(1500);
					yp1 = y_r(970) + y_r(300.f* UltPer);
					if (intParam[7] == -1) {
						xp1 += y_r(280);
						yp1 -= y_r(50);
					}
					for (size_t i = 0; i < 3; i++) {
						auto& g = m_GaugeMask.at((size_t)(i + 2));
						g.SetDraw([&]() {
							m_GaugeParam[(size_t)(i + 5)].DrawGaugeUp(
								-1, -1, 1 + g.GetXSize(), 1 + g.GetYSize(),
								GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
								GetColorU8(0, 0, 255, 255), GetColorU8(255, 0, 0, 255)
							);
								 });
						//
						//Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1 - y_r(12), White, Gray75, "%d", m_GaugeParam[i + 5].GetGauge());

						g.GetGraph().DrawExtendGraph(xp1, yp1, xp1 + y_r(g.GetXSize() * 6 / 10), yp1 + y_r(g.GetYSize() * 6 / 10), true);
						xp1 += y_r(g.GetXSize() * 6 / 10 + 6);
					}

					if ((1.f - UltPer) > 0.f) {
						if (intParam[7] == -1) {
							xp1 = y_r(1760);
							yp1 = y_r(980);
						}
						else {
							xp1 = y_r(1500);
							yp1 = y_r(990);
						}
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*(1.f - UltPer)), 0, 255));
						//弾ストック表示
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																			  xp1, yp1 + y_r(80), White, Gray75,
																			  "AmmoStock");
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																			  xp1 + y_r(110), yp1 + y_r(80), White, Gray75,
																			  "%03d", intParam[3]);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
				}
			}

			void			InitGaugeParam(int ID, int value, int Max) { m_GaugeParam[ID].InitGaugeParam(value, Max); }
			void			SetGaugeParam(int ID, int value, int Max) { m_GaugeParam[ID].SetGaugeParam(value, Max); }

			void			SetIntParam(int ID, int value) { intParam[ID] = value; }
			void			SetfloatParam(int ID, float value) { floatParam[ID] = value; }
			void			SetStrParam(int ID, std::string_view value) { strParam[ID] = value; }
		};
	};
};
