#include	"Base2DObject.hpp"
#include	"Object2DManager.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void Base2DObject::AddBlur(float Blur) noexcept {
			auto* DrawParts = DXDraw::Instance();
			int Max = (int)std::max(1.f, 300.f / std::max(30.f, DrawParts->GetFps()));
			for (int i = 0; i < Max; i++) {
				m_Blur.at(m_BlurNow).Pos = Lerp(this->m_Pos-this->m_Vec * (Tile_DispSize / DrawParts->GetFps()), this->m_Pos, ((float)i / (float)Max));
				m_Blur.at(m_BlurNow).TimeMax = Blur;
				m_Blur.at(m_BlurNow).Time = m_Blur.at(m_BlurNow).TimeMax;
				++m_BlurNow %= (int)m_Blur.size();
			}
		}
		void Base2DObject::Execute() noexcept {
			auto* Obj2DParts = Object2DManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			// ブラー
			for (auto& b : m_Blur) {
				b.Time = std::max(b.Time - 1.f / DrawParts->GetFps(), 0.f);
			}
			//
			Execute_Sub();
			// オブジェクト判定
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Object)) {
				Obj2DParts->CheckColObject(this);
			}
			//何かに当たった
			if (this->m_HitObjectID != -1) {
				Execute_OnHitObject();
				this->m_HitObjectID = -1;
			}
		}
		void Base2DObject::ExecuteAfter() noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			// 衝突込みの演算
			// 壁判定
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Wall)) {
				bool IsHit = false;
				if (m_HitTarget == HitTarget::Physical) {
					int Max = (int)std::max(1.f, 60.f / std::max(30.f, DrawParts->GetFps()));
					for (int i = 0; i < Max; i++) {
						m_Pos += m_Vec * ((1.f / (float)Max) * Tile_DispSize / DrawParts->GetFps());
						IsHit |= BackGround->CheckLinetoMap(m_PrevPos, &m_Pos, Get2DSize(GetSize() / 2.f), true);
						m_PrevPos = m_Pos;
					}
				}
				else {
					m_Pos += m_Vec * (Tile_DispSize / DrawParts->GetFps());
					IsHit |= BackGround->CheckLinetoMap(m_PrevPos, &m_Pos, Get2DSize(GetSize() / 2.f), false);
					m_PrevPos = m_Pos;
				}
				if (IsHit) {
					Execute_OnHitWall();
				}
			}
			else {
				m_Pos += m_Vec * (Tile_DispSize / DrawParts->GetFps());
				m_PrevPos = m_Pos;
			}
		}
	};
};
