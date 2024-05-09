#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI用
		class AI {
		public:
			float ai_time_a{ 0.f };
			float ai_time_d{ 0.f };
			float ai_time_q{ 0.f };
			float ai_time_e{ 0.f };
			float ai_time_shoot{ 0.f };
			float ai_time_tankback{ 0.f };
			float ai_time_tankback_ing{ 0.f };
			float ai_time_find{ 0.f };
			Vector3DX LastFindPoint;
			float ai_time_turn{ 0.f };
			int ai_tankback_cnt = 0;
			float ai_time_walk{ 0.f };
			bool ai_reload{ false };
			int ai_phase{ 0 };
			std::vector<int> wayp_pre{ 0 };
			PlayerID ai_AimTarget{0};
			PlayerID ai_MyID{0};

			AI(void) noexcept {
				wayp_pre.resize(6);
			}
			~AI(void) noexcept {
				wayp_pre.clear();
			}

			void Spawn(int now) {
				this->ai_phase = 0;
				this->ai_AimTarget = -1;
				this->ai_time_shoot = 0.f;
				this->ai_time_a = 0.f;
				this->ai_time_d = 0.f;
				this->ai_time_e = 0.f;
				this->ai_time_tankback = 0.f;
				this->ai_time_tankback_ing = 0.f;
				this->ai_time_find = 0.f;
				this->ai_time_turn = 0.f;
				this->ai_tankback_cnt = 0;
				this->fill_wayp_pre(now);
			}
			void fill_wayp_pre(int now) {
				for (auto& w : this->wayp_pre) { w = now; }
			}
			void Set_wayp_pre(int now) {
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[i - 1]; }
				this->wayp_pre[0] = now;
			}
			void Set_wayp_return(void) noexcept {
				auto ppp = this->wayp_pre[1];
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[0]; }
				this->wayp_pre[0] = ppp;
			}
#ifdef DEBUG
			void Draw_Debug(const std::vector<Vector3DX>* way_point) {
				for (size_t i = 0; i < this->wayp_pre.size() - 1; i++) {
					Vector3DX startpos = (*way_point)[this->wayp_pre[i]];
					Vector3DX endpos = (*way_point)[this->wayp_pre[i + 1]];
					startpos.y += (5.5f*Scale_Rate);
					endpos.y += (5.5f*Scale_Rate);
					DrawCapsule_3D(startpos, endpos, 0.25f*Scale_Rate, GetColor(0, 255, 0), GetColor(0, 255, 0));
				}
			}
#endif
		};

		class AIControl {
			AI									cpu_do;								//AI用
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			float								m_RepairCnt{ 0.f };
			float								m_RepairCnt2{ 0.f };
		private:
			int Get_next_waypoint(std::vector<int> wayp_pre, Vector3DX poss, Vector3DX zvec = Vector3DX::zero()) {
				int now = -1;
				auto tmp = Vector3DX::vget(0, 100.f*Scale_Rate, 0);
				for (auto& w : m_BackGround->GetWayPoint()) {
					auto id = &w - &m_BackGround->GetWayPoint().front();
					bool tt = true;
					for (auto& ww : wayp_pre) {
						if (id == ww) {
							tt = false;
						}
					}
					if (tt) {
						if (tmp.magnitude() >= (w - poss).magnitude()) {
							auto startpos = w + Vector3DX::vget(0, 0.5f*Scale_Rate, 0);
							auto endpos = poss + Vector3DX::vget(0, 0.5f*Scale_Rate, 0);

							if (!m_BackGround->CheckLinetoMap(startpos, &endpos, true, false)) {
								if (zvec == Vector3DX::zero() || Vector3DX::Dot(zvec.normalized(), (w - poss).normalized()) < 0.f) {
									tmp = (w - poss);
									now = int(id);
								}
							}
						}
					}
				}
				return now;
			}
		public:
			//AI操作
			void SetNextWaypoint(const Vector3DX& vec_z) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyVeh = PlayerMngr->GetPlayer(this->cpu_do.ai_MyID).GetVehicle();
				int now = Get_next_waypoint(this->cpu_do.wayp_pre, MyVeh->GetMove().pos, vec_z);
				if (now != -1) {
					this->cpu_do.Set_wayp_pre(now);
				}
			}
			void AI_move(InputControl* MyInput) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyVeh = PlayerMngr->GetPlayer(this->cpu_do.ai_MyID).GetVehicle();
				bool W_key{ false };
				bool A_key{ false };
				bool S_key{ false };
				bool D_key{ false };

				bool shotMain_Key{ false };
				bool shotSub_Key{ false };
				int32_t x_m{ 0 }, y_m{ 0 };
				//AI
				//操作
				Vector3DX vec_to = Vector3DX::zero();
				//
				auto vec_mat = MyVeh->GetMove().mat;
				auto vec_gunmat = MyVeh->GetLookVec();
				auto vec_x = vec_gunmat.xvec();
				auto vec_y = vec_gunmat.yvec();
				auto vec_zp = vec_gunmat.zvec() * -1.f;
				auto vec_z = vec_mat.zvec() * -1.f;
				//狙うキャラを探索+AIのフェーズ選択
				{
					auto CheckCanLookTarget = [&](const std::shared_ptr<VehicleClass>& tgt, Vector3DX* Res) {


						if (!tgt->IsAlive()) { return false; }
						Vector3DX StartPos = MyVeh->Get_EyePos_Base();
						Vector3DX EndPos = tgt->GetMove().pos + Vector3DX::vget(0.f, 1.5f*Scale_Rate, 0.f);
						Vector3DX vec_tmp = EndPos - StartPos;
						if (vec_tmp.magnitude() > 150.f*Scale_Rate) { return false; }
						if (vec_tmp.magnitude() > 30.f*Scale_Rate) {
							auto vec_gunmat2 = MyVeh->GetGunMuzzleMatrix(0);
							auto vec_zp2 = vec_gunmat2.zvec() * -1.f;
							if (!(
								(Vector3DX::Dot(vec_zp2, vec_tmp.normalized()) > std::cos(deg2rad(45))) ||
								(Vector3DX::Dot(vec_z, vec_tmp.normalized()) > std::cos(deg2rad(45)))
								)) {
								return false;
							}
						}
						if (this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, false, false)) { return false; }
						if (Res) { *Res = vec_tmp; }
						return true;
					};

					bool ans = false;
					if (this->cpu_do.ai_phase == 0) {
						for (int i = 0; i < Player_num; i++) {
							auto& tgt = PlayerMngr->GetPlayer(i).GetVehicle();
							if (MyVeh == tgt) { continue; }
							Vector3DX vec_tmp;
							if (CheckCanLookTarget(tgt, &vec_tmp)) {
								if (vec_to == Vector3DX::zero()) { vec_to = vec_tmp; } //基準の作成
								if (vec_to.magnitude() >= vec_tmp.magnitude()) {
									vec_to = vec_tmp;
									this->cpu_do.ai_AimTarget = (PlayerID)i;
									this->cpu_do.LastFindPoint = tgt->GetMove().pos;
									ans = true;
								}
							}
						}
					}
					else {
						{
							auto& tgt = PlayerMngr->GetPlayer(this->cpu_do.ai_AimTarget).GetVehicle();
							Vector3DX StartPos = MyVeh->Get_EyePos_Base();
							Vector3DX vec_to2 = tgt->GetMove().pos - StartPos;
							for (int i = 0; i < Player_num; i++) {
								auto& tgt2 = PlayerMngr->GetPlayer(i).GetVehicle();
								if (MyVeh == tgt2) { continue; }
								if (i == this->cpu_do.ai_AimTarget) { continue; }
								Vector3DX vec_tmp;
								if (CheckCanLookTarget(tgt2, &vec_tmp)) {
									if (vec_to2 == Vector3DX::zero()) { vec_to2 = vec_tmp; } //基準の作成
									if (vec_to2.magnitude() > vec_tmp.magnitude()) {
										vec_to2 = vec_tmp;
										this->cpu_do.ai_AimTarget = (PlayerID)i;
										this->cpu_do.LastFindPoint = tgt2->GetMove().pos;
										ans = true;
									}
								}
							}
						}


						auto& tgt = PlayerMngr->GetPlayer(this->cpu_do.ai_AimTarget).GetVehicle();
						Vector3DX vec_tmp;
						if (CheckCanLookTarget(tgt, &vec_tmp)) {
							this->cpu_do.LastFindPoint = tgt->GetMove().pos;
							ans = true;
						}
						else {
							Vector3DX StartPos = MyVeh->Get_EyePos_Base();
							Vector3DX EndPos = this->cpu_do.LastFindPoint + Vector3DX::vget(0.f, 1.5f*Scale_Rate, 0.f);
							vec_tmp = EndPos - StartPos;
						}
						vec_to = vec_tmp + Vector3DX::vget(GetRandf(2.f*Scale_Rate), GetRandf(2.f*Scale_Rate), GetRandf(2.f*Scale_Rate));


						if (!tgt->IsAlive()) {
							this->cpu_do.ai_time_find = 0.f;
						}
					}
					if (!ans) {
						this->cpu_do.ai_time_find = std::max(this->cpu_do.ai_time_find - 1.f / DrawParts->GetFps(), 0.f);
						if (this->cpu_do.ai_time_find == 0.f) {
							this->cpu_do.ai_AimTarget = -1;
							this->cpu_do.ai_phase = 0;
						}
					}
					else {
						if (this->cpu_do.ai_time_find != 10.f) {
							this->cpu_do.ai_time_turn = 12.f;
							this->cpu_do.ai_phase = 1;
						}
						this->cpu_do.ai_time_find = 10.f;
					}
					this->cpu_do.ai_time_turn = std::max(this->cpu_do.ai_time_turn - 1.f / DrawParts->GetFps(), 0.f);
				}
				//this->cpu_do.ai_phase = 0;
				//
				switch (this->cpu_do.ai_phase) {
				case 0://戦車乗車中通常フェイズ
				{
					W_key = true;

					//向き指定
					vec_to = m_BackGround->GetWayPoint()[this->cpu_do.wayp_pre.front()] - MyVeh->GetMove().pos;
					vec_to.y = (0.f);

					vec_z = MyVeh->GetMove().mat.zvec();
					//到達時に判断
					if (vec_to.magnitude() <= 1.5f*Scale_Rate) {
						SetNextWaypoint(vec_z);
					}

					{
						auto Zbuf = vec_zp.normalized();
						Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
						auto Tobuf = vec_to.normalized();
						Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
						x_m = -int(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x * 40);
					}
					{
						auto Zbuf = vec_zp;
						Zbuf.y = (0.f);
						auto Tobuf = vec_to;
						Tobuf.y = (0.f);
						y_m = int(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y * 40);
					}

					if (this->cpu_do.ai_tankback_cnt >= 2) {
						this->cpu_do.ai_tankback_cnt = 0;
						SetNextWaypoint(vec_z*-1.f);
					}

					Vector3DX vec_z2 = vec_z; vec_z2.y = (0); vec_z2 = vec_z2.normalized();
					Vector3DX vec_to2 = vec_to; vec_to2.y = (0); vec_to2 = vec_to2.normalized();

					float cost = Vector3DX::Cross(vec_to2, vec_z2).y;
					float sint = sqrtf(std::abs(1.f - cost * cost));
					auto view_yrad = std::atan2f(cost, sint); //cos取得2D

					if (this->cpu_do.ai_time_tankback_ing > 0.f && true) {//無効化x
						this->cpu_do.ai_time_tankback_ing -= 1.f / DrawParts->GetFps();
						W_key = false;
						S_key = true;
						A_key = (GetRand(100) > 50);
						D_key = !A_key;
					}
					else {
						this->cpu_do.ai_time_tankback_ing = 0.f;
						if (std::abs(view_yrad) > deg2rad(30)) {
							W_key = false;
							if (x_m < 0) {
								//this->S_key = true;
							}
						}
						if (MyVeh->Getvec_real().magnitude() <= MyVeh->GetMove().vec.magnitude() *(0.5f)) {
							this->cpu_do.ai_time_tankback += 1.f / DrawParts->GetFps();
						}
						else {
							this->cpu_do.ai_time_tankback = 0.f;
						}
						if (this->cpu_do.ai_time_tankback > 3.f) {
							this->cpu_do.ai_time_tankback = 0.f;
							this->cpu_do.ai_time_tankback_ing = 5.f;
							this->cpu_do.ai_tankback_cnt++;
						}
						if (view_yrad > 0) {
							A_key = false;
							D_key = true;
						}
						else if (view_yrad < 0) {
							A_key = true;
							D_key = false;
						}
					}
				}
				break;
				case 1://戦車乗車中戦闘フェイズ
				{
					{
						auto Zbuf = vec_zp.normalized();
						Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
						auto Tobuf = vec_to.normalized();
						Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
						x_m = -int(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x * 40);
					}
					{
						auto Zbuf = vec_zp;
						Zbuf.y = (0.f);
						auto Tobuf = vec_to;
						Tobuf.y = (0.f);
						y_m = int(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y * 40);
					}
					{
						shotMain_Key = true;

						shotMain_Key &= (this->cpu_do.ai_time_find == 10.f);

						auto vec_gunmat2 = MyVeh->GetGunMuzzleMatrix(0);
						auto vec_zp2 = vec_gunmat2.zvec() * -1.f;
						{
							auto Zbuf = vec_zp2.normalized();
							Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
							auto Tobuf = vec_to.normalized();
							Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
							shotMain_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x) < 0.1f);
						}
						{
							auto Zbuf = vec_zp2;
							Zbuf.y = (0.f);
							auto Tobuf = vec_to;
							Tobuf.y = (0.f);
							shotMain_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y) < 0.1f);
						}
					}
					if (MyVeh->Get_Gunsize() >= 2) {
						shotSub_Key = (GetRand(100) <= 5);

						shotSub_Key &= (this->cpu_do.ai_time_find == 10.f);

						auto vec_gunmat2 = MyVeh->GetGunMuzzleMatrix(1);
						auto vec_zp2 = vec_gunmat2.zvec() * -1.f;
						{
							auto Zbuf = vec_zp2.normalized();
							Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
							auto Tobuf = vec_to.normalized();
							Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
							shotSub_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x) < std::sin(deg2rad(2)));
						}
						{
							auto Zbuf = vec_zp2;
							Zbuf.y = (0.f);
							auto Tobuf = vec_to;
							Tobuf.y = (0.f);
							shotSub_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y) < std::sin(deg2rad(2)));
						}
					}
					if (this->cpu_do.ai_time_turn <= 12.f - 3.f) {
						auto Zbuf = vec_z;
						Zbuf.y = (0.f);
						auto Tobuf = vec_to;
						Tobuf.y = (0.f);
						auto SinBuf = -Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y;
						if (std::abs(SinBuf) > 0.2f) {
							A_key = (SinBuf > 0);
							D_key = !A_key;
						}
						else {
							auto cosBuf = Vector3DX::Dot(Zbuf.normalized(), Tobuf.normalized());
							if (cosBuf < 0.f) {
								A_key = true;
								D_key = !A_key;
							}
						}
					}
					if (12.f - 12.f <= this->cpu_do.ai_time_turn && this->cpu_do.ai_time_turn <= 12.f - 6.f) {
						W_key = false;
						S_key = !W_key;
					}
					else {
						W_key = true;
						S_key = !W_key;
					}
					if (this->cpu_do.ai_time_turn <= 0.f) {
						this->cpu_do.ai_phase = 2;
					}


					if (this->cpu_do.ai_time_find <= 5.f) {
						W_key = true;
						S_key = !W_key;
					}
					//
					{
						auto& tgt = PlayerMngr->GetPlayer(this->cpu_do.ai_AimTarget).GetVehicle();
						Vector3DX StartPos = MyVeh->Get_EyePos_Base();
						Vector3DX vec_to2 = tgt->GetMove().pos - StartPos;
						if (vec_to2.magnitude() <= 8.f*Scale_Rate) {
							W_key = false;
							S_key = false;
						}
					}

					//スタック回避
					if (this->cpu_do.ai_time_tankback_ing > 0.f && true) {//無効化x
						this->cpu_do.ai_time_tankback_ing -= 1.f / DrawParts->GetFps();
						W_key = !W_key;
						S_key = !W_key;
						A_key = (GetRand(100) > 50);
						D_key = !A_key;
					}
					else {
						this->cpu_do.ai_time_tankback_ing = 0.f;
						if (MyVeh->Getvec_real().magnitude() <= MyVeh->GetMove().vec.magnitude() *(0.5f)) {
							this->cpu_do.ai_time_tankback += 1.f / DrawParts->GetFps();
						}
						else {
							this->cpu_do.ai_time_tankback = 0.f;
						}
						if (this->cpu_do.ai_time_tankback > 3.f) {
							this->cpu_do.ai_time_tankback = 0.f;
							this->cpu_do.ai_time_tankback_ing = 5.f;
							this->cpu_do.ai_tankback_cnt++;
						}
					}
				}
				break;
				case 2://機動戦闘フェイズ
				{
					W_key = true;

					{
						auto Zbuf = vec_zp.normalized();
						Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
						auto Tobuf = vec_to.normalized();
						Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
						x_m = -int(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x * 40);
					}
					{
						auto Zbuf = vec_zp;
						Zbuf.y = (0.f);
						auto Tobuf = vec_to;
						Tobuf.y = (0.f);
						y_m = int(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y * 40);
					}
					{
						shotMain_Key = true;

						shotMain_Key &= (this->cpu_do.ai_time_find == 10.f);

						auto vec_gunmat2 = MyVeh->GetGunMuzzleMatrix(0);
						auto vec_zp2 = vec_gunmat2.zvec() * -1.f;
						{
							auto Zbuf = vec_zp2.normalized();
							Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
							auto Tobuf = vec_to.normalized();
							Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
							shotMain_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x) < 0.1f);
						}
						{
							auto Zbuf = vec_zp2;
							Zbuf.y = (0.f);
							auto Tobuf = vec_to;
							Tobuf.y = (0.f);
							shotMain_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y) < 0.1f);
						}
					}
					if (MyVeh->Get_Gunsize() >= 2) {
						shotSub_Key = (GetRand(100) <= 5);

						shotSub_Key &= (this->cpu_do.ai_time_find == 10.f);

						auto vec_gunmat2 = MyVeh->GetGunMuzzleMatrix(1);
						auto vec_zp2 = vec_gunmat2.zvec() * -1.f;
						{
							auto Zbuf = vec_zp2.normalized();
							Zbuf.z = (std::hypotf(Zbuf.x, Zbuf.z));
							auto Tobuf = vec_to.normalized();
							Tobuf.z = (std::hypotf(Tobuf.x, Tobuf.z));
							shotSub_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).x) < std::sin(deg2rad(2)));
						}
						{
							auto Zbuf = vec_zp2;
							Zbuf.y = (0.f);
							auto Tobuf = vec_to;
							Tobuf.y = (0.f);
							shotSub_Key &= (std::abs(Vector3DX::Cross(Zbuf.normalized(), Tobuf.normalized()).y) < std::sin(deg2rad(2)));
						}
					}

					//向き指定
					vec_to = m_BackGround->GetWayPoint()[this->cpu_do.wayp_pre.front()] - MyVeh->GetMove().pos;
					vec_to.y = (0.f);

					vec_z = MyVeh->GetMove().mat.zvec();
					//到達時に判断
					if (vec_to.magnitude() <= 1.5f*Scale_Rate) {
						SetNextWaypoint(vec_z);
					}

					if (this->cpu_do.ai_tankback_cnt >= 2) {
						this->cpu_do.ai_tankback_cnt = 0;
						SetNextWaypoint(vec_z*-1.f);
					}

					Vector3DX vec_z2 = vec_z; vec_z2.y = (0); vec_z2 = vec_z2.normalized();
					Vector3DX vec_to2 = vec_to; vec_to2.y = (0); vec_to2 = vec_to2.normalized();

					float cost = Vector3DX::Cross(vec_to2, vec_z2).y;
					float sint = sqrtf(std::abs(1.f - cost * cost));
					auto view_yrad = std::atan2f(cost, sint); //cos取得2D

					if (this->cpu_do.ai_time_tankback_ing > 0.f && true) {//無効化x
						this->cpu_do.ai_time_tankback_ing -= 1.f / DrawParts->GetFps();
						W_key = false;
						S_key = true;
						A_key = (GetRand(100) > 50);
						D_key = !A_key;
					}
					else {
						this->cpu_do.ai_time_tankback_ing = 0.f;
						if (std::abs(view_yrad) > deg2rad(30)) {
							W_key = false;
							if (x_m < 0) {
								//this->S_key = true;
							}
						}
						if (MyVeh->Getvec_real().magnitude() <= MyVeh->GetMove().vec.magnitude() *(0.5f)) {
							this->cpu_do.ai_time_tankback += 1.f / DrawParts->GetFps();
						}
						else {
							this->cpu_do.ai_time_tankback = 0.f;
						}
						if (this->cpu_do.ai_time_tankback > 3.f) {
							this->cpu_do.ai_time_tankback = 0.f;
							this->cpu_do.ai_time_tankback_ing = 5.f;
							this->cpu_do.ai_tankback_cnt++;
						}
						if (view_yrad > 0) {
							A_key = false;
							D_key = true;
						}
						else if (view_yrad < 0) {
							A_key = true;
							D_key = false;
						}
					}
				}
				break;
				default:
					break;
				}


				//W_key = false;
				//A_key = false;
				//S_key = false;
				//D_key = false;

				//shotMain_Key = false;
				//shotSub_Key = false;

				MyInput->SetInputStart((float)x_m / 100.f, (float)y_m / 100.f);
				MyInput->SetInputPADS(PADS::MOVE_W, W_key);
				MyInput->SetInputPADS(PADS::MOVE_S, S_key);
				MyInput->SetInputPADS(PADS::MOVE_A, A_key);
				MyInput->SetInputPADS(PADS::MOVE_D, D_key);
				MyInput->SetInputPADS(PADS::SHOT, shotMain_Key);
				MyInput->SetInputPADS(PADS::JUMP, shotSub_Key);
				//生き返り
				if (!MyVeh->IsAlive()) {
					if (m_RepairCnt > 30.f) {
						m_RepairCnt = 0.f;
						auto& Player = PlayerMngr->GetPlayer(MyVeh->GetMyPlayerID());
						//履帯
						{
							const auto* Ptr = Player.GetInventory(2, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
							if (!Ptr) {
								Player.FillInventory(2, MyVeh->GetData().GetTrackPtr(), 0, 0, Player.GetInventoryXSize(2), Player.GetInventoryYSize(2));
								MyVeh->RepairParts(MyVeh->GetData().Get_module_mesh()[0]);
							}
						}
						{
							const auto* Ptr = Player.GetInventory(3, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
							if (!Ptr) {
								Player.FillInventory(3, MyVeh->GetData().GetTrackPtr(), 0, 0, Player.GetInventoryXSize(3), Player.GetInventoryYSize(3));
								MyVeh->RepairParts(MyVeh->GetData().Get_module_mesh()[1]);
							}
						}
						//弾
						{
							const auto* Ptr = Player.GetInventory(0, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
							if (!Ptr) {
								Player.FillInventory(0, MyVeh->GetGun()[0].GetData()->GetAmmoSpec().at(0), 0, 0, Player.GetInventoryXSize(0) / 2, Player.GetInventoryYSize(0), 3);
								if (MyVeh->Get_Gunsize() >= 2) {
									Player.FillInventory(0, MyVeh->GetGun()[1].GetData()->GetAmmoSpec().at(0), Player.GetInventoryXSize(0) / 2, 0, Player.GetInventoryXSize(0), Player.GetInventoryYSize(0), 3);
								}
							}
						}
						//
						MyVeh->SubHP(-MyVeh->GetHPMax() / 3);


					}
					else {
						m_RepairCnt += 1.f / DrawParts->GetFps();
					}
					m_RepairCnt2 = 0;
				}
				else {
					m_RepairCnt = 0.f;

					if (m_RepairCnt2 > 30.f) {
						m_RepairCnt2 = 0.f;
						MyVeh->SubHP(-MyVeh->GetHPMax() / 3);
					}
				}
			}
		public:
			void Init(std::shared_ptr<BackGroundClass>& BackBround_t, PlayerID ID) noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				this->cpu_do.ai_MyID = ID;
				this->m_BackGround = BackBround_t;
				auto& MyVeh = PlayerMngr->GetPlayer(this->cpu_do.ai_MyID).GetVehicle();

				//AIの選択をリセット
				int now = Get_next_waypoint(this->cpu_do.wayp_pre, MyVeh->GetMove().pos);
				this->cpu_do.Spawn((now != -1) ? now : 0);
				m_RepairCnt = 0.f;
			}
			void Execute() noexcept {

			}
			void Draw() noexcept {
#ifdef DEBUG
				return;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyVeh = PlayerMngr->GetPlayer(this->cpu_do.ai_MyID).GetVehicle();
				if (!MyVeh->IsAlive()) { return; }
				if (MyVeh->GetMyPlayerID() == 0) { return; }
				cpu_do.Draw_Debug(&m_BackGround->GetWayPoint());
				if (this->cpu_do.ai_phase == 1) {
					auto& tgt = PlayerMngr->GetPlayer(this->cpu_do.ai_AimTarget).GetVehicle();
					Vector3DX StartPos = MyVeh->Get_EyePos_Base() + Vector3DX::vget(0.f, 5.f*Scale_Rate, 0.f);
					Vector3DX EndPos = tgt->GetMove().pos + Vector3DX::vget(0.f, 5.f*Scale_Rate, 0.f);

					DrawCapsule_3D(StartPos, EndPos, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
				}
				//*
				{
					auto vec_gunmat = MyVeh->GetGunMuzzleMatrix(0);
					auto vec_x = vec_gunmat.xvec();
					auto vec_y = vec_gunmat.yvec();
					auto vec_zp = vec_gunmat.zvec() * -1.f;

					Vector3DX StartPos = MyVeh->Get_EyePos_Base() + Vector3DX::vget(0.f, 5.f*Scale_Rate, 0.f);

					DrawCapsule_3D(StartPos, StartPos + vec_x * Scale_Rate*5.f, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
					DrawCapsule_3D(StartPos, StartPos + vec_y * Scale_Rate*5.f, 0.1f*Scale_Rate, GetColor(0, 255, 0), GetColor(255, 0, 0));
					DrawCapsule_3D(StartPos, StartPos + vec_zp * Scale_Rate*5.f, 0.1f*Scale_Rate, GetColor(0, 0, 255), GetColor(255, 0, 0));
				}
				//*/
#endif
			}
		};
	};
};
