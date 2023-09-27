#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ObjectBaseClass::LoadModel(PHYSICS_SETUP TYPE, bool UseToonWhenCreateFile, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
			this->m_PHYSICS_SETUP = TYPE;
			this->m_FilePath = filepath;
			this->m_ObjFileName = objfilename;
			this->m_ColFileName = colfilename;
			FILEINFO FileInfo;
			//model
			{
				std::string Path = this->m_FilePath;
				Path += this->m_ObjFileName;

				switch (this->m_PHYSICS_SETUP) {
				case FPS_n2::Sceneclass::PHYSICS_SETUP::DISABLE:
					if (FileRead_findFirst((Path + "_DISABLE.mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						//MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_DISABLE);
						MV1::Load((Path + "_DISABLE.mv1").c_str(), &this->m_obj, DX_LOADMODEL_PHYSICS_DISABLE);
					}
					else {
						MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_DISABLE);
						if (!UseToonWhenCreateFile) {
							MV1SetMaterialTypeAll(this->m_obj.get(), DX_MATERIAL_TYPE_NORMAL);
						}
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
						MV1SaveModelToMV1File(this->m_obj.get(), (Path + "_DISABLE.mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					break;
				case FPS_n2::Sceneclass::PHYSICS_SETUP::LOADCALC:
					if (FileRead_findFirst((Path + "_LOADCALC.mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						//MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_LOADCALC);
						MV1::Load((Path + "_LOADCALC.mv1").c_str(), &this->m_obj, DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					else {
						MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_LOADCALC);
						if (!UseToonWhenCreateFile) {
							MV1SetMaterialTypeAll(this->m_obj.get(), DX_MATERIAL_TYPE_NORMAL);
						}
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
						MV1SaveModelToMV1File(this->m_obj.get(), (Path + "_LOADCALC.mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					break;
				case FPS_n2::Sceneclass::PHYSICS_SETUP::REALTIME:
					if (FileRead_findFirst((Path + "_REALTIME.mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						//MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_REALTIME);
						MV1::Load((Path + "_REALTIME.mv1").c_str(), &this->m_obj, DX_LOADMODEL_PHYSICS_REALTIME);
					}
					else {
						MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_REALTIME);
						if (!UseToonWhenCreateFile) {
							MV1SetMaterialTypeAll(this->m_obj.get(), DX_MATERIAL_TYPE_NORMAL);
						}
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
						MV1SaveModelToMV1File(this->m_obj.get(), (Path + "_REALTIME.mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					break;
				default:
					break;
				}
				MV1::SetAnime(&this->m_obj, m_obj);
			}
			//col
			{
				std::string Path = this->m_FilePath;
				Path += this->m_ColFileName;
				if (FileRead_findFirst((Path + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					MV1::Load(Path + ".pmx", &this->m_col, DX_LOADMODEL_PHYSICS_DISABLE);
					//MV1::Load((Path + ".mv1").c_str(), &this->m_col, DX_LOADMODEL_PHYSICS_DISABLE);
				}
				else {
					if (FileRead_findFirst((Path + ".pmx").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						MV1::Load(Path + ".pmx", &this->m_col, DX_LOADMODEL_PHYSICS_DISABLE);
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
						MV1SaveModelToMV1File(this->m_col.get(), (Path + ".mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
				}
				if (this->m_col.IsActive()) {
					for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
				}

			}
			this->m_IsBaseModel = true;
			this->m_objActive = true;
		}
		void			ObjectBaseClass::CopyModel(const std::shared_ptr<ObjectBaseClass>& pBase) noexcept {
			this->m_PHYSICS_SETUP = pBase->m_PHYSICS_SETUP;
			this->m_FilePath = pBase->m_FilePath;
			this->m_ObjFileName = pBase->m_ObjFileName;
			this->m_ColFileName = pBase->m_ColFileName;
			this->m_obj = pBase->m_obj.Duplicate();
			MV1::SetAnime(&this->m_obj, pBase->m_obj);
			//col
			if (pBase->m_col.IsActive()) {
				this->m_col = pBase->m_col.Duplicate();
				if (this->m_col.IsActive()) {
					for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
				}
			}
			this->m_IsBaseModel = false;
			this->m_objActive = true;
		}
		//
		void			ObjectBaseClass::Init(void) noexcept {
			this->m_IsActive = true;
			this->m_IsResetPhysics = true;
			this->m_IsFirstLoop = true;
			this->m_IsDraw = false;
		}
		//
		void			ObjectBaseClass::SetFrameNum(void) noexcept {
			if (this->m_objActive) {
				int i = 0;
				bool isEnd = false;
				auto fNum = this->GetObj().frame_num();
				for (int f = 0; f < fNum; f++) {
					std::string FName = this->GetObj().frame_name(f);
					bool compare = false;
					switch (this->m_objType) {
					case ObjType::Human:
						compare = (FName == CharaFrameName[i]);
						if (!compare) {
							//compare = (FName.find(CharaFrameName[i]) != std::string::npos);
						}
						break;
					case ObjType::Gun:
					case ObjType::Magazine:
					case ObjType::Lower:
					case ObjType::Upper:
					case ObjType::Barrel:
						compare = (FName == GunFrameName[i]);
						if (!compare) {
							compare = (FName.find(GunFrameName[i]) != std::string::npos);
						}
						break;
					default:
						break;
					}

					if (compare) {
						this->m_Frames.resize(this->m_Frames.size() + 1);
						this->m_Frames.back().first = f;
						this->m_Frames.back().second = MATRIX_ref::Mtrans(this->GetObj().GetFrameLocalMatrix(this->m_Frames.back().first).pos());
						i++;
						f = 0;
					}
					switch (this->m_objType) {
					case ObjType::Human:
						if (i >= (int)CharaFrame::Max) { isEnd = true; }
						break;
					case ObjType::Gun:
					case ObjType::Magazine:
					case ObjType::Lower:
					case ObjType::Upper:
					case ObjType::Barrel:
						if (i >= (int)GunFrame::Max) { isEnd = true; }
						break;
					default:
						isEnd = true;
						break;
					}
					if (f == fNum - 1) {
						if (!isEnd) {
							this->m_Frames.resize(this->m_Frames.size() + 1);
							this->m_Frames.back().first = -1;
							i++;
							f = 0;
						}
					}
					switch (this->m_objType) {
					case ObjType::Human:
						if (i >= (int)CharaFrame::Max) { isEnd = true; }
						break;
					case ObjType::Gun:
					case ObjType::Magazine:
					case ObjType::Lower:
					case ObjType::Upper:
					case ObjType::Barrel:
						if (i >= (int)GunFrame::Max) { isEnd = true; }
						break;
					default:
						isEnd = true;
						break;
					}
					if (isEnd) {
						break;
					}
				}
				//シェイプ
				switch (this->m_objType) {
				case ObjType::Human:
					this->m_Shapes.resize((int)CharaShape::Max);
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						auto s = MV1SearchShape(this->GetObj().get(), CharaShapeName[j]);
						if (s >= 0) {
							this->m_Shapes[j].first = s;
							this->m_Shapes[j].second = 0.f;
						}
					}
					break;
				default:
					break;
				}
			}
		}
		void			ObjectBaseClass::ExecuteCommon(void) noexcept {
			if (this->m_IsFirstLoop) {
				this->m_PrevMat = this->GetObj().GetMatrix();
			}
			//シェイプ更新
			switch (this->m_objType) {
			case ObjType::Human:
				for (int j = 1; j < (int)CharaShape::Max; j++) {
					MV1SetShapeRate(this->GetObj().get(), m_Shapes[j].first, (1.f - this->m_Shapes[0].second)*this->m_Shapes[j].second);
				}
				break;
			default:
				break;
			}
			//物理更新
			if (this->m_PHYSICS_SETUP == PHYSICS_SETUP::REALTIME) {
				if (this->m_IsResetPhysics) {
					this->m_IsResetPhysics = false;
					this->GetObj().PhysicsResetState();
				}
				else {
					auto NowMat = this->GetObj().GetMatrix();
					int Max = 2;
					for (int i = 0; i < Max; i++) {
						this->GetObj().SetMatrix(
							Lerp_Matrix(this->m_PrevMat.GetRot(), NowMat.GetRot(), (float)(i + 1) / (float)Max)
							* MATRIX_ref::Mtrans(Lerp(this->m_PrevMat.pos(), NowMat.pos(), (float)(i + 1) / (float)Max)));
						this->GetObj().PhysicsCalculation(1000.0f / FPS * 60.f);
					}
				}
			}
			//最初のループ終わり
			this->m_IsFirstLoop = false;
		}

		void			ObjectBaseClass::DrawShadow(void) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				this->GetObj().DrawModel();
			}
		}
		void			ObjectBaseClass::CheckDraw(void) noexcept {
			this->m_IsDraw = false;
			this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetCameraPosition()).size();
			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-1.f*Scale_Rate, -0.f*Scale_Rate, -1.f*Scale_Rate)).get(),
				(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(1.f*Scale_Rate, 1.f*Scale_Rate, 1.f*Scale_Rate)).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}
		}
		void			ObjectBaseClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-1.f*Scale_Rate, -0.f*Scale_Rate, -1.f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(1.f*Scale_Rate, 1.f*Scale_Rate, 1.f*Scale_Rate)).get()) == FALSE
					) {
					for (int i = 0; i < this->GetObj().mesh_num(); i++) {
						if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
		//
		void			ObjectBaseClass::Dispose(void) noexcept {
			this->GetObj().Dispose();
			this->m_col.Dispose();
		}
	};
};
