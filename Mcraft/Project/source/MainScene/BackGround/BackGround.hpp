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
				int Xall = 500;
				int Yall = 160;
				int Zall = 500;
				int scaleRate = 1;

				const auto& GetCell(int x, int y, int z) const noexcept {
					return m_Cell.at((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2));
				}
				auto& SetCell(int x, int y, int z) noexcept {
					return m_Cell.at((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2));
				}
			};

			CellsData m_CellBase;

			const int total = 4;
			const int MulPer = 3;
			const float CellScale = Scale_Rate / 2.f / 2.f;
			const float Max = 25.f;

			std::vector<CellsData> m_CellxN;
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
		private:
			void AddCube(const Vector3DX& Pos, float scale, COLOR_U8 color) noexcept;
		public://getter
			//const auto& GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			/*
			auto		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, bool isNearest, Vector3DX* Normal = nullptr) const noexcept {
				bool isHit = false;
				{
					auto col_p = this->m_ObjGroundCol.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (isNearest) {
							*EndPos = col_p.HitPosition;
							if (Normal) { *Normal = col_p.Normal; }
						}
						else {
							return isHit;
						}
					}
				}
				return isHit;
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
					PerlinNoise ns(GetRand(100));
					m_CellBase.m_Cell.resize(m_CellBase.Xall * m_CellBase.Yall * m_CellBase.Zall);
					m_CellBase.scaleRate = 1;
					for (int x = -m_CellBase.Xall / 2; x < m_CellBase.Xall / 2; x++) {
						for (int z = -m_CellBase.Zall / 2; z < m_CellBase.Zall / 2; z++) {
							auto Height = (int)(ns.octaveNoise(2, ((float)(x + m_CellBase.Xall / 2)) / m_CellBase.Xall, ((float)(z + m_CellBase.Zall / 2)) / m_CellBase.Zall) * (m_CellBase.Yall * 4 / 5)) - (m_CellBase.Yall * 4 / 5) / 2;
							for (int y = -m_CellBase.Yall / 2; y < m_CellBase.Yall / 2; y++) {
								if (y <= Height) {
									m_CellBase.SetCell(x, y, z).selset = 0;
									continue;
								}
								m_CellBase.SetCell(x, y, z).selset = INVALID_ID;
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
					m_CellBase.m_Cell.resize(m_CellBase.Xall * m_CellBase.Yall * m_CellBase.Zall);
					fin.read((char*)&m_CellBase.m_Cell.at(0), sizeof(m_CellBase.m_Cell.at(0)) * m_CellBase.m_Cell.size());
					fin.close();
				}
				//簡略版を制作
				{
					m_CellxN.clear();
					m_CellxN.resize(total);
					m_CellxN.at(total - 1) = m_CellBase;//等倍なのでそのままコピー1
					for (int sel = total - 1 - 1; sel >= 0; sel--) {
						auto& cell = m_CellxN.at(sel);
						auto& cell2 = m_CellxN.at(sel + 1);

						cell.scaleRate = (int)pow(MulPer, total - 1 - sel);
						cell.Xall = m_CellBase.Xall / cell.scaleRate;
						cell.Yall = m_CellBase.Yall / cell.scaleRate;
						cell.Zall = m_CellBase.Zall / cell.scaleRate;
						cell.m_Cell.resize(cell.Xall * cell.Yall * cell.Zall);

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
									cell.SetCell(xm, ym, zm).selset = ((FillAll != 0) && ((float)FillCount / FillAll >= ((float)1 / 4))) ? 0 : INVALID_ID;
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
				const float ViewCos = std::cos(DrawParts->GetMainCamera().GetCamFov());
				for (int sel = 0; auto & cell : m_CellxN) {
					Vector3DX center = CamPos / (CellScale * cell.scaleRate);

					int xMaxmin = std::max((int)(center.x - Max), -cell.Xall / 2);
					int xMaxmax = std::min((int)(center.x + Max) + 1, cell.Xall / 2);
					int yMaxmin = std::max((int)(center.y - Max), -cell.Yall / 2);
					int yMaxmax = std::min((int)(center.y + Max) + 1, cell.Yall / 2);
					int zMaxmin = std::max((int)(center.z - Max), -cell.Zall / 2);
					int zMaxmax = std::min((int)(center.z + Max) + 1, cell.Zall / 2);

					int xMinmin = (int)(center.x - Max / MulPer);
					int xMinmax = (int)(center.x + Max / MulPer);
					int yMinmin = (int)(center.y - Max / MulPer);
					int yMinmax = (int)(center.y + Max / MulPer);
					int zMinmin = (int)(center.z - Max / MulPer);
					int zMinmax = (int)(center.z + Max / MulPer);

					for (int x = xMaxmin; x < xMaxmax; x++) {
						for (int y = yMaxmin; y < yMaxmax; y++) {
							for (int z = zMaxmin; z < zMaxmax; z++) {

								if ((cell.scaleRate != 1)
									&& ((xMinmin < x) && (x < xMinmax))
									&& ((yMinmin < y) && (y < yMinmax))
									&& ((zMinmin < z) && (z < zMinmax))
									) {
									continue;
								}

								const auto& Cell = cell.GetCell(x, y, z);
								if (Cell.selset == INVALID_ID) { continue; }
								if (Cell.inRock) { continue; }

								Vector3DX Pos = Vector3DX::vget((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f);
								if (Vector3DX::Dot((Pos - center).normalized(), CamVec) < ViewCos) { continue; }

								AddCube(Pos, (CellScale * cell.scaleRate), Colors[sel]);
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
