#include	"BackGround.hpp"

#include "BackGround.hpp"
const FPS_n2::Sceneclass::BackGroundClassBase* SingletonBase<FPS_n2::Sceneclass::BackGroundClassBase>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {

		const bool BackGroundClassBase::CheckLinetoMap(const Vector3DX & StartPos, Vector3DX * EndPos, float Radius, Vector3DX * Normal) const noexcept {
			Vector3DX EndPt = *EndPos;
			float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));

			Vector3DX Min, Max;

			Min.x = std::min(EndPt.x, StartPos.x) - HalfLen * 3;
			Min.y = std::min(EndPt.y, StartPos.y) - HalfLen * 3;

			Max.x = std::max(EndPt.x, StartPos.x) + HalfLen * 3;
			Max.y = std::max(EndPt.y, StartPos.y) + HalfLen * 3;


			Vector3DX Start;
			Vector3DX End;
			for (auto& b : m_Blick) {
				for (auto& y : b) {
					if (!y->IsWall) { continue; }

					if (!(
						(Min.x < y->m_Pos.x && y->m_Pos.x < Max.x) &&
						(Min.y < y->m_Pos.y && y->m_Pos.y < Max.y)
						)) { continue; }

					Start = y->m_Pos; Start.x -= HalfLen; Start.y -= HalfLen;
					End = y->m_Pos; End.x += HalfLen; End.y += HalfLen;
					Vector3DX Box[4];
					
					Box[0].Set(Start.x, Start.y, 0.f);
					Box[1].Set(End.x, Start.y, 0.f);
					Box[2].Set(End.x, End.y, 0.f);
					Box[3].Set(Start.x, End.y, 0.f);

					auto GetSegSeg = [&](const Vector3DX& StartP, const Vector3DX& EndP, const Vector3DX& In,Vector3DX* Ret,float* Len) {
						//外向きなのでノーヒット
						if (Vector3DX::Dot(Vector3DX::Cross(EndP - StartP, In - StartP), Vector3DX::Cross(EndP - StartP, EndPt - StartPos)) < 0.f) {
							return false;
						}
						VECTOR Pos1t = StartP.get();
						VECTOR Pos2t = EndP.get();
						VECTOR PosAt = StartPos.get();
						VECTOR PosBt = EndPt.get();
						SEGMENT_SEGMENT_RESULT Result;
						Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, &Result);
						*Len = std::sqrt(Result.SegA_SegB_MinDist_Square);
						if (*Len < Radius) {
							//法線ベクトルを求める
							Vector3DX Normal = Vector3DX::Cross(EndP - StartP, Vector3DX::Cross(EndP - StartP, EndPt - StartPos)).normalized();
							//ずり
							*Ret = Result.SegB_MinDist_Pos;
							*Ret += Normal * (Radius - *Len);
							return true;
						}
						return false;
					};
					//
					Vector3DX Rv1;
					float Rfl;
					int sel = -1;
					float Rf = 10000.f;
					for (int i = 0;i < 4;i++) {
						y->HIT[i] = false;
						if (GetSegSeg(Box[i], Box[(i + 1) % 4], Box[(i + 3) % 4], &Rv1, &Rfl)) {
							y->HIT[i] = true;
							if (Rf > Rfl) {
								Rf = Rfl;
								sel = i;
								*EndPos = Rv1;
							}
						}
					}
					if (sel != -1) {
						//return true;
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

					if (
						(20 < x && x <= 21) &&
						(20 < y && y <= 21)
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
						(int)(DispPos.x - Radius), (int)(DispPos.y + Radius),
						(int)(DispPos.x + Radius), (int)(DispPos.y - Radius),
						y->IsWall ? Gray75 : Gray25, TRUE);
					if (y->HIT[0]) {
						DrawLine(
							(int)(DispPos.x - Radius), (int)(DispPos.y + Radius),
							(int)(DispPos.x + Radius), (int)(DispPos.y + Radius),
							Red, 3);
					}
					if (y->HIT[1]) {
						DrawLine(
							(int)(DispPos.x + Radius), (int)(DispPos.y + Radius),
							(int)(DispPos.x + Radius), (int)(DispPos.y - Radius),
							Red, 3);
					}
					if (y->HIT[2]) {
						DrawLine(
							(int)(DispPos.x + Radius), (int)(DispPos.y - Radius),
							(int)(DispPos.x - Radius), (int)(DispPos.y - Radius),
							Red, 3);
					}
					if (y->HIT[3]) {
						DrawLine(
							(int)(DispPos.x - Radius), (int)(DispPos.y - Radius),
							(int)(DispPos.x - Radius), (int)(DispPos.y + Radius),
							Red, 3);
					}
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

