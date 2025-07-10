#include	"ItemObj.hpp"

const FPS_n2::Objects::ItemObjDataManager* SingletonBase<FPS_n2::Objects::ItemObjDataManager>::m_Singleton = nullptr;
const FPS_n2::Objects::ItemObjPool* SingletonBase<FPS_n2::Objects::ItemObjPool>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Objects {
		void ItemObj::FirstUpdate(void) noexcept {
			if (!IsActive()) { return; }
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();

			this->m_Timer += DXLib_refParts->GetDeltaTime();
			Vector3DX PosBuf = GetMove().GetPos();
			PosBuf = PosBuf + GetMove().GetVec() * DXLib_refParts->GetDeltaTime() + Vector3DX::up() * this->m_yAdd;
			this->m_yAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
			Vector3DX Vec = PosBuf - m_Repos; Vec.y = 0.f;
			//壁判定
			if (Vec.sqrMagnitude() > 0.01f * 0.01f) {
				BackGroundParts->CheckMapWall(GetMove().GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (0.8f * Scale3DRate), 0.7f * Scale3DRate);
				this->m_Yrad = Vector3DX::Angle(Vector3DX::right() * -1.f, Vec);
				this->m_IsLR ^= 1;
			}
			{
				Vector3DX EndPos = PosBuf + Vector3DX::up() * 0.8f;
				if (BackGround::BackGroundControl::Instance()->CheckLinetoMap(PosBuf, &EndPos) != 0) {
					PosBuf = EndPos;
					this->m_yAdd = 0.0f;
					Vector3DX Vector = GetMove().GetVec(); Vector.y = 0.f;
					Easing(&Vector, Vector3DX::zero(), 0.9f, EasingType::OutExpo);
					SetMove().SetVec(Vector);
				}
				else {
					Vector3DX Vector = GetMove().GetVec();
					Easing(&Vector, Vector3DX::zero(), 0.95f, EasingType::OutExpo);
					SetMove().SetVec(Vector);
				}
			}
			PosBuf.x = std::clamp(PosBuf.x, -16.f * Scale3DRate, 16.f * Scale3DRate);
			PosBuf.z = std::clamp(PosBuf.z, -16.f * Scale3DRate, 16.f * Scale3DRate);


			this->m_Yrad += deg2rad(60.f * DXLib_refParts->GetDeltaTime() * (m_IsLR ? 1.f : -1.f));
			this->m_Zrotate.Update(DXLib_refParts->GetDeltaTime());
			this->m_Zrotate.AddRad(Vec.magnitude() * 0.02f);
			SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_Zrotate.GetRad()) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_Yrad));
			SetMove().SetPos(PosBuf);
			SetMove().Update(0.0f, 0.0f);
			Easing(&m_Rot, GetMove().GetMat(), 0.95f, EasingType::OutExpo);
			Easing(&m_Pos, GetMove().GetPos(), 0.95f, EasingType::OutExpo);
			UpdateObjMatrix(m_Rot, m_Pos);
			m_Repos = PosBuf;

		}
	}
}