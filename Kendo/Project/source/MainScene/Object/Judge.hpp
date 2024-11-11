#pragma once
#include	"../../Header.hpp"
#include	"JudgeEnum.hpp"

#include	"../../MainScene/Player/Player.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		// ベクトルと上から右回りの角度を取得
		static inline float GetRadVec(const Vector2DX& vec1) noexcept { return std::atan2f(vec1.x, vec1.y); }
		//角度差計算(360度回る際の演算などで使用します)
		static inline float GetRadRad2Rad(float NowRadian, float TargetRadian) noexcept {
			Vector2DX Vec; Vec.Set(std::sin(NowRadian), std::cos(NowRadian));
			Vector2DX vec_a; vec_a.Set(std::sin(TargetRadian), std::cos(TargetRadian));
			float cost = Vector2DX::Cross(vec_a, Vec);
			float sint = sqrtf(std::abs(1.f - cost * cost));
			return GetRadVec(Vector2DX::vget(cost, sint));
		}

		class JudgeClass : public ObjectBaseClass {
		private:
			Vector3DX											m_TargetPos;
			Vector3DX											m_CenterPos;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
			float												m_TargetRad{ 0.f };									//上半身を向けるべき方向
			float												m_BodyRad{ 0.f };									//身体の回転方向
			float												m_BottomRad{ 0.f };
			std::array<float, static_cast<int>(JudgeAnimeID::AnimeIDMax)>	m_AnimPerBuf{ 0 };
			bool												m_TurnBody{ false };
			float												m_XScale{ 1.f };
			bool												m_RedWin{ false };
			bool												m_WhiteWin{ false };
			float												m_Speed{ 0.f };
		public: //コンストラクタ、デストラクタ
			JudgeClass(void) noexcept {
				this->m_objType = static_cast<int>(ObjType::Judge);
				this->m_IsDraw = true;
			}
			~JudgeClass(void) noexcept {}
		private:
			int	GetFrameNum() noexcept override { return static_cast<int>(JudgeFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return JudgeFrameName[id]; }
		public:
			void			MovePoint(float pyRad, const Vector3DX& pPos) noexcept {
				for (int i = 0; i < 4; i++) { this->m_Vec[i] = 0.f; }
				this->m_TargetRad = pyRad;
				this->m_BodyRad = this->m_TargetRad;
				this->m_BottomRad = this->m_TargetRad;
				this->m_TurnBody = false;
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
				this->m_AnimPerBuf[static_cast<int>(JudgeAnimeID::Upper_Ready)] = 1.f;
				SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_BottomRad), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_BottomRad));
				m_RedWin = false;
				m_WhiteWin = false;
			}
			void			SetInput(const Vector3DX& pTargetPos, const Vector3DX& pCenterPos) noexcept {
				m_TargetPos = pTargetPos;
				m_CenterPos = pCenterPos;
			}
			void			SetWin(bool RedWin, bool WhiteWin) noexcept {
				m_RedWin = RedWin;
				m_WhiteWin = WhiteWin;
			}
		private: //継承
			void			Init_Sub(void) noexcept override {}
			void			FirstExecute(void) noexcept override;
			void			CheckDraw(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			DrawShadow(void) noexcept override;
			void			Dispose_Sub(void) noexcept override {}
		};
	};
};
