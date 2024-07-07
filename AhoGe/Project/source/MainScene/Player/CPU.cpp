#include	"CPU.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Caution,
			Alert,
			Dead,
		};
		//
		class PathChecker {
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
				bool SetPrevPolyUnit(PATHPLANNING_UNIT *PUnit, int tris) {
					auto* BackGround = BackGroundClassBase::Instance();
					// �אڂ���|���S�������Ɍo�H�T���������s���Ă��āA����苗���̒����o�H�ƂȂ��Ă���ꍇ�͉������Ȃ�
					auto& Unit = BackGround->GetBuildData(PUnit->PolyIndex);

					auto trisdistance = PUnit->TotalDistance +
						(BackGround->GetBuildData(Unit->GetLinkPolyIndex(tris))->m_Pos + Unit->m_Pos).magnitude();

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
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			Vector3DX GetNextPoint(const Vector3DX& NowPosition, int *TargetPathPlanningIndex) const {
				auto* BackGround = BackGroundClassBase::Instance();
				int NowIndex = BackGround->GetNearestBuilds(NowPosition);
				if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// ���ݏ���Ă���|���S�����S�[���n�_�ɂ���|���S���̏ꍇ�͏����𕪊�
					if (NowIndex == *TargetPathPlanningIndex) {													// ���ݏ���Ă���|���S�����ړ����Ԓn�_�̃|���S���̏ꍇ�͎��̒��Ԓn�_�����肷�鏈�����s��
						const float COLLWIDTH = 0.1f * ((64.f * 64.f) / (1080 / 2));				// �����蔻��̃T�C�Y
						while (true) {																				// ���̒��Ԓn�_�����肷��܂Ń��[�v��������
							if (!this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()) { break; }
							auto NextIndex = this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()->GetPolyIndex();
							if (!BackGround->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// �o�H��̎��̃|���S���̒��S���W�ɒ����I�Ɉړ��ł��Ȃ��ꍇ�̓��[�v���甲����
							(*TargetPathPlanningIndex) = NextIndex;													// �`�F�b�N�Ώۂ��o�H��̍X�Ɉ��̃|���S���ɕύX����
							if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// �����S�[���n�_�̃|���S���������烋�[�v�𔲂���
						}
					}
					// �ړ����������肷��A�ړ������͌��݂̍��W���璆�Ԓn�_�̃|���S���̒��S���W�Ɍ���������
					return BackGround->GetBuildData(*TargetPathPlanningIndex)->m_Pos;
				}
				else {
					// �����͖ڕW���W
					return this->GoalPosition;
				}
			}
		public:
			bool Init(Vector3DX StartPos, Vector3DX GoalPos) {
				auto* BackGround = BackGroundClassBase::Instance();
				// �w��̂Q�_�̌o�H��T������( �߂�l true:�o�H�\�z���� false:�o�H�\�z���s( �X�^�[�g�n�_�ƃS�[���n�_���q���o�H������������ ) )
				this->GoalPosition = GoalPos;			// �S�[���ʒu��ۑ�

				this->UnitArray.resize(BackGround->GetBuildDataNum());			// �o�H�T���p�̃|���S�������i�[���郁�����̈���m�ہA������
				for (auto& p : this->UnitArray) {
					p.Init((int)(&p - &this->UnitArray.front()));
				}

				int StartIndex = BackGround->GetNearestBuilds(StartPos);	// �X�^�[�g�n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray[StartIndex];					// �X�^�[�g�n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�

				int GoalIndex = BackGround->GetNearestBuilds(GoalPos);		// �S�[���n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
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
							int Index = BackGround->GetBuildData(PUnit->GetPolyIndex())->GetLinkPolyIndex(K);
							if (Index == -1) { continue; }											// �ӂɗאڂ���|���S���������ꍇ�͉������Ȃ�
							if (Index == this->StartUnit->GetPolyIndex()) { continue; }				//�X�^�[�g�n�_�̃|���S���������ꍇ�͉������Ȃ�
							auto& NowUnit = this->UnitArray[Index];
							if (!NowUnit.SetPrevPolyUnit(PUnit, K)) {
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
		private:
			const PlayerID							m_TargetCharaID{0};
		public:
			int										TargetPathPlanningIndex{0};		// ���̒��Ԓn�_�ƂȂ�o�H��̃|���S���̌o�H�T����񂪊i�[����Ă��郁�����A�h���X���i�[����ϐ�
			PathChecker								m_PathChecker;
			float									m_PathUpdateTimer{0.f};
			//
			ENUM_AI_PHASE							m_Phase{ENUM_AI_PHASE::Normal};
			PlayerID								m_MyCharaID{0};
			InputControl							m_MyInput;
			float									m_Rad{0.f};
			Vector3DX								m_LastFindPos{};
			float									m_LostTimer{0.f};
			float									m_GraphTimer{0.f};
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		public:
			const auto& GetViewRad() const noexcept { return m_Rad; }
		private:
			const auto GetIsSeeTarget() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				if (Chara->CanLookPlayer0()) {
					float ViewLimit = 10.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						Vector3DX Vec;Vec.Set(std::sin(Chara->GetViewRad()), std::cos(Chara->GetViewRad()), 0.f);
						Vector3DX vec_a;vec_a = (Chara->GetPos() - Target->GetPos()).normalized();
						if (-Vector3DX::Dot(vec_a, Vec) > std::cos(deg2rad(45))) {
							m_LastFindPos = Target->GetPos();
							return true;
						}
					}
				}
				return false;
			}
			//
			void		ChangePoint() noexcept {
				auto* BackGround = BackGroundClassBase::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();

				auto TgtPos = Target->GetPos();
				/*
				std::vector<int> SelList = BackGround->GetNearBuildsList(TgtPos);
				if (SelList.size() > 0) {
					TgtPos = BackGround->GetBuildData(SelList.at(GetRand((int)SelList.size() - 1)))->m_Pos;
				}
				//*/
				m_PathChecker.Dispose();
				m_PathChecker.Init(Chara->GetPos(), TgtPos);	// �w��̂Q�_�̌o�H����T������
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// �ړ��J�n���_�̈ړ����Ԓn�_�̌o�H�T�������X�^�[�g�n�_�ɂ���|���S���̏��
			}
		public:
			void		Init(PlayerID MyCharaID) noexcept {
				m_MyCharaID = MyCharaID;
				this->m_PathUpdateTimer = 0.f;
			}
			//
			void		Execute_Before() noexcept {
				auto* DrawParts = DXDraw::Instance();
				//������
				m_MyInput.ResetAllInput();
				this->m_GraphTimer = std::max(this->m_GraphTimer - 1.f / DrawParts->GetFps(), 0.f);
				//
				m_PathUpdateTimer = std::max(m_PathUpdateTimer - 1.f / DrawParts->GetFps(), 0.f);
				if (m_PathUpdateTimer <= 0.f) {
					m_PathUpdateTimer += 1.f;
					this->ChangePoint();
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//�T��
				{
					m_LastFindPos = m_PathChecker.GetNextPoint(Chara->GetPos(), &this->TargetPathPlanningIndex);

					float Len = 0.1f * ((64.f * 64.f) / (1080 / 2));
					auto Vec = m_LastFindPos - Chara->GetPos();
					if (Vec.magnitude() > Len) {
						m_MyInput.SetInputPADS(PADS::MOVE_W, Vec.y > 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_S, Vec.y < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_A, Vec.x < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_D, Vec.x > 0.f);
					}
				}
				//
				if (GetIsSeeTarget()) {
					float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					else {
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				else {
					float NearLimit = (1.5f + 1.f * Target->GetSpeed() / 128.f) * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < NearLimit) {
						m_LastFindPos = Target->GetPos();
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));

				this->m_Phase = ENUM_AI_PHASE::Normal;
			}
			void		Execute_Caution() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));
				//
				if (GetIsSeeTarget()) {
					float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
						this->m_GraphTimer = 2.f;
					}
				}
			}
			void		Execute_Alert() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(Target->GetPos()));
				//
				m_LastFindPos = Target->GetPos();
				//
				if (GetIsSeeTarget()) {
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						m_LostTimer = 5.f;
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
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
		const float & AIControl::GetViewRad() const noexcept { return this->GetParam()->GetViewRad(); }
		const float & AIControl::GetGraphTimer() const noexcept { return this->GetParam()->m_GraphTimer; }
		const bool AIControl::IsCaution() const noexcept { return this->GetParam()->m_Phase == ENUM_AI_PHASE::Caution; }
		const bool AIControl::IsAlert() const noexcept { return this->GetParam()->m_Phase == ENUM_AI_PHASE::Alert; }
		//
		void AIControl::Init(PlayerID MyCharaID) noexcept {
			this->GetParam()->Init(MyCharaID);
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			//return;
			//AI
			this->GetParam()->Execute_Before();
			switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Caution:
					this->GetParam()->Execute_Caution();
					break;
				case ENUM_AI_PHASE::Alert:
					this->GetParam()->Execute_Alert();
					break;
				case ENUM_AI_PHASE::Dead:
					break;
				default:
					break;
			}
			*MyInput = this->GetParam()->m_MyInput;
		}
	};
};
