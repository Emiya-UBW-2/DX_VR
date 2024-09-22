#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		void BackGroundClass::AddCube(const Vector3DX& Pos, float scale, COLOR_U8 color) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Vector3DX PosCamVec = Pos * scale - DrawParts->GetMainCamera().GetCamPos();

			VECTOR Pos1F = ((Pos + Vector3DX::vget(-0.5f, -0.5f, -0.5f)) * scale).get();
			VECTOR Pos2F = ((Pos + Vector3DX::vget(0.5f, 0.5f, 0.5f)) * scale).get();
			//
			// ���_�f�[�^�̍쐬
			Vector3DX Normal = Vector3DX::back();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize(Now + 4);
				index32.resize(Nowi + 6);

				index32.at(Nowi) = Now;
				index32.at(Nowi + 1) = Now + 1;
				index32.at(Nowi + 2) = Now + 2;
				index32.at(Nowi + 3) = Now + 3;
				index32.at(Nowi + 4) = index32.at(Nowi + 2);
				index32.at(Nowi + 5) = index32.at(Nowi + 1);

				vert32.at(index32.at(Nowi)).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 1)).pos = Vector3DX::vget(Pos2F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 2)).pos = Pos1F;
				vert32.at(index32.at(Nowi + 3)).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos1F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at(Now + i).norm = Normal.get();
					vert32.at(Now + i).dif = color;
					vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
					vert32.at(Now + i).u = 0.0f;
					vert32.at(Now + i).v = 0.0f;
					vert32.at(Now + i).su = 0.0f;
					vert32.at(Now + i).sv = 0.0f;
				}
			}

			Normal = Vector3DX::forward();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize(Now + 4);
				index32.resize(Nowi + 6);

				index32.at(Nowi) = Now;
				index32.at(Nowi + 1) = Now + 1;
				index32.at(Nowi + 2) = Now + 2;
				index32.at(Nowi + 3) = Now + 3;
				index32.at(Nowi + 4) = index32.at(Nowi + 2);
				index32.at(Nowi + 5) = index32.at(Nowi + 1);

				vert32.at(index32.at(Nowi)).pos = Pos2F;
				vert32.at(index32.at(Nowi + 1)).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos2F.z).get();
				vert32.at(index32.at(Nowi + 2)).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos2F.z).get();
				vert32.at(index32.at(Nowi + 3)).pos = Vector3DX::vget(Pos1F.x, Pos1F.y, Pos2F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at(Now + i).norm = Normal.get();
					vert32.at(Now + i).dif = color;
					vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
					vert32.at(Now + i).u = 0.0f;
					vert32.at(Now + i).v = 0.0f;
					vert32.at(Now + i).su = 0.0f;
					vert32.at(Now + i).sv = 0.0f;
				}
			}

			Normal = Vector3DX::left();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize(Now + 4);
				index32.resize(Nowi + 6);

				index32.at(Nowi) = Now;
				index32.at(Nowi + 1) = Now + 1;
				index32.at(Nowi + 2) = Now + 2;
				index32.at(Nowi + 3) = Now + 3;
				index32.at(Nowi + 4) = index32.at(Nowi + 2);
				index32.at(Nowi + 5) = index32.at(Nowi + 1);

				vert32.at(index32.at(Nowi)).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos2F.z).get();
				vert32.at(index32.at(Nowi + 1)).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 2)).pos = Vector3DX::vget(Pos1F.x, Pos1F.y, Pos2F.z).get();
				vert32.at(index32.at(Nowi + 3)).pos = Pos1F;
				for (int i = 0; i < 4; i++) {
					vert32.at(Now + i).norm = Normal.get();
					vert32.at(Now + i).dif = color;
					vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
					vert32.at(Now + i).u = 0.0f;
					vert32.at(Now + i).v = 0.0f;
					vert32.at(Now + i).su = 0.0f;
					vert32.at(Now + i).sv = 0.0f;
				}
			}

			Normal = Vector3DX::right();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize(Now + 4);
				index32.resize(Nowi + 6);

				index32.at(Nowi) = Now;
				index32.at(Nowi + 1) = Now + 1;
				index32.at(Nowi + 2) = Now + 2;
				index32.at(Nowi + 3) = Now + 3;
				index32.at(Nowi + 4) = index32.at(Nowi + 2);
				index32.at(Nowi + 5) = index32.at(Nowi + 1);

				vert32.at(index32.at(Nowi)).pos = Vector3DX::vget(Pos2F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 1)).pos = Pos2F;
				vert32.at(index32.at(Nowi + 2)).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 3)).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos2F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at(Now + i).norm = Normal.get();
					vert32.at(Now + i).dif = color;
					vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
					vert32.at(Now + i).u = 0.0f;
					vert32.at(Now + i).v = 0.0f;
					vert32.at(Now + i).su = 0.0f;
					vert32.at(Now + i).sv = 0.0f;
				}
			}

			Normal = Vector3DX::up();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize(Now + 4);
				index32.resize(Nowi + 6);

				index32.at(Nowi) = Now;
				index32.at(Nowi + 1) = Now + 1;
				index32.at(Nowi + 2) = Now + 2;
				index32.at(Nowi + 3) = Now + 3;
				index32.at(Nowi + 4) = index32.at(Nowi + 2);
				index32.at(Nowi + 5) = index32.at(Nowi + 1);

				vert32.at(index32.at(Nowi)).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos2F.z).get();
				vert32.at(index32.at(Nowi + 1)).pos = Pos2F;
				vert32.at(index32.at(Nowi + 2)).pos = Vector3DX::vget(Pos1F.x, Pos2F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 3)).pos = Vector3DX::vget(Pos2F.x, Pos2F.y, Pos1F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at(Now + i).norm = Normal.get();
					vert32.at(Now + i).dif = color;
					vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
					vert32.at(Now + i).u = 0.0f;
					vert32.at(Now + i).v = 0.0f;
					vert32.at(Now + i).su = 0.0f;
					vert32.at(Now + i).sv = 0.0f;
				}
			}

			Normal = Vector3DX::down();
			if (Vector3DX::Dot(PosCamVec, Normal) < 0.f) {
				int Now = (int)vert32.size();
				int Nowi = (int)index32.size();
				vert32.resize(Now + 4);
				index32.resize(Nowi + 6);

				index32.at(Nowi) = Now;
				index32.at(Nowi + 1) = Now + 1;
				index32.at(Nowi + 2) = Now + 2;
				index32.at(Nowi + 3) = Now + 3;
				index32.at(Nowi + 4) = index32.at(Nowi + 2);
				index32.at(Nowi + 5) = index32.at(Nowi + 1);

				vert32.at(index32.at(Nowi)).pos = Pos1F;
				vert32.at(index32.at(Nowi + 1)).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos1F.z).get();
				vert32.at(index32.at(Nowi + 2)).pos = Vector3DX::vget(Pos1F.x, Pos1F.y, Pos2F.z).get();
				vert32.at(index32.at(Nowi + 3)).pos = Vector3DX::vget(Pos2F.x, Pos1F.y, Pos2F.z).get();
				for (int i = 0; i < 4; i++) {
					vert32.at(Now + i).norm = Normal.get();
					vert32.at(Now + i).dif = color;
					vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
					vert32.at(Now + i).u = 0.0f;
					vert32.at(Now + i).v = 0.0f;
					vert32.at(Now + i).su = 0.0f;
					vert32.at(Now + i).sv = 0.0f;
				}
			}
		}
		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		
		bool BackGroundClass::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) const noexcept
		{
			std::vector<std::pair<const MV1*, int>> cols;
			//cols.emplace_back(std::make_pair<const MV1*, int>(&this->m_ObjGroundCol, -1));
			if (cols.size() == 0) { return false; }
			auto MoveVector = *EndPos - StartPos;
			//MoveVector.y = (0);
			// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
			std::vector<MV1_COLL_RESULT_POLY> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
			for (const auto& objs : cols) {
				if ((&objs - &cols.front()) != 0) {
					if (GetMinLenSegmentToPoint(StartPos, StartPos + Vector3DX::up(), objs.first->GetMatrix().pos()) >=
						(20.f * Scale_Rate + (Radius * 3.f) + MoveVector.magnitude())) {
						continue;
					}
				}
				auto HitDim = objs.first->CollCheck_Sphere(StartPos, (Radius * 3.f) + MoveVector.magnitude(), objs.second);
				// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
				for (int i = 0; i < HitDim.HitNum; ++i) {
					auto& h_d = HitDim.Dim[i];
					//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{(Radius/3.f)��荂���|���S���̂ݓ����蔻����s��
					if (
						(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
						&& (h_d.Position[0].y > StartPos.y + (Radius / 3.f) || h_d.Position[1].y > StartPos.y + (Radius / 3.f) || h_d.Position[2].y > StartPos.y + (Radius / 3.f))
						&& (h_d.Position[0].y < StartPos.y + (Radius * 3.f) || h_d.Position[1].y < StartPos.y + (Radius * 3.f) || h_d.Position[2].y < StartPos.y + (Radius * 3.f))
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
					if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
						HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
						if (MoveVector.magnitude() >= 0.001f) {	// x����z�������� 0.001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
							// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
							*EndPos = Vector3DX::Cross(h_d.Normal, Vector3DX::Cross(MoveVector, h_d.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
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
					for (int k = 0; k < 16; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
						bool HitF = false;
						for (auto& h_d : kabe_) {
							if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
								*EndPos += Vector3DX(h_d.Normal) * (0.015f * Scale_Rate);					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
								//EndPos->y(StartPos.y);
								bool j = false;
								for (auto& h_d2 : kabe_) {
									if (GetHitCapsuleToTriangle(*EndPos + Vector3DX::up() * (Radius + 0.1f), *EndPos + Vector3DX::up() * (PLAYER_HIT_HEIGHT), Radius, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// �������Ă����烋�[�v�𔲂���
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
	}
}
