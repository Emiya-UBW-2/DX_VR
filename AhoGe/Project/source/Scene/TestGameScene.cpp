#include	"TestGameScene.hpp"

#include "../sub.hpp"

#include "../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TestGameScene::Load_Sub(void) noexcept {}
		void			TestGameScene::Set_Sub(void) noexcept {}
		bool			TestGameScene::Update_Sub(void) noexcept { return false; }
		void			TestGameScene::Dispose_Sub(void) noexcept {}
		void			TestGameScene::MainDraw_Sub(void) noexcept {}
		void			TestGameScene::DrawUI_Base_Sub(void) noexcept {}
		void			TestGameScene::DrawUI_In_Sub(void) noexcept {}
		void			TestGameScene::Dispose_Load_Sub(void) noexcept {}
	};
};
