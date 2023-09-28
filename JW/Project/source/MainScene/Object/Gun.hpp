#pragma once
#include	"../../Header.hpp"

#include "GunEnum.hpp"
#include "GunData.hpp"
#include "Gun_before.hpp"

#include "../../sub.hpp"
#include "ObjectBase.hpp"
#include "Ammo.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass :
			public ObjectBaseClass,
			public MuzzleSmokeControl
		{
		private:
			std::shared_ptr<GunDataClass>	m_GunDataClass;
			std::shared_ptr<AmmoDataClass>	m_ChamberAmmoData{ nullptr };		//
			GunAnimeID						m_ShotPhase{ GunAnimeID::Base };	//
			bool							m_ShotSwitch{ false };				//
			int								m_boltSoundSequence{ -1 };			//サウンド
			bool							m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool							m_IsEject{ false };
			FallControl						m_MagFall;
			FallControl						m_CartFall;
			SlotPartsControl				m_ModControl;
			std::vector<const std::shared_ptr<ObjectBaseClass>*> m_PartsList;
			const std::shared_ptr<SightClass>*m_SightPtr{ nullptr };
		private:
			std::shared_ptr<BackGroundClassBase>		m_BackGround;			//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
		public://ゲッター
			const auto& GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const auto& GetFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first; }
			const MATRIX_ref GetFrameLocalMat(GunFrame frame) const noexcept;
			const MATRIX_ref GetFrameWorldMat(GunFrame frame) const noexcept;
		public:
			void ResetFrameLocalMat(GunFrame frame) noexcept;
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept;
		private:
			const auto	GetGunSoundSet(void) const noexcept { return GunSoundSets[this->m_GunDataClass->GetSoundSel()]; }
			const auto& GetMagazinePtr(void) const noexcept { return (std::shared_ptr<MagazineClass>&)m_ModControl.GetPartsPtr(ObjType::Magazine); }
		public://ゲッター
			const GraphHandle* GetReticlePtr(void) const noexcept {
				if (this->m_SightPtr) {
					return &(*this->m_SightPtr)->GetReitcleGraph();
				}
				return nullptr;
			}
			const auto& GetName(void) const noexcept { return this->m_GunDataClass->GetName(); }
			const auto& GetShotType(void) const noexcept { return this->m_GunDataClass->GetShotType(); }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_GunDataClass->GetHumanAnimType(); }
			const auto& GetReloadType(void) const noexcept { return this->m_GunDataClass->GetReloadType(); }
			const auto& GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto& GetShotPhase(void) const noexcept { return this->m_ShotPhase; }
			const auto& GetNowAnime(void) noexcept { return GetObj().get_anime((size_t)this->m_ShotPhase); }
			const auto& GetAmmoAll(void) const noexcept { return GetMagazinePtr()->GetAmmoAll(); }
			const auto	GetIsMagEmpty(void) const noexcept { return GetMagazinePtr()->GetAmmoNum() == 0; }//次弾がない
			const auto	GetIsMagFull(void) const noexcept { return GetMagazinePtr()->GetAmmoNum() == this->GetAmmoAll(); }
			const auto	GetShoting(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Shot; }
			const auto	GetCocking(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Cocking; }
			const auto	GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::ReloadEnd); }
			const auto	GetChecking(void) const noexcept { return (GunAnimeID::CheckStart <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::CheckEnd); }
			const auto	GetShootReady(void) const noexcept { return this->m_ShotPhase <= GunAnimeID::Shot; }
			const auto	GetInChamber(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto	GetCanShot(void) const noexcept { return GetInChamber() && GetShootReady(); }

			const auto	GetEyePos(void) const noexcept { return GetFrameWorldMat(GunFrame::Eyepos).pos(); }
			const auto	GetLensPos(void) const noexcept { return GetFrameWorldMat(GunFrame::Lens).pos(); }
			const auto	GetLensPosSize(void) const noexcept { return GetFrameWorldMat(GunFrame::LensSize).pos(); }
			const auto	GetReticlePos(void) const noexcept { return GetLensPos() + (GetLensPos() - GetEyePos()).Norm()*10.f; }

			const auto	GetMuzzleMatrix(void) const noexcept { return GetFrameWorldMat(GunFrame::Muzzle); }
			const auto	GetCartMat(void) noexcept { return GetFrameWorldMat(GunFrame::Cart); }
			const auto	GetCartVec(void) noexcept { return (GetFrameWorldMat(GunFrame::CartVec).pos() - GetCartMat().pos()).Norm(); }
			const auto	GetAmmoNum(void) const noexcept { return GetMagazinePtr()->GetAmmoNum() + (GetInChamber() ? 1 : 0); }
		public:
			void		SetAmmoHandMatrix(const MATRIX_ref& value, float pPer) noexcept { GetMagazinePtr()->SetHandMatrix(value, pPer); }
			void		FillMag() noexcept { GetMagazinePtr()->SetAmmo(GetAmmoAll()); }
			void		CockByMag() noexcept { this->m_ChamberAmmoData = GetMagazinePtr()->GetAmmoSpecMagTop(); }//マガジンの一番上の弾データをチャンバーイン
			void		UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void		SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void		SetGunMatrix(const MATRIX_ref& value) noexcept {
				SetMove(value.GetRot(), value.pos());
				m_ModControl.UpdatePartsAnim(GetObj());
				m_ModControl.UpdatePartsMove(GetFrameWorldMat(GunFrame::Lower), ObjType::Lower);
				m_ModControl.UpdatePartsMove(GetFrameWorldMat(GunFrame::Upper), ObjType::Upper);
				m_ModControl.UpdatePartsMove(GetFrameWorldMat(GunFrame::Barrel), ObjType::Barrel);
				m_ModControl.UpdatePartsMove(GetFrameWorldMat(GunFrame::Magpos), ObjType::Magazine);
			}
			void		SetShotPhase(GunAnimeID pShotPhase) noexcept { this->m_ShotPhase = pShotPhase; }
			void		SetBullet(void) noexcept;//発砲
			void		SetMagFall() noexcept {
				m_MagFall.SetFall(
					GetFrameWorldMat(GunFrame::Magpos).pos(),
					GetFrameWorldMat(GunFrame::Magpos).GetRot(),
					GetFrameWorldMat(GunFrame::Magpos).yvec()*-1.f*(Scale_Rate * 3.f / 60.f),
					12.f);
			}
			void		SetReloadStart() noexcept { this->m_ShotPhase = (!GetIsMagEmpty()) ? GunAnimeID::ReloadStart : GunAnimeID::ReloadStart_Empty; }
		private:
			void		ExecuteCartInChamber(void) noexcept;//チャンバーへの装弾、排出
		public:
			GunClass(void) noexcept { this->m_objType = ObjType::Gun; }
			~GunClass(void) noexcept {}
		public:
			void			Init(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			Dispose(void) noexcept override;
		};
	};
};
