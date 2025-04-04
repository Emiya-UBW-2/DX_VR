#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class GunAnimeID {
			//どれか択一で行うアニメーション
			LowReady,
			ADS,
			Cocking,
			ReloadStart_Empty,
			ReloadStart,
			Reload,
			ReloadEnd,
			Watch,
			ThrowReady,
			Throw,

			ChoiceOnceMax,//どれか択一で行うアニメーション の総数
			//ここからは常時/特殊パターンのアニメーション
			Base = ChoiceOnceMax,
			Shot,
			Hammer,
			Open,

			Max,//全体

			None,

		};
		static const char* GunAnimeIDName[static_cast<int>(GunAnimeID::Max)] = {
			"Ready",
			"ADS",
			"Cocking",
			"ReloadStart_Empty",
			"ReloadStart",
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
			AK_cocking,
			AK_reloadstart_empty,
			AK_reloadstart,
			AK_reload,
			AK_reloadend,
			AK_ready,
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
			"AK_cocking",
			"AK_reloadstart_empty",
			"AK_reloadstart",
			"AK_reload",
			"AK_reloadend",
			"AK_ready",
		};

		class FingerData {
			std::array<std::array<float, 5>, 2> m_Fingers{};
		public:
			auto& SetFingerPer(int LR, int Number) noexcept { return this->m_Fingers.at(LR).at(Number); }
			const auto& GetFingerPer(int LR, int Number) const noexcept { return this->m_Fingers.at(LR).at(Number); }
		public:
			FingerData operator+(const FingerData& p) const noexcept {
				FingerData Ret;
				for (int LRindex = 0; LRindex < 2; ++LRindex) {
					for (int Numberindex = 0; Numberindex < 5; ++Numberindex) {
					}
				}
				int LRindex = 0;
				for (const auto& LR : this->m_Fingers) {
					int Numberindex = 0;
					for (auto& f : LR) {
						Ret.m_Fingers.at(LRindex).at(Numberindex) = f + p.m_Fingers.at(LRindex).at(Numberindex);
						Numberindex++;
					}
					LRindex++;
				}
				return Ret;
			}
			FingerData operator-(const FingerData& p) const noexcept {
				FingerData Ret;
				for (int LRindex = 0; LRindex < 2; ++LRindex) {
					for (int Numberindex = 0; Numberindex < 5; ++Numberindex) {
					}
				}
				int LRindex = 0;
				for (const auto& LR : this->m_Fingers) {
					int Numberindex = 0;
					for (auto& f : LR) {
						Ret.m_Fingers.at(LRindex).at(Numberindex) = f - p.m_Fingers.at(LRindex).at(Numberindex);
						Numberindex++;
					}
					LRindex++;
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
					for (auto& f : LR) {
						Ret.m_Fingers.at(LRindex).at(Numberindex) = f * scale;
						Numberindex++;
					}
					LRindex++;
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
			const auto	GetMatrix(void) const noexcept {
				return
					(
						Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(this->m_Rotate.z)) *
						Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(this->m_Rotate.x)) *
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(this->m_Rotate.y))
						).Get44DX() *
					Matrix4x4DX::Mtrans(this->m_Pos * Scale3DRate);
			}
			const auto& GetFingerPer(void) const noexcept { return this->m_Finger; }
		};

		class GunAnimManager : public SingletonBase<GunAnimManager> {
		private:
			friend class SingletonBase<GunAnimManager>;
		private:
			class GunanimData {
				EnumGunAnim		m_EnumGunAnim{};
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

					this->m_EnumGunAnim = EnumSel;
					this->m_IsLoop = (Args[0] == "Loop");
				}
			public:
				const auto& GetEnumGunAnim(void) const noexcept { return this->m_EnumGunAnim; }
				const auto& GetIsLoop(void) const noexcept { return this->m_IsLoop; }
			};

			struct AnimDatas {
			public:
				class GunAnim {
					Vector3DX		m_Rotate;
					Vector3DX		m_Pos;
					int				m_Frame{ 1 };
					FingerData		m_Finger{};
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
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back().first = std::make_shared<GunanimData>();

					FileStreamDX FileStream(Path.c_str());
					this->m_Object.back().first->Set(FileStream.SeekLineAndGetStr(), (EnumGunAnim)loop);
					while (true) {
						if (FileStream.ComeEof()) { break; }
						auto ALL = FileStream.SeekLineAndGetStr();
						this->m_Object.back().second.emplace_back(std::make_shared<AnimDatas::GunAnim>());
						this->m_Object.back().second.back()->Set(ALL);
					}
				}
			}

			const AnimDatas* GetAnimData(EnumGunAnim EnumSel) const noexcept {
				auto Find = std::find_if(this->m_Object.begin(), this->m_Object.end(), [&](const AnimDatas& tgt) {return tgt.first->GetEnumGunAnim() == EnumSel; });
				if (Find != this->m_Object.end()) {
					return &*Find;
				}
				return nullptr;
			}

			GunAnimNow	GetAnimNow(const AnimDatas* data, float nowframe) noexcept {
				FingerData			Finger{};
				GunAnimNow Ret; Ret.Set(Vector3DX::zero(), Vector3DX::zero(), Finger);
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
							Finger = Lerp((*ani)->GetFingerPer(), (*(ani + 1))->GetFingerPer(), nowframe / Frame);

							Ret.Set(
								Lerp((*ani)->GetRotate(), (*(ani + 1))->GetRotate(), nowframe / Frame),
								Lerp((*ani)->GetPos(), (*(ani + 1))->GetPos(), nowframe / Frame),
								Finger
							);

#ifdef DEBUG_CAM
							if (0 <= DBG_CamSel && DBG_CamSel <= 3 && false) {
								auto* DXLib_refParts = DXLib_ref::Instance();
								Ret.Set(DBG_AnimRot, DBG_AnimPos, Finger);
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
				EnumGunAnim::M16_ready,
				EnumGunAnim::M16_ads,
				EnumGunAnim::AK_cocking,
				EnumGunAnim::AK_reloadstart_empty,
				EnumGunAnim::AK_reloadstart,
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
				EnumGunAnim::M1911_ready,
				EnumGunAnim::M1911_ads,
				EnumGunAnim::M1911_cocking,
				EnumGunAnim::M1911_reloadstart_empty,
				EnumGunAnim::M1911_reloadstart,
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
				EnumGunAnim::M870_ready,
				EnumGunAnim::M870_ads,
				EnumGunAnim::M870_cocking,
				EnumGunAnim::M870_reloadstart_empty,
				EnumGunAnim::M870_reloadstart,
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
				EnumGunAnim::RGD5_down,
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
				EnumGunAnim::Max,
			},
			//AK
			{
				EnumGunAnim::AK_ready,
				EnumGunAnim::M16_ads,
				EnumGunAnim::AK_cocking,
				EnumGunAnim::AK_reloadstart_empty,
				EnumGunAnim::AK_reloadstart,
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
		};
	};
};
