#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class PHYSICS_SETUP {
			DISABLE,
			LOADCALC,
			REALTIME,
		};

		class ObjectBaseClass {
		protected:
			PHYSICS_SETUP								m_PHYSICS_SETUP{PHYSICS_SETUP::DISABLE};
			bool										m_ColActive{false};
			MV1											m_obj;
			MV1											m_col;
			moves										m_move;
			Matrix4x4DX									m_PrevMat;//�����X�V�̂���
			std::vector<std::pair<int, Matrix4x4DX>>	m_Frames;
			std::vector<std::pair<int, float>>			m_Shapes;
			ObjType										m_objType{(ObjType)0};
			std::string									m_FilePath;
			std::string									m_ObjFileName;
			std::string									m_ColFileName;
			bool										m_IsActive{true};
			bool										m_IsDelete{false};
			bool										m_IsResetPhysics{true};
			bool										m_IsFirstLoop{true};
			bool										m_IsDraw{true};
			float										m_DistanceToCam{0.f};
			bool										m_IsBaseModel{false};
			Vector3DX									m_ScreenPosition;
			float										m_CameraSize{0.f};
			PlayerID									m_MyID{0};
			ShaderUseClass*								m_UseShader{nullptr};
			std::array<int, 2>							m_ShaderTex{-1 , -1};
		public:
			auto&			GetObj(void) noexcept { return this->m_obj; }
			const auto&		GetObj_const(void) const noexcept { return this->m_obj; }
			const auto		GetMatrix(void) const noexcept { return this->m_obj.GetMatrix(); }
			const auto		GetFrameLocalMatrix(int frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(frame); }
			const auto		GetFrameWorldMatrix(int frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(frame); }
			const auto		GetParentFrameWorldMatrix(int frame) const noexcept { return GetFrameWorldMatrix((int)this->m_obj.frame_parent(frame)); }
			const auto		GetChildFrameNum(int frame) const noexcept { return (int)this->m_obj.frame_child_num(frame); }
			const auto		GetChildFrameWorldMatrix(int frame, int ID) const noexcept { return GetFrameWorldMatrix((int)this->m_obj.frame_child(frame, ID)); }

			const bool		HaveFrame(int frame) const noexcept { return this->m_Frames[frame].first != -1; }
			const auto&		GetFrame(int frame) const noexcept { return this->m_Frames[frame].first; }
			const auto&		GetFrameBaseLocalMat(int frame) const noexcept { return this->m_Frames[frame].second; }

		public:
			const auto&		GetIsBaseModel(void) const noexcept { return this->m_IsBaseModel; }
			const auto		GetPathCompare(const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
				return ((this->m_FilePath == filepath) && (this->m_ObjFileName == objfilename) && (this->m_ColFileName == colfilename));
			}
			const auto&		GetobjType(void) const noexcept { return this->m_objType; }
			const auto&		GetScreenPosition(void) const noexcept { return this->m_ScreenPosition; }
			const auto&		GetCameraSize(void) const noexcept { return this->m_CameraSize; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		IsActive(void) const noexcept { return this->m_IsActive; }
			const auto&		GetIsDelete(void) const noexcept { return this->m_IsDelete; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&		GetFilePath(void) const noexcept { return this->m_FilePath; }
		public:
			void			SetShapePer(int pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetActive(bool value) noexcept { this->m_IsActive = value; }
			void			SetDelete() noexcept { this->m_IsDelete = true; }
			void			SetResetP(bool value) { this->m_IsResetPhysics = value; }
			void			SetScreenPosition(const Vector3DX& value, float size) {
				this->m_ScreenPosition = value;
				this->m_CameraSize = size;
			}
			void			SetUseShader(ShaderUseClass* value) noexcept { this->m_UseShader = value; }
			void			SetShaderTexHandle(int id, int value) noexcept { this->m_ShaderTex[id] = value; }
			void			SetAnimOnce(int ID, float speed) {
				this->GetObj().get_anime(ID).time += 30.f / FPS * speed;
				if (this->GetObj().get_anime(ID).TimeEnd()) { this->GetObj().get_anime(ID).GoEnd(); }
			}
			void			SetAnimLoop(int ID, float speed) {
				this->GetObj().get_anime(ID).time += 30.f / FPS * speed;
				if (this->GetObj().get_anime(ID).TimeEnd()) { this->GetObj().get_anime(ID).GoStart(); }
			}
			void			ResetMove(const Matrix4x4DX& mat, const Vector3DX& pos) noexcept {
				this->m_move.vec.Set(0, 0, 0);
				SetMove(mat, pos);
			}
			void			SetMove(const moves& movs) noexcept {
				this->m_move = movs;
				UpdateMove();
			}
			void			SetMove(const Matrix4x4DX& mat, const Vector3DX& pos) noexcept {
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				this->m_move.posbuf = pos;
				UpdateMove();
			}
			void			SetMove(const Matrix4x4DX& mat, const Vector3DX& pos, const Vector3DX& vec) {
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				this->m_move.posbuf = pos;
				this->m_move.vec = vec;
				UpdateMove();
			}

			void			UpdateMove(void) noexcept {
				this->m_PrevMat = this->GetObj().GetMatrix();
				this->GetObj().SetMatrix(this->m_move.MatIn());
				if (this->m_col.IsActive()) {
					this->m_col.SetMatrix(this->m_move.MatIn());
					//this->m_col.RefreshCollInfo();
				}
			}
			//����N��
			const auto		RefreshCol(const Vector3DX& StartPos, const Vector3DX& EndPos, float pRange) noexcept {
				if (this->m_ColActive) { return true; }				//���łɋN�����Ă���Ȃ疳��
				if (GetMinLenSegmentToPoint(StartPos, EndPos, m_move.pos) <= pRange) {
					//����N��
					this->m_ColActive = true;
					for (int i = 0; i < this->m_col.mesh_num(); i++) {
						this->m_col.RefreshCollInfo(-1, i);
					}
					return true;
				}
				return false;
			}
			//����擾
			const auto		GetColCapsule(const Vector3DX& StartPos, const Vector3DX& EndPos, float range, const int sel = 0) const noexcept { return this->m_col.CollCheck_Capsule(StartPos, EndPos, range, -1, sel); }
			const auto		GetColLine(const Vector3DX& StartPos, const Vector3DX& EndPos, const int sel = 0) const noexcept { return this->m_col.CollCheck_Line(StartPos, EndPos, -1, sel); }
			void			GetColNearestInAllMesh(const Vector3DX& StartPos, Vector3DX* EndPos) const noexcept {
				MV1_COLL_RESULT_POLY colres;
				for (int i = 0; i < this->m_col.mesh_num(); ++i) {
					colres = GetColLine(StartPos, *EndPos, i);
					if (colres.HitFlag == TRUE) {
						*EndPos = colres.HitPosition;
					}
				}
			}
		public:
			void			LoadModel(PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
			void			CopyModel(const std::shared_ptr<ObjectBaseClass>& pBase) noexcept;
			void			SaveModel(bool UseToonWhenCreateFile) noexcept;
		public:
			virtual void	Init(void) noexcept;
			virtual void	FirstExecute(void) noexcept {}
			void			ExecuteCommon(void) noexcept;
			virtual void	LateExecute(void) noexcept {}
			virtual void	Depth_Draw(void) noexcept {}
			virtual void	DrawShadow(void) noexcept;
			virtual void	CheckDraw(void) noexcept;
			virtual void	Draw(bool isDrawSemiTrans) noexcept;
			//
			virtual void	Dispose(void) noexcept;
		};
	};
};
