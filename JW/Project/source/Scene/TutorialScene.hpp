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
					m_Time -= 1.f / FPS;
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
			const float GetFlip() { return m_Flip_Y; }
			const float ActivePer() { return (m_Time > 0.2f) ? std::clamp((10.f - m_Time)/ 0.2f + 0.1f, 0.f, 1.f) : std::clamp(m_Time / 0.2f, 0.f, 1.f); }
			const char* GetMsg() { return m_Message; }
		};
		class TutorialLog {
			std::array<TutorialLogData, 29> data;
			int LastSel{0};
			GraphHandle						m_tutorialGraph;
			int xs{0}, ys{0};

			int Limit{0};
		public:
			template <typename... Args>
			void AddLog(const char* Mes, Args&&... args) noexcept {
				for (int i = 0;i <= Limit;i++) {
					for (auto& d : data) {
						d.AddFlip(1.f);
					}
				}
				Limit = 0;
				data.at(LastSel).SetData(Mes, args...);
				++LastSel %= ((int)data.size());
			}
			void AddLog() noexcept {
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
				auto* Fonts = FontPool::Instance();
				auto Gray = GetColor(64, 64, 64);
				auto Black = GetColor(0, 0, 0);

				int xp1, yp1;
				xp1 = y_r(64);
				yp1 = y_r(512);

				for (auto& d : data) {
					if (d.ActivePer() > 0.f) {
						int xp = xp1 - y_r((xs + 128) * (1.f - d.ActivePer()));
						int yp = yp1 - y_r((ys + 5) * d.GetFlip());

						SetDrawBlendMode(DX_BLENDMODE_ALPHA,
										 std::min(
											 std::clamp((int)(255.f*d.ActivePer()), 0, 255),
											 std::min(
												 std::clamp((y_r(960) - yp) * 255 / y_r(255), 0, 255),
												 std::clamp(yp * 255 / y_r(255), 0, 255)
											 )
										 )
						
						);
						this->m_tutorialGraph.DrawExtendGraph(
							xp, yp,
							xp + y_r(xs), yp + y_r(ys),
							true);
						Fonts->Get(FontPool::FontType::Gothic_AA, y_r(24)).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																			  xp + y_r(69), yp + y_r(14), Gray, Black, d.GetMsg());
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
			bool											m_PrevSSAO{true};
			bool											m_IsFirstLoad{true};			//共通リソースをロードしたか
			std::shared_ptr<BackGroundClassTutorial>		m_BackGround;					//BG
			MyPlayerReticleControl							m_MyPlayerReticleControl;		//銃関連
			MainLoopPauseControl							m_MainLoopPauseControl;			//ポーズメニュー
			UIClass											m_UIclass;						//UI関連

			std::vector<std::string>						m_Tutorial{};
			TutorialLog										m_TutorialLog;
			SoundHandle										m_TutorialVoice;
			int												m_TutorialNow{0};

			std::vector<DamageEvent>						m_DamageEvents;				//ダメージ

			std::shared_ptr<GunClass>		m_GunPtr;				//ポインター別持ち
			std::shared_ptr<CharacterClass>	m_CharacterPtr;			//ポインター別持ち
		public:
			TutorialScene(void) noexcept {}
			~TutorialScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load(void) noexcept;
		private:
			void			LoadSE(void) noexcept;
			void			SetSE(void) noexcept;
			void			DisposeSE(void) noexcept;
		private:
			void			LoadChara(const std::string&FolderName) noexcept;
			void			LoadGun(const std::string&FolderName, bool IsPreset, int Sel) noexcept;
		};
	};
};
