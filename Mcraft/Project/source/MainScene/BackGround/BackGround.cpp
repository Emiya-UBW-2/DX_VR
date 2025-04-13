#include	"BackGround.hpp"
#include	<random>

const FPS_n2::BackGround::BackGroundControl* SingletonBase<FPS_n2::BackGround::BackGroundControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		class PerlinNoise {
		private:
			//メンバ変数
			std::array<uint_fast8_t, 512> perlinNoise{ 0 };

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
				return makeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
			}
			constexpr float getGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return makeGrad(hash_ & 15, x_, y_, z_);
			}

			float setNoise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				const std::size_t x_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x_)) & 255);
				const std::size_t y_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y_)) & 255);
				const std::size_t z_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z_)) & 255);
				x_ -= std::floor(x_);
				y_ -= std::floor(y_);
				z_ -= std::floor(z_);
				const float u = getFade(x_);
				const float v = getFade(y_);
				const float w = getFade(z_);
				const std::size_t a0 = static_cast<std::size_t>(this->perlinNoise[x_int] + y_int);
				const std::size_t a1 = static_cast<std::size_t>(this->perlinNoise[a0] + z_int);
				const std::size_t a2 = static_cast<std::size_t>(this->perlinNoise[a0 + 1] + z_int);
				const std::size_t b0 = static_cast<std::size_t>(this->perlinNoise[x_int + 1] + y_int);
				const std::size_t b1 = static_cast<std::size_t>(this->perlinNoise[b0] + z_int);
				const std::size_t b2 = static_cast<std::size_t>(this->perlinNoise[b0 + 1] + z_int);

				return getLerp(w,
					getLerp(v,
						getLerp(u, getGrad(this->perlinNoise[a1], x_, y_, z_), getGrad(this->perlinNoise[b1], x_ - 1, y_, z_)),
						getLerp(u, getGrad(this->perlinNoise[a2], x_, y_ - 1, z_), getGrad(this->perlinNoise[b2], x_ - 1, y_ - 1, z_))),
					getLerp(v,
						getLerp(u, getGrad(this->perlinNoise[a1 + 1], x_, y_, z_ - 1), getGrad(this->perlinNoise[b1 + 1], x_ - 1, y_, z_ - 1)),
						getLerp(u, getGrad(this->perlinNoise[a2 + 1], x_, y_ - 1, z_ - 1), getGrad(this->perlinNoise[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
			}
			float setOctaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				float noise_value = 0.f;
				float amp = 1.0f;
				for (std::size_t loop = 0; loop < octaves_; ++loop) {
					noise_value += setNoise(x_, y_, z_) * amp;
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
				setSeed(seed_);
			}

			//SEED値を設定する
			void setSeed(const std::uint_fast32_t seed_) noexcept {
				for (std::size_t loop = 0; loop < 256; ++loop) {
					this->perlinNoise[loop] = static_cast<uint_fast8_t>(loop);
				}
				std::shuffle(this->perlinNoise.begin(), this->perlinNoise.begin() + 256, std::default_random_engine(seed_));
				for (std::size_t loop = 0; loop < 256; ++loop) {
					this->perlinNoise[256 + loop] = this->perlinNoise[loop];
				}
			}
			//オクターブ無しノイズを取得する
			float noise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return setNoise(x_, y_, z_) * 0.5f + 0.5f;
			}
			//オクターブ有りノイズを取得する
			float octaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return setOctaveNoise(octaves_, x_, y_, z_) * 0.5f + 0.5f;
			}
		};
		//
		bool		BackGroundControl::AddCubeX_CanAddPlane(const CellsData& cellx, int xmin, int xmax, int cy, int cz, int id) noexcept {
			for (int x = xmin; x <= xmax; ++x) {
				if ((cellx.GetCellBuf(x, cy, cz).m_FillInfo & (1 << id)) == 0) {
					return true;
				}
			}
			return false;
		}
		bool		BackGroundControl::AddCubeZ_CanAddPlane(const CellsData& cellx, int cx, int cy, int zmin, int zmax, int id) noexcept {
			for (int z = zmin; z <= zmax; ++z) {
				if ((cellx.GetCellBuf(cx, cy, z).m_FillInfo & (1 << id)) == 0) {
					return true;
				}
			}
			return false;
		}
		//
		void		BackGroundControl::AddPlaneXPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = zmin % 2 == 0;
				Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneXMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = zmin % 2 == 0;
				Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneYPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = zmin % 2 == 0;
				Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 0;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneYMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = zmin % 2 == 0;
				Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 2;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + zscale);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneZPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.m_scaleRate;
				Xofs = xmax % 2 == 0;
				Yofs = ((cellx.GetCellBuf(xmin, y, z).m_Cell - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b001).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + xscale);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 1];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b101).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 2];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b011).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + xscale);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 3];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b111).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneZMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.m_scaleRate;
				Xofs = xmax % 2 == 0;
				Yofs = ((cellx.GetCellBuf(xmin, y, z).m_Cell - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b010).get();
				V.norm = Vector3DX::back().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + xscale);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 1];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b110).get();
				V.norm = Vector3DX::back().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 2];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b000).get();
				V.norm = Vector3DX::back().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + xscale);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& V = pTarget->SetInVert()[ZERO + 3];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b100).get();
				V.norm = Vector3DX::back().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		//
		void		BackGroundControl::AddCubes(size_t id) noexcept {
			auto& cellx = this->m_CellxN.at(id);
			auto& Draws = this->m_Draws.at(id);
			Draws.m_vert32.ResetNum();
			auto center = cellx.GetPoint(Draws.m_CamPos);
			//X
			for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {
				if (!CalcXYActive(Draws.m_CamVec, DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, z - center.z)) { continue; }

				bool CheckFillZ = (cellx.m_scaleRate != 1) && ((center.z + DrawMinZMinus <= z) && (z <= center.z + DrawMinZPlus));
				bool CheckInsideZ = (cellx.m_scaleRate != 1) && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.m_All <= y) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(Draws.m_CamVec, &xMaxminT, &xMaxmaxT, y - center.y, z - center.z)) { continue; }

					bool CheckFill = CheckFillZ && ((center.y + DrawMinYMinus <= y) && (y <= center.y + DrawMinYPlus));
					bool CheckInside = CheckInsideZ && ((center.y + DrawMinYMinus < y) && (y < center.y + DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					int8_t selectBlock = s_EmptyBlick;
					for (int x = center.x + xMaxminT, XMax = center.x + xMaxmaxT; x <= XMax; ++x) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(x == XMax)
							|| (CheckInside && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus)))
							|| (CellBuff.IsEmpty())
							|| (!isHitmin && (selectBlock != CellBuff.m_Cell))
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;

								bool Fill = CheckFill && (xmin <= center.x + DrawMinXPlus && center.x + DrawMinXMinus <= xmax);
								if (z >= center.z) {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 5)) {
										AddPlaneZMinus(&Draws.m_vert32, cellx, xmin, xmax, y, z, true);
									}
								}
								else {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 4)) {
										AddPlaneZPlus(&Draws.m_vert32, cellx, xmin, xmax, y, z, true);
									}
								}
								//この場合だけもう一回判定させるドン
								if (selectBlock != CellBuff.m_Cell) {
									--x;
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = x;
								selectBlock = CellBuff.m_Cell;
							}
							xmax = x;
						}
					}
				}
			}
			//Z
			for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {
				if (!CalcYZActive(Draws.m_CamVec, x - center.x, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = (cellx.m_scaleRate != 1) && ((center.x + DrawMinXMinus <= x) && (x <= center.x + DrawMinXPlus));
				bool CheckInsideX = (cellx.m_scaleRate != 1) && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.m_All <= y) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(Draws.m_CamVec, x - center.x, y - center.y, &zMaxminT, &zMaxmaxT)) { continue; }

					bool CheckFill = CheckFillX && ((center.y + DrawMinYMinus <= y) && (y <= center.y + DrawMinYPlus));
					bool CheckInside = CheckInsideX && ((center.y + DrawMinYMinus < y) && (y < center.y + DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					int8_t selectBlock = s_EmptyBlick;
					for (int z = center.z + zMaxminT, ZMax = center.z + zMaxmaxT; z <= ZMax; ++z) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(z == center.z + zMaxmaxT)
							|| (CheckInside && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus)))
							|| (CellBuff.IsEmpty())
							|| (!isHitmin && (selectBlock != CellBuff.m_Cell))
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								bool Fill = CheckFill && (zmin <= center.z + DrawMinZPlus && center.z + DrawMinZMinus <= zmax);
								if (x >= center.x) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 1)) {
										AddPlaneXMinus(&Draws.m_vert32, cellx, x, y, zmin, zmax, true);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 0)) {
										AddPlaneXPlus(&Draws.m_vert32, cellx, x, y, zmin, zmax, true);
									}
								}
								if (y >= center.y) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 3)) {
										AddPlaneYMinus(&Draws.m_vert32, cellx, x, y, zmin, zmax, true);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 2)) {
										AddPlaneYPlus(&Draws.m_vert32, cellx, x, y, zmin, zmax, true);
									}
								}
								//この場合だけもう一回判定させるドン
								if (selectBlock != CellBuff.m_Cell) {
									--z;
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = z;
								selectBlock = CellBuff.m_Cell;
							}
							zmax = z;
						}
					}
				}
			}
		}
		void		BackGroundControl::AddShadowCubes(size_t id) noexcept {
			auto& cellx = this->m_CellxN.at(id);
			auto& DrawsSB = this->m_DrawsSB.at(id);
			DrawsSB.m_vert32.ResetNum();
			auto center = cellx.GetPoint(DrawsSB.m_CamPos);
			//X
			for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {

				bool CheckInsideZ = (cellx.m_scaleRate != 1) && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.m_All <= y) { continue; }

					bool CheckInside = CheckInsideZ && ((center.y + DrawMinYMinus < y) && (y < center.y + DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(x == center.x + DrawMaxXPlus)
							|| (CheckInside && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (DrawsSB.m_light.z > 0.f) {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 5)) {
										AddPlaneZMinus(&DrawsSB.m_vert32, cellx, xmin, xmax, y, z, false);
									}
								}
								else {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 4)) {
										AddPlaneZPlus(&DrawsSB.m_vert32, cellx, xmin, xmax, y, z, false);
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
			//Z
			for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {

				bool CheckInsideX = (cellx.m_scaleRate != 1) && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.m_All <= y) { continue; }

					bool CheckInside = CheckInsideX && ((DrawMinYMinus < y - center.y) && (y - center.y < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(z == DrawMaxZPlus)
							|| (CheckInside && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (DrawsSB.m_light.x > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 1)) {
										AddPlaneXMinus(&DrawsSB.m_vert32, cellx, x, y, zmin, zmax, false);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 0)) {
										AddPlaneXPlus(&DrawsSB.m_vert32, cellx, x, y, zmin, zmax, false);
									}
								}
								if (DrawsSB.m_light.y > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 3)) {
										AddPlaneYMinus(&DrawsSB.m_vert32, cellx, x, y, zmin, zmax, false);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 2)) {
										AddPlaneYPlus(&DrawsSB.m_vert32, cellx, x, y, zmin, zmax, false);
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
		//
		void		BackGroundControl::AddSetShadowPlaneXPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
		}
		void		BackGroundControl::AddSetShadowPlaneXMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
		}
		void		BackGroundControl::AddSetShadowPlaneYPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
		}
		void		BackGroundControl::AddSetShadowPlaneYMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
		}
		void		BackGroundControl::AddSetShadowPlaneZPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(xmin, y, z, 0b001).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(xmax, y, z, 0b101).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(xmin, y, z, 0b011).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(xmax, y, z, 0b111).get();
		}
		void		BackGroundControl::AddSetShadowPlaneZMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(xmin, y, z, 0b010).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(xmax, y, z, 0b110).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(xmin, y, z, 0b000).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(xmax, y, z, 0b100).get();
		}

		void		BackGroundControl::AddSetShadowCubes(size_t id) noexcept {
			auto& cellx = this->m_CellxN.at(id);
			auto& DrawsSS = this->m_DrawsSS.at(id);
			DrawsSS.m_vert32.ResetNum();
			auto center = cellx.GetPoint(DrawsSS.m_CamPos);
			//X
			for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {
				if (!CalcXYActive(DrawsSS.m_CamVec, DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, z - center.z)) { continue; }

				bool CheckFillZ = (cellx.m_scaleRate != 1) && ((center.z + DrawMinZMinus <= z) && (z <= center.z + DrawMinZPlus));
				bool CheckInsideZ = (cellx.m_scaleRate != 1) && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.m_All <= y) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(DrawsSS.m_CamVec, &xMaxminT, &xMaxmaxT, y - center.y, z - center.z)) { continue; }

					bool CheckFill = CheckFillZ && ((center.y + DrawMinYMinus <= y) && (y <= center.y + DrawMinYPlus));
					bool CheckInside = CheckInsideZ && ((center.y + DrawMinYMinus < y) && (y < center.y + DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int x = center.x + xMaxminT, XMax = center.x + xMaxmaxT; x <= XMax; ++x) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(x == XMax)
							|| (CheckInside && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								bool Fill = CheckFill && (((xmin <= center.x + DrawMinXMinus + 1) && (center.x + DrawMinXMinus + 1 <= xmax)) || ((xmin <= center.x + DrawMinXPlus - 1) && (center.x + DrawMinXPlus - 1 <= xmax)));
								if (z >= center.z) {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 5)) {
										AddSetShadowPlaneZMinus(&DrawsSS.m_vert32, cellx, xmin, xmax, y, z);
									}
								}
								else {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 4)) {
										AddSetShadowPlaneZPlus(&DrawsSS.m_vert32, cellx, xmin, xmax, y, z);
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
			//Z
			for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {
				if (!CalcYZActive(DrawsSS.m_CamVec, x - center.x, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = (cellx.m_scaleRate != 1) && ((center.x + DrawMinXMinus <= x) && (x <= center.x + DrawMinXPlus));
				bool CheckInsideX = (cellx.m_scaleRate != 1) && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.m_All <= y) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(DrawsSS.m_CamVec, x - center.x, y - center.y, &zMaxminT, &zMaxmaxT)) { continue; }

					bool CheckFill = CheckFillX && ((center.y + DrawMinYMinus <= y) && (y <= center.y + DrawMinYPlus));
					bool CheckInside = CheckInsideX && ((center.y + DrawMinYMinus < y) && (y < center.y + DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int z = center.z + zMaxminT, ZMax = center.z + zMaxmaxT; z <= ZMax; ++z) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(z == center.z + zMaxmaxT)
							|| (CheckInside && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								bool Fill = CheckFill && (((zmin <= center.z + DrawMinZMinus + 1) && (center.z + DrawMinZMinus + 1 <= zmax)) || ((zmin <= center.z + DrawMinZPlus - 1) && (center.z + DrawMinZPlus - 1 <= zmax)));
								if (x >= center.x) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 1)) {
										AddSetShadowPlaneXMinus(&DrawsSS.m_vert32, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 0)) {
										AddSetShadowPlaneXPlus(&DrawsSS.m_vert32, cellx, x, y, zmin, zmax);
									}
								}
								if (y >= center.y) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 3)) {
										AddSetShadowPlaneYMinus(&DrawsSS.m_vert32, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 2)) {
										AddSetShadowPlaneYPlus(&DrawsSS.m_vert32, cellx, x, y, zmin, zmax);
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
		//
		bool		BackGroundControl::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			auto& cell = this->m_CellxN.front();
			auto Start = cell.GetPoint(StartPos);
			auto End = cell.GetPoint(*EndPos);// *EndPos

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int x, int y, int z) {
								if (!cell.isInside(y)) { return false; }
								if (cell.GetCellBuf(x, y, z).IsEmpty()) { return false; }
								if (cell.GetCellBuf(x, y, z).IsOcclusion()) { return false; }
								Vector3DX MinPos = cell.GetPos(x, y, z) + Vector3DX::one() * -0.1f * Scale3DRate;
								Vector3DX MaxPos = cell.GetPos(x + 1, y + 1, z + 1) + Vector3DX::one() * 0.1f * Scale3DRate;
								Vector3DX tmpEndPos = *EndPos;
								Vector3DX tmpNormal;
								if (ColRayBox(StartPos, &tmpEndPos, MinPos, MaxPos, &tmpNormal)) {
									if ((tmpEndPos - StartPos).sqrMagnitude() < (*EndPos - StartPos).sqrMagnitude()) {
										*EndPos = tmpEndPos;
										if (Normal) {
											*Normal = tmpNormal;
										}
									}
									isHit = true;
									//return true;
								}
								return false;
							});
						if (isHit) { return true; }
					}
				}
			}
			return false;
		}
		bool		BackGroundControl::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept {
			auto MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			auto& cell = this->m_CellxN.front();
			auto Start = cell.GetPoint(StartPos);
			auto End = cell.GetPoint(*EndPos);// *EndPos

			for (int xm = -3; xm <= 3; ++xm) {
				for (int ym = 3; ym <= 6; ++ym) {
					for (int zm = -3; zm <= 3; ++zm) {
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int x, int y, int z) {
								if (!cell.isInside(y)) { return false; }
								if (cell.GetCellBuf(x, y, z).IsEmpty()) { return false; }
								if (cell.GetCellBuf(x, y, z).IsOcclusion()) { return false; }
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
							bool isHit = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
									isHit = true;
									break;// 当たっていたらループから抜ける
								}
							}
							if (!isHit) {
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
					for (int loop = 0; loop < 32; ++loop) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
						bool HitF = false;
						for (auto& kabe : kabes) {
							// プレイヤーと当たっているかを判定
							if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale3DRate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
								bool isHit = false;
								for (auto& kabe2 : kabes) {
									// 当たっていたらループを抜ける
									if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
										isHit = true;
										break;
									}
								}
								if (!isHit) {// 全てのポリゴンと当たっていなかったらここでループ終了
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
		void		BackGroundControl::LoadCellsFile(void) noexcept {
			auto& cell = this->m_CellxN.front();
			{
				cell.SetScale(static_cast<int>(pow(MulPer, 0)));

				std::ifstream fin{};
				fin.open("data/Map.txt", std::ios::in | std::ios::binary);
				this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
				fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase.at(0))) * 256 * 256 * 256);
				fin.close();
				for (int xm = 0; xm < cell.m_All; ++xm) {
					for (int ym = 0; ym < cell.m_All; ++ym) {
						for (int zm = 0; zm < cell.m_All; ++zm) {
							cell.SetCellBuf(xm, ym, zm).m_Cell = this->m_CellBase[cell.GetCellNum(xm, ym, zm)];
						}
					}
				}
			}
			//一部をペースト
			//LoadCellsClip(0, 128, 0);
			/*
			for (int x = 0; x < cell.m_All; ++x) {
				if (x - cell.m_All/2 > -72) {
					for (int z = 0; z < cell.m_All; ++z) {
						for (int y = cell.m_All; y >=130+15 ; --y) {
							cell.SetCellBuf(x, y, z).m_Cell = cell.GetCellBuf(x, y - 15, z).m_Cell;
						}
					}
				}
			}
			//*/
			/*
			for (int x = 0; x < cell.m_All; ++x) {
				for (int z = 0; z < cell.m_All; ++z) {
					for (int y = cell.m_All; y >= 133; --y) {
						cell.SetCellBuf(x, y, z).m_Cell = 0;
					}
				}
			}
			//*/
		}
		void		BackGroundControl::SaveCellsFile(void) noexcept {
			{
				auto& cell = this->m_CellxN.front();

				this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
				for (int xm = 0; xm < cell.m_All; ++xm) {
					for (int ym = 0; ym < cell.m_All; ++ym) {
						for (int zm = 0; zm < cell.m_All; ++zm) {
							this->m_CellBase[cell.GetCellNum(xm, ym, zm)] = cell.SetCellBuf(xm, ym, zm).m_Cell;
						}
					}
				}

				std::ofstream fout{};
				fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
				fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase.at(0))) * 256 * 256 * 256);
				fout.close();  //ファイルを閉じる
			}
			//一部を切り取って保存
			/*
			int XMin = 37;
			int XMax = 144;
			int YMin = 128;
			int YMax = 205;
			int ZMin = 91;
			int ZMax = 153;
			SaveCellsClip(XMin, XMax, YMin, YMax, ZMin, ZMax);
			//*/
		}
		void		BackGroundControl::LoadCellsClip(int xpos, int ypos, int zpos) noexcept {
			auto& cell = this->m_CellxN.front();
			int XTotal = 1;
			int YTotal = 1;
			int ZTotal = 1;
			std::ifstream fin{};
			fin.open("data/Build/Map.txt", std::ios::in | std::ios::binary);
			fin.read((char*)&XTotal, sizeof(XTotal));
			fin.read((char*)&YTotal, sizeof(YTotal));
			fin.read((char*)&ZTotal, sizeof(ZTotal));
			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase.at(0))) * XTotal * YTotal * ZTotal);
			fin.close();  //ファイルを閉じる

			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						cell.SetCellBuf(xm + xpos, ym + ypos, zm + zpos).m_Cell =
							this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)];
					}
				}
			}
		}
		void		BackGroundControl::SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax) noexcept {
			auto& cell = this->m_CellxN.front();
			int XTotal = (XMax - XMin + 1);
			int YTotal = (YMax - YMin + 1);
			int ZTotal = (ZMax - ZMin + 1);

			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)] = cell.SetCellBuf(XMin + xm, YMin + ym, ZMin + zm).m_Cell;
					}
				}
			}

			std::ofstream fout{};
			fout.open("data/Build/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)&XTotal, sizeof(XTotal));
			fout.write((char*)&YTotal, sizeof(YTotal));
			fout.write((char*)&ZTotal, sizeof(ZTotal));
			fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase.at(0)) * XTotal * YTotal * ZTotal));
			fout.close();  //ファイルを閉じる
		}
		//
		void		BackGroundControl::SettingChange(void) noexcept {
			this->m_ShadowRate = static_cast<int>(pow(MulPer, 1));
			this->m_BaseRate = static_cast<int>(pow(MulPer, 1));
		}
		//
		void		BackGroundControl::SetBlick(int x, int y, int z, int8_t select) noexcept {
			auto& cell = this->m_CellxN.front();
			if (!cell.isInside(y)) { return; }
			//テクスチャだけ変える
			/*
			if (cell.SetCellBuf(x, y, z).IsEmpty()) { return; }
			cell.SetCellBuf(x, y, z).m_Cell = select;
			return;
			//*/
			//
			cell.SetCellBuf(x, y, z).m_Cell = select;
			//簡易版を更新
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				auto& cell1 = this->m_CellxN.at(loop);

				int xm = x / cell1.m_scaleRate;
				int ym = y / cell1.m_scaleRate;
				int zm = z / cell1.m_scaleRate;

				cell1.SetCellBuf(xm, ym, zm).m_Cell = cell.isFill(xm, ym, zm, cell1.m_scaleRate);
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				int xm = x / cellx.m_scaleRate;
				int ym = y / cellx.m_scaleRate;
				int zm = z / cellx.m_scaleRate;
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
		//
		void		BackGroundControl::Load(void) noexcept {
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			this->m_tex.Load("data/tex.png");
			this->m_norm.Load("data/nrm.png");
		}
		//
		void		BackGroundControl::Init(void) noexcept {
			//空
			this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int loop = 0, num = this->m_ObjSky.GetMaterialNum(); loop < num; ++loop) {
				//this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
				this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.7f, 0.7f, 0.7f, 1.f));
				this->m_ObjSky.SetMaterialAmbColor(loop, GetColorF(0.f, 0.f, 0.f, 1.f));
			}
			if (false) {
				//空っぽ
				auto& cell = this->m_CellxN.front();
				cell.SetScale(static_cast<int>(pow(MulPer, 0)));
				for (int x = 0; x < cell.m_All; ++x) {
					for (int z = 0; z < cell.m_All; ++z) {
						for (int y = 0; y < cell.m_All; ++y) {
							if (y <= cell.m_All * 1 / 4) {
								cell.SetCellBuf(x, y, z).m_Cell = 1;
								continue;
							}
							cell.SetCellBuf(x, y, z).m_Cell = s_EmptyBlick;
						}
					}
				}
				SaveCellsFile();
			}
			else if (true) {
				//空っぽ
				auto& cell = this->m_CellxN.front();
				cell.SetScale(static_cast<int>(pow(MulPer, 0)));

				PerlinNoise ns(GetRand(100));
				for (int x = 0; x < cell.m_All; ++x) {
					for (int z = 0; z < cell.m_All; ++z) {
						for (int y = 0; y < cell.m_All; ++y) {
							cell.SetCellBuf(x, y, z).m_Cell = s_EmptyBlick;
						}
					}
				}

				int Size = 17;
				this->m_MazeControl.createMaze(Size, Size);
				int Rate = 6;
				int Heights = 10;
				//*
				int Edge = -Rate;
				int EdgeP = -0;

				for (int x = 0; x < cell.m_All; x += 2) {
					for (int z = 0; z < cell.m_All; z += 2) {
						int xPos = -Size * Rate / 2 + x;
						int zPos = -Size * Rate / 2 + z;
						if ((-EdgeP < x && x < Size * Rate + EdgeP - 1) && (-EdgeP < z && z < Size * Rate + EdgeP - 1)) {
							if (GetRand(100) < 30) {
								int y = cell.m_All * 1 / 4 - 15;
								cell.SetCellBuf(cell.m_Half + xPos + 0, y, cell.m_Half + zPos + 0).m_Cell = 1;
								cell.SetCellBuf(cell.m_Half + xPos + 0, y, cell.m_Half + zPos + 1).m_Cell = 1;
								cell.SetCellBuf(cell.m_Half + xPos + 1, y, cell.m_Half + zPos + 0).m_Cell = 1;
								cell.SetCellBuf(cell.m_Half + xPos + 1, y, cell.m_Half + zPos + 1).m_Cell = 1;
							}
						}
					}
				}

				for (int x = 0; x < cell.m_All; ++x) {
					for (int z = 0; z < cell.m_All; ++z) {
						int xPos = -Size * Rate / 2 + x;
						int zPos = -Size * Rate / 2 + z;

						if ((-EdgeP <= x && x <= Size * Rate + EdgeP - 1) && (-EdgeP <= z && z <= Size * Rate + EdgeP - 1)) {
							if ((x == -EdgeP || x == Size * Rate + EdgeP - 1) || (z == -EdgeP || z == Size * Rate + EdgeP - 1)) {
								for (int y = 0; y <= cell.m_All * 1 / 4; ++y) {
									cell.SetCellBuf(cell.m_Half + xPos, y, cell.m_Half + zPos).m_Cell = 1;
								}
							}
						}
						if ((-EdgeP < x && x < Size * Rate + EdgeP - 1) && (-EdgeP < z && z < Size * Rate + EdgeP - 1)) {
							auto Height = static_cast<int>(ns.octaveNoise(2,
								(static_cast<float>(x)) / (Size * Rate - 1),
								(static_cast<float>(z)) / (Size * Rate - 1)) * static_cast<float>(cell.m_All * 1 / 20));
							for (int y = 0; y <= Height; ++y) {
								cell.SetCellBuf(cell.m_Half + xPos, y, cell.m_Half + zPos).m_Cell = 2;
							}
						}
					}
				}
				//*/
				for (int z = -Edge; z < Size * Rate + Edge; ++z) {
					for (int x = -Edge; x < Size * Rate + Edge; ++x) {
						auto SetWall = [&](int xt,int zt) {
							int xPos = -Size * Rate / 2 + x + xt;
							int zPos = -Size * Rate / 2 + z + zt;
							auto Height = static_cast<int>(ns.octaveNoise(2, 
								(static_cast<float>(x + xt)) / (Size * Rate),
								(static_cast<float>(z + zt)) / (Size * Rate)) * static_cast<float>(cell.m_All * 1 / 20));
							for (int y = Height; y <= Height + Heights; ++y) {
								cell.SetCellBuf(cell.m_Half + xPos, y, cell.m_Half + zPos).m_Cell = 2;
							}
						};
						int xp = x / Rate;
						int zp = z / Rate;
						if (!this->m_MazeControl.PosIsPath(xp, zp) && (x % Rate == 0) && (z % Rate == 0)) {
							SetWall(0, 0);
							if ((xp > 0) && !this->m_MazeControl.PosIsPath(xp - 1, zp)) {
								for (int xt = 0; xt < Rate; ++xt) {
									SetWall(-xt, 0);
								}
							}
							if ((zp > Size - 1) && !this->m_MazeControl.PosIsPath(xp, zp - 1)) {
								for (int zt = 0; zt < Rate; ++zt) {
									SetWall(0, -zt);
								}
							}
							if ((xp < 0) && !this->m_MazeControl.PosIsPath(xp + 1, zp)) {
								for (int xt = 0; xt < Rate; ++xt) {
									SetWall(xt, 0);
								}
							}
							if ((zp < Size - 1) && !this->m_MazeControl.PosIsPath(xp, zp + 1)) {
								for (int zt = 0; zt < Rate; ++zt) {
									SetWall(0, zt);
								}
							}
						}
					}
				}

				SaveCellsFile();
			}
			else if (false) {
				PerlinNoise ns(GetRand(100));
				auto& cell = this->m_CellxN.front();
				cell.SetScale(static_cast<int>(pow(MulPer, 0)));
				for (int x = 0; x < cell.m_All; ++x) {
					for (int z = 0; z < cell.m_All; ++z) {
						auto Height = static_cast<int>(ns.octaveNoise(2, (static_cast<float>(x)) / cell.m_All, (static_cast<float>(z)) / cell.m_All) * static_cast<float>(cell.m_All * 4 / 5));
						for (int y = 0; y < cell.m_All; ++y) {
							if (y <= Height) {
								cell.SetCellBuf(x, y, z).m_Cell = 1;
								continue;
							}
							cell.SetCellBuf(x, y, z).m_Cell = s_EmptyBlick;
						}
					}
				}
				SaveCellsFile();
			}
			else {
				LoadCellsFile();
			}
			//簡略版を制作
			auto& cell = this->m_CellxN.front();
			for (int select = 1; select < TotalCellLayer; ++select) {
				auto& cell1 = this->m_CellxN.at(select);
				cell1.SetScale(static_cast<int>(pow(MulPer, select)));
				for (int xm = 0; xm < cell1.m_All; ++xm) {
					for (int ym = 0; ym < cell1.m_All; ++ym) {
						for (int zm = 0; zm < cell1.m_All; ++zm) {
							cell1.SetCellBuf(xm, ym, zm).m_Cell = cell.isFill(xm, ym, zm, cell1.m_scaleRate);
						}
					}
				}
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				for (int x = 0; x < cellx.m_All; ++x) {
					for (int y = 0; y < cellx.m_All; ++y) {
						for (int z = 0; z < cellx.m_All; ++z) {
							if (cellx.GetCellBuf(x, y, z).IsEmpty()) { continue; }
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
			Param.Power = 500.f;													// スペキュラハイライトの鮮明度
			SetMaterialParam(Param);
			//
			constexpr size_t size = (DrawMax + DrawMax) * (DrawMax + DrawMax) * (DrawMax + DrawMax) / 2 * 3 / 100;
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				this->m_Draws.at(loop).m_vert32.Init(size);
				this->m_DrawsSS.at(loop).m_vert32.Init(size);
				this->m_DrawsSB.at(loop).m_vert32.Init(size);
			}
			//
			this->m_Jobs.at(0).Init(
				[&]() {
					AddCubes(0);
				},
				[&]() {
					size_t id = 0;
					this->m_Draws.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_Draws.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_Draws.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_Draws.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(1).Init(
				[&]() {
					AddCubes(1);
				},
				[&]() {
					size_t id = 1;
					this->m_Draws.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_Draws.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_Draws.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_Draws.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(2).Init(
				[&]() {
					AddCubes(2);
				},
				[&]() {
					size_t id = 2;
					this->m_Draws.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_Draws.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_Draws.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_Draws.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(3).Init(
				[&]() {
					AddCubes(3);
				},
				[&]() {
					size_t id = 3;
					this->m_Draws.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_Draws.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_Draws.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_Draws.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + 0)).Init(
				[&]() {
					AddSetShadowCubes(0);
				},
				[&]() {
					size_t id = 0;
					this->m_DrawsSS.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSS.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSS.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_DrawsSS.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + 1)).Init(
				[&]() {
					AddSetShadowCubes(1);
				},
				[&]() {
					size_t id = 1;
					this->m_DrawsSS.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSS.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSS.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_DrawsSS.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + 2)).Init(
				[&]() {
					AddSetShadowCubes(2);
				},
				[&]() {
					size_t id = 2;
					this->m_DrawsSS.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSS.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSS.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_DrawsSS.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + 3)).Init(
				[&]() {
					AddSetShadowCubes(3);
				},
				[&]() {
					size_t id = 3;
					this->m_DrawsSS.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSS.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSS.at(id).m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - this->m_DrawsSS.at(id).m_CamPos).normalized();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 0)).Init(
				[&]() {
					AddShadowCubes(0);
				},
				[&]() {
					size_t id = 0;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 1)).Init(
				[&]() {
					AddShadowCubes(1);
				},
				[&]() {
					size_t id = 1;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 2)).Init(
				[&]() {
					AddShadowCubes(2);
				},
				[&]() {
					size_t id = 2;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 3)).Init(
				[&]() {
					AddShadowCubes(3);
				},
				[&]() {
					size_t id = 3;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			);
			SettingChange();
			this->m_ThreadCounter = 0;
		}
		//
		void		BackGroundControl::Execute(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
#if defined(DEBUG) & EDITBLICK
			{
				auto& cell = this->m_CellxN.front();
				auto Put = cell.GetPoint(PutPos);
				printfDx("%d,%d,%d\n", Put.x, Put.y, Put.z);
				auto* Pad = PadControl::Instance();
				LenMouse += Pad->GetMouseWheelRot();
				if (Pad->GetPadsInfo(Controls::PADS::SHOT).GetKey().trigger()) {
					blickselect = 1;
				}
				if (Pad->GetPadsInfo(Controls::PADS::AIM).GetKey().trigger()) {
					blickselect = 0;
				}
				if (Pad->GetPadsInfo(Controls::PADS::JUMP).GetKey().trigger()) {
					SaveCellsFile();
				}
			}
#endif
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				if ((loop != 0) && (loop != this->m_ThreadCounter)) { continue; }
				//
				if (this->m_BaseRate < this->m_CellxN.at(loop).m_scaleRate) {
					this->m_Draws.at(loop).m_vert32.Disable();
				}
				else {
					this->m_Jobs.at(loop).Execute();
				}
				//
				if ((OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) || (this->m_ShadowRate < this->m_CellxN.at(loop).m_scaleRate)) {
					this->m_DrawsSS.at(loop).m_vert32.Disable();
					this->m_DrawsSB.at(loop).m_vert32.Disable();
				}
				else {
					this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + loop)).Execute();
					this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + loop)).Execute();
				}
			}
			++this->m_ThreadCounter %= TotalCellLayer;
		}
		//
		void		BackGroundControl::BG_Draw(void) const noexcept {
			SetUseLighting(false);
			this->m_ObjSky.DrawModel();
			SetUseLighting(true);
		}
		void		BackGroundControl::Shadow_Draw(void) const noexcept {
			for (auto& Vert : this->m_DrawsSB) {
				Vert.m_vert32.Draw(this->m_tex);
			}
		}
		void		BackGroundControl::SetShadow_Draw_Rigid(void) const noexcept {
			SetUseTextureToShader(0, this->m_tex.get());
			for (auto& Vert : this->m_DrawsSS) {
				Vert.m_vert32.DrawByShader();
			}
			SetUseTextureToShader(0, InvalidID);
		}
		void		BackGroundControl::Draw(void) const noexcept {
			float Near = GetCameraNear();
			float Far = GetCameraFar();
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;

			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				auto& Vert = this->m_Draws.at(loop).m_vert32;
				if (Vert.GetOutNum() > 0) {
					float Min = (this->m_CellxN.at(loop).m_scaleRate != 1) ? MinLimit * this->m_CellxN.at(loop).m_Scale : 0;
					if (!(Min < Far && Near < MaxLimit * this->m_CellxN.at(loop).m_Scale)) { continue; }
					Vert.Draw(this->m_tex);
				}
			}
#if defined(DEBUG) & EDITBLICK
			int x = static_cast<int>(PutPos.x / CellScale), y = static_cast<int>(PutPos.y / CellScale), z = static_cast<int>(PutPos.z / CellScale);
			SetUseLighting(false);
			DrawCube3D(
				(Vector3DX::vget(static_cast<float>(x - xput / 2), static_cast<float>(y - yput / 2), static_cast<float>(z - zput / 2)) * CellScale).get(),
				(Vector3DX::vget(static_cast<float>(x + xput - xput / 2), static_cast<float>(y + yput - yput / 2), static_cast<float>(z + zput - zput / 2)) * CellScale).get(),
				GetColor(0, 64, 0), GetColor(0, 0, 0), false);
			SetUseLighting(true);
#endif
		}
		//
		void		BackGroundControl::Dispose(void) noexcept {
			for (auto& t : m_Jobs) {
				t.Dispose();
			}
			this->m_ObjSky.Dispose();
			SaveCellsFile();
		}
	}
}
