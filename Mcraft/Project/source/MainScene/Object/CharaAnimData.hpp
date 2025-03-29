#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class GunAnimeID {
			//どれか択一で行うアニメーション
			LowReady,
			ADS,
			Shot,
			Cocking,
			ReloadStart_Empty,
			ReloadStart,
			Reload,
			ReloadEnd,
			Watch,
			ThrowReady,
			Throw,

			ChoiceOnceMax,//どれか択一で行うアニメーション の総数

			Aim = ChoiceOnceMax,
			Hammer,
			Open,

			Max,//全体

			None,

		};
		static const char* GunAnimeIDName[static_cast<int>(GunAnimeID::Max)] = {
			"Ready",
			"ADS",
			"Shot",
			"Cocking",
			"ReloadStart_Empty",
			"ReloadStart",
			"Reload",
			"ReloadEnd",
			"Watch",
			"ThrowReady",
			"Throw",

			"Aim",
			"Hammer",
			"Open",
		};
		enum class EnumGunAnim {
			M1911_ready1,
			M1911_ready2,
			M1911_aim1,
			M1911_aim2,
			M1911_ads,
			M1911_reloadstart_empty,
			M1911_reloadstart,
			M1911_reload,
			M1911_reloadend,
			M1911_watch,
			M16_ready1,
			M16_ready2,
			M16_aim,
			M16_ads,
			M16_reloadstart_empty,
			M16_reloadstart,
			M16_reload,
			M16_reloadend,
			M16_watch,
			M870_raload,
			M870_ready,
			RGD5_aim,
			RGD5_down,
			RGD5_Ready,
			RGD5_throw,
			Max,
		};
		static const char* EnumGunAnimName[static_cast<int>(EnumGunAnim::Max)] = {
			"M1911_ready1",
			"M1911_ready2",
			"M1911_aim1",
			"M1911_aim2",
			"M1911_ads",
			"M1911_reloadstart_empty",
			"M1911_reloadstart",
			"M1911_reload",
			"M1911_reloadend",
			"M1911_watch",
			"M16_ready1",
			"M16_ready2",
			"M16_aim",
			"M16_ads",
			"M16_reloadstart_empty",
			"M16_reloadstart",
			"M16_reload",
			"M16_reloadend",
			"M16_watch",
			"M870_raload",
			"M870_ready",
			"RGD5_aim",
			"RGD5_down",
			"RGD5_Ready",
			"RGD5_throw",
		};

		class GunAnimNow {
			Vector3DX		m_Rotate;
			Vector3DX		m_Pos;
		public:
			void Set(const Vector3DX& Rotate, const Vector3DX& Pos) {
				m_Rotate = Rotate;
				m_Pos = Pos;
			}
		public:
			const auto	GetMatrix() const noexcept {
				return
					(
						Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(this->m_Rotate.z)) *
						Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(this->m_Rotate.x)) *
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(this->m_Rotate.y))
						).Get44DX() *
					Matrix4x4DX::Mtrans(this->m_Pos * Scale3DRate);
			}
		};

		class GunAnimManager : public SingletonBase<GunAnimManager> {
		private:
			friend class SingletonBase<GunAnimManager>;
		private:
			class GunanimData {
				EnumGunAnim		m_EnumGunAnim{ EnumGunAnim::M16_ready1 };
				bool			m_IsLoop{ true };
			public:
				void Set(const std::string& data, EnumGunAnim EnumSel) {
					std::vector<std::string> Args;
					std::string RIGHTBuf = data;
					//タブけし
					while (true) {
						auto RIGHTBufLen = RIGHTBuf.find("\t");
						if (RIGHTBufLen != std::string::npos) {
							RIGHTBuf = RIGHTBuf.substr(0, RIGHTBufLen) + RIGHTBuf.substr(RIGHTBufLen + 1);
						}
						else {
							break;
						}
					}
					while (true) {
						auto div = RIGHTBuf.find(",");
						if (div != std::string::npos) {
							Args.emplace_back(RIGHTBuf.substr(0, div));
							RIGHTBuf = RIGHTBuf.substr(div + 1);
						}
						else {
							Args.emplace_back(RIGHTBuf);
							break;
						}
					}

					m_EnumGunAnim = EnumSel;
					m_IsLoop = (Args[0] == "Loop");
				}
			public:
				const auto& GetEnumGunAnim() const noexcept { return this->m_EnumGunAnim; }
				const auto& GetIsLoop() const noexcept { return this->m_IsLoop; }
			};

			struct AnimDatas {
			public:
				class GunAnim {
					Vector3DX		m_Rotate;
					Vector3DX		m_Pos;
					int				m_Frame{ 1 };
				public:
					void Set(const std::string& data) {
						std::vector<std::string> Args;
						std::string RIGHTBuf = data;
						//タブけし
						while (true) {
							auto RIGHTBufLen = RIGHTBuf.find("\t");
							if (RIGHTBufLen != std::string::npos) {
								RIGHTBuf = RIGHTBuf.substr(0, RIGHTBufLen) + RIGHTBuf.substr(RIGHTBufLen + 1);
							}
							else {
								break;
							}
						}
						while (true) {
							auto div = RIGHTBuf.find(",");
							if (div != std::string::npos) {
								Args.emplace_back(RIGHTBuf.substr(0, div));
								RIGHTBuf = RIGHTBuf.substr(div + 1);
							}
							else {
								Args.emplace_back(RIGHTBuf);
								break;
							}
						}
						m_Rotate.Set(std::stof(Args[0]), std::stof(Args[1]), std::stof(Args[2]));
						m_Pos.Set(std::stof(Args[3]), std::stof(Args[4]), -std::stof(Args[5]));
						m_Frame = std::stoi(Args[6]);
					}
				public:
					const auto& GetRotate() const noexcept { return this->m_Rotate; }
					const auto& GetPos() const noexcept { return this->m_Pos; }
					const auto& GetFrame() const noexcept { return this->m_Frame; }
				};
			public:
				std::shared_ptr<GunanimData> first;
				std::vector<std::shared_ptr<GunAnim>> second;

				const auto GetTotalTime() const {
					int total = 0;
					for (auto& ani : second) {
						total += ani->GetFrame();
					}
					return total;
				}
			};
		private:
			std::vector<AnimDatas>	m_Object;
#ifdef DEBUG_CAM
			Vector3DX DBG_AnimRot = Vector3DX::vget(0.f, 0.f, deg2rad(-20));
			Vector3DX DBG_AnimPos = Vector3DX::vget(-0.09f, -0.10f, -0.45f);
#endif
		public:
			void	Load(const char* filepath) {
				for (int loop = 0; loop < static_cast<int>(EnumGunAnim::Max); loop++) {
					std::string Path = filepath;
					Path += EnumGunAnimName[loop];
					Path += ".txt";
					m_Object.resize(m_Object.size() + 1);
					m_Object.back().first = std::make_shared<GunanimData>();

					FileStreamDX FileStream(Path.c_str());
					m_Object.back().first->Set(FileStream.SeekLineAndGetStr(), (EnumGunAnim)loop);
					while (true) {
						if (FileStream.ComeEof()) { break; }
						auto ALL = FileStream.SeekLineAndGetStr();
						m_Object.back().second.emplace_back(std::make_shared<AnimDatas::GunAnim>());
						m_Object.back().second.back()->Set(ALL);
					}
				}
			}

			const AnimDatas* GetAnimData(EnumGunAnim EnumSel) const noexcept {
				auto Find = std::find_if(m_Object.begin(), m_Object.end(), [&](const AnimDatas& tgt) {return tgt.first->GetEnumGunAnim() == EnumSel; });
				if (Find != m_Object.end()) {
					return &*Find;
				}
				return nullptr;
			}

			GunAnimNow	GetAnimNow(const AnimDatas* data, float nowframe) noexcept {
				GunAnimNow Ret; Ret.Set(Vector3DX::zero(), Vector3DX::zero());
				if (data) {
					float totalTime = (float)data->GetTotalTime();
					if (data->first->GetIsLoop()) {
						while (true) {
							if ((nowframe - totalTime) > 0.f) {
								nowframe -= totalTime;
							}
							else {
								break;
							}
						}
					}
					else {
						if ((nowframe - totalTime) > 0.f) {
							nowframe = totalTime;
						}
					}
					for (auto ani = data->second.begin(), e = data->second.end() - 1; ani != e; ++ani) {
						float Frame = static_cast<float>((*ani)->GetFrame());
						if ((nowframe - Frame) <= 0.f) {
							Ret.Set(
								Lerp((*ani)->GetRotate(), (*(ani + 1))->GetRotate(), nowframe / Frame),
								Lerp((*ani)->GetPos(), (*(ani + 1))->GetPos(), nowframe / Frame)
							);
#ifdef DEBUG_CAM
							if (0 <= DBG_CamSel && DBG_CamSel <= 3 && false) {
								auto* DXLib_refParts = DXLib_ref::Instance();
								Ret.Set(DBG_AnimRot, DBG_AnimPos);
								//
								if (CheckHitKey(KEY_INPUT_RCONTROL) != 0) {
									if (CheckHitKey(KEY_INPUT_J) != 0) {
										DBG_AnimRot.x += 5.f * DXLib_refParts->GetDeltaTime();
									}
									if (CheckHitKey(KEY_INPUT_L) != 0) {
										DBG_AnimRot.x -= 5.f * DXLib_refParts->GetDeltaTime();
									}
									//
									if (CheckHitKey(KEY_INPUT_I) != 0) {
										DBG_AnimRot.y += 5.f * DXLib_refParts->GetDeltaTime();
									}
									if (CheckHitKey(KEY_INPUT_K) != 0) {
										DBG_AnimRot.y -= 5.f * DXLib_refParts->GetDeltaTime();
									}
									//
									if (CheckHitKey(KEY_INPUT_U) != 0) {
										DBG_AnimRot.z += 5.f * DXLib_refParts->GetDeltaTime();
									}
									if (CheckHitKey(KEY_INPUT_O) != 0) {
										DBG_AnimRot.z -= 5.f * DXLib_refParts->GetDeltaTime();
									}
								}
								else {
									if (CheckHitKey(KEY_INPUT_J) != 0) {
										DBG_AnimPos.x += 0.01f * DXLib_refParts->GetDeltaTime();
									}
									if (CheckHitKey(KEY_INPUT_L) != 0) {
										DBG_AnimPos.x -= 0.01f * DXLib_refParts->GetDeltaTime();
									}
									//
									if (CheckHitKey(KEY_INPUT_I) != 0) {
										DBG_AnimPos.y += 0.01f * DXLib_refParts->GetDeltaTime();
									}
									if (CheckHitKey(KEY_INPUT_K) != 0) {
										DBG_AnimPos.y -= 0.01f * DXLib_refParts->GetDeltaTime();
									}
									//
									if (CheckHitKey(KEY_INPUT_U) != 0) {
										DBG_AnimPos.z += 0.01f * DXLib_refParts->GetDeltaTime();
									}
									if (CheckHitKey(KEY_INPUT_O) != 0) {
										DBG_AnimPos.z -= 0.01f * DXLib_refParts->GetDeltaTime();
									}
								}
								printfDx("Rot[%5.2f,%5.2f,%5.2f]\n", DBG_AnimRot.x, DBG_AnimRot.y, DBG_AnimRot.z);
								printfDx("Pos[%5.2f,%5.2f,%5.2f]\n", DBG_AnimPos.x, DBG_AnimPos.y, -DBG_AnimPos.z);
							}
							else {
								//DBG_AnimRot.Set(0.0f, 0.0f, 0.0f);
								//DBG_AnimPos.Set(0.0f, 0.0f, 0.0f);
							}
#endif
							break;
						}
						nowframe -= Frame;
					}
				}
				return Ret;
			}
		};


		struct GunAnimSet {
			std::array<EnumGunAnim, static_cast<int>(GunAnimeID::Max)> Anim;
		};
		const GunAnimSet GunAnimeSets[] = {
			//M4
			{
				EnumGunAnim::M16_ready1,
				EnumGunAnim::M16_ads,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M16_reloadstart_empty,
				EnumGunAnim::M16_reloadstart,
				EnumGunAnim::M16_reload,
				EnumGunAnim::M16_reloadend,
				EnumGunAnim::M16_watch,
				EnumGunAnim::RGD5_Ready,
				EnumGunAnim::RGD5_throw,
				EnumGunAnim::M16_aim,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//ハンドガン
			{
				EnumGunAnim::M1911_ready1,
				EnumGunAnim::M1911_ads,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M1911_reloadstart_empty,
				EnumGunAnim::M1911_reloadstart,
				EnumGunAnim::M1911_reload,
				EnumGunAnim::M1911_reloadend,
				EnumGunAnim::M1911_watch,
				EnumGunAnim::RGD5_Ready,
				EnumGunAnim::RGD5_throw,
				EnumGunAnim::M1911_aim1,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//M870
			{
				EnumGunAnim::M870_ready,
				EnumGunAnim::M16_ads,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M16_reloadstart_empty,
				EnumGunAnim::M16_reloadstart,
				EnumGunAnim::M870_raload,
				EnumGunAnim::M16_reloadend,
				EnumGunAnim::M16_watch,
				EnumGunAnim::RGD5_Ready,
				EnumGunAnim::RGD5_throw,
				EnumGunAnim::M16_aim,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//グレネード
			{
				EnumGunAnim::RGD5_down,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::RGD5_Ready,
				EnumGunAnim::RGD5_throw,
				EnumGunAnim::RGD5_aim,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
		};
	};
};
