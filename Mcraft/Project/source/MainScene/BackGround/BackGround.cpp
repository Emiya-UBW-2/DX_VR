#include	"BackGround.hpp"
#include	<random>

#include"../Player/Player.hpp"

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
					int next_x = xmaze + static_cast<int>(sin(rad) * 2.0f);//0 2 0 -2
					int next_y = ymaze + static_cast<int>(cos(rad) * 2.0f);//2 0 -2 0
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
			std::array<uint8_t, 512> perlinNoise{ 0 };
		private:
			constexpr float getFade(const float t_) const noexcept {
				return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10);
			}
			constexpr float getLerp(const float t_, const float a_, const float b_) const noexcept {
				return a_ + t_ * (b_ - a_);
			}
			constexpr float makeGrad(const uint8_t hash_, const float u_, const float v_) const noexcept {
				return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_);
			}
			constexpr float makeGrad(const uint8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return makeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
			}
			constexpr float getGrad(const uint8_t hash_, const float x_, const float y_, const float z_) const noexcept {
				return makeGrad(hash_ & 15, x_, y_, z_);
			}

			float setNoise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				const size_t x_int = static_cast<size_t>(static_cast<size_t>(std::floor(x_)) & 255);
				const size_t y_int = static_cast<size_t>(static_cast<size_t>(std::floor(y_)) & 255);
				const size_t z_int = static_cast<size_t>(static_cast<size_t>(std::floor(z_)) & 255);
				x_ -= std::floor(x_);
				y_ -= std::floor(y_);
				z_ -= std::floor(z_);
				const float u = getFade(x_);
				const float v = getFade(y_);
				const float w = getFade(z_);
				const size_t a0 = static_cast<size_t>(this->perlinNoise[x_int] + y_int);
				const size_t a1 = static_cast<size_t>(this->perlinNoise[a0] + z_int);
				const size_t a2 = static_cast<size_t>(this->perlinNoise[a0 + 1] + z_int);
				const size_t b0 = static_cast<size_t>(this->perlinNoise[x_int + 1] + y_int);
				const size_t b1 = static_cast<size_t>(this->perlinNoise[b0] + z_int);
				const size_t b2 = static_cast<size_t>(this->perlinNoise[b0 + 1] + z_int);

				return getLerp(w,
					getLerp(v,
						getLerp(u, getGrad(this->perlinNoise[a1], x_, y_, z_), getGrad(this->perlinNoise[b1], x_ - 1, y_, z_)),
						getLerp(u, getGrad(this->perlinNoise[a2], x_, y_ - 1, z_), getGrad(this->perlinNoise[b2], x_ - 1, y_ - 1, z_))),
					getLerp(v,
						getLerp(u, getGrad(this->perlinNoise[a1 + 1], x_, y_, z_ - 1), getGrad(this->perlinNoise[b1 + 1], x_ - 1, y_, z_ - 1)),
						getLerp(u, getGrad(this->perlinNoise[a2 + 1], x_, y_ - 1, z_ - 1), getGrad(this->perlinNoise[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
			}
			float setOctaveNoise(const size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				float noise_value = 0.0f;
				float amp = 1.0f;
				for (size_t loop = 0; loop < octaves_; ++loop) {
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
				for (size_t loop = 0; loop < 256; ++loop) {
					this->perlinNoise[loop] = static_cast<uint8_t>(loop);
				}
				std::shuffle(this->perlinNoise.begin(), this->perlinNoise.begin() + 256, std::default_random_engine(seed));
				for (size_t loop = 0; loop < 256; ++loop) {
					this->perlinNoise[256 + loop] = this->perlinNoise[loop];
				}
			}
			//オクターブ無しノイズを取得する
			float noise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return setNoise(x_, y_, z_) * 0.5f + 0.5f;
			}
			//オクターブ有りノイズを取得する
			float octaveNoise(const size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
				return setOctaveNoise(octaves_, x_, y_, z_) * 0.5f + 0.5f;
			}
		};
		//
		void		BackGroundControl::AddPlaneXPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneXMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneYPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 0;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneYMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 2;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneZPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + ypos, center.z + zpos).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b001).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b101).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b011).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b111).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		BackGroundControl::AddPlaneZMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + ypos, center.z + zpos).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b010).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b110).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b000).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b100).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		//
		void		BackGroundControl::AddCubes(size_t id) noexcept {
			auto& cellx = this->m_CellxN[id];
			auto& Draws = this->m_Draws[id];
			Draws.Vert32.ResetNum();
			auto center = cellx.GetPoint(Draws.CamPos);

			float CamXMinX = Draws.CamVec.x * (static_cast<float>(DrawMaxXMinus) + 0.5f);
			float CamXMaxX = Draws.CamVec.x * (static_cast<float>(DrawMaxXPlus) + 0.5f);
			float CamYMinY = Draws.CamVec.y * (static_cast<float>(DrawMaxYMinus) + 0.5f);
			float CamYMaxY = Draws.CamVec.y * (static_cast<float>(DrawMaxYPlus) + 0.5f);
			float CamZMinZ = Draws.CamVec.z * (static_cast<float>(DrawMaxZMinus) + 0.5f);
			float CamZMaxZ = Draws.CamVec.z * (static_cast<float>(DrawMaxZPlus) + 0.5f);
			//X
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
				float CamZZ = Draws.CamVec.z * (static_cast<float>(zpos) + 0.5f);
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (
					((CamXMinX + CamYMinY + CamZZ) <= 0.0f) &&//Dot
					((CamXMaxX + CamYMaxY + CamZZ) <= 0.0f) &&//Dot
					((CamXMaxX + CamYMinY + CamZZ) <= 0.0f) &&//Dot
					((CamXMinX + CamYMaxY + CamZZ) <= 0.0f)//Dot
					) {
					continue;
				}

				bool CheckFillZ = cellx.isFarCells() && ((DrawMinZMinus <= zpos) && (zpos <= DrawMinZPlus));
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					float CamYY = Draws.CamVec.y * (static_cast<float>(ypos) + 0.5f);

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					{
						float dTa = (CamXMinX + CamYY + CamZZ);//Dot
						float dTb = (CamXMaxX + CamYY + CamZZ);//Dot
						bool OnFront = (dTa >= 0.0f && dTb >= 0.0f);
						bool Onbehind = (dTa < 0.0f && dTb < 0.0f);
						if (Onbehind && !OnFront) {
							continue;
						}
						if (!OnFront && !Onbehind) {
							if (dTa < 0.0f) {
								xMaxminT = std::max(DrawMaxXMinus + static_cast<int>((DrawMaxXPlus - DrawMaxXMinus) * (dTa / (dTa - dTb))) - 1, DrawMaxXMinus);
							}
							else {
								xMaxmaxT = std::min(DrawMaxXMinus + static_cast<int>((DrawMaxXPlus - DrawMaxXMinus) * (dTa / (dTa - dTb))) + 1, DrawMaxXPlus);
							}
						}
					}

					bool CheckFillYZ = CheckFillZ && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInsideYZ = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesZ(&Draws.Vert32, static_cast<float>(zpos), cellx, center, xMaxminT, xMaxmaxT, ypos, zpos, CheckInsideYZ, CheckFillYZ, true);
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
				float CamXX = Draws.CamVec.x * (static_cast<float>(xpos) + 0.5f);
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (
					((CamXX + CamYMinY + CamZMinZ) <= 0.0f) &&//Dot
					((CamXX + CamYMaxY + CamZMaxZ) <= 0.0f) &&//Dot
					((CamXX + CamYMinY + CamZMaxZ) <= 0.0f) &&//Dot
					((CamXX + CamYMaxY + CamZMinZ) <= 0.0f)//Dot
					) {
					continue;
				}

				bool CheckFillX = cellx.isFarCells() && ((DrawMinXMinus <= xpos) && (xpos <= DrawMinXPlus));
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					float CamYY = Draws.CamVec.y * (static_cast<float>(ypos) + 0.5f);

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					{
						float dTa = (CamXX + CamYY + CamZMinZ);//Dot
						float dTb = (CamXX + CamYY + CamZMaxZ);//Dot
						bool OnFront = (dTa >= 0.0f && dTb >= 0.0f);
						bool Onbehind = (dTa < 0.0f && dTb < 0.0f);
						if (Onbehind && !OnFront) {
							continue;
						}
						if (!OnFront && !Onbehind) {
							if (dTa < 0.0f) {
								zMaxminT = std::max(DrawMaxZMinus + static_cast<int>((DrawMaxZPlus - DrawMaxZMinus) * (dTa / (dTa - dTb))) - 1, DrawMaxZMinus);
							}
							else {
								zMaxmaxT = std::min(DrawMaxZMinus + static_cast<int>((DrawMaxZPlus - DrawMaxZMinus) * (dTa / (dTa - dTb))) + 1, DrawMaxZPlus);
							}
						}
					}
					bool CheckFillXY = CheckFillX && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInsideXY = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesXY(&Draws.Vert32, static_cast<float>(xpos), static_cast<float>(ypos), cellx, center, xpos, ypos, zMaxminT, zMaxmaxT, CheckInsideXY, CheckFillXY, true);
				}
			}
		}
		void		BackGroundControl::FlipCubes(size_t id) noexcept {
			auto& Draws = this->m_Draws[id];
			Draws.Vert32.FlipVerts();
			auto* CameraParts = Camera3D::Instance();
			Draws.CamPos = CameraParts->GetMainCamera().GetCamPos();
			Draws.CamVec = (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized();
		}
		void		BackGroundControl::AddShadowCubes(size_t id) noexcept {
			size_t shadow = TotalCellLayer + id;
			auto& cellx = this->m_CellxN[id];
			auto& Draws = this->m_Draws[shadow];
			Draws.Vert32.ResetNum();
			auto center = cellx.GetPoint(Draws.CamPos);
			//X
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));
				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					bool CheckInsideYZ = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesZ(&Draws.Vert32, Draws.CamVec.z, cellx, center, DrawMaxXMinus, DrawMaxXPlus, ypos, zpos, CheckInsideYZ, false, false);
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));
				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					bool CheckInsideXY = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesXY(&Draws.Vert32, Draws.CamVec.x, Draws.CamVec.y, cellx, center, xpos, ypos, DrawMaxZMinus, DrawMaxZPlus, CheckInsideXY, false, false);
				}
			}
		}
		void		BackGroundControl::FlipShadowCubes(size_t id) noexcept {
			size_t shadow = TotalCellLayer + id;
			auto* PostPassParts = PostPassEffect::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto& Draws = this->m_Draws[shadow];
			Draws.Vert32.FlipVerts();
			//Draws.CamPos =  Camera3D::Instance()->GetMainCamera().GetCamPos();
			Draws.CamPos = Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate;
			Draws.CamVec = PostPassParts->GetShadowDir();
			SceneParts->SetIsUpdateFarShadowActive();
		}
		//
		int		BackGroundControl::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			int HitCount = 0;

			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				if (PlayerMngr->GetItemContainerObj()) {
					auto ColLine = PlayerMngr->GetItemContainerObj()->GetCol().CollCheck_Line(StartPos, *EndPos);
					if (ColLine.HitFlag == TRUE) {
						*EndPos = ColLine.HitPosition;
						if (Normal) {
							*Normal = ColLine.Normal;
						}
						HitCount += 3;
					}
				}
			}


			if (isnan<float>(StartPos.x) || isnan<float>(StartPos.y) || isnan<float>(StartPos.z)) {
				return HitCount;
			}
			if (isnan<float>((*EndPos).x) || isnan<float>((*EndPos).y) || isnan<float>((*EndPos).z)) {
				return HitCount;
			}
			float scale = 200.f * Scale3DRate;
			Vector3DX SP = StartPos; SP.y = 0.f;
			Vector3DX EP = (*EndPos); SP.y = 0.f;
			if ((SP.sqrMagnitude() > scale * scale) || (EP.sqrMagnitude() > scale * scale)) {
				return HitCount;
			}


			auto Start = GetReferenceCells().GetPoint(StartPos);
			auto End = GetReferenceCells().GetPoint(*EndPos);// *EndPos

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int xpos, int ypos, int zpos) {
								if (!GetReferenceCells().isInside(ypos)) { return false; }
								if (!GetReferenceCells().GetCellBuf(xpos, ypos, zpos).CanDraw()) { return false; }
								Vector3DX MinPos = GetPos(xpos, ypos, zpos) + Vector3DX::one() * -0.1f * Scale3DRate;
								Vector3DX MaxPos = GetPos(xpos + 1, ypos + 1, zpos + 1) + Vector3DX::one() * 0.1f * Scale3DRate;
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
									++HitCount;
								}
								return false;
							});
						if (isHit) {
							return HitCount;
						}
					}
				}
			}
			return HitCount;
		}
		bool		BackGroundControl::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept {
			auto MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			auto Start = GetReferenceCells().GetPoint(StartPos);
			auto End = GetReferenceCells().GetPoint(*EndPos);// *EndPos

			for (int xm = -3; xm <= 3; ++xm) {
				for (int ym = 3; ym <= 6; ++ym) {
					for (int zm = -3; zm <= 3; ++zm) {
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int xpos, int ypos, int zpos) {
								if (!GetReferenceCells().isInside(ypos)) { return false; }
								if (!GetReferenceCells().GetCellBuf(xpos, ypos, zpos).CanDraw()) { return false; }
								Vector3DX MinPos = GetPos(xpos, ypos, zpos);
								Vector3DX MaxPos = GetPos(xpos + 1, ypos + 1, zpos + 1);

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
			auto* PlayerMngr = Player::PlayerManager::Instance();

			{
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale3DRate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale3DRate)		// 周囲のポリゴン検出に使用する球の初期サイズ
				auto HitDim = PlayerMngr->GetItemContainerObj()->GetCol().CollCheck_Sphere(StartPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.magnitude());
				// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
				for (int i = 0; i < HitDim.HitNum; ++i) {
					auto& h_d = HitDim.Dim[i];
					kabes.emplace_back(h_d);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
				}
				MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
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
			{
				SetReferenceCells().SetScale(0);

				std::ifstream fin{};
				fin.open("data/Map.txt", std::ios::in | std::ios::binary);
				this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
				fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
				fin.close();
				for (int xm = 0; xm < GetReferenceCells().All; ++xm) {
					for (int ym = 0; ym < GetReferenceCells().All; ++ym) {
						for (int zm = 0; zm < GetReferenceCells().All; ++zm) {
							SetReferenceCells().SetCellBuf(xm, ym, zm).Cell = this->m_CellBase[GetReferenceCells().GetCellNum(xm, ym, zm)];
						}
					}
				}
			}
			//一部をペースト
			//LoadCellsClip(0, 128, 0);
			/*
			for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
				if (xpos - GetReferenceCells().Half > -72) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = GetReferenceCells().All; ypos >=130+15 ; --ypos) {
							SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = GetReferenceCells().GetCellBuf(xpos, ypos - 15, zpos).GetCell();
						}
					}
				}
			}
			//*/
			/*
			for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
				for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
					for (int ypos = GetReferenceCells().All; ypos >= 133; --ypos) {
						SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = 0;
					}
				}
			}
			//*/
		}
		void		BackGroundControl::SaveCellsFile(void) noexcept {
			{
				this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
				for (int xm = 0; xm < GetReferenceCells().All; ++xm) {
					for (int ym = 0; ym < GetReferenceCells().All; ++ym) {
						for (int zm = 0; zm < GetReferenceCells().All; ++zm) {
							this->m_CellBase[GetReferenceCells().GetCellNum(xm, ym, zm)] = GetReferenceCells().GetCellBuf(xm, ym, zm).GetCell();
						}
					}
				}

				std::ofstream fout{};
				fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
				fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
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
			int XTotal = 1;
			int YTotal = 1;
			int ZTotal = 1;
			std::ifstream fin{};
			fin.open("data/Build/Map.txt", std::ios::in | std::ios::binary);
			fin.read((char*)&XTotal, sizeof(XTotal));
			fin.read((char*)&YTotal, sizeof(YTotal));
			fin.read((char*)&ZTotal, sizeof(ZTotal));
			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * XTotal * YTotal * ZTotal);
			fin.close(); //ファイルを閉じる

			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						SetReferenceCells().SetCellBuf(xm + xbasepos, ym + ybasepos, zm + zbasepos).Cell =
							this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)];
					}
				}
			}
		}
		void		BackGroundControl::SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax) noexcept {
			int XTotal = (XMax - XMin + 1);
			int YTotal = (YMax - YMin + 1);
			int ZTotal = (ZMax - ZMin + 1);

			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)] = GetReferenceCells().GetCellBuf(XMin + xm, YMin + ym, ZMin + zm).GetCell();
					}
				}
			}

			std::ofstream fout{};
			fout.open("data/Build/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)&XTotal, sizeof(XTotal));
			fout.write((char*)&YTotal, sizeof(YTotal));
			fout.write((char*)&ZTotal, sizeof(ZTotal));
			fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0]) * XTotal * YTotal * ZTotal));
			fout.close(); //ファイルを閉じる
		}
		//
		void		BackGroundControl::SettingChange(void) noexcept {
			this->m_ShadowRate = 0;
			this->m_BaseRate = 1;
		}
		//
		void		BackGroundControl::SetBlick(int xpos, int ypos, int zpos, int8_t select) noexcept {
			if (!GetReferenceCells().isInside(ypos)) { return; }
			//テクスチャだけ変える
			/*
			if (GetReferenceCells().GetCellBuf(xpos, ypos, zpos).IsEmpty()) { return; }
			SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = select;
			return;
			//*/
			//
			SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = select;
			//簡易版を更新
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				auto& cell1 = this->m_CellxN[loop];

				int xm = xpos / cell1.ScaleRate;
				int ym = ypos / cell1.ScaleRate;
				int zm = zpos / cell1.ScaleRate;

				cell1.SetCellBuf(xm, ym, zm).Cell = GetReferenceCells().isFill(xm, ym, zm, cell1.ScaleRate);
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				int xm = xpos / cellx.ScaleRate;
				int ym = ypos / cellx.ScaleRate;
				int zm = zpos / cellx.ScaleRate;
				cellx.CalcOcclusion(xm + 1, ym, zm);
				cellx.CalcOcclusion(xm - 1, ym, zm);
				cellx.CalcOcclusion(xm, ym + 1, zm);
				cellx.CalcOcclusion(xm, ym - 1, zm);
				cellx.CalcOcclusion(xm, ym, zm + 1);
				cellx.CalcOcclusion(xm, ym, zm - 1);
			}
		}
		//
		void		BackGroundControl::Load(void) noexcept {
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			this->m_tex.Load("data/tex.png");
		}
		//
		void		BackGroundControl::Init(bool IsTutorial) noexcept {
			auto* OptionParts = OptionManager::Instance();
			//空
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int loop = 0, num = this->m_ObjSky.GetMaterialNum(); loop < num; ++loop) {
				//this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
				this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.7f, 0.7f, 0.7f, 1.0f));
				this->m_ObjSky.SetMaterialAmbColor(loop, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
			}
			if (IsTutorial) {
				//空っぽ
				SetReferenceCells().SetScale(0);

				int Size = 23;
				int Rate = 6;
				int Heights = 10;
				int Edge = -Rate;
				int EdgePX = -54;
				int EdgeP = -0;
				int Maxheight = 0;
				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = 0; ypos < GetReferenceCells().All; ++ypos) {
							SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = s_EmptyBlick;
						}
					}
				}
				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						int xPos = -Size * Rate / 2 + xpos;
						int zPos = -Size * Rate / 2 + zpos;
						//外壁
						if ((-EdgePX <= xpos && xpos <= Size * Rate + EdgePX - 1) && (-EdgeP <= zpos && zpos <= Size * Rate + EdgeP - 1)) {
							if ((xpos == -EdgePX || xpos == Size * Rate + EdgePX - 1) || (zpos == -EdgeP || zpos == Size * Rate + EdgeP - 1)) {
								for (int ypos = 0; ypos <= GetReferenceCells().All / 8 - 5; ++ypos) {
									SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 1;
								}
							}
						}
						//床
						if ((-EdgePX < xpos && xpos < Size * Rate + EdgePX - 1) && (-EdgeP < zpos && zpos < Size * Rate + EdgeP - 1)) {
							auto Height = GetReferenceCells().All / 8 - 15;
							if (Maxheight < Height) {
								Maxheight = Height;
							}
							for (int ypos = 0; ypos <= Height; ++ypos) {
								SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 4;
							}
						}
					}
				}
				//内壁
				for (int zpos = -Edge; zpos < Size * Rate + Edge; ++zpos) {
					for (int xpos = -Edge; xpos < Size * Rate + Edge; ++xpos) {
						auto SetWall = [&](int xt, int zt) {
							int xPos = -Size * Rate / 2 + xpos + xt;
							int zPos = -Size * Rate / 2 + zpos + zt;
							auto Height = GetReferenceCells().All / 8 - 15;
							for (int ypos = Height + 1; ypos <= Height + Heights; ++ypos) {
								SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 2;
							}
							};
						if ((-EdgePX <= xpos && xpos <= Size * Rate + EdgePX - 1)) {
							if (zpos == Size / 2 * Rate) {
								SetWall(0, 0);
							}
							if (zpos == (Size / 2) * Rate - 1) {
								SetWall(0, 0);
							}
						}

					}
				}
				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = 0; ypos < GetReferenceCells().All; ++ypos) {
							if (GetReferenceCells().GetCellBuf(xpos, ypos, zpos).GetCell() != s_EmptyBlick) {
								SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Life = 100;
							}
							else {
								SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Life = 0;
							}
						}
					}
				}
				this->m_ObjBuilds.resize(1);
				this->m_ObjBuilds.back().Set(0);
				this->m_ObjBuilds.back().SetPosition(Vector3DX::zero(), Vector3DX::zero());
			}
			else if (true) {
				//空っぽ
				SetReferenceCells().SetScale(0);

				int seed = GetRand(100);
#if DEBUG_NET
				seed = 0;
#endif

				PerlinNoise ns(seed);
				MazeControl mazeControl;

				int Size = 23;
				mazeControl.createMaze(Size, Size, seed);
				int Rate = 6;
				int Heights = 10;
				int Edge = -Rate;
				int EdgeP = -0;
				int Maxheight = 0;
				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = 0; ypos < GetReferenceCells().All; ++ypos) {
							SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = s_EmptyBlick;
						}
					}
				}
				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						int xPos = -Size * Rate / 2 + xpos;
						int zPos = -Size * Rate / 2 + zpos;
						//外壁
						if ((-EdgeP <= xpos && xpos <= Size * Rate + EdgeP - 1) && (-EdgeP <= zpos && zpos <= Size * Rate + EdgeP - 1)) {
							if ((xpos == -EdgeP || xpos == Size * Rate + EdgeP - 1) || (zpos == -EdgeP || zpos == Size * Rate + EdgeP - 1)) {
								for (int ypos = 0; ypos <= GetReferenceCells().All / 8 - 5; ++ypos) {
									SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 1;
								}
							}
						}
						//床
						if ((-EdgeP < xpos && xpos < Size * Rate + EdgeP - 1) && (-EdgeP < zpos && zpos < Size * Rate + EdgeP - 1)) {
							auto Height = static_cast<int>(ns.octaveNoise(2,
								(static_cast<float>(xpos)) / (Size * Rate - 1),
								(static_cast<float>(zpos)) / (Size * Rate - 1)) * static_cast<float>(GetReferenceCells().All * 1 / 10));
							if (OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)) {
								Height = GetReferenceCells().All / 8 - 15;
							}
							if (Maxheight < Height) {
								Maxheight = Height;
							}
							for (int ypos = 0; ypos <= Height; ++ypos) {
								SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 4;
							}
						}
					}
				}
				//内壁
				//*
				for (int zpos = -Edge; zpos < Size * Rate + Edge; ++zpos) {
					for (int xpos = -Edge; xpos < Size * Rate + Edge; ++xpos) {
						auto SetWall = [&](int xt, int zt) {
#if DEBUG_NET
							return;
#endif

							int xPos = -Size * Rate / 2 + xpos + xt;
							int zPos = -Size * Rate / 2 + zpos + zt;
							auto Height = static_cast<int>(ns.octaveNoise(2,
								(static_cast<float>(xpos + xt)) / (Size * Rate),
								(static_cast<float>(zpos + zt)) / (Size * Rate)) * static_cast<float>(GetReferenceCells().All * 1 / 10));
							if (OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)) {
								Height = GetReferenceCells().All / 8 - 15;
							}
							for (int ypos = Height + 1; ypos <= Height + Heights; ++ypos) {
								SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 2;
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
				/*
				for (int zpos = -Edge * 2; zpos < Size * Rate + Edge * 3; ++zpos) {
					for (int xpos = -Edge * 2; xpos < Size * Rate + Edge * 3; ++xpos) {
						auto SetWall = [&](int xt, int zt) {
#if DEBUG_NET
							return;
#endif
							int xPos = -Size * Rate / 2 + xpos + xt;
							int zPos = -Size * Rate / 2 + zpos + zt;
							int ypos = Maxheight + Heights;
							SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos, ypos, GetReferenceCells().Half + zPos).Cell = 4;
							};
						if ((xpos % Rate == 0) && (zpos % Rate == 0)) {
							for (int xt = 0; xt <= Rate; ++xt) {
								for (int zt = 0; zt <= Rate; ++zt) {
									SetWall(xt, zt);
								}
							}
						}
					}
				}
				//*/
				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = 0; ypos < GetReferenceCells().All; ++ypos) {
							if (GetReferenceCells().GetCellBuf(xpos, ypos, zpos).GetCell() != s_EmptyBlick) {
								SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Life = 100;
							}
							else {
								SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Life = 0;
							}
						}
					}
				}
				//SaveCellsFile();






						//
				this->m_ObjBuilds.resize(mazeControl.GetPachCount());
				{
					float deg = 0.f;
					int loop = 0;
					for (int y = 0; y < Size; y++) {
						for (int x = 0; x < Size; x++) {
							if (mazeControl.PosIsPath(x, y)) {
								int count = 0;

								bool XP = mazeControl.PosIsPath(x + 1, y);
								bool XM = mazeControl.PosIsPath(x - 1, y);
								bool ZP = mazeControl.PosIsPath(x, y + 1);
								bool ZM = mazeControl.PosIsPath(x, y - 1);

								if (XP) { count++; }
								if (XM) { count++; }
								if (ZP) { count++; }
								if (ZM) { count++; }

								int ID = 0;
								switch (count) {
								case 1:
									ID = 0;
									if (ZM) { deg = 0.f; }
									if (XM) { deg = 90.f; }
									if (ZP) { deg = 180.f; }
									if (XP) { deg = 270.f; }
									break;
								case 2:
									if (ZP && ZM) { ID = 1; deg = 0.f; }
									if (XP && XM) { ID = 1; deg = 90.f; }

									if (XP && ZP) { ID = 4; deg = 270.f; }
									if (XM && ZP) { ID = 4; deg = 180.f; }
									if (XM && ZM) { ID = 4; deg = 90.f; }
									if (XP && ZM) { ID = 4; deg = 0.f; }
									break;
								case 3:
									ID = 2;
									if (ZP && XP && ZM) { deg = 0.f; }
									if (ZP && XM && ZM) { deg = 180.f; }
									if (XP && ZP && XM) { deg = 270.f; }
									if (XP && ZM && XM) { deg = 90.f; }
									break;
								case 4:
									ID = 3;
									break;
								default:
									break;
								}
								this->m_ObjBuilds[loop].Set(loop);
								int xPos = GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate;
								int zPos = GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate;
								this->m_ObjBuilds[loop].SetPosition(GetPos(xPos, 0, zPos), GetPos(xPos + Rate, 0, zPos + Rate));
								loop++;
							}
						}
					}
				}
				{
					int loop = 0;
					for (int y = 0; y < Size; y++) {
						for (int x = 0; x < Size; x++) {
							if (mazeControl.PosIsPath(x, y)) {
								auto& bu = this->m_ObjBuilds[loop];
								if (mazeControl.PosIsPath(x + 1, y)) {
									int xPos = GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate + Rate;
									int zPos = GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate;
									bu.SetLink(0, GetNearestBuilds((GetPos(xPos, 0, zPos) + GetPos(xPos + Rate, 0, zPos + Rate)) / 2));
								}
								if (mazeControl.PosIsPath(x, y + 1)) {
									int xPos = GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate;
									int zPos = GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate + Rate;
									bu.SetLink(1, GetNearestBuilds((GetPos(xPos, 0, zPos) + GetPos(xPos + Rate, 0, zPos + Rate)) / 2));
								}
								if (mazeControl.PosIsPath(x - 1, y)) {
									int xPos = GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate - Rate;
									int zPos = GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate;
									bu.SetLink(2, GetNearestBuilds((GetPos(xPos, 0, zPos) + GetPos(xPos + Rate, 0, zPos + Rate)) / 2));
								}
								if (mazeControl.PosIsPath(x, y - 1)) {
									int xPos = GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate;
									int zPos = GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate - Rate;
									bu.SetLink(3, GetNearestBuilds((GetPos(xPos, 0, zPos) + GetPos(xPos + Rate, 0, zPos + Rate)) / 2));
								}
								loop++;
							}
						}
					}
				}
			}
			else if (false) {
				//空っぽ
				SetReferenceCells().SetScale(0);

				for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = 0; ypos < GetReferenceCells().All; ++ypos) {
							SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = s_EmptyBlick;
						}
					}
				}
				{
					MV1 Model;
					MV1::Load("data/block.mv1", &Model);
					{
						MV1SetupReferenceMesh(Model.get(), 0, FALSE);
						MV1_REF_POLYGONLIST Data = MV1GetReferenceMesh(Model.get(), 0, FALSE);
						for (int loop = 0; loop < Data.VertexNum; ++loop) {
							auto& Ver = Data.Vertexs[loop];
							Vector3DX Pos = Ver.Position;
							Vector3DX Dif = Ver.Position;
							Dif.x = Pos.x - static_cast<int>(Pos.x);
							Dif.y = Pos.y - static_cast<int>(Pos.y);
							Dif.z = Pos.z - static_cast<int>(Pos.z);
							if (Dif.sqrMagnitude() < 0.25f * 0.25f) {
								int xPos = static_cast<int>(Pos.x * 1);
								int yPos = static_cast<int>(Pos.y * 1);
								int zPos = static_cast<int>(Pos.z * 1);
								for (int xp = 0; xp < 1; ++xp) {
									for (int yp = 0; yp < 1; ++yp) {
										for (int zp = 0; zp < 1; ++zp) {
											SetReferenceCells().SetCellBuf(GetReferenceCells().Half + xPos + xp, yPos + yp, GetReferenceCells().Half + zPos + zp).Cell = 1;
										}
									}
								}
							}
						}
						MV1TerminateReferenceMesh(Model.get(), 0, FALSE);
					}
					Model.Dispose();
				}
				SaveCellsFile();
			}
			else {
				LoadCellsFile();
			}
			//簡略版を制作
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				auto& cell1 = this->m_CellxN[loop];
				cell1.SetScale(loop);
				for (int xm = 0; xm < cell1.All; ++xm) {
					for (int ym = 0; ym < cell1.All; ++ym) {
						for (int zm = 0; zm < cell1.All; ++zm) {
							cell1.SetCellBuf(xm, ym, zm).Cell = GetReferenceCells().isFill(xm, ym, zm, cell1.ScaleRate);
						}
					}
				}
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				for (int xpos = 0; xpos < cellx.All; ++xpos) {
					for (int ypos = 0; ypos < cellx.All; ++ypos) {
						for (int zpos = 0; zpos < cellx.All; ++zpos) {
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
			Param.Power = 500.0f;													// スペキュラハイライトの鮮明度
			SetMaterialParam(Param);
			//
			constexpr size_t size = (DrawMaxXPlus - DrawMaxXMinus) * (DrawMaxYPlus - DrawMaxYMinus) * (DrawMaxZPlus - DrawMaxZMinus) / 2 * 3 / 100;
			for (auto& Vert : this->m_Draws) {
				Vert.Vert32.Init(size);
			}
			//
			this->m_Jobs[0].Init(
				[&]() { AddCubes(0); },
				[&]() { FlipCubes(0); }, false);
			this->m_Jobs[1].Init(
				[&]() { AddCubes(1); },
				[&]() { FlipCubes(1); }, false);
			this->m_Jobs[2].Init(
				[&]() { AddCubes(2); },
				[&]() { FlipCubes(2); }, false);
			this->m_Jobs[3].Init(
				[&]() { AddCubes(3); },
				[&]() { FlipCubes(3); }, false);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 0)].Init(
				[&]() { AddShadowCubes(0); },
				[&]() { FlipShadowCubes(0); }, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 1)].Init(
				[&]() { AddShadowCubes(1); },
				[&]() { FlipShadowCubes(1); }, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 2)].Init(
				[&]() { AddShadowCubes(2); },
				[&]() { FlipShadowCubes(2); }, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 3)].Init(
				[&]() { AddShadowCubes(3); },
				[&]() { FlipShadowCubes(3); }, true);
			SettingChange();
			this->m_ThreadCounter = 0;
			this->m_isChangeBlock = false;
		}
		//
		void		BackGroundControl::Update(void) noexcept {
			if (this->m_isChangeBlock) {
				this->m_isChangeBlock = false;
				//UpdateOnce
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					this->m_Jobs[static_cast<size_t>(TotalCellLayer + loop)].JobStart();
				}
			}
			auto* OptionParts = OptionManager::Instance();
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				if ((loop != 0) && (loop != this->m_ThreadCounter)) { continue; }
				auto& cell1 = this->m_CellxN[loop];
				//
				if (this->m_BaseRate < cell1.ScaleInt) {
					this->m_Draws[loop].Vert32.Disable();
				}
				else {
					this->m_Jobs[loop].Update();
				}
				//
				int shadow = TotalCellLayer + loop;
				if ((OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) || (this->m_ShadowRate < cell1.ScaleInt)) {
					this->m_Draws[shadow].Vert32.Disable();
					this->m_Jobs[shadow].UpdateDisable();
				}
				else {
					this->m_Jobs[shadow].Update();
				}
			}
			++this->m_ThreadCounter %= TotalCellLayer;

#if defined(DEBUG) && FALSE
			{
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					auto& Vert = this->m_Draws[loop].Vert32;
					auto& cell1 = this->m_CellxN[loop];
					if (Vert.GetOutNum() == 0) { continue; }
					printfDx("Vertex[%d]\n", Vert.GetOutNum() * 4);
				}
			}
#endif
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
			for (int shadow = TotalCellLayer; shadow < TotalCellLayer + TotalCellLayer; ++shadow) {
				auto& Vert = this->m_Draws[shadow].Vert32;
				Vert.Draw(this->m_tex);
			}
		}
		void		BackGroundControl::SetShadow_Draw_Rigid(void) const noexcept {
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
			float Near = GetCameraNear() / MaxLimit;
			float Far = GetCameraFar() / MinLimit;

			SetUseTextureToShader(0, this->m_tex.get());
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				auto& Vert = this->m_Draws[loop].Vert32;
				auto& cell1 = this->m_CellxN[loop];
				if (Vert.GetOutNum() == 0) { continue; }
				if (!cell1.isFarCells() && !(cell1.Scale < Far)) { continue; }
				if (cell1.isFarCells() && !(Near < cell1.Scale && cell1.Scale < Far)) { continue; }
				Vert.DrawByShader();
			}
			SetUseTextureToShader(0, InvalidID);
		}
		void		BackGroundControl::Draw(void) const noexcept {
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
			float Near = GetCameraNear() / MaxLimit;
			float Far = GetCameraFar() / MinLimit;

			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				auto& Vert = this->m_Draws[loop].Vert32;
				auto& cell1 = this->m_CellxN[loop];
				if (Vert.GetOutNum() == 0) { continue; }
				if (!cell1.isFarCells() && !(cell1.Scale < Far)) { continue; }
				if (cell1.isFarCells() && !(Near < cell1.Scale && cell1.Scale < Far)) { continue; }
				Vert.Draw(this->m_tex);
			}

			/*
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
			float Height = ViewChara->GetEyePositionCache().y;
			for (auto& b : this->m_ObjBuilds) {
				Vector3DX Pos = b.GetPos();
				Pos.y = Height;
				DrawCube3D((Pos - Vector3DX::vget(1.f, 1.f, 1.f)).get(), (Pos + Vector3DX::vget(1.f, 1.f, 1.f)).get(), GetColor(0, 255, 0), GetColor(0, 255, 0), true);
			}
			//*/
		}
		//
		void		BackGroundControl::Dispose(void) noexcept {
			for (auto& job : m_Jobs) {
				job.Dispose();
			}
			for (auto& Vert : this->m_Draws) {
				Vert.Vert32.Dispose();
			}
			for (auto& cellx : this->m_CellxN) {
				cellx.CellBuffer.clear();
			}
		}
		void		BackGroundControl::Dispose_Load(void) noexcept {
			this->m_ObjSky.Dispose();
			this->m_tex.Dispose();
		}
	}
}
