#include	"sub.hpp"

const FPS_n2::HitMark* SingletonBase<FPS_n2::HitMark>::m_Singleton = nullptr;

namespace FPS_n2 {
	void HitMark::Load(void) noexcept {
		this->MenGraph.Load("data/UI/hit_Men.bmp");
		this->KoteGraph.Load("data/UI/hit_Kote.bmp");
		this->DoGraph.Load("data/UI/hit_Do.bmp");
	}
	void HitMark::Set(void) noexcept {
		for (auto& h : m_HitPos) {
			h.Time = -1.f;
		}
	}
	void HitMark::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		for (auto& h : m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.Time == h.TimeMax || h.m_Pos2D.z == -100.f) {
				auto tmp = ConvWorldPosToScreenPos(h.m_Pos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					h.m_Pos2D = tmp;
				}
			}
			h.Time = std::max(h.Time - DXLib_refParts->GetDeltaTime(), 0.f);
		}
	}
	void HitMark::Draw(void) noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		for (auto& h : m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.m_Pos2D.z >= 0.f && h.m_Pos2D.z <= 1.f) {
				float		Per = std::clamp(1.f - h.Time / h.TimeMax, 0.f, 1.f);
				int			Alpha = std::clamp(static_cast<int>(std::sin(Per * 2.f * DX_PI_F) * h.m_Per * 255.f), 0, 255);
				float		Scale = Per * 10.f * h.m_Per;
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
				switch (h.m_Color) {
				case HitType::Head://–Ê
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&MenGraph, static_cast<int>(h.m_Pos2D.x), static_cast<int>(h.m_Pos2D.y), static_cast<float>(static_cast<int>(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Arm://¬Žè
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 128, 0);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&KoteGraph, static_cast<int>(h.m_Pos2D.x), static_cast<int>(h.m_Pos2D.y), static_cast<float>(static_cast<int>(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Body://“·
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 0);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&DoGraph, static_cast<int>(h.m_Pos2D.x), static_cast<int>(h.m_Pos2D.y), static_cast<float>(static_cast<int>(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Leg:
				default:
					break;
				}

			}
		}
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
	}
	void HitMark::Dispose(void) noexcept {
		this->MenGraph.Dispose();
		this->KoteGraph.Dispose();
		this->DoGraph.Dispose();
	}
};
