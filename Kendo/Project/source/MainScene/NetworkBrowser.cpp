#pragma warning(disable:4464)
#include	"NetworkBrowser.hpp"
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//�ʐM
		void NetWorkController::Init(bool IsClient, int Port, const IPDATA& ip) noexcept {
			m_PlayerNet.Int(static_cast<NetTime>((this->m_Tick*1000.f / Frame_Rate)*1000.f));
			this->m_IsClient = IsClient;
			if (this->m_IsClient) {
				m_ClientCtrl.Init(Port, ip);
			}
			else {
				m_ServerCtrl.Init(Port, true);
			}
			this->m_Sequence = NetWorkSequence::Matching;
		}
		void NetWorkController::Update(void) noexcept {
			//���[�J���f�[�^�X�V
			{
				if (this->m_IsClient) {
					//�X�V�Ԋu
					if (this->m_Sequence == NetWorkSequence::Matching) {
						m_LocalData.InitTime();
					}
					else {
						m_LocalData.UpdateTime();
					}
					m_PlayerNet.SetLocalData(m_LocalData);
					if (this->m_Sequence == NetWorkSequence::Matching) {
						//������PlayerID����
						if (m_ClientCtrl.CanGetMyID()) {
							m_PlayerNet.SetLocalPlayerID(m_ClientCtrl.GetMyID());
						}
					}
					m_PlayerNet.SetLocalPlayerFlag(NetAttribute::IsActive, m_ClientCtrl.CanGetMyID());
				}
				else if (this->m_ServerCtrl.GetIsServerPlayer()) {
					//�X�V�Ԋu
					if (this->m_Sequence == NetWorkSequence::Matching) {
						m_LocalData.InitTime();
					}
					else {
						m_LocalData.UpdateTime();
					}
					m_PlayerNet.SetLocalData(m_LocalData);
					if (this->m_Sequence == NetWorkSequence::Matching) {
						//������PlayerID����
						m_PlayerNet.SetLocalPlayerID((PlayerID)0);
					}
				}
			}
			//
			if (this->m_Sequence >= NetWorkSequence::Matching) {
				bool IsUpdateTick = m_PlayerNet.UpdateTick();
				LONGLONG Ping = MAXLONGLONG;
				if (this->m_IsClient) {
					if (this->m_ClientCtrl.Execute(m_PlayerNet.GetLocalPlayerData(), IsUpdateTick)) {
						this->m_Sequence = NetWorkSequence::MainGame;
					}
					if (IsUpdateTick) {
						Ping = m_PlayerNet.Update(this->m_ClientCtrl.GetServerData());
					}
				}
				else {
					if (this->m_ServerCtrl.Execute(m_PlayerNet.GetLocalPlayerData(), IsUpdateTick)) {
						this->m_Sequence = NetWorkSequence::MainGame;
					}
					if (IsUpdateTick) {
						Ping = m_PlayerNet.Update(this->m_ServerCtrl.GetServerData());
					}
				}
				if (IsUpdateTick) {
					if (Ping != MAXLONGLONG) {
						if (Ping != 0) {
							CalcPing(Ping);
						}
					}
					else {
						//���X�g
					}
				}
			}
		}
		//
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

			//�����_
			auto AddSubBox = [&](int xp, int yp, std::function<void()> UpFunc, std::function<void()> DownFunc) {
				auto* Pad = PadControl::Instance();
				unsigned int color = Red;
				int xp1, yp1;
				int xp2, yp2;
				{
					xp1 = xp - DrawParts->GetUIY(25);
					yp1 = yp - DrawParts->GetUIY(25) - LineHeight / 2;
					xp2 = xp + DrawParts->GetUIY(25);
					yp2 = yp1 + LineHeight;
					bool into = IntoMouse(xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray75;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					WindowSystem::SetMsg(xp1 + (xp2 - xp1) / 2, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "+");
					if (into) {
						if (Pad->GetMouseClick().repeat()) {
							UpFunc();
						}
					}
				}
				{
					yp1 += DrawParts->GetUIY(50);
					yp2 += DrawParts->GetUIY(50);
					bool into = IntoMouse(xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray75;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					WindowSystem::SetMsg(xp1 + (xp2 - xp1) / 2, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "-");
					if (into) {
						if (Pad->GetMouseClick().repeat()) {
							DownFunc();
						}
					}
				}
			};
			//
			{
				DrawBox(xp - DrawParts->GetUIY(10), yp - DrawParts->GetUIY(10), xp + xs + DrawParts->GetUIY(10), yp + ys + DrawParts->GetUIY(10), Gray25, TRUE);//�w�i
				WindowSystem::SetMsg(xp + xs, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, " %d/%d", static_cast<int>(this->m_Sequence), static_cast<int>(NetWorkSequence::MainGame));
			}
			auto Prev = this->m_Sequence;

			int y1p = yp + DrawParts->GetUIY(50);
			switch (this->m_Sequence) {
				case BrowserSequence::SelMode:
					if (WindowSystem::SetMsgClickBox(xp, y1p, xp + xs, y1p + LineHeight * 2, LineHeight, Gray75, "�N���C�A���g�ɂȂ�")) {
						this->m_IsClient = true;
						this->m_Sequence = BrowserSequence::CheckPreset;
					}
					if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(50), xp + xs, y1p + DrawParts->GetUIY(50) + LineHeight * 2, LineHeight, Gray75, "�T�[�o�[�ɂȂ�")) {
						this->m_IsClient = false;
						this->m_Sequence = BrowserSequence::CheckPreset;
					}
					break;
				case BrowserSequence::CheckPreset:
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "�v���Z�b�g�ݒ�");
					if (m_SeqFirst) {
						m_NewWorkSettings.Load();
					}
					for (int i = 0; i <= this->m_NewWorkSettings.GetSize(); i++) {
						if (i < this->m_NewWorkSettings.GetSize()) {
							auto& n = this->m_NewWorkSettings.Get(i);
							if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(50)*i, xp + xs, y1p + DrawParts->GetUIY(50)*i + LineHeight * 2, LineHeight, Gray75, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
								this->m_NetSetting = n;
								this->m_Sequence = BrowserSequence::Ready;
								break;
							}
						}
						else {
							if (WindowSystem::SetMsgClickBox(xp, y1p + DrawParts->GetUIY(50)*i, xp + xs, y1p + DrawParts->GetUIY(50)*i + LineHeight * 2, LineHeight, Gray75, "�ݒ��ǉ�����")) {
								this->m_Sequence = BrowserSequence::SetNewData;
								break;
							}
						}
					}
					break;
				case BrowserSequence::SetNewData:
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "�V�K�ݒ�");
					if (m_SeqFirst) {
						m_NewWorkSettings.AddBack();
					}
					{
						WindowSystem::SetMsg(xp + xs / 2, y1p + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "�|�[�g=[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + Player_num - 1);
						AddSubBox(xp + xs / 2, y1p + LineHeight / 2, [&]() { this->m_NetSetting.UsePort++; }, [&]() { this->m_NetSetting.UsePort--; });
					}
					if (this->m_IsClient) {//�T�[�o-�͂���Ȃ�
						int yp1 = y1p + DrawParts->GetUIY(100);
						WindowSystem::SetMsg(xp + xs / 2, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4);
						for (int i = 0; i < 4; i++) {
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
										  else { (*ip_tmp)++; }
									  }, [&]() {
										  if (*ip_tmp == 0) { *ip_tmp = 255; }
										  else { (*ip_tmp)--; }
									  });
						}
					}
					{
						int yp1 = y1p + DrawParts->GetUIY(200);
						if (WindowSystem::SetMsgClickBox(xp + xs - DrawParts->GetUIY(120), yp1, xp + xs, yp1 + LineHeight * 2, LineHeight, Gray75, "Set")) {
							m_NewWorkSettings.SetBack(this->m_NetSetting);
							m_NewWorkSettings.Save();
							this->m_Sequence = BrowserSequence::Ready;
						}
					}
					break;
				case BrowserSequence::Ready:
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "�ʐM��!");
					{
						int yp1 = y1p;
						WindowSystem::SetMsg(xp, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "���[%s]", this->m_IsClient ? "�N���C�A���g" : "�T�[�o�["); yp1 += LineHeight;
						WindowSystem::SetMsg(xp, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "�g�p�|�[�g[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + Player_num - 1); yp1 += LineHeight;
						WindowSystem::SetMsg(xp, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, "IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4); yp1 += LineHeight;
					}
					break;
				default:
					break;
			}
			m_SeqFirst = (Prev != this->m_Sequence);
		}
	};
};
