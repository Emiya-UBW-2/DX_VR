#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class EnumGunAnimType {
			Aim,
			ADS,
			Run,
			ReloadStart_Empty,
			ReloadStart,
			Reload,
			ReloadEnd,
			Ready,
			Check,
			Watch,
			Melee,
			AmmoLoad,
			Max,
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
			M1911_run,
			M1911_check1,
			M1911_watch,
			M1911_melee,
			M1911_ammoload,
			M16_ready1,
			M16_ready2,
			M16_aim,
			M16_ads,
			M16_reloadstart_empty,
			M16_reloadstart,
			M16_reload,
			M16_reloadend,
			M16_run,
			M16_check1,
			M16_watch,
			M16_ammoload,
			M16_melee,
			M870_raload,
			M870_ready,
			FMG9_aim,
			FMG9_ads,
			Max,
		};
		static const char* EnumGunAnimName[(int)EnumGunAnim::Max] = {
			"M1911_ready1",
			"M1911_ready2",
			"M1911_aim1",
			"M1911_aim2",
			"M1911_ads",
			"M1911_reloadstart_empty",
			"M1911_reloadstart",
			"M1911_reload",
			"M1911_reloadend",
			"M1911_run",
			"M1911_check1",
			"M1911_watch",
			"M1911_melee",
			"M1911_ammoload",
			"M16_ready1",
			"M16_ready2",
			"M16_aim",
			"M16_ads",
			"M16_reloadstart_empty",
			"M16_reloadstart",
			"M16_reload",
			"M16_reloadend",
			"M16_run",
			"M16_check1",
			"M16_watch",
			"M16_ammoload",
			"M16_melee",
			"M870_raload",
			"M870_ready",
			"FMG9_aim",
			"FMG9_ads",
		};

		class GunanimData {
			EnumGunAnim		m_EnumGunAnim{EnumGunAnim::M16_ready1};
			bool			m_IsLoop{true};
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
		class GunAnimNow {
			VECTOR_ref		m_Rotate;
			VECTOR_ref		m_Pos;
		public:
			void Set(const VECTOR_ref& Rotate, const VECTOR_ref& Pos) {
				m_Rotate = Rotate;
				m_Pos = Pos;
			}
		public:
			const auto	GetRot() const noexcept {
				return MATRIX_ref::RotY(deg2rad(this->m_Rotate.x())) * MATRIX_ref::RotX(deg2rad(this->m_Rotate.y())) * MATRIX_ref::RotZ(deg2rad(this->m_Rotate.z()));
			}
			const auto GetPos() const noexcept { return this->m_Pos*Scale_Rate; }
		};
		class GunAnim {
			VECTOR_ref		m_Rotate;
			VECTOR_ref		m_Pos;
			int				m_Frame{1};
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

		class GunAnimManager : public SingletonBase<GunAnimManager> {
		private:
			friend class SingletonBase<GunAnimManager>;
			struct AnimDatas {
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
		public:
			void	Load(const char* filepath) {
				for (int loop = 0; loop < (int)EnumGunAnim::Max; loop++) {
					std::string Path = filepath;
					Path += EnumGunAnimName[loop];
					Path += ".txt";
					LoadAction(Path.c_str(), (EnumGunAnim)loop);
				}
			}
			void	LoadAction(const char* filepath, EnumGunAnim EnumSel) noexcept {
				m_Object.resize(m_Object.size() + 1);
				m_Object.back().first = std::make_shared<GunanimData>();

				int mdata = FileRead_open(filepath, FALSE);
				m_Object.back().first->Set(getparams::Getstr(mdata), EnumSel);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					m_Object.back().second.emplace_back(std::make_shared<GunAnim>());
					m_Object.back().second.back()->Set(getparams::Getstr(mdata));
				}
				FileRead_close(mdata);
			}

			const AnimDatas*	GetAnimData(EnumGunAnim EnumSel) const noexcept {
				auto Find = std::find_if(m_Object.begin(), m_Object.end(), [&](const AnimDatas& tgt) {return tgt.first->GetEnumGunAnim() == EnumSel; });
				if (Find != m_Object.end()) {
					return &*Find;
				}
				return nullptr;
			}

			static GunAnimNow	GetAnimNow(const AnimDatas* data, float nowframe) noexcept {
				GunAnimNow Ret; Ret.Set(VECTOR_ref::zero(), VECTOR_ref::zero());
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
						float Frame = (float)(*ani)->GetFrame();
						if ((nowframe - Frame) <= 0.f) {
							Ret.Set(
								Lerp((*ani)->GetRotate(), (*(ani + 1))->GetRotate(), nowframe / Frame),
								Lerp((*ani)->GetPos(), (*(ani + 1))->GetPos(), nowframe / Frame)
							);
							break;
						}
						nowframe -= Frame;
					}
				}
				return Ret;
			}
		};


		struct GunAnimSet {
			std::array<EnumGunAnim, (int)EnumGunAnimType::Max> Anim;
		};
		const GunAnimSet GunAnimeSets[] = {
			{
				//M4
				EnumGunAnim::M16_aim,
				EnumGunAnim::M16_ads,
				EnumGunAnim::M16_run,
				EnumGunAnim::M16_reloadstart_empty,
				EnumGunAnim::M16_reloadstart,
				EnumGunAnim::M16_reload,
				EnumGunAnim::M16_reloadend,
				EnumGunAnim::M16_ready1,
				EnumGunAnim::M16_check1,
				EnumGunAnim::M16_watch,
				EnumGunAnim::M16_melee,
				EnumGunAnim::M1911_ammoload
			},
			{
				//ハンドガン
				EnumGunAnim::M1911_aim1,
				EnumGunAnim::M1911_ads,
				EnumGunAnim::M1911_run,
				EnumGunAnim::M1911_reloadstart_empty,
				EnumGunAnim::M1911_reloadstart,
				EnumGunAnim::M1911_reload,
				EnumGunAnim::M1911_reloadend,
				EnumGunAnim::M1911_ready1,
				EnumGunAnim::M1911_check1,
				EnumGunAnim::M1911_watch,
				EnumGunAnim::M1911_melee,
				EnumGunAnim::M1911_ammoload
			},
			{
				//M4
				EnumGunAnim::M16_aim,
				EnumGunAnim::M16_ads,
				EnumGunAnim::M16_run,
				EnumGunAnim::M16_reloadstart_empty,
				EnumGunAnim::M16_reloadstart,
				EnumGunAnim::M870_raload,
				EnumGunAnim::M16_reloadend,
				EnumGunAnim::M870_ready,
				EnumGunAnim::M16_check1,
				EnumGunAnim::M16_watch,
				EnumGunAnim::M1911_melee,
				EnumGunAnim::M1911_ammoload
			},
			{
				//M4
				EnumGunAnim::FMG9_aim,
				EnumGunAnim::FMG9_ads,
				EnumGunAnim::M16_run,
				EnumGunAnim::M1911_reloadstart_empty,
				EnumGunAnim::M1911_reloadstart,
				EnumGunAnim::M1911_reload,
				EnumGunAnim::M1911_reloadend,
				EnumGunAnim::M16_ready1,
				EnumGunAnim::M16_check1,
				EnumGunAnim::M16_watch,
				EnumGunAnim::M16_melee,
				EnumGunAnim::M1911_ammoload
			},
		};
	};
};
