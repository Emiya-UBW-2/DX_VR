#include	"BackGround.hpp"
#include	<random>

const FPS_n2::BackGround::BackGroundControl* SingletonBase<FPS_n2::BackGround::BackGroundControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		//迷路生成
		class MazeControl {
			enum class MAZETYPE :int {
				WALL,
				PATH,
			};
		private:
			int		m_Width{ 1 };
			int		m_Height{ 1 };
			std::vector<std::vector<MAZETYPE>>		m_Maze;
		private:
			//穴掘り
			void dig(int xmaze, int ymaze, const std::function<int()>& Rand) {
				//指定した部分を掘っておく
				this->m_Maze[xmaze][ymaze] = MAZETYPE::PATH;
				// どの方向を掘ろうとしたかを覚えておく変数
				int ok = 0;
				// 全方向試すまでループ
				while (ok != 0b1111) {
					int Dir = Rand() % 4;
					ok |= (1 << Dir);
					float rad = deg2rad(Dir * 90);
					int next_x = xmaze + static_cast<int>(sin(rad) * 2.f);//0 2 0 -2
					int next_y = ymaze + static_cast<int>(cos(rad) * 2.f);//2 0 -2 0
					if ((0 <= next_x && next_x < this->m_Width) && (0 <= next_y && next_y < this->m_Height)) {
						if (this->m_Maze[next_x][next_y] == MAZETYPE::WALL) {
							this->m_Maze[static_cast<size_t>((next_x + xmaze) / 2)][static_cast<size_t>((next_y + ymaze) / 2)] = MAZETYPE::PATH;
							//その場から次の穴掘り
							dig(next_x, next_y, Rand);
						}
					}
				}
			}
		public:
			//該当座標が通路かどうか
			const auto PosIsPath(int xmaze, int ymaze) {
				if ((0 <= xmaze && xmaze < this->m_Width) && (0 <= ymaze && ymaze < this->m_Height)) {
					return this->m_Maze[xmaze][ymaze] == MAZETYPE::PATH;
				}
				else {
					return false;
				}
			}
			//通路の総数を取得
			const auto GetPachCount(void) noexcept {
				int OneSize = 0;
				for (int ymaze = 0; ymaze < this->m_Height; ++ymaze) {
					for (int xmaze = 0; xmaze < this->m_Width; ++xmaze) {
						if (PosIsPath(xmaze, ymaze)) {
							++OneSize;
						}
					}
				}
				return OneSize;
			}
		public:
			// 迷路を作成する
			void createMaze(int width, int height, int seed) {
				this->m_Width = width;
				this->m_Height = height;

				this->m_Maze.resize(this->m_Width);
				for (auto& mazeX : this->m_Maze) {
					mazeX.resize(this->m_Height);
					for (auto& cell : mazeX) {
						cell = MAZETYPE::WALL; // 全マス壁にする
					}
				}
				std::default_random_engine Rand = std::default_random_engine(seed);
				std::uniform_int_distribution<int> distX(0, this->m_Width / 2);
				std::uniform_int_distribution<int> distY(0, this->m_Height / 2);
				std::uniform_int_distribution<int> distR(0, 3);
				;
				// 開始点をランダム（奇数座標）に決定する
				dig(
					std::clamp(2 * distX(Rand) + 1, 0, this->m_Width - 1),
					std::clamp(2 * distY(Rand) + 1, 0, this->m_Height - 1),
					[&]() {return static_cast<int>(distR(Rand)); });
				//追加で穴あけ
				for (int ymaze = 1; ymaze < this->m_Height - 1; ++ymaze) {
					for (int xmaze = 1; xmaze < this->m_Width - 1; ++xmaze) {
						bool isHit = false;
						if (xmaze == 1 || (xmaze == this->m_Width - 1 - 1)) {
							isHit = true;
						}
						if (ymaze == 1 || (ymaze == this->m_Height - 1 - 1)) {
							isHit = true;
						}
						if (!isHit) { continue; }
						this->m_Maze[xmaze][ymaze] = MAZETYPE::PATH;
					}
				}
				//追加で穴あけ
				for (int ymaze = 1; ymaze < this->m_Height - 1; ++ymaze) {
					for (int xmaze = 1; xmaze < this->m_Width - 1; ++xmaze) {
						bool isHit = false;
						if ((ymaze % 6) == 0) {
							if ((xmaze % 6) == 0) { isHit = true; }
						}
						if (!isHit) { continue; }
						this->m_Maze[xmaze][ymaze] = MAZETYPE::PATH;
					}
				}
			}
			void Reset(void) noexcept {
				for (auto& mx : this->m_Maze) {
					mx.clear();
				}
				this->m_Maze.clear();
				this->m_Width = 1;
				this->m_Height = 1;
			}
		};
		//ノイズ
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
			explicit PerlinNoise(int seed) noexcept {
				setSeed(seed);
			}

			//SEED値を設定する
			void setSeed(int seed) noexcept {
				for (std::size_t loop = 0; loop < 256; ++loop) {
					this->perlinNoise[loop] = static_cast<uint_fast8_t>(loop);
				}
				std::shuffle(this->perlinNoise.begin(), this->perlinNoise.begin() + 256, std::default_random_engine(seed));
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
		bool		BackGroundControl::AddCubeX_CanAddPlane(const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int cy, int cz, int id) noexcept {
			for (int xpos =xmin; xpos <= xmax; ++xpos) {
				if (!cellx.GetCellBuf(center.x + xpos, center.y + cy, center.z + cz).IsOcclusion(id)) {
					return true;
				}
			}
			return false;
		}
		bool		BackGroundControl::AddCubeZ_CanAddPlane(const CellsData& cellx, const Vector3Int& center, int cx, int cy, int zmin, int zmax, int id) noexcept {
			for (int zpos = zmin; zpos <= zmax; ++zpos) {
				if (!cellx.GetCellBuf(center.x + cx, center.y + cy, center.z + zpos).IsOcclusion(id)) {
					return true;
				}
			}
			return false;
		}
		//
		void		BackGroundControl::AddPlaneXPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
				V.norm = Vector3DX::right().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneXMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
				V.norm = Vector3DX::left().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneYPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 0;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
				V.norm = Vector3DX::up().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneYMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.m_scaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 2;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
				V.norm = Vector3DX::down().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneZPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.m_scaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + ypos, center.z + zpos).GetCell() - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b001).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b101).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b011).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b111).get();
				V.norm = Vector3DX::forward().get();
				V.dif = GetColorU8(128, 128, 128, 255);
				V.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					V.u = uAdd * static_cast<float>(Xofs + 0);
					V.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneZMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.m_scaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + ypos, center.z + zpos).GetCell() - 1) * 3) + 1;
				uAdd = 1.f / 2.f;
				vAdd = 1.f / 16.f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& V = pTarget->SetInVert()[ZERO + 0];
				V.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b010).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b110).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b000).get();
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
				V.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b100).get();
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
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
				if (!CalcXYActive(Draws.m_CamVec, DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, zpos)) { continue; }

				bool CheckFillZ = cellx.isFarCells() && ((DrawMinZMinus <= zpos) && (zpos <= DrawMinZPlus));
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(Draws.m_CamVec, &xMaxminT, &xMaxmaxT, ypos, zpos)) { continue; }

					bool CheckFillYZ = CheckFillZ && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInside = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool CheckFillXYZ = false;
					bool isHitmin = true;
					int8_t selectBlock = s_EmptyBlick;
					for (int xpos = xMaxminT; xpos <= xMaxmaxT; ++xpos) {
						auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
						if (
							(xpos == xMaxmaxT)
							|| (CheckInside && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							|| (!isHitmin && (selectBlock != CellBuff.GetCell()))
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;

								if (zpos >= 0) {
									if (CheckFillXYZ || AddCubeX_CanAddPlane(cellx, center, xmin, xmax, ypos, zpos, 5)) {
										AddPlaneZMinus(&Draws.m_vert32, cellx, center, xmin, xmax, ypos, zpos, true);
									}
								}
								else {
									if (CheckFillXYZ || AddCubeX_CanAddPlane(cellx, center, xmin, xmax, ypos, zpos, 4)) {
										AddPlaneZPlus(&Draws.m_vert32, cellx, center, xmin, xmax, ypos, zpos, true);
									}
								}
								//この場合だけもう一回判定させるドン
								if (selectBlock != CellBuff.GetCell()) {
									--xpos;
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = xpos;
								selectBlock = CellBuff.GetCell();
							}
							xmax = xpos;
							bool CheckFillX = (xmin <= DrawMinXPlus && DrawMinXMinus <= xmax);
							CheckFillXYZ = CheckFillYZ && CheckFillX;
						}
					}
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
				if (!CalcYZActive(Draws.m_CamVec, xpos, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = cellx.isFarCells() && ((DrawMinXMinus <= xpos) && (xpos <= DrawMinXPlus));
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(Draws.m_CamVec, xpos, ypos, &zMaxminT, &zMaxmaxT)) { continue; }

					bool CheckFillXY = CheckFillX && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInside = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool CheckFillXYZ = false;
					bool isHitmin = true;
					int8_t selectBlock = s_EmptyBlick;
					for (int zpos = zMaxminT; zpos <= zMaxmaxT; ++zpos) {
						auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
						if (
							(zpos == zMaxmaxT)
							|| (CheckInside && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus)))
							|| (CellBuff.IsEmpty())
							|| (!isHitmin && (selectBlock != CellBuff.GetCell()))
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (xpos >= 0) {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 1)) {
										AddPlaneXMinus(&Draws.m_vert32, cellx, center, xpos, ypos, zmin, zmax, true);
									}
								}
								else {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 0)) {
										AddPlaneXPlus(&Draws.m_vert32, cellx, center, xpos, ypos, zmin, zmax, true);
									}
								}
								if (ypos >= 0) {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 3)) {
										AddPlaneYMinus(&Draws.m_vert32, cellx, center, xpos, ypos, zmin, zmax, true);
									}
								}
								else {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 2)) {
										AddPlaneYPlus(&Draws.m_vert32, cellx, center, xpos, ypos, zmin, zmax, true);
									}
								}
								//この場合だけもう一回判定させるドン
								if (selectBlock != CellBuff.GetCell()) {
									--zpos;
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = zpos;
								selectBlock = CellBuff.GetCell();
							}
							zmax = zpos;
							bool CheckFillZ = (zmin <= DrawMinZPlus && DrawMinZMinus <= zmax);
							CheckFillXYZ = CheckFillXY && CheckFillZ;
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
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {

				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }

					bool CheckInside = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool isHitmin = true;
					for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
						auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
						if (
							(xpos == DrawMaxXPlus)
							|| (CheckInside && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (DrawsSB.m_light.z > 0.f) {
									if (AddCubeX_CanAddPlane(cellx, center, xmin, xmax, ypos, zpos, 5)) {
										AddPlaneZMinus(&DrawsSB.m_vert32, cellx, center, xmin, xmax, ypos, zpos, false);
									}
								}
								else {
									if (AddCubeX_CanAddPlane(cellx, center, xmin, xmax, ypos, zpos, 4)) {
										AddPlaneZPlus(&DrawsSB.m_vert32, cellx, center, xmin, xmax, ypos, zpos, false);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = xpos;
							}
							xmax = xpos;
						}
					}
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {

				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }

					bool CheckInside = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool isHitmin = true;
					for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
						auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
						if (
							(zpos == DrawMaxZPlus)
							|| (CheckInside && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (DrawsSB.m_light.x > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 1)) {
										AddPlaneXMinus(&DrawsSB.m_vert32, cellx, center, xpos, ypos, zmin, zmax, false);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 0)) {
										AddPlaneXPlus(&DrawsSB.m_vert32, cellx, center, xpos, ypos, zmin, zmax, false);
									}
								}
								if (DrawsSB.m_light.y > 0.f) {
									if (AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 3)) {
										AddPlaneYMinus(&DrawsSB.m_vert32, cellx, center, xpos, ypos, zmin, zmax, false);
									}
								}
								else {
									if (AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 2)) {
										AddPlaneYPlus(&DrawsSB.m_vert32, cellx, center, xpos, ypos, zmin, zmax, false);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = zpos;
							}
							zmax = zpos;
						}
					}
				}
			}
		}
		//
		void		BackGroundControl::AddSetShadowPlaneXPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
		}
		void		BackGroundControl::AddSetShadowPlaneXMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
		}
		void		BackGroundControl::AddSetShadowPlaneYPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
		}
		void		BackGroundControl::AddSetShadowPlaneYMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
		}
		void		BackGroundControl::AddSetShadowPlaneZPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b001).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b101).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b011).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b111).get();
		}
		void		BackGroundControl::AddSetShadowPlaneZMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos) noexcept {
			pTarget->AllocatePlane();

			auto ZERO = pTarget->GetInNum() * 4 - 4;

			pTarget->SetInVert()[ZERO + 0].pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b010).get();
			pTarget->SetInVert()[ZERO + 1].pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b110).get();
			pTarget->SetInVert()[ZERO + 2].pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b000).get();
			pTarget->SetInVert()[ZERO + 3].pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b100).get();
		}

		void		BackGroundControl::AddSetShadowCubes(size_t id) noexcept {
			auto& cellx = this->m_CellxN.at(id);
			auto& DrawsSS = this->m_DrawsSS.at(id);
			DrawsSS.m_vert32.ResetNum();
			auto center = cellx.GetPoint(DrawsSS.m_CamPos);
			//X
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
				if (!CalcXYActive(DrawsSS.m_CamVec, DrawMaxXMinus, DrawMaxXPlus, DrawMaxYMinus, DrawMaxYPlus, zpos)) { continue; }

				bool CheckFillZ = cellx.isFarCells() && ((DrawMinZMinus <= zpos) && (zpos <= DrawMinZPlus));
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (!CalcXMinMax(DrawsSS.m_CamVec, &xMaxminT, &xMaxmaxT, ypos, zpos)) { continue; }

					bool CheckFillYZ = CheckFillZ && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInside = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));

					int xmin = 0;
					int xmax = 0;
					bool CheckFillXYZ = false;
					bool isHitmin = true;
					for (int xpos = xMaxminT; xpos <= xMaxmaxT; ++xpos) {
						auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
						if (
							(xpos == xMaxmaxT)
							|| (CheckInside && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (zpos >= 0) {
									if (CheckFillXYZ || AddCubeX_CanAddPlane(cellx, center, xmin, xmax, ypos, zpos, 5)) {
										AddSetShadowPlaneZMinus(&DrawsSS.m_vert32, cellx, center, xmin, xmax, ypos, zpos);
									}
								}
								else {
									if (CheckFillXYZ || AddCubeX_CanAddPlane(cellx, center, xmin, xmax, ypos, zpos, 4)) {
										AddSetShadowPlaneZPlus(&DrawsSS.m_vert32, cellx, center, xmin, xmax, ypos, zpos);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								xmin = xpos;
							}
							xmax = xpos;
							bool CheckFillX = (((xmin <= DrawMinXMinus + 1) && (DrawMinXMinus + 1 <= xmax)) || ((xmin <= DrawMinXPlus - 1) && (DrawMinXPlus - 1 <= xmax)));
							CheckFillXYZ = CheckFillYZ && CheckFillX;
						}
					}
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
				if (!CalcYZActive(DrawsSS.m_CamVec, xpos, DrawMaxYMinus, DrawMaxYPlus, DrawMaxZMinus, DrawMaxZPlus)) { continue; }

				bool CheckFillX = cellx.isFarCells() && ((DrawMinXMinus <= xpos) && (xpos <= DrawMinXPlus));
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (!CalcZMinMax(DrawsSS.m_CamVec, xpos, ypos, &zMaxminT, &zMaxmaxT)) { continue; }

					bool CheckFillXY = CheckFillX && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInside = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));

					int zmin = 0;
					int zmax = 0;
					bool CheckFillXYZ = false;
					bool isHitmin = true;
					for (int zpos = zMaxminT; zpos <= zMaxmaxT; ++zpos) {
						auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
						if (
							(zpos == zMaxmaxT)
							|| (CheckInside && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus)))
							|| (CellBuff.IsEmpty())
							|| (CellBuff.IsOcclusion())
							) {
							//置けない部分なので今まで置けていた分をまとめてポリゴン化
							if (!isHitmin) {
								isHitmin = true;
								if (xpos >= 0) {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 1)) {
										AddSetShadowPlaneXMinus(&DrawsSS.m_vert32, cellx, center, xpos, ypos, zmin, zmax);
									}
								}
								else {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 0)) {
										AddSetShadowPlaneXPlus(&DrawsSS.m_vert32, cellx, center, xpos, ypos, zmin, zmax);
									}
								}
								if (ypos >= 0) {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 3)) {
										AddSetShadowPlaneYMinus(&DrawsSS.m_vert32, cellx, center, xpos, ypos, zmin, zmax);
									}
								}
								else {
									if (CheckFillXYZ || AddCubeZ_CanAddPlane(cellx, center, xpos, ypos, zmin, zmax, 2)) {
										AddSetShadowPlaneYPlus(&DrawsSS.m_vert32, cellx, center, xpos, ypos, zmin, zmax);
									}
								}
							}
						}
						else {
							//ブロックが置ける部分
							if (isHitmin) {
								isHitmin = false;
								zmin = zpos;
							}
							zmax = zpos;
							bool CheckFillZ = (((zmin <= DrawMinZMinus + 1) && (DrawMinZMinus + 1 <= zmax)) || ((zmin <= DrawMinZPlus - 1) && (DrawMinZPlus - 1 <= zmax)));
							CheckFillXYZ = CheckFillXY && CheckFillZ;
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
							[&](int xpos, int ypos, int zpos) {
								if (!cell.isInside(ypos)) { return false; }
								if (cell.GetCellBuf(xpos, ypos, zpos).IsEmpty()) { return false; }
								if (cell.GetCellBuf(xpos, ypos, zpos).IsOcclusion()) { return false; }
								Vector3DX MinPos = cell.GetPos(xpos, ypos, zpos) + Vector3DX::one() * -0.1f * Scale3DRate;
								Vector3DX MaxPos = cell.GetPos(xpos + 1, ypos + 1, zpos + 1) + Vector3DX::one() * 0.1f * Scale3DRate;
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
							[&](int xpos, int ypos, int zpos) {
								if (!cell.isInside(ypos)) { return false; }
								if (cell.GetCellBuf(xpos, ypos, zpos).IsEmpty()) { return false; }
								if (cell.GetCellBuf(xpos, ypos, zpos).IsOcclusion()) { return false; }
								Vector3DX MinPos = cell.GetPos(xpos, ypos, zpos);
								Vector3DX MaxPos = cell.GetPos(xpos + 1, ypos + 1, zpos + 1);

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
			for (int xpos = 0; xpos < cell.m_All; ++xpos) {
				if (xpos - cell.m_Half > -72) {
					for (int zpos = 0; zpos < cell.m_All; ++zpos) {
						for (int ypos = cell.m_All; ypos >=130+15 ; --ypos) {
							cell.SetCellBuf(xpos, ypos, zpos).m_Cell = cell.GetCellBuf(xpos, ypos - 15, zpos).GetCell();
						}
					}
				}
			}
			//*/
			/*
			for (int xpos = 0; xpos < cell.m_All; ++xpos) {
				for (int zpos = 0; zpos < cell.m_All; ++zpos) {
					for (int ypos = cell.m_All; ypos >= 133; --ypos) {
						cell.SetCellBuf(xpos, ypos, zpos).m_Cell = 0;
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
							this->m_CellBase[cell.GetCellNum(xm, ym, zm)] = cell.SetCellBuf(xm, ym, zm).GetCell();
						}
					}
				}

				std::ofstream fout{};
				fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
				fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase.at(0))) * 256 * 256 * 256);
				fout.close(); //ファイルを閉じる
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
		void		BackGroundControl::LoadCellsClip(int xbasepos, int ybasepos, int zbasepos) noexcept {
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
			fin.close(); //ファイルを閉じる

			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						cell.SetCellBuf(xm + xbasepos, ym + ybasepos, zm + zbasepos).m_Cell =
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
						this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)] = cell.SetCellBuf(XMin + xm, YMin + ym, ZMin + zm).GetCell();
					}
				}
			}

			std::ofstream fout{};
			fout.open("data/Build/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)&XTotal, sizeof(XTotal));
			fout.write((char*)&YTotal, sizeof(YTotal));
			fout.write((char*)&ZTotal, sizeof(ZTotal));
			fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase.at(0)) * XTotal * YTotal * ZTotal));
			fout.close(); //ファイルを閉じる
		}
		//
		void		BackGroundControl::SettingChange(void) noexcept {
			this->m_ShadowRate = static_cast<int>(pow(MulPer, 1));
			this->m_BaseRate = static_cast<int>(pow(MulPer, 1));
		}
		//
		void		BackGroundControl::SetBlick(int xpos, int ypos, int zpos, int8_t select) noexcept {
			auto& cell = this->m_CellxN.front();
			if (!cell.isInside(ypos)) { return; }
			//テクスチャだけ変える
			/*
			if (cell.SetCellBuf(xpos, ypos, zpos).IsEmpty()) { return; }
			cell.SetCellBuf(xpos, ypos, zpos).m_Cell = select;
			return;
			//*/
			//
			cell.SetCellBuf(xpos, ypos, zpos).m_Cell = select;
			//簡易版を更新
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				auto& cell1 = this->m_CellxN.at(loop);

				int xm = xpos / cell1.m_scaleRate;
				int ym = ypos / cell1.m_scaleRate;
				int zm = zpos / cell1.m_scaleRate;

				cell1.SetCellBuf(xm, ym, zm).m_Cell = cell.isFill(xm, ym, zm, cell1.m_scaleRate);
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				int xm = xpos / cellx.m_scaleRate;
				int ym = ypos / cellx.m_scaleRate;
				int zm = zpos / cellx.m_scaleRate;
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
			if (true) {
				//空っぽ
				auto& cell = this->m_CellxN.front();
				cell.SetScale(static_cast<int>(pow(MulPer, 0)));

				int seed = GetRand(100);

				PerlinNoise ns(seed);
				MazeControl mazeControl;

				int Size = 23;
				mazeControl.createMaze(Size, Size, seed);
				int Rate = 6;
				int Heights = 10;
				int Edge = -Rate;
				int EdgeP = -0;

				for (int xpos = 0; xpos < cell.m_All; ++xpos) {
					for (int zpos = 0; zpos < cell.m_All; ++zpos) {
						for (int ypos = 0; ypos < cell.m_All; ++ypos) {
							cell.SetCellBuf(xpos, ypos, zpos).m_Cell = s_EmptyBlick;
						}
					}
				}
				for (int xpos = 0; xpos < cell.m_All; ++xpos) {
					for (int zpos = 0; zpos < cell.m_All; ++zpos) {
						int xPos = -Size * Rate / 2 + xpos;
						int zPos = -Size * Rate / 2 + zpos;
						//外壁
						if ((-EdgeP <= xpos && xpos <= Size * Rate + EdgeP - 1) && (-EdgeP <= zpos && zpos <= Size * Rate + EdgeP - 1)) {
							if ((xpos == -EdgeP || xpos == Size * Rate + EdgeP - 1) || (zpos == -EdgeP || zpos == Size * Rate + EdgeP - 1)) {
								for (int ypos = 0; ypos <= cell.m_All / 8; ++ypos) {
									cell.SetCellBuf(cell.m_Half + xPos, ypos, cell.m_Half + zPos).m_Cell = 1;
								}
							}
						}
						//床
						if ((-EdgeP < xpos && xpos < Size * Rate + EdgeP - 1) && (-EdgeP < zpos && zpos < Size * Rate + EdgeP - 1)) {
							auto Height = static_cast<int>(ns.octaveNoise(2,
								(static_cast<float>(xpos)) / (Size * Rate - 1),
								(static_cast<float>(zpos)) / (Size * Rate - 1)) * static_cast<float>(cell.m_All * 1 / 10));
							for (int ypos = 0; ypos <= Height; ++ypos) {
								cell.SetCellBuf(cell.m_Half + xPos, ypos, cell.m_Half + zPos).m_Cell = 2;
							}
						}
					}
				}
				//内壁
				//*
				for (int zpos = -Edge; zpos < Size * Rate + Edge; ++zpos) {
					for (int xpos = -Edge; xpos < Size * Rate + Edge; ++xpos) {
						auto SetWall = [&](int xt,int zt) {
#if DEBUG_NET
							return;
#endif

							int xPos = -Size * Rate / 2 + xpos + xt;
							int zPos = -Size * Rate / 2 + zpos + zt;
							auto Height = static_cast<int>(ns.octaveNoise(2, 
								(static_cast<float>(xpos + xt)) / (Size * Rate),
								(static_cast<float>(zpos + zt)) / (Size * Rate)) * static_cast<float>(cell.m_All * 1 / 10));
							for (int ypos = Height; ypos <= Height + Heights; ++ypos) {
								cell.SetCellBuf(cell.m_Half + xPos, ypos, cell.m_Half + zPos).m_Cell = 2;
							}
						};
						int xp = xpos / Rate;
						int zp = zpos / Rate;
						if (!mazeControl.PosIsPath(xp, zp) && (xpos % Rate == 0) && (zpos % Rate == 0)) {
							SetWall(0, 0);
							if ((xp > 0) && !mazeControl.PosIsPath(xp - 1, zp)) {
								for (int xt = 0; xt < Rate; ++xt) {
									SetWall(-xt, 0);
								}
							}
							if ((zp > Size - 1) && !mazeControl.PosIsPath(xp, zp - 1)) {
								for (int zt = 0; zt < Rate; ++zt) {
									SetWall(0, -zt);
								}
							}
							if ((xp < 0) && !mazeControl.PosIsPath(xp + 1, zp)) {
								for (int xt = 0; xt < Rate; ++xt) {
									SetWall(xt, 0);
								}
							}
							if ((zp < Size - 1) && !mazeControl.PosIsPath(xp, zp + 1)) {
								for (int zt = 0; zt < Rate; ++zt) {
									SetWall(0, zt);
								}
							}
						}
					}
				}
				//*/
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
				for (int xpos = 0; xpos < cellx.m_All; ++xpos) {
					for (int ypos = 0; ypos < cellx.m_All; ++ypos) {
						for (int zpos = 0; zpos < cellx.m_All; ++zpos) {
							if (cellx.GetCellBuf(xpos, ypos, zpos).IsEmpty()) { continue; }
							cellx.CalcOcclusion(xpos, ypos, zpos);
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
			, false);
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
			, false);
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
			, false);
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
			, false);
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
			, false);
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
			, false);
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
			, false);
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
			, false);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 0)).Init(
				[&]() {
					AddShadowCubes(0);
				},
				[&]() {
					size_t id = 0;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSB.at(id).m_CamPos.x = 0.f;
					this->m_DrawsSB.at(id).m_CamPos.y = -30.f * Scale3DRate;
					this->m_DrawsSB.at(id).m_CamPos.z = 0.f;
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			, true);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 1)).Init(
				[&]() {
					AddShadowCubes(1);
				},
				[&]() {
					size_t id = 1;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSB.at(id).m_CamPos.x = 0.f;
					this->m_DrawsSB.at(id).m_CamPos.y = -30.f * Scale3DRate;
					this->m_DrawsSB.at(id).m_CamPos.z = 0.f;
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			, true);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 2)).Init(
				[&]() {
					AddShadowCubes(2);
				},
				[&]() {
					size_t id = 2;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSB.at(id).m_CamPos.x = 0.f;
					this->m_DrawsSB.at(id).m_CamPos.y = -30.f * Scale3DRate;
					this->m_DrawsSB.at(id).m_CamPos.z = 0.f;
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			, true);
			this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + 3)).Init(
				[&]() {
					AddShadowCubes(3);
				},
				[&]() {
					size_t id = 3;
					this->m_DrawsSB.at(id).m_vert32.FlipVerts();
					auto* CameraParts = Camera3D::Instance();
					this->m_DrawsSB.at(id).m_CamPos = CameraParts->GetMainCamera().GetCamPos();
					this->m_DrawsSB.at(id).m_CamPos.x = 0.f;
					this->m_DrawsSB.at(id).m_CamPos.y = -30.f * Scale3DRate;
					this->m_DrawsSB.at(id).m_CamPos.z = 0.f;
					auto* PostPassParts = PostPassEffect::Instance();
					this->m_DrawsSB.at(id).m_light = PostPassParts->GetShadowDir();
				}
			, true);
			SettingChange();
			this->m_ThreadCounter = 0;
		}
		void		BackGroundControl::UpdateOnce(void) noexcept {
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + loop)).JobStart();
			}
		}
		//
		void		BackGroundControl::Execute(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
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
					this->m_Jobs.at(static_cast<size_t>(TotalCellLayer + TotalCellLayer + loop)).m_isEnd = false;
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
			auto Fog = GetFogEnable();
			auto VFog = GetVerticalFogEnable();
			SetFogEnable(false);
			SetVerticalFogEnable(false);
			SetUseLighting(false);
			this->m_ObjSky.DrawModel();
			SetUseLighting(true);
			SetFogEnable(Fog);
			SetVerticalFogEnable(VFog);
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
					float Min = this->m_CellxN.at(loop).isFarCells() ? MinLimit * this->m_CellxN.at(loop).m_Scale : 0;
					if (!(Min < Far && Near < MaxLimit * this->m_CellxN.at(loop).m_Scale)) { continue; }
					Vert.Draw(this->m_tex);
				}
			}
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
