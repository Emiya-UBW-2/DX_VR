#pragma warning(disable:4464)
#include	"NetworkBrowser.hpp"

const FPS_n2::NetWorkBrowser* SingletonBase<FPS_n2::NetWorkBrowser>::m_Singleton = nullptr;

namespace FPS_n2 {
	void NetWorkBrowser::Init(void) noexcept {
		this->m_Sequence = BrowserSequence::SelMode;
	}
	void NetWorkBrowser::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();

		int xp, yp, xs, ys;
		xp = DrawParts->GetUIY(100);
		yp = DrawParts->GetUIY(250);
		xs = DrawParts->GetUIY(500);
		ys = DrawParts->GetUIY(300);

		//ラムダ
		auto AddSubBox = [&](int xp, int yp, std::function<void()> UpFunc, std::function<void()> DownFunc) {
			int xp1, yp1;
			int xp2, yp2;
			{
				xp1 = xp - DrawParts->GetUIY(25);
				yp1 = yp - DrawParts->GetUIY(25) - LineHeight / 2;
				xp2 = xp + DrawParts->GetUIY(25);
				yp2 = yp1 + LineHeight;
				if (WindowSystem::SetMsgClickBox(xp1, yp1, xp2, yp2, LineHeight, Gray75, true, "+")) {
					UpFunc();
				}
			}
			{
				yp1 += DrawParts->GetUIY(50);
				yp2 += DrawParts->GetUIY(50);
				if (WindowSystem::SetMsgClickBox(xp1, yp1, xp2, yp2, LineHeight, Gray75, true, "-")) {
					DownFunc();
				}
			}
			};
		//
		{
			WindowSystem::SetBox(xp - DrawParts->GetUIY(10), yp - DrawParts->GetUIY(10), xp + xs + DrawParts->GetUIY(10), yp + ys + DrawParts->GetUIY(10), Gray25);//背景
			WindowSystem::SetMsg(xp + xs, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, " %d/%d", static_cast<int>(this->m_Sequence), static_cast<int>(NetWork::NetWorkSequence::MainGame));
		}
		auto Prev = this->m_Sequence;

		int y1p = yp + DrawParts->GetUIY(50);
		switch (this->m_Sequence) {
		case BrowserSequence::SelMode:
			if (WindowSystem::SetMsgClickBox(xp, y1p, xp + xs, y1p + LineHeight * 2, LineHeight, Gray75, false, "クライアントになる")) {
				BeClient();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(50), xp + xs, y1p + DrawParts->GetUIY(50) + LineHeight * 2, LineHeight, Gray75, false, "サーバーになる")) {
				BeServerPlayer();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(100), xp + xs, y1p + DrawParts->GetUIY(100) + LineHeight * 2, LineHeight, Gray75, false, "サーバーになる(非プレイヤー)")) {
				BeServer();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			//
			if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(150), xp + xs, y1p + DrawParts->GetUIY(150) + LineHeight * 2, LineHeight, Gray75, false, "クライアントで即プレイ")) {
				BeClient();
				m_NewWorkSettings.Load();
				ReadyConnect(m_NewWorkSettings.Get(0));
			}
			if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(200), xp + xs, y1p + DrawParts->GetUIY(200) + LineHeight * 2, LineHeight, Gray75, false, "サーバーで即プレイ")) {
				BeServerPlayer();
				m_NewWorkSettings.Load();
				ReadyConnect(m_NewWorkSettings.Get(0));
			}
			break;
		case BrowserSequence::CheckPreset:
			WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "プリセット設定");
			if (m_SeqFirst) {
				m_NewWorkSettings.Load();
			}
			for (int i = 0, Num = this->m_NewWorkSettings.GetSize(); i < Num + 1; ++i) {
				if (i < Num) {
					auto& n = this->m_NewWorkSettings.Get(i);
					if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(50) * i, xp + xs, y1p + DrawParts->GetUIY(50) * i + LineHeight * 2, LineHeight, Gray75, false, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
						ReadyConnect(n);
						break;
					}
				}
				else {
					if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(50) * i, xp + xs, y1p + DrawParts->GetUIY(50) * i + LineHeight * 2, LineHeight, Gray75, false, "設定を追加する")) {
						this->m_Sequence = BrowserSequence::SetNewData;
						break;
					}
				}
			}
			break;
		case BrowserSequence::SetNewData:
			WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "新規設定");
			if (m_SeqFirst) {
				m_NewWorkSettings.AddBack();
			}
			{
				WindowSystem::SetMsg(xp + xs / 2, y1p + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "ポート=[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + NetWork::Player_num - 1);
				AddSubBox(xp + xs / 2, y1p + LineHeight / 2, [&]() { ++this->m_NetSetting.UsePort; }, [&]() { --this->m_NetSetting.UsePort; });
			}
			if (this->m_IsClient) {//サーバ-はいらない
				int yp1 = y1p + DrawParts->GetUIY(100);
				WindowSystem::SetMsg(xp + xs / 2, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4);
				for (int i = 0; i < 4; ++i) {
					auto* ip_tmp = &this->m_NetSetting.IP.d1;
					switch (i) {
					case 0:ip_tmp = &this->m_NetSetting.IP.d1; break;
					case 1:ip_tmp = &this->m_NetSetting.IP.d2; break;
					case 2:ip_tmp = &this->m_NetSetting.IP.d3; break;
					case 3:ip_tmp = &this->m_NetSetting.IP.d4; break;
					}
					AddSubBox(xp + xs / 2 + DrawParts->GetUIY(70 * (i - 2) + 35), yp1 + LineHeight / 2,
						[&]() {
							if (*ip_tmp == 255) { *ip_tmp = 0; }
							else { ++(*ip_tmp); }
						}, [&]() {
							if (*ip_tmp == 0) { *ip_tmp = 255; }
							else { --(*ip_tmp); }
							});
				}
			}
			{
				int yp1 = y1p + DrawParts->GetUIY(200);
				if (WindowSystem::SetMsgClickBox(xp + xs - DrawParts->GetUIY(120), yp1, xp + xs, yp1 + LineHeight * 2, LineHeight, Gray75, false, "Set")) {
					m_NewWorkSettings.SetBack(this->m_NetSetting);
					m_NewWorkSettings.Save();
					ReadyConnect(m_NewWorkSettings.Get(this->m_NewWorkSettings.GetSize() - 1));
				}
			}
			break;
		case BrowserSequence::Ready:
			WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "通信中!");
			{
				int yp1 = y1p;
				WindowSystem::SetMsg(xp, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "種別[%s]", this->m_IsClient ? "クライアント" : "サーバー"); yp1 += LineHeight;
				WindowSystem::SetMsg(xp, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "使用ポート[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + NetWork::Player_num - 1); yp1 += LineHeight;
				WindowSystem::SetMsg(xp, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4); yp1 += LineHeight;
			}
			break;
		default:
			break;
		}
		m_SeqFirst = (Prev != this->m_Sequence);
	}
};
