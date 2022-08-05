#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class BulletType {
			FireBall,
			Thunder,
		};
		class BulletClass {
		private://キャラパラメーター
			bool m_IsActive{ false };
			moves m_move;
			float m_speed{ 0.f };
			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			float m_HitTimer{ 0.f };
			moves m_move_Hit;
			BulletType m_Type{ BulletType::FireBall };
		public:
			void Set(const MATRIX_ref& mat, const VECTOR_ref& pos, BulletType pType,float pBaseSpeed) noexcept {
				this->m_IsActive = true;
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				this->m_move.repos = this->m_move.pos;
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
				this->m_Type = pType;
				switch (this->m_Type) {
				case BulletType::FireBall:
					this->m_speed = (pBaseSpeed + 200.f) * 1000.f / 3600.f * Scale_Rate;
					this->m_HitTimer = 10.f;
					break;
				case BulletType::Thunder:
					this->m_speed = (pBaseSpeed + 800.f) * 1000.f / 3600.f * Scale_Rate;
					this->m_HitTimer = 3.f;
					break;
				default:
					break;
				}
			}
			void Execute(void) noexcept {
				if (this->m_IsActive) {
					this->m_move.repos = this->m_move.pos;
					this->m_move.vec = this->m_move.mat.zvec() * (-1.f * this->m_speed / FPS) + VECTOR_ref::up()*this->m_yAdd;
					this->m_move.pos += this->m_move.vec;
					//this->m_yAdd += (M_GR / (FPS*FPS));
				}
				if (this->m_Timer > this->m_HitTimer) {
					this->m_IsActive = false;
				}
				this->m_Timer += 1.f / FPS;
			}
			bool CheckBullet(const MV1* pCol) noexcept {
				if (this->m_IsActive) {
					auto HitResult = pCol->CollCheck_Line(this->m_move.repos, this->m_move.pos);
					if (HitResult.HitFlag == TRUE) {
						this->m_move_Hit.pos = HitResult.HitPosition;
						this->m_move_Hit.vec = HitResult.Normal;
						return true;
					}
				}
				return false;
			}
			void SetHit() noexcept {
				if (this->m_IsActive) {
					this->m_HitTimer = this->m_Timer + 0.5f;
					this->m_IsActive = false;
				}
			}
		public:
			auto* Get_Move(void) noexcept { return (this->m_IsActive) ? &this->m_move : (moves*)nullptr; }
			const auto& GetMoveHit(void) noexcept { return this->m_move_Hit; }
			const auto& GetType(void) noexcept { return this->m_Type; }
		};

		class MagicClass {
		private://キャラパラメーター
			float			m_CoolDown = 60.f;
			MagicType		m_Type{ MagicType::FireBall };
			std::string		m_Name;
		private:
			float m_CoolFrame = 0.f;
		public:
			const auto&		GetCoolFrame(void) const noexcept { return this->m_CoolFrame; }
			const auto&		GetCoolDown(void) const noexcept { return this->m_CoolDown; }
			const auto&		GetGetType(void) const noexcept { return this->m_Type; }
			const auto&		GetName(void) const noexcept { return this->m_Name; }
			
		public:
			void Set(std::string_view pName) noexcept {
				this->m_Name = pName;
				int mdata = FileRead_open(("data/Magic/"+ this->m_Name+"/data.txt").c_str(), FALSE);
				std::string Type = getparams::_str(mdata);
				float cooldown = getparams::_float(mdata);
				FileRead_close(mdata);
				for (int i = 0; i < (int)MagicType::Max; i++) {
					if (Type == MagicTypeName[i]) {
						this->m_Type = (MagicType)i;
						break;
					}
				}
				this->m_CoolDown = cooldown;
				this->m_CoolFrame = 0.f;
			}
			void Execute() noexcept {
				this->m_CoolFrame = std::max(this->m_CoolFrame - 1.f / FPS, 0.f);
			}
			bool Use() noexcept {
				if (this->m_CoolFrame > 0.f) {
					return false;
				}
				this->m_CoolFrame = this->m_CoolDown;
				return true;
			}
		};

		class CharacterClass : public ObjectBaseClass, public Effect_UseControl {
		private://キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;

			const float FlightSpeedMin = 20.f;
			const float FlightSpeedMax = 200.f;

			const float FlightHightLimit = 5000.f;

			const float HPMax = 100.f;
			const float MPMax = 100.f;
			std::string											m_Name;
			CharaTypeID											m_CharaType;
		private:
			bool												m_KeyActive{ true };
			CharacterMoveGroundControl							m_InputGround;
			CharacterMoveFlightControl							m_InputSky;
			switchs												m_Rightkey;
			switchs												m_Leftkey;
			switchs												m_Upkey;
			switchs												m_Downkey;
			switchs												m_UseItemkey;
			switchs												m_UseMagickey;
			//飛行関連
			VECTOR_ref											m_FlightVecBuf;
			MATRIX_ref											m_FlightMatrix;
			VECTOR_ref											m_FlightKickReturnVecBuf;
			VECTOR_ref											m_FlightKickReturnVec;
			float												m_FlightKickReturnSpeed;
			bool												m_FlightKickReturn{ false };
			bool												m_FlightKickReturnAnimOne{ false };
			bool												m_FlightKickReturnAnimLR{ false };
			float												m_FlightSpeedAdd{ 0.f };
			float												m_FlightSpeed_r{ 0.f };
			float												m_FlightSpeed{ 0.f };
			float												m_FlightReturn{ 0.f };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			MATRIX_ref											m_UpperMatrix;
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			float												m_SlingHoukiPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_ReturnStand{ false };
			bool												m_RunReady{ false };
			bool												m_RunReadyFirst{ false };
			bool												m_Running{ false };
			bool												m_ReadySwitch{ false };
			bool												m_UsingItem{ false };
			bool												m_SetMagic{ false };
			float												m_SetMagicPer{ 0.f };
			float												m_SetMagicTime{ 0.f };
			bool												m_UsingMagic{ false };
			bool												m_MagicEffectStart{ false };
			bool												m_MagicEffectLoop{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			//
			std::array<BulletClass, 6>							m_Bullet;
			int													m_NowBullet{ 0 };
			//
			float												m_HP{ 0.f };							//スコア
			float												m_MP{ 0.f };							//スコア
			float												m_Score{ 0.f };							//スコア
			//表情
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//サウンド
			int													m_CharaSound{ -1 };
			//箒
			std::shared_ptr<HoukiClass>							m_Houki_Ptr{ nullptr };					//箒
			//アイテム
			std::vector <std::pair<ItemType, std::vector<std::shared_ptr<ItemClass>>>>	m_Item_Ptrs;
			int													m_ItemSel;
			int													m_MagicSel;
			std::vector<MagicClass>								m_Magic;
			//
			std::shared_ptr<CharacterClass>						m_LockOn{ nullptr };
			//
			bool												m_SendCamShake{ false };
		public://ゲッター
			//
			void			SetAnimOnce(CharaAnimeID ID, float speed) noexcept { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			void			SetAnimLoop(CharaAnimeID ID, float speed) noexcept { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { this->GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { this->GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { m_Shapes[(int)pShape].second = Per; }
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix((int)this->GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			//
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetEyeVecMat(void) const noexcept {
				MATRIX_ref Matrix;

				float per = Lerp(1.f, 0.5f, this->m_FlightSpeed / (FlightSpeedMax - FlightSpeedMin))*10.f;
				Matrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_InputSky.GetFradAdd().x()*per);
				Matrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_InputSky.GetFradAdd().y()*per);
				Matrix *= MATRIX_ref::RotAxis(this->m_move.mat.yvec(), this->m_InputSky.GetFradAdd().z()*per);

				return (GetCharaDir()*Matrix);
			}
			const auto		GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVecMat().zvec() * -1.5f; }
			const auto		GetFlightPer(void) const noexcept { return this->m_InputSky.GetFlightPer(); }
			const auto		GetFlightMode(void) const noexcept { return this->m_InputSky.GetIsFlightMode(); }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return HPMax; }
			const auto&		GetMP(void) const noexcept { return this->m_MP; }
			const auto&		GetMPMax(void) const noexcept { return MPMax; }
			const auto&		GetHoukiPtr(void) const noexcept { return this->m_Houki_Ptr; }
			const auto&		GetFlightSpeed(void) const noexcept { return this->m_FlightSpeed; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetName(void) const noexcept { return this->m_Name; }

			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetHoukiPtr(std::shared_ptr<HoukiClass>& pHoukiPtr) noexcept { this->m_Houki_Ptr = pHoukiPtr; }
			void			AddScore(float value) noexcept { this->m_Score += value; }
			void			SubScore(float value) noexcept { this->m_Score -= value; }
			void			SetScore(float value) noexcept { this->m_Score = value; }
			void			AddHP(float value) noexcept { this->m_HP = std::clamp(this->m_HP + value, 0.f, HPMax); }
			void			SubHP(float value) noexcept { this->m_HP = std::clamp(this->m_HP - value, 0.f, HPMax); }
			void			SetHP(float value) noexcept { this->m_HP = value; }
			void			AddMP(float value) noexcept { this->m_MP = std::clamp(this->m_MP + value, 0.f, MPMax); }
			void			SubMP(float value) noexcept { this->m_MP = std::clamp(this->m_MP - value, 0.f, MPMax); }
			void			SetMP(float value) noexcept { this->m_MP = value; }

			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos) noexcept {
				this->m_KeyActive = false;
				//飛行関連
				//this->m_FlightVecBuf;
				//this->m_FlightMatrix;
				//this->m_FlightKickReturnVecBuf;
				//this->m_FlightKickReturnVec;
				this->m_FlightKickReturnSpeed = 0.f;
				this->m_FlightKickReturn = false;
				this->m_FlightKickReturnAnimOne = false;
				this->m_FlightKickReturnAnimLR = false;
				this->m_FlightSpeedAdd = 0.f;
				this->m_FlightSpeed_r = 0.f;
				this->m_FlightSpeed = 0.f;
				this->m_FlightReturn = 0.f;
				//
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					GetAnimeBuf((CharaAnimeID)i) = 0.f;
				}
				this->m_Speed = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_NeckPer = 0.f;
				this->m_SlingHoukiPer = 0.f;
				this->m_TurnBody = false;
				this->m_ReturnStand = false;
				this->m_RunReady = false;
				this->m_RunReadyFirst = false;
				this->m_Running = false;
				this->m_ReadySwitch = false;
				this->m_UsingItem = false;
				this->m_SetMagic = false;
				this->m_SetMagicPer = 0.f;
				this->m_SetMagicTime = 0.f;
				this->m_UsingMagic = false;
				this->m_MagicEffectStart = false;
				this->m_MagicEffectLoop = false;

				//this->m_UpperAnimSelect;
				//this->m_PrevUpperAnimSel;
				//this->m_BottomAnimSelect;
				this->m_HP = HPMax;
				this->m_MP = MPMax;
				this->m_Score = 0.f;							//スコア
				//表情
				this->m_Eyeclose = 0;
				this->m_EyeclosePer = 0.f;
				//サウンド
				this->m_CharaSound = -1;
				//this->m_Houki_Ptr{ nullptr };					//箒
				this->m_ItemSel = 0;
				this->m_Item_Ptrs.clear();
				this->m_MagicSel = 0;
				this->m_Magic.resize(3);
				this->m_Magic[0].Set("FireBall");
				this->m_Magic[1].Set("Thunder");
				this->m_Magic[2].Set("Sonic");
				//
				this->m_LockOn.reset();
				//動作にかかわる操作
				this->m_InputGround.ValueSet(pxRad, pyRad);
				this->m_InputSky.ValueSet();
				this->m_PosBuf = pPos;
				//上記を反映するもの
				this->m_yrad_Upper = this->m_InputGround.GetRad().y();
				this->m_yrad_Bottom = this->m_InputGround.GetRad().y();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_move.vec.clear();
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_PosBuf);
			}
			void			SetInput(const InputControl& pInput, bool pReady) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;

				bool pressFront = false;
				bool pressFlight = false;

				auto prev = this->m_FlightReturn;
				this->m_FlightReturn = std::max(this->m_FlightReturn - 1.f / FPS, 0.f);
				if (this->m_FlightReturn > 0.f) {
					pressFront = true;
				}
				if (prev > 0 && this->m_FlightReturn == 0.f) {
					pressFlight = true;
				}

				//空
				m_InputSky.SetInput(
					pInput.m_GoFrontPress,
					pInput.m_GoBackPress,
					pInput.m_GoLeftPress,
					pInput.m_GoRightPress,
					pInput.m_GoFlight || pressFlight,
					pInput.m_QPress,
					pInput.m_EPress,
					pInput.m_Press_Accel,
					pInput.m_Press_Brake
				);
				//地
				m_InputGround.SetInput(
					pInput.m_AddxRad*(1.f - GetFlightPer()), pInput.m_AddyRad*(1.f - GetFlightPer()),
					VECTOR_ref::zero(),
					pInput.m_GoFrontPress && !pressFront,
					pInput.m_GoBackPress && !pressFront,
					pInput.m_GoLeftPress && !pressFront,
					pInput.m_GoRightPress && !pressFront,
					pInput.m_RunPress || pressFront,
					m_InputSky.GetIsFlightMode(),
					pInput.m_QPress,
					pInput.m_EPress
				);
				//
				m_Rightkey.GetInput(pInput.m_RightPress);
				m_Leftkey.GetInput(pInput.m_LeftPress);
				m_UseItemkey.GetInput(pInput.m_UseItemPress);
				m_Upkey.GetInput(pInput.m_UpPress);
				m_Downkey.GetInput(pInput.m_DownPress);
				m_UseMagickey.GetInput(pInput.m_UseMagicPress);

				if (m_Rightkey.trigger()) {
					AddItemSel();
				}
				if (m_Leftkey.trigger()) {
					SubItemSel();
				}
				if (m_UseItemkey.trigger() && (this->m_Item_Ptrs.size() > 0)) {
					if (!this->m_SetMagic && !this->m_UsingItem && !this->m_UsingMagic) {
						this->m_UsingItem = true;
						GetAnime(CharaAnimeID::Upper_UseItem).GoStart();
						GetAnime(CharaAnimeID::Upper_FlightUseItem).GoStart();
					}
				}
				if (m_Downkey.trigger()) {
					AddMagicSel();
					if (!this->m_SetMagic && !this->m_UsingItem && !this->m_UsingMagic) {
						this->m_SetMagic = true;
						this->m_SetMagicTime = 1.f;
					}
				}
				if (m_Upkey.trigger()) {
					SubMagicSel();
					if (!this->m_SetMagic && !this->m_UsingItem && !this->m_UsingMagic) {
						this->m_SetMagic = true;
						this->m_SetMagicTime = 1.f;
					}
				}
				if (m_UseMagickey.trigger()) {
					if (!this->m_SetMagic && !this->m_UsingItem && !this->m_UsingMagic) {
						this->m_SetMagic = true;
						this->m_SetMagicTime = 1.f;
					}
				}
				if (m_UseMagickey.trigger() && (true)) {
					if ((this->m_SetMagicPer > 0.95f) && !this->m_UsingMagic) {
						if (this->m_Magic[this->m_MagicSel].Use()) {
							this->m_UsingMagic = true;
							this->m_MagicEffectStart = true;
							this->m_MagicEffectLoop = true;
							GetAnime(CharaAnimeID::Upper_UseMagic1).GoStart();
							GetAnime(CharaAnimeID::Upper_FlightUseMagic1).GoStart();
						}
					}
				}
				//
				{
					auto TmpReady = GetIsRun();
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
			}
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept {
				this->GetObj().frame_Reset(this->m_Frames[(int)CharaFrame::Head].first);
				auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
				auto v2 = Lerp(MATRIX_ref::Vtrans(camvec.Norm(), GetCharaDir().Inverse()), v1, m_NeckPer);

				auto radlimit = deg2rad(70);
				if (v1.dot(v2) <= cos(radlimit)) {
					v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
				}
				SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
			}

			void			AddItem(std::shared_ptr<ItemClass>& pItemPtr) noexcept {
				for (auto& i : this->m_Item_Ptrs) {
					if (i.first == pItemPtr->GetItemType()) {
						i.second.emplace_back(pItemPtr);
						i.second.back()->SetCharaPtr(true);
						return;
					}
				}
				this->m_Item_Ptrs.resize(this->m_Item_Ptrs.size() + 1);
				this->m_Item_Ptrs.back().first = pItemPtr->GetItemType();
				this->m_Item_Ptrs.back().second.emplace_back(pItemPtr);
				this->m_Item_Ptrs.back().second.back()->SetCharaPtr(true);
			}
			void			DelItem(ItemType id) noexcept {
				for (int i = 0; i < this->m_Item_Ptrs.size(); i++) {
					if (this->m_Item_Ptrs[i].first == id) {
						if (this->m_Item_Ptrs[i].second.size() > 0) {
							this->m_Item_Ptrs[i].second.back()->SetCharaPtr(false);
							this->m_Item_Ptrs[i].second.back()->SetUsed(true);
							this->m_Item_Ptrs[i].second.pop_back();
							if (this->m_Item_Ptrs[i].second.size() == 0) {
								std::swap(this->m_Item_Ptrs[i], this->m_Item_Ptrs.back());
								this->m_Item_Ptrs.pop_back();
								SubItemSel();
							}
						}
						else {
							//エラー
						}
						return;
					}
				}
			}
			const auto		GetItemNum(void) const noexcept { return this->m_Item_Ptrs.size(); }//カテゴリごとのアイテム数
			const auto&		GetItem(int id) const noexcept { return this->m_Item_Ptrs[id].second.front(); }
			const auto		GetItemCnt(int id) const noexcept { return this->m_Item_Ptrs[id].second.size(); }
			const auto&		GetItemSel() const noexcept { return this->m_ItemSel; }
			void			AddItemSel() noexcept {
				this->m_ItemSel++;
				if (this->m_ItemSel > this->m_Item_Ptrs.size() - 1) {
					this->m_ItemSel = 0;
				}
			}
			void			SubItemSel() noexcept {
				this->m_ItemSel--;
				if (this->m_ItemSel < 0) {
					this->m_ItemSel = std::max(0, (int)(this->m_Item_Ptrs.size()) - 1);
				}
			}
			//
			const auto&		GetMagicCoolDown(int ID) const noexcept { return this->m_Magic[ID].GetCoolDown(); }
			const auto&		GetMagicCoolFrame(int ID) const noexcept { return this->m_Magic[ID].GetCoolFrame(); }
			const auto&		GetMagicName(int ID) const noexcept { return this->m_Magic[ID].GetName(); }
			const auto		GetMagicNum() const noexcept { return this->m_Magic.size(); }
			const auto&		GetMagicSel() const noexcept { return this->m_MagicSel; }
			void			AddMagicSel() noexcept {
				this->m_MagicSel++;
				if (this->m_MagicSel > this->m_Magic.size() - 1) {
					this->m_MagicSel = 0;
				}
			}
			void			SubMagicSel() noexcept {
				this->m_MagicSel--;
				if (this->m_MagicSel < 0) {
					this->m_MagicSel = std::max(0, (int)(this->m_Magic.size()) - 1);
				}
			}

			void			SetLockOn(const std::shared_ptr<CharacterClass>& pLockOn) noexcept { this->m_LockOn = pLockOn; }
			const auto&		GetLockOn() const noexcept { return this->m_LockOn; }
		private: //更新関連
			//以前の状態保持														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//操作																	//0.01ms
			void			ExecuteInput(void) noexcept {
				//
				this->m_SlingHoukiPer = 1.f - GetFlightPer();
				//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_rad.z()決定
				this->m_InputGround.Execute();
				this->m_InputSky.Execute();
				if (this->m_Speed <= 0.1f) {
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) > deg2rad(50.f)) {
						this->m_TurnBody = true;
					}
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}
				//
				auto FrontP = this->m_InputGround.GetFrontP();
				auto TmpRunPer = Lerp(0.85f, 0.7f, this->m_InputGround.GetRunPer());
				if (m_InputSky.GetIsFlightMode()) { TmpRunPer = 0.95f; }
				if (this->m_TurnBody || (this->m_Speed > 0.1f) || m_InputSky.GetIsFlightMode()) { Easing(&this->m_yrad_Upper, this->m_InputGround.GetRad().y(), TmpRunPer, EasingType::OutExpo); }
				auto OLDP = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);

				if (m_InputSky.GetIsFlightMode()) {
					this->m_InputGround.SetRadBufX(0.f);
				}
				this->m_InputGround.SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);
			}
			//上半身回転															//0.06ms
			void			ExecuteUpperMatrix(void) noexcept {
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->GetObj().frame_Reset(this->m_Frames[(int)CharaFrame::Upper].first);
				SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * Lerp_Matrix(this->m_UpperMatrix, MATRIX_ref::zero(), GetFlightPer()));
			}
			//SetMat指示															//0.03ms
			auto*		GetLatestBulletMove(void) noexcept {
				auto Now = this->m_NowBullet - 1;
				if (Now < 0) { Now = (int)(this->m_Bullet.size()) - 1; }
				return this->m_Bullet[Now].Get_Move();
			}
			void			ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						this->m_SendCamShake = false;
						this->m_UpperAnimSelect = CharaAnimeID::Upper_Down;
						if (this->m_SetMagic) {
							this->m_UpperAnimSelect = CharaAnimeID::Upper_SetMagic;
						}
						if (this->m_UsingMagic) {
							this->m_UpperAnimSelect = CharaAnimeID::Upper_UseMagic1;
							SetAnimOnce(CharaAnimeID::Upper_UseMagic1, 1.5f);
							SetAnimOnce(CharaAnimeID::Upper_FlightUseMagic1, 1.5f);
							switch (this->m_Magic[this->m_MagicSel].GetGetType()) {
							case MagicType::FireBall:
								if (this->m_MagicEffectStart && GetAnime(CharaAnimeID::Upper_UseMagic1).time > 30) {
									MATRIX_ref mat = GetFrameWorldMat(CharaFrame::RightHandJoint);
									Effect_UseControl::Set_Effect(Effect::ef_FireBallStart, mat.pos(), GetCharaDir().yvec(), 0.25f);
									this->m_MagicEffectStart = false;
								}
								if (this->m_MagicEffectLoop && GetAnime(CharaAnimeID::Upper_UseMagic1).time > 35) {
									MATRIX_ref mat = GetFrameWorldMat(CharaFrame::RightHandJoint);
									Effect_UseControl::Set_LoopEffect(Effect::ef_FireBallLoop, mat.pos());
									this->m_MagicEffectLoop = false;

									this->m_Bullet[this->m_NowBullet].Set(MATRIX_ref::RotVec2(VECTOR_ref::front() * -1.f, GetCharaDir().zvec() * -1.f).GetRot(), mat.pos(), BulletType::FireBall, this->m_FlightSpeed);
									++this->m_NowBullet %= this->m_Bullet.size();
								}
								break;
							case MagicType::Thunder:
								if (this->m_MagicEffectStart && GetAnime(CharaAnimeID::Upper_UseMagic1).time > 0) {
									MATRIX_ref mat = GetFrameWorldMat(CharaFrame::RightHandJoint);
									Effect_UseControl::Set_LoopEffect(Effect::ef_ThunderStart, mat.pos());
									this->m_MagicEffectStart = false;
								}
								if (this->m_MagicEffectLoop && GetAnime(CharaAnimeID::Upper_UseMagic1).time > 40) {
									MATRIX_ref mat = GetFrameWorldMat(CharaFrame::RightHandJoint);
									Effect_UseControl::Set_LoopEffect(Effect::ef_ThunderLoop, mat.pos());
									this->m_MagicEffectLoop = false;

									this->m_Bullet[this->m_NowBullet].Set(GetCharaDir(), mat.pos(), BulletType::Thunder, this->m_FlightSpeed);
									++this->m_NowBullet %= this->m_Bullet.size();
								}
								break;
							case MagicType::SearchSonic:
								if (this->m_MagicEffectStart && GetAnime(CharaAnimeID::Upper_UseMagic1).time > 30) {
									MATRIX_ref mat = GetFrameWorldMat(CharaFrame::Upper);
									if (this->m_InputSky.GetIsFlightMode()) {
										Effect_UseControl::Set_Effect(Effect::ef_Sonic, mat.pos() + GetCharaDir().zvec()*-1.f * this->m_FlightSpeed * Frame_Rate / FPS, GetCharaDir().yvec(), 1.f);
									}
									else {
										Effect_UseControl::Set_Effect(Effect::ef_Sonic, mat.pos(), GetCharaDir().yvec(), 1.f);
									}
									this->m_MagicEffectStart = false;
								}
								if (this->m_MagicEffectLoop && GetAnime(CharaAnimeID::Upper_UseMagic1).time > 35) {
									this->m_SendCamShake = true;
									this->m_MagicEffectLoop = false;
								}

								break;
							default:
								break;
							}

							if (GetAnime(CharaAnimeID::Upper_UseMagic1).TimeEnd()) {
								this->m_UsingMagic = false;
							}
						}

						{
							auto* nowptr = GetLatestBulletMove();
							switch (this->m_Magic[this->m_MagicSel].GetGetType()) {
							case MagicType::FireBall:
								Effect_UseControl::SetSpeed_Effect(Effect::ef_FireBallStart, 2.f);
								if (nowptr != nullptr) {
									Effect_UseControl::Update_LoopEffect(Effect::ef_FireBallLoop, nowptr->pos, nowptr->mat.yvec(), 1.0f);
									if (this->m_LockOn != nullptr) {
										Easing_Matrix(&nowptr->mat,
											MATRIX_ref::RotVec2(VECTOR_ref::front() * -1.f, (this->m_LockOn->GetMatrix().pos() - nowptr->pos).Norm()).GetRot(),
											0.96f, EasingType::OutExpo);
									}

								}
								else {
									Effect_UseControl::Stop_Effect(Effect::ef_FireBallLoop);
								}
								break;
							case MagicType::Thunder:
								if (!this->m_MagicEffectStart) {
									MATRIX_ref mat = GetFrameWorldMat(CharaFrame::RightHandJoint);
									Effect_UseControl::Update_LoopEffect(Effect::ef_ThunderStart, mat.pos(), GetCharaDir().zvec()*-1.f, 0.1f);
								}
								if (nowptr != nullptr) {
									Effect_UseControl::Update_LoopEffect(Effect::ef_ThunderLoop, nowptr->pos, nowptr->mat.yvec(), 0.25f);
								}
								break;
							case MagicType::SearchSonic:
								Effect_UseControl::SetSpeed_Effect(Effect::ef_Sonic, 1.5f);
								break;
							default:
								break;
							}

							for (auto& b : this->m_Bullet) {
								if (b.CheckBullet(this->m_MapCol)) {
									auto mat = b.GetMoveHit();
									switch (b.GetType()) {
									case BulletType::FireBall:
										Effect_UseControl::Set_Effect(Effect::ef_FireBallHit, mat.pos, mat.vec, 0.1f);
										break;
									case BulletType::Thunder:
										Effect_UseControl::Set_Effect(Effect::ef_ThunderHit, mat.pos, mat.vec, 0.5f);
										if ((mat.pos - GetMatrix().pos()).size() < Scale_Rate*10.f) {
											this->m_SendCamShake = true;
										}
										break;
									default:
										break;
									}
									b.SetHit();
								}
							}
							Effect_UseControl::SetSpeed_Effect(Effect::ef_FireBallHit, 2.f);
							Effect_UseControl::SetSpeed_Effect(Effect::ef_ThunderHit, 2.f);

						}

						if (this->m_UsingItem) {
							this->m_UpperAnimSelect = CharaAnimeID::Upper_UseItem;
							SetAnimOnce(CharaAnimeID::Upper_UseItem, 1.f);
							SetAnimOnce(CharaAnimeID::Upper_FlightUseItem, 1.f);
							if (GetAnime(CharaAnimeID::Upper_UseItem).TimeEnd()) {
								this->m_UsingItem = false;
								AddHP(30.f);
								DelItem(this->GetItem(this->m_ItemSel)->GetItemType());
							}
						}
						if (this->m_ReadySwitch) {
							this->m_RunReadyFirst = false;
							GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
						}
						if (!this->m_RunReady && !this->m_Running && !GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
						}
						if (this->m_RunReadyFirst) {
							GetAnime(CharaAnimeID::Upper_RunningStart).GoStart();
							GetAnime(CharaAnimeID::Upper_RunningStart).per = 1.f;
						}
						if (!this->m_RunReady) {
							this->m_Running = false;
						}
						else if (!this->m_Running) {
							SetAnimOnce(CharaAnimeID::Upper_RunningStart, 2.f);
							this->m_InputGround.SetSprintPer(0.f);
							if (GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
								this->m_Running = true;
								GetAnime(CharaAnimeID::Upper_Running).GoStart();
							}
							m_UpperAnimSelect = CharaAnimeID::Upper_RunningStart;
						}
						else {
							m_UpperAnimSelect = (!this->m_InputGround.GetSprint()) ? CharaAnimeID::Upper_Running : CharaAnimeID::Upper_Sprint;
						}
						//首
						Easing(&m_NeckPer, (
							m_UpperAnimSelect == CharaAnimeID::Upper_Down
							|| m_UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
					}
					//下半身
					{
						if (this->m_InputGround.GetPressFront()) {
							this->m_BottomAnimSelect = (this->m_InputGround.GetRun()) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (this->m_InputGround.GetPressLeft()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (this->m_InputGround.GetPressRight()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (this->m_InputGround.GetPressRear()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
							if (this->m_InputGround.GetSprint()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Run; }
						}
					}
					//
					SetAnimLoop(CharaAnimeID::Upper_Sprint, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Upper_Running, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop(CharaAnimeID::Bottom_Run, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Walk, 1.15f * this->m_InputGround.GetVecFront());
					SetAnimLoop(CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_InputGround.GetVecLeft());
					SetAnimLoop(CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_InputGround.GetVecRear());
					SetAnimLoop(CharaAnimeID::Bottom_RightStep, 1.15f * this->m_InputGround.GetVecRight());
				}
				//アニメセレクト
				{
					//上半身
					if (this->m_SetMagic && (this->m_SetMagicTime > 0.f)) {
						this->m_SetMagicTime -= 1.f / FPS;
						if (this->m_UsingMagic) {
							this->m_SetMagicTime = 1.f;
						}
						if (this->m_SetMagicTime <= 0.f) {
							this->m_SetMagicTime = 0.f;
							this->m_SetMagic = false;
						}
					}
					Easing(&this->m_SetMagicPer, (this->m_SetMagic) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					GetAnimeBuf(CharaAnimeID::Upper_FlightNormal) = 1.f - this->m_SetMagicPer;
					GetAnimeBuf(CharaAnimeID::Upper_FlightSetMagic) = this->m_SetMagicPer;
					GetAnimeBuf(CharaAnimeID::Upper_FlightUseItem) = 0.f;
					GetAnimeBuf(CharaAnimeID::Upper_FlightUseMagic1) = 0.f;
					if (this->m_UsingItem) {
						GetAnimeBuf(CharaAnimeID::Upper_FlightNormal) = 0.f;
						GetAnimeBuf(CharaAnimeID::Upper_FlightUseItem) = 1.f;
					}
					if (this->m_UsingMagic) {
						GetAnimeBuf(CharaAnimeID::Upper_FlightSetMagic) = 0.f;
						GetAnimeBuf(CharaAnimeID::Upper_FlightNormal) = 0.f;
						GetAnimeBuf(CharaAnimeID::Upper_FlightUseMagic1) = 1.f;
					}
					//下半身
					if (!this->m_FlightKickReturnAnimOne) {
						GetAnimeBuf(CharaAnimeID::Bottom_FlightNormal) = 1.f;

						GetAnimeBuf(CharaAnimeID::Bottom_FlightKickLeft) = 0.f;
						GetAnime(CharaAnimeID::Bottom_FlightKickLeft).GoStart();

						GetAnimeBuf(CharaAnimeID::Bottom_FlightKickRight) = 0.f;
						GetAnime(CharaAnimeID::Bottom_FlightKickRight).GoStart();
					}
					else {
						GetAnimeBuf(CharaAnimeID::Bottom_FlightNormal) = 0.f;
						if (this->m_FlightKickReturnAnimLR) {
							GetAnimeBuf(CharaAnimeID::Bottom_FlightKickLeft) = 1.f;
							SetAnimOnce(CharaAnimeID::Bottom_FlightKickLeft, 2.f);
							if (GetAnime(CharaAnimeID::Bottom_FlightKickLeft).TimeEnd()) {
								this->m_FlightKickReturnAnimOne = false;
							}
						}
						else {
							GetAnimeBuf(CharaAnimeID::Bottom_FlightKickRight) = 1.f;
							SetAnimOnce(CharaAnimeID::Bottom_FlightKickRight, 2.f);
							if (GetAnime(CharaAnimeID::Bottom_FlightKickRight).TimeEnd()) {
								this->m_FlightKickReturnAnimOne = false;
							}
						}
					}

					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//その他
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							//i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Upper_Down ||
							i == (int)CharaAnimeID::Upper_RunningStart ||
							i == (int)CharaAnimeID::Upper_Running ||
							i == (int)CharaAnimeID::Upper_Sprint ||
							i == (int)CharaAnimeID::Upper_UseItem ||
							i == (int)CharaAnimeID::Upper_SetMagic ||
							i == (int)CharaAnimeID::Upper_UseMagic1
							)
						{
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_UpperAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						//下半身
						if (
							//i == (int)CharaAnimeID::Bottom_Flight ||
							i == (int)CharaAnimeID::Bottom_Stand ||
							i == (int)CharaAnimeID::Bottom_Run ||
							i == (int)CharaAnimeID::Bottom_Walk ||
							i == (int)CharaAnimeID::Bottom_LeftStep ||
							i == (int)CharaAnimeID::Bottom_RightStep ||
							i == (int)CharaAnimeID::Bottom_WalkBack
							)
						{
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						GetAnimeBuf((CharaAnimeID)i) = std::clamp(GetAnimeBuf((CharaAnimeID)i), 0.f, 1.f);
						//反映
						GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i);
						if (
							i == (int)CharaAnimeID::Bottom_FlightKickLeft ||
							i == (int)CharaAnimeID::Bottom_FlightKickRight ||
							i == (int)CharaAnimeID::Bottom_FlightNormal ||
							i == (int)CharaAnimeID::Upper_FlightUseItem ||
							i == (int)CharaAnimeID::Upper_FlightSetMagic ||
							i == (int)CharaAnimeID::Upper_FlightUseMagic1 ||
							i == (int)CharaAnimeID::Upper_FlightNormal
							) {
							GetAnime((CharaAnimeID)i).per = GetAnime((CharaAnimeID)i).per * GetFlightPer();
						}
						else {
							GetAnime((CharaAnimeID)i).per = GetAnime((CharaAnimeID)i).per * (1.f - GetFlightPer());
						}
						//*/
					}
					//
				}
				//足音
				if (!m_InputSky.GetIsFlightMode()) {
					if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Stand) {
						auto Time = GetAnime(this->m_BottomAnimSelect).time;
						if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time && Time < 10.f)) {
								if (this->m_CharaSound != 1) {
									this->m_CharaSound = 1;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 5.f);
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (this->m_CharaSound != 3) {
									this->m_CharaSound = 3;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								}
							}
						}
						else {
							//L
							if (
								(18.f < Time &&Time < 19.f) ||
								(38.f < Time &&Time < 39.f)
								) {
								if (this->m_CharaSound != 5) {
									this->m_CharaSound = 5;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (this->m_CharaSound != 6) {
									this->m_CharaSound = 6;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (this->m_CharaSound != 7) {
							this->m_CharaSound = 7;
							SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f, (int)(192.f * this->m_RunPer2 / SpeedLimit));
						}
						this->m_ReturnStand = false;
					}
				}
				//
				this->GetObj().work_anime();
			}
			//SetMat指示更新														//0.03ms
			void			ExecuteMatrix(void) noexcept {
				this->m_RunPer2 = Lerp(0.35f, (SpeedLimit * (1.f + 0.5f * this->m_InputGround.GetSprintPer())), this->m_InputGround.GetRunPer());
				if (this->m_PrevRunPer2 == 0.f) {
					this->m_PrevRunPer2 = this->m_RunPer2;
				}

				auto OLDpos = this->m_PosBuf;
				VECTOR_ref vecBuf = MATRIX_ref::Vtrans(this->m_InputGround.GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
				this->m_Speed = std::clamp(vecBuf.size(), 0.f, 1.f);

				if (this->m_Speed > 0.1f) {
					this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * Frame_Rate / FPS).x());
					this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * Frame_Rate / FPS).z());
				}
				else {
					this->m_move.vec.x(vecBuf.x());
					this->m_move.vec.z(vecBuf.z());
				}
				{
					auto FlightSpeed = this->m_FlightSpeed * 1000.f / 3600.f * (Scale_Rate / FPS);
					auto HitResult = this->m_MapCol->CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, this->m_PosBuf + VECTOR_ref::up() * std::max(15.f, FlightSpeed));
					if (HitResult.HitFlag == TRUE) {
						auto yPos = this->m_PosBuf.y();
						Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
						this->m_PosBuf.y(yPos);
						if (this->m_move.vec.y() != 0.f) {
							if (this->m_FlightSpeed_r <= FlightSpeedMin + (FlightSpeedMax - FlightSpeedMin) *0.3f) {
								if (m_InputSky.GetIsFlightMode()) {
									if (this->m_FlightReturn == 0.f) {
										this->m_FlightReturn = 5.f;
									}
									//
									VECTOR_ref cross = VECTOR_ref::front().cross(this->m_FlightMatrix.zvec());
									float dotFront = VECTOR_ref::front().dot(this->m_FlightMatrix.zvec());
									float dotUp = VECTOR_ref::up().dot(cross);
									this->m_yrad_Upper = std::atan2f(cross.size()*(dotUp >= 0.f ? 1.f : -1.f), dotFront);
									this->m_yrad_Bottom = this->m_yrad_Upper;
									this->m_InputGround.SetRadBufY(this->m_yrad_Upper);
								}
								m_InputSky.SetIsFlightMode(false);
							}
							else {
								if (!this->m_FlightKickReturn) {
									this->m_FlightKickReturn = true;
									this->m_FlightKickReturnSpeed = 1.f;
									this->m_FlightKickReturnVecBuf = HitResult.Normal;
									this->m_FlightKickReturnAnimOne = true;

									VECTOR_ref cross = this->GetCharaDir().yvec().cross(this->m_FlightKickReturnVecBuf);
									this->m_FlightKickReturnAnimLR = (this->GetCharaDir().zvec().dot(cross) > 0.f);
									//高速ではダメージ
									auto spd = FlightSpeedMin + (FlightSpeedMax - FlightSpeedMin) *0.6f;
									if (this->m_FlightSpeed_r >= spd) {
										SubHP((this->m_FlightSpeed_r - spd) / 10.f);
									}
								}
							}
						}
						this->m_move.vec.y(0.f);
					}
					else {
						this->m_FlightKickReturn = false;
						if (!m_InputSky.GetIsFlightMode()) {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
						else {

							if (this->m_FlightSpeed_r >= FlightSpeedMin) {
								this->m_move.vec.y(0.01f);
							}
							else {
								this->m_move.vec.yadd(M_GR / (FPS * FPS));
							}
						}
					}
				}
				this->m_PosBuf += this->m_move.vec;
				//
				if (m_InputSky.GetPressAccel()) {
					this->m_FlightSpeedAdd = std::clamp(this->m_FlightSpeedAdd + 0.1f / FPS, 0.f, 0.3f);
				}
				else if (m_InputSky.GetPressBrake() && (this->m_FlightSpeed_r >= FlightSpeedMin)) {
					this->m_FlightSpeedAdd = std::clamp(this->m_FlightSpeedAdd - 10.f / FPS, -0.75f, 0.f);
				}
				else {
					float per = GetCharaDir().zvec().y();
					Easing(&this->m_FlightSpeedAdd, (per >= 0) ? (per*0.2f) : (per*0.05f), 0.95f, EasingType::OutExpo);
				}
				this->m_FlightSpeed_r = std::clamp(this->m_FlightSpeed_r + this->m_FlightSpeedAdd*Frame_Rate / FPS, 0.f, FlightSpeedMax*std::min(1.f, (2.f - (GetMatrix().pos().y() / Scale_Rate) / FlightHightLimit)));
				Easing(&this->m_FlightSpeed, this->m_FlightSpeed_r, 0.95f, EasingType::OutExpo);
				//
				{
					Easing(&this->m_FlightKickReturnVec, this->m_FlightKickReturnVecBuf*this->m_FlightKickReturnSpeed*200.f, 0.8f, EasingType::OutExpo);
					this->m_FlightKickReturnSpeed = std::max(this->m_FlightKickReturnSpeed - 1.f / FPS, 0.f);
				}
				//
				if (m_InputSky.GetIsFlightMode()) {
					Easing(&this->m_FlightVecBuf,
						(this->GetCharaDir().zvec() * -1.f + VECTOR_ref::up() * m_InputSky.GetGoFlight()) * this->m_FlightSpeed * 1000.f / 3600.f * (Scale_Rate / FPS)
						, 0.95f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_FlightSpeedAdd, 0.f, 0.95f, EasingType::OutExpo);
					this->m_FlightSpeed_r = FlightSpeedMin + 30.f;
					Easing(&this->m_FlightSpeed, this->m_FlightSpeed_r, 0.95f, EasingType::OutExpo);

					Easing(&this->m_FlightVecBuf, VECTOR_ref::zero(), 0.975f, EasingType::OutExpo);
				}
				this->m_PosBuf += this->m_FlightVecBuf + this->m_FlightKickReturnVec * 1000.f / 3600.f* (Scale_Rate / FPS);
				auto OLDbuf = this->m_PosBuf;
				if (col_wall(OLDpos, &this->m_PosBuf, *this->m_MapCol)) {
					if (m_InputSky.GetIsFlightMode()) {
						if (!this->m_FlightKickReturn) {
							this->m_FlightKickReturn = true;
							this->m_FlightKickReturnSpeed = 1.f;
							this->m_FlightKickReturnVecBuf = (this->m_PosBuf - OLDbuf).Norm();
							this->m_FlightKickReturnAnimOne = true;

							VECTOR_ref cross = this->GetCharaDir().yvec().cross(this->m_FlightKickReturnVecBuf);
							this->m_FlightKickReturnAnimLR = (this->GetCharaDir().zvec().dot(cross) > 0.f);
							//高速ではダメージ
							auto spd = FlightSpeedMin + (FlightSpeedMax - FlightSpeedMin) *0.6f;
							if (this->m_FlightSpeed_r >= spd) {
								SubHP(HPMax);
							}
						}
					}
				}
				if (!m_InputSky.GetIsFlightMode()) {
					auto mat_tmp = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
					Easing_Matrix(&this->m_move.mat, mat_tmp, 0.8f, EasingType::OutExpo);
					this->m_FlightMatrix = this->m_move.mat;
				}
				else {
					float per = Lerp(1.f, 0.5f, this->m_FlightSpeed / (FlightSpeedMax - FlightSpeedMin));

					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_InputSky.GetFradAdd().x()*per);
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_InputSky.GetFradAdd().y()*per);
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.yvec(), this->m_InputSky.GetFradAdd().z()*per);
					Easing_Matrix(&this->m_move.mat, this->m_FlightMatrix, 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
				//
				if (this->m_Houki_Ptr != nullptr) {
					//持ち手探索
					VECTOR_ref yVec1, zVec1, Pos1;
					{
						auto mat = (this->m_move.mat*GetCharaDir().Inverse());
						yVec1 = mat.yvec();
						zVec1 = mat.zvec()*-1.f;
						if (this->m_SetMagicPer > 0.05f) {
							Pos1 = GetFrameWorldMat(CharaFrame::LeftHandJoint).pos();
						}
						else {
							Pos1 = GetFrameWorldMat(CharaFrame::RightHandJoint).pos();
						}
					}
					//背負い場所探索
					VECTOR_ref yVec2, zVec2, Pos2;
					{
						yVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).xvec();
						zVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).yvec();
						Pos2 =
							GetFrameWorldMat(CharaFrame::Upper2).pos() +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().yvec() * -1.75f +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().zvec() * 1.75f;
					}
					auto yVec = Lerp(yVec1, yVec2, this->m_SlingHoukiPer);
					auto zVec = Lerp(zVec1, zVec2, this->m_SlingHoukiPer);
					auto PosBuf = Lerp(Pos1, Pos2, this->m_SlingHoukiPer);
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1).Norm(), zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);

					this->m_Houki_Ptr->SetMove(tmp_gunrat.GetRot(), tmp_gunrat.pos());
				}
			}
			//顔																	//0.01ms
			void			ExecuteShape(void) noexcept {
				SetShapePer(CharaShape::None, 0.f);
				if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
					this->m_Eyeclose = 1;
				}
				if (this->m_EyeclosePer >= 0.95f) {
					this->m_Eyeclose = 0;
				}
				Easing(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f, EasingType::OutExpo);
				SetShapePer(CharaShape::EYECLOSE, this->m_EyeclosePer);
			}
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), VECTOR_ref::vget(0.f, 0.f, 0.f));
				this->m_Houki_Ptr.reset();
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per = 1.f;
				GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
				this->m_Name = "Name";
				this->m_CharaType = CharaTypeID::Enemy;
			}
			void			Execute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->GetObj().get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					this->GetObj().work_anime();
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteUpperMatrix();		//上半身回転//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
				if (this->m_UsingItem) {
					this->GetItem(this->m_ItemSel)->SetHand(
						MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 1, 0), VECTOR_ref::vget(0, 0, -1))
						*GetFrameWorldMat(CharaFrame::LeftHandJoint).GetRot(),
						GetFrameWorldMat(CharaFrame::LeftHandJoint).pos());
				}
				for (auto& b : this->m_Bullet) {
					b.Execute();
				}
				for (auto& m : this->m_Magic) {
					m.Execute();
				}
				Effect_UseControl::Update_Effect();
			}
			//void			Draw(void) noexcept override { ObjectBaseClass::Draw(); }
			void			Dispose(void) noexcept override {
				ObjectBaseClass::Dispose();
				Effect_UseControl::Dispose_Effect();
			}
		};
	};
};
