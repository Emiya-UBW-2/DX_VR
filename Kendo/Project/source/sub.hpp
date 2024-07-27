#pragma once
#include	"Header.hpp"

#include "MainScene/Object/CharacterEnum.hpp"
//
namespace FPS_n2 {
	static void			IK_move_Arm(
		const Vector3DX& Localyvec,
		const Vector3DX& Localzvec,
		MV1* pObj,
		float XPer,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {

		auto GetFramePosition = [&](int frame) { return Vector3DX(MV1GetFramePosition(pObj->GetHandle(), frame)); };
		auto AngleOf2Vector = [&](Vector3DX A, Vector3DX B) { return acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); };			//２つのベクトルABのなす角度θを求める

		pObj->ResetFrameUserLocalMatrix(Arm);
		pObj->ResetFrameUserLocalMatrix(Arm2);
		pObj->ResetFrameUserLocalMatrix(Wrist);
		auto matBase = pObj->GetFrameLocalWorldMatrix(static_cast<int>(pObj->GetFrameParent(Arm))).rotation().inverse();

		Vector3DX Dirxvec = Vector3DX::Cross(Dirzvec, Diryvec) * -1.f;

		Vector3DX RetPos = DirPos;
		//基準
		auto vec_a1 = Matrix4x4DX::Vtrans((RetPos - GetFramePosition(Arm)).normalized(), matBase);//基準
		auto vec_a1L1 = Vector3DX(Vector3DX::vget(XPer, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();//x=0とする
		float cos_t = GetCosFormula((GetFramePosition(Wrist) - GetFramePosition(Arm2)).magnitude(), (GetFramePosition(Arm2) - GetFramePosition(Arm)).magnitude(), (GetFramePosition(Arm) - RetPos).magnitude());
		auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
		//上腕
		pObj->SetFrameLocalMatrix(Arm, Matrix4x4DX::identity() * FrameBaseLocalMatArm);
		Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(GetFramePosition(Arm2) - GetFramePosition(Arm), matBase), vec_t);
		pObj->SetFrameLocalMatrix(Arm, a1_inv * FrameBaseLocalMatArm);
		//下腕
		matBase = pObj->GetFrameLocalWorldMatrix(static_cast<int>(pObj->GetFrameParent(Arm2))).rotation().inverse();
		pObj->SetFrameLocalMatrix(Arm2, Matrix4x4DX::identity() * FrameBaseLocalMatArm2);
		Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
			Matrix4x4DX::Vtrans(GetFramePosition(Wrist) - GetFramePosition(Arm2), matBase),
			Matrix4x4DX::Vtrans(RetPos - GetFramePosition(Arm2), matBase));
		pObj->SetFrameLocalMatrix(Arm2, a2_inv * FrameBaseLocalMatArm2);
		//手
		matBase = pObj->GetFrameLocalWorldMatrix(static_cast<int>(pObj->GetFrameParent(Wrist))).rotation().inverse();
		Matrix4x4DX mat1;
		{
			auto zvec = Matrix4x4DX::Vtrans(Localzvec, pObj->GetFrameLocalWorldMatrix(Wrist).rotation());
			mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Dirzvec, matBase)) * mat1;
			pObj->SetFrameLocalMatrix(Wrist, mat1 * FrameBaseLocalMatWrist);
			auto xvec = Matrix4x4DX::Vtrans(Localyvec, pObj->GetFrameLocalWorldMatrix(Wrist).rotation());
			mat1 = Matrix4x4DX::RotAxis(Localzvec, AngleOf2Vector(xvec, Dirxvec) * ((Vector3DX::Dot(Diryvec, xvec) > 0.f) ? -1.f : 1.f)) * mat1;
		}
		pObj->SetFrameLocalMatrix(Wrist, mat1 * FrameBaseLocalMatWrist);
	}

	static void			IK_RightArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {
		IK_move_Arm(
			Vector3DX::vget(0.f, 0.f, -1.f).normalized(),
			Vector3DX::vget(-1.f, -1.f, 0.f).normalized(),
			pObj, -0.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirPos, Diryvec * -1.f, Dirzvec);
	}
	static void			IK_LeftArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {

		IK_move_Arm(
			Vector3DX::vget(0.f, 0.f, -1.f).normalized(),
			Vector3DX::vget(1.f, -1.f, 0.f).normalized(),
			pObj, 1.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirPos, Diryvec, Dirzvec);
	}
	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{ 127 };
		PlayerID				DamageID{ 127 };
		HitPoint				Damage{ 0 };
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
		}
	};
	class DamageEventControl {
		std::array<DamageEvent, 16>							m_Damage;
		uint8_t												m_DamageCount{ 0 };
	public:
		const auto& GetDamageEvent(void) const noexcept { return this->m_Damage; }
		const auto& GetDamageEventCount(void) const noexcept { return this->m_DamageCount; }
	public:
		void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept {
			if (!pRet) { return; }
			for (auto& e : this->m_Damage) {
				if ((&e - &this->m_Damage.front()) >= this->m_DamageCount) { break; }
				pRet->emplace_back(e);
			}
			this->m_DamageCount = 0;
		}
	public:
		void			Reset(void) noexcept {
			this->m_DamageCount = 0;
		}
		void			Add(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage) noexcept {
			this->m_Damage.at(this->m_DamageCount).SetEvent(ShotID_t, DamageID_t, pDamage);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
	};
	//
	class CommonBattleResource : public SingletonBase<CommonBattleResource> {
	private:
		friend class SingletonBase<CommonBattleResource>;
	private:
		CommonBattleResource(void) noexcept {}
		CommonBattleResource(const CommonBattleResource&) = delete;
		CommonBattleResource(CommonBattleResource&& o) = delete;
		CommonBattleResource& operator=(const CommonBattleResource&) = delete;
		CommonBattleResource& operator=(CommonBattleResource&& o) = delete;

		virtual ~CommonBattleResource(void) noexcept {}
	public:
		void			Load(void) noexcept;
		void			Set(void) noexcept;
		void			Dispose(void) noexcept;
	public:
		void			LoadChara(const std::string& FolderName, PlayerID ID) noexcept;
		void			LoadWeapon(const std::string& FolderName, PlayerID ID) noexcept;
	};
};
