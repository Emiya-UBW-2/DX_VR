#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:

			static const int tile_pic_size = 32;	// �摜�̃^�C���T�C�Y

		private:

			struct Blick {
				Vector3DX					m_Pos;
				int							palletNum{0};
				bool						IsWall{false};
				bool						IsCheckWall{ false };//�ǂŏ����`�F�b�N�Ώۂ��ǂ���
				float						ZRad{0};
				std::array<int, 4>			AddpalletNum{};
				std::array<float, 4>		AddZRad{0};
				std::array<int, 4>			LinkIndex{-1,-1,-1,-1};
				std::array<Vector3DX, 4>	BoxSide;
			public:
				Blick(int x, int y) noexcept {
					m_Pos.x = Get2DSize((float)x);
					m_Pos.y = Get2DSize((float)y);
					for (int i = 0; i < 4; i++) {
						LinkIndex.at(i) = -1;
					}
					float HalfLen = Get2DSize(0.5f);
					BoxSide.at(0).Set(m_Pos.x - HalfLen, m_Pos.y - HalfLen, 0.f);
					BoxSide.at(1).Set(m_Pos.x + HalfLen, m_Pos.y - HalfLen, 0.f);
					BoxSide.at(2).Set(m_Pos.x + HalfLen, m_Pos.y + HalfLen, 0.f);
					BoxSide.at(3).Set(m_Pos.x - HalfLen, m_Pos.y + HalfLen, 0.f);
				}
			public:
				bool CheckPointHit(const Vector3DX& Pos) {
					return HitPointToRectangle(
						(int)Pos.x, (int)Pos.y, 
						(int)(this->BoxSide.at(0).x), (int)(this->BoxSide.at(0).y),
						(int)(this->BoxSide.at(2).x), (int)(this->BoxSide.at(2).y));
				}
				bool CheckRectangleHit(const Vector3DX& Min, const Vector3DX& Max) {
					return 
						((int)Min.x < this->BoxSide.at(2).x && this->BoxSide.at(0).x < (int)Max.x) &&
						((int)Min.y < this->BoxSide.at(2).y && this->BoxSide.at(0).y < (int)Max.y);
				}

				bool CheckCapsuleHit(const Vector3DX& Pos1, const Vector3DX& Pos2, float Radius) {
					for (int i = 0; i < 4; i++) {
						if (GetHitCheckToCapsule(Pos1, Pos2, Radius, this->BoxSide.at(i), this->BoxSide.at((i + 1) % 4), 0.f)) {
							return true;
						}
					}
					return false;
				}
				bool CheckLineHit(const Vector3DX& Pos1, const Vector3DX& Pos2) {
					return CheckCapsuleHit(Pos1, Pos2, 0.001f);
				}
			};

			// �v���C���[�̎��͂ɂ���X�e�[�W�ǂ��擾����( ���o����͈͈͂ړ��������l������ )
			struct CheckLines {
				std::array<Vector3DX,3> Position{ };
				Vector3DX Normal{};
				bool canuse{true};
			public:
				void CalcNormal() {
					Vector3DX StartP = this->Position.at(0);
					Vector3DX EndP = this->Position.at(1);
					Vector3DX In = this->Position.at(2);
					Vector3DX VecP = EndP - StartP;
					this->Normal = Vector3DX::Cross(VecP, Vector3DX::Cross(VecP, In - StartP)).normalized();
				}
			};
			// 
			// ���_����e�ɂȂ��Ă��镔���̎l�p�`
			struct ShadowBoxes {
				std::array<Vector3DX,4> Position{ };
			};
			struct PlayerPatrol {
				int					m_index{};
				std::vector<int>	m_Patrol{};
			};
		private:
			std::vector<CheckLines> WallList;// �ǂƔ��f���ꂽ�\���̂̃A�h���X��ۑ����Ă���
			std::vector<ShadowBoxes> m_ShadowBoxes;
			std::vector<std::vector<std::shared_ptr<Blick>>> m_Blick;
			std::vector<PlayerPatrol>	m_PlayerSpawn;
			std::vector<GraphHandle>	m_MapChip;
			std::vector<GraphHandle>	m_WallChip;

			float m_AmbientShadowLength{36.f};
			float m_AmbientLightRad = deg2rad(45);
			Vector3DX m_AmbientLightVec;
			Vector3DX m_PointLightPos;

			GraphHandle m_PointShadowHandle;
			GraphHandle m_AmbientShadowHandle;
		public:// 
			const auto&		GetShadowGraph(void) const noexcept { return this->m_PointShadowHandle; }
			const auto&		GetAmbientLightVec(void) const noexcept { return this->m_AmbientLightVec; }
			const auto&		GetPlayerSpawn(void) const noexcept { return this->m_PlayerSpawn; }
			const auto		GetXSize(void) const noexcept { return this->m_Blick.size(); }
			const auto		GetYSize(void) const noexcept { return this->m_Blick.back().size(); }
			const auto		GetXYToNum(int x, int y) const noexcept { return x * (int)GetXSize() + y; }
			const auto		GetNumToXY(int num) const noexcept { return std::make_pair<int,int>(num / (int)GetXSize(), num % (int)GetXSize()); }

			const auto&		GetFloorData(int x, int y) const noexcept { return this->m_Blick.at(x).at(y); }
			const auto&		GetFloorData(int num) const noexcept { auto XY = GetNumToXY(num); return GetFloorData(XY.first, XY.second); }
			const auto		GetNearestFloors(const Vector3DX& Pos) const noexcept {
				for (auto& b : m_Blick) {
					for (auto& B : b) {
						if (B->IsWall) { continue; }
						if (!B->CheckPointHit(Pos)) { continue; }
						int x = (int)(&b - &m_Blick.front());
						int y = (int)(&B - &b.front());
						return GetXYToNum(x, y);
					}
				}
				return -1;
			}
			const auto		GetNearFloorsList(const Vector3DX& Pos) const noexcept {
				std::vector<int> SelList;
				float ViewLimit = Get2DSize(5.f);
				for (auto& b : m_Blick) {
					for (auto& B : b) {
						if (B->IsWall) { continue; }
						if ((B->m_Pos - Pos).sqrMagnitude() < ViewLimit* ViewLimit) {
							int x = (int)(&b - &m_Blick.front());
							int y = (int)(&B - &b.front());
							SelList.emplace_back(GetXYToNum(x, y));
						}
					}
				}
				return SelList;
			}
			bool CheckNextFloor(int Start, int Target) const {
				// �n�_�����݂��Ȃ�������ړ��ł��Ȃ��̂� false ��Ԃ�
				if (Start == -1 || Target == -1) {
					return false;
				}
				for (int i = 0; i < 4; i++) {
					// �����ӂƐڂ��Ă���n�_���ڕW���W��ɑ��݂���n�_�������� �J�n���W����ڕW���W��܂œr�؂�Ȃ��n�_�����݂���
					if (GetFloorData(Start)->LinkIndex.at(i) == Target) {
						return true;
					}
				}
				return false;
			}
			// 
			const float		CheckHideShadow(const Vector3DX& PosA, const Vector3DX& PosB, float Radius) noexcept;
			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius, bool IsPhysical) noexcept;
			void DrawAmbientShadow(int x, int y) const noexcept;
			void DrawPointShadow(int x, int y) noexcept;
		public:// 
			void		SetAmbientLight(float ShadowLen, float Rad) noexcept {
				m_AmbientShadowLength = ShadowLen;
				m_AmbientLightRad = Rad;
				float Radius = (float)y_r(m_AmbientShadowLength);
				m_AmbientLightVec.Set(std::sin(m_AmbientLightRad) * Radius, std::cos(m_AmbientLightRad) * Radius, 0.f);
			}
			void		SetPointLight(const Vector3DX& Pos) noexcept {
				m_PointLightPos = Convert2DtoDisp(Pos);
			}
		public:// 
			// 
			void			Init(const std::string& MapPath) noexcept;
			// 
			void			Execute(void) noexcept;
			// 
			void			SetupShadow(std::function<void()> AddAmbShadow) noexcept;
			// 
			void			Draw(void) noexcept;
			void			DrawFront(void) noexcept;
			// 
			void			Dispose(void) noexcept;
		};
	};
};
