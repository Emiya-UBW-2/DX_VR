#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
	//����
	class InputControl {
	private:
		float			m_AddxRad{0.f};
		float			m_AddyRad{0.f};
		float			m_xRad{0.f};
		float			m_yRad{0.f};
		unsigned long long	m_Flags{0};
	public:
		void			ResetKeyInput() {
			this->m_Flags = 0;
		}
		void			ResetAllInput() {
			this->m_AddxRad = 0.f;
			this->m_AddyRad = 0.f;
			this->m_xRad = 0.f;
			this->m_yRad = 0.f;
			this->m_Flags = 0;
		}
		void			SetInputStart(float pAddxRad, float pAddyRad, const Vector3DX& pRad) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;
			this->m_xRad = pRad.x;
			this->m_yRad = pRad.y;
			this->m_Flags = 0;
		}
		void			SetInputPADS(PADS select, bool value) {
			if (value) { this->m_Flags |= ((unsigned long long)1 << (0 + (int)select)); }
		}

		void			SetAddxRad(float AddxRad) { this->m_AddxRad = AddxRad; }
		void			SetAddyRad(float AddyRad) { this->m_AddyRad = AddyRad; }
		void			SetKeyInputFlags(const InputControl& o) { this->m_Flags = o.m_Flags; }

		const auto&		GetAddxRad(void) const noexcept { return this->m_AddxRad; }
		const auto&		GetAddyRad(void) const noexcept { return this->m_AddyRad; }

		const auto	GetPADSPress(PADS select) const noexcept { return (this->m_Flags & ((unsigned long long)1 << (0 + (int)select))) != 0; }

		const auto operator+(const InputControl& o) const noexcept {
			InputControl tmp;
			tmp.m_AddxRad = this->m_AddxRad + o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad + o.m_AddyRad;
			tmp.m_xRad = this->m_xRad + o.m_xRad;
			tmp.m_yRad = this->m_yRad + o.m_yRad;
			tmp.m_Flags = this->m_Flags;
			return tmp;
		}
		const auto operator-(const InputControl& o) const noexcept {
			InputControl tmp;
			tmp.m_AddxRad = this->m_AddxRad - o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad - o.m_AddyRad;
			tmp.m_xRad = this->m_xRad - o.m_xRad;
			tmp.m_yRad = this->m_yRad - o.m_yRad;
			tmp.m_Flags = this->m_Flags;
			return tmp;
		}
		const auto operator*(float per) const noexcept {
			InputControl tmp;
			tmp.m_AddxRad = this->m_AddxRad *per;
			tmp.m_AddyRad = this->m_AddyRad *per;
			tmp.m_xRad = this->m_xRad *per;
			tmp.m_yRad = this->m_yRad *per;
			tmp.m_Flags = this->m_Flags;
			return tmp;
		}

		void operator=(const InputControl& o) noexcept {
			this->m_AddxRad = o.m_AddxRad;
			this->m_AddyRad = o.m_AddyRad;
			this->m_xRad = o.m_xRad;
			this->m_yRad = o.m_yRad;
			this->m_Flags = o.m_Flags;
		}
	};
	//�L��������
	class CharacterMoveGroundControl {
	private:
		std::array<float, 4>								m_Vec{ 0,0,0,0 };
		bool												m_Press_GoFront{ false };
		bool												m_Press_GoRear{ false };
		bool												m_Press_GoLeft{ false };
		bool												m_Press_GoRight{ false };
		switchs												m_Squat;
		switchs												m_QKey;
		switchs												m_EKey;
		bool												m_IsRun{ false };
		float												m_RunPer{ 0.f };
		float												m_RunTimer{ 0.f };
		float												m_SquatPer{ 0.f };
		Vector3DX											m_rad_Buf, m_rad, m_radAdd;
		int													m_TurnRate{ 0 };
		float												m_TurnRatePer{ 0.f };
		int													m_LeanRate{ 0 };
		float												m_LeanRatePer{ 0.f };
	private: //����
		void			SetVec(int pDir, bool Press) {
			this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
			this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
		}
	public:
		const auto		GetRadBuf(void) const noexcept { return  this->m_rad_Buf; }
		const auto		GetTurnRatePer(void) const noexcept { return  this->m_TurnRatePer; }
		const auto		GetLeanRatePer(void) const noexcept { return this->m_LeanRatePer; }
		const auto		GetRad(void) const noexcept { return  this->m_rad; }

		const auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
		const auto		GetSquatPer(void) const noexcept { return this->m_SquatPer; }

		const auto		GetVecFront(void) const noexcept { return  this->m_Vec[0]; }
		const auto		GetVecRear(void) const noexcept { return this->m_Vec[2]; }
		const auto		GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
		const auto		GetVecRight(void) const noexcept { return this->m_Vec[3]; }
		const auto		GetPressFront(void) const noexcept { return this->m_Press_GoFront; }
		const auto		GetPressRear(void) const noexcept { return this->m_Press_GoRear; }
		const auto		GetPressLeft(void) const noexcept { return this->m_Press_GoLeft; }
		const auto		GetPressRight(void) const noexcept { return this->m_Press_GoRight; }
		const auto		GetRun(void) const noexcept { return this->m_IsRun; }
		const auto		GetRunPer(void) const noexcept { return  this->m_RunPer; }
		const auto		GetVec(void) const noexcept { return Vector3DX::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
		const auto		GetFrontP(void) const noexcept {
			auto FrontP = ((GetPressFront() && !GetPressRear())) ? (std::atan2f(GetVec().x, -GetVec().z) * GetVecFront()) : 0.f;
			FrontP += (!GetPressFront() && GetPressRear()) ? (std::atan2f(-GetVec().x, GetVec().z) * GetVecRear()) : 0.f;
			return FrontP;
		}

		void			SetRadBufX(float x) noexcept {
			auto xbuf = this->m_rad_Buf.x;
			Easing(&xbuf, x, 0.9f, EasingType::OutExpo);
			this->m_rad_Buf.x = (xbuf);
		}
		void			SetRadBufY(float y) noexcept {
			this->m_rad_Buf.y = (y);
			this->m_rad.y = (y);
		}
		void			SetRadBufZ(float z) noexcept {
			auto zbuf = this->m_rad_Buf.z;
			Easing(&zbuf, z, 0.9f, EasingType::OutExpo);
			this->m_rad_Buf.z = (zbuf);
		}
	public:
		void			ValueSet(float pxRad, float pyRad, bool SquatOn) {
			for (int i = 0; i < 4; i++) {
				this->m_Vec[i] = 0.f;
			}
			this->m_Press_GoFront = false;
			this->m_Press_GoRear = false;
			this->m_Press_GoLeft = false;
			this->m_Press_GoRight = false;
			this->m_radAdd.Set(0, 0, 0);
			this->m_IsRun = false;
			this->m_RunPer = 0.f;
			this->m_RunTimer = 0.f;
			//����ɂ�����鑀��
			this->m_rad_Buf.x = (pxRad);
			this->m_rad_Buf.y = (pyRad);
			this->m_Squat.Set(SquatOn);
			//��L�𔽉f�������
			this->m_rad = this->m_rad_Buf;
			this->m_SquatPer = SquatOn ? 1.f : 0.f;
		}
		void			SetInput(
			float pAddxRad, float pAddyRad,
			const Vector3DX& pAddRadvec,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,
			bool pSquatPress,
			bool pRunPress,
			bool pIsNotActive,
			bool pQPress,
			bool pEPress,
			bool pCannotSprint
		) {
			this->m_Press_GoFront = pGoFrontPress && !pIsNotActive;
			this->m_Press_GoRear = pGoBackPress && !pIsNotActive;
			this->m_Press_GoLeft = pGoLeftPress && !pIsNotActive;
			this->m_Press_GoRight = pGoRightPress && !pIsNotActive;

			if (!this->m_IsRun && (pRunPress && !pIsNotActive)) {
				this->m_RunTimer = 1.f;
			}
			if (this->m_RunTimer > 0.f) {
				this->m_RunTimer -= 1.f / FPS;
				this->m_Press_GoFront = true;
				this->m_IsRun = true;
			}
			else {
				this->m_RunTimer = 0.f;
				this->m_IsRun = (pRunPress && !pIsNotActive);
			}
			if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
				this->m_IsRun = false;
			}
			if (pCannotSprint) {
				this->m_IsRun = false;
			}

			this->m_Squat.Execute(pSquatPress && !pIsNotActive);
			if (this->m_IsRun) { this->m_Squat.Set(false); }

			{
				this->m_QKey.Execute(pQPress && !pIsNotActive);
				this->m_EKey.Execute(pEPress && !pIsNotActive);
				if (this->m_EKey.trigger()) {
					if (this->m_LeanRate == 1) {
						this->m_LeanRate = -1;
					}
					else {
						if (this->m_LeanRate > -1) {
							this->m_LeanRate--;
						}
						else {
							this->m_LeanRate++;
						}
					}

					if (this->m_TurnRate > -1) {
						this->m_TurnRate--;
					}
					else {
						this->m_TurnRate++;
					}
				}
				if (this->m_QKey.trigger()) {
					if (this->m_LeanRate == -1) {
						this->m_LeanRate = 1;
					}
					else {
						if (this->m_LeanRate < 1) {
							this->m_LeanRate++;
						}
						else {
							this->m_LeanRate--;
						}
					}

					if (this->m_TurnRate < 1) {
						this->m_TurnRate++;
					}
					else {
						this->m_TurnRate--;
					}
				}
				if (!GetRun()) {
					this->m_TurnRate = 0;
				}
				else {
					this->m_LeanRate = 0;
				}

				this->m_TurnRate = std::clamp(this->m_TurnRate, -1, 1);
				this->m_LeanRate = std::clamp(this->m_LeanRate, -1, 1);
				float xadd = 0.f;
				if (GetRun()) {
					xadd = 0.2f*(-this->m_TurnRate);//����
				}
				Easing(&this->m_TurnRatePer, xadd, 0.9f, EasingType::OutExpo);

				Easing(&this->m_LeanRatePer, (float)(-this->m_LeanRate), 0.9f, EasingType::OutExpo);
			}
			//��]
			{
				auto limchange = Lerp(1.f, powf(1.f - this->GetVecFront(), 0.5f), this->m_RunPer * 0.8f);
				auto tmp = 1.f;
				Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

				this->m_rad_Buf.x = (
					std::clamp(this->m_rad_Buf.x + pAddxRad * tmp, -deg2rad(80.f) * limchange, deg2rad(80.f) * limchange)
					+ this->m_radAdd.x
				);
				this->m_rad_Buf.y = (
					this->m_rad_Buf.y + (pAddyRad + this->m_TurnRatePer / 100.f)*tmp
					+ this->m_radAdd.y
				);

				Easing(&this->m_rad, this->m_rad_Buf, 0.5f, EasingType::OutExpo);
			}
		}
		void			Execute(void) noexcept {
			//�ړ��x�N�g���擾
			{
				SetVec(0, GetPressFront());
				SetVec(1, GetPressLeft());
				SetVec(2, GetPressRear());
				SetVec(3, GetPressRight());
			}
			//
			Easing(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f, EasingType::OutExpo);
			Easing(&this->m_SquatPer, GetIsSquat() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
		}
	};

	// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale_Rate)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale_Rate)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			(0.4f * Scale_Rate)		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			(16)					// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
	//�ǔ��胆�j�o�[�T��
	static bool col_wall(const Vector3DX& OldPos, Vector3DX* NowPos, const std::vector<MV1*>& col_obj_t) noexcept {
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y = (0);
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		for (const auto& objs : col_obj_t) {
			auto HitDim = objs->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.magnitude());
			// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{PLAYER_ENUM_MIN_SIZE��荂���|���S���̂ݓ����蔻����s��
				if (
					(abs(std::atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
					&& (h_d.Position[0].y > OldPos.y + PLAYER_ENUM_MIN_SIZE || h_d.Position[1].y > OldPos.y + PLAYER_ENUM_MIN_SIZE || h_d.Position[2].y > OldPos.y + PLAYER_ENUM_MIN_SIZE)
					&& (h_d.Position[0].y < OldPos.y + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[1].y < OldPos.y + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[2].y < OldPos.y + PLAYER_ENUM_DEFAULT_SIZE)
					) {
					kabe_.emplace_back(h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
				}
			}
			MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
		}
		bool HitFlag = false;
		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (kabe_.size() > 0) {
			HitFlag = false;
			for (auto& h_d : kabe_) {
				if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					if (MoveVector.magnitude() >= 0.001f) {	// x����z�������� 0.001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
						// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
						*NowPos = Vector3DX::Cross(h_d.Normal, Vector3DX::Cross(MoveVector, h_d.Normal)) + OldPos;
						//NowPos->y(OldPos.y);
						bool j = false;
						for (auto& h_d2 : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
								j = true;
								break;// �������Ă����烋�[�v���甲����
							}
						}
						if (!j) {
							HitFlag = false;
							break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
						}
					}
					else {
						break;
					}
				}
			}
			if (HitFlag) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
				for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
					bool HitF = false;
					for (auto& h_d : kabe_) {
						if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
							*NowPos += Vector3DX(h_d.Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
							//NowPos->y(OldPos.y);
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + Vector3DX::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + Vector3DX::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// �������Ă����烋�[�v�𔲂���
									j = true;
									break;
								}
							}
							if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
						break;
					}
				}
			}
			kabe_.clear();
		}
		return HitFlag;
	}

	//�_���[�W�C�x���g
	struct DamageEvent {
		PlayerID				ID{ 127 };
		Sceneclass::ObjType		CharaType{ Sceneclass::ObjType::Vehicle };
		HitPoint				Damage{ 0 };
		float					rad{ 0.f };
		void SetEvent(PlayerID pID, Sceneclass::ObjType pCharaType, HitPoint pDamage, float pRad) {
			this->ID = pID;
			this->CharaType = pCharaType;
			this->Damage = pDamage;
			this->rad = pRad;
		}
	};


	//�C���X�^�V���O
	class Model_Instance {
	private:
		int						m_Count{ 0 };			//��
		std::vector<VERTEX3D>	m_Vertex;				//
		std::vector<DWORD>		m_Index;				//
		int						m_VerBuf{ -1 };			//
		int						m_IndexBuf{ -1 };		//
		MV1						m_obj;					//���f��
		GraphHandle				m_pic;					//�摜�n���h��
		int						m_vnum{ -1 };			//
		int						m_pnum{ -1 };			//
		MV1_REF_POLYGONLIST		m_RefMesh{};			//
		int						m_Mesh{ 0 };			//
	private:
		void			Init_one(void) noexcept {
			MV1RefreshReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);				//�Q�Ɨp���b�V���̍X�V
			this->m_RefMesh = MV1GetReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);	//�Q�Ɨp���b�V���̎擾
		}
	public:
		//���Z�b�g
		void			Reset(void) noexcept {
			this->m_Count = 0;
			this->m_vnum = 0;
			this->m_pnum = 0;
			this->m_Vertex.clear();								//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			this->m_Vertex.reserve(2000);						//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			this->m_Index.clear();								//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			this->m_Index.reserve(2000);						//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
		}
		void			Set(const float& caliber, const Vector3DX& Position, const Vector3DX& Normal, const Vector3DX& Zvec) {
			this->m_Count++;
			Set_start(this->m_Count);
			float asize = 200.f * caliber;
			const auto& y_vec = Normal;
			auto z_vec = Vector3DX::Cross(y_vec, Zvec).normalized();
			auto scale = Vector3DX::vget(asize / std::abs(Vector3DX::Dot(y_vec, Zvec)), asize, asize);
			Matrix4x4DX mat = Matrix4x4DX::GetScale(scale) * Matrix4x4DX::Axis1(y_vec, z_vec) * Matrix4x4DX::Mtrans(Position + y_vec * 0.02f);
			Set_one(mat);
		}
		void			Set_start(int value) noexcept {
			this->m_Count = value;
			int Num = this->m_RefMesh.VertexNum * this->m_Count;
			this->m_Vertex.resize(Num);			//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			Num = this->m_RefMesh.PolygonNum * 3 * this->m_Count;
			this->m_Index.resize(Num);		//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
		}
		void			Set_one(const Matrix4x4DX& mat) noexcept {
			this->m_obj.SetMatrix(mat);
			Init_one();
			for (size_t j = 0; j < size_t(this->m_RefMesh.VertexNum); ++j) {
				auto& g = this->m_Vertex[j + this->m_vnum];
				const auto& r = this->m_RefMesh.Vertexs[j];
				g.pos = r.Position;
				g.norm = r.Normal;
				g.dif = r.DiffuseColor;
				g.spc = r.SpecularColor;
				g.u = r.TexCoord[0].u;
				g.v = r.TexCoord[0].v;
				g.su = r.TexCoord[1].u;
				g.sv = r.TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(this->m_RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(this->m_RefMesh.Polygons[j].VIndex); ++k) {
					this->m_Index[j * 3 + k + this->m_pnum] = WORD(this->m_RefMesh.Polygons[j].VIndex[k] + this->m_vnum);
				}
			}
			this->m_vnum += this->m_RefMesh.VertexNum;
			this->m_pnum += this->m_RefMesh.PolygonNum * 3;
		}
	public:
		void			Init(MV1& mv1path, int MeshNum) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->m_Mesh = MeshNum;
			auto path = MV1GetTextureGraphHandle(mv1path.get(), MV1GetMaterialDifMapTexture(mv1path.get(), MV1GetMeshMaterial(mv1path.get(), m_Mesh)));
			this->m_pic = path;								 //grass
			this->m_obj = mv1path.Duplicate();				//�e��
			Init_one();
		}
		void			Init(std::string pngpath, std::string mv1path, int MeshNum) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->m_Mesh = MeshNum;
			this->m_pic = GraphHandle::Load(pngpath);		 //grass
			MV1::Load(mv1path, &this->m_obj);				//�e��
			Init_one();
		}
		void			Execute(void) noexcept {
			this->m_VerBuf = CreateVertexBuffer((int)this->m_Vertex.size(), DX_VERTEX_TYPE_NORMAL_3D);
			this->m_IndexBuf = CreateIndexBuffer((int)this->m_Index.size(), DX_INDEX_TYPE_32BIT);
			SetVertexBufferData(0, this->m_Vertex.data(), (int)this->m_Vertex.size(), this->m_VerBuf);
			SetIndexBufferData(0, this->m_Index.data(), (int)this->m_Index.size(), this->m_IndexBuf);
		}
		void			Draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			DrawPolygonIndexed3D_UseVertexBuffer(this->m_VerBuf, this->m_IndexBuf, this->m_pic.get(), TRUE);
			//SetDrawAlphaTest(-1, 0);
		}
		void			Dispose(void) noexcept {
			this->m_Vertex.clear();
			this->m_Index.clear();
			this->m_obj.Dispose();
			this->m_pic.Dispose();
		}
	};
	//������
	class HIT_PASSIVE {
	private:
		Model_Instance	m_inst;
		bool			m_IsUpdate{ true };
	public:
		//������
		void			Init(void) noexcept {
			this->m_inst.Init("data/m_obj/hit/hit.png", "data/m_obj/hit/m_obj.mv1", -1);
		}
		//����̃��Z�b�g
		void			Clear(void) noexcept {
			this->m_inst.Reset();
		}

		void			Set(const float& caliber, const Vector3DX& Position, const Vector3DX& Normal, const Vector3DX& Zvec) {
			this->m_inst.Set(caliber, Position, Normal, Zvec);
			this->m_IsUpdate = true;
		}
		void			Execute(void) noexcept {
			if (this->m_IsUpdate) {
				this->m_IsUpdate = false;
				this->m_inst.Execute();
			}
		}
		void			Draw(void) noexcept {
			this->m_inst.Draw();
		}
	};
};
