#include	"sub.hpp"

const FPS_n2::FadeControl* SingletonBase<FPS_n2::FadeControl>::m_Singleton = nullptr;
const FPS_n2::HitMarkerPool* SingletonBase<FPS_n2::HitMarkerPool>::m_Singleton = nullptr;
const FPS_n2::EffectSingleton* SingletonBase<FPS_n2::EffectSingleton>::m_Singleton = nullptr;

namespace FPS_n2 {
	void FadeControl::Init(void) noexcept {
		this->m_IsBlackOut = false;
		this->m_BlackOutAlpha = 1.0f;
		this->m_BlackSpeed = 0.5f;
	}
	void FadeControl::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.0f : -1.0f) * DXLib_refParts->GetDeltaTime() / this->m_BlackSpeed, 0.0f, 1.0f);
	}
	void FadeControl::Draw(void) const noexcept {
		if (this->m_BlackOutAlpha <= 1.0f / 255.0f) { return; }
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.0f * this->m_BlackOutAlpha), 0, 255));
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Black, true);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
	}
}
