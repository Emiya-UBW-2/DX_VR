#include	"Character_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void		KeyControl::InitKey(float pxRad, float pyRad) {
			this->m_KeyActive = false;
			this->m_ReadySwitch = false;

			for (int i = 0; i < 4; i++) {
				this->m_Vec[i] = 0.f;
			}
			this->m_Press_GoFront = false;
			this->m_Press_GoRear = false;
			this->m_Press_GoLeft = false;
			this->m_Press_GoRight = false;
			this->m_radAdd.clear();
			//“®ì‚É‚©‚©‚í‚é‘€ì
			this->m_radEasingPer.Set(0.5f, 0.2f, 0.5f);
			this->m_rad_Buf.x(pxRad);
			this->m_rad_Buf.y(pyRad);
			//ã‹L‚ð”½‰f‚·‚é‚à‚Ì
			this->m_rad = this->m_rad_Buf;

			this->m_Squat.Set(false);
			this->m_Run.Set(false);
		}
		void		KeyControl::InputKey(const InputControl& pInput, bool pReady, const VECTOR_ref& pAddRadvec) {
			this->m_ReadySwitch = (this->m_KeyActive != pReady);
			this->m_KeyActive = pReady;
			if (!m_KeyActive) { return; }
			//’n
			{
				float pAddxRad = pInput.GetAddxRad()*(pInput.GetRunPress() ? 0.5f : 1.f);
				float pAddyRad = pInput.GetAddyRad()*(pInput.GetRunPress() ? 0.5f : 1.f);

				this->m_Press_GoFront = pInput.GetGoFrontPress() && m_KeyActive;
				this->m_Press_GoRear = pInput.GetGoBackPress() && m_KeyActive;
				this->m_Press_GoLeft = pInput.GetGoLeftPress() && m_KeyActive;
				this->m_Press_GoRight = pInput.GetGoRightPress() && m_KeyActive;

				m_QKey.Execute(pInput.GetQPress() && m_KeyActive);
				m_EKey.Execute(pInput.GetEPress() && m_KeyActive);
				m_RKey.Execute(pInput.GetAction2() && m_KeyActive);
				m_FKey.Execute(pInput.GetAction3() && m_KeyActive);
				m_ShotKey.Execute(pInput.GetAction5() && m_KeyActive);
				m_ADSKey.Execute(pInput.GetAction6() && m_KeyActive);
				this->m_Run.Execute(pInput.GetRunPress() && m_KeyActive);
				this->m_Action.Execute(pInput.GetAction1() && m_KeyActive);
				this->m_Squat.Execute(pInput.GetAction4() && m_KeyActive);
				if (this->GetRun()) { this->m_Squat.Set(false); }
				//‰ñ“]
				{
					//auto limchange = 1.f;
					Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

					this->m_rad_Buf.x(
						std::clamp(this->m_rad_Buf.x() + pAddxRad, deg2rad(-70.f), deg2rad(24.f))
						+ this->m_radAdd.x()
					);
					this->m_rad_Buf.y(
						this->m_rad_Buf.y() + (pAddyRad)
						+this->m_radAdd.y()
					);
					{
						auto Buf = this->m_rad.x();
						Easing(&Buf, m_rad_Buf.x(), this->m_radEasingPer.x(), EasingType::OutExpo);
						this->m_rad.x(Buf);
					}
					{
						auto Buf = this->m_rad.y();
						Easing(&Buf, m_rad_Buf.y(), this->m_radEasingPer.y(), EasingType::OutExpo);
						this->m_rad.y(Buf);
					}
					{
						auto Buf = this->m_rad.z();
						Easing(&Buf, m_rad_Buf.z(), this->m_radEasingPer.z(), EasingType::OutExpo);
						this->m_rad.z(Buf);
					}
				}
			}
			SetVec(0, this->m_Press_GoFront);
			SetVec(1, this->m_Press_GoLeft);
			SetVec(2, this->m_Press_GoRear);
			SetVec(3, this->m_Press_GoRight);
			{
				if (true) {//ƒgƒOƒ‹Ž®
					if (this->m_QKey.trigger()) {
						if (this->m_LeanRate == -1) {
							this->m_LeanRate = 1;
						}
						else {
							if (this->m_LeanRate < 1) {
								this->m_LeanRate++;
							}
							else {
								this->m_LeanRate--;
							}
						}
					}
					if (this->m_EKey.trigger()) {
						if (this->m_LeanRate == 1) {
							this->m_LeanRate = -1;
						}
						else {
							if (this->m_LeanRate > -1) {
								this->m_LeanRate--;
							}
							else {
								this->m_LeanRate++;
							}
						}
					}
				}
				else {
					this->m_LeanRate = 0;
					if (this->m_QKey.press()) {
						this->m_LeanRate = 1;
					}
					if (this->m_EKey.press()) {
						this->m_LeanRate = -1;
					}
				}
				this->m_LeanRate = std::clamp(this->m_LeanRate, -1, 1);
			}
		}
	};
};
