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

	// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale_Rate)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale_Rate)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			(0.6f * Scale_Rate)		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			(16)					// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
	//�ǔ��胆�j�o�[�T��
	static bool col_wall(const Vector3DX& OldPos, Vector3DX* NowPos, const std::vector<std::pair<MV1*, int>>& col_obj_t) noexcept {
		if (col_obj_t.size() == 0) { return false; }
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y = (0);
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		for (const auto& objs : col_obj_t) {
			if ((&objs - &col_obj_t.front()) != 0) {
				if (GetMinLenSegmentToPoint(OldPos, OldPos + Vector3DX::up(), objs.first->GetMatrix().pos()) >=
					(20.f*Scale_Rate + PLAYER_ENUM_DEFAULT_SIZE + MoveVector.magnitude())) {
					continue;
				}
			}
			auto HitDim = objs.first->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.magnitude(), objs.second);
			// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{PLAYER_ENUM_MIN_SIZE��荂���|���S���̂ݓ����蔻����s��
				if (
					(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
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
		PlayerID				ShotID{127};
		PlayerID				DamageID{127};
		HitPoint				Damage{0};
		ArmerPoint				ArmerDamage{0};
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage) {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
			this->ArmerDamage = pArmerDamage;
		}
	};
	//
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		public:
			void			LoadChara(const std::string&FolderName, PlayerID ID) noexcept;
			void			LoadGun(const std::string&FolderName, PlayerID ID, int Sel) noexcept;
		};
	};
};
