#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
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
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage) {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
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
		};
	};
};
