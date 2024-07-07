#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:

			static const int tile_pic_size = 32;	//�摜�̃^�C���T�C�Y

		private:

			struct Blick {
				Vector3DX				m_Pos;
				Vector3DX				m_MinPos;
				Vector3DX				m_MaxPos;
				int						palletNum{0};
				bool					IsWall{false};
				float					ZRad{0};
				int						AddpalletNum[4]{};
				float					AddZRad[4]{0};
				int						LinkPolyIndex[4]{-1,-1,-1,-1};
				Vector3DX				BoxSide[4];
			public:
				Blick(int x, int y) noexcept {
					m_Pos.x = (float)x * ((64.f * 64.f) / (1080 / 2));
					m_Pos.y = (float)y * ((64.f * 64.f) / (1080 / 2));
					for (int i = 0; i < 4; i++) {
						LinkPolyIndex[i] = -1;
					}
					float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));
					m_MinPos.x = m_Pos.x - HalfLen;
					m_MinPos.y = m_Pos.y + HalfLen;

					m_MaxPos.x = m_Pos.x + HalfLen;
					m_MaxPos.y = m_Pos.y - HalfLen;

					BoxSide[0].Set(m_MinPos.x, m_MaxPos.y, 0.f);
					BoxSide[1].Set(m_MaxPos.x, m_MaxPos.y, 0.f);
					BoxSide[2].Set(m_MaxPos.x, m_MinPos.y, 0.f);
					BoxSide[3].Set(m_MinPos.x, m_MinPos.y, 0.f);
				}
			public:
				const int	GetLinkPolyIndex(int ID) const noexcept { return LinkPolyIndex[ID]; }
				void		SetLink(int tris, int index) { LinkPolyIndex[tris] = index; }
			};

			// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
			struct CheckLines {
				Vector3DX Position[3]{ };
				Vector3DX Normal{};
				bool canuse{true};
			public:
				void CalcNormal() {
					Vector3DX StartP = this->Position[0];
					Vector3DX EndP = this->Position[1];
					Vector3DX In = this->Position[2];
					Vector3DX VecP = EndP - StartP;
					this->Normal = Vector3DX::Cross(VecP, Vector3DX::Cross(VecP, In - StartP)).normalized();
				}
			};
			//
			// ���_����e�ɂȂ��Ă��镔���̎l�p�`
			struct ShadowBoxes {
				Vector3DX Position[4]{ };
			};
		private:
			std::vector<CheckLines> WallList;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
			std::vector<ShadowBoxes> m_ShadowBoxes;
			std::vector<std::vector<std::shared_ptr<Blick>>> m_Blick;
			std::vector<GraphHandle>	m_MapChip;
			std::vector<GraphHandle>	m_WallChip;

			float m_AmbientShadowLength{36.f};
			float m_AmbientLightRad = deg2rad(45);
			Vector3DX m_PointLightPos;

			float ShadowPer = 0.5f;
			GraphHandle m_ShadowHandle;
		public://
			const auto&		GetBuildDataNum(void) noexcept { return this->m_Blick.size() * this->m_Blick.back().size(); }
			const auto		GetXYToNum(int x, int y) const noexcept { return x * (int)this->m_Blick.size() + y; }
			const auto		GetNumToXY(int num) const noexcept { return std::make_pair<int,int>(num / (int)this->m_Blick.size(), num % (int)this->m_Blick.size()); }
			const auto&		GetBuildData(int num) const noexcept { auto XY = GetNumToXY(num); return this->m_Blick.at(XY.first).at(XY.second); }
			const auto		GetNearestBuilds(const Vector3DX& Pos) const noexcept {
				for (auto& b : m_Blick) {
					for (auto& y : b) {
						//�ǂł͂Ȃ�
						if (!y->IsWall) {
							if ((y->m_MaxPos.x > Pos.x && Pos.x > y->m_MinPos.x) && (y->m_MinPos.y > Pos.y && Pos.y > y->m_MaxPos.y)) {
								return GetXYToNum((int)(&b - &m_Blick.front()), (int)(&y - &b.front()));
							}
						}
					}
				}
				return -1;
			}
			const auto		GetNearBuildsList(const Vector3DX& Pos) const noexcept {
				std::vector<int> SelList;
				int count = 0;
				float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
				for (auto& b : m_Blick) {
					for (auto& y : b) {
						//�ǂł͂Ȃ�
						if (!y->IsWall) {
							auto Vec = y->m_Pos - Pos;
							if (Vec.magnitude() < ViewLimit) {
								SelList.emplace_back(count);
							}
						}
						count++;
					}
				}
				return SelList;
			}
			// �|���S�����m�̘A�������g�p���Ďw��̓�̍��W�Ԃ𒼐��I�Ɉړ��ł��邩�ǂ������`�F�b�N����( �߂�l true:�����I�Ɉړ��ł��� false:�����I�Ɉړ��ł��Ȃ� )
			bool CheckPolyMove(Vector3DX StartPos, Vector3DX TargetPos) const {
				int CheckPoly[4];
				int CheckPolyPrev[4];
				int NextCheckPoly[4];
				int NextCheckPolyPrev[4];

				// �J�n���W�ƖڕW���W�̒���A�Ⴕ���͒����ɑ��݂���|���S������������
				int StartPoly = this->GetNearestBuilds(StartPos);
				int TargetPoly = this->GetNearestBuilds(TargetPos);

				// �|���S�������݂��Ȃ�������ړ��ł��Ȃ��̂� false ��Ԃ�
				if (StartPoly == -1 || TargetPoly == -1) { return false; }

				// �w�������ɂ��邩�ǂ������`�F�b�N����|���S���Ƃ��ĊJ�n���W�̒���A�Ⴕ���͒����ɑ��݂���|���S����o�^
				int CheckPolyNum = 1;
				CheckPoly[0] = StartPoly;
				int CheckPolyPrevNum = 0;
				CheckPolyPrev[0] = -1;

				// ���ʂ��o��܂Ŗ������ŌJ��Ԃ�
				while (true) {
					int NextCheckPolyNum = 0;			// ���̃��[�v�Ń`�F�b�N�ΏۂɂȂ�|���S���̐������Z�b�g���Ă���
					int NextCheckPolyPrevNum = 0;			// ���̃��[�v�Ń`�F�b�N�Ώۂ���O���|���S���̐������Z�b�g���Ă���
					// �`�F�b�N�Ώۂ̃|���S���̐������J��Ԃ�
					for (int P = 0; P < CheckPolyNum; P++) {
						int Index = CheckPoly[P];
						auto& y = GetBuildData(Index);
						for (int i = 0; i < 4; i++) {
							Vector3DX StartP = y->BoxSide[i];
							Vector3DX EndP = y->BoxSide[(i + 1) % 4];
							int LinkIndex = y->GetLinkPolyIndex(i);
							// �|���S���̒��_�ԍ�0��1�̕ӂɗאڂ���|���S�������݂���ꍇ�ŁA
							// ���ӂ̐����ƈړ��J�n�_�A�I���_�Ō`������������ڂ��Ă����� if �����^�ɂȂ�
							if (LinkIndex != -1 && GetMinLenSegmentToSegment(StartPos, TargetPos, StartP, EndP) < 0.01f) {
								// �����ӂƐڂ��Ă���|���S�����ڕW���W��ɑ��݂���|���S���������� �J�n���W����ڕW���W��܂œr�؂�Ȃ��|���S�������݂���Ƃ������ƂȂ̂� true ��Ԃ�
								if (LinkIndex == TargetPoly) { return true; }

								// �ӂƐڂ��Ă���|���S�������̃`�F�b�N�Ώۂ̃|���S���ɉ�����

								// ���ɓo�^����Ă���|���S���̏ꍇ�͉����Ȃ�
								int IsHit = false;
								for (int j = 0; j < NextCheckPolyNum; j++) { if (NextCheckPoly[j] == LinkIndex) { IsHit = true; break; } }
								if (!IsHit) {
									// ���ɓo�^����Ă��鏜�O�|���S���̏ꍇ�͉����Ȃ�
									IsHit = false;
									for (int j = 0; j < NextCheckPolyPrevNum; j++) { if (NextCheckPolyPrev[j] == Index) { IsHit = true; break; } }
									if (!IsHit) {
										NextCheckPolyPrev[NextCheckPolyPrevNum] = Index;
										NextCheckPolyPrevNum++;
									}

									// ��O�̃��[�v�Ń`�F�b�N�ΏۂɂȂ����|���S���̏ꍇ�������Ȃ�
									IsHit = false;
									for (int j = 0; j < CheckPolyPrevNum; j++) { if (CheckPolyPrev[j] == LinkIndex) { IsHit = true; break; } }
									if (!IsHit) {
										// �����܂ŗ�����Q�����̃`�F�b�N�Ώۂ̃|���S���ɉ�����
										NextCheckPoly[NextCheckPolyNum] = LinkIndex;
										NextCheckPolyNum++;
									}
								}
							}
						}
					}

					// ���̃��[�v�Ń`�F�b�N�ΏۂɂȂ�|���S��������Ȃ������Ƃ������Ƃ�
					// �ړ��J�n�_�A�I���_�Ō`����������Ɛڂ���`�F�b�N�Ώۂ̃|���S���ɗאڂ���
					// �|���S��������Ȃ������Ƃ������ƂȂ̂ŁA�����I�Ȉړ��͂ł��Ȃ��Ƃ������Ƃ� false ��Ԃ�
					if (NextCheckPolyNum == 0) { return false; }
					// ���Ƀ`�F�b�N�ΏۂƂȂ�|���S���̏����R�s�[����
					for (int P = 0; P < NextCheckPolyNum; P++) { CheckPoly[P] = NextCheckPoly[P]; }
					CheckPolyNum = NextCheckPolyNum;
					// ���Ƀ`�F�b�N�ΏۊO�ƂȂ�|���S���̏����R�s�[����
					for (int P = 0; P < NextCheckPolyPrevNum; P++) { CheckPolyPrev[P] = NextCheckPolyPrev[P]; }
					CheckPolyPrevNum = NextCheckPolyPrevNum;
				}
			}
			bool CheckPolyMoveWidth(Vector3DX StartPos, int TargetIndex, float Width) const {
				// �|���S�����m�̘A�������g�p���Ďw��̓�̍��W�Ԃ𒼐��I�Ɉړ��ł��邩�ǂ������`�F�b�N����( �߂�l true:�����I�Ɉړ��ł��� false:�����I�Ɉړ��ł��Ȃ� )( ���w��� )
				Vector3DX TargetPos = GetBuildData(TargetIndex)->m_Pos;
				// �ŏ��ɊJ�n���W����ڕW���W�ɒ����I�Ɉړ��ł��邩�ǂ������`�F�b�N
				if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }
				// �J�n���W����ڕW���W�Ɍ������x�N�g���ɒ��p�ȃx�N�g�����Z�o
				Vector3DX TempVec = Vector3DX::Cross(TargetPos - StartPos, Vector3DX::forward()).normalized() * (Width / 2.0f);
				// �J�n���W�ƖڕW���W�� Width / 2.0f ���������������ɂ��炵�āA�ēx�����I�Ɉړ��ł��邩�ǂ������`�F�b�N
				if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) { return false; }
				// �J�n���W�ƖڕW���W�� Width / 2.0f ��������O�Ƃ͋t�����̐��������ɂ��炵�āA�ēx�����I�Ɉړ��ł��邩�ǂ������`�F�b�N
				if (CheckPolyMove(StartPos - TempVec, TargetPos - TempVec) == false) { return false; }
				return true;		// �����܂ł�����w��̕��������Ă������I�Ɉړ��ł���Ƃ������ƂȂ̂� true ��Ԃ�
			}





			//
			const float		CheckHideShadow(const Vector3DX & PosA, const Vector3DX & PosB, float Radius) noexcept;
			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) noexcept;
			const void		Get2DPos(int x, int y, Vector3DX* Min, Vector3DX* Center, Vector3DX* Max) const noexcept;
			void DrawAmbientShadow(int x, int y) const noexcept;
			void DrawPointShadow(int x, int y) noexcept;
		public://
			void		SetAmbientLight(float ShadowLen, float Rad) noexcept {
				m_AmbientShadowLength = ShadowLen;
				m_AmbientLightRad = Rad;
			}
			void		SetPointLight(const Vector3DX& Pos) noexcept {
				m_PointLightPos = Convert2DtoDisp(Pos);
			}
		public://
			//
			void			Init(const char* MapPath) noexcept;
			//
			void			Execute(void) noexcept;
			//
			void			SetupShadow(void) noexcept;
			//
			void			Draw(void) noexcept;
			void			DrawFront(void) noexcept;
			//
			void			Dispose(void) noexcept;
		};
	};
};
