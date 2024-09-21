#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		void BackGroundClass::AddSquare(const Matrix4x4DX& movement, COLOR_U8 color) noexcept
		{
			auto* DrawParts = DXDraw::Instance();
			Vector3DX CamPos = DrawParts->GetMainCamera().GetCamPos();
			Vector3DX CamVec = DrawParts->GetMainCamera().GetCamVec() - CamPos;
			Vector3DX Normal = Matrix4x4DX::Vtrans(Vector3DX::vget(0.0f, 1.0f, 0.0f), movement.rotation());
			if (Vector3DX::Dot(movement.pos() - CamPos, Normal) > 0.f) { return; }


			vert32.resize(vert32.size() + 1);
			vert32.back().pos = Matrix4x4DX::Vtrans(Vector3DX::vget(0.5f, 0.0f, 0.5f), movement).get();
			vert32.back().norm = Normal.get();
			vert32.back().dif = color;
			vert32.back().spc = GetColorU8(255, 255, 255, 255);
			vert32.back().u = 0.0f;
			vert32.back().v = 0.0f;
			vert32.back().su = 0.0f;
			vert32.back().sv = 0.0f;

			//vert32.back().spos;
			//vert32.back().tan;
			//vert32.back().binorm;


			vert32.resize(vert32.size() + 1);
			vert32.back().pos = Matrix4x4DX::Vtrans(Vector3DX::vget(-0.5f, 0.0f, 0.5f), movement).get();
			vert32.back().norm = Normal.get();
			vert32.back().dif = color;
			vert32.back().spc = GetColorU8(255, 255, 255, 255);
			vert32.back().u = 0.0f;
			vert32.back().v = 0.0f;
			vert32.back().su = 0.0f;
			vert32.back().sv = 0.0f;

			vert32.resize(vert32.size() + 1);
			vert32.back().pos = Matrix4x4DX::Vtrans(Vector3DX::vget(-0.5f, 0.0f, -0.5f), movement).get();
			vert32.back().norm = Normal.get();
			vert32.back().dif = color;
			vert32.back().spc = GetColorU8(255, 255, 255, 255);
			vert32.back().u = 0.0f;
			vert32.back().v = 0.0f;
			vert32.back().su = 0.0f;
			vert32.back().sv = 0.0f;

			vert32.resize(vert32.size() + 1);
			vert32.back().pos = Matrix4x4DX::Vtrans(Vector3DX::vget(0.5f, 0.0f, -0.5f), movement).get();
			vert32.back().norm = Normal.get();
			vert32.back().dif = color;
			vert32.back().spc = GetColorU8(255, 255, 255, 255);
			vert32.back().u = 0.0f;
			vert32.back().v = 0.0f;
			vert32.back().su = 0.0f;
			vert32.back().sv = 0.0f;

			int indexBase = (int)vert32.size() - 4;
			//���_�C���f�b�N�X�̐ݒ�
			index32.emplace_back(indexBase + 0);
			index32.emplace_back(indexBase + 1);
			index32.emplace_back(indexBase + 2);
			index32.emplace_back(indexBase + 0);
			index32.emplace_back(indexBase + 2);
			index32.emplace_back(indexBase + 3);
		}

		void BackGroundClass::AddCube(const Matrix4x4DX& scaleMat, const Matrix4x4DX& movement, COLOR_U8 color) noexcept
		{
			AddSquare(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.5f, 0.f)) * Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(0)) * scaleMat * movement, color);
			AddSquare(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.5f, 0.f)) * Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(90)) * scaleMat * movement, color);
			AddSquare(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.5f, 0.f)) * Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(-90)) * scaleMat * movement, color);
			AddSquare(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.5f, 0.f)) * Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(90)) * scaleMat * movement, color);
			AddSquare(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.5f, 0.f)) * Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(-90)) * scaleMat * movement, color);
			AddSquare(Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.5f, 0.f)) * Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(180)) * scaleMat * movement, color);
		}

		void BackGroundClass::AddCube(int x, int y, int z, float scale, COLOR_U8 color) noexcept
		{
			Vector3DX Pos = Vector3DX::vget((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f) * scale;
			AddCube(Matrix4x4DX::GetScale(Vector3DX::one() * scale), Matrix4x4DX::Mtrans(Pos), color);
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
