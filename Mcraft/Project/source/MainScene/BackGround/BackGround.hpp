#pragma once
#pragma warning(disable:4464)
#include "../../Header.hpp"
#include "../../MainScene/BackGround/BackGroundSub.hpp"
#include <random>

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
			explicit PerlinNoise(const std::uint_fast32_t seed_) {
				this->setSeed(seed_);
			}

			//SEED値を設定する
			void setSeed(const std::uint_fast32_t seed_) {
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

		class BackGroundClass : public SingletonBase<BackGroundClass> {
		private:
			friend class SingletonBase<BackGroundClass>;
		private:
		private:
			MV1							m_ObjSky;
			//MV1							m_ObjGround;
			//MV1							m_ObjGroundCol;

			//頂点データ
			std::vector<VERTEX3D> vert32;
			//頂点インデックスデータ
			std::vector<uint32_t> index32;

			struct CellData {
				int8_t selset = INVALID_ID;
				int8_t inRock = false;
			};
			struct CellsData {
				std::vector<CellData> m_Cell;
				int16_t Xall = 500;
				int16_t Yall = 160;
				int16_t Zall = 500;
				int8_t scaleRate = 1;

				const auto& GetCell(int x, int y, int z) const noexcept {
					return m_Cell.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
				}
				auto& SetCell(int x, int y, int z) noexcept {
					return m_Cell.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
				}
			};

			CellsData m_CellBase;

			const int total = 3;
			const int MulPer = 3;
			const float CellScale = Scale_Rate / 2.f / 2.f;
			const float Max = 30.f;

			std::vector<CellsData> m_CellxN;
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
		private:
			void AddCube(int sel, int x,int y, int z, bool CheckFill, COLOR_U8 color) noexcept;
		public://getter
			//const auto& GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			//*
			bool CalcIntersectionPoint(
				const Vector3DX& pointA, const Vector3DX& pointB,
				const Vector3DX& planePos, const Vector3DX& planenormal,
				Vector3DX* pointIntersection,
				bool* pSameVecNormalToA,
				bool* pOnFront)
			{
				// 線分に当たらない
				float dTa = Vector3DX::Dot(planenormal, (pointA - planePos).normalized());
				float dTb = Vector3DX::Dot(planenormal, (pointB - planePos).normalized());
				*pOnFront = !(dTa <= 0.f && dTb <= 0.f);
				if ((dTa >= 0.f && dTb >= 0.f) || !*pOnFront) {
					return false;
				}

				*pSameVecNormalToA = (dTa > 0.f);
				*pointIntersection = pointA + (pointB - pointA) * (std::abs(dTa) / (std::abs(dTa) + std::abs(dTb)));
				return true;
			}

			void		Bresenham3D(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2, const std::function<bool(int,int,int)>& OutPutLine) const noexcept {

				int err_1{}, err_2{};
				int point[3]{};

				point[0] = x1;
				point[1] = y1;
				point[2] = z1;
				const int dx = x2 - x1;
				const int dy = y2 - y1;
				const int dz = z2 - z1;
				const int x_inc = (dx < 0) ? -1 : 1;
				const int l = std::abs(dx);
				const int y_inc = (dy < 0) ? -1 : 1;
				const int m = std::abs(dy);
				const int z_inc = (dz < 0) ? -1 : 1;
				const int n = std::abs(dz);
				const int dx2 = l << 1;
				const int dy2 = m << 1;
				const int dz2 = n << 1;

				if ((l >= m) && (l >= n)) {
					err_1 = dy2 - l;
					err_2 = dz2 - l;
					for (int i = 0; i < l; i++) {
						if (OutPutLine(point[0], point[1], point[2])) { return; }
						if (err_1 > 0) {
							point[1] += y_inc;
							err_1 -= dx2;
						}
						if (err_2 > 0) {
							point[2] += z_inc;
							err_2 -= dx2;
						}
						err_1 += dy2;
						err_2 += dz2;
						point[0] += x_inc;
					}
				}
				else if ((m >= l) && (m >= n)) {
					err_1 = dx2 - m;
					err_2 = dz2 - m;
					for (int i = 0; i < m; i++) {
						if (OutPutLine(point[0], point[1], point[2])) { return; }
						if (err_1 > 0) {
							point[0] += x_inc;
							err_1 -= dy2;
						}
						if (err_2 > 0) {
							point[2] += z_inc;
							err_2 -= dy2;
						}
						err_1 += dx2;
						err_2 += dz2;
						point[1] += y_inc;
					}
				}
				else {
					err_1 = dy2 - n;
					err_2 = dx2 - n;
					for (int i = 0; i < n; i++) {
						if (OutPutLine(point[0], point[1], point[2])) { return; }
						if (err_1 > 0) {
							point[1] += y_inc;
							err_1 -= dz2;
						}
						if (err_2 > 0) {
							point[0] += x_inc;
							err_2 -= dz2;
						}
						err_1 += dy2;
						err_2 += dx2;
						point[2] += z_inc;
					}
				}
				OutPutLine(point[0], point[1], point[2]);
			}

			bool		ColRayBox(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AABBMinPos, const Vector3DX& AABBMaxPos, Vector3DX* Normal = nullptr, int* NormalNum = nullptr) const noexcept {
				Vector3DX Vec = (*EndPos - StartPos);
				// 交差判定
				float p[3]{}, d[3]{}, min[3]{}, max[3]{};
				p[0] = StartPos.x;
				p[1] = StartPos.y;
				p[2] = StartPos.z;
				d[0] = Vec.x;
				d[1] = Vec.y;
				d[2] = Vec.z;

				min[0] = AABBMinPos.x;
				min[1] = AABBMinPos.y;
				min[2] = AABBMinPos.z;

				max[0] = AABBMaxPos.x;
				max[1] = AABBMaxPos.y;
				max[2] = AABBMaxPos.z;

				float t = -FLT_MAX;
				float t_max = FLT_MAX;

				for (int i = 0; i < 3; ++i) {
					if (abs(d[i]) < FLT_EPSILON) {
						if (p[i] < min[i] || p[i] > max[i])
							return false; // 交差していない
					}
					else {
						// スラブとの距離を算出
						// t1が近スラブ、t2が遠スラブとの距離
						float odd = 1.0f / d[i];
						float t1 = (min[i] - p[i]) * odd;
						float t2 = (max[i] - p[i]) * odd;
						if (t1 > t2) {
							float tmp = t1;
							t1 = t2;
							t2 = tmp;
						}

						if (t1 > t) {
							t = t1;
							//どの向き？
							switch (i) {
							case 0:
								if (Normal) {
									*Normal = Vec.x > 0.f ? Vector3DX::left() : Vector3DX::right();
								}
								if (NormalNum) {
									*NormalNum = Vec.x > 0.f ? 0 : 1;
								}
								break;
							case 1:
								if (Normal) {
									*Normal = Vec.y > 0.f ? Vector3DX::down() : Vector3DX::up();
								}
								if (NormalNum) {
									*NormalNum = Vec.y > 0.f ? 2 : 3;
								}
								break;
							case 2:
								if (Normal) {
									*Normal = Vec.z > 0.f ? Vector3DX::back() : Vector3DX::forward();
								}
								if (NormalNum) {
									*NormalNum = Vec.z > 0.f ? 4 : 5;
								}
								break;
							default:
								break;
							}
						}
						if (t2 < t_max) {
							t_max = t2;
						}

						// スラブ交差チェック
						if (t >= t_max) {
							return false;
						}
					}
				}

				// 交差している
				if (EndPos) {
					*EndPos = StartPos + Vec * t;
				}
				return true;
			}
			auto		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept {
				auto& cell = m_CellxN.back();
				Vector3DX Start = StartPos / (CellScale * cell.scaleRate);
				Vector3DX End = StartPos / (CellScale * cell.scaleRate);

				for (int xm = -1; xm <= 1; xm++) {
					for (int ym = -1; ym <= 1; ym++) {
						for (int zm = -1; zm <= 1; zm++) {
							bool isHit = false;
							int x1 = (int)(Start.x + xm);
							int y1 = (int)(Start.y + ym);
							int z1 = (int)(Start.z + zm);
							int x2 = (int)(End.x + xm);
							int y2 = (int)(End.y + ym);
							int z2 = (int)(End.z + zm);
							Bresenham3D(x1, y1, z1, x2, y2, z2, [&](int x, int y, int z) {
								if (
									((x <= -cell.Xall / 2) || (cell.Xall / 2 < x)) ||
									((y <= -cell.Yall / 2) || (cell.Yall / 2 < y)) ||
									((z <= -cell.Zall / 2) || (cell.Zall / 2 < z))
									) {
									return false;
								}

								const auto& Cell = cell.GetCell(x, y, z);
								if (Cell.selset == INVALID_ID) { return false; }
								if (Cell.inRock) { return false; }
								Vector3DX MinPos = (Vector3DX::vget((float)x, (float)y, (float)z) + (Vector3DX::one() * 0.0f)) * (CellScale * cell.scaleRate);
								Vector3DX MaxPos = (Vector3DX::vget((float)x, (float)y, (float)z) + (Vector3DX::one() * 1.0f)) * (CellScale * cell.scaleRate);
								int NormalNum = -1;
								if (ColRayBox(StartPos, EndPos, MinPos, MaxPos, Normal, &NormalNum)) {
									//if (NormalNum == 3) {
										isHit = true;
										return true;
									//}
								}
								return false;
								});
							if (isHit) { return true; }
						}
					}
				}
				return false;
			}
			//*/
		public:
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) const noexcept;
		public://
			void			Load(void) noexcept {
				//MV1::Load("data/model/map/model_DISABLE.mv1", &this->m_ObjGround);
				//MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
			}
			//
			void			Init(void) noexcept {
				//地形
				//this->m_ObjGroundCol.SetupCollInfo(1, 1, 1);
				//空
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
					//this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
					this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.7f, 0.7f, 0.7f, 1.f));
					this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
				}
				/*
				for (int i = 0, num = this->m_ObjGround.GetMaterialNum(); i < num; ++i) {
					this->m_ObjGround.SetMaterialDifColor(i, GetColorF(1.f, 1.f, 1.f, 1.f));
					this->m_ObjGround.SetMaterialAmbColor(i, GetColorF(0.15f, 0.15f, 0.15f, 1.f));
				}
				//*/
				if (true) {
					if (true) {
						PerlinNoise ns(GetRand(100));
						m_CellBase.Xall = 500;
						m_CellBase.Yall = 160;
						m_CellBase.Zall = 500;
						m_CellBase.m_Cell.resize((size_t)(m_CellBase.Xall * m_CellBase.Yall * m_CellBase.Zall));
						m_CellBase.scaleRate = 1;
						for (int x = -m_CellBase.Xall / 2; x < m_CellBase.Xall / 2; x++) {
							for (int z = -m_CellBase.Zall / 2; z < m_CellBase.Zall / 2; z++) {
								auto Height = (int)(ns.octaveNoise(2, 
									((float)(x + m_CellBase.Xall / 2)) / m_CellBase.Xall,
									((float)(z + m_CellBase.Zall / 2)) / m_CellBase.Zall
								) * (float)(m_CellBase.Yall * 4 / 5)) - (m_CellBase.Yall * 4 / 5) / 2;
								for (int y = -m_CellBase.Yall / 2; y < m_CellBase.Yall / 2; y++) {
									if (y <= Height) {
										m_CellBase.SetCell(x, y, z).selset = 0;
										continue;
									}
									m_CellBase.SetCell(x, y, z).selset = INVALID_ID;
								}
							}
						}
					}
					else {
						m_CellBase.Xall = 50;
						m_CellBase.Yall = 25;
						m_CellBase.Zall = 50;
						m_CellBase.m_Cell.resize((size_t)(m_CellBase.Xall * m_CellBase.Yall * m_CellBase.Zall));
						m_CellBase.scaleRate = 1;
						for (int x = -m_CellBase.Xall / 2; x < m_CellBase.Xall / 2; x++) {
							for (int z = -m_CellBase.Zall / 2; z < m_CellBase.Zall / 2; z++) {
								for (int y = -m_CellBase.Yall / 2; y < m_CellBase.Yall / 2; y++) {
									if (y <= 0) {
										m_CellBase.SetCell(x, y, z).selset = 0;
										continue;
									}
									if (-m_CellBase.Xall / 2 + 2 >= x || x >= m_CellBase.Xall / 2 - 2) {
										m_CellBase.SetCell(x, y, z).selset = 0;
										continue;
									}
									if (-m_CellBase.Zall / 2 + 2 >= z || z >= m_CellBase.Zall / 2 - 2) {
										m_CellBase.SetCell(x, y, z).selset = 0;
										continue;
									}
									m_CellBase.SetCell(x, y, z).selset = INVALID_ID;
								}
							}
						}
					}



					std::ofstream fout;
					fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
					fout.write((char*)&m_CellBase.Xall, sizeof(m_CellBase.Xall));
					fout.write((char*)&m_CellBase.Yall, sizeof(m_CellBase.Yall));
					fout.write((char*)&m_CellBase.Zall, sizeof(m_CellBase.Zall));
					fout.write((char*)&m_CellBase.m_Cell.at(0), sizeof(m_CellBase.m_Cell.at(0)) * m_CellBase.m_Cell.size());
					fout.close();  //ファイルを閉じる
				}
				else {
					std::ifstream fin;
					fin.open("data/Map.txt", std::ios::in | std::ios::binary);
					fin.read((char*)&m_CellBase.Xall, sizeof(m_CellBase.Xall));
					fin.read((char*)&m_CellBase.Yall, sizeof(m_CellBase.Yall));
					fin.read((char*)&m_CellBase.Zall, sizeof(m_CellBase.Zall));
					m_CellBase.scaleRate = 1;
					m_CellBase.m_Cell.resize((size_t)(m_CellBase.Xall * m_CellBase.Yall * m_CellBase.Zall));
					fin.read((char*)&m_CellBase.m_Cell.at(0), sizeof(m_CellBase.m_Cell.at(0)) * m_CellBase.m_Cell.size());
					fin.close();
				}
				//簡略版を制作
				{
					m_CellxN.clear();
					m_CellxN.resize(total);
					m_CellxN.back() = m_CellBase;//等倍なのでそのままコピー1
					for (int sel = total - 1 - 1; sel >= 0; sel--) {
						auto& cell = m_CellxN.at(sel);
						auto& cell2 = m_CellxN.at((size_t)(sel + 1));

						cell.scaleRate = (int8_t)pow(MulPer, total - 1 - sel);
						cell.Xall = m_CellBase.Xall / cell.scaleRate;
						cell.Yall = m_CellBase.Yall / cell.scaleRate;
						cell.Zall = m_CellBase.Zall / cell.scaleRate;
						cell.m_Cell.resize((size_t)(cell.Xall * cell.Yall * cell.Zall));

						for (int xm = -cell.Xall / 2; xm < cell.Xall / 2; xm++) {
							for (int ym = -cell.Yall / 2; ym < cell.Yall / 2; ym++) {
								for (int zm = -cell.Zall / 2; zm < cell.Zall / 2; zm++) {
									int FillCount = 0;
									int FillAll = 0;


									int xMaxmin = std::max(xm * MulPer, -cell2.Xall / 2);
									int xMaxmax = std::min((xm + 1) * MulPer, cell2.Xall / 2);
									int yMaxmin = std::max(ym * MulPer, -cell2.Yall / 2);
									int yMaxmax = std::min((ym + 1) * MulPer, cell2.Yall / 2);
									int zMaxmin = std::max(zm * MulPer, -cell2.Zall / 2);
									int zMaxmax = std::min((zm + 1) * MulPer, cell2.Zall / 2);

									for (int x = xMaxmin; x < xMaxmax; x++) {
										for (int y = yMaxmin; y < yMaxmax; y++) {
											for (int z = zMaxmin; z < zMaxmax; z++) {
												const auto& Cell = cell2.GetCell(x, y, z);
												FillAll++;
												if (Cell.selset == INVALID_ID) { continue; }
												FillCount++;
											}
										}
									}
									cell.SetCell(xm, ym, zm).selset = ((FillAll != 0) && ((float)FillCount / FillAll >= ((float)1 / 2))) ? 0 : INVALID_ID;
								}
							}
						}


					}
				}
				//遮蔽検索
				for (auto& cell : m_CellxN) {
					for (int x = -cell.Xall / 2; x < cell.Xall / 2; x++) {
						for (int y = -cell.Yall / 2; y < cell.Yall / 2; y++) {
							for (int z = -cell.Zall / 2; z < cell.Zall / 2; z++) {
								if (cell.GetCell(x, y, z).selset == INVALID_ID) { continue; }
								//端は全て隠す
								if (
									((x == -cell.Xall / 2) || (x == cell.Xall / 2 - 1)) ||
									((y == -cell.Yall / 2) || (y == cell.Yall / 2 - 1)) ||
									((z == -cell.Zall / 2) || (z == cell.Zall / 2 - 1))
									) {
									cell.SetCell(x, y, z).inRock = true;
									continue;
								}
								cell.SetCell(x, y, z).inRock = (
									(cell.GetCell(x + 1, y, z).selset != INVALID_ID) &&
									(cell.GetCell(x - 1, y, z).selset != INVALID_ID) &&
									(cell.GetCell(x, y + 1, z).selset != INVALID_ID) &&
									(cell.GetCell(x, y - 1, z).selset != INVALID_ID) &&
									(cell.GetCell(x, y, z + 1).selset != INVALID_ID) &&
									(cell.GetCell(x, y, z - 1).selset != INVALID_ID)
									);
							}
						}
					}
				}
			}
			//
			void			FirstExecute(void) noexcept {
			}
			//
			void			Execute(void) noexcept {
				vert32.clear();
				index32.clear();
				const COLOR_U8 Colors[] = {
					GetColorU8(255,  0,  0, 255),
					GetColorU8(255,255,  0, 255),
					GetColorU8(0,255,  0, 255),
					GetColorU8(0,255,255, 255),
					GetColorU8(0,  0,255, 255),
					GetColorU8(255,  0,255, 255),
				};

				auto* DrawParts = DXDraw::Instance();
				const Vector3DX CamPos = DrawParts->GetMainCamera().GetCamPos();
				const Vector3DX CamVec = (DrawParts->GetMainCamera().GetCamVec() - CamPos).normalized();
				for (int sel = 0; auto & cell : m_CellxN) {
					Vector3DX center = CamPos / (CellScale * cell.scaleRate);

					int xMaxmin = std::max((int)(center.x - Max), -cell.Xall / 2);
					int xMaxmax = std::min((int)(center.x + Max) + 1, cell.Xall / 2);
					int yMaxmin = std::max((int)(center.y - Max), -cell.Yall / 2);
					int yMaxmax = std::min((int)(center.y + Max) + 1, cell.Yall / 2);

					int xMinmin = (int)(center.x - Max / MulPer);
					int xMinmax = (int)(center.x + Max / MulPer);
					int yMinmin = (int)(center.y - Max / MulPer);
					int yMinmax = (int)(center.y + Max / MulPer);

					for (int x = xMaxmin; x < xMaxmax; x++) {
						for (int y = yMaxmin; y < yMaxmax; y++) {
							int zMaxmin = std::max((int)(center.z - Max), -cell.Zall / 2);
							int zMaxmax = std::min((int)(center.z + Max) + 1, cell.Zall / 2);
							int zMinmin = (int)(center.z - Max / MulPer);
							int zMinmax = (int)(center.z + Max / MulPer);

							Vector3DX ZMinPos = Vector3DX::vget((float)x, (float)y, (float)zMaxmin) + (Vector3DX::one() * 0.5f);
							Vector3DX ZMaxPos = Vector3DX::vget((float)x, (float)y, (float)zMaxmax) + (Vector3DX::one() * 0.5f);
							Vector3DX HitPos;
							bool pSameVecNormalToA{};
							bool OnFront{};
							if (CalcIntersectionPoint(ZMinPos, ZMaxPos, center, CamVec, &HitPos, &pSameVecNormalToA,&OnFront)) {
								float HitZ = HitPos.z - 0.5f;
								if (!pSameVecNormalToA) {
									zMaxmin = (int)HitZ;
								}
								else {
									zMaxmax = (int)HitZ;
								}
							}
							else {
								if (!OnFront) { continue; }
							}

							for (int z = zMaxmin; z < zMaxmax; z++) {
								bool checkFill = true;
								if (cell.scaleRate != 1) {
									if (((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax)) && ((zMinmin < z) && (z < zMinmax))) {
										continue;
									}
									checkFill = !(((xMinmin <= x) && (x <= xMinmax)) && ((yMinmin <= y) && (y <= yMinmax)) && ((zMinmin <= z) && (z <= zMinmax)));
								}
								const auto& Cell = cell.GetCell(x, y, z);
								if (Cell.selset == INVALID_ID) { continue; }
								if (Cell.inRock) { continue; }

								AddCube(sel, x, y, z, checkFill, Colors[sel]);
							}
						}
					}
					sel++;
				}
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_Far(void) noexcept {
			}
			void			Shadow_Draw(void) noexcept {
				if (vert32.size() > 0 && index32.size() > 0) {
					DrawPolygon32bitIndexed3D(vert32.data(), (int)vert32.size(), index32.data(), (int)index32.size() / 3, DX_NONE_GRAPH, FALSE);
				}
				//this->m_ObjGround.DrawModel();
			}
			void			Draw(void) noexcept {
				if (vert32.size() > 0 && index32.size() > 0) {
					DrawPolygon32bitIndexed3D(vert32.data(), (int)vert32.size(), index32.data(), (int)index32.size() / 3, DX_NONE_GRAPH, FALSE);
				}
				//this->m_ObjGround.DrawModel();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				//this->m_ObjGround.Dispose();
				//this->m_ObjGroundCol.Dispose();
			}
		};
	}
}
