#include	"CommonUIControl.hpp"

const FPS_n2::Sceneclass::ButtonControl* SingletonBase<FPS_n2::Sceneclass::ButtonControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		// 
		bool ButtonControl::GetTriggerButton(void) const noexcept {
			auto* Pad = PadControl::Instance();
			return (select != INVALID_ID) && (this->m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger());
		}
		ButtonControl::ButtonControl(void) noexcept {
			this->m_SelectBackImage = GraphHandle::Load("CommonData/UI/select.png");
			ResetSel();
		}
		ButtonControl::~ButtonControl(void) noexcept {
			Dispose();
			this->m_SelectBackImage.Dispose();
		}
		void ButtonControl::UpdateInput(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();

			int preselect = select;
			bool preMouseSel = this->m_MouseSelMode;
			if (Pad->GetKey(PADS::MOVE_W).trigger() || Pad->GetKey(PADS::MOVE_A).trigger()) {
				if (select != INVALID_ID) {
					--select;
					if (select < 0) { select = static_cast<int>(ButtonSel.size()) - 1; }
				}
				else {
					select = 0;
				}
				this->m_MouseSelMode = false;
			}
			if (Pad->GetKey(PADS::MOVE_S).trigger() || Pad->GetKey(PADS::MOVE_D).trigger()) {
				if (select != INVALID_ID) {
					++select;
					if (select > static_cast<int>(ButtonSel.size()) - 1) { select = 0; }
				}
				else {
					select = 0;
				}
				this->m_MouseSelMode = false;
			}

			if (this->m_MouseSelMode) {
				select = INVALID_ID;
			}
			// 
			for (auto& y : ButtonSel) {
				if (y->GetInto()) {
					this->m_MouseSelMode = true;
					select = static_cast<int>(&y - &ButtonSel.front());
				}
			}
			if (preselect != select || preMouseSel != this->m_MouseSelMode) {
				if (select != INVALID_ID) {
					for (auto& y : ButtonSel) {
						y->SetNone();
					}
					ButtonSel.at(static_cast<size_t>(select))->SetFocus();
					SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					for (auto& y : ButtonSel) {
						y->SetReady();
					}
				}
			}
		}
		void ButtonControl::Update(void) noexcept {
			for (auto& y : ButtonSel) {
				y->Update();
			}
		}
		void ButtonControl::Draw(void) noexcept {
			for (auto& y : ButtonSel) {
				y->Draw();
			}
		}
		void ButtonControl::Dispose(void) noexcept {
			for (auto& y : ButtonSel) {
				y->Dispose();
				y.reset();
			}
			ButtonSel.clear();
		}
		// 
		void CreditControl::Init(void) noexcept {
			int mdata = FileRead_open("data/Credit.txt", FALSE);
			this->m_CreditCoulm = 0;
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).first, LEFT.c_str());
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).second, RIGHT.c_str());
				}
				else {
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).first, ALL.c_str());
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).second, "");
				}
				++this->m_CreditCoulm;
			}
			FileRead_close(mdata);
		}
		void CreditControl::Draw(int xmin, int ymin, int xmax) const noexcept {
			auto* DrawParts = DXDraw::Instance();

			int xp1, yp1;

			xp1 = xmin + DrawParts->GetUIY(24);
			yp1 = ymin + LineHeight;
			int Height = DrawParts->GetUIY(12);
			for (auto& c : this->m_CreditStr) {
				if (this->m_CreditCoulm < static_cast<int>(&c - &this->m_CreditStr.front())) { break; }
				int xpos = xp1 + DrawParts->GetUIY(6);
				int ypos = yp1 + Height / 2;
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::DIZ_UD_Gothic, Height,
					FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xpos, ypos, White, Black, c.first);

				xpos = xmax - DrawParts->GetUIY(24);
				WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::DIZ_UD_Gothic, Height,
					FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xpos, ypos, White, Black, c.second);
				yp1 += Height;
			}
		}
		void CreditControl::Dispose(void) noexcept {
			this->m_CreditCoulm = 0;
			for (auto& c : this->m_CreditStr) {
				sprintfDx(c.first, "");
				sprintfDx(c.second, "");
			}
		}
	};
};
