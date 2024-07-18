#pragma warning(disable:4464)
#include	"NetworkBrowser.hpp"
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//�ʐM
		void NetWorkBrowser::Draw(void) noexcept {
			//*
			auto* DrawParts = DXDraw::Instance();
			auto* Pad = PadControl::Instance();
			auto* Fonts = FontPool::Instance();
			unsigned int color = Red;

			int xp, yp, xs, ys;
			xp = DrawParts->GetUIY(100);
			yp = DrawParts->GetUIY(250);
			xs = DrawParts->GetUIY(500);
			ys = DrawParts->GetUIY(300);

			int y_h = DrawParts->GetUIY(30);
			//�����_
			auto AddSubBox = [&](int xp1, int yp1, std::function<void()> UpFunc, std::function<void()> DownFunc) {
				int xp2, yp2;
				{
					xp2 = xp1 + DrawParts->GetUIY(50);
					yp2 = yp1 + y_h;
					bool into = IntoMouse(xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray75;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "+");
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
					Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "-");
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
				WindowSystem::SetMsgBox(xp, yp, xp + xs, yp + y_h, LineHeight, Black, " %d/%d", static_cast<int>(this->m_Sequence), static_cast<int>(SequenceEnum::MainGame));
				//���O
				{
					int xp1, yp1;
					xp1 = xp;
					yp1 = yp + ys + DrawParts->GetUIY(10) + DrawParts->GetUIY(10);
					if (this->m_Sequence > SequenceEnum::SelMode) {
						Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "���[%s]", this->m_IsClient ? "�N���C�A���g" : "�T�[�o�["); yp1 += y_h;
					}
					if (this->m_Sequence > SequenceEnum::Set_Port) {
						Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "�g�p�|�[�g[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1); yp1 += y_h;
					}
					if (this->m_Sequence > SequenceEnum::SetTick) {
						Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "�e�B�b�N���[�g[%4.1f]", Frame_Rate / this->m_Tick); yp1 += y_h;
					}
					if (this->m_Sequence > SequenceEnum::SetIP) {
						Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4); yp1 += y_h;
					}
				}
			}
			auto Prev = this->m_Sequence;
			switch (this->m_Sequence) {
				case SequenceEnum::SelMode:
					if (WindowSystem::SetMsgClickBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Gray75, "�N���C�A���g�ɂȂ�")) {
						this->m_IsClient = true;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					if (WindowSystem::SetMsgClickBox(xp, yp + DrawParts->GetUIY(100), xp + xs, yp + DrawParts->GetUIY(100) + y_h, LineHeight, Gray75, "�T�[�o�[�ɂȂ�")) {
						this->m_IsClient = false;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					break;
				case SequenceEnum::CheckPreset:
					WindowSystem::SetMsgBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Black, "�v���Z�b�g�ݒ�");
					for (int i = 0; i < this->m_NewWorkSetting.GetSize(); i++) {
						auto n = this->m_NewWorkSetting.Get(i);
						if (WindowSystem::SetMsgClickBox(xp, yp + DrawParts->GetUIY(50)*(i + 2), xp + xs, yp + DrawParts->GetUIY(50)*(i + 2) + y_h, LineHeight, Gray75, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
							this->m_NewSetting.UsePort = n.UsePort;
							this->m_NewSetting.IP = n.IP;
							this->m_Sequence = SequenceEnum::SetTick;
							m_NewWorkSelect = i;
							break;
						}
					}
					{
						int i = this->m_NewWorkSetting.GetSize();
						if (WindowSystem::SetMsgClickBox(xp, yp + DrawParts->GetUIY(50)*(i + 2), xp + xs, yp + DrawParts->GetUIY(50)*(i + 2) + y_h, LineHeight, Gray75, "�ݒ��ǉ�����")) {
							m_NewWorkSetting.Add();
							m_NewWorkSelect = i;
							this->m_Sequence = SequenceEnum::Set_Port;
						}
					}
					break;
				case SequenceEnum::Set_Port://�|�[�g
					WindowSystem::SetMsgBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Black, "�|�[�g=[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1);
					AddSubBox(xp, yp + DrawParts->GetUIY(100), [&]() { this->m_NewSetting.UsePort++; }, [&]() { this->m_NewSetting.UsePort--; });
					if (WindowSystem::SetMsgClickBox(DrawParts->GetUIY(380), yp + DrawParts->GetUIY(100), DrawParts->GetUIY(380) + DrawParts->GetUIY(120), yp + DrawParts->GetUIY(100) + y_h, LineHeight, Gray75, "Set")) {
						this->m_Sequence = SequenceEnum::SetIP;//�T�[�o-�͈ꉞ����Ȃ�
					}
					break;
				case SequenceEnum::SetIP://IP
					WindowSystem::SetMsgBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4);
					for (int i = 0; i < 4; i++) {
						auto* ip_tmp = &this->m_NewSetting.IP.d1;
						switch (i) {
							case 0:ip_tmp = &this->m_NewSetting.IP.d1; break;
							case 1:ip_tmp = &this->m_NewSetting.IP.d2; break;
							case 2:ip_tmp = &this->m_NewSetting.IP.d3; break;
							case 3:ip_tmp = &this->m_NewSetting.IP.d4; break;
						}
						AddSubBox(DrawParts->GetUIY(100 + 70 * i), yp + DrawParts->GetUIY(100),
								 [&]() {
									 if (*ip_tmp == 255) { *ip_tmp = 0; }
									 else { (*ip_tmp)++; }
								 }, [&]() {
									 if (*ip_tmp == 0) { *ip_tmp = 255; }
									 else { (*ip_tmp)--; }
								 });
					}
					if (WindowSystem::SetMsgClickBox(DrawParts->GetUIY(380), yp + DrawParts->GetUIY(100), DrawParts->GetUIY(380) + DrawParts->GetUIY(120), yp + DrawParts->GetUIY(100) + y_h, LineHeight, Gray75, "Set")) {
						this->m_Sequence = SequenceEnum::SetTick;
						m_NewWorkSetting.Set(this->m_NewWorkSelect, this->m_NewSetting);
						m_NewWorkSetting.Save();
					}
					break;
				case SequenceEnum::SetTick:
					WindowSystem::SetMsgBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Black, "�e�B�b�N=[%4.1f]", Frame_Rate / this->m_Tick);
					AddSubBox(xp, yp + DrawParts->GetUIY(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
					if (WindowSystem::SetMsgClickBox(DrawParts->GetUIY(380), yp + DrawParts->GetUIY(100), DrawParts->GetUIY(380) + DrawParts->GetUIY(120), yp + DrawParts->GetUIY(100) + y_h, LineHeight, Gray75, "Set")) {
						this->m_Sequence = SequenceEnum::Matching;
					}
					break;
				case SequenceEnum::Matching:
					WindowSystem::SetMsgBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Black, "���v���C���[�ҋ@���c");
					for (int i = 0; i < Player_num; i++) {
						bool isActive = (this->m_IsClient) ? this->m_ClientCtrl.GetServerData().PlayerData[i].GetIsActive() : this->m_ServerCtrl.GetServerData().PlayerData[i].GetIsActive();
						int yp1 = yp + DrawParts->GetUIY(50) + DrawParts->GetUIY(35)*(i + 1);
						color = isActive ? Black : Gray75;
						DrawBox(DrawParts->GetUIY(200), yp1, DrawParts->GetUIY(200) + DrawParts->GetUIY(300), yp1 + y_h, color, TRUE);
						Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, DrawParts->GetUIY(200), yp1, White, Black, "Player");
						Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, DrawParts->GetUIY(200) + DrawParts->GetUIY(300), yp1, White, Black, (isActive ? "�Z" : ""));
					}
					break;
				case SequenceEnum::MainGame:
					WindowSystem::SetMsgBox(xp, yp + DrawParts->GetUIY(50), xp + xs, yp + DrawParts->GetUIY(50) + y_h, LineHeight, Black, "�ʐM��!");
					break;
				default:
					break;
			}
			m_SeqFirst = (Prev != this->m_Sequence);

			Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, DrawParts->GetUIY(0), DrawParts->GetUIY(64),
																		  White, Black, "Ping[%f]", GetPing());

			Fonts->Get(FontPool::FontType::MS_Gothic, y_h, 1)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, DrawParts->GetUIY(0), DrawParts->GetUIY(64) + y_h,
																		  White, Black, "MyID[%d]", m_PlayerNet.GetMyPlayerID());
		}
	};
};
