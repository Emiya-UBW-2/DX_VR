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
					auto* DXLib_refParts = DXLib_ref::Instance();
					this->m_Now = value;
					this->m_Max = Max;
					if (Rate > 0) {
						float Limit = static_cast<float>(this->m_Max / Rate);
						this->m_Buffer += std::clamp(static_cast<float>(static_cast<float>(this->m_Now) - this->m_Buffer) * Limit, -Limit * 1.5f, Limit * 1.5f) * DXLib_refParts->GetDeltaTime();
					}
					else {
						this->m_Buffer = static_cast<float>(this->m_Now);
					}
				}
				void			DrawGauge(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) const noexcept {
					//return;
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int Edge = 0;
					int length = (xp2 - Edge) - (xp1 + Edge);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + Edge,
						xp2 - Edge, yp2 - Edge,
						Black, TRUE);
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp1 + Edge,
						xp1 + Edge + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - Edge,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge + length * std::max(this->m_Now, ParamBuf) / this->m_Max, yp1 + Edge,
						xp1 + Edge + length * std::min(this->m_Now, ParamBuf) / this->m_Max, yp2 - Edge,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeUp(int xp1, int yp1, int xp2, int yp2,
					COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub) noexcept {
					//return;
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					int ParamBuf = static_cast<int>(this->m_Buffer + 0.5f);
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, White, FALSE);
					int Edge = 0;
					int length = (yp2 - Edge) - (yp1 + Edge);

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (ParamBuf > this->m_Now) ? ColorSub : ColorAdd;

					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp1 + Edge,
						xp2 - Edge, yp2 - Edge - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						Black, TRUE);
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp2 - Edge - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + Edge, yp2 - Edge,
						GetColor(Color.r, Color.g, Color.b), TRUE);
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + Edge, yp2 - Edge - length * std::max(this->m_Now, ParamBuf) / this->m_Max,
						xp2 + Edge, yp2 - Edge - length * std::min(this->m_Now, ParamBuf) / this->m_Max,
						GetColor(ColorAddSub.r, ColorAddSub.g, ColorAddSub.b), TRUE);
				}
				void			DrawGaugeCircleLeft(int xp1, int yp1,
					COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
					const GraphHandle* CircleObj, float deg, int Add) noexcept {
					//return;
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (this->m_Buffer > this->m_Now) ? ColorSub : ColorAdd;
					float per = std::clamp(static_cast<float>(this->m_Now) / this->m_Max, 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / this->m_Max, 0.f, 1.f);
					DrawCtrls->SetAdd(WindowSystem::DrawLayer::Normal, Add);
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, ColorBase.r, ColorBase.g, ColorBase.b);
					DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 + (256), yp1,
						50.0f + ((50.0f - 15.0f * 2.0f) * 1.0f) + 15.0f + deg,
						50.0f + 15.0f + deg,
						(static_cast<float>((1080)) / 1080.0f));
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);
					DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 + (256), yp1,
						50.0f + ((50.0f - 15.0f * 2.0f) * std::max(per, perbuf)) + 15.0f + deg,
						50.0f + ((50.0f - 15.0f * 2.0f) * std::min(per, perbuf)) + 15.0f + deg,
						(static_cast<float>((1080)) / 1080.0f));
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, Color.r, Color.g, Color.b);
					DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 + (256), yp1,
						50.0f + ((50.0f - 15.0f * 2.0f) * std::min(per, perbuf)) + 15.0f + deg,
						50.0f + 15.0f + deg,
						(static_cast<float>((1080)) / 1080.0f));
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				}
				void			DrawGaugeCircleRight(int xp1, int yp1,
					COLOR_U8 ColorBase, COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, COLOR_U8 ColorFull, COLOR_U8 ColorAdd, COLOR_U8 ColorSub,
					const GraphHandle* CircleObj, float deg, int Add) noexcept {
					//return;
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
					COLOR_U8 ColorAddSub = (this->m_Buffer > this->m_Now) ? ColorSub : ColorAdd;
					float per = std::clamp(static_cast<float>(this->m_Now) / this->m_Max, 0.f, 1.f);
					float perbuf = std::clamp(this->m_Buffer / this->m_Max, 0.f, 1.f);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					DrawCtrls->SetAdd(WindowSystem::DrawLayer::Normal, Add);
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, ColorBase.r, ColorBase.g, ColorBase.b);

					DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 - (256), yp1,
						(50.0f - 15.0f * 2.0f) + 15.0f + deg,
						((50.0f - 15.0f * 2.0f) * (1.0f - 1.0f)) + 15.0f + deg,
						(static_cast<float>((1080)) / 1080.0f));

					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, ColorAddSub.r, ColorAddSub.g, ColorAddSub.b);

					DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 - (256), yp1,
						((50.0f - 15.0f * 2.0f) * (1.0f - std::min(per, perbuf))) + 15.0f + deg,
						((50.0f - 15.0f * 2.0f) * (1.0f - std::max(per, perbuf))) + 15.0f + deg,
						(static_cast<float>((1080)) / 1080.0f));

					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, Color.r, Color.g, Color.b);
					DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, CircleObj,
						xp1 - (256), yp1,
						(50.0f - 15.0f * 2.0f) + 15.0f + deg,
						((50.0f - 15.0f * 2.0f) * (1.0f - std::min(per, perbuf))) + 15.0f + deg,
						(static_cast<float>((1080)) / 1080.0f));

					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
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

					COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, ColorFull, static_cast<float>(this->m_Now) / static_cast<float>(this->m_Max));
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
					ans = static_cast<int>(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * static_cast<float>(pInt3));
					ans += static_cast<int>(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * static_cast<float>(pInt2));
					ans += static_cast<int>(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * static_cast<float>(pInt1));
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
		private:
			std::array<GaugeParam, 3>		m_GaugeParam;
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;

			GraphHandle						m_HeartIcon;
			GraphHandle						m_Kiai;
			GraphHandle						m_Mine;

			GraphHandle		m_GameStart;
			GraphHandle		m_GameRestart;
			GraphHandle*	m_pStart{ nullptr };
			GraphHandle		m_Once;
			GraphHandle		m_GameEnd;

			float m_GameStartTimer{ 2.f };
			float m_GameStartAlpha{ 0.f };
			float m_GameStartScale{ 0.f };

			float m_WinOnceTimer{ 2.f };
			float m_WinOnceAlpha{ 0.f };
			float m_WinOnceScale{ 0.f };

			float m_GameEndTimer{ 2.f };
			float m_GameEndAlpha{ 0.f };
			float m_GameEndScale{ 0.f };
		public:
			UIClass(void) noexcept {}
			UIClass(const UIClass&) = delete;
			UIClass(UIClass&& o) = delete;
			UIClass& operator=(const UIClass&) = delete;
			UIClass& operator=(UIClass&& o) = delete;
			~UIClass(void) noexcept {}
		public:
			bool			IsGameStartEnd() const noexcept { return m_GameStartTimer <= 0.f; }
			void			SetGameStart(bool isFirstRound) noexcept {
				if (isFirstRound) {
					m_pStart = &m_GameStart;
				}
				else {
					m_pStart = &m_GameRestart;
				}
				m_GameStartTimer = 2.f;
			}

			bool			IsWinOnceEnd() const noexcept { return m_WinOnceTimer <= 0.f; }
			void			SetWinOnceStart() noexcept { m_WinOnceTimer = 1.5f; }

			bool			IsGameEndEnd() const noexcept { return m_GameEndTimer <= 0.f; }
			void			SetTimeUpStart() noexcept { m_GameEndTimer = 2.5f; }
			void			SetGameEndStart() noexcept { m_GameEndTimer = std::max(m_GameEndTimer, 1.5f); }
		public:
			void			Load(void) noexcept {
				auto* HitMarkParts = HitMark::Instance();

				m_HeartIcon.Load("data/UI/Heart.png");
				m_Kiai.Load("data/UI/Kiai.png");
				m_Mine.Load("data/UI/Mine.png");
				m_GameStart.Load("data/UI/GameStart.png");
				m_GameRestart.Load("data/UI/GameReStart.png");
				m_Once.Load("data/UI/Once.png");
				m_GameEnd.Load("data/UI/GameEnd.png");
				HitMarkParts->Load();
			}
			void			Dispose_Load(void) noexcept {
				auto* HitMarkParts = HitMark::Instance();
				m_HeartIcon.Dispose();
				m_Kiai.Dispose();
				m_Mine.Dispose();
				m_GameStart.Dispose();
				m_GameRestart.Dispose();
				m_Once.Dispose();
				m_GameEnd.Dispose();

				HitMarkParts->Dispose();
			}
			void			Set(void) noexcept {
				auto* HitMarkParts = HitMark::Instance();

				m_GameStartAlpha = 0.f;
				m_GameStartScale = 0.f;
				m_GameStartTimer = 2.f;

				m_WinOnceAlpha = 0.f;
				m_WinOnceScale = 0.f;
				m_WinOnceTimer = -1.f;

				m_GameEndAlpha = 0.f;
				m_GameEndScale = 0.f;
				m_GameEndTimer = -1.f;
				//
				HitMarkParts->Set();
			}
			void			Update(void) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				{
					if (m_GameStartTimer > 1.f) {
						float Per = std::clamp((1.4f - m_GameStartTimer) / 0.2f, 0.f, 1.f);//0to1
						m_GameStartAlpha = Lerp(0.f, 1.f, Per);
						m_GameStartScale = Lerp(0.f, 1.0f, Per);
					}
					else if (m_GameStartTimer > 0.f) {
						float Per = std::clamp((1.f - m_GameStartTimer), 0.f, 1.f);//0to1
						m_GameStartAlpha = Lerp(1.f, 1.f, Per);
						m_GameStartScale = Lerp(1.0f, 1.1f, Per);
					}
					else if (m_GameStartTimer > -0.2f) {
						float Per = std::clamp((-m_GameStartTimer) / 0.2f, 0.f, 1.f);//0to1
						m_GameStartAlpha = Lerp(1.f, 0.f, Per);
						m_GameStartScale = Lerp(1.1f, 5.f, Per);
					}
					else {
						m_GameStartAlpha = 0.f;
						m_GameStartScale = 0.f;
					}
					m_GameStartTimer = std::max(m_GameStartTimer - DXLib_refParts->GetDeltaTime(), -1.f);
				}
				{
					if (m_WinOnceTimer > 1.f) {
						float Per = std::clamp((1.4f - m_WinOnceTimer) / 0.2f, 0.f, 1.f);//0to1
						m_WinOnceAlpha = Lerp(0.f, 1.f, Per);
						m_WinOnceScale = Lerp(0.f, 1.0f, Per);
					}
					else if (m_WinOnceTimer > 0.f) {
						float Per = std::clamp((1.f - m_WinOnceTimer), 0.f, 1.f);//0to1
						m_WinOnceAlpha = Lerp(1.f, 1.f, Per);
						m_WinOnceScale = Lerp(1.0f, 1.1f, Per);
					}
					else if (m_WinOnceTimer > -0.2f) {
						float Per = std::clamp((-m_WinOnceTimer) / 0.2f, 0.f, 1.f);//0to1
						m_WinOnceAlpha = Lerp(1.f, 0.f, Per);
						m_WinOnceScale = Lerp(1.1f, 5.f, Per);
					}
					else {
						m_WinOnceAlpha = 0.f;
						m_WinOnceScale = 0.f;
					}
					m_WinOnceTimer = std::max(m_WinOnceTimer - DXLib_refParts->GetDeltaTime(), -1.f);
				}
				{
					if (m_GameEndTimer > 1.f) {
						float Per = std::clamp((1.4f - m_GameEndTimer) / 0.2f, 0.f, 1.f);//0to1
						m_GameEndAlpha = Lerp(0.f, 1.f, Per);
						m_GameEndScale = Lerp(0.f, 1.0f, Per);
					}
					else if (m_GameEndTimer > 0.f) {
						float Per = std::clamp((1.f - m_GameEndTimer), 0.f, 1.f);//0to1
						m_GameEndAlpha = Lerp(1.f, 1.f, Per);
						m_GameEndScale = Lerp(1.0f, 1.1f, Per);
					}
					else if (m_GameEndTimer > -0.2f) {
						float Per = std::clamp((-m_GameEndTimer) / 0.2f, 0.f, 1.f);//0to1
						m_GameEndAlpha = Lerp(1.f, 0.f, Per);
						m_GameEndScale = Lerp(1.1f, 5.f, Per);
					}
					else {
						m_GameEndAlpha = 0.f;
						m_GameEndScale = 0.f;
					}
					m_GameEndTimer = std::max(m_GameEndTimer - DXLib_refParts->GetDeltaTime(), -1.f);
				}
			}
			void			Draw(void) const noexcept {
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				auto* HitMarkParts = HitMark::Instance();
				//
				HitMarkParts->Draw();
				//気合
				{
					float radius = Lerp(static_cast<float>(1.f), static_cast<float>(1.01f), floatParam[2]);
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
						255, 150, 155);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255 * (m_GaugeParam[0].GetGaugeMax() - m_GaugeParam[0].GetGaugeBuff()) / m_GaugeParam[0].GetGaugeMax());
					DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Kiai,
						BaseScreenWidth / 2 - static_cast<int>(static_cast<float>(BaseScreenWidth / 2) * radius),
						BaseScreenHeight / 2 - static_cast<int>(static_cast<float>(BaseScreenHeight / 2) * radius),
						BaseScreenWidth / 2 + static_cast<int>(static_cast<float>(BaseScreenWidth / 2) * radius),
						BaseScreenHeight / 2 + static_cast<int>(static_cast<float>(BaseScreenHeight / 2) * radius),
						true);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
						255, 255, 255);
				}
				int xp1, yp1;
				//タイム
				{
					xp1 = (30);
					yp1 = (static_cast<int>(Lerp(10.0f, -50.f, std::clamp(((intParam[3]) ? 2.f : std::max(m_GameStartTimer, 0.f)) - 0.5f, 0.f, 1.f))));
					float per = std::cos(DX_PI_F * 10.f * ((intParam[3]) ? 2.f : std::max(m_GameStartTimer, 0.f)));
					if ((per * 255.f) > 1.f) {
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, (floatParam[0] < 60.f) ? Red : Yellow, Black, "TIME");
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (300), yp1, (floatParam[0] < 60.f) ? Red : Yellow, Black, "%d:%05.2f",
							static_cast<int>(floatParam[0] / 60.f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}
				}
				//スコア
				{
					xp1 = (1920 / 2);
					yp1 = (static_cast<int>(Lerp(28.0f, -100.f, std::clamp(((intParam[3]) ? 2.f : std::max(m_GameStartTimer, 0.f)) - 0.5f, 0.f, 1.f))));

					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
						92, 0, 0);
					DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Mine,
						xp1 - (10) - (40) - (36) + (1),
						yp1 - (10) + (1),
						xp1 - (10) - (40) + (1),
						yp1 - (10) + (36) + (1),
						true);
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
						192, 0, 0);
					DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Mine,
						xp1 - (10) - (40) - (36),
						yp1 - (10),
						xp1 - (10) - (40),
						yp1 - (10) + (36),
						true);
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
						255, 255, 255);


					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 - (10) - (40), yp1 + (20),
						xp1 + (10) + (40), yp1 + (24), Black, true);

					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 - (10) - (40 - 20 * intParam[1]), yp1 + (20),
						xp1 - (10), yp1 + (24), Red, true);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::MIDDLE, xp1 - (20), yp1, Red, Black, "%d", intParam[0]);

					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 - (10), yp1 + (20),
						xp1 + (10), yp1 + (24), Gray75, true);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, xp1, yp1, Gray25, Black, ":");

					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
						xp1 + (10), yp1 + (20),
						xp1 + (10) + (40 - 20 * intParam[0]), yp1 + (24), White, true);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp1 + (20), yp1, White, Black, "%d", intParam[1]);
				}
				//情報
				{
					//心拍数
					xp1 = ((24 + 300 + 24 + 32));
					yp1 = (1080 - 80 - 28 * 1 + 6 / 2);
					float per = Lerp(0.4f, 0.6f, floatParam[2]);
					if ((per * 255.f) > 1.f) {
						int radius = static_cast<int>(Lerp(static_cast<float>((24)), static_cast<float>((32)), floatParam[2]));
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
							255, 0, 0);
						DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
							&m_HeartIcon, xp1 - radius, yp1 - radius, xp1 + radius, yp1 + radius, true);
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal,
							255, 255, 255);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, xp1, yp1, GetColor(255, 150, 150), Black, "%d", intParam[2]);

					//気合
					xp1 = (24 + 9 * 2);
					yp1 = (1080 - 80 - 28 * 2);

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1, yp1 - (2), White, Black, LocalizeParts->Get(5000));
					m_GaugeParam[0].DrawGauge(
						xp1, yp1, xp1 + (300), yp1 + (6),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(128, 255, 0, 255), GetColorU8(192, 255, 192, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);
					int KihakuPoints = static_cast<int>(Lerp(-30.f, 20.f, 1.f - static_cast<float>(m_GaugeParam[0].GetGauge()) / m_GaugeParam[0].GetGaugeMax()));
					if (KihakuPoints > 0) {
						DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
							xp1, yp1, xp1 + (300), yp1 + (6), White, false);
					}

					//スタミナ
					xp1 = (24 + 9 * 1);
					yp1 = (1080 - 80 - 28 * 1);

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1, yp1 - (2), White, Black, LocalizeParts->Get(5001));
					m_GaugeParam[1].DrawGauge(
						xp1, yp1, xp1 + (300), yp1 + (6),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(128, 255, 0, 255), GetColorU8(192, 255, 192, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);

					//ガード
					xp1 = (24 + 9 * 0);
					yp1 = (1080 - 80 - 28 * 0);

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1, yp1 - (2), White, Black, LocalizeParts->Get(5002));
					m_GaugeParam[2].DrawGauge(
						xp1, yp1, xp1 + (300), yp1 + (6),
						GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(128, 255, 0, 255), GetColorU8(192, 255, 192, 255),
						GetColorU8(0, 0, 128, 255), GetColorU8(128, 0, 0, 255)
					);
				}
				//
				{
					xp1 = (1920 / 2);
					yp1 = (240);

					if ((m_GameStartAlpha * 255.f) > 1.f) {
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_GameStartAlpha), 0, 255));
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 0);
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
							m_pStart, xp1, yp1, m_GameStartScale, 0.f, true);
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}

					if ((m_WinOnceAlpha * 255.f) > 1.f) {
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_WinOnceAlpha), 0, 255));
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
							&m_Once, xp1, yp1, m_WinOnceScale, 0.f, true);
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}

					if ((m_GameEndAlpha * 255.f) > 1.f) {
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * m_GameEndAlpha), 0, 255));
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
							&m_GameEnd, xp1, yp1, m_GameEndScale, 0.f, true);
						DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}
					if (0 <= floatParam[1] && floatParam[1] < 3.f) {
						if (static_cast<int>(floatParam[1] * 100) % 30 < 15) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, (64), (900),
								Yellow, Black,
								"場外まであと%3.1f秒", floatParam[1]);
						}
					}
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
