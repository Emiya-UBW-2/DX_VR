#include	"CPU.hpp"
#include "../../CommonScene/NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PathChecker {
			std::shared_ptr<BackGroundClassMain>	m_BackGround{nullptr};				//BG
		public:
			// �o�H�T�������p�̂P�|���S���̏��
			class PATHPLANNING_UNIT {
				int PolyIndex;						// �|���S���ԍ�
				float TotalDistance;				// �o�H�T���ł��̃|���S���ɓ��B����܂łɒʉ߂����|���S���Ԃ̋����̍��v
				PATHPLANNING_UNIT *PrevPolyUnit;		// �o�H�T���Ŋm�肵���o�H��̈�O�̃|���S��( ���|���S�����o�H��ɖ����ꍇ�� nullptr )
				PATHPLANNING_UNIT *NextPolyUnit;		// �o�H�T���Ŋm�肵���o�H��̈��̃|���S��( ���|���S�����o�H��ɖ����ꍇ�� nullptr )
			public:
				PATHPLANNING_UNIT *ActiveNextUnit;		// �o�H�T�������ΏۂɂȂ��Ă��鎟�̃|���S���̃������A�h���X���i�[����ϐ�
			public:
				const auto& GetPolyIndex() const noexcept { return this->PolyIndex; }
				const auto& GetNextPolyUnit() const noexcept { return this->NextPolyUnit; }
			public:
				bool SetPrevPolyUnit(PATHPLANNING_UNIT *PUnit, int tris, const std::shared_ptr<BackGroundClassMain>& m_BackGround) {
					// �אڂ���|���S�������Ɍo�H�T���������s���Ă��āA����苗���̒����o�H�ƂȂ��Ă���ꍇ�͉������Ȃ�
					auto& Unit = m_BackGround->GetBuildData().at(PUnit->PolyIndex);

					auto trisdistance = PUnit->TotalDistance +
						(m_BackGround->GetBuildData().at(Unit.GetLinkPolyIndex(tris)).GetMatrix().pos() + Unit.GetMatrix().pos()).magnitude();

					if (this->TotalDistance > trisdistance) {
						this->TotalDistance = trisdistance;		// �אڂ���|���S���ɂ����ɓ��B����܂ł̋�����������
					}
					else {
						if (this->PrevPolyUnit) { return false; }
					}
					this->PrevPolyUnit = PUnit;			// �אڂ���|���S���Ɍo�H���ƂȂ鎩���̃|���S���̔ԍ���������
					return true;
				}
				bool SearchThisUnit(PATHPLANNING_UNIT *SearchUnit) {
					// ���̃��[�v�ōs���o�H�T�������Ώۂɒǉ�����A���ɒǉ�����Ă�����ǉ����Ȃ�
					PATHPLANNING_UNIT *PUnitSub2 = SearchUnit;
					while (true) {
						if (PUnitSub2 == nullptr) { break; }
						if (PUnitSub2 == this) { return false; }//���ɒǉ�����Ƃ�
						PUnitSub2 = PUnitSub2->ActiveNextUnit;
					}
					return PUnitSub2;
				}
			public:
				// �S�[���n�_�̃|���S������X�^�[�g�n�_�̃|���S���ɒH���Čo�H��̃|���S���Ɏ��Ɉړ����ׂ��|���S���̔ԍ���������
				static void SetNextIndex(PATHPLANNING_UNIT *pGoal, PATHPLANNING_UNIT *pStart) {
					PATHPLANNING_UNIT *PUnit = pGoal;
					while (true) {
						auto* PrevPUnitIndex = PUnit;
						PUnit = PUnit->PrevPolyUnit;
						PUnit->NextPolyUnit = PrevPUnitIndex;
						if (PUnit == pStart) { break; }
					}
				}
			public:
				void Init(int index) {
					this->PolyIndex = index;
					this->TotalDistance = 0.0f;
					this->PrevPolyUnit = nullptr;
					this->NextPolyUnit = nullptr;
					this->ActiveNextUnit = nullptr;
				}
			};
		private:
			Vector3DX GoalPosition;					// �ڕW�ʒu
			std::vector<PATHPLANNING_UNIT>UnitArray;	// �o�H�T�������Ŏg�p����S�|���S���̏��z�񂪊i�[���ꂽ�������̈�̐擪�������A�h���X���i�[����ϐ�
			PATHPLANNING_UNIT *StartUnit{nullptr};			// �o�H�̃X�^�[�g�n�_�ɂ���|���S�����ւ̃������A�h���X���i�[����ϐ�
			PATHPLANNING_UNIT *GoalUnit{nullptr};				// �o�H�̃S�[���n�_�ɂ���|���S�����ւ̃������A�h���X���i�[����ϐ�
		public:
			void		SetBackGround(const std::shared_ptr<BackGroundClassMain>& BackBround_t) noexcept { m_BackGround = BackBround_t; }
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			Vector3DX GetNextPoint(const Vector3DX& NowPosition, int *TargetPathPlanningIndex) const {
				int NowIndex = m_BackGround->GetNearestBuilds(NowPosition);
				if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// ���ݏ���Ă���|���S�����S�[���n�_�ɂ���|���S���̏ꍇ�͏����𕪊�
					if (NowIndex == *TargetPathPlanningIndex) {													// ���ݏ���Ă���|���S�����ړ����Ԓn�_�̃|���S���̏ꍇ�͎��̒��Ԓn�_�����肷�鏈�����s��
						const float COLLWIDTH = 0.1f*Scale_Rate;				// �����蔻��̃T�C�Y
						while (true) {																				// ���̒��Ԓn�_�����肷��܂Ń��[�v��������
							if (!this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()) { break; }
							auto NextIndex = this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()->GetPolyIndex();
							if (!m_BackGround->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// �o�H��̎��̃|���S���̒��S���W�ɒ����I�Ɉړ��ł��Ȃ��ꍇ�̓��[�v���甲����
							(*TargetPathPlanningIndex) = NextIndex;													// �`�F�b�N�Ώۂ��o�H��̍X�Ɉ��̃|���S���ɕύX����
							if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// �����S�[���n�_�̃|���S���������烋�[�v�𔲂���
						}
					}
					// �ړ����������肷��A�ړ������͌��݂̍��W���璆�Ԓn�_�̃|���S���̒��S���W�Ɍ���������
					return m_BackGround->GetBuildData().at(*TargetPathPlanningIndex).GetMatrix().pos();
				}
				else {
					// �����͖ڕW���W
					return this->GoalPosition;
				}
			}
		public:
			bool Init(Vector3DX StartPos, Vector3DX GoalPos) {
				// �w��̂Q�_�̌o�H��T������( �߂�l true:�o�H�\�z���� false:�o�H�\�z���s( �X�^�[�g�n�_�ƃS�[���n�_���q���o�H������������ ) )
				this->GoalPosition = GoalPos;			// �S�[���ʒu��ۑ�

				this->UnitArray.resize(m_BackGround->GetBuildData().size());			// �o�H�T���p�̃|���S�������i�[���郁�����̈���m�ہA������
				for (auto& p : this->UnitArray) {
					p.Init((int)(&p - &this->UnitArray.front()));
				}

				int StartIndex = m_BackGround->GetNearestBuilds(StartPos);	// �X�^�[�g�n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray[StartIndex];					// �X�^�[�g�n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�

				int GoalIndex = m_BackGround->GetNearestBuilds(GoalPos);		// �S�[���n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
				if (GoalIndex == -1) { return false; }
				this->GoalUnit = &this->UnitArray[GoalIndex];				// �S�[���n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
				if (GoalIndex == StartIndex) { return false; }				// �S�[���n�_�ɂ���|���S���ƃX�^�[�g�n�_�ɂ���|���S���������������� false ��Ԃ�

				PATHPLANNING_UNIT *ActiveFirstUnit = this->StartUnit;		// �o�H�T�������Ώۂ̃|���S���Ƃ��ăX�^�[�g�n�_�ɂ���|���S����o�^����
				// �o�H��T�����ăS�[���n�_�̃|���S���ɂ��ǂ蒅���܂Ń��[�v���J��Ԃ�
				while (true) {
					bool Goal = false;
					// �o�H�T�������ΏۂɂȂ��Ă���|���S�������ׂď���
					PATHPLANNING_UNIT *PUnit = ActiveFirstUnit;
					ActiveFirstUnit = nullptr;
					while (true) {
						// �|���S���̕ӂ̐������J��Ԃ�
						for (int K = 0; K < 4; K++) {
							int Index = m_BackGround->GetBuildData().at(PUnit->GetPolyIndex()).GetLinkPolyIndex(K);
							if (Index == -1) { continue; }											// �ӂɗאڂ���|���S���������ꍇ�͉������Ȃ�
							if (Index == this->StartUnit->GetPolyIndex()) { continue; }				//�X�^�[�g�n�_�̃|���S���������ꍇ�͉������Ȃ�
							auto& NowUnit = this->UnitArray[Index];
							if (!NowUnit.SetPrevPolyUnit(PUnit, K, m_BackGround)) {
								continue;
							}
							// ���̃��[�v�ōs���o�H�T�������Ώۂɒǉ�����A���ɒǉ�����Ă�����ǉ����Ȃ�
							if (!NowUnit.SearchThisUnit(ActiveFirstUnit)) {
								NowUnit.ActiveNextUnit = ActiveFirstUnit;
								ActiveFirstUnit = &NowUnit;
							}
							// �אڂ���|���S�����S�[���n�_�ɂ���|���S����������S�[���ɒH�蒅�����t���O�𗧂Ă�
							if (Index == this->GoalUnit->GetPolyIndex()) {
								Goal = true;
							}
						}
						PUnit = PUnit->ActiveNextUnit;
						if (PUnit == nullptr) { break; }
					}

					if (!ActiveFirstUnit) { return false; }			// �X�^�[�g�n�_�ɂ���|���S������S�[���n�_�ɂ���|���S���ɒH�蒅���Ȃ��Ƃ������ƂȂ̂� false ��Ԃ�
					if (Goal) { break; }
				}
				PATHPLANNING_UNIT::SetNextIndex(this->GoalUnit, this->StartUnit);		// �S�[���n�_�̃|���S������X�^�[�g�n�_�̃|���S���ɒH���Čo�H��̃|���S���Ɏ��Ɉړ����ׂ��|���S���̔ԍ���������
				return true;										// �����ɂ�����X�^�[�g�n�_����S�[���n�_�܂ł̌o�H���T���ł����Ƃ������ƂȂ̂� true ��Ԃ�
			}
			void Dispose(void) {
				this->UnitArray.clear();
			}
		};
		//AI�p
		class AIControl::Impl {
		public:
			ENUM_AI_PHASE							m_Phase{ENUM_AI_PHASE::Normal};
			PlayerID								m_MyCharaID{0};
			InputControl							m_MyInput;
		private:
			const PlayerID							m_TargetCharaID{0};

			int										TargetPathPlanningIndex{0};		// ���̒��Ԓn�_�ƂȂ�o�H��̃|���S���̌o�H�T����񂪊i�[����Ă��郁�����A�h���X���i�[����ϐ�
			PathChecker								m_PathChecker;

			std::shared_ptr<BackGroundClassMain>	m_BackGround{nullptr};				//BG

			float									m_PathUpdateTimer{0.f};
			float									m_CheckAgain{0.f};
			float									m_ShotTimer{0.f};
			float									m_BackTimer{0.f};
			float									m_RepopTimer{0.f};
			bool									m_CanRepop{true};
			float									m_MoveFrontTimer{0.f};

			int										m_LeanLR{0};
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		private:
			bool									IsGotLengthToTarget{true};
			float									LengthToTarget{0.f};
			Vector3DX								VectorToTarget;
			float					GetLengthToTarget() {
				if (IsGotLengthToTarget) {
					return LengthToTarget;
				}
				IsGotLengthToTarget = true;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

				auto MyPos = MyChara->GetEyeMatrix().pos();

				LengthToTarget = (TgtPos - MyPos).magnitude();
				VectorToTarget = (TgtPos - MyPos).normalized();

				return LengthToTarget;
			}
			const Vector3DX&		GetVectorToTarget() {
				if (IsGotLengthToTarget) {
					return VectorToTarget;
				}
				IsGotLengthToTarget = true;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

				auto MyPos = MyChara->GetEyeMatrix().pos();

				LengthToTarget = (TgtPos - MyPos).magnitude();
				VectorToTarget = (TgtPos - MyPos).normalized();

				return VectorToTarget;
			}
		private:
			void		Reset() noexcept {
				this->m_Phase = ENUM_AI_PHASE::Normal;
				this->m_CheckAgain = 0.f;
				this->m_PathUpdateTimer = 1.f;
			}
			void		ChangePoint() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto MyPos = MyChara->GetEyeMatrix().pos();

				auto Target = TgtPos;
				std::vector<int> SelList;
				SelList.clear();
				auto GetRandomMyPos = [&]() {
					if (SelList.size() == 0) {
						for (auto& C : this->m_BackGround->GetBuildData()) {
							if (C.GetMeshSel() < 0) { continue; }
							auto Vec = C.GetMatrix().pos() - MyPos; Vec.y = (0.f);
							if (Vec.magnitude() < 10.f * Scale_Rate) {
								SelList.emplace_back((int)(&C - &this->m_BackGround->GetBuildData().front()));
							}
						}
					}
					MyPos = this->m_BackGround->GetBuildData().at(SelList.at(GetRand((int)SelList.size() - 1))).GetMatrix().pos();
					};
				auto CheckPathToTarget = [&]() {
					m_PathChecker.Dispose();
					auto MyPos_XZ = MyPos; MyPos_XZ.y = (0.f);
					Target.y = (0.f);
					return m_PathChecker.Init(MyPos_XZ, Target);	// �w��̂Q�_�̌o�H����T������
					};

				if (GetLengthToTarget() > 20.f*Scale_Rate) {
					if (SelList.size() == 0) {
						for (auto& C : this->m_BackGround->GetBuildData()) {
							if (C.GetMeshSel() < 0) { continue; }
							auto Vec = C.GetMatrix().pos() - Target; Vec.y = (0.f);
							if (Vec.magnitude() < 10.f * Scale_Rate) {
								SelList.emplace_back((int)(&C - &this->m_BackGround->GetBuildData().front()));
							}
						}
					}
					Target = this->m_BackGround->GetBuildData().at(SelList.at(GetRand((int)SelList.size() - 1))).GetMatrix().pos();
				}
				this->TargetPathPlanningIndex = -1;
				for (int i = 0; i < 10; i++) {
					if (CheckPathToTarget()) {
						this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// �ړ��J�n���_�̈ړ����Ԓn�_�̌o�H�T�������X�^�[�g�n�_�ɂ���|���S���̏��
						break;
					}
					else {
						GetRandomMyPos();//�đI��
					}
				}
			}
			void		Repop(bool CanRepop) noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

				Vector3DX pos_t;
				while (true) {
					pos_t = this->m_BackGround->GetBuildData().at(GetRand((int)(this->m_BackGround->GetBuildData().size()) - 1)).GetMatrix().pos();

					Vector3DX EndPos = pos_t + Vector3DX::up() * 1.f*Scale_Rate;
					if (CanRepop) {
						if ((TgtPos - EndPos).magnitude() <= 10.f*Scale_Rate) { continue; }
					}
					else {
						if ((TgtPos - EndPos).magnitude() >= 15.f*Scale_Rate) { continue; }
					}
					if (this->m_BackGround->CheckLinetoMap(TgtPos, &EndPos, false)) {
						break;
					}
				}

				/*
				Vector3DX EndPos = pos_t + Vector3DX::up() * 10.f*Scale_Rate;
				if (this->m_BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * -10.f*Scale_Rate, &EndPos, false)) {
					pos_t = EndPos;
				}
				//*/

				MyChara->MovePoint(deg2rad(0.f), deg2rad(GetRandf(180.f)), pos_t, 0);
				MyChara->Heal(100, true);
				this->Reset();
			}
			void		AimDir(const Vector3DX& VEC) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				auto Vec = VEC; Vec.y = (0.f);

				auto Dir = MyChara->GetEyeMatrix().zvec() * -1.f;
				auto Dir_XZ = Dir; Dir_XZ.y = (0.f);
				{
					Vector3DX DirHY; DirHY.Set(Dir_XZ.magnitude(), 0.f, Dir.y);
					Vector3DX VecHY; VecHY.Set(Vec.magnitude(), 0.f, VEC.y);
					auto IsFront = ((Vector3DX::Dot(DirHY.normalized(), VecHY.normalized())) > 0.f);
					auto cross = Vector3DX::Cross(DirHY.normalized(), VecHY.normalized()).y;
					m_MyInput.SetAddxRad(IsFront ? (-0.04f*cross) : 0.f);
				}
				{
					auto IsFront = ((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized())) > 0.f);
					auto cross = Vector3DX::Cross(Dir_XZ.normalized(), Vec.normalized()).y;
					if (IsFront) {
						if (abs(cross) < 0.4f) {
							m_MyInput.SetAddyRad(cross * 0.07f);
						}
						else {
							m_MyInput.SetAddyRad(cross * 0.14f);
						}
					}
					else {
						m_MyInput.SetAddyRad((cross > 0) ? 0.1f : -0.1f);
					}
					if (!(IsFront && (abs(cross) < 0.4f))) {
						if (m_MyInput.GetAddyRad() > 0.07f) {
							this->m_LeanLR = 1;
						}
						if (m_MyInput.GetAddyRad() < -0.07f) {
							this->m_LeanLR = -1;
						}
					}
				}
			}
		public:
			void		SetupBackGround(const std::shared_ptr<BackGroundClassMain>& BackBround_t) noexcept {
				m_BackGround = BackBround_t;
				m_PathChecker.SetBackGround(this->m_BackGround);
			}
			void		SetMyCharaID(PlayerID MyCharaID) noexcept { this->m_MyCharaID = MyCharaID; }

			const bool CannotRepop() const noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				return (!m_CanRepop && !MyChara->IsAlive());
			}
		public:
			void		Init() noexcept {
				this->Reset();
				this->m_PathUpdateTimer = 0.f;
				this->m_MoveFrontTimer = (float)(GetRand(6));
				this->m_CanRepop = true;
			}
			//
			void		Execute_Before(bool CanRepop) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//������
				m_MyInput.ResetAllInput();
				IsGotLengthToTarget = false;
				//�O����
				m_PathUpdateTimer = std::max(m_PathUpdateTimer - 1.f / DrawParts->GetFps(), 0.f);
				if (m_PathUpdateTimer <= 0.f) {
					m_PathUpdateTimer += 1.f;
					this->ChangePoint();
				}

				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				m_MyInput.SetInputPADS(PADS::SQUAT, MyChara->GetIsSquat() && (GetRand(100) < 1));

				if (m_CanRepop) {
					if (
						(CanRepop && !MyChara->CanLookTarget && (GetLengthToTarget() > 10.f*Scale_Rate)) ||
						(!CanRepop && !MyChara->IsAlive())
						) {
						this->m_RepopTimer += 1.f / DrawParts->GetFps();
						if (this->m_RepopTimer > 10.f) {
							this->m_RepopTimer -= 10.f;
							Repop(CanRepop);
							m_CanRepop = CanRepop;
						}
					}
					else {
						this->m_RepopTimer = 0.f;
					}
				}
				else {
					this->m_RepopTimer = 0.f;
				}
				//CanRepop
				if (!MyChara->IsAlive()) {
					this->m_Phase = ENUM_AI_PHASE::Dead;
				}
			}
			void		Execute_Normal() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto MyPos = MyChara->GetEyeMatrix().pos();
				//
				if (this->m_MoveFrontTimer > 6.f) {
					this->m_MoveFrontTimer -= 6.f;
				}
				this->m_MoveFrontTimer += 1.f / DrawParts->GetFps();
				m_MyInput.SetInputPADS(PADS::MOVE_W, this->m_MoveFrontTimer > 4.f);
				m_MyInput.SetInputPADS(PADS::MOVE_A, GetRand(100) > 50);
				m_MyInput.SetInputPADS(PADS::MOVE_D, GetRand(100) > 50);
				//�G�C��
				Vector3DX Vec = m_PathChecker.GetNextPoint(MyPos, &this->TargetPathPlanningIndex) - MyPos; Vec.y = (0.f);
				AimDir(Matrix4x4DX::Vtrans(Vec, Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(15.f))) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(15.f)))));
				//
				if (MyChara->CanLookTarget) {
					this->m_CheckAgain = 0.f;
					this->m_Phase = ENUM_AI_PHASE::Shot;
					if (GetLengthToTarget() < 10.f*Scale_Rate) {
						auto SE = SoundPool::Instance();
						if (GetRand(1) == 0) {
							SE->Get((int)SoundEnum::Man_contact).Play_3D(0, MyPos, Scale_Rate * 10.f);
						}
						else {
							SE->Get((int)SoundEnum::Man_openfire).Play_3D(0, MyPos, Scale_Rate * 10.f);
						}
					}
				}
			}
			void		Execute_Shot() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				//�G�C��
				AimDir(Matrix4x4DX::Vtrans(GetVectorToTarget(), Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(15.f))) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(15.f)))));
				//���[��
				m_MyInput.SetInputPADS(PADS::LEAN_L, (this->m_LeanLR == 1));
				m_MyInput.SetInputPADS(PADS::LEAN_R, (this->m_LeanLR == -1));
				//���
				if (MyChara->CanLookTarget) {
					this->m_BackTimer = std::max(this->m_BackTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (this->m_BackTimer == 0.f) {
						this->m_BackTimer = 3.f + GetRandf(2.f);
					}
					if (this->m_BackTimer <= 1.f) {
						m_MyInput.SetInputPADS(PADS::MOVE_S, true);
					}
				}
				else {
					this->m_BackTimer = 3.f + GetRandf(2.f);
				}
				//
				this->m_ShotTimer = std::max(this->m_ShotTimer - 1.f / DrawParts->GetFps(), 0.f);
				if (this->m_ShotTimer == 0.f) {
					m_MyInput.SetInputPADS(PADS::SHOT, true);
					this->m_ShotTimer = MyChara->CanLookTarget ? ((float)(10 + GetRand(100)) / 100.f) : ((float)(50 + GetRand(400)) / 100.f);
				}
				//
				if (!MyChara->CanLookTarget) {
					if (
						(GetLengthToTarget() > 20.f*Scale_Rate) ||
						(this->m_CheckAgain == 0.f)
						) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					this->m_CheckAgain = std::max(this->m_CheckAgain - 1.f / DrawParts->GetFps(), 0.f);
				}
				else {
					this->m_CheckAgain = 5.f;
				}
			}
			void		Execute_Dead() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				if (MyChara->IsAlive()) {
					this->m_Phase = ENUM_AI_PHASE::Normal;
				}
			}
		};
		//
		AIControl::AIControl() {
			m_Param = new Impl;
		}
		AIControl::~AIControl() {
			delete m_Param;
		}
		//
		void AIControl::Init(const std::shared_ptr<BackGroundClassMain>& BackBround_t, PlayerID MyCharaID) noexcept {
			this->GetParam()->SetupBackGround(BackBround_t);
			this->GetParam()->SetMyCharaID(MyCharaID);
			this->GetParam()->Init();
		}
		void AIControl::Execute(InputControl* MyInput, bool CanRepop) noexcept {
			//return;
			//AI
			this->GetParam()->Execute_Before(CanRepop);
			switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Shot:
					this->GetParam()->Execute_Shot();
					break;
				case ENUM_AI_PHASE::Dead:
					this->GetParam()->Execute_Dead();
					break;
				default:
					break;
			}
			*MyInput = this->GetParam()->m_MyInput;
		}

		const bool AIControl::CannotRepop() noexcept {
			return this->GetParam()->CannotRepop();
		}
	};
};
