#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include	"../MainScene/BackGround/BackGround.hpp"
#include	"../MainScene/Object/Character.hpp"
#include	"../MainScene/Player/Player.hpp"

#include	"../MainScene/NetWork.hpp"

#include	"../MainScene/UI/MainSceneUIControl.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"
#include	"../MainScene/MainSceneSub.hpp"

#include	"../MainScene/EventScene/EventScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class Tutorial {
			class TutorialLogData {
				char m_Message[256]{};
				float m_Time{ -1.f };
				float m_Flip{ 0.f };
				float m_Flip_Y{ 0.f };
			public:
				void AddFlip(float value) noexcept { m_Flip += value; }
				template <typename... Args>
				void SetData(const char* Mes) noexcept {
					snprintfDx(m_Message, 256, Mes);
					m_Time = 10.f;
					m_Flip = 0.f;
					m_Flip_Y = -1.f;
				}
				void UpdateActive() noexcept {
					if (m_Time > 5.f) {
						auto* DXLib_refParts = DXLib_ref::Instance();
						m_Time -= 1.f / DXLib_refParts->GetFps();
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
				const float ActivePer() const noexcept { return (m_Time > 0.2f) ? std::clamp((10.f - m_Time) / 0.2f + 0.1f, 0.f, 1.f) : std::clamp(m_Time / 0.2f, 0.f, 1.f); }
				const char* GetMsg() const noexcept { return m_Message; }
			};
			struct Msgs {
				int			m_GraphID{ 0 };
				std::string m_Mes;
				HitType		m_CheckHit{ HitType::None };
			};
		private:
			std::vector<Msgs>								m_Tutorial{};
			std::array<TutorialLogData, 64>					m_data;
			int												LastSel{ 0 };
			int												Limit{ 0 };
			SoundHandle										m_TutorialVoice;
			int												m_TutorialNow{ 0 };
			std::vector<GraphHandle>						m_Teacher;
			int												m_GraphID{ 0 };
			HitType											m_CheckHit{ HitType::None };
			bool											m_Hit{ false };
			GraphHandle										m_Men;
			GraphHandle										m_Kote;
			GraphHandle										m_Dou;
		public:
			bool CheckHitOk(HitType hitPos) {
				if (m_CheckHit == hitPos) {
					m_Hit = true;
					return true;
				}
				return false;
			}

			bool IsEndTutorial() { return (m_TutorialNow >= m_Tutorial.size()); }
		public:
			void Load(void) noexcept {
				GraphHandle::LoadDiv("data/UI/Teacher.png", 4, 1, 4, 400, 1824 / 4, &m_Teacher);
				m_Men.Load("data/UI/Men.mp4");
				m_Kote.Load("data/UI/Kote.mp4");
				m_Dou.Load("data/UI/Dou.mp4");
			}
			void Set(void) noexcept {
				auto* OptionParts = OPTION::Instance();
				std::string Path = "data/Tutorial_";
				Path += LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)];
				Path += ".txt";

				Limit = 0;
				LastSel = 0;
				m_CheckHit = HitType::None;
				this->m_Tutorial.clear();
				{
					int mdata = FileRead_open(Path.c_str(), FALSE);
					while (true) {
						if (FileRead_eof(mdata) != 0) { break; }
						auto ALL = getparams::Getstr(mdata);
						//コメントアウト
						if (ALL.find("//") != std::string::npos) {
							ALL = ALL.substr(0, ALL.find("//"));
						}
						//
						if (ALL == "") { continue; }
						if (ALL.find('=') != std::string::npos) {
							auto LEFT = getparams::getleft(ALL);
							auto RIGHT = getparams::getright(ALL);
							if (LEFT == "Face") {
								m_Tutorial.resize(m_Tutorial.size() + 1);
								m_Tutorial.back().m_GraphID = std::stoi(RIGHT);
							}
							else if (LEFT == "Voice") {
								m_Tutorial.back().m_Mes = RIGHT + "\n";
							}
							else if (LEFT == "Wait") {
								if (RIGHT == "GetMen") {
									m_Tutorial.back().m_CheckHit = HitType::Head;
								}
								else if (RIGHT == "GetKote") {
									m_Tutorial.back().m_CheckHit = HitType::Arm;
								}
								else if (RIGHT == "GetDou") {
									m_Tutorial.back().m_CheckHit = HitType::Body;
								}
								else if (RIGHT == "GetGuard") {
									//m_Tutorial.back().m_CheckHit = HitType::Head;
								}
							}
						}
						else {
							m_Tutorial.back().m_Mes += ALL + "\n";
						}
					}
					FileRead_close(mdata);
				}

				m_TutorialNow = 0;
			}
			void Update(void) noexcept {
				auto* Pad = PadControl::Instance();
				auto* OptionParts = OPTION::Instance();
				if (Pad->GetPadsInfo(PADS::INTERACT).GetKey().trigger()) {
					if (m_CheckHit == HitType::None) {
						m_Hit = true;
					}
					else {
						//ログ送り
						if (Limit > 0) {
							for (auto& d : m_data) {
								d.AddFlip(1.f);
							}
							Limit--;
						}
					}
				}
				if (m_Hit) {
					m_Hit = false;
					m_CheckHit = HitType::None;
					if (m_TutorialNow < m_Tutorial.size()) {
						auto& Data = m_Tutorial.at(m_TutorialNow);
						//末尾にログを追加
						for (int i = 0; i <= Limit; i++) {
							for (auto& d : m_data) {
								d.AddFlip(1.f);
							}
						}
						Limit = 0;
						m_data.at(LastSel).SetData(Data.m_Mes.c_str());
						++LastSel %= static_cast<int>(m_data.size());
						//サウンド
						SetCreate3DSoundFlag(FALSE);
						m_TutorialVoice.Stop();
						m_TutorialVoice.Dispose();
						char Mes[256]{};
						snprintfDx(Mes, 256, "data/Sound/VOICE/Tutorial/%d.wav", m_TutorialNow);
						m_TutorialVoice.Load(Mes);
						m_TutorialVoice.Play(DX_PLAYTYPE_BACK, TRUE);
						m_TutorialVoice.SetVol(static_cast<int>(128 * OptionParts->GetParamFloat(EnumSaveParam::SE)));
						//画像
						m_GraphID = Data.m_GraphID;
						//
						m_CheckHit = Data.m_CheckHit;
						switch (m_CheckHit) {
						case HitType::Head:
							ChangeMovieVolumeToGraph(0, m_Men.get());
							PlayMovieToGraph(m_Men.get());
							break;
						case HitType::Arm:
							ChangeMovieVolumeToGraph(0, m_Kote.get());
							PlayMovieToGraph(m_Kote.get());
							break;
						case HitType::Body:
							ChangeMovieVolumeToGraph(0, m_Dou.get());
							PlayMovieToGraph(m_Dou.get());
							break;
						default:
							break;
						}
						//ID
						m_TutorialNow++;
					}
					else {
						//ログ送り
						if (Limit > 0) {
							for (auto& d : m_data) {
								d.AddFlip(1.f);
							}
							Limit--;
						}
					}
				}
				if (Pad->GetPadsInfo(PADS::RELOAD).GetKey().trigger()) {
					//ログ巻き戻し
					int count = 0;
					for (auto& d : m_data) {
						if (d.ActivePer() > 0.f) {
							count++;
						}
					}

					if (Limit < count - 1) {
						for (auto& d : m_data) {
							d.AddFlip(-1.f);
						}
						Limit++;
					}
				}
				for (auto& d : m_data) {
					if (d.ActivePer() > 0.f) {
						d.UpdateActive();
					}
				}
			}
			void Dispose(void) noexcept {
				for (auto& d : m_data) {
					d.Reset();
				}
				this->m_Tutorial.clear();
			}
			void Dispose_Load(void) noexcept {
				m_Teacher.clear();
				m_Men.Dispose();
				m_Kote.Dispose();
				m_Dou.Dispose();
			}
			void Draw(void) const noexcept {
				auto* WindowParts = WindowSystem::DrawControl::Instance();
				auto* LocalizeParts = LocalizePool::Instance();

				int xp1, yp1;
				xp1 = (480 + 400 / 2 + 48);
				yp1 = (1080 - 64 - 64);

				for (auto& d : m_data) {
					if (d.ActivePer() > 0.f) {
						int yp = yp1 - (static_cast<int>((64 + 5) * d.GetFlip()));

						WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal,
							std::min(
								std::clamp(static_cast<int>(255.f * d.ActivePer()), 0, 255),
								255 - std::clamp(std::abs(yp - yp1) * 2 * 255 / (255), 0, 255)
							));
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
							xp1, yp, Green, Black, d.GetMsg());
					}
				}
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				if (m_TutorialNow != 0) {
					WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Teacher.at(m_GraphID),
						(480), (1080 - 1824 / 4 / 2), (480 + 400 / 2), (1080), true);
				}
				{
					int xp, yp;
					xp = (32);
					yp = (384);
					switch (m_CheckHit) {
					case HitType::Head:
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
							xp, yp, Green, Black, LocalizeParts->Get(8990));
						break;
					case HitType::Arm:
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
							xp, yp, Green, Black, LocalizeParts->Get(8991));
						break;
					case HitType::Body:
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
							xp, yp, Green, Black, LocalizeParts->Get(8992));
						break;
					default:
						break;
					}
					if (m_TutorialNow == 0) {
						auto* KeyGuideParts = KeyGuide::Instance();
						auto* Pad = PadControl::Instance();
						KeyGuideParts->DrawButton(xp, yp - 24, KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()));
						xp += KeyGuideParts->GetDrawSize(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType())) + 3;
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, (24),
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
							xp, yp, Green, Black, LocalizeParts->Get(8993));

					}
				}
				{
					switch (m_CheckHit) {
					case HitType::Head:
						if (GetMovieStateToGraph(m_Men.get()) == 0) {
							SeekMovieToGraph(m_Men.get(), 0);
							PlayMovieToGraph(m_Men.get());
						}
						WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Men,
							(64), (600 - 720 / 2 / 2),
							(64 + 600 / 2), (600 + 720 / 2 / 2), false);
						break;
					case HitType::Arm:
						if (GetMovieStateToGraph(m_Kote.get()) == 0) {
							SeekMovieToGraph(m_Kote.get(), 0);
							PlayMovieToGraph(m_Kote.get());
						}
						WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Kote,
							(64), (600 - 720 / 2 / 2),
							(64 + 600 / 2), (600 + 720 / 2 / 2), false);
						break;
					case HitType::Body:
						if (GetMovieStateToGraph(m_Dou.get()) == 0) {
							SeekMovieToGraph(m_Dou.get(), 0);
							PlayMovieToGraph(m_Dou.get());
						}
						WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_Dou,
							(64), (600 - 720 / 2 / 2),
							(64 + 600 / 2), (600 + 720 / 2 / 2), false);
						break;
					default:
						break;
					}
				}
			}
		};


		class MainGameScene : public TEMPSCENE ,
			public FadeControl
		{
		private:
			Tutorial									m_Tutorial;
			//UI関連
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			UIClass										m_UIclass;
			PauseMenuControl							m_PauseMenuControl;
			//NetWork
			std::unique_ptr<NetWork::NetWorkController>	m_NetWorkController{ nullptr };
			std::vector<DamageEvent>					m_DamageEvents;
			//
			bool										m_IsEnd{ false };//シーン全体の終了フラグ
			bool										m_isTraining{ false };
#ifdef DEBUG
			float m_D1{ 38.f / 255.f }, m_D2{ 192.f / 255.f }, m_D3{ 1.f };
#endif
			EventScene		m_EventScene;
			bool			m_IsEventSceneActive{ false };
			bool			m_IsEventSceneFlag{ false };
			std::string		m_EventSelect;

			GraphHandle		m_GameStart;
			GraphHandle		m_GameRestart;
			GraphHandle*	m_pStart{ nullptr };
			GraphHandle		m_Once;
			GraphHandle		m_GameEnd;
			GraphHandle		m_Result;

			float m_GameStartTimer{ 2.f };
			float m_GameStartAlpha{ 0.f };
			float m_GameStartScale{ 0.f };

			bool m_IsGameStart{ false };
			bool m_IsGameEnd{ false };
			float m_Timer{ 0.f };
			bool m_IsPlayable{ false };

			float m_WinOnceTimer{ 2.f };
			float m_WinOnceAlpha{ 0.f };
			float m_WinOnceScale{ 0.f };

			float m_GameEndTimer{ 2.f };
			float m_GameEndAlpha{ 0.f };
			float m_GameEndScale{ 0.f };

			bool m_IsDrawOneMinute{ false };
			bool m_IsTimeUp{ false };
			bool m_IsWinSound{ false };

			bool m_IsResult{ false };
			int m_TimerSE{ 0 };

			float m_DivideTimer{ 5.f };

			float m_TutorialTimer{ 0.f };

			float m_TutorialResetTimer{ 0.f };

			float							m_ScoreUp0{ 0.f };
			float							m_ScoreUp1{ 0.f };
		private:
			auto		GetMyPlayerID(void) const noexcept {
				if (m_NetWorkController) {
					return m_NetWorkController->GetMyPlayerID();
				}
				return (PlayerID)0;
			}
		public:
			void		SetIsTraining(bool value)noexcept { m_isTraining = value; }
		public:
			MainGameScene(void) noexcept {}
			MainGameScene(const MainGameScene&) = delete;
			MainGameScene(MainGameScene&& o) = delete;
			MainGameScene& operator=(const MainGameScene&) = delete;
			MainGameScene& operator=(MainGameScene&& o) = delete;

			virtual ~MainGameScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) const noexcept override;
			void			ShadowDraw_Far_Sub(void) const noexcept override;
			void			ShadowDraw_Sub(void) const noexcept override;
			void			CubeMap_Sub(void) const noexcept override;

			void			SetShadowDraw_Sub(void) const noexcept override;

			void			MainDraw_Sub(void) const noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept override;
			void			DrawUI_In_Sub(void) const noexcept override;
		};
	}
}
