#pragma once
#pragma warning(disable:4464)

#include	"../sub.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PauseMenuControl {
			bool			m_IsRetire{ false };
		public:
			const auto&		IsRetire(void) const noexcept { return this->m_IsRetire; }
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
		private:
			std::array<int, 6>				intParam{};
			std::array<float, 6>			floatParam{};
			std::array<std::string, 1>		strParam;

			bool CanLookTarget = false;
			float LookPer{};
			float IsDrawAimUIPer{};
		public:
			MainSceneUI(void) noexcept {}
			MainSceneUI(const MainSceneUI&) = delete;
			MainSceneUI(MainSceneUI&&) = delete;
			MainSceneUI& operator=(const MainSceneUI&) = delete;
			MainSceneUI& operator=(MainSceneUI&&) = delete;

			virtual ~MainSceneUI(void) noexcept {}
		public:
			void			Load(void) noexcept {}
			void			Set(void) noexcept {}
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept {}
		public:
			void			SetIntParam(int ID, int value) noexcept { intParam[static_cast<size_t>(ID)] = value; }
			void			SetfloatParam(int ID, float value) noexcept { floatParam[static_cast<size_t>(ID)] = value; }
			void			SetStrParam(int ID, std::string_view value) noexcept { strParam[static_cast<size_t>(ID)] = value; }
		};
	};
};
