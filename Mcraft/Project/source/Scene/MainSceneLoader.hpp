#pragma once
#include"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class LoadScene : public TEMPSCENE {
		private:
		public:
			LoadScene(void) noexcept {}
			LoadScene(const LoadScene&) = delete;
			LoadScene(LoadScene&& o) = delete;
			LoadScene& operator=(const LoadScene&) = delete;
			LoadScene& operator=(LoadScene&& o) = delete;

			virtual ~LoadScene(void) noexcept {}
		public:
			void Set_Sub(void) noexcept override {
				SetUseASyncLoadFlag(TRUE);
				Get_Next()->Load();//���V�[���̃��[�h��
				SetUseASyncLoadFlag(FALSE);
			}
			bool Update_Sub(void) noexcept override {
				return (GetASyncLoadNum() > 0);
			}
			void Dispose_Sub(void) noexcept override {
			}
		public:
			//UI�\��
			void DrawUI_Base_Sub(void) noexcept override {
			}
		};
	}
}