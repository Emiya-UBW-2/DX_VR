#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class EnumWeaponAnim {
			Saber_Ready,
			Saber_Run,
			Saber_Slash1,
			Saber_Slash2,
			Saber_Slash3,
			Max,
		};
		static const char* EnumWeaponAnimName[(int)EnumWeaponAnim::Max] = {
			"Saber_Ready",
			"Saber_Run",
			"Saber_Slash1",
			"Saber_Slash2",
			"Saber_Slash3",
		};

		class WeaponanimData {
			EnumWeaponAnim		m_EnumWeaponAnim{ EnumWeaponAnim::Saber_Ready };
			bool			m_IsLoop{ true };
		public:
			void Set(const std::string& data, EnumWeaponAnim EnumSel) {
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

				m_EnumWeaponAnim = EnumSel;
				m_IsLoop = (Args[0] == "Loop");
			}
		public:
			const auto& GetEnumWeaponAnim() const noexcept { return this->m_EnumWeaponAnim; }
			const auto& GetIsLoop() const noexcept { return this->m_IsLoop; }
		};
		class WeaponAnimNow {
			VECTOR_ref		m_BodyRotate;
			VECTOR_ref		m_Rotate;
			VECTOR_ref		m_Pos;
		public:
			void Set(const VECTOR_ref& BodyRotate, const VECTOR_ref& Rotate, const VECTOR_ref& Pos) {
				m_BodyRotate = BodyRotate;
				m_Rotate = Rotate;
				m_Pos = Pos;
			}
		public:
			const auto& GetBodyRotate() const noexcept { return this->m_BodyRotate; }
			const auto& GetRotate() const noexcept { return this->m_Rotate; }
			const auto& GetPos() const noexcept { return this->m_Pos; }
		};
		class WeaponAnim {
			VECTOR_ref		m_BodyRotate;
			VECTOR_ref		m_Rotate;
			VECTOR_ref		m_Pos;
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
				m_BodyRotate.Set(std::stof(Args[0]), std::stof(Args[1]), std::stof(Args[2]));
				m_Rotate.Set(std::stof(Args[3]), std::stof(Args[4]), std::stof(Args[5]));
				m_Pos.Set(std::stof(Args[6]), std::stof(Args[7]), -std::stof(Args[8]));
				m_Frame = std::stoi(Args[9]);
			}
		public:
			const auto& GetBodyRotate() const noexcept { return this->m_BodyRotate; }
			const auto& GetRotate() const noexcept { return this->m_Rotate; }
			const auto& GetPos() const noexcept { return this->m_Pos; }
			const auto& GetFrame() const noexcept { return this->m_Frame; }
		};

		class WeaponAnimManager : public SingletonBase<WeaponAnimManager> {
		private:
			friend class SingletonBase<WeaponAnimManager>;
			typedef std::pair<std::shared_ptr<WeaponanimData>, std::vector<std::shared_ptr<WeaponAnim>>> DataPair;
		private:
			std::vector<DataPair>	m_Object;
		public:
			void	LoadAction(const char* filepath, EnumWeaponAnim EnumSel) noexcept {
				m_Object.resize(m_Object.size() + 1);
				m_Object.back().first = std::make_shared<WeaponanimData>();

				int mdata = FileRead_open(filepath, FALSE);
				m_Object.back().first->Set(getparams::Getstr(mdata), EnumSel);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					m_Object.back().second.emplace_back(std::make_shared<WeaponAnim>());
					m_Object.back().second.back()->Set(getparams::Getstr(mdata));
				}
				FileRead_close(mdata);
			}

			const DataPair*	GetAnimData(EnumWeaponAnim EnumSel) const noexcept {
				auto Find = std::find_if(m_Object.begin(), m_Object.end(), [&](const DataPair& tgt) {return tgt.first->GetEnumWeaponAnim() == EnumSel; });
				if (Find != m_Object.end()) {
					return &*Find;
				}
				return nullptr;
			}
			static float			GetAnimPer(const DataPair* data, float nowframe) noexcept {
				if (data) {
					int total = 0;
					for (auto& ani : data->second) {
						total += ani->GetFrame();
					}
					return nowframe / (float)total;
				}
				return 0.f;
			}
			static WeaponAnimNow	GetAnimNow(const DataPair* data, float nowframe) noexcept {
				WeaponAnimNow Ret; Ret.Set(VECTOR_ref::zero(), VECTOR_ref::zero(), VECTOR_ref::zero());
				if (data) {
					int total = 0;
					for (auto& ani : data->second) {
						total += ani->GetFrame();
					}
					if (data->first->GetIsLoop()) {
						while (true) {
							if ((nowframe - (float)total) > 0.f) {
								nowframe -= (float)total;
							}
							else {
								break;
							}
						}
					}
					else {
						if ((nowframe - (float)total) > 0.f) {
							nowframe = (float)total;
						}
					}
					for (auto ani = data->second.begin(), e = data->second.end() - 1; ani != e; ++ani) {
						float Frame = (float)(*ani)->GetFrame();
						if ((nowframe - Frame) <= 0.f) {
							Ret.Set(
								Lerp((*ani)->GetBodyRotate(), (*(ani + 1))->GetBodyRotate(), nowframe / Frame),
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

	};
};
