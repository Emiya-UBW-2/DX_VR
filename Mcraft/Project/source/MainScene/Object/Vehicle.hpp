#pragma once
#include	"../../Header.hpp"
#include	"VehicleEnum.hpp"
#include	"VehicleData.hpp"

#include "Ammo.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Objects {
		//戦車
		class VehicleObj : public BaseObject, public EffectControl {
			static const PlayerID								m_MyPlayerID{ -1 };//車両はとりあえず-1
			//操作
			InputControl										m_Input;
			Matrix3x3DX											m_MouseVec;								//マウスエイム用変数確保
			float												m_range{ 6.0f };						//
			//車体
			std::unique_ptr<VhehicleData>						m_VecData{ nullptr };					//固有値
			std::vector<Guns>									m_Gun;									//
			//
			float												m_speed{};								//移動速度
			Vector3DX											m_radAdd;								//
			float												m_speed_add{};							//
			float												m_speed_sub{};							//
			float												m_yradadd_left{};						//
			float												m_yradadd_right{};						//
			Vector3DX											m_BodyNormal{ Vector3DX::up() };		//
			Vector2DX											m_Tilt;									//
			//
			float												m_engine_time{};						//エンジン音声
			std::vector<HitSortInfo>							m_hitssort;								//フレームに当たった順番
			std::vector<MV1_COLL_RESULT_POLY>					m_hitres;								//確保
			//転輪
			float												m_CrawlerRotateLeft{};					//転輪回転
			float												m_CrawlerRotateRight{};					//転輪回転
			std::vector<float>									m_WheelHeight{};						//転輪のY方向保持
			std::array<std::vector<CrawlerFrameControl>, 2>		m_CrawlerFrame;							//履帯
			//
			DamageEventControl									m_Damage;
		public:
			void			Spawn(float yRad, const Vector3DX& pos) noexcept {
				SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), yRad));
				SetMove().SetPos(pos);
				SetMove().SetVec(Vector3DX::zero());
				SetMove().Update(0.0f, 0.0f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				for (auto& height : this->m_WheelHeight) { height = 0.0f; }

				this->m_MouseVec = GetMove().GetMat();
				//戦車スポーン
				this->m_speed = 0.0f;
				this->m_speed_add = 0.0f;
				this->m_speed_sub = 0.0f;
				this->m_yradadd_left = 0.0f;
				this->m_yradadd_right = 0.0f;
				this->m_Tilt = Vector2DX::zero();
				this->m_radAdd = Vector3DX::zero();
			}
			void			SetInput(const InputControl& pInput, bool pReady) noexcept {
				//エイム
				auto Vec = this->m_MouseVec.zvec();
				auto y_mouse = std::atan2f(Vec.x, Vec.z) + pInput.GetAddyRad();
				auto x_mouse = std::clamp(std::atan2f(-Vec.y, std::hypotf(Vec.x, Vec.z)) + pInput.GetAddxRad(), -deg2rad(40.0f), deg2rad(40.0f));
				this->m_MouseVec = Matrix3x3DX::RotAxis(Vector3DX::right(), x_mouse) * Matrix3x3DX::RotAxis(Vector3DX::up(), y_mouse);
				//
				if (pReady) {
					this->m_Input = pInput;
				}
				else {
					this->m_Input.ResetAllInput();
				}
			}
			void			SetCam(Camera3DInfo* pMainCamera) noexcept {
				this->m_range = std::clamp(this->m_range - float(PadControl::Instance()->GetMouseWheelRot()), 1.0f, 4.0f);
				Vector3DX eyeposBase = GetMove().GetPos() + (GetMove().GetMat().yvec() * (3.0f * Scale3DRate));
				pMainCamera->SetCamPos(
					eyeposBase + this->m_MouseVec.zvec() * (this->m_range * Scale3DRate) + Camera3D::Instance()->GetCamShake() * 10.0f,
					eyeposBase + this->m_MouseVec.zvec() * -std::max(this->m_range * Scale3DRate, 1.0f) + Camera3D::Instance()->GetCamShake()*2.0f * 10.0f,
					Lerp(GetMove().GetMat().yvec(), Vector3DX::up(), std::clamp(this->m_range / 3.0f, 0.0f, 1.0f))
				);
			}
			const bool		CheckAmmoHit(const Vector3DX& StartPos, Vector3DX* EndPos) noexcept;
			//一番近い装甲をチェック
			const bool		CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) noexcept {
				if (!RefreshCol(StartPos, *EndPos, 10.0f * Scale3DRate)) {
					return false;
				}
				bool IsHit = false;
				for (const auto& mesh : this->m_VecData->GetArmerMeshIDList()) {
					auto Res = GetColLine(StartPos, *EndPos, mesh.first);
					if (Res.HitFlag) {
						IsHit = true;
						*EndPos = Res.HitPosition;
						if (Normal) {
							*Normal = Res.Normal;
						}
					}
				}
				return IsHit;
			}
			//自分がダメージを与えたと通知
			void			SetDamage(PlayerID damageID, HitPoint damage, int hitType, const Vector3DX& startPos, const Vector3DX& endPos) noexcept {
				this->m_Damage.Add(this->m_MyPlayerID, damageID, damage, hitType, startPos, endPos);
			}
			const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
			void			SetDamageEventReset(void) noexcept { this->m_Damage.Reset(); }
			void			PopDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_Damage.Pop(pRet); }
		public: //コンストラクタ、デストラクタ
			VehicleObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Vehicle); }
			~VehicleObj(void) noexcept {}
		public: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstUpdate(void) noexcept override;
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override;
			void			Dispose_Sub(void) noexcept override {
				this->m_VecData.reset();
				for (auto& frame : this->m_CrawlerFrame) {
					frame.clear();
				}
				this->m_WheelHeight.clear();
				for (auto& cg : this->m_Gun) {
					cg.Dispose();
				}
				this->m_Gun.clear();
				this->m_hitres.clear();
				this->m_hitssort.clear();
			}
		};
	}
}
