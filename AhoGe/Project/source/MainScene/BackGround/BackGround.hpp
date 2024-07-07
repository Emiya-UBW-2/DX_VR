#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:

			static const int tile_pic_size = 32;	//画像のタイルサイズ

		private:

			struct Blick {
				Vector3DX m_Pos;
				int palletNum{0};
				bool IsWall{false};
				float ZRad{0};

				int AddpalletNum[4]{};
				float AddZRad[4]{0};
			public:
				Blick(int x, int y) noexcept {
					m_Pos.x = (float)x;
					m_Pos.y = (float)y;
				}
			};

			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			struct CheckLines {
				Vector3DX Position[3]{ };
				Vector3DX Normal{};
				bool canuse{true};
			public:
				void CalcNormal() {
					Vector3DX StartP = this->Position[0];
					Vector3DX EndP = this->Position[1];
					Vector3DX In = this->Position[2];
					Vector3DX VecP = EndP - StartP;
					this->Normal = Vector3DX::Cross(VecP, Vector3DX::Cross(VecP, In - StartP)).normalized();
				}
			};
			//
			// 視点から影になっている部分の四角形
			struct ShadowBoxes {
				Vector3DX Position[4]{ };
			};
		private:
			std::vector<CheckLines> WallList;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			std::vector<ShadowBoxes> m_ShadowBoxes;
			std::vector<std::vector<std::shared_ptr<Blick>>> m_Blick;
			std::vector<GraphHandle>	m_MapChip;
			std::vector<GraphHandle>	m_WallChip;

			float m_AmbientShadowLength{36.f};
			float m_AmbientLightRad = deg2rad(45);
			Vector3DX m_PointLightPos;

			float ShadowPer = 0.5f;
			GraphHandle m_ShadowHandle;
		public://
			const float		CheckHideShadow(const Vector3DX & PosA, const Vector3DX & PosB, float Radius) noexcept;

			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) noexcept;

			const void		Get2DPos(int x, int y, Vector3DX* Min, Vector3DX* Center, Vector3DX* Max) const noexcept;
			void DrawAmbientShadow(int x, int y) const noexcept;
			void DrawPointShadow(int x, int y) noexcept;
		public://
			void		SetAmbientLight(float ShadowLen, float Rad) noexcept {
				m_AmbientShadowLength = ShadowLen;
				m_AmbientLightRad = Rad;
			}
			void		SetPointLight(const Vector3DX& Pos) noexcept {
				m_PointLightPos = Convert2DtoDisp(Pos);
			}
		public://
			//
			void			Init(const char* MapPath) noexcept;
			//
			void			Execute(void) noexcept;
			//
			void			SetupShadow(void) noexcept;
			//
			void			Draw(void) noexcept;
			void			DrawFront(void) noexcept;
			//
			void			Dispose(void) noexcept;
		};
	};
};
