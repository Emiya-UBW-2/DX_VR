#include	"sub.hpp"

const FPS_n2::HitMarkerPool* SingletonBase<FPS_n2::HitMarkerPool>::m_Singleton = nullptr;
const FPS_n2::EffectSingleton* SingletonBase<FPS_n2::EffectSingleton>::m_Singleton = nullptr;

namespace FPS_n2 {
	void FadeControl::Init(void) noexcept {
		this->m_IsBlackOut = false;
		this->m_BlackOutAlpha = 1.f;
	}
	void FadeControl::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.f : -1.f) * DXLib_refParts->GetDeltaTime() / 0.5f, 0.f, 1.f);
	}
	void FadeControl::Draw(void) const noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Black, true);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
	}
}
