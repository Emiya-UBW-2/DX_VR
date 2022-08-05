#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemClass : public ObjectBaseClass {
			bool													m_Used{ false };
			bool													m_Have{ false };
			bool													m_Hand{ false };
			GraphHandle												m_ItemGraph;
			ItemType												m_type{ ItemType::YellowPotion };
		public:
			auto& GetItemGraph() const noexcept { return m_ItemGraph; }
			const auto& GetItemType() const noexcept { return m_type; }

			void SetCharaPtr(bool value) noexcept { m_Have = value; }
			const auto GetHaveChara() const noexcept { return m_Have; }

			void SetUsed(bool value) noexcept { m_Used = value; }
			const auto GetUsed() const noexcept { return m_Used; }
		public:
			ItemClass(void) noexcept { this->m_objType = ObjType::Item; }
			~ItemClass(void) noexcept {}
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_move.vec.clear();
				this->m_Hand = false;
				this->m_Have = false;
				this->m_Used = false;
				this->m_ItemGraph = GraphHandle::Load(this->m_FilePath + "pic.png");
				//todo:ƒAƒCƒeƒ€Ý’è
				if (this->m_FilePath.find("YellowPotion") != std::string::npos) {
					this->m_type = ItemType::YellowPotion;
				}
				else if (this->m_FilePath.find("BluePotion") != std::string::npos) {
					this->m_type = ItemType::BluePotion;
				}
				else if (this->m_FilePath.find("RedPotion") != std::string::npos) {
					this->m_type = ItemType::RedPotion;
				}
			}
			void Execute(void) noexcept override {
				if (!this->m_Have) {
					this->m_IsActive = true;
					auto HitResult = this->m_MapCol->CollCheck_Line(this->m_move.pos + VECTOR_ref::up() * (-1.f - 3.f), this->m_move.pos + VECTOR_ref::up() * 15.f);
					if (HitResult.HitFlag == TRUE) {
						auto yPos = this->m_move.pos.y();
						Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
						this->m_move.pos.y(yPos);
						this->m_move.vec.y(0.f);
					}
					else {
						this->m_move.vec.yadd(M_GR / (FPS * FPS));
					}
					this->m_move.pos += this->m_move.vec;

					UpdateMove();
				}
				else if (!this->m_Hand) {
					this->m_IsActive = false;
				}
				this->m_Hand = false;
			}
			void SetHand(const MATRIX_ref& mat, const VECTOR_ref& pos) noexcept {
				this->m_Hand = true;
				this->m_IsActive = true;
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				UpdateMove();
			}
		};
	};
};
