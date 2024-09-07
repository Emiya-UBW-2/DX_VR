#pragma once
#include	"../Header.hpp"

#include "../MainScene/MainSceneSub.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/UI/MainSceneUIControl.hpp"

#include "../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TutorialLogData {
			char m_Message[256]{};
			float m_Time{-1.f};
			float m_Flip{0.f};
			float m_Flip_Y{0.f};
		public:
			void AddFlip(float value) noexcept { m_Flip += value; }
			template <typename... Args>
			void SetData(const char* Mes, Args&&... args) noexcept {
				snprintfDx(m_Message, 256, Mes, args...);
				m_Time = 10.f;
				m_Flip = 0.f;
				m_Flip_Y = -1.f;
			}
			void UpdateActive() noexcept {
				if (m_Time > 5.f) {
					auto* DrawParts = DXDraw::Instance();
					m_Time -= 1.f / DrawParts->GetFps();
				}
				else if (m_Time <= 0.f) {
					m_Time = -1.f;
				}
				Easing(&m_Flip_Y, m_Flip, 0.9f, EasingType::OutExpo);
			}
			void Reset() noexcept {
				m_Time = -1.f;
				m_Flip = 0.f;
				m_Flip_Y = 0.f;
			}
		public:
			const float GetFlip() const noexcept { return m_Flip_Y; }
			const float ActivePer() { return (m_Time > 0.2f) ? std::clamp((10.f - m_Time) / 0.2f + 0.1f, 0.f, 1.f) : std::clamp(m_Time / 0.2f, 0.f, 1.f); }
			const char* GetMsg() { return m_Message; }
		};
		class TutorialLog {
			std::array<TutorialLogData, 64> data;
			int LastSel{0};
			GraphHandle						m_tutorialGraph;
			int xs{0}, ys{0};

			int Limit{0};
		public:
			const auto GetOffset() const noexcept { return Limit; }
		public:
			template <typename... Args>
			void Add(const char* Mes, Args&&... args) noexcept {
				for (int i = 0;i <= Limit;i++) {
					for (auto& d : data) {
						d.AddFlip(1.f);
					}
				}
				Limit = 0;
				data.at(LastSel).SetData(Mes, args...);
				++LastSel %= ((int)data.size());
			}
			void Add() noexcept {
				if (Limit > 0) {
					for (auto& d : data) {
						d.AddFlip(1.f);
					}
					Limit--;
				}
			}
			void SubLog() noexcept {
				int count = 0;
				for (auto& d : data) {
					if (d.ActivePer() > 0.f) {
						count++;
					}
				}

				if (Limit < count - 1) {
					for (auto& d : data) {
						d.AddFlip(-1.f);
					}
					Limit++;
				}
			}
			void Set() noexcept {
				this->m_tutorialGraph = GraphHandle::Load("data/UI/tutorial.png");
				this->m_tutorialGraph.GetSize(&xs, &ys);
				xs = 128 * xs / ys;
				ys = 128;
				Limit = 0;
			}
			void Update() noexcept {
				for (auto& d : data) {
					if (d.ActivePer() > 0.f) {
						d.UpdateActive();
					}
				}
			}
			void Draw() noexcept {
				auto* DrawParts = DXDraw::Instance();

				int xp1, yp1;
				xp1 = DrawParts->GetUIY(64);
				yp1 = DrawParts->GetUIY(512);

				for (auto& d : data) {
					if (d.ActivePer() > 0.f) {
						int xp = xp1 - DrawParts->GetUIY((int)((xs + 128) * (1.f - d.ActivePer())));
						int yp = yp1 - DrawParts->GetUIY((int)((ys + 5) * d.GetFlip()));

						WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal,
										 std::min(
											 std::clamp((int)(255.f*d.ActivePer()), 0, 255),
											 std::min(
												 std::clamp((DrawParts->GetUIY(960) - yp) * 255 / DrawParts->GetUIY(255), 0, 255),
												 std::clamp(yp * 255 / DrawParts->GetUIY(255), 0, 255)
											 )
										 )

						);
						WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &this->m_tutorialGraph,
							xp, yp,
							xp + DrawParts->GetUIY(xs), yp + DrawParts->GetUIY(ys),
							true);
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																					  xp + DrawParts->GetUIY(69), yp + DrawParts->GetUIY(14), Gray75, Black, d.GetMsg());
					}
				}
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
			void Dispose() noexcept {
				this->m_tutorialGraph.Dispose();
				for (auto& d : data) {
					d.Reset();
				}
			}
		};


		class TutorialScene : public TEMPSCENE, public EffectControl, public GunsModify {
		private:
			float											m_FirstFade{0.f};
			bool											m_IsFirstGame{false};
			bool											m_PrevSSAO{true};
			std::shared_ptr<BackGroundClassTutorial>		m_BackGround;					//BG
			MainLoopPauseControl							m_MainLoopPauseControl;			//ポーズメニュー
			UIClass											m_UIclass;						//UI関連

			struct Msgs {
				std::string m_Mes;
				std::vector<PADS>m_PADS;
			};

			std::vector<Msgs>								m_Tutorial{};

			TutorialLog										m_TutorialLog;
			SoundHandle										m_TutorialVoice;
			int												m_TutorialNow{0};

			std::vector<DamageEvent>						m_DamageEvents;				//ダメージ

			int tgtSel = 0;
			float tgtTimer = 0.f;
			GraphHandle ScoreBoard;
			GraphHandle ScoreBoard2;
			bool											m_Sound{true};
		public:
			TutorialScene(void) noexcept {}
			~TutorialScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		private:
			void			LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept;
		};
	};
};
