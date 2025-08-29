#include	"BackGround.hpp"
#include	<random>

#include"../Player/Player.hpp"

const FPS_n2::BackGround::BackGroundControl* SingletonBase<FPS_n2::BackGround::BackGroundControl>::m_Singleton = nullptr;
const FPS_n2::BackGround::BackGroundControlTitle* SingletonBase<FPS_n2::BackGround::BackGroundControlTitle>::m_Singleton = nullptr;

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
			void dig(int xmaze, int ymaze, const std::function<int()>& Rand) noexcept {
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
			const bool PosIsPath(int xmaze, int ymaze) const noexcept {
				if ((0 <= xmaze && xmaze < this->m_Width) && (0 <= ymaze && ymaze < this->m_Height)) {
					return this->m_Maze[xmaze][ymaze] == MAZETYPE::PATH;
				}
				else {
					return false;
				}
			}
			//通路の総数を取得
			const int GetPachCount(void) const noexcept {
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
			void createMaze(int width, int height, int seed) noexcept {
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
		int			BackGroundControl::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			int HitCount = 0;

			for (auto& ad : this->m_AddonColObj) {
				MV1_COLL_RESULT_POLY ColLine = ad->CollCheck_Line(StartPos, *EndPos);
				if (ColLine.HitFlag == TRUE) {
					*EndPos = ColLine.HitPosition;
					if (Normal) {
						*Normal = ColLine.Normal;
					}
					HitCount += 3;
				}
			}
			HitCount += this->m_VoxelControl->CheckLine(StartPos, EndPos, Normal);
			return HitCount;
		}
		//
		void		BackGroundControl::Load(void) noexcept {
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			this->m_VoxelControl = std::make_unique<VoxelControl>();
			this->m_VoxelControl->Load();
		}
		void		BackGroundControl::Init(bool IsTutorial) noexcept {
			auto* OptionParts = OptionManager::Instance();
			//空
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int loop = 0, num = this->m_ObjSky.GetMaterialNum(); loop < num; ++loop) {
				//this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
				this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.7f, 0.7f, 0.7f, 1.0f));
				this->m_ObjSky.SetMaterialAmbColor(loop, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
			}
			//ボクセル描画の設定
			this->m_VoxelControl->Init();//まっさらな状態に初期化
			this->m_WayPoint = std::make_unique<WayPoint>();
			if (true) {
				//今作向けの設定
				if (IsTutorial) {
					int Size = 23;
					int Rate = 6;
					int Heights = 10;
					int Edge = -Rate;
					int EdgePX = -54;
					int EdgeP = -0;
					int Maxheight = 0;
					for (int Xvoxel = 0; Xvoxel < this->m_VoxelControl->GetReferenceCells().All; ++Xvoxel) {
						for (int Zvoxel = 0; Zvoxel < this->m_VoxelControl->GetReferenceCells().All; ++Zvoxel) {
							int xPos = -Size * Rate / 2 + Xvoxel;
							int zPos = -Size * Rate / 2 + Zvoxel;
							//外壁
							if ((-EdgePX <= Xvoxel && Xvoxel <= Size * Rate + EdgePX - 1) && (-EdgeP <= Zvoxel && Zvoxel <= Size * Rate + EdgeP - 1)) {
								if ((Xvoxel == -EdgePX || Xvoxel == Size * Rate + EdgePX - 1) || (Zvoxel == -EdgeP || Zvoxel == Size * Rate + EdgeP - 1)) {
									for (int Yvoxel = 0; Yvoxel <= this->m_VoxelControl->GetReferenceCells().All / 8 - 5; ++Yvoxel) {
										this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos, Yvoxel, this->m_VoxelControl->GetReferenceCells().Half + zPos, 1, false);
									}
								}
							}
							//床
							if ((-EdgePX < Xvoxel && Xvoxel < Size * Rate + EdgePX - 1) && (-EdgeP < Zvoxel && Zvoxel < Size * Rate + EdgeP - 1)) {
								int Height = this->m_VoxelControl->GetReferenceCells().All / 8 - 15;
								if (Maxheight < Height) {
									Maxheight = Height;
								}
								for (int Yvoxel = 0; Yvoxel <= Height; ++Yvoxel) {
									this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos, Yvoxel, this->m_VoxelControl->GetReferenceCells().Half + zPos, 4, false);
								}
							}
						}
					}
					//内壁
					for (int Zvoxel = -Edge; Zvoxel < Size * Rate + Edge; ++Zvoxel) {
						for (int Xvoxel = -Edge; Xvoxel < Size * Rate + Edge; ++Xvoxel) {
							auto SetWall = [&](int xt, int zt) {
								int xPos = -Size * Rate / 2 + Xvoxel + xt;
								int zPos = -Size * Rate / 2 + Zvoxel + zt;
								int Height = this->m_VoxelControl->GetReferenceCells().All / 8 - 15;
								for (int Yvoxel = Height + 1; Yvoxel <= Height + Heights; ++Yvoxel) {
									this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos, Yvoxel, this->m_VoxelControl->GetReferenceCells().Half + zPos, 2, false);
								}
								};
							if ((-EdgePX <= Xvoxel && Xvoxel <= Size * Rate + EdgePX - 1)) {
								if (Zvoxel == Size / 2 * Rate) {
									SetWall(0, 0);
								}
								if (Zvoxel == (Size / 2) * Rate - 1) {
									SetWall(0, 0);
								}
							}

						}
					}
					//誘導はなし
					this->m_WayPoint->Init(0);
				}
				else {
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
					for (int Xvoxel = 0; Xvoxel < this->m_VoxelControl->GetReferenceCells().All; ++Xvoxel) {
						for (int Zvoxel = 0; Zvoxel < this->m_VoxelControl->GetReferenceCells().All; ++Zvoxel) {
							int xPos = -Size * Rate / 2 + Xvoxel;
							int zPos = -Size * Rate / 2 + Zvoxel;
							//外壁
							if ((-EdgeP <= Xvoxel && Xvoxel <= Size * Rate + EdgeP - 1) && (-EdgeP <= Zvoxel && Zvoxel <= Size * Rate + EdgeP - 1)) {
								if ((Xvoxel == -EdgeP || Xvoxel == Size * Rate + EdgeP - 1) || (Zvoxel == -EdgeP || Zvoxel == Size * Rate + EdgeP - 1)) {
									for (int Yvoxel = 0; Yvoxel <= this->m_VoxelControl->GetReferenceCells().All / 8 - 5; ++Yvoxel) {
										this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos, Yvoxel, this->m_VoxelControl->GetReferenceCells().Half + zPos, 1, false);
									}
								}
							}
							//床
							if ((-EdgeP < Xvoxel && Xvoxel < Size * Rate + EdgeP - 1) && (-EdgeP < Zvoxel && Zvoxel < Size * Rate + EdgeP - 1)) {
								int Height = static_cast<int>(ns.octaveNoise(2,
									(static_cast<float>(Xvoxel)) / (Size * Rate - 1),
									(static_cast<float>(Zvoxel)) / (Size * Rate - 1)) * static_cast<float>(this->m_VoxelControl->GetReferenceCells().All * 1 / 10));
								if (OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)) {
									Height = this->m_VoxelControl->GetReferenceCells().All / 8 - 15;
								}
								if (Maxheight < Height) {
									Maxheight = Height;
								}
								for (int Yvoxel = 0; Yvoxel <= Height; ++Yvoxel) {
									this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos, Yvoxel, this->m_VoxelControl->GetReferenceCells().Half + zPos, 4, false);
								}
							}
						}
					}
					//内壁
					for (int Zvoxel = -Edge; Zvoxel < Size * Rate + Edge; ++Zvoxel) {
						for (int Xvoxel = -Edge; Xvoxel < Size * Rate + Edge; ++Xvoxel) {
							auto SetWall = [&](int xt, int zt) {
#if DEBUG_NET
								return;
#endif
								int xPos = -Size * Rate / 2 + Xvoxel + xt;
								int zPos = -Size * Rate / 2 + Zvoxel + zt;
								int Height = static_cast<int>(ns.octaveNoise(2,
									(static_cast<float>(Xvoxel + xt)) / (Size * Rate),
									(static_cast<float>(Zvoxel + zt)) / (Size * Rate)) * static_cast<float>(this->m_VoxelControl->GetReferenceCells().All * 1 / 10));
								if (OptionParts->GetParamBoolean(EnumSaveParam::FlatEarth)) {
									Height = this->m_VoxelControl->GetReferenceCells().All / 8 - 15;
								}
								for (int Yvoxel = Height + 1; Yvoxel <= Height + Heights; ++Yvoxel) {
									this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos, Yvoxel, this->m_VoxelControl->GetReferenceCells().Half + zPos, 2, false);
								}
								};
							int xp = Xvoxel / Rate;
							int zp = Zvoxel / Rate;
							if (!mazeControl.PosIsPath(xp, zp) && (Xvoxel % Rate == 0) && (Zvoxel % Rate == 0)) {
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
					//経路探索
					{
						this->m_WayPoint->Init(mazeControl.GetPachCount());
						for (int y = 0; y < Size; y++) {
							for (int x = 0; x < Size; x++) {
								if (mazeControl.PosIsPath(x, y)) {
									int xPos = 0;
									int zPos = 0;
									xPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate;
									zPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate;
									auto& bu = this->m_WayPoint->AddWayPoint(this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos, 0, zPos), this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos + Rate, 0, zPos + Rate));
									if (mazeControl.PosIsPath(x + 1, y)) {
										xPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate + Rate;
										zPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate;
										bu.SetLinkBuffer(0, (this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos, 0, zPos) + this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos + Rate, 0, zPos + Rate)) / 2);
									}
									if (mazeControl.PosIsPath(x, y + 1)) {
										xPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate;
										zPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate + Rate;
										bu.SetLinkBuffer(1, (this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos, 0, zPos) + this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos + Rate, 0, zPos + Rate)) / 2);
									}
									if (mazeControl.PosIsPath(x - 1, y)) {
										xPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate - Rate;
										zPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate;
										bu.SetLinkBuffer(2, (this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos, 0, zPos) + this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos + Rate, 0, zPos + Rate)) / 2);
									}
									if (mazeControl.PosIsPath(x, y - 1)) {
										xPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + x - 1) * Rate;
										zPos = this->m_VoxelControl->GetReferenceCells().Half + (-Size / 2 + y - 1) * Rate - Rate;
										bu.SetLinkBuffer(3, (this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos, 0, zPos) + this->m_VoxelControl->GetReferenceCells().GetWorldPos(xPos + Rate, 0, zPos + Rate)) / 2);
									}
								}
							}
						}
						//リンクを確立
						this->m_WayPoint->Setup();
					}
				}
			}
			else if (false) {
				//モデルのポリゴンベースでボクセルを配置
				MV1 Model;
				MV1::Load("data/block.mv1", &Model);
				{
					MV1SetupReferenceMesh(Model.get(), 0, FALSE);
					MV1_REF_POLYGONLIST Data = MV1GetReferenceMesh(Model.get(), 0, FALSE);
					for (int loop = 0; loop < Data.VertexNum; ++loop) {
						MV1_REF_VERTEX& Ver = Data.Vertexs[loop];
						Vector3DX Pos = Ver.Position;
						Vector3DX Dif = Ver.Position;
						Dif.x = Pos.x - static_cast<int>(Pos.x);
						Dif.y = Pos.y - static_cast<int>(Pos.y);
						Dif.z = Pos.z - static_cast<int>(Pos.z);
						if (Dif.IsRangeSmaller(0.25f)) {
							int xPos = static_cast<int>(Pos.x * 1);
							int yPos = static_cast<int>(Pos.y * 1);
							int zPos = static_cast<int>(Pos.z * 1);
							for (int xp = 0; xp < 1; ++xp) {
								for (int yp = 0; yp < 1; ++yp) {
									for (int zp = 0; zp < 1; ++zp) {
										this->m_VoxelControl->SetBlick(this->m_VoxelControl->GetReferenceCells().Half + xPos + xp, yPos + yp, this->m_VoxelControl->GetReferenceCells().Half + zPos + zp, 1, false);
									}
								}
							}
						}
					}
					MV1TerminateReferenceMesh(Model.get(), 0, FALSE);
				}
				Model.Dispose();
				this->m_VoxelControl->SaveCellsFile("data/Map.txt");
			}
			else {
				this->m_VoxelControl->LoadCellsFile("data/Map.txt");
			}
			this->m_VoxelControl->Setup();
			this->m_AddonColObj.clear();
			this->m_AddonColObj.emplace_back(&this->m_ItemContainerObj->GetCol());
			//
			MATERIALPARAM Param{};
			Param.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 1.0f);						// ディフューズカラー
			Param.Ambient = GetColorF(0.5f, 0.5f, 0.5f, 1.0f);						// アンビエントカラー
			Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// スペキュラカラー
			Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// エミッシブカラー
			Param.Power = 500.0f;													// スペキュラハイライトの鮮明度
			SetMaterialParam(Param);
			//コンテナ配置
			{
				Vector3DX TargetPos = Vector3DX::zero();
				if (IsTutorial) {
					TargetPos = Vector3DX::vget(0.f, 0.f, -16.f) * Scale3DRate;
				}
				else {
					TargetPos = Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 0.f, 16.f * Scale3DRate), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
				}
				//一旦床判定
				{
					Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
					if (CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
						TargetPos = EndPos - Vector3DX::up() * 0.5f * Scale3DRate;;
					}
				}
				//周りの壁を破壊する
				//壁破壊
				{
					int								xput = 25;
					int								yput = 20;
					int								zput = 25;
					int8_t							damage = 100;
					Vector3Int Put = this->m_VoxelControl->GetReferenceCells().GetVoxelPoint(TargetPos);
					for (int xp = -xput / 2; xp <= xput / 2; ++xp) {
						for (int zp = -zput / 2; zp <= zput / 2; ++zp) {
							for (int yp = -yput / 2; yp <= yput / 2; ++yp) {
								this->m_VoxelControl->DamageCell(Put.x + xp, Put.y + yp, Put.z + zp, damage);
							}
							for (int yp = -yput / 2; yp <= 0; ++yp) {
								//this->m_VoxelControl->SetBlick(Put.x + xp, Put.y + yp, Put.z + zp, 1);
							}
						}

					}
				}
				//再度床判定しなおす
				{
					Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
					if (CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
						TargetPos = EndPos - Vector3DX::up() * 0.5f * Scale3DRate;;
					}
				}
				if (this->m_ItemContainerObj) {
					const std::shared_ptr<Objects::ItemContainerObj>& Container = (std::shared_ptr<Objects::ItemContainerObj>&)(this->m_ItemContainerObj);
					Container->Put(TargetPos, Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
				}
				{
					auto obj = std::make_shared<Objects::CircleEffect>();
					ObjectManager::Instance()->InitObject(obj, "data/model/circle/");
					obj->Put(TargetPos);
				}
			}
		}
		void		BackGroundControl::Update(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			this->m_VoxelControl->SetDrawPosition(
				CameraParts->GetMainCamera().GetCamPos(),
				(CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized());
			this->m_VoxelControl->SetShadowDrawPosition(Vector3DX::vget(0.0f, -25.0f * Scale3DRate, 0.0f), PostPassParts->GetShadowDir());
			this->m_VoxelControl->Update();
		}
		void		BackGroundControl::BG_Draw(void) const noexcept {
			int Fog = GetFogEnable();
			int VFog = GetVerticalFogEnable();
			SetFogEnable(false);
			SetVerticalFogEnable(false);
			SetUseLighting(false);
			this->m_ObjSky.DrawModel();
			SetUseLighting(true);
			SetFogEnable(Fog);
			SetVerticalFogEnable(VFog);
		}
		void		BackGroundControl::Shadow_Draw(void) const noexcept {
			this->m_VoxelControl->DrawShadow();
		}
		void		BackGroundControl::SetShadow_Draw_Rigid(void) const noexcept {
			this->m_VoxelControl->DrawByShader();
		}
		void		BackGroundControl::Draw(void) const noexcept {
			this->m_VoxelControl->Draw();
			/*
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = ViewPlayer->GetChara();
			float Height = ViewChara->GetEyePositionCache().y;
			for (auto& b : this->m_WayPoint->GetWayPoints()) {
				Vector3DX Pos = b.GetPos();
				Pos.y = Height;
				DrawCube3D((Pos - Vector3DX::vget(1.f, 1.f, 1.f)).get(), (Pos + Vector3DX::vget(1.f, 1.f, 1.f)).get(), GetColor(0, 255, 0), GetColor(0, 255, 0), true);
			}
			//*/
		}
		void		BackGroundControl::Dispose(void) noexcept {
			this->m_WayPoint.reset();
			this->m_VoxelControl->Dispose();
			this->m_AddonColObj.clear();
		}
		void		BackGroundControl::Dispose_Load(void) noexcept {
			this->m_VoxelControl->Dispose_Load();
			this->m_VoxelControl.reset();
			m_ItemContainerObj.reset();
			this->m_ObjSky.Dispose();
		}
		//
		void		BackGroundControlTitle::Load(void) noexcept {
			MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
		}
		void		BackGroundControlTitle::Init(void) noexcept {
			this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
			for (int loop = 0, num = this->m_ObjSky.GetMaterialNum(); loop < num; ++loop) {
				//this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
				this->m_ObjSky.SetMaterialDifColor(loop, GetColorF(0.7f, 0.7f, 0.7f, 1.0f));
				this->m_ObjSky.SetMaterialAmbColor(loop, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
		void		BackGroundControlTitle::BG_Draw(void) const noexcept {
			FillGraph(GetDrawScreen(), 0, 0, 0);
			SetUseLighting(false);
			this->m_ObjSky.DrawModel();
			SetUseLighting(true);
		}
		void		BackGroundControlTitle::Dispose_Load(void) noexcept {
			this->m_ObjSky.Dispose();
		}
	}
}
