#pragma once
#include	"../Header.hpp"
#include	"NetWork.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
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
			//サーバー専用
			ServerControl			m_ServerCtrl;																//
			//クライアント専用
			ClientControl			m_ClientCtrl;																//
			//共通
			bool					m_IsClient{true};															//
			SequenceEnum			m_Sequence{SequenceEnum::SelMode};										//
			bool					m_SeqFirst{false};														//
			float					m_Tick{1.f};																//
			NewWorkSetting			m_NewWorkSetting;															//
			int						m_NewWorkSelect{0};														//
			NewSetting				m_NewSetting;																//
			double					m_ClientFrame{0.0};
			float					m_Ping{0.f};

			switchs					m_LeftClick;
		public:
			const auto& GetClient(void) const noexcept { return this->m_IsClient; }
			const auto& GetSequence(void) const noexcept { return this->m_Sequence; }
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetMyPlayerID() : this->m_ServerCtrl.GetMyPlayerID(); }
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
			void FirstExecute(const InputControl& MyInput, const moves & move_t, const std::vector<DamageEvent>& Damage_t) noexcept {
				if (this->m_IsClient) {
					m_ClientCtrl.SetMyPlayer(MyInput, move_t, Damage_t, this->m_ClientFrame);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ClientCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, this->m_NewSetting.IP);
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ClientCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
				//サーバー
				else {
					m_ServerCtrl.SetMyPlayer(MyInput, move_t, Damage_t, this->m_ClientFrame);
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
			void Draw(void) noexcept;
			void Dispose(void) noexcept {
				m_ServerCtrl.Dispose();
				m_ClientCtrl.Dispose();
			}
		};
	};
};
