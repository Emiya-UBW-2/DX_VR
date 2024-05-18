#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			std::array<int, 23>				intParam{ 0 };
			std::array<float, 5>			floatParam{ 0 };
			std::array<std::string, 7>		strParam;
			std::array<GraphHandle*, 3>		ItemGraphPtr{ 0 };

			int Blend3Int(int pInt1, int pInt2, int pInt3, float per) {
				int ans;
				ans = (int)(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * (float)pInt3);
				ans += (int)(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * (float)pInt2);
				ans += (int)(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * (float)pInt1);
				return ans;
			}

			COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, float per) {
				int r, g, b;
				r = Blend3Int(Color1.r, Color2.r, Color3.r, per);
				g = Blend3Int(Color1.g, Color2.g, Color3.g, per);
				b = Blend3Int(Color1.b, Color2.b, Color3.b, per);
				return GetColorU8(r, g, b, 255);
			}
			void			DrawGauge(int xp1, int yp1, int xp2, int yp2,
				int Param1, int ParamMax, int ParamBuf,
				COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, unsigned int ColorAdd, unsigned int ColorSub, unsigned int EdgeColor) {
				DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, EdgeColor, FALSE);
				DrawBox(xp1 + 1, yp1 + 1, xp2 - 1, yp2 - 1, EdgeColor, FALSE);
				int length = (xp2 - 2) - (xp1 + 2);

				COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)Param1 / (float)ParamMax);

				DrawBox(xp1 + 2 + length * std::max(Param1, ParamBuf) / ParamMax, yp1 + 2, xp2 - 2, yp2 - 2, GetColor(0, 0, 0), TRUE);
				DrawBox(xp1 + 2, yp1 + 2, xp1 + 2 + length * Param1 / ParamMax, yp2 - 2, GetColor(Color.r, Color.g, Color.b), TRUE);
				DrawBox(xp1 + 2 + length * ParamBuf / ParamMax, yp1 + 2, xp1 + 2 + length * Param1 / ParamMax, yp2 - 2, (ParamBuf > Param1) ? ColorSub : ColorAdd, TRUE);
			}
		public:
			void			Set(void) noexcept {
				for (int i = 0; i < 3; i++) {
					ItemGraphPtr[i] = nullptr;
				}
			}
			void			Draw(void) noexcept {
			}
			void			Dispose(void) noexcept {}

			void			SetIntParam(int ID, int value) { intParam[ID] = value; }
			void			SetfloatParam(int ID, float value) { floatParam[ID] = value; }
			void			SetStrParam(int ID, std::string_view value) { strParam[ID] = value; }
			void			SetItemGraph(int ID, const GraphHandle* value) { ItemGraphPtr[ID] = (GraphHandle*)value; }
		};
	};
};