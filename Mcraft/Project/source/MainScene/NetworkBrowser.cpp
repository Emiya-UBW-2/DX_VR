#pragma warning(disable:4464)
#include	"NetworkBrowser.hpp"

const FPS_n2::NetWorkBrowser* SingletonBase<FPS_n2::NetWorkBrowser>::m_Singleton = nullptr;

namespace FPS_n2 {
	void NetWorkBrowser::Init(void) noexcept {
		this->m_Sequence = BrowserSequence::SelMode;
	}
	void NetWorkBrowser::Draw(void) noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();

		int xp, yp, xs, ys;
		xp = 100;
		yp = 250;
		xs = 500;
		ys = 300;

		//�����_
		auto AddSubBox = [&](int xp, int yp, std::function<void()> UpFunc, std::function<void()> DownFunc) {
			int xp1, yp1;
			int xp2, yp2;
			{
				xp1 = xp - 25;
				yp1 = yp - 25 - LineHeight / 2;
				xp2 = xp + 25;
				yp2 = yp1 + LineHeight;
				if (WindowSystem::SetMsgClickBox(xp1, yp1, xp2, yp2, LineHeight, Gray75, true, true, "+")) {
					UpFunc();
				}
			}
			{
				yp1 += 50;
				yp2 += 50;
				if (WindowSystem::SetMsgClickBox(xp1, yp1, xp2, yp2, LineHeight, Gray75, true, true, "-")) {
					DownFunc();
				}
			}
			};
		//
		{
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp - 10, yp - 10, xp + xs + 10, yp + ys + 10, Gray25, true);//�w�i
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp + xs, yp, White, Black, " %d/%d", static_cast<int>(this->m_Sequence), static_cast<int>(NetWork::NetWorkSequence::MainGame));
		}
		auto Prev = this->m_Sequence;

		int y1p = yp + 50;
		switch (this->m_Sequence) {
		case BrowserSequence::SelMode:
			if (WindowSystem::SetMsgClickBox(xp, y1p, xp + xs, y1p + LineHeight * 2, LineHeight, Gray75, false, true, "�N���C�A���g�ɂȂ�")) {
				BeClient();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			if (WindowSystem::SetMsgClickBox(xp, y1p + 50, xp + xs, y1p + 50 + LineHeight * 2, LineHeight, Gray75, false, true, "�T�[�o�[�ɂȂ�")) {
				BeServerPlayer();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			if (WindowSystem::SetMsgClickBox(xp, y1p + 100, xp + xs, y1p + 100 + LineHeight * 2, LineHeight, Gray75, false, true, "�T�[�o�[�ɂȂ�(��v���C���[)")) {
				BeServer();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			//
			if (WindowSystem::SetMsgClickBox(xp, y1p + 150, xp + xs, y1p + 150 + LineHeight * 2, LineHeight, Gray75, false, true, "�N���C�A���g�ő��v���C")) {
				BeClient();
				this->m_NewWorkSettings.Load();
				ReadyConnect(this->m_NewWorkSettings.Get(0));
			}
			if (WindowSystem::SetMsgClickBox(xp, y1p + 200, xp + xs, y1p + 200 + LineHeight * 2, LineHeight, Gray75, false, true, "�T�[�o�[�ő��v���C")) {
				BeServerPlayer();
				this->m_NewWorkSettings.Load();
				ReadyConnect(this->m_NewWorkSettings.Get(0));
			}
			break;
		case BrowserSequence::CheckPreset:
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp, White, Black, "�v���Z�b�g�ݒ�");
			if (this->m_SeqFirst) {
				this->m_NewWorkSettings.Load();
			}
			for (int loop = 0, Num = this->m_NewWorkSettings.GetSize(); loop < Num + 1; ++loop) {
				if (loop < Num) {
					auto& n = this->m_NewWorkSettings.Get(loop);
					if (WindowSystem::SetMsgClickBox(xp, y1p + 50 * loop, xp + xs, y1p + 50 * loop + LineHeight * 2, LineHeight, Gray75, false, true, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
						ReadyConnect(n);
						break;
					}
				}
				else {
					if (WindowSystem::SetMsgClickBox(xp, y1p + 50 * loop, xp + xs, y1p + 50 * loop + LineHeight * 2, LineHeight, Gray75, false, true, "�ݒ��ǉ�����")) {
						this->m_Sequence = BrowserSequence::SetNewData;
						break;
					}
				}
			}
			break;
		case BrowserSequence::SetNewData:
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp, White, Black, "�V�K�ݒ�");
			if (this->m_SeqFirst) {
				this->m_NewWorkSettings.AddBack();
			}
			{
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp + xs / 2, y1p, White, Black, "�|�[�g=[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + NetWork::Player_num - 1);
				AddSubBox(xp + xs / 2, y1p + LineHeight / 2, [&]() { ++this->m_NetSetting.UsePort; }, [&]() { --this->m_NetSetting.UsePort; });
			}
			if (this->m_IsClient) {//�T�[�o-�͂���Ȃ�
				int yp1 = y1p + 100;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp + xs / 2, yp1, White, Black, "IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4);
				for (int loop = 0; loop < 4; ++loop) {
					auto* ip_tmp = &this->m_NetSetting.IP.d1;
					switch (loop) {
					case 0:ip_tmp = &this->m_NetSetting.IP.d1; break;
					case 1:ip_tmp = &this->m_NetSetting.IP.d2; break;
					case 2:ip_tmp = &this->m_NetSetting.IP.d3; break;
					case 3:ip_tmp = &this->m_NetSetting.IP.d4; break;
					}
					AddSubBox(xp + xs / 2 + 70 * (loop - 2) + 35, yp1 + LineHeight / 2,
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
				int yp1 = y1p + 200;
				if (WindowSystem::SetMsgClickBox(xp + xs - 120, yp1, xp + xs, yp1 + LineHeight * 2, LineHeight, Gray75, false, true, "Set")) {
					this->m_NewWorkSettings.SetBack(this->m_NetSetting);
					this->m_NewWorkSettings.Save();
					ReadyConnect(this->m_NewWorkSettings.Get(this->m_NewWorkSettings.GetSize() - 1));
				}
			}
			break;
		case BrowserSequence::Ready:
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp, White, Black, "�ʐM��!");
			{
				int yp1 = y1p;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp1, White, Black, "���[%s]", this->m_IsClient ? "�N���C�A���g" : "�T�[�o�["); yp1 += LineHeight;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp1, White, Black, "�g�p�|�[�g[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + NetWork::Player_num - 1); yp1 += LineHeight;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp1, White, Black, "IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4); yp1 += LineHeight;
			}
			break;
		default:
			break;
		}
		this->m_SeqFirst = (Prev != this->m_Sequence);
	}
};
