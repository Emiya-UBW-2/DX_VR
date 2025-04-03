#include	"FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		bool FallObjClass::PopGrenadeBombSwitch(void) noexcept {
			if (this->m_GrenadeBombFlag) {
				this->m_GrenadeBombFlag = false;
				return true;
			}
			return false;
		}
		void			FallObjClass::SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, FallObjectType Type) noexcept {
			this->m_Timer = timer;
			this->m_FallObjectType = Type;

			SetMove().SetAll(pos, pos, pos, vec, mat, mat);
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			ObjectBaseClass::SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
			ObjectBaseClass::SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			SetActive(true);
			this->m_yAdd = 0.f;
			this->m_SoundSwitch = true;
			this->m_GrenadeBombFlag = false;
		}
		void			FallObjClass::FirstExecute(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			if (IsActive()) {
				Vector3DX PosBuf = GetMove().GetPos() + GetMove().GetVec() * 60.f * DXLib_refParts->GetDeltaTime() + Vector3DX::up() * this->m_yAdd;
				this->m_yAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
				{
					Vector3DX EndPos = PosBuf;
					Vector3DX Normal;
					if (BackGround->CheckLinetoMap(GetMove().GetRePos(), &EndPos, &Normal)) {
						PosBuf = EndPos + Normal * (0.5f * Scale3DRate);
						SetMove().SetVec(Vector3DX::Reflect(GetMove().GetVec(), Normal) * 0.5f);
						this->m_yAdd = 0.f;
						if (this->m_SoundSwitch) {
							this->m_SoundSwitch = false;
							switch (this->m_FallObjectType) {
							case FallObjectType::Cart:
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::CartFall))->Play3D(PosBuf, Scale3DRate * 5.f);
								break;
							case FallObjectType::Grenade:
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::FallGrenade))->Play3D(PosBuf, Scale3DRate * 5.f);
								break;
							case FallObjectType::Magazine:
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::MagFall))->Play3D(PosBuf, Scale3DRate * 5.f);
								break;
							default:
								break;
							}
						}
						switch (this->m_FallObjectType) {
						case FallObjectType::Magazine:
							//NormalをX軸に指定して横に向く
							SetMove().SetMat(GetMove().GetMat() * Matrix3x3DX::RotVec2(GetMove().GetMat().xvec(), Vector3DX::up()));
							break;
						default:
							break;
						}

					}
				}
				switch (this->m_FallObjectType) {
				case FallObjectType::Cart:
				case FallObjectType::Grenade:
				{
					//テキトーに飛び回る
					auto BB = (PosBuf - GetMove().GetRePos()).normalized();
					if ((PosBuf - GetMove().GetRePos()).y <= 0.f) {
						BB *= -1.f;
					}
					SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(BB, GetMove().GetMat().zvec()), deg2rad(-50.f * 60.f * DXLib_refParts->GetDeltaTime())) * GetMove().GetMat());
				}
					break;
				case FallObjectType::Magazine:
					//そのままを維持する
					break;
				default:
					break;
				}
				SetMove().SetPos(PosBuf);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				//
				if (this->m_Timer < 0.f) {
					switch (this->m_FallObjectType) {
					case FallObjectType::Cart:
						break;
					case FallObjectType::Grenade:
						if (!this->m_GrenadeBombFlag) {
							this->m_GrenadeBombFlag = true;
							EffectSingleton::Instance()->SetOnce_Any(Sceneclass::Effect::ef_greexp, GetMove().GetPos(), Vector3DX::forward(), 0.5f * Scale3DRate, 2.f);
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Explosion))->Play3D(GetMove().GetPos(), Scale3DRate * 25.f);
						}
						break;
					case FallObjectType::Magazine:
						break;
					default:
						break;
					}
					SetActive(false);
				}
				else {
					this->m_Timer -= DXLib_refParts->GetDeltaTime();
				}
			}
		}
	};
};
