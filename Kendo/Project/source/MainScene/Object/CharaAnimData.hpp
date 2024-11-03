#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

namespace FPS_n2 {
	//アクション
	enum class EnumArmAnimType : uint8_t {
		Ready,
		Run,
		Men,
		Kote,
		Dou,
		Tsuki,
		Tsuba,
		HikiMen,
		HikiKote,
		HikiDou,

		GuardSuriage,
		Max,
	};
	//アニメ
	enum class EnumArmAnim : uint8_t {
		Saber_Ready,
		Saber_Run,
		Saber_Men,
		Saber_Kote,
		Saber_Dou,
		Saber_Tsuki,
		Saber_Tsuba,
		Saber_G_Suriage,
		Saber_G_Left,
		Saber_G_Right,
		Max,
	};
	static const char* EnumArmAnimName[static_cast<int>(EnumArmAnim::Max)] = {
		"Saber_Ready",
		"Saber_Run",
		"Saber_Men",
		"Saber_Kote",
		"Saber_Dou",
		"Saber_Tsuki",
		"Saber_Tsuba",
		"Saber_G_Suriage",
		"Saber_G_Left",
		"Saber_G_Right",
	};
	//
	class ArmAnimManager : public SingletonBase<ArmAnimManager> {
	private:
		friend class SingletonBase<ArmAnimManager>;
	private:
		class ArmanimData {
			EnumArmAnim		m_EnumArmAnim{ EnumArmAnim::Saber_Ready };
			bool			m_IsLoop{ true };
		public:
			void Set(const std::string& data, EnumArmAnim EnumSel) noexcept {
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

				m_EnumArmAnim = EnumSel;
				m_IsLoop = (Args[0] == "Loop");
			}
		public:
			const auto& GetEnumArmAnim(void) const noexcept { return this->m_EnumArmAnim; }
			const auto& GetIsLoop(void) const noexcept { return this->m_IsLoop; }
		};
		class ArmAnimNow {
			Vector3DX		m_BodyRotate{ Vector3DX::zero() };
			Vector3DX		m_Rotate{ Vector3DX::zero() };
			Vector3DX		m_Pos{ Vector3DX::zero() };
			Matrix4x4DX		m_ArmMat{ Matrix4x4DX::identity() };
		public:
			void Set(const Vector3DX& BodyRotate, const Vector3DX& Rotate, const Vector3DX& Pos) noexcept {
				m_BodyRotate = BodyRotate;
				m_Rotate = Rotate;
				m_Pos = Pos;
				m_ArmMat =
					Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(this->m_Rotate.x)) *
					Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(this->m_Rotate.y)) *
					Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(this->m_Rotate.z)) *
					Matrix4x4DX::Mtrans(this->m_Pos * Scale_Rate);
			}
		public:
			//const auto& GetBodyRotate(void) const noexcept { return this->m_BodyRotate; }
			const auto& GetMatrix(void) const noexcept { return m_ArmMat; }
		};
		class ArmAnim {
			Vector3DX		m_BodyRotate;
			Vector3DX		m_Rotate;
			Vector3DX		m_Pos;
			int				m_Frame{ 1 };
		public:
			void Set(const std::string& data) noexcept {
				std::vector<std::string> Args;
				std::string RIGHTBuf = data;
				//タブけし
				while (true) {
					auto RIGHTBufLen = RIGHTBuf.find(" ");
					if (RIGHTBufLen != std::string::npos) {
						RIGHTBuf = RIGHTBuf.substr(0, RIGHTBufLen) + RIGHTBuf.substr(RIGHTBufLen + 1);
					}
					else {
						break;
					}
				}
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
				m_BodyRotate.Set(std::stof(Args[0]), std::stof(Args[1]), std::stof(Args[2]));
				m_Rotate.Set(std::stof(Args[3]), std::stof(Args[4]), std::stof(Args[5]));
				m_Pos.Set(std::stof(Args[6]), std::stof(Args[7]), -std::stof(Args[8]));
				m_Frame = std::stoi(Args[9]);
			}
		public:
			const auto& GetBodyRotate(void) const noexcept { return this->m_BodyRotate; }
			const auto& GetRotate(void) const noexcept { return this->m_Rotate; }
			const auto& GetPos(void) const noexcept { return this->m_Pos; }
			const auto& GetFrame(void) const noexcept { return this->m_Frame; }
		};

		struct AnimDatas {
		public:
		public:
			std::shared_ptr<ArmanimData> first;
			std::vector<std::shared_ptr<ArmAnim>> second;

			auto	GetTotalTime() const {
				int total = 0;
				for (auto& ani : second) {
					total += ani->GetFrame();
				}
				return total;
			}
		};
	private:
		std::vector<AnimDatas>	m_Object;

#ifdef DEBUG
		Vector3DX DBG_AnimRot;
		Vector3DX DBG_AnimPos;

#endif
	private:
		ArmAnimManager(void) noexcept {}
		ArmAnimManager(const ArmAnimManager&) = delete;
		ArmAnimManager(ArmAnimManager&& o) = delete;
		ArmAnimManager& operator=(const ArmAnimManager&) = delete;
		ArmAnimManager& operator=(ArmAnimManager&& o) = delete;

		virtual ~ArmAnimManager(void) noexcept {}
	public:
		void	Load(const char* filepath) noexcept {
			for (int loop = 0; loop < static_cast<int>(EnumArmAnim::Max); ++loop) {
				std::string Path = filepath;
				Path += EnumArmAnimName[loop];
				Path += ".txt";
				LoadAction(Path.c_str(), (EnumArmAnim)loop);
			}
		}
		void	LoadAction(const char* filepath, EnumArmAnim EnumSel) noexcept {
			m_Object.resize(m_Object.size() + 1);
			m_Object.back().first = std::make_shared<ArmanimData>();

			int mdata = FileRead_open(filepath, FALSE);
			m_Object.back().first->Set(getparams::Getstr(mdata), EnumSel);
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				m_Object.back().second.emplace_back(std::make_shared<ArmAnim>());
				m_Object.back().second.back()->Set(getparams::Getstr(mdata));
			}
			FileRead_close(mdata);
		}

		const AnimDatas* GetAnimData(EnumArmAnim EnumSel) const noexcept {
			auto Find = std::find_if(m_Object.begin(), m_Object.end(), [&](const AnimDatas& tgt) {return tgt.first->GetEnumArmAnim() == EnumSel; });
			if (Find != m_Object.end()) {
				return &*Find;
			}
			return nullptr;
		}

		ArmAnimNow	GetAnimNow(const AnimDatas* data, float nowframe) noexcept {
			ArmAnimNow Ret;
			if (data) {
				float totalTime = static_cast<float>(data->GetTotalTime());
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
							Vector3DX::zero(),
							Lerp((*ani)->GetRotate(), (*(ani + 1))->GetRotate(), nowframe / Frame),
							Lerp((*ani)->GetPos(), (*(ani + 1))->GetPos(), nowframe / Frame)
						);
#ifdef DEBUG
						if (0 <= DBG_CamSel && DBG_CamSel <= 2) {
							Ret.Set(Vector3DX::zero(), DBG_AnimRot, DBG_AnimPos);
							auto* DrawParts = DXDraw::Instance();
							//
							if (CheckHitKeyWithCheck(KEY_INPUT_RCONTROL) != 0) {
								if (CheckHitKeyWithCheck(KEY_INPUT_J) != 0) {
									DBG_AnimRot.x += 5.f * DrawParts->GetDeltaTime();
								}
								if (CheckHitKeyWithCheck(KEY_INPUT_L) != 0) {
									DBG_AnimRot.x -= 5.f * DrawParts->GetDeltaTime();
								}
								//
								if (CheckHitKeyWithCheck(KEY_INPUT_I) != 0) {
									DBG_AnimRot.y += 5.f * DrawParts->GetDeltaTime();
								}
								if (CheckHitKeyWithCheck(KEY_INPUT_K) != 0) {
									DBG_AnimRot.y -= 5.f * DrawParts->GetDeltaTime();
								}
								//
								if (CheckHitKeyWithCheck(KEY_INPUT_U) != 0) {
									DBG_AnimRot.z += 5.f * DrawParts->GetDeltaTime();
								}
								if (CheckHitKeyWithCheck(KEY_INPUT_O) != 0) {
									DBG_AnimRot.z -= 5.f * DrawParts->GetDeltaTime();
								}
							}
							else {
								if (CheckHitKeyWithCheck(KEY_INPUT_J) != 0) {
									DBG_AnimPos.x += 0.01f * DrawParts->GetDeltaTime();
								}
								if (CheckHitKeyWithCheck(KEY_INPUT_L) != 0) {
									DBG_AnimPos.x -= 0.01f * DrawParts->GetDeltaTime();
								}
								//
								if (CheckHitKeyWithCheck(KEY_INPUT_I) != 0) {
									DBG_AnimPos.y += 0.01f * DrawParts->GetDeltaTime();
								}
								if (CheckHitKeyWithCheck(KEY_INPUT_K) != 0) {
									DBG_AnimPos.y -= 0.01f * DrawParts->GetDeltaTime();
								}
								//
								if (CheckHitKeyWithCheck(KEY_INPUT_U) != 0) {
									DBG_AnimPos.z += 0.01f * DrawParts->GetDeltaTime();
								}
								if (CheckHitKeyWithCheck(KEY_INPUT_O) != 0) {
									DBG_AnimPos.z -= 0.01f * DrawParts->GetDeltaTime();
								}
							}
							printfDx("Rot[%5.2f,%5.2f,%5.2f]\n", DBG_AnimRot.x, DBG_AnimRot.y, DBG_AnimRot.z);
							printfDx("Pos[%5.2f,%5.2f,%5.2f]\n", DBG_AnimPos.x, DBG_AnimPos.y, -DBG_AnimPos.z);
						}
						else {
							DBG_AnimRot.Set(0.0f, -80.0f, 0.0f);
							DBG_AnimPos.Set(-0.02f, -0.35f, -0.41f);
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


	using ArmAnimSet = std::array<EnumArmAnim, static_cast<int>(EnumArmAnimType::Max)>;
	const ArmAnimSet ArmAnimeSets[] = {
		{
			EnumArmAnim::Saber_Ready,
			EnumArmAnim::Saber_Run,
			EnumArmAnim::Saber_Men,
			EnumArmAnim::Saber_Kote,
			EnumArmAnim::Saber_Dou,
			EnumArmAnim::Saber_Tsuki,
			EnumArmAnim::Saber_Tsuba,
			EnumArmAnim::Saber_Men,
			EnumArmAnim::Saber_Kote,
			EnumArmAnim::Saber_Dou,
			EnumArmAnim::Saber_G_Suriage,
		},
	};
};
