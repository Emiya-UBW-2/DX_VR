#pragma once
#pragma warning(disable:4464)

#include	"../sub.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PauseMenuControl {
			bool			m_IsRetire{ false };
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

			float						m_AmmoRand{};
			float						m_AmmoRandR{};
			float						m_AmmoInPer{};
			int							m_AmmoNumTotal{};

			int							m_RadPrev{};
			float						m_RadR{};

			float						m_Gram{};

			float						m_Score{};
		public:
			MainSceneUI(void) noexcept {}
			MainSceneUI(const MainSceneUI&) = delete;
			MainSceneUI(MainSceneUI&&) = delete;
			MainSceneUI& operator=(const MainSceneUI&) = delete;
			MainSceneUI& operator=(MainSceneUI&&) = delete;

			virtual ~MainSceneUI(void) noexcept {}
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
	}
}
