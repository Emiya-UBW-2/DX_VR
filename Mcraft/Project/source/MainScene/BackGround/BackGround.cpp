#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		class PerlinNoise {
		private:
			//メンバ変数
			std::array<uint_fast8_t, 512> p{ 0 };

			constexpr float getFade(const float t_) const noexcept {
				return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10);
			}
			constexpr float getLerp(const float t_, const float a_, const float b_) const noexcept {
				return a_ + t_ * (b_ - a_);
			}
			constexpr float makeGrad(const uint_fast8_t hash_, const float u_, const float v_) const noexcept {
				return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_);
			}
			constexpr float makeGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return this->makeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
			}
			constexpr float getGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return this->makeGrad(hash_ & 15, x_, y_, z_);
			}

			float setNoise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				const std::size_t x_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x_)) & 255);
				const std::size_t y_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y_)) & 255);
				const std::size_t z_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z_)) & 255);
				x_ -= std::floor(x_);
				y_ -= std::floor(y_);
				z_ -= std::floor(z_);
				const float u = this->getFade(x_);
				const float v = this->getFade(y_);
				const float w = this->getFade(z_);
				const std::size_t a0 = static_cast<std::size_t>(this->p[x_int] + y_int);
				const std::size_t a1 = static_cast<std::size_t>(this->p[a0] + z_int);
				const std::size_t a2 = static_cast<std::size_t>(this->p[a0 + 1] + z_int);
				const std::size_t b0 = static_cast<std::size_t>(this->p[x_int + 1] + y_int);
				const std::size_t b1 = static_cast<std::size_t>(this->p[b0] + z_int);
				const std::size_t b2 = static_cast<std::size_t>(this->p[b0 + 1] + z_int);

				return this->getLerp(w,
					this->getLerp(v,
						this->getLerp(u, this->getGrad(this->p[a1], x_, y_, z_), this->getGrad(this->p[b1], x_ - 1, y_, z_)),
						this->getLerp(u, this->getGrad(this->p[a2], x_, y_ - 1, z_), this->getGrad(this->p[b2], x_ - 1, y_ - 1, z_))),
					this->getLerp(v,
						this->getLerp(u, this->getGrad(this->p[a1 + 1], x_, y_, z_ - 1), this->getGrad(this->p[b1 + 1], x_ - 1, y_, z_ - 1)),
						this->getLerp(u, this->getGrad(this->p[a2 + 1], x_, y_ - 1, z_ - 1), this->getGrad(this->p[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
			}
			float setOctaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				float noise_value = 0.f;
				float amp = 1.0f;
				for (std::size_t i = 0; i < octaves_; ++i) {
					noise_value += this->setNoise(x_, y_, z_) * amp;
					x_ *= 2.0f;
					y_ *= 2.0f;
					z_ *= 2.0f;
					amp *= 0.5f;
				}
				return noise_value;
			}
		public:
			constexpr PerlinNoise() = default;
			explicit PerlinNoise(const std::uint_fast32_t seed_) noexcept {
				this->setSeed(seed_);
			}

			//SEED値を設定する
			void setSeed(const std::uint_fast32_t seed_) noexcept {
				for (std::size_t i = 0; i < 256; ++i)
					this->p[i] = static_cast<uint_fast8_t>(i);
				std::shuffle(this->p.begin(), this->p.begin() + 256, std::default_random_engine(seed_));
				for (std::size_t i = 0; i < 256; ++i) {
					this->p[256 + i] = this->p[i];
				}
			}
			//オクターブ無しノイズを取得する
			float noise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return this->setNoise(x_, y_, z_) * 0.5f + 0.5f;
			}
			//オクターブ有りノイズを取得する
			float octaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return this->setOctaveNoise(octaves_, x_, y_, z_) * 0.5f + 0.5f;
			}
		};
		//
		void		BackGroundClass::AddPlane(const CellsData& cellx, int x, int y, int z, COLOR_U8 DifColor, COLOR_U8 SpcColor, int xscale,
			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) noexcept {
			if (m_32Num + 1 > m_32Size) {
				m_32Size = m_32Num + 1;
				m_vert32.resize(m_32Size * 4);
				m_index32.resize(m_32Size * 6);
			}

			float Xofs = static_cast<float>((z % 2 == 0));
			float Yofs = static_cast<float>(1 - yN);
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;
			float uMin = Xofs * uAdd;
			float vMin = Yofs * vAdd;

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				Vector3DX Pos;
				if (xN > 0) {
					Pos = cellx.GetPos(x + 1, y + xt, z + yt * xscale);//
				}
				else if (yN > 0) {
					Pos = cellx.GetPos(x + xt, y + 1, z + yt * xscale);//
				}
				else if (zN > 0) {
					Pos = cellx.GetPos(x + xt, y + yt, z + 1 * xscale);
				}
				else if (xN < 0) {
					Pos = cellx.GetPos(x + 0, y + xt, z + yt * xscale);
				}
				else if (yN < 0) {
					Pos = cellx.GetPos(x + xt, y + 0, z + yt * xscale);
				}
				else if (zN < 0) {
					Pos = cellx.GetPos(x + xt, y + yt, z + 0 * xscale);
				}

				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = Pos.get();
				V.u = uMin + uAdd * static_cast<float>(u * cellx.scaleRate);
				V.v = vMin + vAdd * static_cast<float>(v);// * cellx.scaleRate
				V.norm = VGet(static_cast<float>(xN), static_cast<float>(yN), static_cast<float>(zN));
				V.dif = DifColor;
				V.spc = SpcColor;
				};

			AddPointPos(x1, y1, xscale, 0, 0);
			AddPointPos(x2, y2, 0, 0, 1);
			AddPointPos(x3, y3, xscale, 1, 2);
			AddPointPos(x4, y4, 0, 1, 3);
			m_index32.at(m_32Num * 6) = (uint32_t)(m_32Num * 4);
			m_index32.at(m_32Num * 6 + 1) = (uint32_t)(m_32Num * 4 + 1);
			m_index32.at(m_32Num * 6 + 2) = (uint32_t)(m_32Num * 4 + 2);
			m_index32.at(m_32Num * 6 + 3) = (uint32_t)(m_32Num * 4 + 3);
			m_index32.at(m_32Num * 6 + 4) = (uint32_t)(m_32Num * 4 + 2);
			m_index32.at(m_32Num * 6 + 5) = (uint32_t)(m_32Num * 4 + 1);
			++m_32Num;
		}
		void BackGroundClass::AddCubeZ(const CellsData& cellx, int x, int y, int zmin, int zmax, int centerX, int centerY, int centerZ) noexcept {
			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);
			bool isHit1 = false;
			bool isHit2 = false;
			bool isHit3 = false;
			bool isHit4 = false;
			for (int z = zmin; z <= zmax; ++z) {
				bool checkFill = true;
				if (cellx.scaleRate != 1) {
					int xMinmin = centerX - DrawMax / MulPer;
					int xMinmax = centerX + DrawMax / MulPer;
					int yMinmin = centerY - DrawMax / MulPer;
					int yMinmax = centerY + DrawMax / MulPer;
					int zMinmin = centerZ - DrawMax / MulPer;
					int zMinmax = centerZ + DrawMax / MulPer;
					checkFill = !(((xMinmin <= x) && (x <= xMinmax)) && ((yMinmin <= y) && (y <= yMinmax)) && ((zMinmin <= z) && (z <= zMinmax)));
				}
				auto& fillinfo = cellx.GetOcclusionInfo(x, y, z);
				// 頂点データの作成
				if (!isHit1 && (x + 1 <= centerX) && !(checkFill && (fillinfo & (1 << 0)))) {
					isHit1 = true;
				}
				if (!isHit2 && (x >= centerX) && !(checkFill && (fillinfo & (1 << 1)))) {
					isHit2 = true;
				}
				if (!isHit3 && (y + 1 <= centerY) && !(checkFill && (fillinfo & (1 << 2)))) {
					isHit3 = true;
				}
				if (!isHit4 && (y >= centerY) && !(checkFill && (fillinfo & (1 << 3)))) {
					isHit4 = true;
				}
				if (z == zmax) {
					if (isHit1) {
						AddPlane(cellx, x, y, zmin, DifColor, SpcColor,
							zmax - zmin + 1,
							0, 1,
							0, 0,
							1, 1,
							1, 0,
							1, 0, 0);
					}
					if (isHit2) {
						AddPlane(cellx, x, y, zmin, DifColor, SpcColor,
							zmax - zmin + 1,
							1, 1,
							1, 0,
							0, 1,
							0, 0,
							-1, 0, 0);
					}
					if (isHit3) {
						AddPlane(cellx, x, y, zmin, DifColor, SpcColor,
							zmax - zmin + 1,
							1, 1,
							1, 0,
							0, 1,
							0, 0,
							0, 1, 0);
					}
					if (isHit4) {
						AddPlane(cellx, x, y, zmin, DifColor, SpcColor,
							zmax - zmin + 1,
							0, 1,
							0, 0,
							1, 1,
							1, 0,
							0, -1, 0);
					}
				}
				//こいつらはそのまま
				if (z + 1 <= centerZ && !(checkFill && (fillinfo & (1 << 4)))) {
					AddPlane(cellx, x, y, z, DifColor, SpcColor,
						1,
						0, 0,
						1, 0,
						0, 1,
						1, 1,
						0, 0, 1);
				}
				if (z >= centerZ && !(checkFill && (fillinfo & (1 << 5)))) {
					AddPlane(cellx, x, y, z, DifColor, SpcColor,
						1,
						0, 1,
						1, 1,
						0, 0,
						1, 0,
						0, 0, -1);
				}
			}
		}
		void		BackGroundClass::AddCubes(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			float scale = CellScale * cellx.scaleRate;
			Vector3DX mid = Vector3DX::one() * (0.5f * scale);
			int xMaxmin = std::max(centerX - DrawMax, 0);
			int xMaxmax = std::min(centerX + DrawMax + 1, cellx.Xall);
			int yMaxmin = std::max(centerY - DrawMax, 0);
			int yMaxmax = std::min(centerY + DrawMax + 1, cellx.Yall);
			int zMaxmin = std::max(centerZ - DrawMax, 0);
			int zMaxmax = std::min(centerZ + DrawMax + 1, cellx.Zall);

			int xMinmin = centerX - DrawMax / MulPer;
			int xMinmax = centerX + DrawMax / MulPer;
			int yMinmin = centerY - DrawMax / MulPer;
			int yMinmax = centerY + DrawMax / MulPer;
			int zMinmin = centerZ - DrawMax / MulPer;
			int zMinmax = centerZ + DrawMax / MulPer;
			for (int x = xMaxmin; x < xMaxmax; ++x) {
				if (!CalcYZActive(cellx, x, yMaxmin, yMaxmax, zMaxmin, zMaxmax)) { continue; }
				for (int y = yMaxmin; y < yMaxmax; ++y) {
					int zMaxminT = zMaxmin;
					int zMaxmaxT = zMaxmax;
					if (!CalcZMinMax(cellx, x, y, &zMaxminT, &zMaxmaxT)) { continue; }
					bool OnNearestPoint = ((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax));
					int zmin = zMaxminT;
					int zmax = zMaxminT;
					bool isHitmin = true;
					for (int z = zMaxminT; z < zMaxmaxT; ++z) {
						if ((cellx.scaleRate != 1) && OnNearestPoint && ((zMinmin < z) && (z < zMinmax))) {
							//スキップ
							z = zMinmax - 1;
						}
						else if (!cellx.IsActiveCell(x, y, z) || (cellx.GetOcclusionInfo(x, y, z) == 0b111111) || (z == zMaxmaxT - 1)) {}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = z;
							}
							zmax = z;
							continue;
						}
						if (!isHitmin) {
							isHitmin = true;
							AddCubeZ(cellx, x, y, zmin, zmax, centerX, centerY, centerZ);
						}
					}
				}
			}
		}
		//
		void BackGroundClass::AddShadowPlane(const CellsData& cellx, int x, int y, int z, int xscale,
			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) noexcept {
			if (m_S32Num + 1 > m_S32Size) {
				m_S32Size = m_S32Num + 1;
				m_vert32S.resize(m_S32Size * 4);
				m_vert32SB.resize(m_S32Size * 4);
				m_index32S.resize(m_S32Size * 6);
			}

			auto AddPointPos = [&](int xt, int yt, int sel) {
				Vector3DX Pos;
				if (xN > 0) {
					Pos = cellx.GetPos(x + 1, y + xt, z + yt * xscale);//
				}
				else if (yN > 0) {
					Pos = cellx.GetPos(x + xt, y + 1, z + yt * xscale);//
				}
				else if (zN > 0) {
					Pos = cellx.GetPos(x + xt, y + yt, z + 1 * xscale);
				}
				else if (xN < 0) {
					Pos = cellx.GetPos(x + 0, y + xt, z + yt * xscale);
				}
				else if (yN < 0) {
					Pos = cellx.GetPos(x + xt, y + 0, z + yt * xscale);
				}
				else if (zN < 0) {
					Pos = cellx.GetPos(x + xt, y + yt, z + 0 * xscale);
				}

				m_vert32S.at(m_S32Num * 4 + sel).pos = Pos.get();
				auto& V = m_vert32SB.at(m_S32Num * 4 + sel);
				V.pos = Pos.get();
				V.norm = VGet(static_cast<float>(xN), static_cast<float>(yN), static_cast<float>(zN));
				V.dif = GetColorU8(128, 128, 128, 255);
				};

			AddPointPos(x1, y1, 0);
			AddPointPos(x2, y2, 1);
			AddPointPos(x3, y3, 2);
			AddPointPos(x4, y4, 3);
			m_index32S.at(m_S32Num * 6) = (uint32_t)(m_S32Num * 4);
			m_index32S.at(m_S32Num * 6 + 1) = (uint32_t)(m_S32Num * 4 + 1);
			m_index32S.at(m_S32Num * 6 + 2) = (uint32_t)(m_S32Num * 4 + 2);
			m_index32S.at(m_S32Num * 6 + 3) = (uint32_t)(m_S32Num * 4 + 3);
			m_index32S.at(m_S32Num * 6 + 4) = (uint32_t)(m_S32Num * 4 + 2);
			m_index32S.at(m_S32Num * 6 + 5) = (uint32_t)(m_S32Num * 4 + 1);
			++m_S32Num;
		}
		void BackGroundClass::AddShadowCubeZ(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			bool isHit1 = false;
			bool isHit2 = false;
			bool isHit3 = false;
			bool isHit4 = false;
			for (int z = zmin; z <= zmax; ++z) {
				auto& fillinfo = cellx.GetOcclusionInfo(x, y, z);
				// 頂点データの作成
				if (!isHit1 && (0 <= lightX && (fillinfo & (1 << 0)) == 0)) {
					isHit1 = true;
				}
				if (!isHit2 && (0 >= lightX && (fillinfo & (1 << 1)) == 0)) {
					isHit2 = true;
				}
				if (!isHit3 && (0 <= lightY && (fillinfo & (1 << 2)) == 0)) {
					isHit3 = true;
				}
				if (!isHit4 && (0 >= lightY && (fillinfo & (1 << 3)) == 0)) {
					isHit4 = true;
				}
				if (z == zmax) {
					if (isHit1) {
						AddShadowPlane(cellx, x, y, zmin,
							zmax - zmin + 1,
							1, 0,
							1, 1,
							0, 0,
							0, 1,
							1, 0, 0);
					}
					if (isHit2) {
						AddShadowPlane(cellx, x, y, zmin,
							zmax - zmin + 1,
							1, 1,
							1, 0,
							0, 1,
							0, 0,
							-1, 0, 0);
					}
					if (isHit3) {
						AddShadowPlane(cellx, x, y, zmin,
							zmax - zmin + 1,
							0, 1,
							1, 1,
							0, 0,
							1, 0,
							0, 1, 0);
					}
					if (isHit4) {
						AddShadowPlane(cellx, x, y, zmin,
							zmax - zmin + 1,
							0, 0,
							1, 0,
							0, 1,
							1, 1,
							0, -1, 0);
					}
				}
				//こいつらはそのまま
				if (0 <= lightZ && (fillinfo & (1 << 4)) == 0) {
					AddShadowPlane(cellx, x, y, z,
						1,
						0, 0,
						1, 0,
						0, 1,
						1, 1,
						0, 0, 1);
				}
				if (0 >= lightZ && (fillinfo & (1 << 5)) == 0) {
					AddShadowPlane(cellx, x, y, z,
						1,
						0, 1,
						1, 1,
						0, 0,
						1, 0,
						0, 0, -1);
				}
			}
		}
		void		BackGroundClass::AddShadowCubes(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			int xMaxmin = std::max(centerX - ShadowMax, 0);
			int xMaxmax = std::min(centerX + ShadowMax + 1, cellx.Xall);
			int yMaxmin = std::max(centerY - ShadowMax, 0);
			int yMaxmax = std::min(centerY + ShadowMax + 1, cellx.Yall);
			int zMaxmin = std::max(centerZ - ShadowMax, 0);
			int zMaxmax = std::min(centerZ + ShadowMax + 1, cellx.Zall);

			int xMinmin = centerX - ShadowMax / MulPer;
			int xMinmax = centerX + ShadowMax / MulPer;
			int yMinmin = centerY - ShadowMax / MulPer;
			int yMinmax = centerY + ShadowMax / MulPer;
			int zMinmin = centerZ - ShadowMax / MulPer;
			int zMinmax = centerZ + ShadowMax / MulPer;
			for (int x = xMaxmin; x < xMaxmax; ++x) {
				for (int y = yMaxmin; y < yMaxmax; ++y) {
					bool OnNearestPoint = ((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax));

					int zmin = zMaxmin;
					int zmax = zMaxmin;
					bool isHitmin = true;
					for (int z = zMaxmin; z < zMaxmax; ++z) {
						if ((cellx.scaleRate != 1) && OnNearestPoint && ((zMinmin < z) && (z < zMinmax))) {
							//スキップ
							z = zMinmax - 1;
						}
						else if (!cellx.IsActiveCell(x, y, z) || (cellx.GetOcclusionInfo(x, y, z) == 0b111111) || (z == zMaxmax - 1)) {}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = z;
							}
							zmax = z;
							continue;
						}
						if (!isHitmin) {
							isHitmin = true;
							AddShadowCubeZ(cellx, x, y, zmin, zmax);
						}
					}
				}
			}
		}
		//
		bool		BackGroundClass::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			auto& cell = m_CellxN.front();
			float scale = CellScale * cell.scaleRate;
			Vector3DX Start = StartPos / scale;
			Vector3DX End = StartPos / scale;

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							static_cast<int>(Start.x + xm + cell.Xall / 2), static_cast<int>(Start.y + ym + cell.Yall / 2), static_cast<int>(Start.z + zm + cell.Zall / 2),
							static_cast<int>(End.x + xm + cell.Xall / 2), static_cast<int>(End.y + ym + cell.Yall / 2), static_cast<int>(End.z + zm + cell.Zall / 2),
							[&](int x, int y, int z) {
							if (!cell.isInside(x, y, z)) { return false; }
							if (!cell.IsActiveCell(x, y, z)) { return false; }
							if (!(cell.GetOcclusionInfo(x, y, z) != 0b111111)) { return false; }
							Vector3DX MinPos = cell.GetPos(x, y, z);
							Vector3DX MaxPos = cell.GetPos(x + 1, y + 1, z + 1);
							if (ColRayBox(StartPos, EndPos, MinPos, MaxPos, Normal)) {
								isHit = true;
								return true;
							}
							return false;
							});
						if (isHit) { return true; }
					}
				}
			}
			return false;
		}
		//
		bool		BackGroundClass::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept {
			auto MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			auto& cell = m_CellxN.front();
			float scale = CellScale * cell.scaleRate;
			Vector3DX Start = StartPos / scale;
			Vector3DX End = StartPos / scale;

			for (int xm = -2; xm <= 2; ++xm) {
				for (int ym = 2; ym <= 2; ++ym) {
					for (int zm = -2; zm <= 2; ++zm) {
						Bresenham3D(
							static_cast<int>(Start.x + xm + cell.Xall / 2), static_cast<int>(Start.y + ym + cell.Yall / 2), static_cast<int>(Start.z + zm + cell.Zall / 2),
							static_cast<int>(End.x + xm + cell.Xall / 2), static_cast<int>(End.y + ym + cell.Yall / 2), static_cast<int>(End.z + zm + cell.Zall / 2),
							[&](int x, int y, int z) {
							if (!cell.isInside(x, y, z)) { return false; }
							if (!cell.IsActiveCell(x, y, z)) { return false; }
							if (!(cell.GetOcclusionInfo(x, y, z) != 0b111111)) { return false; }
							Vector3DX MinPos = cell.GetPos(x, y, z);
							Vector3DX MaxPos = cell.GetPos(x + 1, y + 1, z + 1);

							MV1_COLL_RESULT_POLY tmp{};
							// Left
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							// Right
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							// Back
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							// Forward
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							return true;
							});
					}
				}
			}
			bool HitFlag = false;
			// 壁ポリゴンとの当たり判定処理
			if (kabes.size() > 0) {
				HitFlag = false;
				for (auto& kabe : kabes) {
					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
						HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
						if (MoveVector.magnitude() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							*EndPos = Vector3DX::Cross(kabe.Normal, Vector3DX::Cross(MoveVector, kabe.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
									j = true;
									break;// 当たっていたらループから抜ける
								}
							}
							if (!j) {
								HitFlag = false;
								break;//どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
							}
						}
						else {
							break;
						}
					}
				}
				if (HitFlag) {		// 壁に当たっていたら壁から押し出す処理を行う
					for (int k = 0; k < 16; ++k) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
						bool HitF = false;
						for (auto& kabe : kabes) {
							// プレイヤーと当たっているかを判定
							if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale_Rate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
								bool j = false;
								for (auto& kabe2 : kabes) {
									// 当たっていたらループを抜ける
									if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
										j = true;
										break;
									}
								}
								if (!j) {// 全てのポリゴンと当たっていなかったらここでループ終了
									break;
								}
								HitF = true;
							}
						}
						if (!HitF) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
							break;
						}
					}
				}
				kabes.clear();
			}
			return HitFlag;
		}
		//
		void		BackGroundClass::LoadCellsFile() noexcept {
			std::ifstream fin{};
			fin.open("data/Map.txt", std::ios::in | std::ios::binary);
			auto& cell = m_CellxN.front();
			fin.read((char*)&cell.m_Cell[0], sizeof(cell.m_Cell[0])* (cell.Xall* cell.Yall* cell.Zall));
			fin.close();
		}
		void		BackGroundClass::SaveCellsFile() noexcept {
			std::ofstream fout{};
			fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			auto& cell = m_CellxN.front();
			fout.write((char*)&cell.m_Cell[0], sizeof(cell.m_Cell[0]) * (size_t)(cell.Xall * cell.Yall * cell.Zall));
			fout.close();  //ファイルを閉じる
		}
		//
		void		BackGroundClass::SetBlick(int x, int y, int z, int8_t select) noexcept {
			auto& cell = m_CellxN.front();
			if (!cell.isInside(x, y, z)) { return; }
			cell.SetCell(x, y, z) = select;
			//簡易版を更新
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);
				auto& cell2 = m_CellxN.at(static_cast<size_t>(sel - 1));

				int xm = x / cell1.scaleRate;
				int ym = y / cell1.scaleRate;
				int zm = z / cell1.scaleRate;

				cell1.SetCell(xm, ym, zm) = cell2.isFill(xm, ym, zm) ? 1 : s_EmptyBlick;
			}
			//遮蔽検索
			for (auto& cellx : m_CellxN) {
				int xm = x / cellx.scaleRate;
				int ym = y / cellx.scaleRate;
				int zm = z / cellx.scaleRate;
				auto CheckCell = [&](int xp, int yp, int zp) {
					if (cellx.isInside(xp, yp, zp)) {
						cellx.CalcOcclusion(xp, yp, zp);
					}
					};
				CheckCell(xm + 1, ym, zm);
				CheckCell(xm - 1, ym, zm);
				CheckCell(xm, ym + 1, zm);
				CheckCell(xm, ym - 1, zm);
				CheckCell(xm, ym, zm + 1);
				CheckCell(xm, ym, zm - 1);
			}
		}
		//
		void		BackGroundClass::Load(void) noexcept {
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			m_tex = GraphHandle::Load("data/tex.png");
			m_norm = GraphHandle::Load("data/nrm.png");
		}
		//
		void		BackGroundClass::Init(void) noexcept {
			//空
			this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
				//this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
				this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.7f, 0.7f, 0.7f, 1.f));
				this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
			}
			if (false) {
				PerlinNoise ns(GetRand(100));
				auto& cell = m_CellxN.front();
				for (int x = 0; x < cell.Xall; ++x) {
					for (int z = 0; z < cell.Zall; ++z) {
						auto Height = static_cast<int>(ns.octaveNoise(2,
							(static_cast<float>(x)) / cell.Xall,
							(static_cast<float>(z)) / cell.Zall
						) * static_cast<float>(cell.Yall * 4 / 5));
						for (int y = 0; y < cell.Yall; ++y) {
							if (y <= Height) {
								cell.SetCell(x, y, z) = 1;
								continue;
							}
							cell.SetCell(x, y, z) = s_EmptyBlick;
						}
					}
				}
				SaveCellsFile();
			}
			else {
				LoadCellsFile();
			}
			//簡略版を制作
			auto& cell = m_CellxN.front();
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);
				auto& cell2 = m_CellxN.at(static_cast<size_t>(sel - 1));

				cell1.SetScale(static_cast<int>(pow(MulPer, sel)), cell.Xall, cell.Yall, cell.Zall);

				for (int xm = 0; xm < cell1.Xall; ++xm) {
					for (int ym = 0; ym < cell1.Yall; ++ym) {
						for (int zm = 0; zm < cell1.Zall; ++zm) {
							cell1.SetCell(xm, ym, zm) = cell2.isFill(xm, ym, zm) ? 1 : s_EmptyBlick;
						}
					}
				}
			}
			//遮蔽検索
			for (auto& cellx : m_CellxN) {
				for (int x = 0; x < cellx.Xall; ++x) {
					for (int y = 0; y < cellx.Yall; ++y) {
						for (int z = 0; z < cellx.Zall; ++z) {
							if (!cellx.IsActiveCell(x, y, z)) { continue; }
							cellx.CalcOcclusion(x, y, z);
						}
					}
				}
			}

			MATERIALPARAM Param{};
			Param.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 1.0f);						// ディフューズカラー
			Param.Ambient = GetColorF(0.5f, 0.5f, 0.5f, 1.0f);						// アンビエントカラー
			Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// スペキュラカラー
			Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// エミッシブカラー
			Param.Power = 5.f;							// スペキュラハイライトの鮮明度
			SetMaterialParam(Param);

			m_vert32.clear();
			m_index32.clear();
			m_32Num = 0;
			m_32Size = 0;
			m_vert32SB.clear();
			m_vert32S.clear();
			m_index32S.clear();
			m_S32Num = 0;
			m_S32Size = 0;

			m_JobEnd = true;
		}
		//
		void		BackGroundClass::Execute(void) noexcept {
			if (m_JobEnd) {
				m_JobEnd = false;
				if (m_Job.joinable()) {
					m_Job.detach();
				}
				//
				m_vert32Out = m_vert32;
				m_index32Out = m_index32;
				m_32NumOut = m_32Num;
				m_vert32SBOut = m_vert32SB;
				m_vert32SOut = m_vert32S;
				m_index32SOut = m_index32S;
				m_S32NumOut = m_S32Num;
				//
				auto* DrawParts = DXDraw::Instance();
				auto* OptionParts = OPTION::Instance();
				CamPos = DrawParts->GetMainCamera().GetCamPos();
				CamVec = (DrawParts->GetMainCamera().GetCamVec() - CamPos).normalized();
				BaseRate = 100;
				ShadowRate = 100;
				ShadowMax = DrawMax;
				switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)) {
				case 0:
				case 1:
					BaseRate = MulPer * MulPer;
					ShadowRate = 1;
					ShadowMax = DrawMax / 2;
					break;
				case 2:
					BaseRate = MulPer * MulPer * MulPer;
					ShadowRate = 1;
					ShadowMax = DrawMax;
					break;
				case 3:
					BaseRate = MulPer * MulPer * MulPer * MulPer;
					ShadowRate = MulPer;
					ShadowMax = DrawMax;
					break;
				default:
					break;
				};
				if (!OptionParts->GetParamBoolean(EnumSaveParam::shadow)) {
					ShadowRate = 0;
				}
				Vector3DX light = DrawParts->GetLightVec();
				lightX = static_cast<int>(light.x);
				lightY = static_cast<int>(light.y);
				lightZ = static_cast<int>(light.z);
				//
				if (blicksel >= 0) {
					auto& cell = m_CellxN.front();
					Vector3DX PutPos = (CamPos + CamVec * (LenMouse * Scale_Rate)) / CellScale;
					int x = (int)PutPos.x, y = (int)PutPos.y, z = (int)PutPos.z;
					for (int xp = 0; xp < xput; xp++) {
						for (int yp = 0; yp < yput; yp++) {
							for (int zp = 0; zp < zput; zp++) {
								SetBlick((x + xp - xput / 2 + cell.Xall / 2), (y + yp - yput / 2 + cell.Yall / 2), (z + zp - zput / 2 + cell.Zall / 2), blicksel);
							}
						}
					}
					blicksel = -1;
				}
				//
				std::thread tmp([this]() {
					m_32Num = 0;
					m_S32Num = 0;
					for (auto& cellx : m_CellxN) {
						Vector3DX center = CamPos / (CellScale * cellx.scaleRate);
						int centerX = static_cast<int>(center.x) + cellx.Xall / 2;
						int centerY = static_cast<int>(center.y) + cellx.Yall / 2;
						int centerZ = static_cast<int>(center.z) + cellx.Zall / 2;
						//
						if (BaseRate >= cellx.scaleRate) {
							AddCubes(cellx, centerX, centerY, centerZ);
						}
						//
						if (ShadowRate >= cellx.scaleRate) {
							AddShadowCubes(cellx, centerX, centerY, centerZ);
						}
					}
					m_JobEnd = true;
					});
				m_Job.swap(tmp);
			}

			auto* Pad = PadControl::Instance();
			LenMouse += Pad->GetMouseWheelRot();
			if (Pad->GetKey(PADS::SHOT).trigger()) {
				blicksel = 1;
			}
			if (Pad->GetKey(PADS::AIM).trigger()) {
				blicksel = 0;
			}
			if (Pad->GetKey(PADS::JUMP).trigger()) {
				SaveCellsFile();
			}
		}
		//
		void		BackGroundClass::BG_Draw(void) const noexcept {
			SetUseLighting(FALSE);
			this->m_ObjSky.DrawModel();
			SetUseLighting(TRUE);
		}
		void		BackGroundClass::Shadow_Draw(void) const noexcept {
			if (m_S32NumOut > 0) {
				DrawPolygon32bitIndexed3D(m_vert32SBOut.data(), static_cast<int>(m_S32NumOut * 4), m_index32SOut.data(), static_cast<int>(m_S32NumOut * 6 / 3), m_tex.get(), TRUE);
			}
		}
		void		BackGroundClass::SetShadow_Draw_Rigid(void) const noexcept {
			if (m_S32NumOut > 0) {
				SetUseTextureToShader(0, m_tex.get());
				DrawPolygon32bitIndexed3DToShader(m_vert32SOut.data(), static_cast<int>(m_S32NumOut * 4), m_index32SOut.data(), static_cast<int>(m_S32NumOut * 6 / 3));
				SetUseTextureToShader(0, INVALID_ID);
			}
		}
		void		BackGroundClass::Draw(void) const noexcept {
			if (m_32NumOut > 0) {
				DrawPolygon32bitIndexed3D(m_vert32Out.data(), static_cast<int>(m_32NumOut * 4), m_index32Out.data(), static_cast<int>(m_32NumOut * 6 / 3), m_tex.get(), TRUE);
			}

			//
			Vector3DX PutPos = (CamPos + CamVec * (LenMouse * Scale_Rate)) / CellScale;
			int x = (int)PutPos.x, y = (int)PutPos.y, z = (int)PutPos.z;
			SetUseLighting(FALSE);
			DrawCube3D(
				(Vector3DX::vget((float)(x - xput / 2), (float)(y - yput / 2), (float)(z - zput / 2)) * CellScale).get(),
				(Vector3DX::vget((float)(x + xput - xput / 2), (float)(y + yput - yput / 2), (float)(z + zput - zput / 2)) * CellScale).get(),
				GetColor(0, 64, 0), GetColor(0, 0, 0), FALSE);
			SetUseLighting(TRUE);
		}
		//
		void		BackGroundClass::Dispose(void) noexcept {
			this->m_ObjSky.Dispose();
			SaveCellsFile();
		}
	}
}
