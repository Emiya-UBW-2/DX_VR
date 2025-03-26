#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void CharacterClass::LoadCharaGun(const std::string& FolderName, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<GunClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			m_GunPtrControl.SetGunPtr(Sel, Ptr);
		}
		void CharacterClass::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto& p = PlayerMngr->GetPlayer(ID);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";

			std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			p->SetChara(Ptr);
			auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
			c->SetPlayerID(ID);
			p->SetAI(std::make_shared<AIControl>());
			//p->GetAI()->Init();
		}
		//
		void			CharacterClass::Shot_Start() noexcept {
			if (GetMyPlayerID() == 0) {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				PlayerMngr->GetPlayer(GetMyPlayerID())->AddShot(m_GunPtrControl.GetGunPtrNow()->GetPelletNum());
			}
			m_GunPtrControl.GetGunPtrNow()->SetBullet();
			//
			float Power = 0.0001f * m_GunPtrControl.GetGunPtrNow()->GetRecoilPower();
			if (m_KeyControl.GetIsSquat()) {
				Power *= 0.5f;
			}
			if (m_ArmBreak) {
				Power *= 1.5f;
			}
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power, 0.f);
			//
			if (GetMyPlayerID() == 0) {
				Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
			}
			//エフェクト
			auto mat = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle);
			switch (m_GunPtrControl.GetGunPtrNow()->GetGunShootSound()) {
			case GunShootSound::Normal:
				EffectControl::SetOnce((int)Sceneclass::Effect::ef_fire2, mat.pos(), mat.zvec() * -1.f, 0.5f);
				EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_fire2, 2.f);
				break;
			case GunShootSound::Suppressor:
				EffectControl::SetOnce((int)Sceneclass::Effect::ef_fire2, mat.pos(), mat.zvec() * -1.f, 0.25f);
				EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_fire2, 2.f);
				break;
			default:
				break;
			}
		}
		void			CharacterClass::Reload_Start() noexcept {
			m_GunPtrControl.SwapMagazine();
			if (GetMyPlayerID() != 0) {
				auto* SE = SoundPool::Instance();
				if (GetRand(100) < 50) {
					SE->Get(SoundType::SE, (int)SoundEnum::Man_reload)->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
				}
				else {
					SE->Get(SoundType::SE, (int)SoundEnum::Man_takecover)->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
				}
			}
		}
		//
		const Matrix3x3DX CharacterClass::GetEyeRotation(void) const noexcept {
			Matrix3x3DX tmpUpperMatrix;
			auto* OptionParts = OptionManager::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				tmpUpperMatrix = m_WalkSwingControl.GetWalkSwingMat();
			}
			tmpUpperMatrix *= m_KeyControl.GetEyeLocalDir() * this->GetMove().GetMat();
			return tmpUpperMatrix;
		}
		const Vector3DX CharacterClass::GetEyePosition(void) const noexcept {
			auto* OptionParts = OptionManager::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));

			auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f;
			if (HeadBobbing) {
				EyePosition += m_EyeSwingControl.GetEyeSwingPos();
			}
			if (m_GunPtrControl.GetGunPtrNow()) {
				EyePosition = Lerp<Vector3DX>(EyePosition, m_GunPtrControl.GetGunPtrNow()->GetEyePos(), this->m_ADSPer);
			}
			return EyePosition;
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			if (GetMyPlayerID() == value.DamageID) {
				auto PrevLive = IsAlive();
				this->m_HP.Sub(value.Damage);
				this->m_AP.Sub(value.ArmerDamage);
				if (value.ShotID == 0) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(value.ShotID)->AddHit(1);
					}
				}
				if (IsAlive()) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get(SoundType::SE, (int)SoundEnum::Man_Hurt1 + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						//SE->Get(SoundType::SE, (int)SoundEnum::Man_Hurt1 + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
						//
						if (!PrevLive) {
						}
					}
				}
				else if (PrevLive) {
					for (int i = 0; i < 6; i++) {
						SE->Get(SoundType::SE, (int)SoundEnum::Man_Hurt1 + i)->StopAll();
					}
					SE->Get(SoundType::SE, (int)SoundEnum::Man_contact)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_openfire)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_reload)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_takecover)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_breathend)->StopAll();

					SE->Get(SoundType::SE, (int)SoundEnum::Man_Death1 + GetRand(8 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					if (value.ShotID == 0) {
						PlayerMngr->GetPlayer(value.ShotID)->AddScore(100);
						PlayerMngr->GetPlayer(value.ShotID)->AddKill(1);
					}
				}
				return true;
			}
			return false;
		}
		const bool		CharacterClass::CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, this->GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//被弾処理
			auto* HitPtr = m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(AttackID)->GetChara();
				//部位ダメージ演算
				if (CheckBodyParts) {
					switch (HitPtr->GetColType()) {
					case HitType::Head:
						if (GetMyPlayerID() == 0) {
							*Damage = *Damage;
							*ArmerDamage = 0;
						}
						else {
							*Damage = *Damage * 100 / 10;
							*ArmerDamage = 0;
						}
						break;
					case HitType::Body:
						if (m_AP.IsNotZero()) {
							*ArmerDamage = std::clamp((ArmerPoint)*Damage, (ArmerPoint)0, m_AP.GetPoint());
							*Damage -= (HitPoint)*ArmerDamage;
						}
						else {
							*ArmerDamage = 0;
							*Damage = *Damage;
						}
						break;
					case HitType::Arm:
						*Damage = *Damage * 5 / 10;
						*ArmerDamage = 0;
						break;
					case HitType::Leg:
						*Damage = *Damage * 7 / 10;
						*ArmerDamage = 0;
						break;
					default:
						break;
					}
					*Damage = std::min(*Damage, m_HP.GetMax());
					*ArmerDamage = std::min(*ArmerDamage, m_AP.GetMax());
				}
				//ダメージ登録
				{
					//auto v1 = GetEyeRotationCache().zvec() * -1.f;
					//auto v2 = (Chara->GetMove().GetPos() - this->GetMove().GetPos()).normalized(); v2.y = (0);
					//atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2))
					m_Damage.Add(AttackID, GetMyPlayerID(), *Damage, *ArmerDamage, Vector3DX::up());
				}
				//SE
				if (AttackID == 0) {
					if (*Damage > 0) {
						SE->Get(SoundType::SE, (int)SoundEnum::Hit)->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, (int)SoundEnum::HitGuard)->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 128);
					}
				}
				if (GetMyPlayerID() == 0) {
					if (*Damage > 0) {
						SE->Get(SoundType::SE, (int)SoundEnum::HitMe)->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, (int)SoundEnum::HitGuard)->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (*Damage > 0) {
					EffectControl::SetOnce((int)(int)Sceneclass::Effect::ef_hitblood, *pEndPos, Vector3DX::forward(), Scale3DRate);
					EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce((int)Sceneclass::Effect::ef_gndsmoke, *pEndPos, (Chara->GetMove().GetPos() - this->GetMove().GetPos()).normalized(), 0.25f * Scale3DRate);
				}
				//ヒットモーション
				{
					m_HitReactionControl.SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((*pEndPos - StartPos).normalized(), Vector3DX::up()) * -1.f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						m_KeyControl.SetIsSquat(true);
					}
					if (HitPtr->GetColType() == HitType::Arm && !m_ArmBreak) {
						m_ArmBreak = true;
					}
				}
				//todo : ヒットした部分に近い頂点を赤くする
				return true;
			}
			return false;
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//auto* OptionParts = OptionManager::Instance();

			Easing(&this->m_ADSPer, (this->m_GunReadySeq == EnumGunReadySeq::ADS) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);//
			if (this->m_GunReadySeq == EnumGunReadySeq::ADS) {
				this->m_GunReadySeq = EnumGunReadySeq::Aim;
			}
			if (m_GunPtrControl.GetGunPtrNow()) {
				m_GunPtrControl.GetGunPtrNow()->SetShotSwitchOff();
			}
			//リコイルの演算
			if (m_GunPtrControl.GetGunPtrNow()) {
				if (this->m_RecoilRadAdd.y < 0.f) {
					Easing(&this->m_RecoilRadAdd, Vector3DX::vget(0.f, 0.01f, 0.f), m_GunPtrControl.GetGunPtrNow()->GetRecoilReturn(), EasingType::OutExpo);
				}
				else {
					Easing(&this->m_RecoilRadAdd, Vector3DX::zero(), 0.7f, EasingType::OutExpo);
				}
			}
			//Shot
			bool Press_Shot = m_KeyControl.GetInputControl().GetPADSPress(Controls::PADS::SHOT) && !m_IsChange;
			//Reload
			bool Press_Reload = m_KeyControl.GetInputControl().GetPADSPress(Controls::PADS::RELOAD) && !m_IsChange;
			if (m_GunPtrControl.GetGunPtrNow() && m_GunPtrControl.GetGunPtrNow()->GetAmmoNumTotal() == 0 && m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) {
				Press_Reload |= Press_Shot;				//打ち切りで自動リロード
			}
			Press_Reload &= m_GunPtrControl.IsNeedReload();//リロードの必要がある場合のみリロード
			//ADS
			bool Press_Aim = m_KeyControl.GetInputControl().GetPADSPress(Controls::PADS::AIM) && !m_IsChange;
			if (m_GunPtrControl.GetGunPtrNow()) {
				switch (m_GunPtrControl.GetGunPtrNow()->GetShotType()) {
				case SHOTTYPE::BOLT:
					if (m_GunPtrControl.GetGunPtrNow()->GetGunAnime() > GunAnimeID::Shot) {//撃てない状態
						Press_Aim = false;
					}
					break;
				default:
					break;
				}
			}
			Press_Aim &= !m_IsStuckGun;//スタックしていない時だけADSできる
			//ターゲットが見える場合かまえっぱなし
			if (!m_IsChanging) {
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!c->IsAlive()) { continue; }
					if (!c->CanLookTarget) { continue; }
					if (this->m_GunReadySeq == EnumGunReadySeq::LowReady) {
						this->m_GunReadySeq = EnumGunReadySeq::Aim;
					}
					break;
				}
			}
			//
			auto PrevAction = m_CharaAction;
			switch (m_CharaAction) {
			case CharaActionID::Ready:
				if (m_ActionFirstFrame && m_GunPtrControl.GetGunPtrNow()) {
					m_GunPtrControl.GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
				}
				//射撃
				if (Press_Shot) {
					if (this->m_GunReadySeq == EnumGunReadySeq::LowReady) {
						this->m_GunReadySeq = EnumGunReadySeq::Aim;
					}
					if ((this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f) &&
						m_GunPtrControl.GetGunPtrNow() &&
						(m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) &&
						m_GunPtrControl.GetGunPtrNow()->GetInChamber()
						) {
						Shot_Start();
					}
				}
				//リロード/マガジンチェック
				if (Press_Reload) {
					m_CharaAction = CharaActionID::Reload;
				}
				//姿勢指定
				if (Press_Aim) {
					this->m_GunReadySeq = EnumGunReadySeq::ADS;
				}
				//
				if (m_GunPtrControl.GetGunPtrNow() && m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) {
					bool ischeck = true;
					switch (m_GunPtrControl.GetGunPtrNow()->GetShotType()) {
					case SHOTTYPE::FULL:
						ischeck = m_GunPtrControl.GetGunPtrNow()->GetInChamber() || m_GunPtrControl.GetGunPtrNow()->GetIsMagEmpty();
						break;
					case SHOTTYPE::SEMI:
					case SHOTTYPE::BOLT:
						ischeck = !Press_Shot;
						break;
					default:
						break;
					}

					if (m_GunPtrControl.GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::Shot).TimeEnd() && ischeck) {
						if (!m_GunPtrControl.GetGunPtrNow()->GetIsMagEmpty()) {
							switch (m_GunPtrControl.GetGunPtrNow()->GetShotType()) {
							case SHOTTYPE::FULL:
							case SHOTTYPE::SEMI:
								m_GunPtrControl.GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
								break;
							case SHOTTYPE::BOLT:
								m_CharaAction = CharaActionID::Cocking;
								break;
							default:
								break;
							}
						}
						else {
							m_GunPtrControl.GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
					}
				}
				break;
			case CharaActionID::Cocking:
				if (m_ActionFirstFrame && m_GunPtrControl.GetGunPtrNow()) {
					m_GunPtrControl.GetGunPtrNow()->SetGunAnime(GunAnimeID::Cocking);
				}
				//
				if (m_GunPtrControl.GetGunPtrNow() && m_GunPtrControl.GetGunPtrNow()->GetGunAnime() != GunAnimeID::Cocking) {
					m_CharaAction = CharaActionID::Ready;
				}
				//姿勢指定
				this->m_GunReadySeq = EnumGunReadySeq::Aim;
				break;
			case CharaActionID::Reload:
				if (m_ActionFirstFrame) {
					Reload_Start();
				}
				else {
					if ((m_GunPtrControl.GetGunPtrNow()->GetReloadType() == RELOADTYPE::AMMO) && (m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne)) {
						if (Press_Shot) {
							m_GunPtrControl.GetGunPtrNow()->SetCancel(true);
						}
					}
					if (m_IsChange) {
						m_GunPtrControl.GetGunPtrNow()->SetCancel(true);
						m_CharaAction = CharaActionID::Ready;
					}
					if (!m_GunPtrControl.GetGunPtrNow()->GetReloading()) {
						if (m_GunPtrControl.GetGunPtrNow()->GetInChamber()) {
							m_CharaAction = CharaActionID::Ready;
						}
						else {
							if (!m_GunPtrControl.GetGunPtrNow()->GetIsMagEmpty()) {
								m_CharaAction = CharaActionID::Cocking;
							}
							else {
								m_CharaAction = CharaActionID::Ready;
							}
						}
					}
				}
				//姿勢指定
				this->m_GunReadySeq = EnumGunReadySeq::Aim;
				break;
			case CharaActionID::Watch:
				if (m_ActionFirstFrame) {
					m_GunPtrControl.GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
				}
				//
				if (m_GunPtrControl.GetGunPtrNow()->GetGunAnime() != GunAnimeID::Watch) {
					m_CharaAction = CharaActionID::Ready;
				}
				//姿勢指定
				this->m_GunReadySeq = EnumGunReadySeq::Aim;
				break;
			default:
				break;
			}
			//
			m_ActionFirstFrame = (PrevAction != m_CharaAction);

			//グレネード投てき
			auto* SE = SoundPool::Instance();
			{
				if (m_KeyControl.GetThrowKey().trigger()) {
					SE->Get(SoundType::SE, (int)SoundEnum::Pin)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
				}
				if (m_KeyControl.GetThrowKey().release_trigger()) {
					SE->Get(SoundType::SE, (int)SoundEnum::Throw)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
					m_Grenade.SetFall(
					GetFrameWorldMat(CharaFrame::RightHandJoint).pos(),
					GetEyeRotationCache(),
					(GetEyeRotationCache().zvec() * -1.f).normalized() * (Scale3DRate * 15.f / 60.f), 3.5f, SoundEnum::FallGrenade, true);
				}
			}
			for (auto& g : m_Grenade.m_Ptr) {
				if (g->PopGrenadeBombSwitch()) {
					EffectControl::SetOnce((int)Sceneclass::Effect::ef_greexp, g->GetMove().GetPos(), Vector3DX::forward(), 0.5f * Scale3DRate);
					EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_greexp, 2.f);
					SE->Get(SoundType::SE, (int)SoundEnum::Explosion)->Play3D(g->GetMove().GetPos(), Scale3DRate * 25.f);

					auto* ObjMngr = ObjectManager::Instance();
					auto* BackGround = BackGround::BackGroundClass::Instance();

					for (int i = 0, max = m_ChamberAmmoData->GetPellet(); i < max; i++) {
						auto LastAmmo = std::make_shared<AmmoClass>();
						ObjMngr->AddObject(LastAmmo);
						LastAmmo->Init();
						auto mat =
							Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) *
							Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						LastAmmo->Put(this->m_ChamberAmmoData, g->GetMove().GetPos() + mat.zvec() * (0.5f * Scale3DRate) + Vector3DX::up() * (0.5f * Scale3DRate), mat.zvec(), GetMyPlayerID());
					}

					//破壊
					int								xput = 6;
					int								yput = 8;
					int								zput = 6;
					Vector3DX						PutPos = g->GetMove().GetPos();
					auto Put = BackGround->GetPoint(PutPos);
					for (int xp = 0; xp < xput; xp++) {
						for (int yp = 0; yp < yput; yp++) {
							for (int zp = 0; zp < zput; zp++) {
								auto& cell = BackGround->GetCellBuf((Put.x + xp - xput / 2), (Put.y + yp), (Put.z + zp - zput / 2));
								if (cell.m_Cell==1) {
									continue;
								}
								BackGround->SetBlick((Put.x + xp - xput / 2), (Put.y + yp), (Put.z + zp - zput / 2), FPS_n2::BackGround::s_EmptyBlick);
							}
						}
					}
				}
			}
			if (m_GunPtrControl.GetGunPtrNow()) {
				m_SightChange.Update((this->m_GunReadySeq == EnumGunReadySeq::ADS) && m_KeyControl.GetInputControl().GetPADSPress(Controls::PADS::JUMP));
				if (m_SightChange.trigger()) {
					m_GunPtrControl.GetGunPtrNow()->ChangeSightSel();
				}
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Update(this->m_GunReadySeq == EnumGunReadySeq::ADS, 0.2f, 0.2f, 0.9f);
			if (m_GunPtrControl.GetGunPtrNow()) {
				this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Update(m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
				this->m_Arm[(int)EnumGunAnimType::ReloadStart].Update(m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart, 0.2f, 0.2f);
				{
					bool IsReload = false;
					switch (m_GunPtrControl.GetGunPtrNow()->GetReloadType()) {
					case RELOADTYPE::MAG:
					{
						IsReload = (m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne);
						this->m_Arm[(int)EnumGunAnimType::Reload].Update(IsReload, 0.1f, 0.2f, 0.8f);
					}
					break;
					case RELOADTYPE::AMMO:
					{
						IsReload = (m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne) && (m_GunPtrControl.GetGunPtrNow()->GetObj().SetAnim((int)m_GunPtrControl.GetGunPtrNow()->GetGunAnime()).GetTimePer() < 0.5f);
						this->m_Arm[(int)EnumGunAnimType::Reload].Update(IsReload, 0.1f, 0.2f, 0.9f);
					}
					break;
					default:
						break;
					}
				}
				this->m_Arm[(int)EnumGunAnimType::ReloadEnd].Update(m_GunPtrControl.GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadEnd, 0.1f, 0.2f, 0.9f);
				this->m_Arm[(int)EnumGunAnimType::Ready].Update(this->m_GunReadySeq == EnumGunReadySeq::LowReady, 0.1f, 0.2f, 0.87f);
				this->m_Arm[(int)EnumGunAnimType::Watch].Update(GetCharaAction() == CharaActionID::Watch, 0.1f, 0.1f);
			}
			//
			m_KeyControl.SetIsSwitchRight(m_KeyControl.GetLeanRate() <= 0 || m_GunPtrControl.GetGunPtrNow()->GetReloading());
			//
			m_KeyControl.Update();
		}
		//音指示
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (!m_KeyControl.GetIsStop()) {
				auto Time = this->GetObj().SetAnim((int)m_KeyControl.GetBottomAnimSelectNow()).GetTime();
				//L
				if ((9.f < Time && Time < 10.f)) {
					if (this->m_CharaSound != 1) {
						this->m_CharaSound = 1;
						if ((CameraParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {

							SE->Get(SoundType::SE, (int)SoundEnum::RunFoot)->Play3D(GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 5.f);
							if (this->m_GunReadySeq != EnumGunReadySeq::ADS) {
								if (GetMyPlayerID() == 0) {
									auto* OptionParts = OptionManager::Instance();
									bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
									if (HeadBobbing) {
										//Camera3D::Instance()->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
				}
				//R
				if ((27.f < Time && Time < 28.f)) {
					if (this->m_CharaSound != 3) {
						this->m_CharaSound = 3;
						if ((CameraParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {

							SE->Get(SoundType::SE, (int)SoundEnum::RunFoot)->Play3D(GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 5.f);
							if (this->m_GunReadySeq != EnumGunReadySeq::ADS) {
								if (GetMyPlayerID() == 0) {
									auto* OptionParts = OptionManager::Instance();
									bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
									if (HeadBobbing) {
										//Camera3D::Instance()->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
				}
			}
			//しゃがみ音
			if (m_KeyControl.GetSquatSwitch()) {
				SE->Get(SoundType::SE, (int)SoundEnum::StandupFoot)->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
			//リーン音
			if (m_KeyControl.GetLeanSwitch()) {
				SE->Get(SoundType::SE, (int)SoundEnum::StandupFoot)->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			//
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper),
				(Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper)) * Matrix3x3DX::RotAxis(Vector3DX::right(), -m_KeyControl.GetRad().x / 2.f) * m_KeyControl.GetCharaLocalDir()).Get44DX()
				* GetFrameBaseLocalMat((int)CharaFrame::Upper));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper2));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper2),
				(Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper2)) * Matrix3x3DX::RotAxis(Vector3DX::right(), m_KeyControl.GetRad().x / 2.f) * m_HitReactionControl.GetHitReactionMat()).Get44DX()
				* GetFrameBaseLocalMat((int)CharaFrame::Upper2));
			m_HitReactionControl.Update();
			//上半身演算
			if (m_GunPtrControl.GetGunPtrNow()) {
				m_GunPtrControl.GetGunPtrNow()->UpdateGunAnims();
			}
			//上半身アニメ演算
			m_KeyControl.SetCharaAnimeBufID(CharaAnimeID::Upper_Ready) = 1.f;
			m_KeyControl.SetCharaAnimeBufID(CharaAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop((int)m_KeyControl.GetBottomTurnAnimSel(), 0.5f);
			ObjectBaseClass::SetAnimLoop((int)CharaAnimeID::Bottom_Stand_Run, m_KeyControl.GetSpeedPer() / 2.f);
			ObjectBaseClass::SetAnimLoop((int)m_KeyControl.GetBottomWalkAnimSel(), m_KeyControl.GetVecFront());
			ObjectBaseClass::SetAnimLoop((int)m_KeyControl.GetBottomLeftStepAnimSel(), m_KeyControl.GetVecLeft());
			ObjectBaseClass::SetAnimLoop((int)m_KeyControl.GetBottomWalkBackAnimSel(), m_KeyControl.GetVecRear());
			ObjectBaseClass::SetAnimLoop((int)m_KeyControl.GetBottomRightStepAnimSel(), m_KeyControl.GetVecRight());
			//アニメ反映
			for (int i = 0; i < GetObj().GetAnimNum(); i++) {
				this->GetObj().SetAnim(i).SetPer(m_KeyControl.GetCharaAnimeBufID((CharaAnimeID)i));
			}
			GetObj().UpdateAnimAll();
			//移動の際の視点制御
			m_EyeSwingControl.Update(m_CharaRotationCache, std::clamp(m_KeyControl.GetVec().magnitude() / 0.65f, 0.f, 0.85f / 0.65f), 5.f);
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			//vector
			Vector3DX PosBuf = this->GetMove().GetPos();
			Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
			bool IsHitGround = BackGround->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.f * Scale3DRate), &EndPos);
			if (IsHitGround) {
				auto yPos = PosBuf.y;
				Easing(&yPos, EndPos.y - (0.12f * Scale3DRate), 0.6f, EasingType::OutExpo);
				PosBuf.y = (yPos);

				Vector3DX vec = m_KeyControl.GetVec(); vec.y = 0.f;
				this->SetMove().SetVec(vec);
			}
			else {
				float Y = this->GetMove().GetVec().y;
				Vector3DX vec = m_KeyControl.GetVec(); vec.y = (Y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
				this->SetMove().SetVec(vec);
			}
			PosBuf += this->GetMove().GetVec();
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (1.6f * Scale3DRate), 0.6f * Scale3DRate);
			//ほかプレイヤーとの判定
			{
				float Radius = 2.f * 0.5f * Scale3DRate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!c->IsAlive()) { continue; }
					//自分が当たったら押し戻す
					Vector3DX Vec = (c->GetMove().GetPos() - this->GetMove().GetPos()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						PosBuf = PosBuf + Vec.normalized() * (Len - Radius);
					}
				}
			}
			//座標オーバーライド
			if (this->m_MoveOverRideFlag) {
				this->m_MoveOverRideFlag = false;
				this->m_move = this->m_OverRideInfo;
			}
			this->SetMove().SetPos(PosBuf);
			this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), m_KeyControl.GetYRadBottom()));
			this->SetMove().Update(0.5f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			m_EyeRotationCache = GetEyeRotation();
			m_EyePositionCache = GetEyePosition();
			m_CharaRotationCache = GetCharaDir();
			//スリング場所探索
			int GunSel = 1;
			if (m_GunPtrControl.GetGunPtr(GunSel) && m_GunPtrControl.m_SlingPer[GunSel] < 1.f) {
				m_GunPtrControl.SetGunMountMat(GunSel,
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-90)) *
					Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper)) *
					m_CharaRotationCache.inverse(),
					GetFrameWorldMat(CharaFrame::Upper).pos() +
					GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) +
					GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate)
				);
			}
			GunSel = 1 - GunSel;
			if (m_GunPtrControl.GetGunPtr(GunSel) && m_GunPtrControl.m_SlingPer[GunSel] < 1.f) {
				m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
				m_SlingZrad.AddRad(((1.f - m_GunPtrControl.m_SlingPer.at(m_GunPtrControl.GetOtherGunSelect())) + 0.5f * (m_KeyControl.GetRad().y - m_KeyControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());

				m_GunPtrControl.SetGunMountMat(GunSel,
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
					(
						Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
						Matrix3x3DX::RotAxis(Vector3DX::up(), m_KeyControl.GetRad().y) *
						m_CharaRotationCache.inverse()
						),
					GetFrameWorldMat(CharaFrame::Head).pos() +
					GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
					(
						this->GetMove().GetMat().xvec() * sin(m_SlingZrad.GetRad()) +
						this->GetMove().GetMat().yvec() * cos(m_SlingZrad.GetRad())
						) * -(0.5f * Scale3DRate)
				);
			}
			if (m_GunPtrControl.GetGunPtrNow()) {
				//銃ひっこめ
				{
					if (m_StuckGunTimer == 0.f) {
						m_StuckGunTimer = 0.1f;
						auto EndPost = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle).pos();
						if (BackGround->CheckLinetoMap(GetEyePositionCache(), &EndPost)) {//0.03ms
							m_IsStuckGun = true;
						}
						else {
							if (m_IsStuckGun) {
								this->m_GunReadySeq = EnumGunReadySeq::Aim;
							}
							m_IsStuckGun = false;
						}
					}
					else {
						m_StuckGunTimer = std::max(m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					}
					if (m_IsStuckGun) {
						this->m_GunReadySeq = EnumGunReadySeq::LowReady;
					}
				}
				//オートエイム
				m_AutoAimControl.Update(true);
				Easing(&m_AutoAimControl.m_AutoAimPer, (this->m_GunReadySeq == EnumGunReadySeq::ADS && m_AutoAimControl.GetAutoAimActive()) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				//
				if (IsAlive()) {
					//銃の位置を指定するアニメ
					Matrix4x4DX AnimMat;
					for (int i = 0; i < (int)EnumGunAnimType::Max; i++) {
						Matrix4x4DX AnimData;
						if (!m_GunPtrControl.GetGunPtrNow()->GetGunAnimeNow((EnumGunAnimType)i, &AnimData)) { continue; }
						AnimMat = Lerp(AnimMat, AnimData, this->m_Arm[i].Per());
					}
					Matrix3x3DX AnimRot = Matrix3x3DX::Get33DX(AnimMat);
					Vector3DX AnimPos = AnimMat.pos();
					AnimPos.x *= m_KeyControl.GetSwitchPer();
					Vector3DX Post0 = GetFrameWorldMat(CharaFrame::Head).pos() + Matrix3x3DX::Vtrans(AnimPos, m_CharaRotationCache);
					//
					for (int index = 0; index < 2; index++) {
						auto& p = m_GunPtrControl.GetGunPtr(index);
						if (p) {
							m_GunPtrControl.UpdateGun(index);

							Matrix3x3DX tmp_gunmat =
								m_GunPtrControl.GetGunRotMat(index, AnimRot) *
								m_KeyControl.GetGunSwingMat() * m_CharaRotationCache *
								Matrix3x3DX::RotVec2(Lerp(p->GetObj().GetMatrix().yvec(), p->GetEyeYVec(), this->m_ADSPer), p->GetObj().GetMatrix().yvec());



							Vector3DX BasePos = GetEyePositionCache();
							//オートエイム
							if ((index == m_GunPtrControl.GetNowGunSelect()) && this->m_GunReadySeq == EnumGunReadySeq::ADS) {
								for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
									if (i == GetMyPlayerID()) { continue; }
									auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
									if (!c->IsAlive()) { continue; }
									if (!c->CanLookTarget) { continue; }
									int pos = -1;
									float CosMax = -1.f;
									for (const auto& h : c->m_HitBoxControl.GetHitBoxPointList()) {
										auto EndPost = h.GetPos();
										auto ColResGround = BackGround->CheckLinetoMap(BasePos, &EndPost);
										if (ColResGround) { continue; }
										Vector3DX Vec = (h.GetPos() - BasePos);
										//if (Vec.magnitude() >= (Scale3DRate * Range)) { continue; }
										switch (h.GetColType()) {
										case FPS_n2::Sceneclass::HitType::Head:
										case FPS_n2::Sceneclass::HitType::Body:
											break;
										default:
											continue;
											break;
										}
										float Cos = Vector3DX::Dot((p->GetFrameWorldMat_P(GunFrame::Muzzle).zvec() * -1.f), Vec.normalized());
										if (Cos > cos(p->GetAutoAimRadian())) {
											if (CosMax < Cos) {
												CosMax = Cos;
												pos = (int)(&h - &c->m_HitBoxControl.GetHitBoxPointList().front());
											}
										}
									}
									if (pos != -1) {
										m_AutoAimControl.SetAutoAimActive(i, pos);
										break;
									}
								}
							}

							if (index == m_GunPtrControl.GetNowGunSelect()) {
								if (m_AutoAimControl.GetAutoAimActive()) {
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AutoAimControl.GetAutoAimID())->GetChara();
									Easing(&m_AutoAimControl.m_AutoAimVec, (Chara->m_HitBoxControl.GetHitBoxPointList().at(m_AutoAimControl.GetAutoAimPoint()).GetPos() - BasePos).normalized(), 0.8f, EasingType::OutExpo);
								}
								else {
									Easing(&m_AutoAimControl.m_AutoAimVec, tmp_gunmat.zvec() * -1.f, 0.95f, EasingType::OutExpo);
								}
								tmp_gunmat = Lerp(tmp_gunmat, tmp_gunmat * Matrix3x3DX::RotVec2(tmp_gunmat.zvec() * -1.f, m_AutoAimControl.m_AutoAimVec), m_AutoAimControl.m_AutoAimPer);
							}
							p->SetGunMatrix(tmp_gunmat, m_GunPtrControl.GetGunPos(index, Post0));
						}
					}
				}
				else {
					auto Mat = m_RagDollControl.lagframe_.RIGHThand_f.GetFrameWorldPosition().rotation().inverse() * m_RagDollControl.GetRagDoll().GetFrameLocalWorldMatrix(m_RagDollControl.lagframe_.RIGHThand_f.GetFrameID());
					m_GunPtrControl.GetGunPtrNow()->SetGunMatrix(Matrix3x3DX::Get33DX(Mat), Mat.pos());
					if (m_GunPtrControl.GetGunPtrOther()) {
						//仮の画面外指定
						Mat = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(90)).Get44DX() * GetFrameWorldMat(CharaFrame::Upper);
						m_GunPtrControl.GetGunPtrOther()->SetGunMatrix(Matrix3x3DX::Get33DX(Mat), Mat.pos());
					}
				}
				//LeftMag
				{
					auto tmp_gunmat = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos);
					float AnimPer = m_GunPtrControl.GetGunPtrNow()->GetObj().SetAnim((int)m_GunPtrControl.GetGunPtrNow()->GetGunAnime()).GetTimePer();
					float BasePer = 0.f;
					float MaxPer = 0.f;
					switch (m_GunPtrControl.GetGunPtrNow()->GetGunAnime()) {
					case GunAnimeID::ReloadStart_Empty:
						tmp_gunmat = GetFrameWorldMat(CharaFrame::LeftMag);
						break;
					case GunAnimeID::ReloadStart:
						BasePer = 0.3f;
						MaxPer = 0.6f;
						if (AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
							break;
						}

						BasePer = 0.5f;
						MaxPer = 1.f;
						if (AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), GetFrameWorldMat(CharaFrame::LeftMag), Per);
							break;
						}
						break;
					case GunAnimeID::ReloadOne:
					{
						switch (m_GunPtrControl.GetGunPtrNow()->GetReloadType()) {
						case RELOADTYPE::MAG:
						{
							if (AnimPer <= 0.1f) {
								float MissPer = GetRandf(0.025f);
								m_GunPtrControl.GetGunPtrNow()->SetMagLoadSuccess(GetRand(100) < 50);
								m_MagMiss = Matrix4x4DX::Mtrans(m_GunPtrControl.GetGunPtrNow()->GetMove().GetMat().xvec() * (MissPer * Scale3DRate));
								m_MagSuccess = Matrix4x4DX::Mtrans(
									m_GunPtrControl.GetGunPtrNow()->GetMove().GetMat().yvec() * (-0.05f * Scale3DRate) +
									m_GunPtrControl.GetGunPtrNow()->GetMove().GetMat().xvec() * (MissPer / 3.f * Scale3DRate)
								);
							}
							BasePer = 0.f;
							MaxPer = 0.55f;
							if (AnimPer <= MaxPer) {
								//float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
								float Per = this->m_Arm[(int)EnumGunAnimType::Reload].Per();
								if (m_GunPtrControl.GetGunPtrNow()->IsMagLoadSuccess()) {
									tmp_gunmat = Lerp(GetFrameWorldMat(CharaFrame::LeftMag), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
								}
								else {
									tmp_gunmat = Lerp(GetFrameWorldMat(CharaFrame::LeftMag), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss, Per);
								}
								break;
							}
							if (m_GunPtrControl.GetGunPtrNow()->IsMagLoadSuccess()) {
								BasePer = MaxPer;
								MaxPer = 0.75f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), Per);
									break;
								}
							}
							else {
								BasePer = MaxPer;
								MaxPer = 0.75f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss, m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess, Per);
									break;
								}
								BasePer = MaxPer;
								MaxPer = 0.85f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess, m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
									break;
								}
								BasePer = MaxPer;
								MaxPer = 1.0f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), Per);
									break;
								}
							}
						}
						break;
						case RELOADTYPE::AMMO:
						{
							m_GunPtrControl.GetGunPtrNow()->SetMagLoadSuccess(false);
							BasePer = 0.f;
							MaxPer = 0.5f;
							if (AnimPer <= MaxPer) {
								float Per = this->m_Arm[(int)EnumGunAnimType::Reload].Per();
								tmp_gunmat = Lerp(GetFrameWorldMat(CharaFrame::LeftMag), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
								break;
							}
							BasePer = 0.7f;
							MaxPer = 0.9f;
							if (AnimPer <= MaxPer) {
								float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
								tmp_gunmat = Lerp(m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), Per);
								break;
							}
						}
						break;
						default:
							break;
						}
					}
					break;
					default:
						break;
					}
					m_GunPtrControl.GetGunPtrNow()->GetMagazinePtr()->SetHandMatrix(tmp_gunmat);
					if (m_GunPtrControl.GetGunPtrOther()) {
						m_GunPtrControl.GetGunPtrOther()->GetMagazinePtr()->SetHandMatrix(m_GunPtrControl.GetGunPtrOther()->GetFrameWorldMat_P(GunFrame::Magpos));
					}
				}
				//銃表示
				if (m_GunPtrControl.GetGunPtrOther()) {
					m_GunPtrControl.GetGunPtrOther()->SetActiveAll(true);
				}
				//手の位置を制御
				if ((GetMyPlayerID() == 0) || this->CanLookTarget) {
					//銃座標
					Vector3DX GunPos = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandPos).pos();
					Vector3DX Gunyvec = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandYvec).pos() - GunPos;
					Vector3DX Gunzvec = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandZvec).pos() - GunPos;
					IK_RightArm(
						&GetObj(),
						GetFrame(static_cast<int>(CharaFrame::RightArm)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
						GetFrame(static_cast<int>(CharaFrame::RightArm2)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
						GetFrame(static_cast<int>(CharaFrame::RightWrist)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
						GunPos, Gunyvec, Gunzvec);

					{
						bool IsBreak = false;
						switch (m_GunPtrControl.GetGunPtrNow()->GetReloadType()) {
						case RELOADTYPE::MAG:
							switch (m_GunPtrControl.GetGunPtrNow()->GetGunAnime()) {
							case GunAnimeID::ReloadStart_Empty:
								if (m_GunPtrControl.GetGunPtrNow()->GetObj().SetAnim((int)m_GunPtrControl.GetGunPtrNow()->GetGunAnime()).GetTimePer() > 0.5f) {
									if (!m_MagHand) {
										m_GunPtrControl.GetGunPtrNow()->SetMagFall();
									}
									m_MagHand = true;
								}
								else {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::ReloadStart:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadOne:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadEnd:
								if (m_GunPtrControl.GetGunPtrNow()->GetObj().SetAnim((int)m_GunPtrControl.GetGunPtrNow()->GetGunAnime()).GetTimePer() > 0.6f) {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::Base:
							case GunAnimeID::Shot:
								m_MagHand = false;
								if (m_ArmBreak) {
									IsBreak = true;
								}
								break;
							default:
								m_MagHand = false;
								break;
							}
							break;
						case RELOADTYPE::AMMO:
							switch (m_GunPtrControl.GetGunPtrNow()->GetGunAnime()) {
							case GunAnimeID::ReloadStart_Empty:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadStart:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadOne:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadEnd:
								m_MagHand = false;
								break;
							case GunAnimeID::Base:
							case GunAnimeID::Shot:
								m_MagHand = false;
								if (m_ArmBreak) {
									IsBreak = true;
								}
								break;
							default:
								m_MagHand = false;
								break;
							}
							break;
						default:
							break;
						}
						m_MagArm.Update(m_MagHand, 0.1f, 0.1f, 0.7f);

						Vector3DX HandPos = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandPos).pos();
						Vector3DX Handyvec = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandYvec).pos() - HandPos;
						Vector3DX Handzvec = m_GunPtrControl.GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandZvec).pos() - HandPos;

						Vector3DX MagPos = m_GunPtrControl.GetGunPtrNow()->GetMagazinePtr()->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
						Vector3DX Magyvec = m_GunPtrControl.GetGunPtrNow()->GetMagazinePtr()->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - MagPos;
						Vector3DX Magzvec = m_GunPtrControl.GetGunPtrNow()->GetMagazinePtr()->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - MagPos;
						/*
						Vector3DX MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
						Vector3DX Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
						Vector3DX Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;
						//*/
						Vector3DX HandsPos = Lerp(HandPos, MagPos, m_MagArm.Per());
						Vector3DX Handsyvec = Lerp(Handyvec, Magyvec, m_MagArm.Per());
						Vector3DX Handszvec = Lerp(Handzvec, Magzvec, m_MagArm.Per());

						Easing(&m_ArmBreakPer, IsBreak ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
						if (m_ArmBreakPer > 0.01f) {
							m_SlingArmZrad.Update(DXLib_refParts->GetDeltaTime());
							m_SlingArmZrad.AddRad((0.5f * (m_KeyControl.GetRad().y - m_KeyControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
							Matrix3x3DX SlingArmMat =
								(
									Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
									(
										Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingArmZrad.GetRad()) *
										Matrix3x3DX::RotAxis(Vector3DX::up(), m_KeyControl.GetRad().y)
										)
									);
							Vector3DX SlingArmPos =
								GetFrameWorldMat(CharaFrame::Head).pos() +
								GetFrameWorldMat(CharaFrame::Head).xvec() * (0.15f * Scale3DRate) +
								GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.1f * Scale3DRate) +
								(
									this->GetMove().GetMat().xvec() * sin(m_SlingArmZrad.GetRad()) +
									this->GetMove().GetMat().yvec() * cos(m_SlingArmZrad.GetRad())
									) * -(0.5f * Scale3DRate)
								;

							HandPos = SlingArmPos;
							Handyvec = SlingArmMat.yvec();
							Handzvec = SlingArmMat.zvec() * -1.f;

							HandsPos = Lerp(HandsPos, HandPos, m_ArmBreakPer);
							Handsyvec = Lerp(Handsyvec, Handyvec, m_ArmBreakPer);
							Handszvec = Lerp(Handszvec, Handzvec, m_ArmBreakPer);
						}
						if (m_ArmBreak) {
							HandsPos += Vector3DX::vget(GetRandf(1.f * Scale3DRate), GetRandf(1.f * Scale3DRate), GetRandf(1.f * Scale3DRate)) * 0.002f;
						}

						IK_LeftArm(
							&GetObj(),
							GetFrame(static_cast<int>(CharaFrame::LeftArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
							GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
							HandsPos, Handsyvec, Handszvec);
					}
				}
			}
			m_RagDollControl.Update(this->GetObj(), IsAlive());													//ラグドール
			m_HitBoxControl.Update(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			m_WalkSwingControl.Update(GetEyePositionCache() - GetMove().GetPos(), std::clamp(this->GetMove().GetVec().magnitude() / 2.f, 0.f, 0.5f));
		}
		//
		void			CharacterClass::ValueSet(PlayerID pID, CharaTypeID value) noexcept {
			this->m_CharaType = value;
			this->m_HP.Init();
			this->m_AP.Init();
			//
			SetPlayerID(pID);
			//
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumGunAnimType::Aim].Init(true);
			//
			this->m_MagArm.Init(false);
			this->m_MagHand = false;
			m_EyeSwingControl.Init();
			this->m_IsChanging = false;
			this->m_IsChange = false;
			//
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			this->m_MoveOverRideFlag = false;
			//銃のIDセットアップ
			for (int loop = 0; loop < 2; ++loop) {
				if (m_GunPtrControl.GetGunPtr(loop)) {
					m_GunPtrControl.GetGunPtr(loop)->SetFallObject();
					m_GunPtrControl.GetGunPtr(loop)->SetPlayerID(GetMyPlayerID());
				}
			}
			m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
			m_SlingArmZrad.Init(0.08f * Scale3DRate, 3.f, deg2rad(50));
			m_ArmBreakPer = 0.f;
			m_HPRec = 0.f;
			m_CharaAction = CharaActionID::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept {
			m_KeyControl.Init(pxRad, pyRad);
			SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), m_KeyControl.GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), m_KeyControl.GetRad().y));
			m_GunPtrControl.SetupMagazine();
			m_GunPtrControl.SelectGun(GunSel, true);
			this->m_GunReadySeq = EnumGunReadySeq::LowReady;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			m_KeyControl.Input(pInput, pReady, Vector2DX::zero());
			//
			if (pReady && IsAlive()) {
				if (IsLowHP()) {
					m_HPRec += DXLib_refParts->GetDeltaTime();
					if (m_HPRec >= 0.f) {
						m_HPRec -= 2.f;
						Heal(2, false);
						if (GetMyPlayerID() == 0) {
							SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->Play(DX_PLAYTYPE_BACK);
						}
					}
				}
				else {
					if (m_HPRec != 0.f) {
						if (GetMyPlayerID() == 0) {
							SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->StopAll();
							SE->Get(SoundType::SE, (int)SoundEnum::Man_breathend)->Play(DX_PLAYTYPE_BACK);
						}
					}
					m_HPRec = 0.f;
				}
			}
			//AIM
			if (m_GunPtrControl.GetGunPtrNow()) {
				if (!m_IsChanging) {
					if (m_KeyControl.GetULTKey()) {
						m_IsChanging = true;
						m_GunPtrControl.SelectReserveGun(1 - m_GunPtrControl.GetNowGunSelect());
					}
				}
				else {
					this->m_GunReadySeq = EnumGunReadySeq::LowReady;
					if (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f) {
						this->m_GunReadySeq = EnumGunReadySeq::Aim;
						m_GunPtrControl.InvokeReserveGunSel();
						m_IsChanging = false;
					}
				}
				m_IsChange |= m_IsChanging;
				if (!m_IsChanging && (this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f)) {
					m_IsChange = false;
				}
			}
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			m_HitBoxControl.Init();
			m_ArmBreak = false;
			EffectControl::Init();
			m_Grenade.Dispose();
			m_Grenade.Init("data/model/RGD5/", 4);	//装填したマガジンの弾に合わせて薬莢生成

			m_ChamberAmmoData = AmmoDataManager::Instance()->LoadAction("data/ammo/FragRGD/");
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				int num = GetObj().GetMaterialNum();
				for (int i = 0; i < num; i++) {
					GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
					GetObj().SetMaterialAmbColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
				}
			}
			ExecuteInput();
			ExecuteSound();
			ExecuteMatrix();
			EffectControl::Execute();
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if ((CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.5f * Scale3DRate, -2.5f * Scale3DRate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE)
					) {
					//
					int fog_enable;
					int fog_mode;
					int fog_r, fog_g, fog_b;
					float fog_start, fog_end;
					float fog_density;
					fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効 FALSE:無効 )
					fog_mode = GetFogMode();														// フォグモードを取得する
					GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
					GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
					fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )


					//キャラ描画
					SetFogEnable(TRUE);
					SetFogColor(0, 0, 0);
					//MV1SetMaterialTypeAll(this->GetObj().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
					if (IsAlive()) {
						for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
							if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
					else {
						for (int i = 0; i < m_RagDollControl.GetRagDoll().GetMeshNum(); i++) {
							if (m_RagDollControl.GetRagDoll().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								m_RagDollControl.GetRagDoll().DrawMesh(i);
							}
						}
					}
					//hitbox描画
					//m_HitBoxControl.DrawHitBox();
					//
					SetFogEnable(fog_enable);
					SetFogMode(fog_mode);
					SetFogColor(fog_r, fog_g, fog_b);
					SetFogStartEnd(fog_start, fog_end);
					SetFogDensity(fog_density);
				}
			}
		}
	};
};
