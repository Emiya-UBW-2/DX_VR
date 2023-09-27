#pragma once
#include	"../../Header.hpp"

#include "GunEnum.hpp"
#include "GunData.hpp"
#include "Gun_before.hpp"

#include "../../sub.hpp"
#include "ObjectBase.hpp"
#include "Ammo.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass :
			public ObjectBaseClass,
			public MuzzleSmokeControl,
			public FallMagControl,
			public FallCartControl,
			public SlotPartsControl
		{
		private:
			std::shared_ptr<GunDataClass>	m_GunDataClass;

			std::shared_ptr<AmmoDataClass>	m_ChamberAmmoData{ nullptr };		//
			GunAnimeID						m_ShotPhase{ GunAnimeID::Base };	//
			bool							m_ShotSwitch{ false };				//
			int								m_boltSoundSequence{ -1 };			//サウンド
			bool							m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool							m_IsEject{ false };
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
		public://ゲッター
			const auto& GetName(void) const noexcept { return this->m_GunDataClass->GetName(); }
			const auto& GetShotType(void) const noexcept { return this->m_GunDataClass->GetShotType(); }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_GunDataClass->GetHumanAnimType(); }
			const auto& GetReloadType(void) const noexcept { return this->m_GunDataClass->GetReloadType(); }
			const auto& GetAmmoAll(void) const noexcept { return this->GetMagazinePtr()->GetAmmoAll(); }
			const auto& GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto& GetShotPhase(void) const noexcept { return this->m_ShotPhase; }
			const auto& GetNowAnime(void) noexcept { return GetObj().get_anime((size_t)this->m_ShotPhase); }
			const auto	GetIsMagEmpty(void) const noexcept { return this->GetMagazinePtr()->GetAmmoNum() == 0; }//次弾がない
			const auto	GetIsMagFull(void) const noexcept { return this->GetMagazinePtr()->GetAmmoNum() == this->GetAmmoAll(); }
			const auto	GetShoting(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Shot; }
			const auto	GetCocking(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Cocking; }
			const auto	GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::ReloadEnd); }
			const auto	GetChecking(void) const noexcept { return (GunAnimeID::CheckStart <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::CheckEnd); }
			const auto	GetShootReady(void) const noexcept { return this->m_ShotPhase <= GunAnimeID::Shot; }
			const auto	GetInChamber(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto	GetCanShot(void) const noexcept { return GetInChamber() && GetShootReady(); }
			const auto	GetEyePos(void) const noexcept { return GetFrameWorldMat(GunFrame::Eyepos).pos(); }
			const auto	GetMuzzleMatrix(void) const noexcept { return GetFrameWorldMat(GunFrame::Muzzle); }
			const auto	GetCartMat(void) noexcept { return GetFrameWorldMat(GunFrame::Cart); }
			const auto	GetCartVec(void) noexcept { return (GetFrameWorldMat(GunFrame::CartVec).pos() - GetCartMat().pos()).Norm(); }
			const auto	GetAmmoNum(void) const noexcept { return this->GetMagazinePtr()->GetAmmoNum() + (GetInChamber() ? 1 : 0); }
		public:
			void		UpdateMagMove(const MATRIX_ref& pMat) { this->GetMagazinePtr()->SetMove(pMat.GetRot(), pMat.pos()); }
			void		SetAmmoHandMatrix(const MATRIX_ref& value, float pPer) noexcept { this->GetMagazinePtr()->SetHandMatrix(value, pPer); }
			void		FillMag() noexcept { this->GetMagazinePtr()->SetAmmo(GetAmmoAll()); }
			void		CockByMag() noexcept { this->m_ChamberAmmoData = this->GetMagazinePtr()->GetAmmoSpecMagTop(); }//マガジンの一番上の弾データをチャンバーイン
			void		UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void		SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void		SetGunMatrix(const MATRIX_ref& value) noexcept {
				SetMove(value.GetRot(), value.pos());
				UpdatePartsAnim(GetObj());
				UpdatePartsMove(GetFrameWorldMat(GunFrame::Lower), ObjType::Lower);
				UpdatePartsMove(GetFrameWorldMat(GunFrame::Upper), ObjType::Upper);
				UpdatePartsMove(GetFrameWorldMat(GunFrame::Barrel), ObjType::Barrel);
				UpdateMagMove(GetFrameWorldMat(GunFrame::Magpos));
			}
			void		SetShotPhase(GunAnimeID pShotPhase) noexcept { this->m_ShotPhase = pShotPhase; }
			void		SetBullet(void) noexcept;//発砲
			void		SetMagFall() noexcept {
				SetFallMagControl(
					GetFrameWorldMat(GunFrame::Magpos).pos(),
					GetFrameWorldMat(GunFrame::Magpos).GetRot(),
					GetFrameWorldMat(GunFrame::Magpos).yvec()*-1.f*(Scale_Rate * 3.f / 60.f));
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
