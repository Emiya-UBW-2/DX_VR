#include	"CPU.hpp"
#include	"../../MainScene/Player/Player.hpp"

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
			// �o�H�T�������p�̂P�n�_�̏��
			class PATHPLANNING_UNIT {
				int Index{ 0 };						// �ԍ�
				float TotalDistance{ 0.f };				// �o�H�T���ł��̒n�_�ɓ��B����܂łɒʉ߂����n�_�Ԃ̋����̍��v
				PATHPLANNING_UNIT* PrevUnit{ nullptr };		// �o�H�T���Ŋm�肵���o�H��̈�O�̒n�_( ���n�_���o�H��ɖ����ꍇ�� nullptr )
				PATHPLANNING_UNIT* NextUnit{ nullptr };		// �o�H�T���Ŋm�肵���o�H��̈��̒n�_( ���n�_���o�H��ɖ����ꍇ�� nullptr )
			public:
				PATHPLANNING_UNIT* ActiveNextUnit{ nullptr };		// �o�H�T�������ΏۂɂȂ��Ă��鎟�̒n�_�̃������A�h���X���i�[����ϐ�
			public:
				const auto& GetIndex() const noexcept { return this->Index; }
				const auto& GetNextUnit() const noexcept { return this->NextUnit; }
			public:
				bool SetPrevUnit(PATHPLANNING_UNIT *PUnit, int tris) {
					auto* BackGround = BackGroundClassBase::Instance();
					// �אڂ���n�_�����Ɍo�H�T���������s���Ă��āA����苗���̒����o�H�ƂȂ��Ă���ꍇ�͉������Ȃ�
					auto& Unit = BackGround->GetFloorData(PUnit->Index);

					auto trisdistance = PUnit->TotalDistance + (BackGround->GetFloorData(Unit->LinkIndex.at(tris))->m_Pos + Unit->m_Pos).magnitude();

					if (this->TotalDistance > trisdistance) {
						this->TotalDistance = trisdistance;		// �אڂ���n�_�ɂ����ɓ��B����܂ł̋�����������
					}
					else {
						if (this->PrevUnit) { return false; }
					}
					this->PrevUnit = PUnit;			// �אڂ���n�_�Ɍo�H���ƂȂ鎩���̒n�_�̔ԍ���������
					return true;
				}
			public:
				static bool SetStartToGoal(const std::vector<PATHPLANNING_UNIT>& UnitArray, PATHPLANNING_UNIT* pStart, PATHPLANNING_UNIT* pGoal) {
					auto* BackGround = BackGroundClassBase::Instance();
					PATHPLANNING_UNIT* ActiveFirstUnit = pStart;		// �o�H�T�������Ώۂ̒n�_�Ƃ��ăX�^�[�g�n�_�ɂ���n�_��o�^����
					// �o�H��T�����ăS�[���n�_�̒n�_�ɂ��ǂ蒅���܂Ń��[�v���J��Ԃ�
					while (true) {
						bool Goal = false;
						// �o�H�T�������ΏۂɂȂ��Ă���n�_�����ׂď���
						PATHPLANNING_UNIT* PUnit = ActiveFirstUnit;
						ActiveFirstUnit = nullptr;
						while (true) {
							// �n�_�̕ӂ̐������J��Ԃ�
							for (int K = 0; K < 4; K++) {
								int Index = BackGround->GetFloorData(PUnit->GetIndex())->LinkIndex.at(K);
								if (Index == -1) { continue; }											// �ӂɗאڂ���n�_�������ꍇ�͉������Ȃ�
								if (Index == pStart->GetIndex()) { continue; }				// �X�^�[�g�n�_�̒n�_�������ꍇ�͉������Ȃ�
								PATHPLANNING_UNIT* NowUnit = (PATHPLANNING_UNIT*) & UnitArray.at(Index);
								if (!NowUnit->SetPrevUnit(PUnit, K)) {
									continue;
								}
								// ���̃��[�v�ōs���o�H�T�������Ώۂɒǉ�����A���ɒǉ�����Ă�����ǉ����Ȃ�
								auto SearchThisUnit = [&]() {
									PATHPLANNING_UNIT* PUnitSub2 = ActiveFirstUnit;
									while (true) {
										if (!PUnitSub2) { break; }
										if (PUnitSub2 == NowUnit) { return true; }// ���ɒǉ�����Ƃ�
										PUnitSub2 = PUnitSub2->ActiveNextUnit;
									}
									return PUnitSub2 != nullptr;
									};
								if (!SearchThisUnit()) {
									NowUnit->ActiveNextUnit = ActiveFirstUnit;
									ActiveFirstUnit = NowUnit;
								}
								// �אڂ���n�_���S�[���n�_�ɂ���n�_��������S�[���ɒH�蒅�����t���O�𗧂Ă�
								if (Index == pGoal->GetIndex()) {
									Goal = true;
								}
							}
							PUnit = PUnit->ActiveNextUnit;
							if (!PUnit) { break; }
						}

						if (!ActiveFirstUnit) { return false; }			// �X�^�[�g�n�_�ɂ���n�_����S�[���n�_�ɂ���n�_�ɒH�蒅���Ȃ��Ƃ������ƂȂ̂� false ��Ԃ�
						if (Goal) { break; }
					}
					// �S�[���n�_�̒n�_����X�^�[�g�n�_�̒n�_�ɒH���Čo�H��̒n�_�Ɏ��Ɉړ����ׂ��n�_�̔ԍ���������
					PATHPLANNING_UNIT* PUnit = pGoal;
					while (true) {
						auto* PrevPUnitIndex = PUnit;
						PUnit = PUnit->PrevUnit;
						PUnit->NextUnit = PrevPUnitIndex;
						if (PUnit == pStart) { break; }
					}
					return true;
				}
			public:
				void Init(int index) {
					this->Index = index;
					this->TotalDistance = 0.0f;
					this->PrevUnit = nullptr;
					this->NextUnit = nullptr;
					this->ActiveNextUnit = nullptr;
				}
			};
		private:
			Vector3DX GoalPosition;					// �ڕW�ʒu
			std::vector<PATHPLANNING_UNIT>UnitArray;	// �o�H�T�������Ŏg�p����S�n�_�̏��z�񂪊i�[���ꂽ�������̈�̐擪�������A�h���X���i�[����ϐ�
			PATHPLANNING_UNIT *StartUnit{nullptr};			// �o�H�̃X�^�[�g�n�_�ɂ���n�_���ւ̃������A�h���X���i�[����ϐ�
			PATHPLANNING_UNIT *GoalUnit{nullptr};				// �o�H�̃S�[���n�_�ɂ���n�_���ւ̃������A�h���X���i�[����ϐ�
		public:
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			Vector3DX GetNextPoint(const Vector3DX& NowPosition, int *TargetPathPlanningIndex) const {
				auto* BackGround = BackGroundClassBase::Instance();
				int NowIndex = BackGround->GetNearestFloors(NowPosition);
				if (NowIndex != this->GoalUnit->GetIndex()) {												// ���ݏ���Ă���n�_���S�[���n�_�ɂ���n�_�̏ꍇ�͏����𕪊�
					if (NowIndex == *TargetPathPlanningIndex) {												// ���ݏ���Ă���n�_���ړ����Ԓn�_�̒n�_�̏ꍇ�͎��̒��Ԓn�_�����肷�鏈�����s��
						while (true) {																		// ���̒��Ԓn�_�����肷��܂Ń��[�v��������
							auto* NextPtr = this->UnitArray.at(NowIndex).GetNextUnit();
							if (!NextPtr) { break; }
							if (!BackGround->CheckNextFloor(NowIndex, NextPtr->GetIndex())) { break; }		// �o�H��̎��̒n�_�̒��S���W�ɒ����I�Ɉړ��ł��Ȃ��ꍇ�̓��[�v���甲����
							NowIndex = NextPtr->GetIndex();													// �`�F�b�N�Ώۂ��o�H��̍X�Ɉ��̒n�_�ɕύX����
							if (NowIndex == this->GoalUnit->GetIndex()) { break; }							// �����S�[���n�_�̒n�_�������烋�[�v�𔲂���
						}
						*TargetPathPlanningIndex = NowIndex;
					}
					// �ړ����������肷��A�ړ������͌��݂̍��W���璆�Ԓn�_�̒n�_�̒��S���W�Ɍ���������
					return BackGround->GetFloorData(*TargetPathPlanningIndex)->m_Pos;
				}
				else {
					// �����͖ڕW���W
					return this->GoalPosition;
				}
			}
		public:
			// �w��̂Q�_�̌o�H��T������( �߂�l true:�o�H�\�z���� false:�o�H�\�z���s( �X�^�[�g�n�_�ƃS�[���n�_���q���o�H������������ ) )
			bool UpdatePath(const Vector3DX& StartPos, const Vector3DX& GoalPos) {
				auto* BackGround = BackGroundClassBase::Instance();
				this->GoalPosition = GoalPos;			// �S�[���ʒu��ۑ�
				// �o�H�T���p�̒n�_�����i�[���郁�����̈���m�ہA������
				for (auto& p : this->UnitArray) {
					p.Init((int)(&p - &this->UnitArray.front()));
				}
				//�X�^�[�g�n�_������
				int StartIndex = BackGround->GetNearestFloors(StartPos);
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray.at(StartIndex);
				//�S�[���n�_������
				int GoalIndex = BackGround->GetNearestFloors(this->GoalPosition);
				if (GoalIndex == -1) { return false; }
				this->GoalUnit = &this->UnitArray.at(GoalIndex);
				// �S�[���n�_�ɂ���n�_�ƃX�^�[�g�n�_�ɂ���n�_�������������� false ��Ԃ�
				if (GoalIndex == StartIndex) { return false; }
				// �o�H��T��
				return PATHPLANNING_UNIT::SetStartToGoal(UnitArray, this->StartUnit, this->GoalUnit);
			}
			void Init(void) {
				auto* BackGround = BackGroundClassBase::Instance();
				this->UnitArray.resize(BackGround->GetXSize() * BackGround->GetYSize());
			}
			void Dispose(void) {
				this->UnitArray.clear();
			}
		};
		// AI�p
		class AIControl::Impl {
		private:
			const PlayerID							m_TargetCharaID{0};
		public:
			int										TargetPathPlanningIndex{0};		// ���̒��Ԓn�_�ƂȂ�o�H��̒n�_�̌o�H�T����񂪊i�[����Ă��郁�����A�h���X���i�[����ϐ�
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
					float ViewLimit = Get2DSize(10.f);
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < ViewLimit * ViewLimit) {
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
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();

				auto TgtPos = Target->GetPos();
				//*
				auto* BackGround = BackGroundClassBase::Instance();
				std::vector<int> SelList = BackGround->GetNearFloorsList(TgtPos);
				if (SelList.size() > 0) {
					TgtPos = BackGround->GetFloorData(SelList.at(GetRand((int)SelList.size() - 1)))->m_Pos;
				}
				// */
				m_PathChecker.UpdatePath(Chara->GetPos(), TgtPos);	// �w��̂Q�_�̌o�H����T������
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetIndex();	// �ړ��J�n���_�̈ړ����Ԓn�_�̌o�H�T�������X�^�[�g�n�_�ɂ���n�_�̏��
			}
		public:
			void		Init() noexcept {
				this->m_PathUpdateTimer = -(float)m_MyCharaID / (float)Player_Num*5.f;
				m_PathChecker.Init();
			}
			// 
			void		Execute_Before() noexcept {
				auto* DrawParts = DXDraw::Instance();
				// ������
				m_MyInput.ResetAllInput();
				this->m_GraphTimer = std::max(this->m_GraphTimer - 1.f / DrawParts->GetFps(), 0.f);
				// 
				//printfDx("%f\n", m_PathUpdateTimer);
				m_PathUpdateTimer -= 1.f / DrawParts->GetFps();
				if (m_PathUpdateTimer <= 0.f) {
					m_PathUpdateTimer += 5.f;
					this->ChangePoint();
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				// �T��
				{
					m_LastFindPos = m_PathChecker.GetNextPoint(Chara->GetPos(), &this->TargetPathPlanningIndex);

					float Len = Get2DSize(0.1f);
					auto Vec = m_LastFindPos - Chara->GetPos();
					if (Vec.sqrMagnitude() > Len * Len) {
						m_MyInput.SetInputPADS(PADS::MOVE_W, Vec.y > 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_S, Vec.y < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_A, Vec.x < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_D, Vec.x > 0.f);
						m_MyInput.SetInputPADS(PADS::WALK, true);
					}
				}
				// 
				if (GetIsSeeTarget()) {
					float ViewLimit = Get2DSize(5.f);
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < ViewLimit * ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					else {
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				else {
					float NearLimit = Get2DSize(1.5f + 1.f * Target->GetSpeedPer());
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < NearLimit * NearLimit) {
						m_LastFindPos = Target->GetPos();
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				// 
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));

				//this->m_Phase = ENUM_AI_PHASE::Normal;
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
					float ViewLimit = Get2DSize(5.f);
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < ViewLimit * ViewLimit) {
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
			//
			void		Dispose() noexcept {
				m_PathChecker.Dispose();
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
		void AIControl::SetPlayerID(PlayerID MyCharaID) noexcept { this->GetParam()->m_MyCharaID = MyCharaID; }
		// 
		void AIControl::Init() noexcept {
			this->GetParam()->Init();
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			// return;
			// AI
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
		void AIControl::Draw() noexcept
		{
			//�o�H������
			/*
			float Radius = (float)GetDispSize(0.5f);

			{
				auto* BackGround = BackGroundClassBase::Instance();
				auto* PUnit = this->GetParam()->m_PathChecker.GetStartUnit();
				while (true) {
					auto DispPos = Convert2DtoDisp(BackGround->GetFloorData(PUnit->GetIndex())->m_Pos);
					DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, Red);
					PUnit = PUnit->GetNextUnit();
					if (!PUnit) { break; }
				}
			}
			{
				auto DispPos = Convert2DtoDisp(this->GetParam()->m_LastFindPos);
				DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, Red);
			}
			// */
		}
		void AIControl::Dispose() noexcept {
			this->GetParam()->Dispose();
		}
	};
};
