#include	"CommonUIControl.hpp"

const FPS_n2::Sceneclass::ButtonControl* SingletonBase<FPS_n2::Sceneclass::ButtonControl>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::FadeControl* SingletonBase<FPS_n2::Sceneclass::FadeControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		// 
		bool ButtonControl::GetTriggerButton(void) const noexcept {
			auto* Pad = PadControl::Instance();
			return (select != InvalidID)
				&& ButtonSel.at(static_cast<size_t>(select))->IsEnableSelect()
				&& (this->m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetPadsInfo(PADS::INTERACT).GetKey().trigger());
		}
		ButtonControl::ButtonControl(void) noexcept {
			this->m_SelectBackImage.Load("CommonData/UI/select.png");
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
			if (Pad->GetPadsInfo(PADS::MOVE_W).GetKey().trigger() || Pad->GetPadsInfo(PADS::MOVE_A).GetKey().trigger()) {
				if (select != InvalidID) {
					--select;
					if (select < 0) { select = static_cast<int>(ButtonSel.size()) - 1; }
				}
				else {
					select = 0;
				}
				this->m_MouseSelMode = false;
			}
			if (Pad->GetPadsInfo(PADS::MOVE_S).GetKey().trigger() || Pad->GetPadsInfo(PADS::MOVE_D).GetKey().trigger()) {
				if (select != InvalidID) {
					++select;
					if (select > static_cast<int>(ButtonSel.size()) - 1) { select = 0; }
				}
				else {
					select = 0;
				}
				this->m_MouseSelMode = false;
			}

			if (this->m_MouseSelMode) {
				select = InvalidID;
			}
			// 
			for (auto& y : ButtonSel) {
				if (y->GetInto()) {
					this->m_MouseSelMode = true;
					select = static_cast<int>(&y - &ButtonSel.front());
				}
			}
			if (preselect != select || preMouseSel != this->m_MouseSelMode) {
				if (select != InvalidID) {
					for (auto& y : ButtonSel) {
						y->SetNone();
					}
					ButtonSel.at(static_cast<size_t>(select))->SetFocus();
					SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
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
			this->m_CreditCoulm = 0;
			FileStreamDX FileStream("data/Credit.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = FileStreamDX::getleft(ALL);
					auto RIGHT = FileStreamDX::getright(ALL);
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).first, LEFT.c_str());
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).second, RIGHT.c_str());
				}
				else {
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).first, ALL.c_str());
					sprintfDx(this->m_CreditStr.at(static_cast<size_t>(this->m_CreditCoulm)).second, "");
				}
				++this->m_CreditCoulm;
			}
		}
		void CreditControl::Draw(int xmin, int ymin, int xmax) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();

			int xp1, yp1;

			xp1 = xmin + (24);
			yp1 = ymin + LineHeight;
			int Height = (12);
			for (auto& c : this->m_CreditStr) {
				if (this->m_CreditCoulm < static_cast<int>(&c - &this->m_CreditStr.front())) { break; }
				int xpos = xp1 + (6);
				int ypos = yp1 + Height / 2;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, Height,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xpos, ypos, White, Black, c.first);

				xpos = xmax - (24);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, Height,
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::MIDDLE, xpos, ypos, White, Black, c.second);
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
		void InfoControl::DelPage() noexcept {
			if (this->m_InfoStr.at(m_InfoNow).m_Graph.IsActive()) {
				if (this->m_InfoStr.at(m_InfoNow).m_IsMovie) {
					PauseMovieToGraph(this->m_InfoStr.at(m_InfoNow).m_Graph.get());
				}
			}
		}
		void InfoControl::SetPage() noexcept {
			if (this->m_InfoStr.at(m_InfoNow).m_Graph.IsActive()) {
				if (this->m_InfoStr.at(m_InfoNow).m_IsMovie) {
					ChangeMovieVolumeToGraph(0, this->m_InfoStr.at(m_InfoNow).m_Graph.get());
					PlayMovieToGraph(this->m_InfoStr.at(m_InfoNow).m_Graph.get());
				}
			}
		}
		// 
		void InfoControl::Init(void) noexcept {
			FileStreamDX FileStream("data/Info.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL == "") { continue; }
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = FileStreamDX::getleft(ALL);
					auto RIGHT = FileStreamDX::getright(ALL);
					if (LEFT == "AddPage") {
						this->m_InfoStr.emplace_back();
					}
					else if (LEFT == "Text") {
						this->m_InfoStr.back().m_String.emplace_back(RIGHT);
					}
					else if (LEFT == "Graph") {
						this->m_InfoStr.back().m_Graph.Load(RIGHT);
						this->m_InfoStr.back().m_IsMovie = (RIGHT.find("mp4") != std::string::npos);
					}
				}
			}
		}
		void InfoControl::Start() noexcept {
			m_InfoNow = 0;
			SetPage();
		}
		void InfoControl::Draw(int xmin, int ymin, int xmax, int ymax) noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* Pad = PadControl::Instance();

			int xp1, yp1;

			xp1 = xmin + 72;
			yp1 = ymin + LineHeight;
			if (this->m_InfoStr.at(m_InfoNow).m_Graph.IsActive()) {
				if (this->m_InfoStr.at(m_InfoNow).m_IsMovie) {
					if (GetMovieStateToGraph(this->m_InfoStr.at(m_InfoNow).m_Graph.get()) == 0) {
						SeekMovieToGraph(this->m_InfoStr.at(m_InfoNow).m_Graph.get(), 0);
						PlayMovieToGraph(this->m_InfoStr.at(m_InfoNow).m_Graph.get());
					}
				}
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
					&this->m_InfoStr.at(m_InfoNow).m_Graph,
					xp1, yp1,
					(xmax - 72), yp1 + ((xmax - 72) - xp1) * 720 / 600,
					false);
				yp1 = yp1 + ((xmax - 72) - xp1) * 720 / 600;
			}
			int Height = (16);
			for (const auto& c : this->m_InfoStr.at(m_InfoNow).m_String) {
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, Height,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1 + (6), yp1, White, Black, c);
				yp1 += Height + 4;
			}

			xp1 = (xmax + xmin) / 2;
			yp1 = ymax - LineHeight * 1;
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::DIZ_UD_Gothic, Height,
				FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1 + (6), yp1, White, Black, "%d / %d", m_InfoNow + 1, (int)(this->m_InfoStr.size()));
			// 
			{
				yp1 = ymax - LineHeight * 3;
				bool ret = WindowSystem::SetMsgClickBox(xmin + 6, yp1, xmin + 48, yp1 + LineHeight * 2, LineHeight, Gray15, false, true, "<");
				if (Pad->GetPadsInfo(PADS::MOVE_A).GetKey().trigger() || ret) {
					DelPage();
					--m_InfoNow;
					if (m_InfoNow < 0) {
						m_InfoNow = (int)(this->m_InfoStr.size()) -1;
					}
					SetPage();
				}
			}
			// 
			{
				yp1 = ymax - LineHeight * 3;
				bool ret = WindowSystem::SetMsgClickBox(xmax - 48, yp1, xmax - 6, yp1 + LineHeight * 2, LineHeight, Gray15, false, true, ">");
				if (Pad->GetPadsInfo(PADS::MOVE_D).GetKey().trigger() || ret) {
					DelPage();
					++m_InfoNow %= (int)(this->m_InfoStr.size());
					SetPage();
				}
			}
		}
		void InfoControl::End() noexcept {
			DelPage();
		}
		void InfoControl::Guide() noexcept {
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(PADS::MOVE_A), "");
			KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(PADS::MOVE_D), "");
			KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(PADS::MOVE_STICK), LocalizeParts->Get(9993));
		}
		void InfoControl::Dispose(void) noexcept {
			for (auto& c : this->m_InfoStr) {
				c.Dispose();
			}
			this->m_InfoStr.clear();
		}
		// 
		void FadeControl::SetFadeIn(float Per) noexcept {
			this->m_IsBlackOut = false;
			this->m_BlackOutAlpha = 1.f;
			this->m_BlackOutPower = Per;
		}
		void FadeControl::SetFadeOut(float Per) noexcept {
			this->m_IsBlackOut = true;
			this->m_BlackOutAlpha = 0.f;
			this->m_BlackOutPower = Per;
		}
		void FadeControl::Update(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.f : -1.f) * DXLib_refParts->GetDeltaTime() * this->m_BlackOutPower, 0.f, 1.f);
		}
		void FadeControl::Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			if (this->m_BlackOutAlpha > 0.f) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, BaseScreenWidth, BaseScreenHeight, Black, TRUE);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
		//
	};
};
