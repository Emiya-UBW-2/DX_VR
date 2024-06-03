#include	"NetworkBrowser.hpp"
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		void NetWorkBrowser::Draw(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* Fonts = FontPool::Instance();
			unsigned int color = Red;

			int xp, yp, xs, ys;
			xp = y_r(100);
			yp = y_r(250);
			xs = y_r(500);
			ys = y_r(300);

			int y_h = y_r(30);
			//ラムダ
			auto AddSubBox = [&](int xp1, int yp1, std::function<void()> UpFunc, std::function<void()> DownFunc) {
				int xp2, yp2;
				{
					xp2 = xp1 + y_r(50);
					yp2 = yp1 + y_h;
					bool into = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray75;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "+");
					if (into) {
						if (Pad->GetMouseClick().repeat()) {
							UpFunc();
						}
					}
				}
				{
					yp1 += y_r(50);
					yp2 += y_r(50);
					bool into = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray75;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "-");
					if (into) {
						if (Pad->GetMouseClick().repeat()) {
							DownFunc();
						}
					}
				}
			};
			//
			{
				DrawBox(xp - y_r(10), yp - y_r(10), xp + xs + y_r(10), yp + ys + y_r(10), Gray25, TRUE);//背景
				WindowSystem::SetMsgBox(xp, yp, xp + xs, yp + y_h, Black, " %d/%d", (int)this->m_Sequence, (int)SequenceEnum::MainGame);
				//ログ
				{
					int xp1, yp1;
					xp1 = xp;
					yp1 = yp + ys + y_r(10) + y_r(10);
					if (this->m_Sequence > SequenceEnum::SelMode) {
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "種別[%s]", this->m_IsClient ? "クライアント" : "サーバー"); yp1 += y_h;
					}
					if (this->m_Sequence > SequenceEnum::Set_Port) {
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "使用ポート[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1); yp1 += y_h;
					}
					if (this->m_Sequence > SequenceEnum::SetTick) {
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "ティックレート[%4.1f]", Frame_Rate / this->m_Tick); yp1 += y_h;
					}
					if (this->m_Sequence > SequenceEnum::SetIP) {
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4); yp1 += y_h;
					}
				}
			}
			auto Prev = this->m_Sequence;
			switch (this->m_Sequence) {
				case SequenceEnum::SelMode:
					if (WindowSystem::SetMsgClickBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, 1.f, Gray75, "クライアントになる")) {
						this->m_IsClient = true;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					if (WindowSystem::SetMsgClickBox(xp, yp + y_r(100), xp + xs, yp + y_r(100) + y_h, Gray75, "サーバーになる")) {
						this->m_IsClient = false;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					break;
				case SequenceEnum::CheckPreset:
					WindowSystem::SetMsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, Black, "プリセット設定");
					for (int i = 0; i < this->m_NewWorkSetting.GetSize(); i++) {
						auto n = this->m_NewWorkSetting.Get(i);
						if (WindowSystem::SetMsgClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, Gray75, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
							this->m_NewSetting.UsePort = n.UsePort;
							this->m_NewSetting.IP = n.IP;
							this->m_Sequence = SequenceEnum::SetTick;
							m_NewWorkSelect = i;
							break;
						}
					}
					{
						int i = this->m_NewWorkSetting.GetSize();
						if (WindowSystem::SetMsgClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, Gray75, "設定を追加する")) {
							m_NewWorkSetting.Add();
							m_NewWorkSelect = i;
							this->m_Sequence = SequenceEnum::Set_Port;
						}
					}
					break;
				case SequenceEnum::Set_Port://ポート
					WindowSystem::SetMsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, Black, "ポート=[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_NewSetting.UsePort++; }, [&]() { this->m_NewSetting.UsePort--; });
					if (WindowSystem::SetMsgClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, Gray75, "Set")) {
						this->m_Sequence = SequenceEnum::SetIP;//サーバ-は一応いらない
					}
					break;
				case SequenceEnum::SetIP://IP
					WindowSystem::SetMsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4);
					for (int i = 0; i < 4; i++) {
						auto* ip_tmp = &this->m_NewSetting.IP.d1;
						switch (i) {
							case 0:ip_tmp = &this->m_NewSetting.IP.d1; break;
							case 1:ip_tmp = &this->m_NewSetting.IP.d2; break;
							case 2:ip_tmp = &this->m_NewSetting.IP.d3; break;
							case 3:ip_tmp = &this->m_NewSetting.IP.d4; break;
						}
						AddSubBox(y_r(100 + 70 * i), yp + y_r(100),
								  [&]() {
									  if (*ip_tmp == 255) { *ip_tmp = 0; }
									  else { (*ip_tmp)++; }
								  }, [&]() {
									  if (*ip_tmp == 0) { *ip_tmp = 255; }
									  else { (*ip_tmp)--; }
								  });
					}
					if (WindowSystem::SetMsgClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, Gray75, "Set")) {
						this->m_Sequence = SequenceEnum::SetTick;
						m_NewWorkSetting.Set(this->m_NewWorkSelect, this->m_NewSetting);
						m_NewWorkSetting.Save();
					}
					break;
				case SequenceEnum::SetTick:
					WindowSystem::SetMsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, Black, "ティック=[%4.1f]", Frame_Rate / this->m_Tick);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
					if (WindowSystem::SetMsgClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, Gray75, "Set")) {
						this->m_Sequence = SequenceEnum::Matching;
					}
					break;
				case SequenceEnum::Matching:
					WindowSystem::SetMsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, Black, "他プレイヤー待機中…");
					for (int i = 0; i < Player_num; i++) {
						bool isActive = (this->m_IsClient) ? this->m_ClientCtrl.GetServerDataCommon().PlayerData[i].IsActive : this->m_ServerCtrl.GetServerData().PlayerData[i].IsActive;
						int yp1 = yp + y_r(50) + y_r(35)*(i + 1);
						color = isActive ? Black : Gray75;
						DrawBox(y_r(200), yp1, y_r(200) + y_r(300), yp1 + y_h, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(200), yp1, White, Black, "Player");
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(200) + y_r(300), yp1, White, Black, (isActive ? "〇" : ""));
					}
					break;
				case SequenceEnum::MainGame:
					WindowSystem::SetMsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, Black, "通信中!");
					break;
				default:
					break;
			}
			m_SeqFirst = (Prev != this->m_Sequence);
		}
	};
};
