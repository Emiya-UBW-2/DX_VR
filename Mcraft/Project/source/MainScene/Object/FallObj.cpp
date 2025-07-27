#include	"FallObj.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Objects {
		//それぞれのオブジェクトの動き
		class FallObjCart : public FallObjChildBase {
		public:
			FallObjCart(void) noexcept {}
			virtual ~FallObjCart(void) noexcept {}
		public:
			SoundEnum GetFallSound(void) const noexcept override { return SoundEnum::CartFall; }
		public:
			void RotateOnAir(moves* objMove) noexcept override {
				//テキトーに飛び回る
				auto* DXLib_refParts = DXLib_ref::Instance();
				objMove->SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(objMove->GetVec().normalized(), objMove->GetMat().zvec()), deg2rad(-50.0f * 60.0f * DXLib_refParts->GetDeltaTime())) * objMove->GetMat());
			}
			void RotateOnGround(moves*) noexcept override {}
			void OnTimeEnd(const moves&) noexcept override {}//なにもしない
		public:
			bool IsDrawFar(void) const noexcept override { return false; }
		};
		class FallObjMagazine : public FallObjChildBase {
		public:
			FallObjMagazine(void) noexcept {}
			virtual ~FallObjMagazine(void) noexcept {}
		public:
			SoundEnum GetFallSound(void) const noexcept override { return SoundEnum::MagFall; }
		public:
			void RotateOnAir(moves*) noexcept override {}// なにもしない
			void RotateOnGround(moves* objMove) noexcept override {
				//NormalをX軸に指定して横に向く
				objMove->SetMat(objMove->GetMat() * Matrix3x3DX::RotVec2(objMove->GetMat().xvec(), Vector3DX::up()));
			}
			void OnTimeEnd(const moves&) noexcept override {}//なにもしない
		public:
			bool IsDrawFar(void) const noexcept override { return false; }
		};
		class FallObjGrenade : public FallObjChildBase {
		public:
			FallObjGrenade(void) noexcept {}
			virtual ~FallObjGrenade(void) noexcept {}
		public:
			SoundEnum GetFallSound(void) const noexcept override { return SoundEnum::FallGrenade; }
		public:
			void RotateOnAir(moves* objMove) noexcept override {
				//テキトーに飛び回る
				auto* DXLib_refParts = DXLib_ref::Instance();
				objMove->SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(objMove->GetVec().normalized(), objMove->GetMat().zvec()), deg2rad(-50.0f * 60.0f * DXLib_refParts->GetDeltaTime())) * objMove->GetMat());
			}
			void RotateOnGround(moves*) noexcept override {}//なにもしない
			void OnTimeEnd(const moves& objMove) noexcept override {
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					EffectSingleton::Instance()->SetOnce_Any(Effect::ef_greexp, objMove.GetPos(), Vector3DX::forward(), 0.5f * Scale3DRate, 2.0f);
				}
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Explosion))->Play3D(objMove.GetPos(), Scale3DRate * 25.0f);
			}
		public:
			bool IsDrawFar(void) const noexcept override { return true; }
		};


		void			FallObj::SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, FallObjectType Type) noexcept {
			this->m_Timer = timer;
			switch (Type) {
			case FallObjectType::Cart:
				this->m_FallObject = std::make_unique<FallObjCart>();
				break;
			case FallObjectType::Grenade:
				this->m_FallObject = std::make_unique<FallObjGrenade>();
				break;
			case FallObjectType::Magazine:
				this->m_FallObject = std::make_unique<FallObjMagazine>();
				break;
			default:
				break;
			}

			SetMove().SetAll(pos, pos, pos, vec, mat, mat);
			SetMove().Update(0.0f, 0.0f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			SetActive(true);
			this->m_yAdd = 0.0f;
			this->m_SoundSwitch = true;
			this->m_IsEndFall = false;
			this->m_CalcTimer = 0.0f;
		}
		void			FallObj::FirstUpdate(void) noexcept {
			if (!IsActive()) { return; }
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();

			Vector3DX PosBuf = GetMove().GetPos();
			m_CalcTimer += DXLib_refParts->GetDeltaTime();
			while(true) {
				if (m_CalcTimer > 1.0f / 60.0f) {
					m_CalcTimer -= 1.0f / 60.0f;
					Vector3DX RePos = PosBuf;
					PosBuf = PosBuf + GetMove().GetVec() + Vector3DX::up() * this->m_yAdd;
					this->m_yAdd += (GravityRate / (60.0f * 60.0f));
					{
						Vector3DX EndPos = PosBuf;
						Vector3DX Normal;
						/*
						if (Player::PlayerManager::Instance()->GetVehicle()->CheckLine(GetMove().GetRePos(), &EndPos, &Normal)) {
							PosBuf = EndPos + Normal * (0.5f * Scale3DRate);
							SetMove().SetVec(Vector3DX::Reflect(GetMove().GetVec(), Normal) * 0.5f);
							this->m_yAdd = 0.0f;
							if (this->m_SoundSwitch) {
								this->m_SoundSwitch = false;
								SE->Get(SoundType::SE, static_cast<int>(this->m_FallObject->GetFallSound()))->Play3D(PosBuf, Scale3DRate * 5.0f);
							}
							this->m_FallObject->RotateOnGround(&SetMove());
						}
						else
						//*/
						if (BackGround::BackGroundControl::Instance()->CheckLinetoMap(RePos, &EndPos, &Normal) != 0) {
							PosBuf = EndPos + Normal * (0.5f * Scale3DRate);
							SetMove().SetVec(Vector3DX::Reflect(GetMove().GetVec(), Normal) * 0.5f);
							this->m_yAdd = 0.0f;
							if (this->m_SoundSwitch) {
								this->m_SoundSwitch = false;
								SE->Get(SoundType::SE, static_cast<int>(this->m_FallObject->GetFallSound()))->Play3D(PosBuf, Scale3DRate * 5.0f);
							}
							this->m_FallObject->RotateOnGround(&SetMove());
						}
					}
				}
				else {
					break;
				}
			}

			this->m_FallObject->RotateOnAir(&SetMove());

			SetMove().SetPos(PosBuf);
			SetMove().Update(0.0f, 0.0f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			//
			if (this->m_Timer < 0.0f) {
				this->m_IsEndFall = true;
				this->m_FallObject->OnTimeEnd(GetMove());
				this->m_FallObject.reset();
				SetActive(false);
			}
			else {
				this->m_Timer -= DXLib_refParts->GetDeltaTime();
			}
		}
	}
}
