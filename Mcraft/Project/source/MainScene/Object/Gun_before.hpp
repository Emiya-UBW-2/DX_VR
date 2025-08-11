#pragma once
#include	"../../Header.hpp"

#include	"FallObj.hpp"
#include	"ModData.hpp"

namespace FPS_n2 {
	namespace Guns {
		//
		class ArmMovePer {
		public:
			float												m_ArmPer{ 0.0f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn)noexcept {
				this->m_Armon = isOn;
				this->m_ArmPer = isOn ? 1.0f : 0.0f;
			}
			void Update(bool isOn, float OnOver = 0.2f, float OffOver = 0.2f, float UpPer = 0.8f, float DownPer = 0.8f) noexcept {
				if (isOn) {
					if (this->m_Armon) {
						Easing(&this->m_ArmPer, 1.0f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 1.0f + OnOver, UpPer, EasingType::OutExpo);
						if (this->m_ArmPer >= 1.0f + OnOver / 2.0f) {
							this->m_Armon = true;
						}
					}
				}
				else {
					if (!this->m_Armon) {
						Easing(&this->m_ArmPer, 0.0f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 0.0f - OffOver, DownPer, EasingType::OutExpo);
						if (this->m_ArmPer <= 0.0f - OffOver / 2.0f) {
							this->m_Armon = false;
						}
					}
				}
			}
		public:
			const auto& Per(void) const noexcept { return this->m_ArmPer; }
		};
		//
		class AutoAimControl {
		private:
			int													m_AutoAim{ InvalidID };
			int													m_AutoAimPoint{ InvalidID };
			float												m_AutoAimTimer{ 0.0f };
			Vector3DX											m_AutoAimVec{};
			bool												m_AutoAimActive{};
			float												m_AutoAimPer{ 0.0f };
		public:
			AutoAimControl(void) noexcept {}
			virtual ~AutoAimControl(void) noexcept {}
		public://ゲッター
			const auto&		GetAutoAimID(void) const noexcept { return this->m_AutoAim; }
			const auto&		GetAutoAimPos(void) const noexcept { return this->m_AutoAimPoint; }
			const auto		GetAutoAimActive(void) const noexcept { return this->m_AutoAimActive; }
		public:
			void CalcAutoAimMat(Matrix3x3DX* ptmp_gunmat) const noexcept {
				*ptmp_gunmat = Lerp(*ptmp_gunmat, (*ptmp_gunmat) * Matrix3x3DX::RotVec2(ptmp_gunmat->zvec2(), this->m_AutoAimVec), this->m_AutoAimPer);
			}
		public:
			void				OverrideAutoAimID(PlayerID ID, int pos) noexcept {
				this->m_AutoAimTimer = 1.0f;
				this->m_AutoAim = ID;
				this->m_AutoAimPoint = pos;
			}
			void Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept;
		};
		//
		class FallControl {
		private:
			std::vector<std::shared_ptr<Objects::FallObj>>	m_Ptr;
			int											m_Now{ 0 };
		public:
			const auto& GetPtrList(void) const noexcept { return this->m_Ptr; }
		public:
			void		Init(const std::string& pPath, int count) {
				this->m_Ptr.resize(count);
				for (auto& ptr : this->m_Ptr) {
					ptr = std::make_shared<Objects::FallObj>();
					ObjectManager::Instance()->InitObject(ptr, pPath);
				}
			}
			void		SetFall(const Vector3DX& pPos, const Matrix3x3DX& pMat, const Vector3DX& pVec, float time, Objects::FallObjectType Type) {
				this->m_Ptr[this->m_Now]->SetFall(pPos, pMat, pVec, time, Type);
				++this->m_Now %= this->m_Ptr.size();
			}
			void		Dispose(void) noexcept {
				for (auto& ptr : this->m_Ptr) {
					ObjectManager::Instance()->DelObj(ptr);
					ptr.reset();
				}
				this->m_Ptr.clear();
			}
		};
		//
		class MuzzleSmokeControl {
			static const int								m_LineTotal{ 16 };
		private:
			std::array<std::pair<Vector3DX,float>, m_LineTotal>		m_Line;
			int														m_LineSelect = 0;
			float													m_LinePer{ 0.0f };
		public://ゲッター
			void			AddMuzzleSmokePower(void) noexcept { this->m_LinePer = std::clamp(this->m_LinePer + 0.1f, 0.0f, 1.0f); }
		public:
			void		InitMuzzleSmoke(const Vector3DX& pPos) {
				for (auto& line : this->m_Line) {
					line.first = pPos;
					line.second = 1.0f;
				}
			}
			void		UpdateMuzzleSmoke(const Vector3DX& pPos, bool IsAddSmoke) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				for (auto& line : this->m_Line) {
					float Per = 0.1f * line.second;
					line.first += Vector3DX::vget(GetRandf(Per), 0.8f + GetRandf(Per), GetRandf(Per)) * Scale3DRate * DXLib_refParts->GetDeltaTime();
					Easing(&line.second, 0.0f, 0.8f, EasingType::OutExpo);
				}
				this->m_Line[this->m_LineSelect].first = pPos;
				this->m_Line[this->m_LineSelect].second = IsAddSmoke ? 1.0f : 0.0f;
				++this->m_LineSelect %= this->m_LineTotal;
				this->m_LinePer = std::clamp(this->m_LinePer - DXLib_refParts->GetDeltaTime() / 30.0f, 0.0f, 1.0f);
			}
			void		DrawMuzzleSmoke(float Caliber) noexcept {
				SetUseLighting(false);
				SetUseHalfLambertLighting(false);
				int min = 1 + static_cast<int>((1.0f - this->m_LinePer) * this->m_LineTotal);
				for (int loop = this->m_LineTotal - 1; loop >= min; --loop) {
					auto p1 = (this->m_LineSelect + loop - 1) % this->m_LineTotal;
					auto p2 = (this->m_LineSelect + loop) % this->m_LineTotal;
					if (this->m_Line[p2].second <= 0.0f) { continue; }
					float Per = static_cast<float>(loop - min) / this->m_LineTotal;
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * Per));
					DrawCapsule_3D(this->m_Line[p1].first.get(), this->m_Line[p2].first.get(), Caliber * Per, GetColor(216, 216, 216), GetColor(96, 96, 64));
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetUseLighting(true);
				SetUseHalfLambertLighting(true);
			}
		};
		//
		class GunObj;
		class GunPartsObj;
		using SharedGunParts = std::shared_ptr<GunPartsObj>;
		//銃、パーツの共通ソケット
		class ModifySlot {
		private:
			std::array<SharedGunParts, static_cast<int>(GunSlot::Max)>	m_PartsObj{ nullptr };//子パーツのポインタ
			int															m_MyDataID{};//自分のスペックを含めたデータ
		public:
			ModifySlot(const std::string& FilePath) noexcept {
				this->m_MyDataID = GunPartsDataManager::Instance()->Add(FilePath);
			}
			virtual ~ModifySlot(void) noexcept {
				for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
					Remove((GunSlot)loop);
				}
			}
		private:
			const bool	IsEffectParts(const SharedGunParts& SlotParts, GunFrame frame) const noexcept;
			void		Add(GunSlot gunSlot, const char* FilePath, const SharedGunParts& NewObj, const SharedObj& BaseModel) noexcept;
		public:
			void		Attach(GunSlot gunSlot, int ID, const SharedObj& BaseModel) noexcept;
			void		Remove(GunSlot gunSlot) noexcept;
		public:
			const auto&	GetMyData(void) const noexcept { return *GunPartsDataManager::Instance()->GetData(this->m_MyDataID); }
			const auto&	GetParts(GunSlot gunSlot) const noexcept { return this->m_PartsObj[static_cast<int>(gunSlot)]; }
			const auto	IsAttachedParts(GunSlot gunSlot) const noexcept { return GetParts(gunSlot) != nullptr; }
			//自分の子孫のパーツを参照する
			void		GetAnyByChild(const std::function<void(const SharedGunParts&)>& Doing) const noexcept;
			//小孫の中で該当のフレームに影響するパーツの行列を得る
			const bool	GetPartsFrameMatChild(GunFrame frame, Matrix4x4DX* pOutMat) const noexcept;
			//パーツのアニメーションを更新する
			void		UpdatePartsAnim(const MV1& pParent);
			//パーツの座標を更新する
			void		UpdatePartsMove(GunSlot gunSlot, const Matrix4x4DX& pMat);
		};
		class GunsModify {
		private:
			struct SlotSaveData {
				GunSlot										SlotType{ GunSlot::Magazine };
				int											Select{ 0 };
				GunSlot										ParentSlotType{ GunSlot::Gun };
				int											Parentselect{ 0 };
			};
			class SlotData {
			private:
			public:
				GunSlot										m_SlotType{ GunSlot::Magazine };
				const SlotData*								m_ParentSlot{ nullptr };
				const std::unique_ptr<ModifySlot>*			m_MySlot{ nullptr };
				int											m_select{ 0 };//セーブへの保持にしか使わん
			public:
				bool IsAttachedParts(void) const noexcept { return (*this->m_MySlot)->IsAttachedParts(this->m_SlotType); }
				const std::unique_ptr<ModifySlot>& GetAttachedPartsSlot(void) const noexcept;
				const auto& GetParentSlot(void) const noexcept { return this->m_ParentSlot; }
			public:
				SlotData() noexcept {}
				virtual ~SlotData(void) noexcept {
					/*
					if (this->m_MySlot && (*this->m_MySlot)) {
						(*this->m_MySlot)->Remove(this->m_SlotType);
					}
					//*/
				}
			public:
				void Init(GunSlot SlotSelect, const SlotData* pParent, const std::unique_ptr<ModifySlot>* pParts) noexcept {
					this->m_SlotType = SlotSelect;
					this->m_ParentSlot = pParent;
					this->m_MySlot = pParts;
				}
				void Set(const SharedObj& BaseModel, int select) noexcept {
					this->m_select = select;
					(*this->m_MySlot)->Attach(this->m_SlotType, select, BaseModel);
				}
			public:
				//持っているデータのセーブデータへの変換
				SlotSaveData GetSaveData(void) const noexcept {
					SlotSaveData Tmp;
					Tmp.SlotType = this->m_SlotType;
					Tmp.Select = this->m_select;
					Tmp.ParentSlotType = (this->m_ParentSlot) ? (this->m_ParentSlot)->m_SlotType : GunSlot::Gun;
					Tmp.Parentselect = (this->m_ParentSlot) ? (this->m_ParentSlot)->m_select : 0;
					return Tmp;
				}
				//自身が該当のスロットと同一かどうか確認
				bool IsSavedSlot(const SlotSaveData& slotSave) const noexcept {
					if (this->m_ParentSlot) {
						if (!(slotSave.ParentSlotType == (this->m_ParentSlot)->m_SlotType) && (slotSave.Parentselect == (this->m_ParentSlot)->m_select)) { return false; }
					}
					else {
						if (slotSave.ParentSlotType != GunSlot::Gun) { return false; }
					}
					return (this->m_SlotType == slotSave.SlotType);
				}
			};
		private:
			std::vector<SlotData>							m_SlotDataPool;
			std::vector<SlotSaveData>						m_SlotSave;
			std::shared_ptr<GunObj>							m_BaseGun{ nullptr };
		public:
			GunsModify(const std::shared_ptr<GunObj>& pBaseGun, bool isPreset) noexcept;
			virtual ~GunsModify(void) noexcept {
				this->m_SlotDataPool.clear();
				this->m_BaseGun.reset();
			}
		public:
			const SlotData* GetSlotData(const GunSlot Slot)const noexcept {
				for (auto& data : this->m_SlotDataPool) {
					if (data.m_SlotType == Slot) {
						return &data;
					}
				}
				return nullptr;
			}
			void	DeleteAllSlot() noexcept {
				for (int loop = 0; loop < static_cast<int>(this->m_SlotDataPool.size()); ++loop) {
					const auto* data = &this->m_SlotDataPool[loop];
					if (DeleteSlotsChildParts(data)) {
						--loop;
					}
				}
				this->m_SlotDataPool.clear();
			}
			void ChangeToSaveData() noexcept;
		private:
			//pBasePartsの子供以降のスロットにデフォルトパーツを設定
			void			SetupDefaultGunParts(const std::unique_ptr<ModifySlot>* pBaseParts, const SlotData* pParentSlot, bool isPreset) noexcept;
			//該当スロット以下のパーツをすべて外す
			bool			DeleteSlotsChildParts(const SlotData* pSlot) noexcept;
		public:
			//該当スロットのパーツをselectの番号のパーツに取り換える(子供以降のスロットはデフォルトパーツ)
			void			ChangeSelectData(const SlotData pSlot, int select) noexcept;
		public:
			//プリセットデータのロード、セーブ
			void			LoadSlots(const char* path) noexcept;
			void			SaveSlots(const char* path) noexcept;
		};
	}
}
