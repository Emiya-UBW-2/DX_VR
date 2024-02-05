#include	"CPU.hpp"
#include "../../NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PathChecker {
			std::shared_ptr<BackGroundClassMain>	m_BackGround{ nullptr };				//BG
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
					auto& Unit = m_BackGround->GetBuildDatas().at(PUnit->PolyIndex);

					auto trisdistance = PUnit->TotalDistance +
						(m_BackGround->GetBuildDatas().at(Unit.GetLinkPolyIndex(tris)).GetMatrix().pos() + Unit.GetMatrix().pos()).Length();

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
			VECTOR_ref GoalPosition;					// �ڕW�ʒu
			std::vector<PATHPLANNING_UNIT>UnitArray;	// �o�H�T�������Ŏg�p����S�|���S���̏��z�񂪊i�[���ꂽ�������̈�̐擪�������A�h���X���i�[����ϐ�
			PATHPLANNING_UNIT *StartUnit{ nullptr };			// �o�H�̃X�^�[�g�n�_�ɂ���|���S�����ւ̃������A�h���X���i�[����ϐ�
			PATHPLANNING_UNIT *GoalUnit{ nullptr };				// �o�H�̃S�[���n�_�ɂ���|���S�����ւ̃������A�h���X���i�[����ϐ�
		public:
			void		SetBackGround(const std::shared_ptr<BackGroundClassMain>& BackBround_t) noexcept { m_BackGround = BackBround_t; }
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			VECTOR_ref GetNextPoint(const VECTOR_ref& NowPosition, int *TargetPathPlanningIndex) const {
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
					return m_BackGround->GetBuildDatas().at(*TargetPathPlanningIndex).GetMatrix().pos();
				}
				else {
					// �����͖ڕW���W
					return  this->GoalPosition;
				}
			}
		public:
			bool Init(VECTOR_ref StartPos, VECTOR_ref GoalPos) {
				// �w��̂Q�_�̌o�H��T������( �߂�l  true:�o�H�\�z����  false:�o�H�\�z���s( �X�^�[�g�n�_�ƃS�[���n�_���q���o�H������������ ) )
				this->GoalPosition = GoalPos;			// �S�[���ʒu��ۑ�

				this->UnitArray.resize(m_BackGround->GetBuildDatas().size());			// �o�H�T���p�̃|���S�������i�[���郁�����̈���m�ہA������
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
							int Index = m_BackGround->GetBuildDatas().at(PUnit->GetPolyIndex()).GetLinkPolyIndex(K);
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
			ENUM_AI_PHASE							m_Phase{ ENUM_AI_PHASE::Normal };
			PlayerID								m_MyCharaID{ 0 };
			PlayerID								m_TargetCharaID{ 0 };
		private:
			int			TargetPathPlanningIndex;		// ���̒��Ԓn�_�ƂȂ�o�H��̃|���S���̌o�H�T����񂪊i�[����Ă��郁�����A�h���X���i�[����ϐ�
			PathChecker m_PathChecker;
			float									m_PathUpdateTimer{ 0.f };

			std::shared_ptr<BackGroundClassMain>	m_BackGround{ nullptr };				//BG
			float									m_CheckAgain{ 0.f };
			float									m_ShotTimer{ 0.f };
			float									m_BackTimer{ 0.f };
			int										m_LeanLR{ 0 };
		public:
			BOOL CanLookTarget{ true };
			float									m_RepopTimer{ 0.f };
		private:
			bool W_key{ false };
			bool S_key{ false };
			bool A_key{ false };
			bool D_key{ false };
			bool Run_key{ false };
			bool Q_key{ false };
			bool E_key{ false };
			bool R_key{ false };
			bool F_key{ false };
			bool C_key{ false };
			bool shotMain_Key{ false };
			int32_t x_m{ 0 }, y_m{ 0 };
		public:
			Impl(void) noexcept { }
			~Impl(void) noexcept { }
		public:
			auto&		GetBackGround() noexcept { return m_BackGround; }
		private:
			void		SetBackGround(const std::shared_ptr<BackGroundClassMain>& BackBround_t) noexcept { m_BackGround = BackBround_t; }
		public:
			void		Reset() noexcept {
				this->m_Phase = ENUM_AI_PHASE::Normal;
				this->m_CheckAgain = 0.f;
			}

			void		ChangePoint() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();

				auto MyPos_XZ = MyChara->GetMove().pos; MyPos_XZ.y(0.f);
				auto TgtPos_XZ = TargetChara->GetMove().pos; TgtPos_XZ.y(0.f);
				auto Dir_t = TgtPos_XZ - MyPos_XZ;
				if (Dir_t.Length() > 10.f*Scale_Rate) {
					std::vector<int> SelList;
					for (auto& C : this->m_BackGround->GetBuildDatas()) {
						if (C.GetMeshSel() < 0) { continue; }
						auto BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y(0.f);
						if ((BGPos_XZ - TgtPos_XZ).Length() < 10.f*Scale_Rate) {
							SelList.emplace_back((int)(&C - &this->m_BackGround->GetBuildDatas().front()));
						}
					}
					auto& C = this->m_BackGround->GetBuildDatas().at(SelList.at(GetRand((int)SelList.size() - 1)));
					auto BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y(0.f);
					TgtPos_XZ = BGPos_XZ;
				}
				m_PathChecker.Dispose();
				m_PathChecker.Init(MyPos_XZ, TgtPos_XZ);	// �w��̂Q�_�̌o�H����T������
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// �ړ��J�n���_�̈ړ����Ԓn�_�̌o�H�T�������X�^�[�g�n�_�ɂ���|���S���̏��
			}
		public:
			void		Init(const std::shared_ptr<BackGroundClassMain>& BackBround_t, PlayerID MyCharaID) noexcept {
				this->m_MyCharaID = MyCharaID;
				this->SetBackGround(BackBround_t);
				this->Reset();

				m_PathChecker.SetBackGround(this->m_BackGround);

				this->ChangePoint();
				m_PathUpdateTimer = 1.f;
			}
			void		Execute_Before() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto MyPos = MyChara->GetEyePosition();

				W_key = false;
				S_key = false;
				A_key = false;
				D_key = false;
				Run_key = false;
				Q_key = false;
				E_key = false;
				R_key = false;
				F_key = false;
				C_key = false;
				shotMain_Key = false;
				x_m = 0;
				y_m = 0;

				{
					//
					CanLookTarget = true;
					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() < 15.f*Scale_Rate) {
						for (auto& C : this->m_BackGround->GetBuildDatas()) {
							if (C.GetMeshSel() < 0) { continue; }
							auto StartPos = MyPos;
							auto EndPos = TgtPos;
							if (GetMinLenSegmentToPoint(StartPos, EndPos, C.GetMatrix().pos()) >= 5.f*Scale_Rate) { continue; }
							auto ret = C.GetColLine(StartPos, EndPos);
							if (ret.HitFlag == TRUE) {
								CanLookTarget = false;
								break;
							}
						}
					}
					else {
						CanLookTarget = false;
					}
				}

				if(m_PathUpdateTimer<=0.f){
					this->ChangePoint();
					m_PathUpdateTimer = 1.f;
				}
				else {
					m_PathUpdateTimer = std::max(m_PathUpdateTimer - 1.f / FPS, 0.f);
				}

				if (MyChara->GetIsSquat()) {
					this->C_key = GetRand(100) < 1;
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto TgtPos_XZ = TgtPos; TgtPos_XZ.y(0.f);

				auto Dir = MyChara->GetEyeVector();
				auto Dir_XZ = Dir; Dir_XZ.y(0.f);

				auto MyPos = MyChara->GetMove().pos;
				auto MyPos_XZ = MyPos; MyPos_XZ.y(0.f);

				auto Dir_t = TgtPos - MyPos;
				//
				Dir_XZ = Dir_XZ.Norm();

				VECTOR_ref GonePoint = m_PathChecker.GetNextPoint(MyPos, &this->TargetPathPlanningIndex);
				GonePoint.y(0.f);
				auto Vec = GonePoint - MyPos_XZ;

				A_key = GetRand(100) > 50;
				D_key = GetRand(100) > 50;
				auto dot = Dir_XZ.dot(Vec.Norm());
				auto cross = Dir_XZ.cross(Vec.Norm()).y();
				if (dot > 0.f) {
					if (abs(cross) < sin(deg2rad(40.f))) {
						W_key = true;

						if (abs(cross) < sin(deg2rad(10.f))) {
							if (Dir_t.Length() < 10.f*Scale_Rate) {
								Run_key = true;
							}
						}
					}
					y_m = (int32_t)(700.f*cross);
				}
				else {
					y_m = (int32_t)((cross > 0) ? 1000.f : -1000.f);
				}
				this->m_CheckAgain = 0.f;
				if (CanLookTarget) {
					if (Dir_t.Length() < 20.f*Scale_Rate) {
						this->m_Phase = ENUM_AI_PHASE::Shot;
						if (Dir_t.Length() < 5.f*Scale_Rate) {
							auto SE = SoundPool::Instance();
							if (GetRand(1) == 0) {
								SE->Get((int)SoundEnum::Man_contact).Play_3D(0, Dir, Scale_Rate * 10.f);
							}
							else {
								SE->Get((int)SoundEnum::Man_openfire).Play_3D(0, Dir, Scale_Rate * 10.f);
							}
						}
					}
				}
			}
			void		Execute_Shot() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto TgtPos_XZ = TgtPos; TgtPos_XZ.y(0.f);

				auto Dir = MyChara->GetEyeVector();
				auto Dir_XZ = Dir; Dir_XZ.y(0.f);

				auto MyPos = MyChara->GetMove().pos;
				auto MyPos_XZ = MyPos; MyPos_XZ.y(0.f);

				//
				auto VecT = TgtPos - MyChara->GetEyePosition();
				VecT = MATRIX_ref::Vtrans(VecT.Norm(), MATRIX_ref::RotX(deg2rad(GetRandf(25.f))) * MATRIX_ref::RotY(deg2rad(GetRandf(25.f))));
				auto Vec = VecT; Vec.y(0.f);
				{
					auto dot = Dir_XZ.Norm().dot(Vec.Norm());
					auto cross = Dir_XZ.Norm().cross(Vec.Norm()).y();
					if (dot > 0.f) {
						if (abs(cross) < 0.4f) {
							y_m = (int32_t)(700.f*cross);
						}
						else {
							y_m = (int32_t)(1400.f*cross);
						}
					}
					else {
						y_m = (int32_t)((cross > 0) ? 1000.f : -1000.f);
					}

					if (!((dot > 0.f) && (abs(cross) < 0.4f))) {
						if (y_m > 700) {
							this->m_LeanLR = 1;
						}
						if (y_m < -700) {
							this->m_LeanLR = -1;
						}
					}
				}
				{
					VECTOR_ref DirHY; DirHY.Set(Dir_XZ.Length(), 0.f, Dir.y()); DirHY = DirHY.Norm();
					VECTOR_ref VecHY; VecHY.Set(Vec.Length(), 0.f, VecT.y()); VecHY = VecHY.Norm();
					auto dot = DirHY.dot(VecHY);
					auto cross = DirHY.cross(VecHY).y();
					if (dot > 0.f) {
						x_m = -(int32_t)(400.f*cross);
					}
				}
				//���[��
				if (this->m_LeanLR == 1) {
					Q_key = true;
				}
				else if (this->m_LeanLR == -1) {
					E_key = true;
				}
				//
				if (this->m_ShotTimer == 0.f) {
					shotMain_Key = GetRand(100) < 10;
					if (shotMain_Key) {
						if (CanLookTarget) {
							this->m_ShotTimer = (float)(50 + GetRand(200)) / 100.f;
						}
						else {
							this->m_ShotTimer = (float)(10 + GetRand(100)) / 100.f;
						}
					}
				}
				else {
					this->m_ShotTimer = std::max(this->m_ShotTimer - 1.f / FPS, 0.f);
				}

				if (!CanLookTarget) {
					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() > 20.f*Scale_Rate) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					if (this->m_CheckAgain == 0.f) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					this->m_CheckAgain = std::max(this->m_CheckAgain - 1.f / FPS, 0.f);
					this->m_BackTimer = 3.f + GetRandf(2.f);
				}
				else {
					this->m_CheckAgain = 5.f;

					this->m_BackTimer = std::max(this->m_BackTimer - 1.f / FPS, 0.f);
					if (this->m_BackTimer == 0.f) {
						this->m_BackTimer = 3.f + GetRandf(2.f);
					}
					if (this->m_BackTimer <= 1.f) {
						S_key = true;
					}
				}
			}
			void		Execute_After(InputControl* MyInput) noexcept {
				MyInput->SetInputStart((float)this->x_m / 10000.f, (float)this->y_m / 10000.f, VECTOR_ref::zero());
				MyInput->SetInputPADS(PADS::MOVE_W, this->W_key);
				MyInput->SetInputPADS(PADS::MOVE_S, this->S_key);
				MyInput->SetInputPADS(PADS::MOVE_A, this->A_key);
				MyInput->SetInputPADS(PADS::MOVE_D, this->D_key);
				MyInput->SetInputPADS(PADS::RUN, this->Run_key);
				MyInput->SetInputPADS(PADS::LEAN_L, this->Q_key);
				MyInput->SetInputPADS(PADS::LEAN_R, this->E_key);
				MyInput->SetInputPADS(PADS::MELEE, false);
				MyInput->SetInputPADS(PADS::RELOAD, this->R_key);
				MyInput->SetInputPADS(PADS::INTERACT, this->F_key);
				MyInput->SetInputPADS(PADS::SQUAT, this->C_key);
				MyInput->SetInputPADS(PADS::SHOT, this->shotMain_Key);
				MyInput->SetInputPADS(PADS::AIM, false);
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
			this->GetParam()->Init(BackBround_t, MyCharaID);
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->GetParam()->m_MyCharaID).GetChara();
			auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->GetParam()->m_TargetCharaID).GetChara();

			//AI
			this->GetParam()->Execute_Before();
			if (MyChara->IsAlive()) {
				switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Shot:
					this->GetParam()->Execute_Shot();
					break;
				default:
					break;
				}

				if (!this->GetParam()->CanLookTarget) {
					if ((TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos() - MyChara->GetEyePosition()).Length() > 15.f*Scale_Rate) {
						if (this->GetParam()->m_RepopTimer > 10.f) {
							auto TgtPos_XZ = TargetChara->GetMove().pos; TgtPos_XZ.y(0.f);
							VECTOR_ref pos_t;
							while (true) {
								auto& C = this->GetParam()->GetBackGround()->GetBuildDatas().at(GetRand((int)(this->GetParam()->GetBackGround()->GetBuildDatas().size()) - 1));
								pos_t = C.GetMatrix().pos(); pos_t.y(0.f);
								if ((pos_t - TgtPos_XZ).Length() > 10.f*Scale_Rate) {
									break;
								}
							}

							auto HitResult = this->GetParam()->GetBackGround()->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
							if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }


							MyChara->ValueSet(deg2rad(0.f), deg2rad(GetRandf(180.f)), pos_t, MyChara->GetMyPlayerID());
							MyChara->Heal(100);
							this->GetParam()->Reset();
							this->GetParam()->m_RepopTimer = 0.f;
						}
						else {
							this->GetParam()->m_RepopTimer += 1.f / FPS;
						}
					}
					else {
						this->GetParam()->m_RepopTimer = 0.f;
					}
				}
			}
			else {
				if (!this->GetParam()->CanLookTarget) {
					if ((TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos() - MyChara->GetEyePosition()).Length() > 15.f*Scale_Rate) {
						auto TgtPos_XZ = TargetChara->GetMove().pos; TgtPos_XZ.y(0.f);
						VECTOR_ref pos_t;
						while (true) {
							auto& C = this->GetParam()->GetBackGround()->GetBuildDatas().at(GetRand((int)(this->GetParam()->GetBackGround()->GetBuildDatas().size()) - 1));
							pos_t = C.GetMatrix().pos(); pos_t.y(0.f);
							if ((pos_t - TgtPos_XZ).Length() > 10.f*Scale_Rate) {
								break;
							}
						}

						auto HitResult = this->GetParam()->GetBackGround()->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
						if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }


						MyChara->ValueSet(deg2rad(0.f), deg2rad(GetRandf(180.f)), pos_t, MyChara->GetMyPlayerID());
						MyChara->Heal(100);
						this->GetParam()->Reset();
					}
				}
			}
			this->GetParam()->Execute_After(MyInput);
		}
	};
};
