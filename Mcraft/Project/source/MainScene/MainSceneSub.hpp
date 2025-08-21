#pragma once
#pragma warning(disable:4464)

#include	"../sub.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

#include	"../MainScene/Player/Player.hpp"
#include	"../MainScene/Object/MovieCharacter.hpp"

#include <random>

namespace FPS_n2 {
	namespace Sceneclass {
		class PauseMenuControl {
			bool			m_IsRetire{ false };
		public:
			PauseMenuControl(void) noexcept { Load(); }
			PauseMenuControl(const PauseMenuControl&) = delete;
			PauseMenuControl(PauseMenuControl&&) = delete;
			PauseMenuControl& operator=(const PauseMenuControl&) = delete;
			PauseMenuControl& operator=(PauseMenuControl&&) = delete;

			virtual ~PauseMenuControl(void) noexcept { Dispose(); }
		public:
			const auto& IsRetire(void) const noexcept { return this->m_IsRetire; }
		public:
			void			Load(void) noexcept;
			void			Init(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept;
		};

		class CommonBattleResource {
		public:
			static void			Load(void) noexcept;
			static void			Set(void) noexcept;
			static void			Dispose(void) noexcept;
		};

		class MainSceneUI {
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;

			bool CanLookTarget{ false };
			float LookPer{};
			float IsDrawAimUIPer{};
			float IsDownUIPer{};

			GraphHandle					m_BodyGraph;
			GraphHandle					m_ArmorGraph;
			GraphHandle					m_HeadGraph;

			GraphHandle					m_BodyCGraph;
			GraphHandle					m_ArmorCGraph;
			GraphHandle					m_HeadCGraph;

			GraphHandle					m_ItembackGraph;

			GraphHandle					OIL_Graph;

			GraphHandle					DeleteItemGraph;
			GraphHandle					ReturnGraph;
			GraphHandle					GaugeGraph;
			GraphHandle					RunGraph;
			GraphHandle					AdrenalineGraph;

			float						m_DamagePer{};
			float						m_DamagePerR{};
			int							m_ReHP{};

			bool						m_AmmoRand{};
			float						m_AmmoRandR{};
			float						m_AmmoInPer{};
			int							m_AmmoNumTotal{};

			int							m_RadPrev{};
			float						m_RadR{};

			float						m_Gram{};

			float						m_Score{};
		public:
			MainSceneUI(void) noexcept { Load(); }
			MainSceneUI(const MainSceneUI&) = delete;
			MainSceneUI(MainSceneUI&&) = delete;
			MainSceneUI& operator=(const MainSceneUI&) = delete;
			MainSceneUI& operator=(MainSceneUI&&) = delete;

			virtual ~MainSceneUI(void) noexcept { Dispose(); }
		public:
			void			Load(void) noexcept {}
			void			Set(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept {
				this->m_BodyGraph.Dispose();
				this->m_ArmorGraph.Dispose();
				this->m_HeadGraph.Dispose();
				this->m_BodyCGraph.Dispose();
				this->m_ArmorCGraph.Dispose();
				this->m_HeadCGraph.Dispose();
				this->m_ItembackGraph.Dispose();
				this->OIL_Graph.Dispose();
				this->DeleteItemGraph.Dispose();
				this->ReturnGraph.Dispose();
				this->GaugeGraph.Dispose();
				this->RunGraph.Dispose();
				this->AdrenalineGraph.Dispose();
			}
		public:
			void			SetIntParam(size_t ID, int value) noexcept { intParam[ID] = value; }
			void			SetfloatParam(size_t ID, float value) noexcept { floatParam[ID] = value; }
			void			SetStrParam(size_t ID, std::string_view value) noexcept { strParam[ID] = value; }
		};

		class MainSceneResultUI {
			std::array<Player::SkillType, 3>			m_SkillSelect{};
			float										m_GameClearTimer = 0.f;
			int											m_SkillSelectNow = 0;
			float										m_GameClearCount = 0.f;
			float										m_SkillSelectTimer = 0.f;
			bool										m_IsSkillSelect = false;
			bool										m_IsSkillSelectEnd = false;
			bool										m_IsGameClearEnd = false;
			GraphHandle									m_GameEndScreen;
			GraphHandle									m_ResultGraph;
			float										m_BattleTimer{};
		public:
			MainSceneResultUI(void) noexcept { Load(); }
			MainSceneResultUI(const MainSceneResultUI&) = delete;
			MainSceneResultUI(MainSceneResultUI&&) = delete;
			MainSceneResultUI& operator=(const MainSceneResultUI&) = delete;
			MainSceneResultUI& operator=(MainSceneResultUI&&) = delete;

			virtual ~MainSceneResultUI(void) noexcept { Dispose(); }
		public:
			const auto GetSkillSelect() const noexcept { return static_cast<int>(this->m_SkillSelect.at(this->m_SkillSelectNow)); }
			const auto GetTimer() const noexcept { return this->m_GameClearTimer; }
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept {
				this->m_SkillSelectNow = 0;
				this->m_GameClearCount = 0.6f;
				this->m_GameClearTimer = 0.f;
				this->m_IsSkillSelect = false;
				this->m_IsSkillSelectEnd = false;
				this->m_IsGameClearEnd = false;
				this->m_SkillSelectTimer = 0.f;
			}
			void			Start(float Time) noexcept;
			int				Update(void) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept {
				this->m_GameEndScreen.Dispose();
				this->m_ResultGraph.Dispose();
			}
		};

		class TransceiverUI {
			int											m_TextID = InvalidID;
			int											m_PrevTextSoundID = InvalidID;
			float										m_TextTimer{ 0.f };
			float										m_TextTimerMax{ -1.f };
		public:
			TransceiverUI(void) noexcept {}
			TransceiverUI(const TransceiverUI&) = delete;
			TransceiverUI(TransceiverUI&&) = delete;
			TransceiverUI& operator=(const TransceiverUI&) = delete;
			TransceiverUI& operator=(TransceiverUI&&) = delete;

			virtual ~TransceiverUI(void) noexcept {}
		public:
			void			Put(int ID, float Sec = -1.f) noexcept;
			const auto&		GetID(void) const noexcept { return this->m_TextID; }
		public:
			void			Set(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
		};


		enum class TaskType {
			Obtain,
			KillEnemy,
		};

		class TaskOperator {
			class TaskInfo {
			public:
				TaskType	m_TaskType{};
				int		m_ItemID{ InvalidID };
				int		m_Count{ 1 };
			};
		private:
			GraphHandle									m_KillGraph;
			std::vector<std::pair<TaskInfo, int>>		m_TaskInfoList;
		public:
			TaskOperator(void) noexcept { Load(); }
			TaskOperator(const TaskOperator&) = delete;
			TaskOperator(TaskOperator&&) = delete;
			TaskOperator& operator=(const TaskOperator&) = delete;
			TaskOperator& operator=(TaskOperator&&) = delete;

			virtual ~TaskOperator(void) noexcept { Dispose(); }
		public:
			const auto HaveTask() const noexcept {
				return (this->m_TaskInfoList.size() > 0);
			}
			const auto IsActiveTask(TaskType Type) const noexcept {
				return HaveTask() && (m_TaskInfoList.begin()->first.m_TaskType == Type);
			}
			const auto GetNowTaskItemID() const noexcept {
				return this->m_TaskInfoList.begin()->first.m_ItemID;
			}
			
			void			AddObtain(int ItemID, int Count) noexcept {
				std::pair<TaskInfo, int> Tmp;
				Tmp.first.m_TaskType = TaskType::Obtain;
				Tmp.first.m_ItemID = ItemID;
				Tmp.first.m_Count = Count;
				Tmp.second = 0;
				m_TaskInfoList.emplace_back(Tmp);
			}
			void			AddKillEnemy(int Count) noexcept {
				std::pair<TaskInfo, int> Tmp;
				Tmp.first.m_TaskType = TaskType::KillEnemy;
				Tmp.first.m_Count = Count;
				Tmp.second = 0;
				m_TaskInfoList.emplace_back(Tmp);
			}
			void			Shuffle() noexcept {
				std::random_device seed_gen;
				std::mt19937 engine(seed_gen());
				std::shuffle(this->m_TaskInfoList.begin(), m_TaskInfoList.end(), engine);
			}
			void			StartNextTask() noexcept;
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			bool			CheckItem(int ItemID) noexcept;
			bool			CheckKill(void) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept;
		};

		class StartMovie {
			std::shared_ptr<Charas::MovieObject>		m_MovieHeli;
			float										m_StartAnimTimer{ 0.0f };
			float										m_MovieEndTimer{ 0.0f };
			bool										m_IsSkipMovie{ false };
		public:
			StartMovie(void) noexcept { Load(); }
			StartMovie(const StartMovie&) = delete;
			StartMovie(StartMovie&&) = delete;
			StartMovie& operator=(const StartMovie&) = delete;
			StartMovie& operator=(StartMovie&&) = delete;

			virtual ~StartMovie(void) noexcept { Dispose(); }
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			bool			Update(void) noexcept;
			void			Dispose(void) noexcept {
				m_MovieHeli.reset();
			}
		};
	}
}
