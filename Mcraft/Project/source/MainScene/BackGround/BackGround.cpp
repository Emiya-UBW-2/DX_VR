#include	"BackGround.hpp"
#include <random>

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
		bool		BackGroundClass::AddCubeX_CanAddPlane(const CellsData& cellx, int xmin, int xmax, int cy, int cz, int centerX, bool CheckFillYZ, int id) noexcept {
			for (int x = xmin; x <= xmax; ++x) {
				// 頂点データの作成
				if (CheckFillYZ && ((DrawMinXMinus + 1 == x) || (x == DrawMinXPlus - 1))) {
					return true;
				}
				else if ((cellx.GetOcclusionInfo(centerX + x, cy, cz) & (1 << id)) == 0) {
					return true;
				}
			}
			return false;
		}
		bool		BackGroundClass::AddCubeZ_CanAddPlane(const CellsData& cellx, int cx, int cy, int zmin, int zmax, int centerZ, bool CheckFillXY, int id) noexcept {
			for (int z = zmin; z <= zmax; ++z) {
				// 頂点データの作成
				if (CheckFillXY && ((DrawMinZMinus + 1 == z) || (z == DrawMinZPlus - 1))) {
					return true;
				}
				else {
					if ((cellx.GetOcclusionInfo(cx, cy, centerZ + z) & (1 << id)) == 0) {
						return true;
					}
				}
			}
			return false;
		}
		//
		void		BackGroundClass::AllocatePlane(void) noexcept {
			++m_32Num;
			if (m_32Num > m_32Size) {
				m_32Size = m_32Num;
				m_vert32.resize(m_32Size * 4);
				m_index32.resize(m_32Size * 6);
			}
			m_index32.at(m_32Num * 6 - 6) = (uint32_t)(m_32Num * 4 - 4);
			m_index32.at(m_32Num * 6 - 5) = (uint32_t)(m_32Num * 4 - 3);
			m_index32.at(m_32Num * 6 - 4) = (uint32_t)(m_32Num * 4 - 2);
			m_index32.at(m_32Num * 6 - 3) = (uint32_t)(m_32Num * 4 - 1);
			m_index32.at(m_32Num * 6 - 2) = (uint32_t)(m_32Num * 4 - 2);
			m_index32.at(m_32Num * 6 - 1) = (uint32_t)(m_32Num * 4 - 3);
		}
		void		BackGroundClass::AddPlaneXPlus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocatePlane();

			int zscale = zmax - zmin + 1;

			int Xofs = zmin % 2 == 0;
			int Yofs = 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;

			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = cellx.GetPos(x + 1, y + xt, zmin + yt * zscale).get();
				V.u = uAdd * static_cast<float>(Xofs + u * zscale * cellx.scaleRate);
				V.v = vAdd * static_cast<float>(Yofs + v);// * cellx.scaleRate
				V.norm = Vector3DX::right().get();
				V.dif = DifColor;
				V.spc = SpcColor;
			};

			AddPointPos(0, 1, 1, 0, -4);
			AddPointPos(0, 0, 0, 0, -3);
			AddPointPos(1, 1, 1, 1, -2);
			AddPointPos(1, 0, 0, 1, -1);
		}
		void		BackGroundClass::AddPlaneXMinus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocatePlane();

			int zscale = zmax - zmin + 1;

			int Xofs = zmin % 2 == 0;
			int Yofs = 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;

			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = cellx.GetPos(x, y + xt, zmin + yt * zscale).get();
				V.u = uAdd * static_cast<float>(Xofs + u * zscale * cellx.scaleRate);
				V.v = vAdd * static_cast<float>(Yofs + v);// * cellx.scaleRate
				V.norm = Vector3DX::left().get();
				V.dif = DifColor;
				V.spc = SpcColor;
				};
			AddPointPos(1, 1, 1, 0, -4);
			AddPointPos(1, 0, 0, 0, -3);
			AddPointPos(0, 1, 1, 1, -2);
			AddPointPos(0, 0, 0, 1, -1);
		}
		void		BackGroundClass::AddPlaneYPlus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocatePlane();

			int zscale = zmax - zmin + 1;

			int Xofs = zmin % 2 == 0;
			int Yofs = 0;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;

			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = cellx.GetPos(x + xt, y + 1, zmin + yt * zscale).get();
				V.u = uAdd * static_cast<float>(Xofs + u * zscale * cellx.scaleRate);
				V.v = vAdd * static_cast<float>(Yofs + v);// * cellx.scaleRate
				V.norm = Vector3DX::up().get();
				V.dif = DifColor;
				V.spc = SpcColor;
				};

			AddPointPos(1, 1, 1, 0, -4);
			AddPointPos(1, 0, 0, 0, -3);
			AddPointPos(0, 1, 1, 1, -2);
			AddPointPos(0, 0, 0, 1, -1);
		}
		void		BackGroundClass::AddPlaneYMinus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocatePlane();

			int zscale = zmax - zmin + 1;

			int Xofs = zmin % 2 == 0;
			int Yofs = 2;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;

			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = cellx.GetPos(x + xt, y, zmin + yt * zscale).get();
				V.u = uAdd * static_cast<float>(Xofs + u * zscale * cellx.scaleRate);
				V.v = vAdd * static_cast<float>(Yofs + v);// * cellx.scaleRate
				V.norm = Vector3DX::down().get();
				V.dif = DifColor;
				V.spc = SpcColor;
				};

			AddPointPos(0, 1, 1, 0, -4);
			AddPointPos(0, 0, 0, 0, -3);
			AddPointPos(1, 1, 1, 1, -2);
			AddPointPos(1, 0, 0, 1, -1);
		}
		void		BackGroundClass::AddPlaneZPlus(const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			AllocatePlane();

			int xscale = xmax - xmin + 1;

			int Xofs = xmax % 2 == 0;
			int Yofs = 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;

			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = cellx.GetPos(xmin + xt * xscale, y + yt, z + 1).get();
				V.u = uAdd * static_cast<float>(Xofs + u * xscale * cellx.scaleRate);
				V.v = vAdd * static_cast<float>(Yofs + v);// * cellx.scaleRate
				V.norm = Vector3DX::forward().get();
				V.dif = DifColor;
				V.spc = SpcColor;
				};

			AddPointPos(0, 0, 1, 0, -4);
			AddPointPos(1, 0, 0, 0, -3);
			AddPointPos(0, 1, 1, 1, -2);
			AddPointPos(1, 1, 0, 1, -1);
		}
		void		BackGroundClass::AddPlaneZMinus(const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			AllocatePlane();

			int xscale = xmax - xmin + 1;

			int Xofs = xmax % 2 == 0;
			int Yofs = 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 4.f;

			COLOR_U8 DifColor = GetColorU8(128, 128, 128, 255);
			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
				auto& V = m_vert32.at(m_32Num * 4 + sel);
				V.pos = cellx.GetPos(xmin + xt * xscale, y + yt, z).get();
				V.u = uAdd * static_cast<float>(Xofs + u * xscale * cellx.scaleRate);
				V.v = vAdd * static_cast<float>(Yofs + v);// * cellx.scaleRate
				V.norm = Vector3DX::back().get();
				V.dif = DifColor;
				V.spc = SpcColor;
				};

			AddPointPos(0, 1, 1, 0, -4);
			AddPointPos(1, 1, 0, 0, -3);
			AddPointPos(0, 0, 1, 1, -2);
			AddPointPos(1, 0, 0, 1, -1);
		}

		void		BackGroundClass::AddCubesX(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			for (int z = DrawMaxZMinus; z <= DrawMaxZPlus; ++z) {
				if (!CalcXYActive(CamVec, DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, z)) { continue; }

				bool CheckFillZ = (cellx.scaleRate != 1) && ((DrawMinZMinus <= z) && (z <= DrawMinZPlus));
				bool CheckInsideZ = (cellx.scaleRate != 1) && ((DrawMinZMinus < z) && (z < DrawMinZPlus));

				for (int y = DrawMaxYMinus; y <= DrawMaxYPlus; ++y) {
					if (y < -centerY || cellx.Yall - centerY <= y) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(CamVec, &xMaxminT, &xMaxmaxT, y, z)) { continue; }

					bool CheckFill = CheckFillZ && ((DrawMinYMinus <= y) && (y <= DrawMinYPlus));
					bool CheckInside = CheckInsideZ && ((DrawMinYMinus < y) && (y < DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int x = xMaxminT; x <= xMaxmaxT; ++x) {
						if (
							(x == xMaxmaxT) ||
							(CheckInside && ((DrawMinXMinus < x) && (x < DrawMinXPlus))) ||
							(!cellx.IsActiveCell(centerX + x, centerY + y, centerZ + z))
							//|| (cellx.GetOcclusionInfo(centerX + x, centerY + y, centerZ + z) == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (z >= 0) {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, centerY + y, centerZ + z, centerX, CheckFill, 5)) {
										AddPlaneZMinus(cellx, centerX + xmin, centerX + xmax, centerY + y, centerZ + z);
									}
								}
								else {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, centerY + y, centerZ + z, centerX, CheckFill, 4)) {
										AddPlaneZPlus(cellx, centerX + xmin, centerX + xmax, centerY + y, centerZ + z);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = x;
							}
							xmax = x;
						}
					}
				}
			}
		}
		void		BackGroundClass::AddCubesZ(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			for (int x = DrawMaxXMinus; x <= DrawMaxXPlus; ++x) {
				if (!CalcYZActive(CamVec, x, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = (cellx.scaleRate != 1) && ((DrawMinXMinus <= x) && (x <= DrawMinXPlus));
				bool CheckInsideX = (cellx.scaleRate != 1) && ((DrawMinXMinus < x) && (x < DrawMinXPlus));

				for (int y = DrawMaxYMinus; y <= DrawMaxYPlus; ++y) {
					if (y < -centerY || cellx.Yall - centerY <= y) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(CamVec, x, y, &zMaxminT, &zMaxmaxT)) { continue; }

					bool CheckFill = CheckFillX && ((DrawMinYMinus <= y) && (y <= DrawMinYPlus));
					bool CheckInside = CheckInsideX && ((DrawMinYMinus < y) && (y < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int z = zMaxminT; z <= zMaxmaxT; ++z) {
						if (
							(z == zMaxmaxT) ||
							(CheckInside && ((DrawMinZMinus < z) && (z < DrawMinZPlus))) ||
							(!cellx.IsActiveCell(centerX + x, centerY + y, centerZ + z))
							//|| (cellx.GetOcclusionInfo(centerX + x, centerY + y, centerZ + z) == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (x >= 0) {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 1)) {
										AddPlaneXMinus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 0)) {
										AddPlaneXPlus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								if (y >= 0) {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 3)) {
										AddPlaneYMinus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 2)) {
										AddPlaneYPlus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = z;
							}
							zmax = z;
						}
					}
				}
			}
		}
		void		BackGroundClass::AddCubes(void) noexcept {
			m_32Num = 0;
			for (auto& cellx : m_CellxN) {
				if (BaseRate < cellx.scaleRate) {
					continue;
				}
				Vector3DX center = CamPos / (CellScale * cellx.scaleRate);
				int centerX = static_cast<int>(center.x) + cellx.Xall / 2;
				int centerY = static_cast<int>(center.y) + cellx.Yall / 2;
				int centerZ = static_cast<int>(center.z) + cellx.Zall / 2;
				//
				AddCubesX(cellx, centerX, centerY, centerZ);
				AddCubesZ(cellx, centerX, centerY, centerZ);
			}
		}
		//
		void		BackGroundClass::AllocateShadowPlane(void) noexcept {
			++m_SB32Num;
			if (m_SB32Num > m_SB32Size) {
				m_SB32Size = m_SB32Num;
				m_vert32SB.resize(m_SB32Size * 4);
				m_index32SB.resize(m_SB32Size * 6);
			}
			m_index32SB.at(m_SB32Num * 6 - 6) = (uint32_t)(m_SB32Num * 4 - 4);
			m_index32SB.at(m_SB32Num * 6 - 5) = (uint32_t)(m_SB32Num * 4 - 3);
			m_index32SB.at(m_SB32Num * 6 - 4) = (uint32_t)(m_SB32Num * 4 - 2);
			m_index32SB.at(m_SB32Num * 6 - 3) = (uint32_t)(m_SB32Num * 4 - 1);
			m_index32SB.at(m_SB32Num * 6 - 2) = (uint32_t)(m_SB32Num * 4 - 2);
			m_index32SB.at(m_SB32Num * 6 - 1) = (uint32_t)(m_SB32Num * 4 - 3);
		}
		void		BackGroundClass::AddShadowPlaneXPlus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32SB.at(m_SB32Num * 4 + sel);
				V.pos = cellx.GetPos(x + 1, y + xt, zmin + yt * zscale).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				};

			AddPointPos(1, 0, -4);
			AddPointPos(1, 1, -3);
			AddPointPos(0, 0, -2);
			AddPointPos(0, 1, -1);
		}
		void		BackGroundClass::AddShadowPlaneXMinus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32SB.at(m_SB32Num * 4 + sel);
				V.pos = cellx.GetPos(x + 0, y + xt, zmin + yt * zscale).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				};

			AddPointPos(1, 1, -4);
			AddPointPos(1, 0, -3);
			AddPointPos(0, 1, -2);
			AddPointPos(0, 0, -1);
		}
		void		BackGroundClass::AddShadowPlaneYPlus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32SB.at(m_SB32Num * 4 + sel);
				V.pos = cellx.GetPos(x + xt, y + 1, zmin + yt * zscale).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				};

			AddPointPos(0, 1, -4);
			AddPointPos(1, 1, -3);
			AddPointPos(0, 0, -2);
			AddPointPos(1, 0, -1);
		}
		void		BackGroundClass::AddShadowPlaneYMinus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32SB.at(m_SB32Num * 4 + sel);
				V.pos = cellx.GetPos(x + xt, y + 0, zmin + yt * zscale).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				};

			AddPointPos(0, 0, -4);
			AddPointPos(1, 0, -3);
			AddPointPos(0, 1, -2);
			AddPointPos(1, 1, -1);
		}
		void		BackGroundClass::AddShadowPlaneZPlus(const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			AllocateShadowPlane();

			int xscale = xmax - xmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32SB.at(m_SB32Num * 4 + sel);
				V.pos = cellx.GetPos(xmin + xt * xscale, y + yt, z + 1).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				};

			AddPointPos(0, 0, -4);
			AddPointPos(1, 0, -3);
			AddPointPos(0, 1, -2);
			AddPointPos(1, 1, -1);
		}
		void		BackGroundClass::AddShadowPlaneZMinus(const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			AllocateShadowPlane();

			int xscale = xmax - xmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32SB.at(m_SB32Num * 4 + sel);
				V.pos = cellx.GetPos(xmin + xt * xscale, y + yt, z + 0).get();
				V.norm = Vector3DX::back().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				};
			AddPointPos(0, 1, -4);
			AddPointPos(1, 1, -3);
			AddPointPos(0, 0, -2);
			AddPointPos(1, 0, -1);
		}

		void		BackGroundClass::AddShadowCubesX(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			for (int z = DrawMaxZMinus; z <= DrawMaxZPlus; ++z) {

				bool CheckInsideZ = (cellx.scaleRate != 1) && ((DrawMinZMinus < z) && (z < DrawMinZPlus));

				for (int y = DrawMaxYMinus; y <= DrawMaxYPlus; ++y) {
					if (y < -centerY || cellx.Yall - centerY <= y) { continue; }

					bool CheckInside = CheckInsideZ && ((DrawMinYMinus < y) && (y < DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int x = DrawMaxXMinus; x <= DrawMaxXPlus; ++x) {
						if (
							(x == DrawMaxXPlus) ||
							(CheckInside && ((DrawMinXMinus < x) && (x < DrawMinXPlus))) ||
							(!cellx.IsActiveCell(centerX + x, centerY + y, centerZ + z))
							//|| (cellx.GetOcclusionInfo(centerX + x, centerY + y, centerZ + z) == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (light.z > 0.f) {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, centerY + y, centerZ + z, centerX, false, 5)) {
										AddShadowPlaneZMinus(cellx, centerX + xmin, centerX + xmax, centerY + y, centerZ + z);
									}
								}
								else {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, centerY + y, centerZ + z, centerX, false, 4)) {
										AddShadowPlaneZPlus(cellx, centerX + xmin, centerX + xmax, centerY + y, centerZ + z);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = x;
							}
							xmax = x;
						}
					}
				}
			}
		}
		void		BackGroundClass::AddShadowCubesZ(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			for (int x = DrawMaxXMinus; x <= DrawMaxXPlus; ++x) {

				bool CheckInsideX = (cellx.scaleRate != 1) && ((DrawMinXMinus < x) && (x < DrawMinXPlus));

				for (int y = DrawMaxYMinus; y <= DrawMaxYPlus; ++y) {
					if (y < -centerY || cellx.Yall - centerY <= y) { continue; }

					bool CheckInside = CheckInsideX && ((DrawMinYMinus < y) && (y < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int z = DrawMaxZMinus; z <= DrawMaxZPlus; ++z) {
						if (
							(z == DrawMaxZPlus) ||
							(CheckInside && ((DrawMinZMinus < z) && (z < DrawMinZPlus))) ||
							(!cellx.IsActiveCell(centerX + x, centerY + y, centerZ + z))
							//|| (cellx.GetOcclusionInfo(centerX + x, centerY + y, centerZ + z) == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (light.x > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, false, 1)) {
										AddShadowPlaneXMinus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, false, 0)) {
										AddShadowPlaneXPlus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								if (light.y > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, false, 3)) {
										AddShadowPlaneYMinus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, false, 2)) {
										AddShadowPlaneYPlus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = z;
							}
							zmax = z;
						}
					}
				}
			}
		}
		void		BackGroundClass::AddShadowCubes(void) noexcept {
			m_SB32Num = 0;
			for (auto& cellx : m_CellxN) {
				if (ShadowRate < cellx.scaleRate) {
					continue;
				}
				Vector3DX center = CamPosSB / (CellScale * cellx.scaleRate);
				int centerX = static_cast<int>(center.x) + cellx.Xall / 2;
				int centerY = static_cast<int>(center.y) + cellx.Yall / 2;
				int centerZ = static_cast<int>(center.z) + cellx.Zall / 2;
				AddShadowCubesX(cellx, centerX, centerY, centerZ);
				AddShadowCubesZ(cellx, centerX, centerY, centerZ);
			}
		}
		//
		void		BackGroundClass::AllocateSetShadowPlane(void) noexcept {
			++m_S32Num;
			if (m_S32Num > m_S32Size) {
				m_S32Size = m_S32Num;
				m_vert32S.resize(m_S32Size * 4);
				m_index32S.resize(m_S32Size * 6);
			}
			m_index32S.at(m_S32Num * 6 - 6) = (uint32_t)(m_S32Num * 4 - 4);
			m_index32S.at(m_S32Num * 6 - 5) = (uint32_t)(m_S32Num * 4 - 3);
			m_index32S.at(m_S32Num * 6 - 4) = (uint32_t)(m_S32Num * 4 - 2);
			m_index32S.at(m_S32Num * 6 - 3) = (uint32_t)(m_S32Num * 4 - 1);
			m_index32S.at(m_S32Num * 6 - 2) = (uint32_t)(m_S32Num * 4 - 2);
			m_index32S.at(m_S32Num * 6 - 1) = (uint32_t)(m_S32Num * 4 - 3);
		}
		void		BackGroundClass::AddSetShadowPlaneXPlus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateSetShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32S.at(m_S32Num * 4 + sel);
				V.pos = cellx.GetPos(x + 1, y + xt, zmin + yt * zscale).get();
				};

			AddPointPos(1, 0, -4);
			AddPointPos(1, 1, -3);
			AddPointPos(0, 0, -2);
			AddPointPos(0, 1, -1);
		}
		void		BackGroundClass::AddSetShadowPlaneXMinus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateSetShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32S.at(m_S32Num * 4 + sel);
				V.pos = cellx.GetPos(x + 0, y + xt, zmin + yt * zscale).get();
				};

			AddPointPos(1, 1, -4);
			AddPointPos(1, 0, -3);
			AddPointPos(0, 1, -2);
			AddPointPos(0, 0, -1);
		}
		void		BackGroundClass::AddSetShadowPlaneYPlus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateSetShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32S.at(m_S32Num * 4 + sel);
				V.pos = cellx.GetPos(x + xt, y + 1, zmin + yt * zscale).get();
				};

			AddPointPos(0, 1, -4);
			AddPointPos(1, 1, -3);
			AddPointPos(0, 0, -2);
			AddPointPos(1, 0, -1);
		}
		void		BackGroundClass::AddSetShadowPlaneYMinus(const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			AllocateSetShadowPlane();

			int zscale = zmax - zmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32S.at(m_S32Num * 4 + sel);
				V.pos = cellx.GetPos(x + xt, y + 0, zmin + yt * zscale).get();
				};

			AddPointPos(0, 0, -4);
			AddPointPos(1, 0, -3);
			AddPointPos(0, 1, -2);
			AddPointPos(1, 1, -1);
		}
		void		BackGroundClass::AddSetShadowPlaneZPlus(const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			AllocateSetShadowPlane();

			int xscale = xmax - xmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32S.at(m_S32Num * 4 + sel);
				V.pos = cellx.GetPos(xmin + xt * xscale, y + yt, z + 1).get();
				};

			AddPointPos(0, 0, -4);
			AddPointPos(1, 0, -3);
			AddPointPos(0, 1, -2);
			AddPointPos(1, 1, -1);
		}
		void		BackGroundClass::AddSetShadowPlaneZMinus(const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			AllocateSetShadowPlane();

			int xscale = xmax - xmin + 1;
			auto AddPointPos = [&](int xt, int yt, int sel) {
				auto& V = m_vert32S.at(m_S32Num * 4 + sel);
				V.pos = cellx.GetPos(xmin + xt * xscale, y + yt, z + 0).get();
				};
			AddPointPos(0, 1, -4);
			AddPointPos(1, 1, -3);
			AddPointPos(0, 0, -2);
			AddPointPos(1, 0, -1);
		}

		void		BackGroundClass::AddSetShadowCubesX(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			for (int z = DrawMaxZMinus; z <= DrawMaxZPlus; ++z) {
				if (!CalcXYActive(CamVecS, DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, z)) { continue; }

				bool CheckFillZ = (cellx.scaleRate != 1) && ((DrawMinZMinus <= z) && (z <= DrawMinZPlus));
				bool CheckInsideZ = (cellx.scaleRate != 1) && ((DrawMinZMinus < z) && (z < DrawMinZPlus));

				for (int y = DrawMaxYMinus; y <= DrawMaxYPlus; ++y) {
					if (y < -centerY || cellx.Yall - centerY <= y) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(CamVecS, &xMaxminT, &xMaxmaxT, y, z)) { continue; }

					bool CheckFill = CheckFillZ && ((DrawMinYMinus <= y) && (y <= DrawMinYPlus));
					bool CheckInside = CheckInsideZ && ((DrawMinYMinus < y) && (y < DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int x = xMaxminT; x <= xMaxmaxT; ++x) {
						if (
							(x == xMaxmaxT) ||
							(CheckInside && ((DrawMinXMinus < x) && (x < DrawMinXPlus))) ||
							(!cellx.IsActiveCell(centerX + x, centerY + y, centerZ + z))
							//|| (cellx.GetOcclusionInfo(centerX + x, centerY + y, centerZ + z) == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (z >= 0) {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, centerY + y, centerZ + z, centerX, CheckFill, 5)) {
										AddSetShadowPlaneZMinus(cellx, centerX + xmin, centerX + xmax, centerY + y, centerZ + z);
									}
								}
								else {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, centerY + y, centerZ + z, centerX, CheckFill, 4)) {
										AddSetShadowPlaneZPlus(cellx, centerX + xmin, centerX + xmax, centerY + y, centerZ + z);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = x;
							}
							xmax = x;
						}
					}
				}
			}
		}
		void		BackGroundClass::AddSetShadowCubesZ(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept {
			for (int x = DrawMaxXMinus; x <= DrawMaxXPlus; ++x) {
				if (!CalcYZActive(CamVecS, x, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = (cellx.scaleRate != 1) && ((DrawMinXMinus <= x) && (x <= DrawMinXPlus));
				bool CheckInsideX = (cellx.scaleRate != 1) && ((DrawMinXMinus < x) && (x < DrawMinXPlus));

				for (int y = DrawMaxYMinus; y <= DrawMaxYPlus; ++y) {
					if (y < -centerY || cellx.Yall - centerY <= y) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(CamVecS, x, y, &zMaxminT, &zMaxmaxT)) { continue; }

					bool CheckFill = CheckFillX && ((DrawMinYMinus <= y) && (y <= DrawMinYPlus));
					bool CheckInside = CheckInsideX && ((DrawMinYMinus < y) && (y < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int z = zMaxminT; z <= zMaxmaxT; ++z) {
						if (
							(z == zMaxmaxT) ||
							(CheckInside && ((DrawMinZMinus < z) && (z < DrawMinZPlus))) ||
							(!cellx.IsActiveCell(centerX + x, centerY + y, centerZ + z))
							//|| (cellx.GetOcclusionInfo(centerX + x, centerY + y, centerZ + z) == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (x >= 0) {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 1)) {
										AddSetShadowPlaneXMinus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 0)) {
										AddSetShadowPlaneXPlus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								if (y >= 0) {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 3)) {
										AddSetShadowPlaneYMinus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, centerX + x, centerY + y, zmin, zmax, centerZ, CheckFill, 2)) {
										AddSetShadowPlaneYPlus(cellx, centerX + x, centerY + y, centerZ + zmin, centerZ + zmax);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = z;
							}
							zmax = z;
						}
					}
				}
			}
		}
		void		BackGroundClass::AddSetShadowCubes(void) noexcept {
			m_S32Num = 0;
			for (auto& cellx : m_CellxN) {
				if (ShadowRate < cellx.scaleRate) {
					continue;
				}
				Vector3DX center = CamPosS / (CellScale * cellx.scaleRate);
				int centerX = static_cast<int>(center.x) + cellx.Xall / 2;
				int centerY = static_cast<int>(center.y) + cellx.Yall / 2;
				int centerZ = static_cast<int>(center.z) + cellx.Zall / 2;
				AddSetShadowCubesX(cellx, centerX, centerY, centerZ);
				AddSetShadowCubesZ(cellx, centerX, centerY, centerZ);
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
							if (!cell.isInside(y)) { return false; }
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
							if (!cell.isInside(y)) { return false; }
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
			/*
			for (int x = 0; x < cell.Xall; ++x) {
				for (int z = 0; z < cell.Zall; ++z) {
					for (int y = cell.Yall; y >= 2; --y) {
						cell.SetCell(x, y, z) = cell.GetCell(x, y - 2, z);
					}
				}
			}
			//*/
			/*
			for (int x = 0; x < cell.Xall; ++x) {
				if (x - cell.Xall/2 > -72) {
					for (int z = 0; z < cell.Zall; ++z) {
						for (int y = 15; y < cell.Yall / 2; ++y) {
							cell.SetCell(x, y, z) = cell.GetCell(x, y + 15, z);
						}
					}
				}
			}
			//*/
			/*
			for (int x = 0; x < cell.Xall; ++x) {
				if (x - cell.Xall/2 <= -72) {
					for (int z = 0; z < cell.Zall; ++z) {
						for (int y = 15; y < cell.Yall / 2-18; ++y) {
							cell.SetCell(x, y, z) = cell.GetCell(x, y + 15, z);
						}
					}
				}
			}
			//*/
			/*
			for (int x = 0; x < cell.Xall; ++x) {
				for (int z = 0; z < cell.Zall; ++z) {
					if (z <= cell.Zall / 2 - 40 || cell.Zall / 2 + 26 <= z) {
						for (int y = cell.Yall / 2 - 48; y <= cell.Yall; ++y) {
							cell.SetCell(x, y, z) = 0;
						}
					}
				}
			}
			//*/
			/*
			for (int x = 0; x < cell.Xall; ++x) {
				for (int z = 0; z < cell.Zall; ++z) {
					for (int y = 0; y <= cell.Yall / 2; ++y) {
						cell.SetCell(x, y, z) = 1;
					}
				}
			}
			//*/
		}
		void		BackGroundClass::SaveCellsFile() noexcept {
			std::ofstream fout{};
			fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			auto& cell = m_CellxN.front();
			fout.write((char*)&cell.m_Cell[0], sizeof(cell.m_Cell[0]) * (size_t)(cell.Xall * cell.Yall * cell.Zall));
			fout.close();  //ファイルを閉じる
		}
		//
#if EDITBLICK
		void		BackGroundClass::SetBlick(int x, int y, int z, int8_t select) noexcept {
			auto& cell = m_CellxN.front();
			if (!cell.isInside(y)) { return; }
			cell.SetCell(x, y, z) = select;
			//簡易版を更新
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);
				auto& cell2 = m_CellxN.at(static_cast<size_t>(sel - 1));

				int xm = x / cell1.scaleRate;
				int ym = y / cell1.scaleRate;
				int zm = z / cell1.scaleRate;

				cell1.SetCell(xm, ym, zm) = cell2.isFill(xm, ym, zm, cell1.scaleRate / cell2.scaleRate) ? 1 : s_EmptyBlick;
			}
			//遮蔽検索
			for (auto& cellx : m_CellxN) {
				int xm = x / cellx.scaleRate;
				int ym = y / cellx.scaleRate;
				int zm = z / cellx.scaleRate;
				auto CheckCell = [&](int xp, int yp, int zp) {
					if (cellx.isInside(yp)) {
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
#endif
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
				cell1.SetScale(static_cast<int>(pow(MulPer, sel)), cell.Xall, cell.Yall, cell.Zall);
				for (int xm = 0; xm < cell1.Xall; ++xm) {
					for (int ym = 0; ym < cell1.Yall; ++ym) {
						for (int zm = 0; zm < cell1.Zall; ++zm) {
							cell1.SetCell(xm, ym, zm) = cell.isFill(xm, ym, zm, cell1.scaleRate / cell.scaleRate) ? 1 : s_EmptyBlick;
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

			constexpr size_t size = (DrawMax + DrawMax) * (DrawMax + DrawMax) * (DrawMax + DrawMax) / 2 * 3 / 100;

			m_vert32.resize(size * 4);
			m_index32.resize(size * 6);
			m_32Num = 0;
			m_32Size = size;

			m_vert32SB.resize(size * 4);
			m_index32SB.resize(size * 6);
			m_SB32Num = 0;
			m_SB32Size = size;

			m_vert32S.resize(size * 4);
			m_index32S.resize(size * 6);
			m_S32Num = 0;
			m_S32Size = size;

			m_JobEnd = true;
			m_ShadowJobEnd = true;
			m_SetShadowJobEnd = true;
		}
		//
		void		BackGroundClass::Execute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			if (m_JobEnd) {
				m_JobEnd = false;
				if (m_Job.joinable()) {
					m_Job.detach();
				}
				//
				m_vert32Out = m_vert32;
				m_index32Out = m_index32;
				m_32NumOut = m_32Num;
				//
				CamPos = DrawParts->GetMainCamera().GetCamPos();
				CamVec = (DrawParts->GetMainCamera().GetCamVec() - CamPos).normalized();
				switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)) {
				case 0:
				case 1:
					BaseRate = static_cast<int>(pow(MulPer, 2));
					break;
				case 2:
					BaseRate = static_cast<int>(pow(MulPer, 3));
					break;
				case 3:
					BaseRate = static_cast<int>(pow(MulPer, 4));
					break;
				default:
					BaseRate = static_cast<int>(pow(MulPer, 4));
					break;
				};
				//
#if EDITBLICK
				PutPos = (CamPos + CamVec * (LenMouse * Scale_Rate)) / CellScale;
				if (blicksel >= 0) {
					auto& cell = m_CellxN.front();
					int x = (int)PutPos.x + cell.Xall / 2, y = (int)PutPos.y + cell.Yall / 2, z = (int)PutPos.z + cell.Zall / 2;
					for (int xp = 0; xp < xput; xp++) {
						for (int yp = 0; yp < yput; yp++) {
							for (int zp = 0; zp < zput; zp++) {
								SetBlick((x + xp - xput / 2), (y + yp - yput / 2), (z + zp - zput / 2), blicksel);
							}
						}
					}
					blicksel = -1;
				}
#endif
				//
				{
					std::thread tmp([this]() {
						AddCubes();
						m_JobEnd = true;
						});
					m_Job.swap(tmp);
					//強制待機
					//m_Job.join();
				}
			}

			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				if (m_ShadowJobEnd) {
					m_ShadowJobEnd = false;
					if (m_ShadowJob.joinable()) {
						m_ShadowJob.detach();
					}
					//
					m_vert32SBOut = m_vert32SB;
					m_index32SBOut = m_index32SB;
					m_SB32NumOut = m_SB32Num;
					//
					CamPosSB = DrawParts->GetMainCamera().GetCamPos();
					switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)) {
					case 0:
					case 1:
					case 2:
						ShadowRate = static_cast<int>(pow(MulPer, 0));
						break;
					case 3:
						ShadowRate = static_cast<int>(pow(MulPer, 1));
						break;
					default:
						ShadowRate = static_cast<int>(pow(MulPer, 1));
						break;
					};
					light = DrawParts->GetLightVec();
					//
					std::thread tmp([this]() {
						AddShadowCubes();
						m_ShadowJobEnd = true;
						});
					m_ShadowJob.swap(tmp);
					//強制待機
					//m_ShadowJob.join();
				}
				if (m_SetShadowJobEnd) {
					m_SetShadowJobEnd = false;
					if (m_SetShadowJob.joinable()) {
						m_SetShadowJob.detach();
					}
					//
					m_vert32SOut = m_vert32S;
					m_index32SOut = m_index32S;
					m_S32NumOut = m_S32Num;
					//
					CamPosS = DrawParts->GetMainCamera().GetCamPos();
					CamVecS = (DrawParts->GetMainCamera().GetCamVec() - CamPosS).normalized();
					//
					std::thread tmp([this]() {
						AddSetShadowCubes();
						m_SetShadowJobEnd = true;
						});
					m_SetShadowJob.swap(tmp);
					//強制待機
					//m_SetShadowJob.join();
			}
				}
#if EDITBLICK
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
#endif
		}
		//
		void		BackGroundClass::BG_Draw(void) const noexcept {
			SetUseLighting(FALSE);
			this->m_ObjSky.DrawModel();
			SetUseLighting(TRUE);
		}
		void		BackGroundClass::Shadow_Draw(void) const noexcept {
			if (m_SB32NumOut > 0) {
				DrawPolygon32bitIndexed3D(m_vert32SBOut.data(), static_cast<int>(m_SB32NumOut * 4), m_index32SBOut.data(), static_cast<int>(m_SB32NumOut * 6 / 3), m_tex.get(), TRUE);
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
#if EDITBLICK
			int x = (int)PutPos.x, y = (int)PutPos.y, z = (int)PutPos.z;
			SetUseLighting(FALSE);
			DrawCube3D(
				(Vector3DX::vget((float)(x - xput / 2), (float)(y - yput / 2), (float)(z - zput / 2)) * CellScale).get(),
				(Vector3DX::vget((float)(x + xput - xput / 2), (float)(y + yput - yput / 2), (float)(z + zput - zput / 2)) * CellScale).get(),
				GetColor(0, 64, 0), GetColor(0, 0, 0), FALSE);
			SetUseLighting(TRUE);
			clsDx();
			printfDx("%d,%d,%d\n", (x - xput / 2), (y - yput / 2), (z - zput / 2));
#endif
		}
		//
		void		BackGroundClass::Dispose(void) noexcept {
			this->m_ObjSky.Dispose();
			SaveCellsFile();
		}
	}
}
