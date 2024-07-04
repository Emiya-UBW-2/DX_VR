#include	"BackGround.hpp"

#include "BackGround.hpp"
const FPS_n2::Sceneclass::BackGroundClassBase* SingletonBase<FPS_n2::Sceneclass::BackGroundClassBase>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {


		bool CheckHitCircleToBox(const Vector3DX& b1, const Vector3DX&  b2, const Vector3DX&  cS, const Vector3DX&  cE, float cR, Vector3DX* NearPos) {
			bool nResult = false;

			Vector3DX bxy;bxy.Set(b1.x, b1.y, 0.f);
			Vector3DX bXy;bXy.Set(b2.x, b1.y, 0.f);
			Vector3DX bxY;bxY.Set(b1.x, b2.y, 0.f);
			Vector3DX bXY;bXY.Set(b2.x, b2.y, 0.f);

			auto GetSegSeg = [&](const Vector3DX& Start, const Vector3DX& End) {
				VECTOR Pos1t = Start.get();
				VECTOR Pos2t = End.get();
				VECTOR PosAt = cS.get();
				VECTOR PosBt = cE.get();

				SEGMENT_SEGMENT_RESULT Result;

				Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, &Result);
				if (Result.SegA_SegB_MinDist_Square <= (cR*cR)) {
					nResult = true;
					if (NearPos) {
						//‚±‚±‚ðC³: cR•ª‚ÍŠµ‚ê‚½Bã‚Ì“_‚É
						*NearPos = Result.SegB_MinDist_Pos;
					}
				}
			};

			GetSegSeg(bxy, bXy);
			GetSegSeg(bXy, bXY);
			GetSegSeg(bXY, bxY);
			GetSegSeg(bxY, bxy);
			return nResult;
		}


		const bool BackGroundClassBase::CheckLinetoMap(const Vector3DX & StartPos, Vector3DX * EndPos, float Radius, Vector3DX * Normal) const noexcept {
			Vector3DX Vec = *EndPos - StartPos;

			float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));
			Vector3DX Start;
			Vector3DX End;
			Vector3DX Ret;

			for (auto& b : m_Blick) {
				for (auto& y : b) {
					if (!y->IsWall) { continue; }
					Start = y->m_Pos; Start.x -= HalfLen; Start.y -= HalfLen;
					End = y->m_Pos; End.x += HalfLen; End.y += HalfLen;
					if (CheckHitCircleToBox(Start,End,StartPos,*EndPos,Radius,&Ret)) {
						*EndPos = Ret;
						return true;
					}
				}
			}
			return false;
		}
		void BackGroundClassBase::Init(const char * MapPath) noexcept {
			int xsize = 64;
			int ysize = 64;

			m_Blick.resize(xsize);
			int x = 0;
			for (auto& b : m_Blick) {
				for (int y = 0;y < ysize;y++) {
					b.emplace_back(std::make_shared<Blick>(x - xsize/2,y - ysize/2));
					if (
						(x == 0 || x == xsize - 1) ||
						(y == 0 || y == ysize - 1)
						) {
						b.back()->IsWall = true;
					}

					if (
						(10 < x && x <= 16) &&
						(10 < y && y <= 16)
						) {
						b.back()->IsWall = true;
					}

					b.back()->m_Pos = b.back()->m_Pos*((64.f * 64.f) / (1080 / 2));
				}
				x++;
			}
		}
		//
		void BackGroundClassBase::Execute(void) noexcept {}
		//
		void BackGroundClassBase::BG_Draw(void) noexcept {}
		void BackGroundClassBase::Draw(void) noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			float Radius = y_r(64.f* 64.f / CamPos.z) / 2;
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					auto DispPos = Convert2DtoDisp(y->m_Pos);
					if (
						(DispPos.x < -Radius || y_r(1920) + Radius < DispPos.x) ||
						(DispPos.y < -Radius || y_r(1080) + Radius < DispPos.y)
						) {
						continue;
					}
					DrawBox(
						(int)(DispPos.x - Radius), (int)(DispPos.y - Radius),
						(int)(DispPos.x + Radius), (int)(DispPos.y + Radius),
						y->IsWall ? Gray75 : Gray25, TRUE);
				}
			}
		}
		void BackGroundClassBase::DrawFront(void) noexcept {}
		//
		void BackGroundClassBase::Dispose(void) noexcept {
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					y.reset();
				}
				b.clear();
			}
			m_Blick.clear();
		}
	};
};

