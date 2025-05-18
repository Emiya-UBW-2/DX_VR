#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Charas {
		enum class GunAnimeID {
			//どれか択一で行うアニメーション
			HighReady,
			LowReady,
			ADS,
			Cocking,
			ReloadStart_Empty,
			ReloadStart,
			ReloadWait,
			Reload,
			ReloadEnd,
			Watch,
			ThrowReady,
			Throw,

			ChoiceOnceMax,//どれか択一で行うアニメーション の総数
			//ここからは常時/特殊パターンのアニメーション
			Aim = ChoiceOnceMax,
			Shot,
			Hammer,
			Open,

			Max,//全体

			Base,

		};
		static const char* GunAnimeIDName[static_cast<int>(GunAnimeID::Max)] = {
			"Ready",
			"Ready",
			"ADS",
			"Cocking",
			"ReloadStart_Empty",
			"ReloadStart",
			"ReloadWait",
			"Reload",
			"ReloadEnd",
			"Watch",
			"ThrowReady",
			"Throw",

			"Aim",
			"Shot",
			"Hammer",
			"Open",
		};
		enum class EnumGunAnim {
			M1911_ready,
			M1911_aim,
			M1911_shot,
			M1911_ads,
			M1911_reloadstart_empty,
			M1911_reloadstart,
			M1911_reload,
			M1911_reloadend,
			M1911_watch,
			M1911_cocking,
			M16_ready,
			M16_aim,
			M16_ads,
			M16_shot,
			M16_reloadstart_empty,
			M16_reloadstart,
			M16_reload,
			M16_reloadend,
			M16_watch,
			M16_cocking,
			M870_raloadwait,
			M870_raload,
			M870_ready,
			M870_cocking,
			M870_ads,
			M870_reloadstart_empty,
			M870_reloadstart,
			M870_reloadend,
			M870_watch,
			M870_aim,
			M870_shot,
			RGD5_aim,
			RGD5_down,
			RGD5_Ready,
			RGD5_throw,
			RGD5_watch,
			AK_cocking,
			AK_reloadstart_empty,
			AK_reloadstart,
			AK_reloadwait,
			AK_reload,
			AK_reloadend,
			AK_ready,
			AK_highready,
			M700_highready,
			M700_ready,
			M700_ads,
			M700_cocking,
			M700_reloadstart,
			M700_raloadwait,
			M700_raload,
			M700_reloadend,
			M700_watch,
			M700_aim,
			M700_shot,
			Max,
		};
		static const char* EnumGunAnimName[static_cast<int>(EnumGunAnim::Max)] = {
			"M1911_ready",
			"M1911_aim",
			"M1911_shot",
			"M1911_ads",
			"M1911_reloadstart_empty",
			"M1911_reloadstart",
			"M1911_reload",
			"M1911_reloadend",
			"M1911_watch",
			"M1911_cocking",
			"M16_ready",
			"M16_aim",
			"M16_ads",
			"M16_shot",
			"M16_reloadstart_empty",
			"M16_reloadstart",
			"M16_reload",
			"M16_reloadend",
			"M16_watch",
			"M16_cocking",
			"M870_raloadwait",
			"M870_raload",
			"M870_ready",
			"M870_cocking",
			"M870_ads",
			"M870_reloadstart_empty",
			"M870_reloadstart",
			"M870_reloadend",
			"M870_watch",
			"M870_aim",
			"M870_shot",
			"RGD5_aim",
			"RGD5_down",
			"RGD5_Ready",
			"RGD5_throw",
			"RGD5_watch",
			"AK_cocking",
			"AK_reloadstart_empty",
			"AK_reloadstart",
			"AK_reloadwait",
			"AK_reload",
			"AK_reloadend",
			"AK_ready",
			"AK_highready",
			"M700_highready",
			"M700_ready",
			"M700_ads",
			"M700_cocking",
			"M700_reloadstart",
			"M700_raloadwait",
			"M700_raload",
			"M700_reloadend",
			"M700_watch",
			"M700_aim",
			"M700_shot",
		};

		class FingerData {
			std::array<std::array<float, 5>, 2>	m_Fingers{};
		public:
			auto& SetFingerPer(int LR, int Number) noexcept { return this->m_Fingers[LR][Number]; }
			const auto& GetFingerPer(int LR, int Number) const noexcept { return this->m_Fingers[LR][Number]; }
		public:
			FingerData operator+(const FingerData& param) const noexcept {
				FingerData Ret;
				for (int LRindex = 0; LRindex < 2; ++LRindex) {
					for (int Numberindex = 0; Numberindex < 5; ++Numberindex) {
					}
				}
				int LRindex = 0;
				for (const auto& LR : this->m_Fingers) {
					int Numberindex = 0;
					for (auto& finger : LR) {
						Ret.SetFingerPer(LRindex, Numberindex) = finger + param.GetFingerPer(LRindex, Numberindex);
						++Numberindex;
					}
					++LRindex;
				}
				return Ret;
			}
			FingerData operator-(const FingerData& param) const noexcept {
				FingerData Ret;
				for (int LRindex = 0; LRindex < 2; ++LRindex) {
					for (int Numberindex = 0; Numberindex < 5; ++Numberindex) {
					}
				}
				int LRindex = 0;
				for (const auto& LR : this->m_Fingers) {
					int Numberindex = 0;
					for (auto& finger : LR) {
						Ret.SetFingerPer(LRindex, Numberindex) = finger - param.GetFingerPer(LRindex, Numberindex);
						++Numberindex;
					}
					++LRindex;
				}
				return Ret;
			}
			FingerData operator*(float scale) const noexcept {
				FingerData Ret;
				for (int LRindex = 0; LRindex < 2; ++LRindex) {
					for (int Numberindex = 0; Numberindex < 5; ++Numberindex) {
					}
				}
				int LRindex = 0;
				for (const auto& LR : this->m_Fingers) {
					int Numberindex = 0;
					for (auto& finger : LR) {
						Ret.SetFingerPer(LRindex, Numberindex) = finger * scale;
						++Numberindex;
					}
					++LRindex;
				}
				return Ret;
			}
		};

		class GunAnimNow {
			Vector3DX		m_Rotate;
			Vector3DX		m_Pos;
			FingerData		m_Finger{};
		public:
			void Set(const Vector3DX& Rotate, const Vector3DX& Pos, const FingerData& Finger) {
				this->m_Rotate = Rotate;
				this->m_Pos = Pos;
				this->m_Finger = Finger;
			}
		public:
			const auto	GetRot(void) const noexcept {
				return
					Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(this->m_Rotate.z)) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(this->m_Rotate.x)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(this->m_Rotate.y));
			}
			const auto	GetPos(void) const noexcept {
				return this->m_Pos * Scale3DRate;
			}
			const auto& GetFingerPer(void) const noexcept { return this->m_Finger; }
		public:
			void LerpNow(const GunAnimNow& target, float Per) {
				this->m_Rotate = Lerp(this->m_Rotate, target.m_Rotate, Per);
				this->m_Pos = Lerp(this->m_Pos, target.m_Pos, Per);
				this->m_Finger = Lerp(this->m_Finger, target.m_Finger, Per);
			}
		};

		class GunAnimManager : public SingletonBase<GunAnimManager> {
		private:
			friend class SingletonBase<GunAnimManager>;
		private:
			class GunAnimData {
				EnumGunAnim		m_EnumGunAnim{};
				bool			m_IsLoop{ true };
			public:
				GunAnimData(const std::string& data, EnumGunAnim EnumSelect) noexcept {
					Set(data, EnumSelect);
				}
			public:
				void Set(const std::string& data, EnumGunAnim EnumSelect) {
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

					this->m_EnumGunAnim = EnumSelect;
					this->m_IsLoop = (Args[0] == "Loop");
				}
			public:
				const auto& GetEnumGunAnim(void) const noexcept { return this->m_EnumGunAnim; }
				const auto& GetIsLoop(void) const noexcept { return this->m_IsLoop; }
			};

			class AnimDatas {
			public:
				class GunAnim {
					Vector3DX		m_Rotate;
					Vector3DX		m_Pos;
					int				m_Frame{ 1 };
					FingerData		m_Finger{};
				public:
					GunAnim(const std::string& data) noexcept {
						Set(data);
					}
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
						this->m_Rotate.Set(std::stof(Args[0]), std::stof(Args[1]), std::stof(Args[2]));
						this->m_Pos.Set(std::stof(Args[3]), std::stof(Args[4]), -std::stof(Args[5]));
						this->m_Frame = std::stoi(Args[6]);

						int loop = 7;
						for (int LRindex = 0; LRindex < 2;++LRindex) {
							for (int Numberindex = 0; Numberindex < 5;++Numberindex) {
								this->m_Finger.SetFingerPer(LRindex, Numberindex) = std::stof(Args[loop]);
								++loop;
							}
						}
					}
				public:
					const auto& GetRotate(void) const noexcept { return this->m_Rotate; }
					const auto& GetPos(void) const noexcept { return this->m_Pos; }
					const auto& GetFrame(void) const noexcept { return this->m_Frame; }
					const auto& GetFingerPer(void) const noexcept { return this->m_Finger; }
				};
			public:
				std::unique_ptr<GunAnimData>			m_AnimData;
				std::vector<std::unique_ptr<GunAnim>>	m_AnimPoint;
			public:
				AnimDatas(void) noexcept {}
				~AnimDatas(void) noexcept {
					this->m_AnimData.reset();
					for (auto& point : this->m_AnimPoint) {
						point.reset();
					}
					this->m_AnimPoint.clear();
				}
			public:
				const auto GetTotalTime() const {
					int Ret = 0;
					for (auto& ani : m_AnimPoint) {
						Ret += ani->GetFrame();
					}
					return Ret;
				}
			};
		private:
			std::vector<std::unique_ptr<AnimDatas>>	m_Data;
#if defined(DEBUG) && DEBUG_CAM && DEBUG_ANIM
			Vector3DX DBG_AnimRot = Vector3DX::vget(0.0f, 0.0f, deg2rad(0));
			Vector3DX DBG_AnimPos = Vector3DX::vget(-0.06f, -0.15f, -0.23f);
#endif
		private:
			GunAnimManager(void) noexcept {}
			virtual ~GunAnimManager(void) noexcept {
				for (auto& obj : this->m_Data) {
					obj.reset();
				}
				this->m_Data.clear();
			}
		public:
			void	Load(const char* filepath) {
				for (int loop = 0; loop < static_cast<int>(EnumGunAnim::Max); ++loop) {
					std::string Path = filepath;
					Path += EnumGunAnimName[loop];
					Path += ".txt";
					this->m_Data.emplace_back(std::make_unique<AnimDatas>());
					FileStreamDX FileStream(Path.c_str());
					this->m_Data.back()->m_AnimData = std::make_unique<GunAnimData>(FileStream.SeekLineAndGetStr(), (EnumGunAnim)loop);
					while (true) {
						if (FileStream.ComeEof()) { break; }
						this->m_Data.back()->m_AnimPoint.emplace_back(std::make_unique<AnimDatas::GunAnim>(FileStream.SeekLineAndGetStr()));
					}
				}
			}

			const std::unique_ptr<AnimDatas>* GetAnimData(EnumGunAnim EnumSelect) const noexcept {
				auto Find = std::find_if(this->m_Data.begin(), this->m_Data.end(), [&](const std::unique_ptr<AnimDatas>& tgt) {return tgt->m_AnimData->GetEnumGunAnim() == EnumSelect; });
				if (Find != this->m_Data.end()) {
					return &*Find;
				}
				return nullptr;
			}

			GunAnimNow	GetAnimNow(PlayerID
#if defined(DEBUG) && DEBUG_CAM && DEBUG_ANIM
				ID
#endif
				, const std::unique_ptr<AnimDatas>* data, float nowframe) noexcept {
				FingerData			Finger{};
				GunAnimNow Ret; Ret.Set(Vector3DX::zero(), Vector3DX::zero(), Finger);
				if (data) {
					float totalTime = static_cast<float>((*data)->GetTotalTime());
					if ((*data)->m_AnimData->GetIsLoop()) {
						while (true) {
							if ((nowframe - totalTime) > 0.0f) {
								nowframe -= totalTime;
							}
							else {
								break;
							}
						}
					}
					else {
						if ((nowframe - totalTime) > 0.0f) {
							nowframe = totalTime;
						}
					}
					for (auto ani = (*data)->m_AnimPoint.begin(), endi = (*data)->m_AnimPoint.end() - 1; ani != endi; ++ani) {
						float Frame = static_cast<float>((*ani)->GetFrame());
						if ((nowframe - Frame) <= 0.0f) {
							Finger = Lerp((*ani)->GetFingerPer(), (*(ani + 1))->GetFingerPer(), nowframe / Frame);

							Ret.Set(
								Lerp((*ani)->GetRotate(), (*(ani + 1))->GetRotate(), nowframe / Frame),
								Lerp((*ani)->GetPos(), (*(ani + 1))->GetPos(), nowframe / Frame),
								Finger
							);

#if defined(DEBUG) && DEBUG_CAM && DEBUG_ANIM
							if (ID == 0) {
								if (0 <= DBG_CamSelect && DBG_CamSelect <= 3) {
									auto* DXLib_refParts = DXLib_ref::Instance();
									//
									if (CheckHitKey(KEY_INPUT_RCONTROL) != 0) {
										if (CheckHitKey(KEY_INPUT_J) != 0) {
											DBG_AnimRot.x += 5.0f * DXLib_refParts->GetDeltaTime();
										}
										if (CheckHitKey(KEY_INPUT_L) != 0) {
											DBG_AnimRot.x -= 5.0f * DXLib_refParts->GetDeltaTime();
										}
										//
										if (CheckHitKey(KEY_INPUT_I) != 0) {
											DBG_AnimRot.y += 5.0f * DXLib_refParts->GetDeltaTime();
										}
										if (CheckHitKey(KEY_INPUT_K) != 0) {
											DBG_AnimRot.y -= 5.0f * DXLib_refParts->GetDeltaTime();
										}
										//
										if (CheckHitKey(KEY_INPUT_U) != 0) {
											DBG_AnimRot.z += 5.0f * DXLib_refParts->GetDeltaTime();
										}
										if (CheckHitKey(KEY_INPUT_O) != 0) {
											DBG_AnimRot.z -= 5.0f * DXLib_refParts->GetDeltaTime();
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
									Ret.Set(DBG_AnimRot, DBG_AnimPos, Finger);
								}
								else {
									//DBG_AnimRot.Set(0.0f, 0.0f, 0.0f);
									//DBG_AnimPos.Set(0.0f, 0.0f, 0.0f);
								}
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
				EnumGunAnim::AK_highready,
				EnumGunAnim::M16_ready,
				EnumGunAnim::M16_ads,
				EnumGunAnim::M16_cocking,
				EnumGunAnim::AK_reloadstart_empty,
				EnumGunAnim::AK_reloadstart,
				EnumGunAnim::AK_reloadwait,
				EnumGunAnim::AK_reload,
				EnumGunAnim::AK_reloadend,
				EnumGunAnim::M16_watch,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M16_aim,
				EnumGunAnim::M16_shot,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//ハンドガン
			{
				EnumGunAnim::AK_highready,
				EnumGunAnim::M1911_ready,
				EnumGunAnim::M1911_ads,
				EnumGunAnim::M1911_cocking,
				EnumGunAnim::M1911_reloadstart_empty,
				EnumGunAnim::M1911_reloadstart,
				EnumGunAnim::M1911_reload,
				EnumGunAnim::M1911_reload,
				EnumGunAnim::M1911_reloadend,
				EnumGunAnim::M1911_watch,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M1911_aim,
				EnumGunAnim::M1911_shot,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//M870
			{
				EnumGunAnim::AK_highready,
				EnumGunAnim::M870_ready,
				EnumGunAnim::M870_ads,
				EnumGunAnim::M870_cocking,
				EnumGunAnim::M870_reloadstart_empty,
				EnumGunAnim::M870_reloadstart,
				EnumGunAnim::M870_raloadwait,
				EnumGunAnim::M870_raload,
				EnumGunAnim::M870_reloadend,
				EnumGunAnim::M870_watch,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M870_aim,
				EnumGunAnim::M870_shot,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//グレネード
			{
				EnumGunAnim::AK_highready,
				EnumGunAnim::RGD5_down,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::RGD5_watch,
				EnumGunAnim::RGD5_Ready,
				EnumGunAnim::RGD5_throw,
				EnumGunAnim::RGD5_aim,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//AK
			{
				EnumGunAnim::AK_highready,
				EnumGunAnim::AK_ready,
				EnumGunAnim::M16_ads,
				EnumGunAnim::AK_cocking,
				EnumGunAnim::AK_reloadstart_empty,
				EnumGunAnim::AK_reloadstart,
				EnumGunAnim::AK_reloadwait,
				EnumGunAnim::AK_reload,
				EnumGunAnim::AK_reloadend,
				EnumGunAnim::M16_watch,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M16_aim,
				EnumGunAnim::M16_shot,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
			//M870
			{
				EnumGunAnim::M700_highready,
				EnumGunAnim::M700_ready,
				EnumGunAnim::M700_ads,
				EnumGunAnim::M700_cocking,
				EnumGunAnim::M700_reloadstart,
				EnumGunAnim::M700_reloadstart,
				EnumGunAnim::M700_raloadwait,
				EnumGunAnim::M700_raload,
				EnumGunAnim::M700_reloadend,
				EnumGunAnim::M700_watch,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
				EnumGunAnim::M700_aim,
				EnumGunAnim::M700_shot,
				EnumGunAnim::Max,
				EnumGunAnim::Max,
			},
		};
	}
}
