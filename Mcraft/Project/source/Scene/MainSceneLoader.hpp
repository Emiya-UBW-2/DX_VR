#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class LoadScene : public TEMPSCENE {
		private:
		public:
			LoadScene(void) noexcept {}
			LoadScene(const LoadScene&) = delete;
			LoadScene(LoadScene&&) = delete;
			LoadScene& operator=(const LoadScene&) = delete;
			LoadScene& operator=(LoadScene&&) = delete;

			virtual ~LoadScene(void) noexcept {}
		public:
			void Set_Sub(void) noexcept override {
				SetUseASyncLoadFlag(true);
				Get_Next()->Load();//���V�[���̃��[�h��
				SetUseASyncLoadFlag(false);
			}
			bool Update_Sub(void) noexcept override {
				return (GetASyncLoadNum() > 0);
			}
			void Dispose_Sub(void) noexcept override {
			}
		public:
			//UI�\��
			void DrawUI_Base_Sub(void) const noexcept override {
			}
		};
	}
}