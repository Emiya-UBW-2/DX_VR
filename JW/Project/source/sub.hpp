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
		void			SetInputStart(float pAddxRad, float pAddyRad, const VECTOR_ref& pRad) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;
			this->m_xRad = pRad.x();
			this->m_yRad = pRad.y();
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
#define PLAYER_HIT_WIDTH			(0.45f * Scale_Rate)		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			(16)					// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
	//�ǔ��胆�j�o�[�T��
	static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const std::vector<std::pair<MV1*, int>>& col_obj_t) noexcept {
		if (col_obj_t.size() == 0) { return false; }
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y(0);
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		for (const auto& objs : col_obj_t) {
			if ((&objs - &col_obj_t.front()) != 0) {
				if (GetMinLenSegmentToPoint(OldPos, OldPos + VECTOR_ref::up(), objs.first->GetMatrix().pos()) >=
					(20.f*Scale_Rate + PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size())) {
					continue;
				}
			}
			auto HitDim = objs.first->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), objs.second);
			// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{PLAYER_ENUM_MIN_SIZE��荂���|���S���̂ݓ����蔻����s��
				if (
					(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
					&& (h_d.Position[0].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE || h_d.Position[1].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE || h_d.Position[2].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE)
					&& (h_d.Position[0].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[1].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[2].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE)
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
				if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					if (MoveVector.size() >= 0.001f) {	// x����z�������� 0.001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
						// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
						*NowPos = VECTOR_ref(h_d.Normal).cross(MoveVector.cross(h_d.Normal)) + OldPos;
						//NowPos->y(OldPos.y());
						bool j = false;
						for (auto& h_d2 : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
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
						if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
							*NowPos += VECTOR_ref(h_d.Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
							//NowPos->y(OldPos.y());
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// �������Ă����烋�[�v�𔲂���
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


	//�C���X�^�V���O
	class Model_Instance {
	private:
		int						m_Count{0};			//��
		std::vector<VERTEX3D>	m_Vertex;				//
		std::vector<DWORD>		m_Index;				//
		int						m_VerBuf{-1};			//
		int						m_IndexBuf{-1};		//
		MV1						m_obj;					//���f��
		GraphHandle				m_pic;					//�摜�n���h��
		int						m_vnum{-1};			//
		int						m_pnum{-1};			//
		MV1_REF_POLYGONLIST		m_RefMesh{};			//
		int						m_Mesh{0};			//
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
		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->m_Count++;
			Set_start(this->m_Count);
			float asize = 200.f * caliber;
			const auto& y_vec = Normal;
			auto z_vec = y_vec.cross(Zvec).Norm();
			auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
			MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec) * MATRIX_ref::Mtrans(Position + y_vec * 0.02f);
			Set_one(mat);
		}
		void			Set_start(int value) noexcept {
			this->m_Count = value;
			int Num = this->m_RefMesh.VertexNum * this->m_Count;
			this->m_Vertex.resize(Num);			//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			Num = this->m_RefMesh.PolygonNum * 3 * this->m_Count;
			this->m_Index.resize(Num);		//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
		}
		void			Set_one(const MATRIX_ref& mat) noexcept {
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
			MV1::Load(mv1path, &this->m_obj, DX_LOADMODEL_PHYSICS_DISABLE);				//�e��
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
		bool			m_IsUpdate{true};
	public:
		//������
		void			Init(void) noexcept {
			this->m_inst.Init("data/m_obj/hit/hit.png", "data/m_obj/hit/m_obj.mv1", -1);
		}
		//����̃��Z�b�g
		void			Clear(void) noexcept {
			this->m_inst.Reset();
		}

		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
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



	template <typename... Args>
	static void DrawFetteString(int xp1, int yp1, float per, bool IsSelect, FontHandle::FontXCenter LR, const std::string& String, Args&&... args) noexcept {
		auto* Fonts = FontPool::Instance();
		auto Red = GetColor(255, 0, 0);
		auto Red75 = GetColor(192, 0, 0);
		auto White = GetColor(255, 255, 255);
		auto Gray75 = GetColor(128, 128, 128);
		auto Gray = GetColor(64, 64, 64);

		Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r((int)((float)(48 * 3 / 2 * 3 / 4) * per)), LR, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40.f*per), yp1 + y_r(20.f*per), IsSelect ? Red75 : Gray75, Gray, String, args...);
		Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r((int)((float)(48 * 2 * 3 / 4) * per)), LR, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? Red : White, Gray, String, args...);
	}
};
