#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"VehicleEnum.hpp"
#include	"VehicleData.hpp"

#include "Ammo.hpp"

namespace FPS_n2 {
	namespace Objects {
		//���
		class VehicleObj : public BaseObject, public EffectControl {
		private:
			//����
			std::array<bool, 7>									m_key{ false };									//�L�[
			Matrix3x3DX											m_MouseVec;										//�}�E�X�G�C���p�ϐ��m��
			float												m_AimingDistance{ 100.f*Scale3DRate };			//
			bool												m_view_override{ false };						//
			std::vector<Vector3DX>								m_view_rad;										//
			float												m_range{ 6.0f };								//
			float												m_range_r{ this->m_range };						//
			float												m_range_change{ this->m_range / 10.f };			//
			float												m_ratio{ 1.f };									//
			bool												m_changeview{ false };							//
			//�ԑ�
			const VhehicleData*									m_VecData{ nullptr };							//�ŗL�l
			std::vector<Guns>									m_Gun;											//
			float												m_speed{ 0.f };									//�ړ����x
			float												m_speed_add{ 0.f };								//
			float												m_speed_sub{ 0.f };								//
			Vector3DX											m_BodyNormal{ Vector3DX::up() };				//
			Vector3DX											m_Tilt;											//
			Vector3DX											m_radAdd;										//
			float												m_yradadd_left{ 0.f };							//
			float												m_yradadd_right{ 0.f };							//
			float												m_engine_time{ 0.f };							//�G���W������
			std::vector<HitSortInfo> hitssort;																	//�t���[���ɓ�����������
			std::vector<MV1_COLL_RESULT_POLY> hitres;															//�m��
			bool												m_PosBufOverRideFlag{ false };					//
			Vector3DX											m_PosBufOverRide;								//
			Vector3DX											m_VecBufOverRide;								//
			Vector3DX											m_RadOverRide;									//
			//�]��
			float												m_wheel_Left{ 0.f };							//�]�։�]
			float												m_wheel_Right{ 0.f };							//�]�։�]
			std::vector<float>									m_wheel_frameYpos{ 0.f };						//�]�ւ�Y�����ێ�
			float												m_spd_buf{ 0.f };								//BOX2D
			std::array<FootWorld, 2>							m_b2Foot;										//����BOX2D
		public:			//setter,getter
			void			SetAimingDistance(float value) noexcept { this->m_AimingDistance = value; }
			const auto&		GetName(void) const noexcept { return this->m_VecData->GetName(); }
			const auto&		Get_module_mesh(void) const noexcept { return this->m_VecData->Get_module_mesh(); }
			const auto&		GetLookVec(void) const noexcept { return this->m_MouseVec; }
			const auto&		Gunloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].Getloadtime(); }
			const auto&		GetTotalloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].GetTotalloadtime(); }
			const auto&		GetGun(void) const noexcept { return this->m_Gun; }
			const auto		Get_Gunsize(void) const noexcept { return this->m_Gun.size(); }
			const auto&		Get_changeview(void) const noexcept { return this->m_changeview; }																	//�Ə��ύX��
			const auto&		GetViewRad(void) const noexcept { return this->m_view_rad[0]; }
			const auto&		Get_ratio(void) const noexcept { return this->m_ratio; }																			//UI�p
			const auto&		GetAimingDistance(void) const noexcept { return this->m_AimingDistance; }
			const auto		Get_body_yrad(void) const noexcept { auto pp = GetMove().GetMat().zvec() * -1.f; return std::atan2f(pp.x, pp.z); }
			const auto		is_ADS(void) const noexcept { return this->m_range == 0.f; }																		//ADS��
			const auto		GetGunMuzzleMatrix(int ID) const noexcept { return GetObj().GetFrameLocalWorldMatrix(this->m_Gun[ID].GetGunTrunnionFrameID()).rotation(); }
			const auto		GetGunMuzzleBase(int ID) const noexcept { return GetObj().GetFramePosition(this->m_Gun[ID].GetGunTrunnionFrameID()); }
			const auto		GetGunMuzzlePos(int ID) const noexcept { return GetObj().GetFramePosition(this->m_Gun[ID].GetGunMuzzleFrameID()); }
			const auto		GetGunMuzzleVec(int ID) const noexcept { return (GetGunMuzzlePos(ID) - GetGunMuzzleBase(ID)).normalized(); }

			const auto		GetSquarePos(int ID) const noexcept { return GetObj().GetFramePosition(this->m_VecData->Get_square(ID)); }

			const auto&		GetRadAdd(void) const noexcept { return this->m_radAdd; }

			const auto		Get_EyePos_Base(void) const noexcept { return (is_ADS()) ? GetGunMuzzleBase(0) : (GetMove().GetPos() + (GetMove().GetMat().yvec() * 3.f * Scale3DRate)); }
			const auto		Set_MidPos(void) noexcept { return (GetMove().GetPos() + (GetMove().GetMat().yvec() * 1.5f * Scale3DRate)); }							//HP�o�[��\������ꏊ
			//
			void			SetPosBufOverRide(const Vector3DX& pos_t, const Vector3DX& pVec, const Vector3DX& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pos_t;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}
		public:
			void			ValueInit(const VhehicleData* pVeh_data) noexcept;
			void			ValueSet(float pxRad, float pyRad, const Vector3DX& pos_t) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;													//
			void			Setcamera(Camera3DInfo& m_MainCamera, const float fov_base) noexcept;																	//�J�����ݒ�o��
			const std::pair<bool, bool>		CheckAmmoHit(Objects::AmmoObj* pAmmo) noexcept;

			const bool		CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) noexcept;

		private://�X�V�֘A
			const auto		CheckAmmoHited(const Objects::AmmoObj& pAmmo) noexcept;																					//��e�`�F�b�N
			const auto		CalcAmmoHited(float EffectSize) noexcept;															//��e����

			void			ExecuteSavePrev(void) noexcept;																										//�ȑO�̏�ԕێ�
			void			ExecuteElse(void) noexcept;																											//���̑�
			void			ExecuteInput(void) noexcept;																										//����
			void			ExecuteFrame(void) noexcept;																										//�t���[������
			void			ExecuteMove(void) noexcept;																											//�ړ�����
			void			DrawCommon(void) noexcept;
		public: //�R���X�g���N�^�A�f�X�g���N�^
			VehicleObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Vehicle); }
			~VehicleObj(void) noexcept {}
		public: //�p��
			void			Init_Sub(void) noexcept override {
				SetActive(true);
				SetMinAABB(Vector3DX::vget(-10.f, -1.f, -10.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(10.f, 10.f, 10.f) * Scale3DRate);
			}
			void			FirstExecute(void) noexcept override {
				auto* SE = SoundPool::Instance();
				//����̂ݍX�V������e
				if (IsFirstLoop()) {
					this->m_view_override = false;

					this->m_engine_time = (float)GetRand(100) / 100.f;
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->Play3D(GetMove().GetPos(), 30.f*Scale3DRate, DX_PLAYTYPE_LOOP);
				}
				ExecuteSavePrev();			//�ȑO�̏�ԕێ�
				ExecuteElse();				//���̑�
				ExecuteInput();				//����
				ExecuteFrame();				//�t���[������
				ExecuteMove();				//�ړ�����
			}
			void			DrawShadow(void) noexcept override {
				if (!is_ADS()) {
					GetObj().DrawModel();
				}
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				DrawCommon();
			}
			void			Dispose_Sub(void) noexcept override {
				for (auto& f : this->m_b2Foot) {
					f.Dispose();
				}
				this->m_wheel_frameYpos.clear();
				for (auto& cg : this->m_Gun) {
					cg.Dispose();
				}
				this->m_Gun.clear();
				this->hitres.clear();
				this->hitssort.clear();
			}
		};
	};
};
