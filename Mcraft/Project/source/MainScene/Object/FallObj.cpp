#include	"FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//それぞれのオブジェクトの動き
		class FallObjCart : public FallObjChildBase {
		public:
			FallObjCart() {}
			virtual ~FallObjCart() {}
		public:
			SoundEnum GetFallSound(void) const noexcept override { return SoundEnum::CartFall; }
		public:
			void RotateOnAir(moves*) noexcept override {}//なにもしない
			void RotateOnGround(moves* objMove) noexcept override {
				//テキトーに飛び回る
				auto* DXLib_refParts = DXLib_ref::Instance();
				objMove->SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(objMove->GetVec().normalized(), objMove->GetMat().zvec()), deg2rad(-50.f * 60.f * DXLib_refParts->GetDeltaTime())) * objMove->GetMat());
			}
			void OnTimeEnd(const moves&) noexcept override {}//なにもしない
		};
		class FallObjMagazine : public FallObjChildBase {
		public:
			FallObjMagazine() {}
			virtual ~FallObjMagazine() {}
		public:
			SoundEnum GetFallSound(void) const noexcept override { return SoundEnum::MagFall; }
		public:
			void RotateOnAir(moves* objMove) noexcept override {
				//NormalをX軸に指定して横に向く
				objMove->SetMat(objMove->GetMat() * Matrix3x3DX::RotVec2(objMove->GetMat().xvec(), Vector3DX::up()));
			}
			void RotateOnGround(moves*) noexcept override {}// なにもしない
			void OnTimeEnd(const moves&) noexcept override {}//なにもしない
		};
		class FallObjGrenade : public FallObjChildBase {
		public:
			FallObjGrenade() {}
			virtual ~FallObjGrenade() {}
		public:
			SoundEnum GetFallSound(void) const noexcept override { return SoundEnum::FallGrenade; }
		public:
			void RotateOnAir(moves*) noexcept override {}//なにもしない
			void RotateOnGround(moves* objMove) noexcept override {
				//テキトーに飛び回る
				auto* DXLib_refParts = DXLib_ref::Instance();
				objMove->SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(objMove->GetVec().normalized(), objMove->GetMat().zvec()), deg2rad(-50.f * 60.f * DXLib_refParts->GetDeltaTime())) * objMove->GetMat());
			}
			void OnTimeEnd(const moves& objMove) noexcept override {
				auto* SE = SoundPool::Instance();
				EffectSingleton::Instance()->SetOnce_Any(Sceneclass::Effect::ef_greexp, objMove.GetPos(), Vector3DX::forward(), 0.5f * Scale3DRate, 2.f);
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Explosion))->Play3D(objMove.GetPos(), Scale3DRate * 25.f);
			}
		};


		void			FallObjClass::SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, FallObjectType Type) noexcept {
			this->m_Timer = timer;
			switch (Type) {
			case FallObjectType::Cart:
				m_FallObject = std::make_unique<FallObjCart>();
				break;
			case FallObjectType::Grenade:
				m_FallObject = std::make_unique<FallObjGrenade>();
				break;
			case FallObjectType::Magazine:
				m_FallObject = std::make_unique<FallObjMagazine>();
				break;
			default:
				break;
			}

			SetMove().SetAll(pos, pos, pos, vec, mat, mat);
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			ObjectBaseClass::SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
			ObjectBaseClass::SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			SetActive(true);
			this->m_yAdd = 0.f;
			this->m_SoundSwitch = true;
			this->m_IsEndFall = false;
		}
		void			FallObjClass::FirstExecute(void) noexcept {
			if (!IsActive()) { return; }
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
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
						SE->Get(SoundType::SE, static_cast<int>(m_FallObject->GetFallSound()))->Play3D(PosBuf, Scale3DRate * 5.f);
					}
					m_FallObject->RotateOnGround(&SetMove());
				}
			}
			m_FallObject->RotateOnAir(&SetMove());

			SetMove().SetPos(PosBuf);
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			//
			if (this->m_Timer < 0.f) {
				this->m_IsEndFall = true;
				m_FallObject->OnTimeEnd(GetMove());
				m_FallObject.reset();
				SetActive(false);
			}
			else {
				this->m_Timer -= DXLib_refParts->GetDeltaTime();
			}
		}
	};
};
