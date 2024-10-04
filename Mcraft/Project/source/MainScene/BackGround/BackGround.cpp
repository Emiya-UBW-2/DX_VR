#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		class PerlinNoise {
		private:
			//�����o�ϐ�
			std::array<uint_fast8_t, 512> p{ 0 };

			constexpr float getFade(const float t_) const noexcept {
				return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10);
			}
			constexpr float getLerp(const float t_, const float a_, const float b_) const noexcept {
				return a_ + t_ * (b_ - a_);
			}
			constexpr float makeGrad(const uint_fast8_t hash_, const float u_, const float v_) const noexcept {
				return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_);
			}
			constexpr float makeGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return this->makeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
			}
			constexpr float getGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return this->makeGrad(hash_ & 15, x_, y_, z_);
			}

			float setNoise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				const std::size_t x_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x_)) & 255);
				const std::size_t y_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y_)) & 255);
				const std::size_t z_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z_)) & 255);
				x_ -= std::floor(x_);
				y_ -= std::floor(y_);
				z_ -= std::floor(z_);
				const float u = this->getFade(x_);
				const float v = this->getFade(y_);
				const float w = this->getFade(z_);
				const std::size_t a0 = static_cast<std::size_t>(this->p[x_int] + y_int);
				const std::size_t a1 = static_cast<std::size_t>(this->p[a0] + z_int);
				const std::size_t a2 = static_cast<std::size_t>(this->p[a0 + 1] + z_int);
				const std::size_t b0 = static_cast<std::size_t>(this->p[x_int + 1] + y_int);
				const std::size_t b1 = static_cast<std::size_t>(this->p[b0] + z_int);
				const std::size_t b2 = static_cast<std::size_t>(this->p[b0 + 1] + z_int);

				return this->getLerp(w,
					this->getLerp(v,
						this->getLerp(u, this->getGrad(this->p[a1], x_, y_, z_), this->getGrad(this->p[b1], x_ - 1, y_, z_)),
						this->getLerp(u, this->getGrad(this->p[a2], x_, y_ - 1, z_), this->getGrad(this->p[b2], x_ - 1, y_ - 1, z_))),
					this->getLerp(v,
						this->getLerp(u, this->getGrad(this->p[a1 + 1], x_, y_, z_ - 1), this->getGrad(this->p[b1 + 1], x_ - 1, y_, z_ - 1)),
						this->getLerp(u, this->getGrad(this->p[a2 + 1], x_, y_ - 1, z_ - 1), this->getGrad(this->p[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
			}
			float setOctaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				float noise_value = 0.f;
				float amp = 1.0f;
				for (std::size_t i = 0; i < octaves_; ++i) {
					noise_value += this->setNoise(x_, y_, z_) * amp;
					x_ *= 2.0f;
					y_ *= 2.0f;
					z_ *= 2.0f;
					amp *= 0.5f;
				}
				return noise_value;
			}
		public:
			constexpr PerlinNoise() = default;
			explicit PerlinNoise(const std::uint_fast32_t seed_) noexcept {
				this->setSeed(seed_);
			}

			//SEED�l��ݒ肷��
			void setSeed(const std::uint_fast32_t seed_) noexcept {
				for (std::size_t i = 0; i < 256; ++i)
					this->p[i] = static_cast<uint_fast8_t>(i);
				std::shuffle(this->p.begin(), this->p.begin() + 256, std::default_random_engine(seed_));
				for (std::size_t i = 0; i < 256; ++i) {
					this->p[256 + i] = this->p[i];
				}
			}
			//�I�N�^�[�u�����m�C�Y���擾����
			float noise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return this->setNoise(x_, y_, z_) * 0.5f + 0.5f;
			}
			//�I�N�^�[�u�L��m�C�Y���擾����
			float octaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return this->setOctaveNoise(octaves_, x_, y_, z_) * 0.5f + 0.5f;
			}
		};
		//
		void		BackGroundClass::AddCube(const CellsData& cellx, int x, int y, int z, bool CheckFill, COLOR_U8 DifColor, COLOR_U8 SpcColor, int centerX, int centerY, int centerZ) noexcept {
			auto& fillinfo = cellx.GetOcclusionInfo(x, y, z);
			auto GetPlane = [&](int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) {
				if (m_32Num + 1 > m_32Size) {
					m_32Size = m_32Num + 1;
					m_vert32.resize(m_32Size * 4);
					m_index32.resize(m_32Size * 6);
				}

				float Xofs = 1.0f - static_cast<float>(yN);
				float uvAdd = 1.f / 8.f;
				float uMin = Xofs * uvAdd;
				float vMin = 0.f * uvAdd;

				auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
					Vector3DX Pos;
					if (xN != 0) {
						Pos = cellx.GetPos(x + (xN + 1) / 2, y + xt, z + yt);
					}
					else if (yN != 0) {
						Pos = cellx.GetPos(x + xt, y + (yN + 1) / 2, z + yt);
					}
					else if (zN != 0) {
						Pos = cellx.GetPos(x + xt, y + yt, z + (zN + 1) / 2);
					}

					auto& V = m_vert32.at(m_32Num * 4 + sel);
					V.pos = Pos.get();
					V.u = uMin + uvAdd * static_cast<float>(u);
					V.v = vMin + uvAdd * static_cast<float>(v);
					V.norm = VGet(static_cast<float>(xN), static_cast<float>(yN), static_cast<float>(zN));
					V.dif = DifColor;
					V.spc = SpcColor;
					};

				AddPointPos(x1, y1, 1, 0, 0);
				AddPointPos(x2, y2, 0, 0, 1);
				AddPointPos(x3, y3, 1, 1, 2);
				AddPointPos(x4, y4, 0, 1, 3);
				m_index32.at(m_32Num * 6) = (uint32_t)(m_32Num * 4);
				m_index32.at(m_32Num * 6 + 1) = (uint32_t)(m_32Num * 4 + 1);
				m_index32.at(m_32Num * 6 + 2) = (uint32_t)(m_32Num * 4 + 2);
				m_index32.at(m_32Num * 6 + 3) = (uint32_t)(m_32Num * 4 + 3);
				m_index32.at(m_32Num * 6 + 4) = (uint32_t)(m_32Num * 4 + 2);
				m_index32.at(m_32Num * 6 + 5) = (uint32_t)(m_32Num * 4 + 1);
				++m_32Num;
				};
			// ���_�f�[�^�̍쐬
			if (x + 1 <= centerX && !(CheckFill && (fillinfo & (1 << 0)))) {
				GetPlane(
					1, 0,
					1, 1,
					0, 0,
					0, 1,
					1, 0, 0);
			}
			if (x >= centerX && !(CheckFill && (fillinfo & (1 << 1)))) {
				GetPlane(
					1, 1,
					1, 0,
					0, 1,
					0, 0,
					-1, 0, 0);
			}
			if (y + 1 <= centerY && !(CheckFill && (fillinfo & (1 << 2)))) {
				GetPlane(
					0, 1,
					1, 1,
					0, 0,
					1, 0,
					0, 1, 0);
			}
			if (y >= centerY && !(CheckFill && (fillinfo & (1 << 3)))) {
				GetPlane(
					0, 0,
					1, 0,
					0, 1,
					1, 1,
					0, -1, 0);
			}
			if (z + 1 <= centerZ && !(CheckFill && (fillinfo & (1 << 4)))) {
				GetPlane(
					0, 0,
					1, 0,
					0, 1,
					1, 1,
					0, 0, 1);
			}
			if (z >= centerZ && !(CheckFill && (fillinfo & (1 << 5)))) {
				GetPlane(
					0, 1,
					1, 1,
					0, 0,
					1, 0,
					0, 0, -1);
			}
		}
		//
		void		BackGroundClass::AddShadowCube(const CellsData& cellx, int x, int y, int z, int lightX, int lightY, int lightZ) noexcept {
			auto& fillinfo = cellx.GetOcclusionInfo(x, y, z);
			auto GetPlane = [&](int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) {
				if (m_S32Num + 1 > m_S32Size) {
					m_S32Size = m_S32Num + 1;
					m_vert32S.resize(m_S32Size * 4);
					m_vert32SB.resize(m_S32Size * 4);
					m_index32S.resize(m_S32Size * 6);
				}

				auto AddPointPos = [&](int xt, int yt, int sel) {
					Vector3DX Pos;
					if (xN != 0) {
						Pos = cellx.GetPos(x + (xN + 1) / 2, y + xt, z + yt);
					}
					else if (yN != 0) {
						Pos = cellx.GetPos(x + xt, y + (yN + 1) / 2, z + yt);
					}
					else if (zN != 0) {
						Pos = cellx.GetPos(x + xt, y + yt, z + (zN + 1) / 2);
					}

					m_vert32S.at(m_S32Num * 4 + sel).pos = Pos.get();
					auto& V = m_vert32SB.at(m_S32Num * 4 + sel);
					V.pos = Pos.get();
					V.norm = VGet(static_cast<float>(xN), static_cast<float>(yN), static_cast<float>(zN));
					V.dif = GetColorU8(128, 128, 128, 255);
				};

				AddPointPos(x1, y1, 0);
				AddPointPos(x2, y2, 1);
				AddPointPos(x3, y3, 2);
				AddPointPos(x4, y4, 3);
				m_index32S.at(m_S32Num * 6) = (uint32_t)(m_S32Num * 4);
				m_index32S.at(m_S32Num * 6 + 1) = (uint32_t)(m_S32Num * 4 + 1);
				m_index32S.at(m_S32Num * 6 + 2) = (uint32_t)(m_S32Num * 4 + 2);
				m_index32S.at(m_S32Num * 6 + 3) = (uint32_t)(m_S32Num * 4 + 3);
				m_index32S.at(m_S32Num * 6 + 4) = (uint32_t)(m_S32Num * 4 + 2);
				m_index32S.at(m_S32Num * 6 + 5) = (uint32_t)(m_S32Num * 4 + 1);
				++m_S32Num;
				};
			// ���_�f�[�^�̍쐬
			if (0 <= lightX && (fillinfo & (1 << 0)) == 0) {
				GetPlane(
					1, 0,
					1, 1,
					0, 0,
					0, 1,
					1, 0, 0);
			}
			if (0 >= lightX && (fillinfo & (1 << 1)) == 0) {
				GetPlane(
					1, 1,
					1, 0,
					0, 1,
					0, 0,
					-1, 0, 0);
			}
			if (0 <= lightY && (fillinfo & (1 << 2)) == 0) {
				GetPlane(
					0, 1,
					1, 1,
					0, 0,
					1, 0,
					0, 1, 0);
			}
			if (0 >= lightY && (fillinfo & (1 << 3)) == 0) {
				GetPlane(
					0, 0,
					1, 0,
					0, 1,
					1, 1,
					0, -1, 0);
			}
			if (0 <= lightZ && (fillinfo & (1 << 4)) == 0) {
				GetPlane(
					0, 0,
					1, 0,
					0, 1,
					1, 1,
					0, 0, 1);
			}
			if (0 >= lightZ && (fillinfo & (1 << 5)) == 0) {
				GetPlane(
					0, 1,
					1, 1,
					0, 0,
					1, 0,
					0, 0, -1);
			}
		}
		//
		bool		BackGroundClass::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			auto& cell = m_CellxN.front();
			float scale = CellScale * cell.scaleRate;
			Vector3DX Start = StartPos / scale;
			Vector3DX End = StartPos / scale;

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							static_cast<int>(Start.x + xm + cell.Xall / 2), static_cast<int>(Start.y + ym + cell.Yall / 2), static_cast<int>(Start.z + zm + cell.Zall / 2),
							static_cast<int>(End.x + xm + cell.Xall / 2), static_cast<int>(End.y + ym + cell.Yall / 2), static_cast<int>(End.z + zm + cell.Zall / 2),
							[&](int x, int y, int z) {
							if (!cell.isInside(x, y, z)) { return false; }
							if (!cell.IsActiveCell(x, y, z)) { return false; }
							if (!(cell.GetOcclusionInfo(x, y, z) != 0b111111)) { return false; }
							Vector3DX MinPos = cell.GetPos(x, y, z);
							Vector3DX MaxPos = cell.GetPos(x + 1, y + 1, z + 1);
							if (ColRayBox(StartPos, EndPos, MinPos, MaxPos, Normal)) {
								isHit = true;
								return true;
							}
							return false;
							});
						if (isHit) { return true; }
					}
				}
			}
			return false;
		}
		//
		bool		BackGroundClass::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept {
			auto MoveVector = *EndPos - StartPos;
			// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
			auto& cell = m_CellxN.front();
			float scale = CellScale * cell.scaleRate;
			Vector3DX Start = StartPos / scale;
			Vector3DX End = StartPos / scale;

			for (int xm = -2; xm <= 2; ++xm) {
				for (int ym = 2; ym <= 2; ++ym) {
					for (int zm = -2; zm <= 2; ++zm) {
						Bresenham3D(
							static_cast<int>(Start.x + xm + cell.Xall / 2), static_cast<int>(Start.y + ym + cell.Yall / 2), static_cast<int>(Start.z + zm + cell.Zall / 2),
							static_cast<int>(End.x + xm + cell.Xall / 2), static_cast<int>(End.y + ym + cell.Yall / 2), static_cast<int>(End.z + zm + cell.Zall / 2),
							[&](int x, int y, int z) {
							if (!cell.isInside(x, y, z)) { return false; }
							if (!cell.IsActiveCell(x, y, z)) { return false; }
							if (!(cell.GetOcclusionInfo(x, y, z) != 0b111111)) { return false; }
							Vector3DX MinPos = cell.GetPos(x, y, z);
							Vector3DX MaxPos = cell.GetPos(x + 1, y + 1, z + 1);

							MV1_COLL_RESULT_POLY tmp{};
							// Left
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							// Right
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							// Back
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							// Forward
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							return true;
							});
					}
				}
			}
			bool HitFlag = false;
			// �ǃ|���S���Ƃ̓����蔻�菈��
			if (kabes.size() > 0) {
				HitFlag = false;
				for (auto& kabe : kabes) {
					// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
						HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
						if (MoveVector.magnitude() >= 0.001f) {	// x����z�������� 0.001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
							// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
							*EndPos = Vector3DX::Cross(kabe.Normal, Vector3DX::Cross(MoveVector, kabe.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
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
						for (auto& kabe : kabes) {
							// �v���C���[�Ɠ������Ă��邩�𔻒�
							if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale_Rate);					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
								bool j = false;
								for (auto& kabe2 : kabes) {
									// �������Ă����烋�[�v�𔲂���
									if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
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
				kabes.clear();
			}
			return HitFlag;
		}
		//
		void		BackGroundClass::LoadCellsFile() noexcept {
			std::ifstream fin{};
			fin.open("data/Map.txt", std::ios::in | std::ios::binary);
			auto& cell = m_CellxN.front();
			fin.read((char*)&cell.m_Cell[0], sizeof(cell.m_Cell[0])* (cell.Xall* cell.Yall* cell.Zall));
			fin.close();
		}
		void		BackGroundClass::SaveCellsFile() noexcept {
			std::ofstream fout{};
			fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			auto& cell = m_CellxN.front();
			fout.write((char*)&cell.m_Cell[0], sizeof(cell.m_Cell[0]) * (size_t)(cell.Xall * cell.Yall * cell.Zall));
			fout.close();  //�t�@�C�������
		}
		//
		void		BackGroundClass::SetBlick(int x, int y, int z, int8_t sel) noexcept {
			auto& cell = m_CellxN.front();
			if (!cell.isInside(x, y, z)) { return; }
			cell.SetCell(x, y, z) = sel;
			//�ȈՔł��X�V
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);
				auto& cell2 = m_CellxN.at(static_cast<size_t>(sel - 1));

				int xm = x / cell1.scaleRate;
				int ym = y / cell1.scaleRate;
				int zm = z / cell1.scaleRate;

				cell1.SetCell(xm, ym, zm) = cell2.isFill(xm, ym, zm) ? 1 : s_EmptyBlick;
			}
			//�Օ�����
			for (auto& cellx : m_CellxN) {
				int xm = x / cellx.scaleRate;
				int ym = y / cellx.scaleRate;
				int zm = z / cellx.scaleRate;
				auto CheckCell = [&](int xp, int yp, int zp) {
					if (cellx.isInside(xp, yp, zp)) {
						cellx.CalcOcclusion(xp, yp, zp);
					}
					};
				CheckCell(xm + 1, ym, zm);
				CheckCell(xm - 1, ym, zm);
				CheckCell(xm, ym + 1, zm);
				CheckCell(xm, ym - 1, zm);
				CheckCell(xm, ym, zm + 1);
				CheckCell(xm, ym, zm - 1);
			}
		}
		//
		void		BackGroundClass::Load(void) noexcept {
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			m_tex = GraphHandle::Load("data/tex.png");
			m_norm = GraphHandle::Load("data/nrm.png");
		}
		//
		void		BackGroundClass::Init(void) noexcept {
			//��
			this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
				//this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
				this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.7f, 0.7f, 0.7f, 1.f));
				this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
			}
			if (false) {
				PerlinNoise ns(GetRand(100));
				auto& cell = m_CellxN.front();
				for (int x = 0; x < cell.Xall; ++x) {
					for (int z = 0; z < cell.Zall; ++z) {
						auto Height = static_cast<int>(ns.octaveNoise(2,
							(static_cast<float>(x)) / cell.Xall,
							(static_cast<float>(z)) / cell.Zall
						) * static_cast<float>(cell.Yall * 4 / 5));
						for (int y = 0; y < cell.Yall; ++y) {
							if (y <= Height) {
								cell.SetCell(x, y, z) = 1;
								continue;
							}
							cell.SetCell(x, y, z) = s_EmptyBlick;
						}
					}
				}
				SaveCellsFile();
			}
			else {
				LoadCellsFile();
			}
			//�ȗ��ł𐧍�
			auto& cell = m_CellxN.front();
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);
				auto& cell2 = m_CellxN.at(static_cast<size_t>(sel - 1));

				cell1.SetScale(static_cast<int>(pow(MulPer, total - 1 - sel)), cell.Xall, cell.Yall, cell.Zall);

				for (int xm = 0; xm < cell1.Xall; ++xm) {
					for (int ym = 0; ym < cell1.Yall; ++ym) {
						for (int zm = 0; zm < cell1.Zall; ++zm) {
							cell1.SetCell(xm, ym, zm) = cell2.isFill(xm, ym, zm) ? 1 : s_EmptyBlick;
						}
					}
				}
			}
			//�Օ�����
			for (auto& cellx : m_CellxN) {
				for (int x = 0; x < cellx.Xall; ++x) {
					for (int y = 0; y < cellx.Yall; ++y) {
						for (int z = 0; z < cellx.Zall; ++z) {
							if (!cellx.IsActiveCell(x, y, z)) { continue; }
							cellx.CalcOcclusion(x, y, z);
						}
					}
				}
			}

			MATERIALPARAM Param{};
			Param.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 1.0f);						// �f�B�t���[�Y�J���[
			Param.Ambient = GetColorF(0.5f, 0.5f, 0.5f, 1.0f);						// �A���r�G���g�J���[
			Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// �X�y�L�����J���[
			Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// �G�~�b�V�u�J���[
			Param.Power = 5.f;							// �X�y�L�����n�C���C�g�̑N���x
			SetMaterialParam(Param);

			m_vert32.clear();
			m_index32.clear();
			m_32Num = 0;
			m_32Size = 0;
			m_vert32SB.clear();
			m_vert32S.clear();
			m_index32S.clear();
			m_S32Num = 0;
			m_S32Size = 0;
		}
		//
		void		BackGroundClass::Execute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			const Vector3DX CamPos = DrawParts->GetMainCamera().GetCamPos();
			const Vector3DX CamVec = (DrawParts->GetMainCamera().GetCamVec() - CamPos).normalized();

			int BaseRate = 100;
			int ShadowRate = 100;
			int ShadowMax = DrawMax;
			switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)) {
			case 0:
			case 1:
				BaseRate = MulPer * MulPer;
				ShadowRate = 1;
				ShadowMax = DrawMax / 2;
				break;
			case 2:
				BaseRate = MulPer * MulPer * MulPer;
				ShadowRate = MulPer;
				ShadowMax = DrawMax / 2;
				break;
			case 3:
				BaseRate = MulPer * MulPer * MulPer * MulPer;
				ShadowRate = MulPer;
				ShadowMax = DrawMax / 2;
				break;
			default:
				break;
			};
			if (!OptionParts->GetParamBoolean(EnumSaveParam::shadow)) {
				ShadowRate = 0;
			}

			Vector3DX light = DrawParts->GetLightVec();
			int lightX = static_cast<int>(light.x);
			int lightY = static_cast<int>(light.y);
			int lightZ = static_cast<int>(light.z);

#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//�f�o�b�O
#endif // DEBUG
			m_32Num = 0;
			m_S32Num = 0;
			for (auto& cellx : m_CellxN) {
#ifdef DEBUG
				DebugParts->SetPoint(("---" + std::to_string(cellx.scaleRate)).c_str());
#endif // DEBUG
				float scale = CellScale * cellx.scaleRate;
				Vector3DX center = CamPos / scale;
				int centerX = static_cast<int>(center.x) + cellx.Xall / 2;
				int centerY = static_cast<int>(center.y) + cellx.Yall / 2;
				int centerZ = static_cast<int>(center.z) + cellx.Zall / 2;
				//
				if (BaseRate >= cellx.scaleRate) {
					Vector3DX mid = Vector3DX::one() * (0.5f * scale);
					int xMaxmin = std::max(centerX - DrawMax, 0);
					int xMaxmax = std::min(centerX + DrawMax + 1, cellx.Xall);
					int yMaxmin = std::max(centerY - DrawMax, 0);
					int yMaxmax = std::min(centerY + DrawMax + 1, cellx.Yall);
					int zMaxmin = std::max(centerZ - DrawMax, 0);
					int zMaxmax = std::min(centerZ + DrawMax + 1, cellx.Zall);

					int xMinmin = centerX - DrawMax / MulPer;
					int xMinmax = centerX + DrawMax / MulPer;
					int yMinmin = centerY - DrawMax / MulPer;
					int yMinmax = centerY + DrawMax / MulPer;
					int zMinmin = centerZ - DrawMax / MulPer;
					int zMinmax = centerZ + DrawMax / MulPer;
					for (int x = xMaxmin; x < xMaxmax; ++x) {
						//*
						//��`���J�����̕��ʊ�藠�ɂ���ꍇ(4�_�����ׂė��ɂ���ꍇ)�̓X�L�b�v
						Vector3DX YZPos0 = cellx.GetPos(x, yMaxmin, zMaxmin) + mid;
						Vector3DX YZPos1 = cellx.GetPos(x, yMaxmax, zMaxmax) + mid;
						Vector3DX YZPos2 = cellx.GetPos(x, yMaxmin, zMaxmax) + mid;
						Vector3DX YZPos3 = cellx.GetPos(x, yMaxmax, zMaxmin) + mid;
						bool IsHit = false;
						while (true) {
							if (Vector3DX::Dot(CamVec, (YZPos0 - CamPos)) > 0.f) { IsHit = true; break; }
							if (Vector3DX::Dot(CamVec, (YZPos1 - CamPos)) > 0.f) { IsHit = true; break; }
							if (Vector3DX::Dot(CamVec, (YZPos2 - CamPos)) > 0.f) { IsHit = true; break; }
							if (Vector3DX::Dot(CamVec, (YZPos3 - CamPos)) > 0.f) { IsHit = true; break; }
							break;
						}
						if (!IsHit) {
							continue;
						}
						//*/
						for (int y = yMaxmin; y < yMaxmax; ++y) {
							int zMaxminT = zMaxmin;
							int zMaxmaxT = zMaxmax;
							//*
							Vector3DX ZMinPos = cellx.GetPos(x, y, zMaxmin) + mid;
							Vector3DX ZMaxPos = cellx.GetPos(x, y, zMaxmax) + mid;
							Vector3DX HitPos;
							bool pSameVecNormalToA{};
							bool OnFront{};
							if (CalcIntersectionPoint(ZMinPos, ZMaxPos, CamPos, CamVec, &HitPos, &pSameVecNormalToA, &OnFront)) {
								if (!pSameVecNormalToA) {
									zMaxminT = std::max(static_cast<int>((HitPos.z - 0.5) / scale) + cellx.Zall / 2, zMaxmin);
								}
								else {
									zMaxmaxT = std::min(static_cast<int>((HitPos.z - 0.5) / scale) + cellx.Zall / 2 + 1, zMaxmax);
								}
							}
							else {
								if (!OnFront) { continue; }
							}
							//*/

							bool OnNearestPoint = ((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax));
							for (int z = zMaxminT; z < zMaxmaxT; ++z) {
								if (cellx.scaleRate != 1) {
									if (OnNearestPoint) {
										if ((zMinmin < z) && (z < zMinmax)) {
											z = zMinmax - 1;
											continue;
										}
									}
								}
								if (!cellx.IsActiveCell(x, y, z)) { continue; }
								if (!(cellx.GetOcclusionInfo(x, y, z) != 0b111111)) { continue; }

								bool checkFill = true;
								if (cellx.scaleRate != 1) {
									checkFill = !(((xMinmin <= x) && (x <= xMinmax)) && ((yMinmin <= y) && (y <= yMinmax)) && ((zMinmin <= z) && (z <= zMinmax)));
								}

								AddCube(cellx, x, y, z, checkFill, GetColorU8(128, 128, 128, 255), GetColorU8(64, 64, 64, 255), centerX, centerY, centerZ);
							}
						}
					}
				}
				//
				if (ShadowRate >= cellx.scaleRate) {
					int xMaxmin = std::max(centerX - ShadowMax, 0);
					int xMaxmax = std::min(centerX + ShadowMax + 1, cellx.Xall);
					int yMaxmin = std::max(centerY - ShadowMax, 0);
					int yMaxmax = std::min(centerY + ShadowMax + 1, cellx.Yall);
					int zMaxmin = std::max(centerZ - ShadowMax, 0);
					int zMaxmax = std::min(centerZ + ShadowMax + 1, cellx.Zall);

					int xMinmin = centerX - ShadowMax / MulPer;
					int xMinmax = centerX + ShadowMax / MulPer;
					int yMinmin = centerY - ShadowMax / MulPer;
					int yMinmax = centerY + ShadowMax / MulPer;
					int zMinmin = centerZ - ShadowMax / MulPer;
					int zMinmax = centerZ + ShadowMax / MulPer;
					for (int x = xMaxmin; x < xMaxmax; ++x) {
						for (int y = yMaxmin; y < yMaxmax; ++y) {
							bool OnNearestPoint = ((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax));
							for (int z = zMaxmin; z < zMaxmax; ++z) {
								if (cellx.scaleRate != 1) {
									if (OnNearestPoint) {
										if ((zMinmin < z) && (z < zMinmax)) {
											z = zMinmax - 1;
											continue;
										}
									}
								}
								if (!cellx.IsActiveCell(x, y, z)) { continue; }
								if (!(cellx.GetOcclusionInfo(x, y, z) != 0b111111)) { continue; }
								AddShadowCube(cellx, x, y, z, lightX, lightY, lightZ);
							}
						}
					}
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("END");
#endif // DEBUG
		}
		//
		void		BackGroundClass::BG_Draw(void) const noexcept {
			SetUseLighting(FALSE);
			this->m_ObjSky.DrawModel();
			SetUseLighting(TRUE);
		}
		void		BackGroundClass::Shadow_Draw(void) const noexcept {
			if (m_S32Num > 0) {
				DrawPolygon32bitIndexed3D(m_vert32SB.data(), static_cast<int>(m_S32Num * 4), m_index32S.data(), static_cast<int>(m_S32Num * 6 / 3), m_tex.get(), TRUE);
			}
		}
		void		BackGroundClass::SetShadow_Draw_Rigid(void) const noexcept {
			if (m_S32Num > 0) {
				SetUseTextureToShader(0, m_tex.get());
				DrawPolygon32bitIndexed3DToShader(m_vert32S.data(), static_cast<int>(m_S32Num * 4), m_index32S.data(), static_cast<int>(m_S32Num * 6 / 3));
				SetUseTextureToShader(0, INVALID_ID);
			}
		}
		void		BackGroundClass::Draw(void) const noexcept {
			if (m_32Num > 0) {
				DrawPolygon32bitIndexed3D(m_vert32.data(), static_cast<int>(m_32Num * 4), m_index32.data(), static_cast<int>(m_32Num * 6 / 3), m_tex.get(), TRUE);
			}
		}
		//
		void		BackGroundClass::Dispose(void) noexcept {
			this->m_ObjSky.Dispose();
			SaveCellsFile();
		}
	}
}
