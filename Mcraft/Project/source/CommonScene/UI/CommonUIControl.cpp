#include	"CommonUIControl.hpp"

const FPS_n2::UIs::ButtonControl* SingletonBase<FPS_n2::UIs::ButtonControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace UIs {
		// 
		bool ButtonControl::GetTriggerButton(void) const noexcept {
			auto* Pad = PadControl::Instance();
			return (this->m_select != InvalidID) && (this->m_MouseSelectMode ? Pad->GetMouseClick().trigger() : Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger());
		}
		ButtonControl::ButtonControl(void) noexcept {
			this->m_SelectBackImage.Load("CommonData/UI/select.png");
			ResetSelect();
		}
		ButtonControl::~ButtonControl(void) noexcept {
			Dispose();
			this->m_SelectBackImage.Dispose();
		}
		void ButtonControl::UpdateInput(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();

			int preselect = this->m_select;
			bool preMouseSelect = this->m_MouseSelectMode;
			int Max = static_cast<int>(this->m_ButtonSelect.size()) - 1;
			if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().trigger() || Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().trigger()) {
				if (this->m_select != InvalidID) {
					--this->m_select;
					if (this->m_select < 0) { this->m_select = Max; }
				}
				else {
					this->m_select = 0;
				}
				this->m_MouseSelectMode = false;
			}
			if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().trigger() || Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().trigger()) {
				if (this->m_select != InvalidID) {
					++this->m_select;
					if (this->m_select > Max) { this->m_select = 0; }
				}
				else {
					this->m_select = 0;
				}
				this->m_MouseSelectMode = false;
			}

			if (this->m_MouseSelectMode) {
				this->m_select = InvalidID;
			}
			// 
			for (auto& select : this->m_ButtonSelect) {
				if (select->GetInto()) {
					this->m_MouseSelectMode = true;
					this->m_select = static_cast<int>(&select - &m_ButtonSelect.front());
				}
			}
			if (preselect != this->m_select || preMouseSelect != this->m_MouseSelectMode) {
				if (this->m_select != InvalidID) {
					for (auto& select : this->m_ButtonSelect) {
						select->SetNone();
					}
					this->m_ButtonSelect[static_cast<size_t>(this->m_select)]->SetFocus();
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
				}
				else {
					for (auto& select : this->m_ButtonSelect) {
						select->SetReady();
					}
				}
			}
		}
		void ButtonControl::Update(void) noexcept {
			for (auto& select : this->m_ButtonSelect) {
				select->Update();
			}
		}
		void ButtonControl::Draw(void) noexcept {
			for (auto& select : this->m_ButtonSelect) {
				select->Draw();
			}
		}
		void ButtonControl::Dispose(void) noexcept {
			for (auto& select : this->m_ButtonSelect) {
				select.reset();
			}
			this->m_ButtonSelect.clear();
		}
		// 
		CreditControl::CreditControl(void) noexcept {
			FileStreamDX FileStream("data/Credit.txt");
			this->m_CreditCoulm = 0;
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = FileStreamDX::getleft(ALL);
					auto RIGHT = FileStreamDX::getright(ALL);
					sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].first, LEFT.c_str());
					sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].second, RIGHT.c_str());
				}
				else {
					sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].first, ALL.c_str());
					sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].second, "");
				}
				++this->m_CreditCoulm;
			}
		}
		CreditControl::~CreditControl(void) noexcept {
			this->m_CreditCoulm = 0;
			for (auto& credit : this->m_CreditStr) {
				sprintfDx(credit.first, "");
				sprintfDx(credit.second, "");
			}
		}

		void CreditControl::Draw(int xmin, int ymin, int xmax) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			int xp1, yp1;

			xp1 = xmin + (24);
			yp1 = ymin + LineHeight;
			int Height = (12);
			for (auto& credit : this->m_CreditStr) {
				if (this->m_CreditCoulm < static_cast<int>(&credit - &this->m_CreditStr.front())) { break; }
				int xpos = xp1 + (6);
				int ypos = yp1 + Height / 2;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, Height,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xpos, ypos, White, Black, credit.first);

				xpos = xmax - (24);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, Height,
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::MIDDLE, xpos, ypos, White, Black, credit.second);
				yp1 += Height;
			}
		}
	};
};
