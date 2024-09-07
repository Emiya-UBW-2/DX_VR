#include	"Character_before.hpp"
#include	"Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		bool MagStockControl::GetNeedAmmoLoad(bool MagInGunFull, bool MagInGunEmpty) noexcept {
			int Total = (int)m_MagazineStock.size();
			//半端マグが2個以上ある
			//もしくは半端マグがあってストックもある
			int RetFull = MagInGunFull ? 1 : 0;
			int RetEmpty = MagInGunEmpty ? 1 : 0;
			int RetNotFull = (!MagInGunEmpty && !MagInGunFull) ? 1 : 0;
			for (int i = 0; i < Total; i++) {
				if (m_MagazineStock[i].AmmoNum == 0) {
					RetEmpty++;
				}
				else if (m_MagazineStock[i].AmmoNum == m_MagazineStock[i].AmmoAll) {
					RetFull++;
				}
				else {
					RetNotFull++;
				}
			}
			if (RetFull >= Total + 1) { return false; }//全部満タン
			if (RetEmpty >= Total + 1 && m_AmmoStock == 0) { return false; }//全部空で予備もない
			if (RetNotFull >= 2) { return true; }//半端マグが2本以上ある
			if (RetNotFull == 1 && !MagInGunFull) { return true; }//半端マグが2本以上ある
			if ((RetEmpty + RetNotFull) >= 1 && m_AmmoStock > 0) { return true; }//半端マグが1本以上あって予備弾もある
			//
			return false;
		}


		void HitBoxControl::UpdataHitBox(const ObjectBaseClass* ptr, float SizeRate) noexcept {
			auto* Ptr = (CharacterClass*)ptr;
			int ID = 0;
			auto headpos = Ptr->GetEyeMatrix().pos();
			m_HitBox[ID].Execute(headpos, 0.13f*Scale_Rate*SizeRate, HitType::Head); ID++;
			m_HitBox[ID].Execute((headpos + Ptr->GetFrameWorldMat(CharaFrame::Upper).pos()) / 2.f, 0.16f*Scale_Rate*SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::Upper).pos(), 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos()) / 2.f, 0.13f * Scale_Rate * SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos()) / 2.f, 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::RightArm)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::RightWrist)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::RightWrist)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::LeftArm)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::LeftWrist)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::LeftWrist)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos()) / 2.f, 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos()*0.25f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos()*0.75f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos()*0.5f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos()*0.5f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos()*0.75f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos()*0.25f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;

			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()) / 2.f, 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos()*0.25f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()*0.75f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos()*0.5f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()*0.5f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos()*0.75f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()*0.25f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
		}

		void AutoAimControl::UpdateAutoAim(bool isActive) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto prev = m_AutoAimTimer;
			m_AutoAimTimer = std::max(m_AutoAimTimer - 1.f / DrawParts->GetFps(), 0.f);
			if (prev > 0.f && m_AutoAimTimer == 0.f) {
				m_AutoAim = -1;
			}
			if (m_AutoAim != -1) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AutoAim).GetChara();
				if (!Chara->IsAlive()) {
					m_AutoAim = -1;
				}
			}

			Easing(&m_AutoAimOn, isActive ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
		}

		void ItemPopControl::ItemFallControl::Init(const std::shared_ptr<BackGroundClassBase>& backGround, const std::string& pPath, ItemType type) {
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& c : m_Ptr) {
				c = std::make_shared<ItemObjClass>();
				ObjMngr->AddObject(c);
				ObjMngr->LoadModel(c, c, pPath.c_str());

				c->SetMapCol(backGround);
				c->SetItemType(type);

				c->Init();
			}
		}
		void ItemPopControl::ItemFallControl::SetFall(const Vector3DX& pPos, const Vector3DX& pVec) {
			this->m_Ptr[this->m_Now]->SetFall(pPos, pVec);
			++this->m_Now %= this->m_Ptr.size();
		}
		void ItemPopControl::ItemFallControl::Dispose() noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& c : m_Ptr) {
				ObjMngr->DelObj((SharedObj*)&c);
				c.reset();
			}
		}
		void ItemPopControl::RepopItem(const std::shared_ptr<BackGroundClassBase>& backGround) {
			DisposeItemPop();
			if (backGround) {
				m_ItemFallControl.at(0).Init(backGround, "data/model/AmmoBox/", ItemType::AMMO);
				m_ItemFallControl.at(1).Init(backGround, "data/model/Armer/", ItemType::ARMER);
			}
		}
		void ItemPopControl::SetPop(const Vector3DX & pPos) {
			auto* DrawParts = DXDraw::Instance();
			int Rand = GetRand(99);
			if (Rand < 45) {
				m_ItemFallControl.at(0).SetFall(pPos, Vector3DX::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / DrawParts->GetFps());
			}
			else if (Rand < 45 + 15) {
				m_ItemFallControl.at(1).SetFall(pPos, Vector3DX::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / DrawParts->GetFps());
			}
			else if (Rand < 45 + 15 + 20) {
				m_ItemFallControl.at(0).SetFall(pPos, Vector3DX::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / DrawParts->GetFps());
				m_ItemFallControl.at(1).SetFall(pPos, Vector3DX::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / DrawParts->GetFps());
			}
		}
		void ItemPopControl::DisposeItemPop() noexcept {
			m_ItemFallControl.at(0).Dispose();
			m_ItemFallControl.at(1).Dispose();
		}
	};
};
