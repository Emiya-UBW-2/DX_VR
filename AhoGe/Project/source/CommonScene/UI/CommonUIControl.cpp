#include	"CommonUIControl.hpp"

const FPS_n2::Sceneclass::ButtonControl* SingletonBase<FPS_n2::Sceneclass::ButtonControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		//
		const bool ButtonControl::GetTriggerButton() const noexcept {
			auto* Pad = PadControl::Instance();
			return (select != -1) && (m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger());
		}
		ButtonControl::ButtonControl() noexcept {
			m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
			ResetSel();
		}
		ButtonControl::~ButtonControl() noexcept {
			Dispose();
			m_SelectBackImage.Dispose();
		}
		void ButtonControl::UpdateInput() noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();

			int preselect = select;
			bool preMouseSel = m_MouseSelMode;
			if (Pad->GetKey(PADS::MOVE_W).trigger() || Pad->GetKey(PADS::MOVE_A).trigger()) {
				if (select != -1) {
					--select;
					if (select < 0) { select = (int)ButtonSel.size() - 1; }
				}
				else {
					select = 0;
				}
				m_MouseSelMode = false;
			}
			if (Pad->GetKey(PADS::MOVE_S).trigger() || Pad->GetKey(PADS::MOVE_D).trigger()) {
				if (select != -1) {
					++select;
					if (select > (int)ButtonSel.size() - 1) { select = 0; }
				}
				else {
					select = 0;
				}
				m_MouseSelMode = false;
			}

			if (m_MouseSelMode) {
				select = -1;
			}
			//
			for (auto& y : ButtonSel) {
				if (y->GetInto()) {
					m_MouseSelMode = true;
					select = (int)(&y - &ButtonSel.front());
				}
			}
			if (preselect != select || preMouseSel != m_MouseSelMode) {
				if (select != -1) {
					for (auto& y : ButtonSel) {
						y->SetNone();
					}
					ButtonSel.at(select)->SetFocus();
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					for (auto& y : ButtonSel) {
						y->SetReady();
					}
				}
			}
		}
		void ButtonControl::Update() noexcept {
			for (auto& y : ButtonSel) {
				y->Update();
			}
		}
		void ButtonControl::Draw() noexcept {
			for (auto& y : ButtonSel) {
				y->Draw();
			}
		}
		void ButtonControl::Dispose() noexcept {
			for (auto& y : ButtonSel) {
				y->Dispose();
				y.reset();
			}
			ButtonSel.clear();
		}
		//
		void CreditControl::Init() noexcept {
			int mdata = FileRead_open("data/Credit.txt", FALSE);
			m_CreditCoulm = 0;
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					sprintfDx(m_CreditStr.at(m_CreditCoulm).first, LEFT.c_str());
					sprintfDx(m_CreditStr.at(m_CreditCoulm).second, RIGHT.c_str());
				}
				else {
					sprintfDx(m_CreditStr.at(m_CreditCoulm).first, ALL.c_str());
					sprintfDx(m_CreditStr.at(m_CreditCoulm).second, "");
				}
				m_CreditCoulm++;
			}
			FileRead_close(mdata);
		}
		void CreditControl::Draw(int xmin, int ymin, int xmax) noexcept {
			auto* Fonts = FontPool::Instance();

			int xp1, yp1;

			xp1 = xmin + y_UI(24);
			yp1 = ymin + LineHeight;
			int Height = y_UI(12);
			for (int i = 0; i < m_CreditCoulm; i++) {
				int xpos = xp1 + y_UI(6);
				int ypos = yp1 + (yp1 + Height - yp1) / 2;
				Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, m_CreditStr.at(i).first);

				xpos = xmax - y_UI(24);
				Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, m_CreditStr.at(i).second);
				yp1 += Height;
			}
		}
		void CreditControl::Dispose() noexcept {
			m_CreditCoulm = 0;
			for (auto& c : m_CreditStr) {
				sprintfDx(c.first, "");
				sprintfDx(c.second, "");
			}
		}
	};
};
