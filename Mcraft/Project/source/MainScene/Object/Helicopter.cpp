#include	"Helicopter.hpp"

#include	"../Player/Player.hpp"

namespace FPS_n2 {
	namespace Objects {
		const bool HelicopterObj::CheckAmmoHit(PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* EndPos) noexcept {
			//auto* SE = SoundPool::Instance();
			//とりあえず当たったかどうか探す
			if (!RefreshCol(StartPos, *EndPos, 10.0f * Scale3DRate)) {
				return false;
			}
			auto colres = GetColLine(StartPos, *EndPos, 0);
			if (colres.HitFlag == TRUE) {
				*EndPos = colres.HitPosition;
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					EffectSingleton::Instance()->SetOnce_Any(Effect::ef_gndsmoke, *EndPos, colres.Normal, 2.0f);
				}
				if (AttackID >= 0) {
					this->m_IsHit = true;
					return true;
				}
			}
			return false;
		}
		void HelicopterObj::SetAction(HelicopterMove Move) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();

			m_HelicopterMove = Move;
			m_Timer = 0.0f;
			switch (this->m_HelicopterMove) {
			case HelicopterMove::Random:
				m_PrevPos = m_NowPos;
				while (true) {
					m_TargetPos = Vector3DX::vget(GetRandf(16.f), 0.0f, GetRandf(16.f)) * Scale3DRate;
					if (!PlayerMngr->GetTeamHelicopter()) { break; }
					Vector3DX Vec = PlayerMngr->GetTeamHelicopter()->GetMove().GetPos() - m_TargetPos; Vec.y = 0.f;
					if (Vec.magnitude() > (10.f * Scale3DRate)) {
						break;
					}
				}
				break;
			case HelicopterMove::Rappelling:
				m_PrevPos = m_NowPos;
				{
					auto* BackGroundParts = BackGround::BackGroundControl::Instance();
					auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
					while (true) {
						m_TargetPos = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();
						if (std::abs(m_TargetPos.x) < 16.f * Scale3DRate && std::abs(m_TargetPos.z) < 16.f * Scale3DRate) {
							Vector3DX Vec = ViewChara->GetMove().GetPos() - m_TargetPos; Vec.y = 0.f;
							if (Vec.sqrMagnitude() > (8.f * Scale3DRate) * (8.f * Scale3DRate)) {
								break;
							}
						}
					}
					m_TargetPos.y = 0.f;
				}
				break;
			case HelicopterMove::Intercept:
				m_PrevPos = m_NowPos;
				m_TargetPos = Matrix3x3DX::Vtrans(Vector3DX::vget(0.0f, 0.0f, 15.0f + GetRandf(10.0f)) * Scale3DRate, Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180.0f))));
				break;
			default:
				break;
			}
			auto Vec = (m_TargetPos - m_PrevPos);
			if (Vec.magnitude() > 0.0f) {
				m_YradRT = rad2deg(std::atan2(-Vec.x, -Vec.z));
				if ((m_YradRT - m_Yrad) > deg2rad(180)) {
					m_YradRT -= deg2rad(360);
				}
				else if ((m_YradRT - m_Yrad) < -deg2rad(180)) {
					m_YradRT += deg2rad(360);
				}
			}
		}
		void HelicopterObj::FirstUpdate(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			m_RopePer = std::clamp(m_RopePer + (m_Rope ? 10.0f : -5.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 10.0f);
			Easing(&m_OpenPer, (m_Open) ? 1.0f : 0.0f, 0.95f, EasingType::OutExpo);

			SetAnimLoop(static_cast<int>(0), 1.0f);
			SetObj().SetAnim(static_cast<int>(0)).SetPer(1.0f);
			SetAnimLoop(static_cast<int>(1), 1.0f);
			SetObj().SetAnim(static_cast<int>(1)).SetPer(m_OpenPer);
			SetObj().UpdateAnimAll();

			{
				auto& chara = PlayerMngr->GetPlayer(m_TargetPlayerID)->GetChara();

				Vector3DX Vec = Matrix3x3DX::Vtrans((chara->GetFrameWorldMat(Charas::CharaFrame::Upper).pos() - SetObj().GetFramePosition(m_GunRot.GetFrameID())).normalized(), GetMove().GetMat().inverse());

				float YVec = 0.f;
				float XVec = 0.f;

				YVec = std::clamp(std::atan2f(-Vec.x, -Vec.z), deg2rad(-90), deg2rad(90));
				XVec = std::clamp(std::atan2f(Vec.y, std::hypotf(Vec.x, Vec.z)), deg2rad(-50), deg2rad(0));
				this->m_CanShot = (deg2rad(-90) < YVec && YVec < deg2rad(90)) && (deg2rad(-50) < XVec && XVec < deg2rad(0));
				if (this->m_HelicopterMove != HelicopterMove::Intercept) {
					XVec = 0.0f;
					YVec = 0.0f;
				}
				Easing(&m_GunRotR, Vector3DX::vget(XVec, YVec, 0.0f), 0.95f, EasingType::OutExpo);

				SetObj().SetFrameLocalMatrix(m_GunRot.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::up(), m_GunRotR.y) * m_GunRot.GetFrameLocalPosition());
				SetObj().SetFrameLocalMatrix(m_GunAngle.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), m_GunRotR.x) * m_GunAngle.GetFrameLocalPosition());
			}
			if (this->m_GunAmmo == 0) {
				this->m_ReloadTimer = std::max(this->m_ReloadTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
				if (this->m_ReloadTimer <= 0.0f) {
					this->m_GunAmmo = 5;
				}
			}
			else {
				this->m_ReloadTimer = 2.0f;
			}
			if (this->m_ShotTimer == 0.0f) {
				if (this->m_CanShot && (this->m_GunAmmo > 0) && (this->m_HelicopterMove == HelicopterMove::Intercept)) {
					this->m_ShotTimer = 0.2f;
					Vector3DX MuzzlePos = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::GunAngle))).pos();
					Vector3DX MuzzleVec = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::GunAngle))).zvec2();
					MuzzlePos += MuzzleVec * (1.0f * Scale3DRate);

					auto& AmmoData = Objects::AmmoDataManager::Instance()->Get(this->m_AmmoSpecID);

					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Shot1))->Play3D(GetMove().GetPos(), 100.0f * Scale3DRate);													//サウンド
					auto* OptionParts = OptionManager::Instance();
					if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_fire2, MuzzlePos, MuzzleVec, AmmoData->GetCaliber() * 10.0f * Scale3DRate, 2.0f);	//銃発砲エフェクトのセット
					}
					Objects::AmmoPool::Instance()->Put(&AmmoData, MuzzlePos, MuzzleVec, this->m_MyPlayerID);
					--this->m_GunAmmo;
				}
			}
			else {
				this->m_ShotTimer = std::max(this->m_ShotTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
			}

			if (this->m_RocketGunAmmo == 0) {
				this->m_RocketReloadTimer = std::max(this->m_RocketReloadTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
				if (this->m_RocketReloadTimer <= 0.0f) {
					this->m_RocketGunAmmo = 20;
				}
			}
			else {
				this->m_RocketReloadTimer = 5.0f;
			}
			if (this->m_RocketShotTimer == 0.0f) {
				if (this->m_CanShot && (this->m_RocketGunAmmo > 0) && (this->m_HelicopterMove == HelicopterMove::Intercept)) {
					this->m_RocketShotTimer = 0.15f;
					Vector3DX MuzzlePos = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>((m_RocketGunAmmo % 2 == 0) ? HeliFrame::rocket1 : HeliFrame::rocket2))).pos();
					Vector3DX MuzzleVec = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>((m_RocketGunAmmo % 2 == 0) ? HeliFrame::rocket1 : HeliFrame::rocket2))).zvec2();
					MuzzlePos += MuzzleVec * (1.0f * Scale3DRate);

					auto& AmmoData = Objects::AmmoDataManager::Instance()->Get(this->m_RocketSpecID);

					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::rolling_rocket))->Play3D(GetMove().GetPos(), 100.0f * Scale3DRate);													//サウンド
					auto* OptionParts = OptionManager::Instance();
					if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_fire2, MuzzlePos, MuzzleVec, 0.1f * Scale3DRate, 2.0f);	//銃発砲エフェクトのセット
					}
					Objects::AmmoPool::Instance()->Put(&AmmoData, MuzzlePos, MuzzleVec, this->m_MyPlayerID);
					--this->m_RocketGunAmmo;
				}
			}
			else {
				this->m_RocketShotTimer = std::max(this->m_RocketShotTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
			}

			m_Timer += DXLib_refParts->GetDeltaTime();

			switch (this->m_HelicopterMove) {
			case HelicopterMove::Random:
				if (this->m_IsHit) {
					SetAction(HelicopterMove::Intercept);
				}
				else {
					bool IsDeadAll = true;
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (chara->IsAlive()) {
							IsDeadAll = false;
							break;
						}
					}
					if (IsDeadAll) {
						SetAction(HelicopterMove::Rappelling);
					}
				}
				if (m_Timer > 10.0f) {
					SetAction(HelicopterMove::Random);
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.0f, 0.0f, 1.0f));
				break;
			case HelicopterMove::Rappelling:
				if (m_Timer <= 8.0f) {
					if (this->m_IsHit) {
						SetAction(HelicopterMove::Intercept);
						m_Rope = false;
						m_Open = false;
					}
				}
				else if (m_Timer <= 10.f) {
					m_Open = true;
				}
				else if (m_Timer <= 15.f) {
					m_Rope = true;
				}
				else {
					bool IsRappellingAny = false;
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (chara->GetIsRappelling()) {
							IsRappellingAny = true;
							break;
						}
					}
					if (!IsRappellingAny) {
						//緊急退避
						if (this->m_IsHit) {
							SetAction(HelicopterMove::Intercept);
							m_Rope = false;
							m_Open = false;
						}
						else{
							SetAction(HelicopterMove::Random);
							m_Rope = false;
							m_Open = false;
						}
					}
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.0f, 0.0f, 1.0f));
				break;
			case HelicopterMove::Intercept:
				if (m_Timer > 8.0f) {
					auto& chara = PlayerMngr->GetPlayer(m_TargetPlayerID)->GetChara();
					Vector3DX Vec = (chara->GetFrameWorldMat(Charas::CharaFrame::Upper).pos() - SetObj().GetFramePosition(m_GunRot.GetFrameID())).normalized();
					if (Vec.magnitude() > 0.0f) {
						m_YradRT = rad2deg(std::atan2(-Vec.x, -Vec.z));
						if ((m_YradRT - m_Yrad) > deg2rad(180)) {
							m_YradRT -= deg2rad(360);
						}
						else if ((m_YradRT - m_Yrad) < -deg2rad(180)) {
							m_YradRT += deg2rad(360);
						}

					}
					if (!chara->IsAlive()) {
						SetAction(HelicopterMove::Random);
						this->m_IsHit = false;
					}
				}
				if (m_Timer > 30.0f) {
					SetAction(HelicopterMove::Random);
					this->m_IsHit = false;
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.0f, 0.0f, 1.0f));
				break;
			default:
				break;
			}


			if (this->m_HelicopterMove == HelicopterMove::Rappelling) {
				Easing(&RopeVec, GetMove().GetMat().yvec(), 0.9f, EasingType::OutExpo);
			}
			else {
				Easing(&RopeVec, Vector3DX::up(), 0.9f, EasingType::OutExpo);
			}


			auto PrevPos = m_Pos;

			Easing(&m_PosR, m_NowPos + (Vector3DX::vget(0.0f, -20.0f, 0.0f) + Vector3DX::vget(GetRandf(1.0f), GetRandf(1.0f), GetRandf(1.0f))) * Scale3DRate, 0.95f, EasingType::OutExpo);
			Easing(&m_Pos, m_PosR, 0.95f, EasingType::OutExpo);

			auto Vec = Matrix3x3DX::Vtrans((m_Pos - PrevPos), GetMove().GetMat().inverse()) / (60.0f * DXLib_refParts->GetDeltaTime());
			Easing(&m_flontSpeedPer, std::clamp(-Vec.z, -1.0f, 1.0f), 0.975f, EasingType::OutExpo);
			Easing(&m_SideSpeedPer, std::clamp(-Vec.x, -1.0f, 1.0f), 0.975f, EasingType::OutExpo);

			auto PrevYrad = m_Yrad;
			Easing(&m_YradR, m_YradRT, 0.985f, EasingType::OutExpo);
			Easing(&m_Yrad, m_YradR, 0.985f, EasingType::OutExpo);

			Easing(&m_ZradR, GetRandf(30) + (m_Yrad - PrevYrad) * 50.0f, 0.95f, EasingType::OutExpo);
			Easing(&m_Zrad, m_ZradR, 0.95f, EasingType::OutExpo);

			SetMat(m_Pos,
				Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_Zrad + 20.0f * m_SideSpeedPer)) *
				Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30.0f * m_flontSpeedPer)) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(m_Yrad)));

		}

		const bool TeamHelicopterObj::CheckAmmoHit(PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* EndPos) noexcept {
			//auto* SE = SoundPool::Instance();
			//とりあえず当たったかどうか探す
			if (!RefreshCol(StartPos, *EndPos, 10.0f * Scale3DRate)) {
				return false;
			}
			auto colres = GetColLine(StartPos, *EndPos, 0);
			if (colres.HitFlag == TRUE) {
				*EndPos = colres.HitPosition;
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					EffectSingleton::Instance()->SetOnce_Any(Effect::ef_gndsmoke, *EndPos, colres.Normal, 2.0f);
				}
				if (AttackID >= 0) {
					this->m_IsHit = true;
					return true;
				}
			}
			return false;
		}
		void TeamHelicopterObj::SetAction(HelicopterMove Move) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();

			m_HelicopterMove = Move;
			m_Timer = 0.0f;
			switch (this->m_HelicopterMove) {
			case HelicopterMove::Random:
				m_PrevPos = m_NowPos;
				while (true) {
					m_TargetPos = Vector3DX::vget(GetRandf(16.f), 0.0f, GetRandf(16.f)) * Scale3DRate;
					Vector3DX Vec = PlayerMngr->GetHelicopter()->GetMove().GetPos() - m_TargetPos; Vec.y = 0.f;
					if (Vec.magnitude() > (10.f * Scale3DRate)) {
						break;
					}
				}
				break;
			case HelicopterMove::Rappelling:
				m_PrevPos = m_NowPos;
				{
					auto* BackGroundParts = BackGround::BackGroundControl::Instance();
					auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
					while (true) {
						m_TargetPos = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();
						if (std::abs(m_TargetPos.x) < 16.f * Scale3DRate && std::abs(m_TargetPos.z) < 16.f * Scale3DRate) {
							Vector3DX Vec = ViewChara->GetMove().GetPos() - m_TargetPos; Vec.y = 0.f;
							if (Vec.sqrMagnitude() > (8.f * Scale3DRate) * (8.f * Scale3DRate)) {
								break;
							}
						}
					}
					m_TargetPos.y = 0.f;
				}
				break;
			case HelicopterMove::Intercept:
				m_PrevPos = m_NowPos;
				m_TargetPos = Matrix3x3DX::Vtrans(Vector3DX::vget(0.0f, 0.0f, 15.0f + GetRandf(10.0f)) * Scale3DRate, Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180.0f))));
				break;
			default:
				break;
			}
			auto Vec = (m_TargetPos - m_PrevPos);
			if (Vec.magnitude() > 0.0f) {
				m_YradRT = rad2deg(std::atan2(-Vec.x, -Vec.z));
				if ((m_YradRT - m_Yrad) > deg2rad(180)) {
					m_YradRT -= deg2rad(360);
				}
				else if ((m_YradRT - m_Yrad) < -deg2rad(180)) {
					m_YradRT += deg2rad(360);
				}
			}
		}
		void TeamHelicopterObj::FirstUpdate(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			m_RopePer = std::clamp(m_RopePer + (m_Rope ? 10.0f : -5.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 10.0f);
			Easing(&m_OpenPer, (m_Open) ? 1.0f : 0.0f, 0.95f, EasingType::OutExpo);

			SetAnimLoop(static_cast<int>(0), 1.0f);
			SetObj().SetAnim(static_cast<int>(0)).SetPer(1.0f);
			SetAnimLoop(static_cast<int>(1), 1.0f);
			SetObj().SetAnim(static_cast<int>(1)).SetPer(m_OpenPer);
			SetObj().UpdateAnimAll();

			{
				auto& chara = PlayerMngr->GetPlayer(m_TargetPlayerID)->GetChara();

				Vector3DX Vec = Matrix3x3DX::Vtrans((chara->GetFrameWorldMat(Charas::CharaFrame::Upper).pos() - SetObj().GetFramePosition(m_GunRot.GetFrameID())).normalized(), GetMove().GetMat().inverse());

				float YVec = 0.f;
				float XVec = 0.f;
				YVec = std::clamp(std::atan2f(-Vec.x, -Vec.z), deg2rad(30), deg2rad(150));
				XVec = std::clamp(std::atan2f(Vec.y, std::hypotf(Vec.x, Vec.z)), deg2rad(-70), deg2rad(0));
				this->m_CanShot = (deg2rad(30) < YVec && YVec < deg2rad(150)) && (deg2rad(-70) < XVec && XVec < deg2rad(0));
				if (this->m_HelicopterMove != HelicopterMove::Intercept) {
					XVec = 0.0f;
					YVec = 0.0f;
				}
				Easing(&m_GunRotR, Vector3DX::vget(XVec, YVec, 0.0f), 0.95f, EasingType::OutExpo);

				SetObj().SetFrameLocalMatrix(m_GunRot.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::up(), m_GunRotR.y) * m_GunRot.GetFrameLocalPosition());
				SetObj().SetFrameLocalMatrix(m_GunAngle.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), m_GunRotR.x) * m_GunAngle.GetFrameLocalPosition());
			}
			if (this->m_GunAmmo == 0) {
				this->m_ReloadTimer = std::max(this->m_ReloadTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
				if (this->m_ReloadTimer <= 0.0f) {
					this->m_GunAmmo = 5;
				}
			}
			else {
				this->m_ReloadTimer = 2.0f;
			}
			if (this->m_ShotTimer == 0.0f) {
				if (this->m_CanShot && (this->m_GunAmmo > 0) && (this->m_HelicopterMove == HelicopterMove::Intercept)) {
					this->m_ShotTimer = 0.2f;
					Vector3DX MuzzlePos = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::GunAngle))).pos();
					Vector3DX MuzzleVec = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::GunAngle))).zvec2();
					MuzzlePos += MuzzleVec * (1.0f * Scale3DRate);

					auto& AmmoData = Objects::AmmoDataManager::Instance()->Get(this->m_AmmoSpecID);

					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Shot1))->Play3D(GetMove().GetPos(), 100.0f * Scale3DRate);													//サウンド
					auto* OptionParts = OptionManager::Instance();
					if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_fire2, MuzzlePos, MuzzleVec, AmmoData->GetCaliber() * 10.0f * Scale3DRate, 2.0f);	//銃発砲エフェクトのセット
					}
					Objects::AmmoPool::Instance()->Put(&AmmoData, MuzzlePos, MuzzleVec, this->m_MyPlayerID);
					--this->m_GunAmmo;
				}
			}
			else {
				this->m_ShotTimer = std::max(this->m_ShotTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
			}

			if (this->m_RocketGunAmmo == 0) {
				this->m_RocketReloadTimer = std::max(this->m_RocketReloadTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
				if (this->m_RocketReloadTimer <= 0.0f) {
					this->m_RocketGunAmmo = 20;
				}
			}
			else {
				this->m_RocketReloadTimer = 5.0f;
			}

			m_Timer += DXLib_refParts->GetDeltaTime();

			switch (this->m_HelicopterMove) {
			case HelicopterMove::Random:
				if (this->m_IsHit) {
					this->m_IsHit = false;
				}
				else {
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (!chara->IsAlive()) { continue; }
						if (chara->GetCanLookByPlayerTimer() < 5.f) { continue; }
						m_TargetPlayerID = static_cast<PlayerID>(loop);
						SetAction(HelicopterMove::Intercept);
						break;
					}
				}
				if (m_Timer > 10.0f) {
					SetAction(HelicopterMove::Random);
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.0f, 0.0f, 1.0f));
				break;
			case HelicopterMove::Rappelling:
				if (m_Timer <= 8.0f) {
					if (this->m_IsHit) {
						SetAction(HelicopterMove::Intercept);
						m_Rope = false;
						m_Open = false;
					}
				}
				else if (m_Timer <= 10.f) {
					m_Open = true;
				}
				else if (m_Timer <= 15.f) {
					m_Rope = true;
				}
				else {
					bool IsRappellingAny = false;
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (chara->GetIsRappelling()) {
							IsRappellingAny = true;
							break;
						}
					}
					if (!IsRappellingAny) {
						//緊急退避
						if (this->m_IsHit) {
							SetAction(HelicopterMove::Intercept);
							m_Rope = false;
							m_Open = false;
						}
						else {
							SetAction(HelicopterMove::Random);
							m_Rope = false;
							m_Open = false;
						}
					}
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.0f, 0.0f, 1.0f));
				break;
			case HelicopterMove::Intercept:
				if (m_Timer > 8.0f) {
					auto& chara = PlayerMngr->GetPlayer(m_TargetPlayerID)->GetChara();
					Vector3DX Vec = (chara->GetFrameWorldMat(Charas::CharaFrame::Upper).pos() - SetObj().GetFramePosition(m_GunRot.GetFrameID())).normalized();
					if (Vec.magnitude() > 0.0f) {
						m_YradRT = rad2deg(std::atan2(-Vec.x, -Vec.z));
						m_YradRT += -90;
						if ((m_YradRT - m_Yrad) > deg2rad(180)) {
							m_YradRT -= deg2rad(360);
						}
						else if ((m_YradRT - m_Yrad) < -deg2rad(180)) {
							m_YradRT += deg2rad(360);
						}

					}
					if (!chara->IsAlive()) {
						SetAction(HelicopterMove::Random);
						this->m_IsHit = false;
					}
				}
				if (m_Timer > 30.0f) {
					SetAction(HelicopterMove::Random);
					this->m_IsHit = false;
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.0f, 0.0f, 1.0f));
				break;
			default:
				break;
			}


			if (this->m_HelicopterMove == HelicopterMove::Rappelling) {
				Easing(&RopeVec, GetMove().GetMat().yvec(), 0.9f, EasingType::OutExpo);
			}
			else {
				Easing(&RopeVec, Vector3DX::up(), 0.9f, EasingType::OutExpo);
			}


			auto PrevPos = m_Pos;

			Easing(&m_PosR, m_NowPos + (Vector3DX::vget(0.0f, -20.0f, 0.0f) + Vector3DX::vget(GetRandf(1.0f), GetRandf(1.0f), GetRandf(1.0f))) * Scale3DRate, 0.95f, EasingType::OutExpo);
			Easing(&m_Pos, m_PosR, 0.95f, EasingType::OutExpo);

			auto Vec = Matrix3x3DX::Vtrans((m_Pos - PrevPos), GetMove().GetMat().inverse()) / (60.0f * DXLib_refParts->GetDeltaTime());
			Easing(&m_flontSpeedPer, std::clamp(-Vec.z, -1.0f, 1.0f), 0.975f, EasingType::OutExpo);
			Easing(&m_SideSpeedPer, std::clamp(-Vec.x, -1.0f, 1.0f), 0.975f, EasingType::OutExpo);

			auto PrevYrad = m_Yrad;
			Easing(&m_YradR, m_YradRT, 0.985f, EasingType::OutExpo);
			Easing(&m_Yrad, m_YradR, 0.985f, EasingType::OutExpo);

			Easing(&m_ZradR, GetRandf(30) + (m_Yrad - PrevYrad) * 50.0f, 0.95f, EasingType::OutExpo);
			Easing(&m_Zrad, m_ZradR, 0.95f, EasingType::OutExpo);

			SetMat(m_Pos,
				Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_Zrad + 20.0f * m_SideSpeedPer)) *
				Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30.0f * m_flontSpeedPer)) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(m_Yrad)));

		}
	}
}
