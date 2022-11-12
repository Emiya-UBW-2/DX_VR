#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//�ʐM
		enum class SequenceEnum {
			SelMode,
			CheckPreset,
			Set_Port,
			SetIP,
			SetTick,
			Matching,
			MainGame,
		};
		class NetWorkBrowser {
			//�T�[�o�[��p
			ServerControl			m_ServerCtrl;																//
			//�N���C�A���g��p
			ClientControl			m_ClientCtrl;																//
			//����
			bool					m_IsClient{ true };															//
			SequenceEnum			m_Sequence{ SequenceEnum::SelMode };										//
			bool					m_SeqFirst{ false };														//
			float					m_Tick{ 1.f };																//
			NewWorkSetting			m_NewWorkSetting;															//
			int						m_NewWorkSelect{ 0 };														//
			NewSetting				m_NewSetting;																//
			double					m_ClientFrame{ 0.0 };
			float					m_Ping{ 0.f };

			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };
		public:
			//const auto& Get(void) const noexcept { return m_SeqFirst; }
			const auto& GetClient(void) const noexcept { return this->m_IsClient; }
			const auto& GetSequence(void) const noexcept { return this->m_Sequence; }
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetMyPlayer().ID : this->m_ServerCtrl.GetMyPlayer().ID; }
			const auto		GetNowServerPlayerData(int ID, bool isYradReset) noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetNowServerPlayerData(ID, isYradReset) : this->m_ServerCtrl.GetNowServerPlayerData(ID, isYradReset); }
			void			GetRecvData(int ID, double ServerFrame) noexcept {
				if ((this->m_IsClient) ? this->m_ClientCtrl.GetRecvData(ID) : this->m_ServerCtrl.GetRecvData(ID)) {
					this->m_Ping = (float)(this->m_ClientFrame - ServerFrame)*1000.f;
				}
				printfDx("ping %lf \n", this->m_Ping);
			}
		public:
			void Init(void) noexcept {
				m_NewWorkSetting.Load();
				m_NewWorkSelect = 0;
			}
			void FirstExecute(const InputControl& MyInput, const SendInfo& SendMove) noexcept {
				this->m_LeftClick.GetInput((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
				if (!this->m_LeftClick.press()) {
					this->m_LeftPressTimer = 0.f;
				}

				if (this->m_IsClient) {
					m_ClientCtrl.SetMyPlayer(MyInput, SendMove.m_Pos, SendMove.m_Vec, SendMove.m_rad, this->m_ClientFrame, SendMove.m_Damage, SendMove.m_DamageSwitch);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ClientCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, this->m_NewSetting.IP);
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ClientCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
				//�T�[�o�[
				else {
					m_ServerCtrl.SetMyPlayer(MyInput, SendMove.m_Pos, SendMove.m_Vec, SendMove.m_rad, this->m_ClientFrame, SendMove.m_Damage, SendMove.m_DamageSwitch);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ServerCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, IPDATA());
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ServerCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
			}
			void LateExecute(void) noexcept {
				if (this->m_Sequence == SequenceEnum::MainGame) {
					this->m_ClientFrame += 1.0 / (double)FPS;
				}
				else {
					this->m_ClientFrame = 0.0;
				}
			}
			void Draw(void) noexcept {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();
				//auto* DrawParts = DXDraw::Instance();
				auto* Fonts = FontPool::Instance();
				auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				//auto White = GetColor(212, 255, 239);
				auto Gray = GetColor(64, 64, 64);
				auto Black = GetColor(0, 0, 0);
				unsigned int color = Red;

				int xp, yp, xs, ys;
				xp = y_r(100);
				yp = y_r(250);
				xs = y_r(500);
				ys = y_r(300);

				int y_h = y_r(30);
				//bool Mid_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
				//bool Right_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
				int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
				GetMousePoint(&mx, &my);
				//�����_
				auto MsgBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
					color = Black;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, String, args...);
				};
				auto ClickBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
					bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, String, args...);
					return into && this->m_LeftClick.trigger();
				};
				auto AddSubBox = [&](int xp1, int yp1, std::function<void()> UpFunc, std::function<void()> DownFunc) {
					int xp2, yp2;
					{
						xp2 = xp1 + y_r(50);
						yp2 = yp1 + y_h;
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "+", GetColor(255, 255, 255));
						if (into) {
							if (this->m_LeftClick.trigger()) {
								UpFunc();
							}
							if (this->m_LeftClick.press()) {
								m_LeftPressTimer += 1.f / FPS;
								if (this->m_LeftPressTimer > 0.5f) {
									UpFunc();
								}
							}
						}
					}
					{
						yp1 += y_r(50);
						yp2 += y_r(50);
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "-", GetColor(255, 255, 255));
						if (into) {
							if (this->m_LeftClick.trigger()) {
								DownFunc();
							}
							if (this->m_LeftClick.press()) {
								m_LeftPressTimer += 1.f / FPS;
								if (this->m_LeftPressTimer > 0.5f) {
									DownFunc();
								}
							}
						}
					}
				};
				//
				{
					DrawBox(xp - y_r(10), yp - y_r(10), xp + xs + y_r(10), yp + ys + y_r(10), GetColor(164, 164, 164), TRUE);//�w�i
					MsgBox(xp, yp, xp + xs, yp + y_h, " %d/%d", (int)this->m_Sequence, (int)SequenceEnum::MainGame);
					//���O
					{
						int xp1, yp1;
						xp1 = xp;
						yp1 = yp + ys + y_r(10) + y_r(10);
						if (this->m_Sequence > SequenceEnum::SelMode) {
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "���[%s]", this->m_IsClient ? "�N���C�A���g" : "�T�[�o�["); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::Set_Port) {
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "�g�p�|�[�g[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::SetTick) {
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "�e�B�b�N���[�g[%4.1f]", Frame_Rate / this->m_Tick); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::SetIP) {
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4); yp1 += y_h;
						}
					}
				}
				auto Prev = this->m_Sequence;
				switch (this->m_Sequence) {
				case SequenceEnum::SelMode:
					if (ClickBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "�N���C�A���g�ɂȂ�")) {
						this->m_IsClient = true;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					if (ClickBox(xp, yp + y_r(100), xp + xs, yp + y_r(100) + y_h, "�T�[�o�[�ɂȂ�")) {
						this->m_IsClient = false;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					break;
				case SequenceEnum::CheckPreset:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "�v���Z�b�g�ݒ�");
					for (int i = 0; i < this->m_NewWorkSetting.GetSize(); i++) {
						auto n = this->m_NewWorkSetting.Get(i);
						if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
							this->m_NewSetting.UsePort = n.UsePort;
							this->m_NewSetting.IP = n.IP;
							this->m_Sequence = SequenceEnum::SetTick;
							m_NewWorkSelect = i;
							break;
						}
					}
					{
						int i = this->m_NewWorkSetting.GetSize();
						if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "�ݒ��ǉ�����")) {
							m_NewWorkSetting.Add();
							m_NewWorkSelect = i;
							this->m_Sequence = SequenceEnum::Set_Port;
						}
					}
					break;
				case SequenceEnum::Set_Port://�|�[�g
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "�|�[�g=[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_NewSetting.UsePort++; }, [&]() { this->m_NewSetting.UsePort--; });
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::SetIP;//�T�[�o-�͈ꉞ����Ȃ�
					}
					break;
				case SequenceEnum::SetIP://IP
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4);
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
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::SetTick;
						m_NewWorkSetting.Set(this->m_NewWorkSelect, this->m_NewSetting);
						m_NewWorkSetting.Save();
					}
					break;
				case SequenceEnum::SetTick:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "�e�B�b�N=[%4.1f]", Frame_Rate / this->m_Tick);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::Matching;
					}
					break;
				case SequenceEnum::Matching:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "���v���C���[�ҋ@���c");
					for (int i = 0; i < Player_num; i++) {
						bool isActive = (((this->m_IsClient) ? this->m_ClientCtrl.GetServerDataCommon() : this->m_ServerCtrl.GetServerData()).PlayerData[i].IsActive == 1);
						int yp1 = yp + y_r(50) + y_r(35)*(i + 1);
						color = isActive ? Black : Gray;
						DrawBox(y_r(200), yp1, y_r(200) + y_r(300), yp1 + y_h, color, TRUE);
						Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString(y_r(200), yp1, "Player", GetColor(255, 255, 255));
						Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString_RIGHT(y_r(200) + y_r(300), yp1, (isActive ? "�Z" : ""), GetColor(255, 255, 255));
					}
					break;
				case SequenceEnum::MainGame:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "�ʐM��!");
					break;
				default:
					break;
				}
				m_SeqFirst = (Prev != this->m_Sequence);
			}
			void Dispose(void) noexcept {
				m_ServerCtrl.Dispose();
				m_ClientCtrl.Dispose();
			}
		};


		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			static const int		Chara_num = Player_num;
			static const int		Vehicle_num = 0;
			static const int		gun_num = Chara_num;
		private:
			//���\�[�X�֘A
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			SoundHandle				m_Env;
			MV1						hit_pic;					//�e��  
			//�l
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//�|�C���^�[�ʎ���
			//��ԃf�[�^
			std::vector<VhehicleData>	vehc_data;
			std::vector<std::shared_ptr<VehicleClass>> vehicle_Pool;	//�|�C���^�[�ʎ���
			//����֘A
			float					m_EyePosPer_Prone = 0.f;
			float					m_EyePosPer = 0.f;
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			int						m_LookMode{ 0 };
			//UI�֘A
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			GraphHandle				hit_Graph;
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//�e�֘A
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			//
			MATRIX_ref				m_FreeLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			float					m_Rader{ 1.f };
			float					m_Rader_r{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;

			NetWorkBrowser			m_NetWorkBrowser;
			//����
			double					m_ClientFrame{ 0.0 };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			using TEMPSCENE::TEMPSCENE;
			void			Set(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				Set_EnvLight(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_BackGround->Init();
				//
				auto data_t = GetFileNamesInDirectory("data/tank/");
				for (auto& d : data_t) {
					if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						this->vehc_data.resize(this->vehc_data.size() + 1);
						this->vehc_data.back().Set_Pre(d.cFileName);
					}
				}
				for (auto& t : this->vehc_data) { t.Set(); }										//���2
				//
				ObjMngr->Init(this->m_BackGround);
				for (int i = 0; i < Chara_num; i++) {
					//ObjMngr->AddObject(ObjType::Human, "data/Charactor/WinningTicket/");
					//ObjMngr->AddObject(ObjType::Human, "data/Charactor/Sweep/");
					//ObjMngr->AddObject(ObjType::Human, "data/Charactor/Marisa/");
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/WinningTicket/")));
				}
				for (int i = 0; i < Vehicle_num; i++) {
					vehicle_Pool.emplace_back((std::shared_ptr<VehicleClass>&)(*ObjMngr->AddObject(ObjType::Vehicle)));
				}

				for (int i = 0; i < gun_num; i++) {
					ObjMngr->AddObject(ObjType::Gun, "data/gun/AR15/");//MP5K//AR15//AKS74U//Gorushi//Mosin//MEU1911
					ObjMngr->AddObject(ObjType::Gun, "data/gun/MEU1911/");//MP5K//AR15//AKS74U//Gorushi//Mosin//MEU1911
				}
				//���[�h
				SetCreate3DSoundFlag(FALSE);
				this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Env.vol(64);
				//
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic);						//�e�����f��
				//UI
				this->m_UIclass.Set();

				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				//
				TEMPSCENE::Set();
				//Set
				//�l
				for (auto& c : this->character_Pool) {
					size_t index = &c - &this->character_Pool.front();
					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(index)*20.f, 0.f, 0.f);
					auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), false, pos_t, (PlayerID)index);
					c->SetGunPtr(
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 0))),
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 1)))
					);
					if (index == 0) {
						//c->SetUseRealTimePhysics(true);
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Enemy);
					}
				}
				//�o�^
				for (auto& v : this->vehicle_Pool) {
					size_t index = &v - &this->vehicle_Pool.front();
					VECTOR_ref pos_t = VECTOR_ref::vget(600.f + (float)(index)*10.f*Scale_Rate, 0.f, 0.f);
					auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					v->ValueInit(&vehc_data[0], hit_pic, this->m_BackGround->GetBox2Dworld(), (PlayerID)index);
					v->ValueSet(deg2rad(0), deg2rad(90), pos_t);
				}
				//player
				PlayerMngr->Init(Player_num);
				for (int i = 0; i < Player_num; i++) {
					PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i)));
					//PlayerMngr->GetPlayer(i).SetChara(nullptr);
					//PlayerMngr->GetPlayer(i).SetVehicle((std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i)));
					PlayerMngr->GetPlayer(i).SetVehicle(nullptr);
				}
				if (!PlayerMngr->GetPlayer(0).IsRide()) {
					this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetChara()->GetHP();
					this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
				}
				else {
					this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetVehicle()->GetHP();
					this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
				}

				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				//�T�E���h
				auto SE = SoundPool::Instance();
				SE->Add((int)SoundEnum::Shot_Gun, 3, "data/Sound/SE/gun/shot.wav");
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking0 + i, 3, "data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, 9, "data/Sound/SE/move/heart.wav");
				//SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");
				SE->Add((int)SoundEnum::Tank_Shot, 3, "data/Sound/SE/gun/fire/7.wav");
				for (int i = 0; i < 17; i++) {
					SE->Add((int)SoundEnum::Tank_Ricochet, 3, "data/Sound/SE/ricochet/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 2; i++) {
					SE->Add((int)SoundEnum::Tank_Damage, 3, "data/Sound/SE/damage/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::Tank_engine, 10, "data/Sound/SE/engine.wav");
				for (int i = 0; i < 7; i++) {
					SE->Add((int)SoundEnum::Tank_Eject, 3, "data/Sound/SE/gun/reload/eject/" + std::to_string(i) + ".wav", false);
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Tank_Reload, 3, "data/Sound/SE/gun/reload/hand/" + std::to_string(i) + ".wav", false);
				}

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				//SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);

				SE->Get((int)SoundEnum::Tank_Shot).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_engine).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Ricochet).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Damage).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Eject).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Reload).SetVol(0.25f);


				//����
				this->m_FPSActive.Init(true);
				this->m_MouseActive.Init(true);

				this->m_DamageEvents.clear();

				m_NetWorkBrowser.Init();
			}
			//
			bool			Execute(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
#ifdef DEBUG
				//auto* DebugParts = DebugClass::Instance();					//�f�o�b�O
#endif // DEBUG
				//FirstDoingv
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					Chara->LoadReticle();//�v���C���[�ύX������
				}
				//Input,AI
				{
					float cam_per = ((camera_main.fov / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = 0.f, pp_y = 0.f;
					bool look_key = false;
					bool eyechange_key = false;
					bool Lockon_key = false;
					InputControl MyInput;

					if (GetJoypadNum() > 0) {
						DINPUT_JOYSTATE input;
						int padID = DX_INPUT_PAD1;
						GetJoypadInputState(padID);
						switch (GetJoypadType(padID)) {
						case DX_PADTYPE_OTHER:
						case DX_PADTYPE_DUAL_SHOCK_4:
						case DX_PADTYPE_DUAL_SENSE:
						case DX_PADTYPE_SWITCH_JOY_CON_L:
						case DX_PADTYPE_SWITCH_JOY_CON_R:
						case DX_PADTYPE_SWITCH_PRO_CTRL:
						case DX_PADTYPE_SWITCH_HORI_PAD:
							GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
							{
								pp_x = std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f) * cam_per;
								pp_y = std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f) * cam_per;

								float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
								bool w_key = false;
								bool s_key = false;
								bool a_key = false;
								bool d_key = false;
								if (!(input.X == 0 && input.Y == 0)) {
									w_key = (-50.f <= deg && deg <= 50.f);
									a_key = (-140.f <= deg && deg <= -40.f);
									s_key = (130.f <= deg || deg <= -130.f);
									d_key = (40.f <= deg && deg <= 140.f);
								}
								//���_�؂�ւ�
								look_key = (input.Buttons[11] != 0);
								//eyechange_key = (input.Buttons[11]!=0);
								//�\��
								deg = (float)(input.POV[0]) / 100.f;
								bool right_key = (40.f <= deg && deg <= 140.f);
								bool left_key = (220.f <= deg && deg <= 320.f);
								bool up_key = (310.f <= deg || deg <= 50.f);
								bool down_key = (130.f <= deg && deg <= 230.f);
								//�{�^��
								Lockon_key = (input.Buttons[0] != 0);/*��*/
								//_key = (input.Buttons[2] != 0);/*�~*/
								auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
								MyInput.SetInput(
									pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
									pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
									w_key, s_key, a_key, d_key,
									(input.Buttons[10] != 0),
									(input.Buttons[6] != 0), (input.Buttons[7] != 0),
									right_key, left_key, up_key, down_key,
									(input.Buttons[10] != 0), (input.Buttons[5] != 0), (input.Buttons[4] != 0), (input.Buttons[3] != 0)/*��*/, (input.Buttons[1] != 0)/*�Z*/,
									false
								);
							}
							break;
						case DX_PADTYPE_XBOX_360:
						case DX_PADTYPE_XBOX_ONE:
							break;
						default:
							break;
						}
					}
					else {//�L�[�{�[�h
						this->m_MouseActive.GetInput(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
						int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
						if (this->m_MouseActive.on()) {
							if (this->m_MouseActive.trigger()) {
								SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
							}
							GetMousePoint(&mx, &my);
							SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
							SetMouseDispFlag(FALSE);
						}
						else {
							SetMouseDispFlag(TRUE);
						}
						pp_x = std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per;
						pp_y = std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per;
						look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on();
						eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;
						Lockon_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						MyInput.SetInput(
							pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
							pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
							(CheckHitKeyWithCheck(KEY_INPUT_W) != 0), (CheckHitKeyWithCheck(KEY_INPUT_S) != 0), (CheckHitKeyWithCheck(KEY_INPUT_A) != 0), (CheckHitKeyWithCheck(KEY_INPUT_D) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0), (CheckHitKeyWithCheck(KEY_INPUT_E) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0), (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0),

							(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_R) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_X) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_C) != 0),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on()),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on())
						);
					}

					this->m_FPSActive.GetInput(eyechange_key);
					if (look_key) {
						this->m_LookMode = 1;
					}
					else {
						this->m_LookMode = 0;
					}
					Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					this->m_TPS_Xrad += pp_x;
					this->m_TPS_Yrad += pp_y;
					this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-60), deg2rad(60));
					if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
					if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

					this->m_TPS_Xrad *= this->m_TPS_Per;
					this->m_TPS_Yrad *= this->m_TPS_Per;

					Easing(&this->m_TPS_XradR, this->m_TPS_Xrad, 0.5f, EasingType::OutExpo);

					this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
					MyInput.SetRadBuf(PlayerMngr->GetPlayer(GetMyPlayerID()).GetRadBuf());
					//�l�b�g���[�N
					m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
					//�N���C�A���g
					if (m_NetWorkBrowser.GetClient()) {
						for (auto& c : this->character_Pool) {
							if (c->GetMyPlayerID() == GetMyPlayerID() && !PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								c->SetUseRealTimePhysics(false);
								//c->SetUseRealTimePhysics(true);
								c->SetCharaType(CharaTypeID::Mine);
							}
							else {
								c->SetUseRealTimePhysics(false);
								c->SetCharaType(CharaTypeID::Enemy);
							}
						}
						for (auto& v : this->vehicle_Pool) {
							if (v->GetMyPlayerID() == GetMyPlayerID() && PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								v->SetCharaType(CharaTypeID::Mine);
							}
							else {
								v->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
					//
					bool isready = true;
					for (int i = 0; i < Player_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
						auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
						if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
							auto tmp = m_NetWorkBrowser.GetNowServerPlayerData(i, PlayerMngr->GetPlayer(i).IsRide());
							if (i == GetMyPlayerID()) {
								MyInput.SetKeyInput(tmp.Input.GetKeyInput());//�L�[�t���[�������T�[�o�[�ɍ��킹��
								if (!PlayerMngr->GetPlayer(i).IsRide()) {
									c->SetInput(MyInput, isready);
								}
								else {
									v->SetInput(MyInput, isready, false);
								}
								m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
							}
							else {
								if (!PlayerMngr->GetPlayer(i).IsRide()) {
									c->SetInput(tmp.Input, isready);
								}
								else {
									v->SetInput(tmp.Input, isready, true);
								}
								bool override_true = true;
								if (!PlayerMngr->GetPlayer(i).IsRide()) {
									override_true = (tmp.CalcCheckSum() != 0);
								}
								else {
									override_true = true;
									for (auto& v2 : this->vehicle_Pool) {
										if ((v != v2) && ((v->GetMove().pos - v2->GetMove().pos).size() <= 10.f*Scale_Rate)) {
											override_true = false;
											break;
										}
									}
								}
								if (override_true) {
									if (!PlayerMngr->GetPlayer(i).IsRide()) {
										c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
									}
									else {
										v->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
									}
								}

							}
							//�_���[�W�C�x���g����
							if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
									if (tmp.DamageSwitch != c->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(tmp.Damage);
										c->SetDamageSwitchRec(tmp.DamageSwitch);
									}
								}
							}
							else {
								if (ObjMngr->GetObj(ObjType::Vehicle, i) != nullptr) {
									if (tmp.DamageSwitch != v->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(tmp.Damage);
										v->SetDamageSwitchRec(tmp.DamageSwitch);
									}
								}
							}
						}
						else {
							if (i == GetMyPlayerID()) {
								if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
									c->SetInput(MyInput, isready);
								}
								else {
									v->SetInput(MyInput, isready, false);
								}
							}
							//�_���[�W�C�x���g����
							if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
									if (c->GetDamageSwitch() != c->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(c->GetDamageEvent());
										c->SetDamageSwitchRec(c->GetDamageSwitch());
									}
								}
							}
							else {
								if (ObjMngr->GetObj(ObjType::Vehicle, i) != nullptr) {
									if (v->GetDamageSwitch() != v->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(v->GetDamageEvent());
										v->SetDamageSwitchRec(v->GetDamageSwitch());
									}
								}
							}
						}
					}
					m_NetWorkBrowser.LateExecute();
					//�_���[�W�C�x���g
					for (auto& c : this->character_Pool) {
						for (int j = 0; j < this->m_DamageEvents.size(); j++) {
							if (c->SetDamageEvent(this->m_DamageEvents[j])) {
								std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[j]);
								this->m_DamageEvents.pop_back();
								j--;
							}
						}
					}
					for (auto& v : this->vehicle_Pool) {
						for (int j = 0; j < this->m_DamageEvents.size(); j++) {
							if (v->SetDamageEvent(this->m_DamageEvents[j])) {
								std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[j]);
								this->m_DamageEvents.pop_back();
								j--;
							}
						}
					}
				}
				//���[�U�[�T�C�g
				if (PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
					auto StartPos = Vehicle->GetGunMuzzlePos();
					auto EndPos = StartPos + Vehicle->GetGunMuzzleVec() * 500.f*Scale_Rate;
					Vehicle->GetMapColNearest(StartPos, &EndPos);
					while (true) {
						auto colres = this->m_BackGround->GetGroundCol().CollCheck_Line(StartPos, EndPos);
						if (colres.HitFlag == TRUE) {
							if (EndPos == colres.HitPosition) { break; }
							EndPos = colres.HitPosition;
						}
						else {
							break;
						}
					}
					for (auto& v : this->vehicle_Pool) {
						if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
						if (v->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) {
							v->GetColNearestInAllMesh(StartPos, &EndPos);
						}
						v->SetAimingDistance(-1.f);
					}
					Vehicle->SetAimingDistance((StartPos - EndPos).size());
				}
				//Execute
				ObjMngr->ExecuteObject();
				//
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//�G�t�F�N�g
						auto mat = Gun->GetMuzzleMatrix();
						Effect_UseControl::SetOnce(Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
					}
				}
				//����Ȃ��I�u�W�F�N�g�̏���
				ObjMngr->DeleteCheck();
				//�e�̍X�V
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (a->IsActive()) {
								//AmmoClass
								MV1_COLL_RESULT_POLY ColResGround = a->ColCheckGround();
								VECTOR_ref pos_tmp = a->GetMove().pos;
								VECTOR_ref norm_tmp;
								bool hitwall = this->m_BackGround->GetWallCol(a->GetMove().repos, &pos_tmp, &norm_tmp, a->GetCaliberSize());//0.00762f
								bool is_HitAll = false;
								auto& c = *ObjMngr->GetObj(ObjType::Human, a->GetShootedID());//(std::shared_ptr<CharacterClass>&)
								for (auto& tgt : this->character_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), c->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								auto& v = *ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID());
								for (auto& tgt : this->vehicle_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), v->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								if ((ColResGround.HitFlag == TRUE || hitwall) && !is_HitAll) {
									a->HitGround(pos_tmp);

									if (hitwall) {
										Effect_UseControl::Set_FootEffect(pos_tmp, norm_tmp, a->GetCaliberSize() / 0.1f * Scale_Rate);
									}
									else if (ColResGround.HitFlag == TRUE) {
										Effect_UseControl::Set_FootEffect(ColResGround.HitPosition, ColResGround.Normal, a->GetCaliberSize() / 0.1f * Scale_Rate);
									}
									/*
									if (ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID())) {
										if (ColResGround.HitFlag == TRUE) {
										}
										else if (hitwall) {
											((std::shared_ptr<VehicleClass>&)v)->HitGround(pos_tmp, (pos_tmp - a->GetMove().repos).Norm(), a->GetMove().vec);
										}
									}
									//*/
								}
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				this->m_BackGround->GetBox2Dworld()->Step(1.f, 1, 1);//�����X�V
				ObjMngr->LateExecuteObject();
				//���_
				if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					if (Chara->GetSendCamShake()) {
						this->m_CamShake = 1.f;
					}
					Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
					Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
					this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

					if (this->m_FPSActive.on() || Chara->GetIsADS()) {
						camera_main.campos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), this->m_EyePosPer);
						camera_main.camvec = camera_main.campos + Chara->GetEyeVector();
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), this->m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), this->m_TPS_YradR);
						Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = Lerp(Chara->GetEyeVector(), MATRIX_ref::Vtrans(Chara->GetEyeVector(), m_FreeLookMat), this->m_TPS_Per);

						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;
						CamPos += Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), this->m_EyeRunPer);

						camera_main.campos = Lerp(CamPos + CamVec * Lerp(Lerp(-20.f, -50.f, this->m_TPS_Per), 2.f, this->m_EyePosPer_Prone), Chara->GetScopePos(), this->m_EyePosPer);
						camera_main.camvec = Lerp(CamPos, Chara->GetScopePos(), this->m_EyePosPer) + CamVec * 100.f;

						camera_main.camup = Chara->GetEyeVecMat().yvec() + this->m_CamShake2 * 0.25f;
					}
					Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
					Easing(&this->m_EyePosPer, Chara->GetIsADS() ? 1.f : 0.f, 0.8f, EasingType::OutExpo);//
					if (this->m_FPSActive.on()) {
						float fov = 0;
						if (Chara->GetIsADS()) {
							fov = deg2rad(40);
						}
						else if (Chara->GetIsRun()) {
							fov = deg2rad(100);
						}
						else {
							fov = deg2rad(95);
						}
						Easing(&camera_main.near_, Scale_Rate * 0.5f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);

						if (Chara->GetShotSwitch()) {
							fov -= deg2rad(5);
							Easing(&camera_main.fov, fov, 0.5f, EasingType::OutExpo);
						}
						else {
							Easing(&camera_main.fov, fov, 0.9f, EasingType::OutExpo);
						}
					}
					else {
						if (Chara->GetIsADS()) {
							//Easing(&camera_main.fov, deg2rad(90), 0.9f, EasingType::OutExpo);
							Easing(&camera_main.fov, deg2rad(30), 0.9f, EasingType::OutExpo);
							Easing(&camera_main.near_, 10.f, 0.9f, EasingType::OutExpo);
							Easing(&camera_main.far_, Scale_Rate * 300.f, 0.9f, EasingType::OutExpo);
						}
						else if (Chara->GetIsRun()) {
							Easing(&camera_main.fov, deg2rad(70), 0.9f, EasingType::OutExpo);
							Easing(&camera_main.near_, 3.f, 0.9f, EasingType::OutExpo);
							Easing(&camera_main.far_, Scale_Rate * 150.f, 0.9f, EasingType::OutExpo);
						}
						else {
							Easing(&camera_main.fov, deg2rad(55), 0.9f, EasingType::OutExpo);
							Easing(&camera_main.near_, 10.f, 0.9f, EasingType::OutExpo);
							Easing(&camera_main.far_, Scale_Rate * 300.f, 0.9f, EasingType::OutExpo);
						}
					}
				}
				else {
					auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
					Vehicle->Setcamera(camera_main, fov_base);
					{
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis((camera_main.camvec - camera_main.campos).cross(camera_main.camup), this->m_TPS_XradR) * MATRIX_ref::RotAxis(camera_main.camup, this->m_TPS_YradR);
						Easing_Matrix(&this->m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = (camera_main.camvec - camera_main.campos).Norm();
						CamVec = Lerp(CamVec, MATRIX_ref::Vtrans(CamVec, this->m_FreeLookMat), this->m_TPS_Per);

						VECTOR_ref CamPos = Vehicle->Get_EyePos_Base();

						camera_main.campos = Lerp(camera_main.campos, (CamPos + CamVec * -100.f), this->m_TPS_Per);
						camera_main.camvec = CamPos + CamVec * 100.f;
					}
				}
				this->m_BackGround->Execute();
				//UI�p�����[�^�[
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					if (!PlayerMngr->GetPlayer(0).IsRide()) {
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
						this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
						this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
						this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;
					}
					else {
						auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
						this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
						this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
						this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
						this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;
					}

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);


					this->m_UIclass.SetIntParam(12, (int)0);//���ݑI��
					this->m_UIclass.SetIntParam(13, (int)1);//�e�̑���
					{
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(1, "GUN");
						this->m_UIclass.SetIntParam(14, (int)(Chara->GetAmmoNum()));
						this->m_UIclass.SetIntParam(15, (int)(Chara->GetAmmoAll()));
					}
				}
				TEMPSCENE::Execute();
				Effect_UseControl::Execute_Effect();
				return true;
			}
			void			Dispose(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				m_NetWorkBrowser.Dispose();
				Effect_UseControl::Dispose_Effect();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
				for (auto& v : this->vehicle_Pool) {
					v.reset();
				}
				this->vehicle_Pool.clear();
				this->m_BackGround->Dispose();
			}
			//
			void			Depth_Draw(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();

				this->m_BackGround->Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			Shadow_Draw_NearFar(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
			}
			void			Shadow_Draw(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				//this->m_BackGround->Shadow_Draw();
				ObjMngr->DrawObject_Shadow();
			}
			void			Main_Draw(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				SetFogStartEnd(camera_main.near_, camera_main.far_*2.f);
				this->m_BackGround->Draw();
				ObjMngr->DrawObject();
				//ObjMngr->DrawDepthObject();
				//�V�F�[�_�[�`��p�p�����[�^�[�Z�b�g
				{
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					//
					Set_is_Blackout(true);
					Set_Per_Blackout((1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					//
					Set_is_lens(Chara->GetIsADS() && Chara->GetReticleSize()>1.f);
					if (Chara->GetIsADS()) {
						VECTOR_ref LensPos = ConvWorldPosToScreenPos(Chara->GetLensPos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Set_xp_lens(LensPos.x());
							Set_yp_lens(LensPos.y());
							LensPos = ConvWorldPosToScreenPos(Chara->GetLensPosSize().get());
							if (0.f < LensPos.z() && LensPos.z() < 1.f) {
								Set_size_lens(std::hypotf(xp_lens() - LensPos.x(), yp_lens() - LensPos.y()));
							}
						}
						LensPos = ConvWorldPosToScreenPos(Chara->GetReticlePos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Reticle_xpos = LensPos.x();
							Reticle_ypos = LensPos.y();
							Reticle_on = (size_lens() > std::hypotf(xp_lens() - Reticle_xpos, yp_lens() - Reticle_ypos));
						}
					}
					else {
						Reticle_on = false;
					}
					Set_zoom_lens(Chara->GetReticleSize());
				}
				for (auto& c : this->character_Pool) {
					if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
				for (auto& v : this->vehicle_Pool) {
					if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = v->Set_MidPos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						v->SetCameraPosition(campos);
						v->SetCameraSize(std::max(80.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
			}
			void			Main_Draw2(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				ObjMngr->DrawDepthObject();
			}
			void			LAST_Draw(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//���e�B�N���\��
				if (Reticle_on) {
					int x, y;
					Chara->GetReticle().GetSize(&x, &y);
					Chara->GetReticle().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (y / 2.f), Chara->GetReticleRad(), true);
				}
			}
			//UI�\��
			void			UI_Draw(void) noexcept  override {
				auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();
				//auto* Fonts = FontPool::Instance();
				//auto* DrawParts = DXDraw::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				//auto White = GetColor(212, 255, 239);
				//auto Gray = GetColor(64, 64, 64);
				//auto Black = GetColor(0, 0, 0);
				//unsigned int color = Red;
				//
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							a->Draw_Hit_UI(hit_Graph);
						}
						else {
							break;
						}
						loop++;
					}
				}
				//UI
				this->m_UIclass.Draw();

				//�ʐM�ݒ�
				if (!this->m_MouseActive.on()) {
					m_NetWorkBrowser.Draw();
				}
				//
			}
		};
	};
};
