#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		enum class CharaObjAnimeID : uint8_t {
			//
			AnimeIDMax,
		};
		enum class CharaFrame : uint8_t {
			Gun,
			Holster,
			Max,
		};
		static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
			"Gun",
			"Holster",
		};
		enum class CharaMaterial : uint8_t {
			Body,
			Max,
		};
		static const char* CharaMaterialName[static_cast<int>(CharaFrame::Max)] = {
			"‘D‘Ì",
		};
		enum class CharaShape : uint8_t {
			None,
			Max,
		};
		static const char* CharaShapeName[static_cast<int>(CharaShape::Max)] = {
			"None",
		};
	}
}
