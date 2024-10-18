#pragma once
#include	"../Header.hpp"
#include "../CommonScene/Object/GunsModify.hpp"
#include "../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class LookSelect {
			ModSet,
			ULTSet,
			FreeLook,
			Max,
		};

		class CustomScene : public TEMPSCENE, public GunsModify {
			class SlotMove {
			public:
				float				Xadd{0.f};
				float				Yadd{0.f};
				GunsModify::Slot*	Ptr{nullptr};
				int					index{0};
			};
		private:
			int							select{0};
			float						m_SelAlpha{0.f};
			float						m_Yrad{0.f};
			float						m_Xrad{0.f};
			float						m_Yrad_R{0.f};
			float						m_Xrad_R{0.f};
			float						m_Range{1.f};
			std::vector<SlotMove>		SelMoveClass;
			bool						m_IsEnd{false};
			float						m_Alpha{0.f};
			LookSelect					m_LookSel{LookSelect::ModSet};
			GraphHandle					m_SelectBackImage;
			std::array<float, 3>		ButtonAlpha{};
			std::array<ButtonClass, 8>	ButtonSel{};
			int							bselect{0};
			bool						m_MouseSelMode{false};
			std::array<Matrix4x4DX, (int)ULT_GUN::Max>	m_UltMat;
			std::array<float, (int)ULT_GUN::Max>	m_UltPer{};
		private:

			bool			IsG17Parts(const Slot& S) {
				bool IsG17 = false;
				{
					auto* Data = &S;
					while (true) {
						if (Data == nullptr) { break; }
						if (Data->m_Data->GetModData()->GetName() == "Glock17 Gen3") {
							IsG17 = true;
							break;
						}
						Data = Data->ParentSlot;
					}
				}
				return IsG17;
			}

			void			UpdateSlotMove() {
				//�������݂�����̂��폜
				for (int loop = 0; loop < SelMoveClass.size(); loop++) {
					bool isHit = false;
					for (const auto& S : GunsModify::GetSelData()) {
						if (S.get() == SelMoveClass[loop].Ptr) {
							isHit = true;
							break;
						}
					}
					if (!isHit) {
						SelMoveClass.erase(SelMoveClass.begin() + loop);
						loop--;
					}
				}
				//���݂�����̂�ǉ�+���Ԃ��w��
				for (const auto& S : GunsModify::GetSelData()) {
					if (!IsG17Parts(*S)) { continue; }
					int index = static_cast<int>(&S - &GunsModify::GetSelData().front());
					bool isHit = false;
					for (int loop = 0; loop < SelMoveClass.size(); loop++) {
						if (S.get() == SelMoveClass[loop].Ptr) {
							SelMoveClass[loop].index = index;
							isHit = true;
							break;
						}
					}
					if (!isHit) {
						SelMoveClass.resize(SelMoveClass.size() + 1);
						SelMoveClass.back().Ptr = S.get();
						SelMoveClass.back().index = index;
						SelMoveClass.back().Xadd = 0.f;
						SelMoveClass.back().Yadd = 0.f;
					}
				}
				//SelMoveClass��GunsModify::GetSelData()�ɍ��킹�ă\�[�g
				std::sort(SelMoveClass.begin(), SelMoveClass.end(),
						  [](const SlotMove& a, const SlotMove& b) {
							  return a.index < b.index;
						  }
				);
			}
			void			DrawCustomUI(void) noexcept;
			void			DrawULTUI(void) noexcept;

			const char*		ItemSelName(int SlotSel, int sel);

			void			ResetMod() noexcept;
			void			ChangeModSel(int add) noexcept;
			void			ChangeMod(int add) noexcept;
			void			ChangeULT(int add) noexcept;
		public:
			CustomScene(void) noexcept {}
			~CustomScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			//void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			SetShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			//UI�\��
			void			DrawUI_Base_Sub(void) noexcept override;
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
