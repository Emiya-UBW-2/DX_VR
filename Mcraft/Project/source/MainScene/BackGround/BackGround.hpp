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

			//頂点データ
			std::vector<VERTEX3D>			m_vert32;
			size_t						m_vert32Num{ 0 };
			std::vector<VERTEX3D>			m_vert32SB;
			std::vector<VERTEX3DSHADER>		m_vert32S;
			size_t						m_vert32SNum{ 0 };
			//頂点インデックスデータ
			std::vector<uint32_t>		m_index32;
			size_t						m_index32Num{ 0 };
			std::vector<uint32_t>		m_index32S;
			size_t						m_index32SNum{ 0 };

			struct CellsData {
				std::vector<int8_t> m_Cell;
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

			const int total = 3;
			const int MulPer = 3;
			const float CellScale = Scale_Rate / 2.f / 2.f;
			const float Max = 50.f;

			std::vector<CellsData> m_CellxN;

			GraphHandle		m_tex{};
			GraphHandle		m_norm{};
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
		private:
			static bool		CalcIntersectionPoint(const Vector3DX& pointA, const Vector3DX& pointB, const Vector3DX& planePos, const Vector3DX& planenormal, Vector3DX* pointIntersection, bool* pSameVecNormalToA, bool* pOnFront) noexcept;
			static void		Bresenham3D(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2, const std::function<bool(int, int, int)>& OutPutLine) noexcept;

			void			AddCube(const CellsData& cell, int x, int y, int z, bool CheckFill, COLOR_U8 DifColor, COLOR_U8 SpcColor) noexcept;
			void			AddShadowCube(const CellsData& cell, int x, int y, int z, COLOR_U8 DifColor, COLOR_U8 SpcColor) noexcept;
		public:
			bool			ColRayBox(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AABBMinPos, const Vector3DX& AABBMaxPos, Vector3DX* Normal = nullptr, int* NormalNum = nullptr) const noexcept;
			bool			CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
		public:
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept;
		public:
			void			LoadCellsFile() noexcept;
			void			SaveCellsFile() noexcept;
		private:
			//簡略版を制作
			void			MakeSimple() noexcept;
			//遮蔽検索
			void			CalcOcclusion() noexcept;
		public://
			void			SetBlick(int x, int y, int z, int8_t sel) noexcept;
		public://
			void			Load(void) noexcept {
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				m_tex = GraphHandle::Load("data/tex.png");
				m_norm = GraphHandle::Load("data/nrm.png");
			}
			//
			void			Init(void) noexcept {
				//空
				this->m_ObjSky.SetScale(Vector3DX::vget(10.f, 10.f, 10.f));
				this->m_ObjSky.SetDifColorScale(GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				for (int i = 0, num = this->m_ObjSky.GetMaterialNum(); i < num; ++i) {
					//this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
					this->m_ObjSky.SetMaterialDifColor(i, GetColorF(0.7f, 0.7f, 0.7f, 1.f));
					this->m_ObjSky.SetMaterialAmbColor(i, GetColorF(0.f, 0.f, 0.f, 1.f));
				}
				m_CellxN.clear();
				m_CellxN.resize(total);
				if (false) {
					{
						PerlinNoise ns(GetRand(100));
						m_CellxN.back().Xall = 500;
						m_CellxN.back().Yall = 160;
						m_CellxN.back().Zall = 500;
						m_CellxN.back().m_Cell.resize((size_t)(m_CellxN.back().Xall * m_CellxN.back().Yall * m_CellxN.back().Zall));
						m_CellxN.back().scaleRate = 1;
						for (int x = -m_CellxN.back().Xall / 2; x < m_CellxN.back().Xall / 2; x++) {
							for (int z = -m_CellxN.back().Zall / 2; z < m_CellxN.back().Zall / 2; z++) {
								auto Height = static_cast<int>(ns.octaveNoise(2,
									(static_cast<float>(x + m_CellxN.back().Xall / 2)) / m_CellxN.back().Xall,
									(static_cast<float>(z + m_CellxN.back().Zall / 2)) / m_CellxN.back().Zall
								) * static_cast<float>(m_CellxN.back().Yall * 4 / 5)) - (m_CellxN.back().Yall * 4 / 5) / 2;
								for (int y = -m_CellxN.back().Yall / 2; y < m_CellxN.back().Yall / 2; y++) {
									if (y <= Height) {
										m_CellxN.back().SetCell(x, y, z) = 1;
										continue;
									}
									m_CellxN.back().SetCell(x, y, z) = 0;
								}
							}
						}
					}

					SaveCellsFile();
				}
				else if (false) {
					{
						PerlinNoise ns(GetRand(100));
						m_CellxN.back().Xall = 100;
						m_CellxN.back().Yall = 100;
						m_CellxN.back().Zall = 100;
						m_CellxN.back().m_Cell.resize((size_t)(m_CellxN.back().Xall * m_CellxN.back().Yall * m_CellxN.back().Zall));
						m_CellxN.back().scaleRate = 1;
						for (int x = -m_CellxN.back().Xall / 2; x < m_CellxN.back().Xall / 2; x++) {
							for (int z = -m_CellxN.back().Zall / 2; z < m_CellxN.back().Zall / 2; z++) {
								for (int y = -m_CellxN.back().Yall / 2; y < m_CellxN.back().Yall / 2; y++) {
									if (y <= 0) {
										m_CellxN.back().SetCell(x, y, z) = 1;
										continue;
									}
									if (y <= 2 + MulPer * 4) {
										if (x == -MulPer * 8 + 2 || x == -MulPer * 8 + 1 || x == -MulPer * 8) {
											m_CellxN.back().SetCell(x, y, z) = 1;
											continue;
										}
										if (x == MulPer * 8 - 2 || x == MulPer * 8 - 1 || x == MulPer * 8) {
											m_CellxN.back().SetCell(x, y, z) = 1;
											continue;
										}
									}
									m_CellxN.back().SetCell(x, y, z) = 0;
								}
							}
						}
					}

					SaveCellsFile();
				}
				else {
					LoadCellsFile();
				}
				//簡略版を制作
				MakeSimple();
				//遮蔽検索
				CalcOcclusion();

				MATERIALPARAM Param{};
				Param.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 1.0f);						// ディフューズカラー
				Param.Ambient = GetColorF(0.5f, 0.5f, 0.5f, 1.0f);						// アンビエントカラー
				Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// スペキュラカラー
				Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// エミッシブカラー
				Param.Power = 5.f;							// スペキュラハイライトの鮮明度
				SetMaterialParam(Param);
			}
			//
			void			Execute(void) noexcept {
				m_vert32Num = 0;
				m_index32Num = 0;
				m_vert32SNum = 0;
				m_index32SNum = 0;

				auto* DrawParts = DXDraw::Instance();
				auto* OptionParts = OPTION::Instance();
				const Vector3DX CamPos = DrawParts->GetMainCamera().GetCamPos();
				const Vector3DX CamVec = (DrawParts->GetMainCamera().GetCamVec() - CamPos).normalized();

				int BaseRate = 100;
				int ShadowRate = 100;
				float ShadowMax = Max;
				switch (OptionParts->GetParamInt(EnumSaveParam::ObjLevel)) {
				case 0:
				case 1:
					BaseRate = MulPer * MulPer;
					ShadowRate = 1;
					ShadowMax = 10.f;
					break;
				case 2:
					BaseRate = MulPer * MulPer * MulPer;
					ShadowRate = MulPer;
					ShadowMax = 15.f;
					break;
				case 3:
					BaseRate = MulPer * MulPer * MulPer * MulPer;
					ShadowRate = MulPer * MulPer;
					ShadowMax = 25.f;
					break;
				default:
					break;
				};
				for (auto & cell : m_CellxN) {
					Vector3DX center = CamPos / (CellScale * cell.scaleRate);
					//
					if (BaseRate >= cell.scaleRate) {
						int xMaxmin = std::max(static_cast<int>(center.x - Max), -cell.Xall / 2);
						int xMaxmax = std::min(static_cast<int>(center.x + Max) + 1, cell.Xall / 2);
						int yMaxmin = std::max(static_cast<int>(center.y - Max), -cell.Yall / 2);
						int yMaxmax = std::min(static_cast<int>(center.y + Max) + 1, cell.Yall / 2);
						int zMaxmin = std::max(static_cast<int>(center.z - Max), -cell.Zall / 2);
						int zMaxmax = std::min(static_cast<int>(center.z + Max) + 1, cell.Zall / 2);

						int xMinmin = static_cast<int>(center.x - Max / MulPer);
						int xMinmax = static_cast<int>(center.x + Max / MulPer);
						int yMinmin = static_cast<int>(center.y - Max / MulPer);
						int yMinmax = static_cast<int>(center.y + Max / MulPer);
						int zMinmin = static_cast<int>(center.z - Max / MulPer);
						int zMinmax = static_cast<int>(center.z + Max / MulPer);
						for (int x = xMaxmin; x < xMaxmax; x++) {
							/*
							//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
							Vector3DX YZMinPos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(yMaxmin), static_cast<float>(zMaxmin)) + (Vector3DX::one() * 0.5f);
							Vector3DX YZMaxPos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(zMaxmax), static_cast<float>(zMaxmax)) + (Vector3DX::one() * 0.5f);
							Vector3DX YZPos2 = Vector3DX::vget(YZMinPos.x, YZMinPos.y, YZMaxPos.z);
							Vector3DX YZPos3 = Vector3DX::vget(YZMinPos.x, YZMaxPos.y, YZMinPos.z);
							bool IsHit = false;
							while (true) {
								if (Vector3DX::Dot(CamVec, (YZMinPos - center)) > 0.f) { IsHit = true; break; }
								if (Vector3DX::Dot(CamVec, (YZMaxPos - center)) > 0.f) { IsHit = true; break; }
								if (Vector3DX::Dot(CamVec, (YZPos2 - center)) > 0.f) { IsHit = true; break; }
								if (Vector3DX::Dot(CamVec, (YZPos3 - center)) > 0.f) { IsHit = true; break; }
								break;
							}
							if (!IsHit) {
								continue;
							}
							//*/
							for (int y = yMaxmin; y < yMaxmax; y++) {
								int zMaxminT = zMaxmin;
								int zMaxmaxT = zMaxmax;
								//*
								Vector3DX ZMinPos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(zMaxmin)) + (Vector3DX::one() * 0.5f);
								Vector3DX ZMaxPos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(zMaxmax)) + (Vector3DX::one() * 0.5f);
								Vector3DX HitPos;
								bool pSameVecNormalToA{};
								bool OnFront{};
								if (CalcIntersectionPoint(ZMinPos, ZMaxPos, center, CamVec, &HitPos, &pSameVecNormalToA, &OnFront)) {
									if (!pSameVecNormalToA) {
										zMaxminT = static_cast<int>(HitPos.z - 0.5) - 1;
									}
									else {
										zMaxmaxT = static_cast<int>(HitPos.z - 0.5) + 1;
									}
								}
								else {
									if (!OnFront) { continue; }
								}
								//*/
								zMaxminT = std::max(zMaxminT, zMaxmin);
								zMaxmaxT = std::min(zMaxmaxT, zMaxmax);

								for (int z = zMaxminT; z < zMaxmaxT; z++) {
									bool checkFill = true;
									if (cell.scaleRate != 1) {
										if (((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax))) {
											if ((zMinmin < z) && (z < zMinmax)) {
												z = zMinmax - 1;
												continue;
											}
										}
										checkFill = !(((xMinmin <= x) && (x <= xMinmax)) && ((yMinmin <= y) && (y <= yMinmax)) && ((zMinmin <= z) && (z <= zMinmax)));
									}
									const auto& Cell = cell.GetCell(x, y, z);
									if (Cell <= 0) { continue; }

									AddCube(cell, x, y, z, checkFill, GetColorU8(128, 128, 128, 255), GetColorU8(64, 64, 64, 255));
								}
							}
						}
					}
					//
					if (ShadowRate >= cell.scaleRate) {
						int xMaxmin = std::max(static_cast<int>(center.x - ShadowMax), -cell.Xall / 2);
						int xMaxmax = std::min(static_cast<int>(center.x + ShadowMax) + 1, cell.Xall / 2);
						int yMaxmin = std::max(static_cast<int>(center.y - ShadowMax), -cell.Yall / 2);
						int yMaxmax = std::min(static_cast<int>(center.y + ShadowMax) + 1, cell.Yall / 2);
						int zMaxmin = std::max(static_cast<int>(center.z - ShadowMax), -cell.Zall / 2);
						int zMaxmax = std::min(static_cast<int>(center.z + ShadowMax) + 1, cell.Zall / 2);

						int xMinmin = static_cast<int>(center.x - ShadowMax / MulPer);
						int xMinmax = static_cast<int>(center.x + ShadowMax / MulPer);
						int yMinmin = static_cast<int>(center.y - ShadowMax / MulPer);
						int yMinmax = static_cast<int>(center.y + ShadowMax / MulPer);
						int zMinmin = static_cast<int>(center.z - ShadowMax / MulPer);
						int zMinmax = static_cast<int>(center.z + ShadowMax / MulPer);
						for (int x = xMaxmin; x < xMaxmax; x++) {
							for (int y = yMaxmin; y < yMaxmax; y++) {
								for (int z = zMaxmin; z < zMaxmax; z++) {
									if (cell.scaleRate != 1) {
										if (((xMinmin < x) && (x < xMinmax)) && ((yMinmin < y) && (y < yMinmax)) && ((zMinmin < z) && (z < zMinmax))) {
											continue;
										}
									}
									const auto& Cell = cell.GetCell(x, y, z);
									if (Cell <= 0) { continue; }
									AddShadowCube(cell, x, y, z, GetColorU8(128, 128, 128, 255), GetColorU8(64, 64, 64, 255));
								}
							}
						}
					}
				}
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw(void) noexcept {
				if (m_vert32SNum > 0 && m_index32SNum > 0) {
					DrawPolygon32bitIndexed3D(m_vert32SB.data(), static_cast<int>(m_vert32SNum), m_index32S.data(), static_cast<int>(m_index32SNum / 3), m_tex.get(), TRUE);
				}
			}
			void			SetShadow_Draw_Rigid(void) noexcept {
				if (m_vert32SNum > 0 && m_index32SNum > 0) {
					SetUseTextureToShader(0, m_tex.get());
					DrawPolygon32bitIndexed3DToShader(m_vert32S.data(), static_cast<int>(m_vert32SNum), m_index32S.data(), static_cast<int>(m_index32SNum / 3));
					SetUseTextureToShader(0, INVALID_ID);
				}
			}
			void			Draw(void) noexcept {
				if (m_vert32Num > 0 && m_index32Num > 0) {
					DrawPolygon32bitIndexed3D(m_vert32.data(), static_cast<int>(m_vert32Num), m_index32.data(), static_cast<int>(m_index32Num / 3), m_tex.get(), TRUE);
				}
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				SaveCellsFile();
			}
		};
	}
}
