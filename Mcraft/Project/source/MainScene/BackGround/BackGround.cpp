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
		bool		BackGroundClass::AddCubeX_CanAddPlane(const CellsData& cellx, int xmin, int xmax, int cy, int cz, int id) noexcept {
			for (int x = xmin; x <= xmax; ++x) {
				if ((cellx.GetCellBuf(x, cy, cz).m_FillInfo & (1 << id)) == 0) {
					return true;
				}
			}
			return false;
		}
		bool		BackGroundClass::AddCubeZ_CanAddPlane(const CellsData& cellx, int cx, int cy, int zmin, int zmax, int id) noexcept {
			for (int z = zmin; z <= zmax; ++z) {
				if ((cellx.GetCellBuf(cx, cy, z).m_FillInfo & (1 << id)) == 0) {
					return true;
				}
			}
			return false;
		}
		//
		void		BackGroundClass::AddPlaneXPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32s.at(id);
			Verts.AllocatePlane();

			int zscale = (zmax - zmin + 1) * cellx.scaleRate;

			int Xofs = zmin % 2 == 0;
			int Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 16.f;

			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto ZERO = Verts.m_32Num * 4 - 4;

			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::right().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b101));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::right().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b100));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::right().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b111));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::right().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b110));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
		}
		void		BackGroundClass::AddPlaneXMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32s.at(id);
			Verts.AllocatePlane();

			int zscale = (zmax - zmin + 1) * cellx.scaleRate;

			int Xofs = zmin % 2 == 0;
			int Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 16.f;

			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::left().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b011));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::left().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b010));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::left().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b001));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::left().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b000));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
		}
		void		BackGroundClass::AddPlaneYPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32s.at(id);
			Verts.AllocatePlane();

			int zscale = (zmax - zmin + 1) * cellx.scaleRate;

			int Xofs = zmin % 2 == 0;
			int Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 0;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 16.f;

			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::up().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b111));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::up().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b110));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::up().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b011));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::up().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b010));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
		}
		void		BackGroundClass::AddPlaneYMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32s.at(id);
			Verts.AllocatePlane();

			int zscale = (zmax - zmin + 1) * cellx.scaleRate;

			int Xofs = zmin % 2 == 0;
			int Yofs = ((cellx.GetCellBuf(x, y, zmin).m_Cell - 1) * 3) + 2;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 16.f;

			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::down().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b001));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::down().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b000));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
				V.u = uAdd * static_cast<float>(Xofs + zscale);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::down().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmax).m_DifColorPow.at(0b101));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::down().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(x, y, zmin).m_DifColorPow.at(0b100));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
		}
		void		BackGroundClass::AddPlaneZPlus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			auto& Verts = m_vert32s.at(id);
			Verts.AllocatePlane();

			int xscale = (xmax - xmin + 1) * cellx.scaleRate;

			int Xofs = xmax % 2 == 0;
			int Yofs = ((cellx.GetCellBuf(xmin, y, z).m_Cell - 1) * 3) + 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 16.f;

			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b001).get();
				V.u = uAdd * static_cast<float>(Xofs + xscale);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::forward().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmin, y, z).m_DifColorPow.at(0b001));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b101).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::forward().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmax, y, z).m_DifColorPow.at(0b101));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b011).get();
				V.u = uAdd * static_cast<float>(Xofs + xscale);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::forward().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmin, y, z).m_DifColorPow.at(0b011));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b111).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::forward().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmax, y, z).m_DifColorPow.at(0b111));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
		}
		void		BackGroundClass::AddPlaneZMinus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			auto& Verts = m_vert32s.at(id);
			Verts.AllocatePlane();

			int xscale = (xmax - xmin + 1) * cellx.scaleRate;

			int Xofs = xmax % 2 == 0;
			int Yofs = ((cellx.GetCellBuf(xmin, y, z).m_Cell - 1) * 3) + 1;
			float uAdd = 1.f / 2.f;
			float vAdd = 1.f / 16.f;

			COLOR_U8 SpcColor = GetColorU8(64, 64, 64, 255);

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b010).get();
				V.u = uAdd * static_cast<float>(Xofs + xscale);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::back().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmin, y, z).m_DifColorPow.at(0b010));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b110).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 0);
				V.norm = Vector3DX::back().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmax, y, z).m_DifColorPow.at(0b110));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b000).get();
				V.u = uAdd * static_cast<float>(Xofs + xscale);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::back().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmin, y, z).m_DifColorPow.at(0b000));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b100).get();
				V.u = uAdd * static_cast<float>(Xofs + 0);
				V.v = vAdd * static_cast<float>(Yofs + 1);
				V.norm = Vector3DX::back().get();
				uint8_t Power = static_cast<uint8_t>(cellx.GetCellBuf(xmax, y, z).m_DifColorPow.at(0b100));
				V.dif = GetColorU8(Power, Power, Power, 255);
				V.spc = SpcColor;
			}
		}

		void		BackGroundClass::AddCubesX(size_t id) noexcept {
			auto& cellx = m_CellxN.at(id);
			auto center = cellx.GetPoint(CamPos.at(id));
			for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {
				if (!CalcXYActive(CamVec.at(id), DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, z - center.z)) { continue; }

				bool CheckFillZ = (cellx.scaleRate != 1) && ((center.z + DrawMinZMinus <= z) && (z <= center.z + DrawMinZPlus));
				bool CheckInsideZ = (cellx.scaleRate != 1) && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.All <= y) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(CamVec.at(id), &xMaxminT, &xMaxmaxT, y - center.y, z - center.z)) { continue; }

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
							|| (CellBuff.m_Cell == s_EmptyBlick)
							|| (!isHitmin && (selectBlock != CellBuff.m_Cell))
							|| (CellBuff.m_FillInfo == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;

								bool Fill = CheckFill && (xmin <= center.x + DrawMinXPlus && center.x + DrawMinXMinus <= xmax);
								if (z >= center.z) {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 5)) {
										AddPlaneZMinus(id, cellx, xmin, xmax, y, z);
									}
								}
								else {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 4)) {
										AddPlaneZPlus(id, cellx, xmin, xmax, y, z);
									}
								}
								//この場合だけもう一回判定させるドン
								if (selectBlock != CellBuff.m_Cell) {
									x--;
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
		}
		void		BackGroundClass::AddCubesZ(size_t id) noexcept {
			auto& cellx = m_CellxN.at(id);
			auto center = cellx.GetPoint(CamPos.at(id));
			for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {
				if (!CalcYZActive(CamVec.at(id), x - center.x, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = (cellx.scaleRate != 1) && ((center.x + DrawMinXMinus <= x) && (x <= center.x + DrawMinXPlus));
				bool CheckInsideX = (cellx.scaleRate != 1) && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.All <= y) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(CamVec.at(id), x - center.x, y - center.y, &zMaxminT, &zMaxmaxT)) { continue; }

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
							|| (CellBuff.m_Cell == s_EmptyBlick)
							|| (!isHitmin && (selectBlock != CellBuff.m_Cell))
							|| (CellBuff.m_FillInfo == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								bool Fill = CheckFill && (zmin <= center.z + DrawMinZPlus && center.z + DrawMinZMinus <= zmax);
								if (x >= center.x) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 1)) {
										AddPlaneXMinus(id, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 0)) {
										AddPlaneXPlus(id, cellx, x, y, zmin, zmax);
									}
								}
								if (y >= center.y) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 3)) {
										AddPlaneYMinus(id, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 2)) {
										AddPlaneYPlus(id, cellx, x, y, zmin, zmax);
									}
								}
								//この場合だけもう一回判定させるドン
								if (selectBlock != CellBuff.m_Cell) {
									z--;
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
		//
		void		BackGroundClass::AddShadowPlaneXPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sSB.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
		}
		void		BackGroundClass::AddShadowPlaneXMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sSB.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
		}
		void		BackGroundClass::AddShadowPlaneYPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sSB.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
		}
		void		BackGroundClass::AddShadowPlaneYMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sSB.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
		}
		void		BackGroundClass::AddShadowPlaneZPlus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			auto& Verts = m_vert32sSB.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b001).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b101).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b011).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b111).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
		}
		void		BackGroundClass::AddShadowPlaneZMinus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			auto& Verts = m_vert32sSB.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;
			{
				auto& V = Verts.m_vert32[ZERO + 0];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b011).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 1];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b111).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 2];
				V.pos = cellx.GetPosBuffer(xmin, y, z, 0b001).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
			{
				auto& V = Verts.m_vert32[ZERO + 3];
				V.pos = cellx.GetPosBuffer(xmax, y, z, 0b101).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
			}
		}

		void		BackGroundClass::AddShadowCubesX(size_t id) noexcept {
			auto& cellx = m_CellxN.at(id);
			auto center = cellx.GetPoint(CamPosS.at(id));
			for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {

				bool CheckInsideZ = (cellx.scaleRate != 1) && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.All <= y) { continue; }

					bool CheckInside = CheckInsideZ && ((center.y + DrawMinYMinus < y) && (y < center.y + DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(x == center.x + DrawMaxXPlus)
							|| (CheckInside && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus)))
							|| (CellBuff.m_Cell == s_EmptyBlick)
							|| (CellBuff.m_FillInfo == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (light.at(id).z > 0.f) {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 5)) {
										AddShadowPlaneZMinus(id, cellx, xmin, xmax, y, z);
									}
								}
								else {
									if (AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 4)) {
										AddShadowPlaneZPlus(id, cellx, xmin, xmax, y, z);
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
		void		BackGroundClass::AddShadowCubesZ(size_t id) noexcept {
			auto& cellx = m_CellxN.at(id);
			auto center = cellx.GetPoint(CamPosS.at(id));
			for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {

				bool CheckInsideX = (cellx.scaleRate != 1) && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.All <= y) { continue; }

					bool CheckInside = CheckInsideX && ((DrawMinYMinus < y - center.y) && (y - center.y < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {
						auto& CellBuff = cellx.GetCellBuf(x, y, z);
						if (
							(z == DrawMaxZPlus)
							|| (CheckInside && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus)))
							|| (CellBuff.m_Cell == s_EmptyBlick)
							|| (CellBuff.m_FillInfo == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (light.at(id).x > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 1)) {
										AddShadowPlaneXMinus(id, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 0)) {
										AddShadowPlaneXPlus(id, cellx, x, y, zmin, zmax);
									}
								}
								if (light.at(id).y > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 3)) {
										AddShadowPlaneYMinus(id, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 2)) {
										AddShadowPlaneYPlus(id, cellx, x, y, zmin, zmax);
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
		void		BackGroundClass::AddSetShadowPlaneXPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sS.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;

			Verts.m_vert32[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
			Verts.m_vert32[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
			Verts.m_vert32[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
			Verts.m_vert32[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
		}
		void		BackGroundClass::AddSetShadowPlaneXMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sS.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;

			Verts.m_vert32[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
			Verts.m_vert32[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
			Verts.m_vert32[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
			Verts.m_vert32[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
		}
		void		BackGroundClass::AddSetShadowPlaneYPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sS.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;

			Verts.m_vert32[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmax, 0b011).get();
			Verts.m_vert32[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmax, 0b111).get();
			Verts.m_vert32[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmin, 0b010).get();
			Verts.m_vert32[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmin, 0b110).get();
		}
		void		BackGroundClass::AddSetShadowPlaneYMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept {
			auto& Verts = m_vert32sS.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;

			Verts.m_vert32[ZERO + 0].pos = cellx.GetPosBuffer(x, y, zmin, 0b000).get();
			Verts.m_vert32[ZERO + 1].pos = cellx.GetPosBuffer(x, y, zmin, 0b100).get();
			Verts.m_vert32[ZERO + 2].pos = cellx.GetPosBuffer(x, y, zmax, 0b001).get();
			Verts.m_vert32[ZERO + 3].pos = cellx.GetPosBuffer(x, y, zmax, 0b101).get();
		}
		void		BackGroundClass::AddSetShadowPlaneZPlus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			auto& Verts = m_vert32sS.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;

			Verts.m_vert32[ZERO + 0].pos = cellx.GetPosBuffer(xmin, y, z, 0b001).get();
			Verts.m_vert32[ZERO + 1].pos = cellx.GetPosBuffer(xmax, y, z, 0b101).get();
			Verts.m_vert32[ZERO + 2].pos = cellx.GetPosBuffer(xmin, y, z, 0b011).get();
			Verts.m_vert32[ZERO + 3].pos = cellx.GetPosBuffer(xmax, y, z, 0b111).get();
		}
		void		BackGroundClass::AddSetShadowPlaneZMinus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept {
			auto& Verts = m_vert32sS.at(id);
			Verts.AllocatePlane();

			auto ZERO = Verts.m_32Num * 4 - 4;

			Verts.m_vert32[ZERO + 0].pos = cellx.GetPosBuffer(xmin, y, z, 0b010).get();
			Verts.m_vert32[ZERO + 1].pos = cellx.GetPosBuffer(xmax, y, z, 0b110).get();
			Verts.m_vert32[ZERO + 2].pos = cellx.GetPosBuffer(xmin, y, z, 0b000).get();
			Verts.m_vert32[ZERO + 3].pos = cellx.GetPosBuffer(xmax, y, z, 0b100).get();
		}

		void		BackGroundClass::AddSetShadowCubesX(size_t id) noexcept {
			auto& cellx = m_CellxN.at(id);
			auto center = cellx.GetPoint(CamPosS.at(id));
			for (int z = center.z + DrawMaxZMinus, ZMax = center.z + DrawMaxZPlus; z <= ZMax; ++z) {
				if (!CalcXYActive(CamVecS.at(id), DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, z - center.z)) { continue; }

				bool CheckFillZ = (cellx.scaleRate != 1) && ((center.z + DrawMinZMinus <= z) && (z <= center.z + DrawMinZPlus));
				bool CheckInsideZ = (cellx.scaleRate != 1) && ((center.z + DrawMinZMinus < z) && (z < center.z + DrawMinZPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.All <= y) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(CamVecS.at(id), &xMaxminT, &xMaxmaxT, y - center.y, z - center.z)) { continue; }

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
							|| (CellBuff.m_Cell == s_EmptyBlick)
							|| (CellBuff.m_FillInfo == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								bool Fill = CheckFill && (((xmin <= center.x + DrawMinXMinus + 1) && (center.x + DrawMinXMinus + 1 <= xmax)) || ((xmin <= center.x + DrawMinXPlus - 1) && (center.x + DrawMinXPlus - 1 <= xmax)));
								if (z >= center.z) {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 5)) {
										AddSetShadowPlaneZMinus(id, cellx, xmin, xmax, y, z);
									}
								}
								else {
									if (Fill || AddCubeX_CanAddPlane(cellx, xmin, xmax, y, z, 4)) {
										AddSetShadowPlaneZPlus(id, cellx, xmin, xmax, y, z);
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
		void		BackGroundClass::AddSetShadowCubesZ(size_t id) noexcept {
			auto& cellx = m_CellxN.at(id);
			auto center = cellx.GetPoint(CamPosS.at(id));
			for (int x = center.x + DrawMaxXMinus, XMax = center.x + DrawMaxXPlus; x <= XMax; ++x) {
				if (!CalcYZActive(CamVecS.at(id), x - center.x, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = (cellx.scaleRate != 1) && ((center.x + DrawMinXMinus <= x) && (x <= center.x + DrawMinXPlus));
				bool CheckInsideX = (cellx.scaleRate != 1) && ((center.x + DrawMinXMinus < x) && (x < center.x + DrawMinXPlus));

				for (int y = center.y + DrawMaxYMinus, YMax = center.y + DrawMaxYPlus; y <= YMax; ++y) {
					if (y < 0 || cellx.All <= y) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(CamVecS.at(id), x - center.x, y - center.y, &zMaxminT, &zMaxmaxT)) { continue; }

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
							|| (CellBuff.m_Cell == s_EmptyBlick)
							|| (CellBuff.m_FillInfo == 0b111111)
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								bool Fill = CheckFill && (((zmin <= center.z + DrawMinZMinus + 1) && (center.z + DrawMinZMinus + 1 <= zmax)) || ((zmin <= center.z + DrawMinZPlus - 1) && (center.z + DrawMinZPlus - 1 <= zmax)));
								if (x >= center.x) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 1)) {
										AddSetShadowPlaneXMinus(id, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 0)) {
										AddSetShadowPlaneXPlus(id, cellx, x, y, zmin, zmax);
									}
								}
								if (y >= center.y) {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 3)) {
										AddSetShadowPlaneYMinus(id, cellx, x, y, zmin, zmax);
									}
								}
								else {
									if (Fill || AddCubeZ_CanAddPlane(cellx, x, y, zmin, zmax, 2)) {
										AddSetShadowPlaneYPlus(id, cellx, x, y, zmin, zmax);
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
		bool		BackGroundClass::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			auto& cell = m_CellxN.front();
			auto Start = cell.GetPoint(StartPos);
			auto End = cell.GetPoint(StartPos);// *EndPos

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int x, int y, int z) {
								if (!cell.isInside(y)) { return false; }
								if (cell.GetCellBuf(x, y, z).m_Cell == s_EmptyBlick) { return false; }
								if (!(cell.GetCellBuf(x, y, z).m_FillInfo != 0b111111)) { return false; }
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
			auto Start = cell.GetPoint(StartPos);
			auto End = cell.GetPoint(StartPos);// *EndPos

			for (int xm = -3; xm <= 3; ++xm) {
				for (int ym = 3; ym <= 6; ++ym) {
					for (int zm = -3; zm <= 3; ++zm) {
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int x, int y, int z) {
								if (!cell.isInside(y)) { return false; }
								if (cell.GetCellBuf(x, y, z).m_Cell == s_EmptyBlick) { return false; }
								if (!(cell.GetCellBuf(x, y, z).m_FillInfo != 0b111111)) { return false; }
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
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale3DRate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
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
			auto& cell = m_CellxN.front();
			cell.SetScale(static_cast<int>(pow(MulPer, 0)));

			std::ifstream fin{};
			fin.open("data/Map.txt", std::ios::in | std::ios::binary);
			m_CellBase.resize(256 * 256 * 256);
			fin.read((char*)m_CellBase.data(), sizeof(m_CellBase.at(0))*(256 * 256 * 256));
			fin.close();
			for (int xm = 0; xm < cell.All; ++xm) {
				for (int ym = 0; ym < cell.All; ++ym) {
					for (int zm = 0; zm < cell.All; ++zm) {
						cell.SetCellBuf(xm, ym, zm).m_Cell = m_CellBase[cell.GetCellNum(xm, ym, zm)];
					}
				}
			}
			/*
			for (int x = 0; x < cell.All; ++x) {
				if (x - cell.All/2 > -72) {
					for (int z = 0; z < cell.All; ++z) {
						for (int y = cell.All; y >=130+15 ; --y) {
							cell.SetCellBuf(x, y, z).m_Cell = cell.GetCellBuf(x, y - 15, z).m_Cell;
						}
					}
				}
			}
			//*/
			/*
			for (int x = 0; x < cell.All; ++x) {
				for (int z = 0; z < cell.All; ++z) {
					for (int y = cell.All; y >= 190; --y) {
						cell.SetCellBuf(x, y, z).m_Cell = 0;
					}
				}
			}
			//*/
		}
		void		BackGroundClass::SaveCellsFile() noexcept {
			auto& cell = m_CellxN.front();

			m_CellBase.resize(256 * 256 * 256);
			for (int xm = 0; xm < cell.All; ++xm) {
				for (int ym = 0; ym < cell.All; ++ym) {
					for (int zm = 0; zm < cell.All; ++zm) {
						m_CellBase[cell.GetCellNum(xm, ym, zm)] = cell.SetCellBuf(xm, ym, zm).m_Cell;
					}
				}
			}

			std::ofstream fout{};
			fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)m_CellBase.data(), sizeof(m_CellBase.at(0)) * (256 * 256 * 256));
			fout.close();  //ファイルを閉じる
		}
		//
		void		BackGroundClass::SettingChange() noexcept {
			auto* OptionParts = OPTION::Instance();
			switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)) {
			case 0:
			case 1:
				ShadowRate = static_cast<int>(pow(MulPer, 0));
				BaseRate = static_cast<int>(pow(MulPer, 2));
				break;
			case 2:
				ShadowRate = static_cast<int>(pow(MulPer, 0));
				BaseRate = static_cast<int>(pow(MulPer, 3));
				break;
			case 3:
				ShadowRate = static_cast<int>(pow(MulPer, 1));
				BaseRate = static_cast<int>(pow(MulPer, 4));
				break;
			default:
				ShadowRate = static_cast<int>(pow(MulPer, 1));
				BaseRate = static_cast<int>(pow(MulPer, 4));
				break;
			};
		}
		//
#if defined(DEBUG) & EDITBLICK
		void		BackGroundClass::SetBlick(int x, int y, int z, int8_t select) noexcept {
			auto& cell = m_CellxN.front();
			if (!cell.isInside(y)) { return; }
			//テクスチャだけ変える
			/*
			if (cell.SetCellBuf(x, y, z).m_Cell == s_EmptyBlick) { return; }
			cell.SetCellBuf(x, y, z).m_Cell = select;
			return;
			//*/
			//
			cell.SetCellBuf(x, y, z).m_Cell = select;
			//簡易版を更新
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);

				int xm = x / cell1.scaleRate;
				int ym = y / cell1.scaleRate;
				int zm = z / cell1.scaleRate;

				cell1.SetCellBuf(xm, ym, zm).m_Cell = cell.isFill(xm, ym, zm, cell1.scaleRate);
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
				cell.SetScale(static_cast<int>(pow(MulPer, 0)));
				for (int x = 0; x < cell.All; ++x) {
					for (int z = 0; z < cell.All; ++z) {
						auto Height = static_cast<int>(ns.octaveNoise(2, (static_cast<float>(x)) / cell.All, (static_cast<float>(z)) / cell.All) * static_cast<float>(cell.All * 4 / 5));
						for (int y = 0; y < cell.All; ++y) {
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
			auto& cell = m_CellxN.front();
			for (int sel = 1; sel < total; ++sel) {
				auto& cell1 = m_CellxN.at(sel);
				cell1.SetScale(static_cast<int>(pow(MulPer, sel)));
				for (int xm = 0; xm < cell1.All; ++xm) {
					for (int ym = 0; ym < cell1.All; ++ym) {
						for (int zm = 0; zm < cell1.All; ++zm) {
							cell1.SetCellBuf(xm, ym, zm).m_Cell = cell.isFill(xm, ym, zm, cell1.scaleRate);
						}
					}
				}
			}
			//遮蔽検索
			for (auto& cellx : m_CellxN) {
				for (int x = 0; x < cellx.All; ++x) {
					for (int y = 0; y < cellx.All; ++y) {
						for (int z = 0; z < cellx.All; ++z) {
							if (cellx.GetCellBuf(x, y, z).m_Cell == s_EmptyBlick) { continue; }
							cellx.CalcOcclusion(x, y, z);
						}
					}
				}
			}
			//各辺の明るさを指定
			for (auto& cellx : m_CellxN) {
				for (int x = 0; x < cellx.All; ++x) {
					for (int y = 0; y < cellx.All; ++y) {
						for (int z = 0; z < cellx.All; ++z) {
							if (cellx.GetCellBuf(x, y, z).m_Cell == s_EmptyBlick) { continue; }
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b000) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b001) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b010) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b011) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b100) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b101) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b110) = 128;
							cellx.SetCellBuf(x, y, z).m_DifColorPow.at(0b111) = 128;
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
			for (int i = 0; i < total; i++) {
				m_vert32s.at(i).Init(size);
				m_vert32sS.at(i).Init(size);
				m_vert32sSB.at(i).Init(size);
			}
			//
			m_Jobs.at(0).Init(
				[&]() {
					size_t id = 0;
					m_vert32s.at(id).ResetNum();
					AddCubesX(id);
					AddCubesZ(id);
				},
				[&]() {
					size_t id = 0;
					m_vert32s.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPos.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVec.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPos.at(id)).normalized();
					//
#if defined(DEBUG) & EDITBLICK
					PutPos = (CamPos.at(0) + CamVec.at(0) * (LenMouse * Scale3DRate));
					//PutPos.y = CellScale * (2.f+16.f);
					if (blicksel >= 0) {
						auto& cell = m_CellxN.front();
						auto Put = cell.GetPoint(PutPos);
						for (int xp = 0; xp < xput; xp++) {
							for (int yp = 0; yp < yput; yp++) {
								for (int zp = 0; zp < zput; zp++) {
									SetBlick((Put.x + xp - xput / 2), (Put.y + yp - yput / 2), (Put.z + zp - zput / 2), blicksel);
								}
							}
						}
						blicksel = -1;
					}
#endif
				}
			);
			m_Jobs.at(1).Init(
				[&]() {
					size_t id = 1;
					m_vert32s.at(id).ResetNum();
					AddCubesX(id);
					AddCubesZ(id);
				},
				[&]() {
					size_t id = 1;
					m_vert32s.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPos.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVec.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPos.at(id)).normalized();
				}
			);
			m_Jobs.at(2).Init(
				[&]() {
					size_t id = 2;
					m_vert32s.at(id).ResetNum();
					AddCubesX(id);
					AddCubesZ(id);
				},
				[&]() {
					size_t id = 2;
					m_vert32s.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPos.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVec.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPos.at(id)).normalized();
				}
			);
			m_Jobs.at(3).Init(
				[&]() {
					size_t id = 3;
					m_vert32s.at(id).ResetNum();
					AddCubesX(id);
					AddCubesZ(id);
				},
				[&]() {
					size_t id = 3;
					m_vert32s.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPos.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVec.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPos.at(id)).normalized();
				}
			);
			m_Jobs.at((size_t)(total + 0)).Init(
				[&]() {
					size_t id = 0;
					m_vert32sS.at(id).ResetNum();
					AddSetShadowCubesX(id);
					AddSetShadowCubesZ(id);
				},
				[&]() {
					size_t id = 0;
					m_vert32sS.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosS.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVecS.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPosS.at(id)).normalized();
				}
			);
			m_Jobs.at((size_t)(total + 1)).Init(
				[&]() {
					size_t id = 1;
					m_vert32sS.at(id).ResetNum();
					AddSetShadowCubesX(id);
					AddSetShadowCubesZ(id);
				},
				[&]() {
					size_t id = 1;
					m_vert32sS.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosS.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVecS.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPosS.at(id)).normalized();
				}
			);
			m_Jobs.at((size_t)(total + 2)).Init(
				[&]() {
					size_t id = 2;
					m_vert32sS.at(id).ResetNum();
					AddSetShadowCubesX(id);
					AddSetShadowCubesZ(id);
				},
				[&]() {
					size_t id = 2;
					m_vert32sS.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosS.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVecS.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPosS.at(id)).normalized();
				}
			);
			m_Jobs.at((size_t)(total + 3)).Init(
				[&]() {
					size_t id = 3;
					m_vert32sS.at(id).ResetNum();
					AddSetShadowCubesX(id);
					AddSetShadowCubesZ(id);
				},
				[&]() {
					size_t id = 3;
					m_vert32sS.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosS.at(id) = DrawParts->GetMainCamera().GetCamPos();
					CamVecS.at(id) = (DrawParts->GetMainCamera().GetCamVec() - CamPosS.at(id)).normalized();
				}
			);
			m_Jobs.at((size_t)(total + total + 0)).Init(
				[&]() {
					size_t id = 0;
					m_vert32sSB.at(id).ResetNum();
					AddShadowCubesX(id);
					AddShadowCubesZ(id);
				},
				[&]() {
					size_t id = 0;
					m_vert32sSB.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosSB.at(id) = DrawParts->GetMainCamera().GetCamPos();
					light.at(id) = DrawParts->GetLightVec();
				}
			);
			m_Jobs.at((size_t)(total + total + 1)).Init(
				[&]() {
					size_t id = 1;
					m_vert32sSB.at(id).ResetNum();
					AddShadowCubesX(id);
					AddShadowCubesZ(id);
				},
				[&]() {
					size_t id = 1;
					m_vert32sSB.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosSB.at(id) = DrawParts->GetMainCamera().GetCamPos();
					light.at(id) = DrawParts->GetLightVec();
				}
			);
			m_Jobs.at((size_t)(total + total + 2)).Init(
				[&]() {
					size_t id = 2;
					m_vert32sSB.at(id).ResetNum();
					AddShadowCubesX(id);
					AddShadowCubesZ(id);
				},
				[&]() {
					size_t id = 2;
					m_vert32sSB.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosSB.at(id) = DrawParts->GetMainCamera().GetCamPos();
					light.at(id) = DrawParts->GetLightVec();
				}
			);
			m_Jobs.at((size_t)(total + total + 3)).Init(
				[&]() {
					size_t id = 3;
					m_vert32sSB.at(id).ResetNum();
					AddShadowCubesX(id);
					AddShadowCubesZ(id);
				},
				[&]() {
					size_t id = 3;
					m_vert32sSB.at(id).FlipVerts();
					//
					auto* DrawParts = DXDraw::Instance();
					CamPosSB.at(id) = DrawParts->GetMainCamera().GetCamPos();
					light.at(id) = DrawParts->GetLightVec();
				}
			);
			SettingChange();
			ThreadCounter = 0;
		}
		//
		void		BackGroundClass::Execute(void) noexcept {
			auto* OptionParts = OPTION::Instance();
#if defined(DEBUG) & EDITBLICK
			{
				auto& cell = m_CellxN.front();
				auto Put = cell.GetPoint(PutPos);
				printfDx("%d,%d,%d\n", Put.x, Put.y, Put.z);
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
#endif
			for (int i = 0; i < total; i++) {
				if ((i != 0) && (i != ThreadCounter)) { continue; }
				//
				if (BaseRate < m_CellxN.at(i).scaleRate) {
					m_vert32s.at(i).Disable();
				}
				else {
					m_Jobs.at(i).Execute();
				}
				//
				if ((OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) || (ShadowRate < m_CellxN.at(i).scaleRate)) {
					m_vert32sS.at(i).Disable();
					m_vert32sSB.at(i).Disable();
				}
				else {
					m_Jobs.at((size_t)(total + i)).Execute();
					m_Jobs.at((size_t)(total + total + i)).Execute();
				}
			}
			++ThreadCounter %= total;
		}
		//
		void		BackGroundClass::BG_Draw(void) const noexcept {
			SetUseLighting(FALSE);
			this->m_ObjSky.DrawModel();
			SetUseLighting(TRUE);
		}
		void		BackGroundClass::Shadow_Draw(void) const noexcept {
			for (int i = 0; i < total; i++) {
				auto& Vert = m_vert32sSB.at(i);
				if (Vert.m_32NumOut > 0) {
					DrawPolygon32bitIndexed3D(Vert.m_vert32Out.data(), static_cast<int>(Vert.m_32NumOut * 4), Vert.m_index32Out.data(), static_cast<int>(Vert.m_32NumOut * 6 / 3), m_tex.get(), TRUE);
				}
			}
		}
		void		BackGroundClass::SetShadow_Draw_Rigid(void) const noexcept {
			SetUseTextureToShader(0, m_tex.get());
			for (int i = 0; i < total; i++) {
				auto& Vert = m_vert32sS.at(i);
				if (Vert.m_32NumOut > 0) {
					DrawPolygon32bitIndexed3DToShader(Vert.m_vert32Out.data(), static_cast<int>(Vert.m_32NumOut * 4), Vert.m_index32Out.data(), static_cast<int>(Vert.m_32NumOut * 6 / 3));
				}
			}
			SetUseTextureToShader(0, InvalidID);
		}
		void		BackGroundClass::Draw(void) const noexcept {
			float Near = GetCameraNear();
			float Far = GetCameraFar();
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;

			for (int i = 0; i < total; i++) {
				auto& Vert = m_vert32s.at(i);
				if (Vert.m_32NumOut > 0) {
					float Min = (m_CellxN.at(i).scaleRate != 1) ? MinLimit * m_CellxN.at(i).Scale : 0;
					if (!(Min < Far && Near < MaxLimit * m_CellxN.at(i).Scale)) { continue; }
					DrawPolygon32bitIndexed3D(Vert.m_vert32Out.data(), static_cast<int>(Vert.m_32NumOut * 4), Vert.m_index32Out.data(), static_cast<int>(Vert.m_32NumOut * 6 / 3), m_tex.get(), TRUE);
				}
			}
#if defined(DEBUG) & EDITBLICK
			int x = (int)(PutPos.x / CellScale), y = (int)(PutPos.y / CellScale), z = (int)(PutPos.z / CellScale);
			SetUseLighting(FALSE);
			DrawCube3D(
				(Vector3DX::vget((float)(x - xput / 2), (float)(y - yput / 2), (float)(z - zput / 2)) * CellScale).get(),
				(Vector3DX::vget((float)(x + xput - xput / 2), (float)(y + yput - yput / 2), (float)(z + zput - zput / 2)) * CellScale).get(),
				GetColor(0, 64, 0), GetColor(0, 0, 0), FALSE);
			SetUseLighting(TRUE);
#endif
		}
		//
		void		BackGroundClass::Dispose(void) noexcept {
			this->m_ObjSky.Dispose();
			SaveCellsFile();
		}
	}
}
