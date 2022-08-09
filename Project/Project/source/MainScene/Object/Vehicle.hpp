#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		static auto* CreateB2Body(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) noexcept {
			b2BodyDef f_bodyDef;
			f_bodyDef.type = type;
			f_bodyDef.position.Set(x_, y_);
			f_bodyDef.angle = angle;
			return world->CreateBody(&f_bodyDef);
		}
		static const size_t max_bullet{ 64 };
		//インスタシング
		class Model_Instance {
		public:
			int hitss = 0;					/*hitsの数*/
			std::vector<VERTEX3D> hitsver;	/*hits*/
			std::vector<DWORD> hitsind;	    /*hits*/
			int VerBuf = -1, IndexBuf = -1;	/*hits*/
			MV1 hits;						/*hitsモデル*/
			GraphHandle hits_pic;			/*画像ハンドル*/
			int IndexNum = -1, VerNum = -1;	/*hits*/
			int vnum = -1, pnum = -1;		/*hits*/
			MV1_REF_POLYGONLIST RefMesh{};	/*hits*/
			//初期化
			void Init(std::string pngpath, std::string mv1path) noexcept {
				SetUseASyncLoadFlag(FALSE);
				this->hits_pic = GraphHandle::Load(pngpath);		 /*grass*/
				MV1::Load(mv1path, &this->hits);	//弾痕
				Init_one();
			}
			void Init_one(void) noexcept {
				MV1RefreshReferenceMesh(this->hits.get(), -1, TRUE);			/*参照用メッシュの更新*/
				this->RefMesh = MV1GetReferenceMesh(this->hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
			}
			//毎回のリセット
			void Clear(void) noexcept {
				this->hitss = 0;
				this->vnum = 0;
				this->pnum = 0;
				this->hitsver.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsind.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsver.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsind.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			}

			void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
				this->hitss++;
				Set_start();
				{
					float asize = 200.f * caliber;
					const auto& y_vec = Normal;
					auto z_vec = y_vec.cross(Zvec).Norm();
					auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
					auto pos = Position + y_vec * 0.02f;
					MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec);

					this->hits.SetMatrix(mat * MATRIX_ref::Mtrans(pos));
				}
				Set_one();
			}
			void Set_start(void) noexcept {
				this->IndexNum = this->RefMesh.PolygonNum * 3 * this->hitss;				/*インデックスの数を取得*/
				this->VerNum = this->RefMesh.VertexNum * this->hitss;						/*頂点の数を取得*/
				this->hitsver.resize(this->VerNum);									/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsind.resize(this->IndexNum);								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			}
			void Set_one(void) noexcept {
				Init_one();
				for (size_t j = 0; j < size_t(this->RefMesh.VertexNum); ++j) {
					auto& g = this->hitsver[j + this->vnum];
					g.pos = this->RefMesh.Vertexs[j].Position;
					g.norm = this->RefMesh.Vertexs[j].Normal;
					g.dif = this->RefMesh.Vertexs[j].DiffuseColor;
					g.spc = this->RefMesh.Vertexs[j].SpecularColor;
					g.u = this->RefMesh.Vertexs[j].TexCoord[0].u;
					g.v = this->RefMesh.Vertexs[j].TexCoord[0].v;
					g.su = this->RefMesh.Vertexs[j].TexCoord[1].u;
					g.sv = this->RefMesh.Vertexs[j].TexCoord[1].v;
				}
				for (size_t j = 0; j < size_t(this->RefMesh.PolygonNum); ++j) {
					for (size_t k = 0; k < std::size(this->RefMesh.Polygons[j].VIndex); ++k)
						this->hitsind[j * 3 + k + this->pnum] = WORD(this->RefMesh.Polygons[j].VIndex[k] + this->vnum);
				}
				this->vnum += this->RefMesh.VertexNum;
				this->pnum += this->RefMesh.PolygonNum * 3;
			}

			void Update(void) noexcept {
				this->VerBuf = CreateVertexBuffer(this->VerNum, DX_VERTEX_TYPE_NORMAL_3D);
				this->IndexBuf = CreateIndexBuffer(this->IndexNum, DX_INDEX_TYPE_32BIT);
				SetVertexBufferData(0, this->hitsver.data(), this->VerNum, this->VerBuf);
				SetIndexBufferData(0, this->hitsind.data(), this->IndexNum, this->IndexBuf);
			}
			void Draw(void) noexcept {
				//SetDrawAlphaTest(DX_CMP_GREATER, 128);
				{
					DrawPolygonIndexed3D_UseVertexBuffer(this->VerBuf, this->IndexBuf, this->hits_pic.get(), TRUE);
				}
				//SetDrawAlphaTest(-1, 0);
			}
		};
		//命中根
		class HIT_PASSIVE {
			Model_Instance inst;
			bool isUpdate{ true };
		public:
			//初期化
			void Init(void) noexcept {
				inst.Init("data/m_obj/hit/hit.png", "data/m_obj/hit/m_obj.mv1");
			}
			//毎回のリセット
			void Clear(void) noexcept {
				inst.Clear();
			}

			void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
				inst.Set(caliber, Position, Normal, Zvec);
				isUpdate = true;
			}
			void Update(void) noexcept {
				if (isUpdate) {
					isUpdate = false;
					inst.Update();
				}
			}
			void Draw(void) noexcept {
				inst.Draw();
			}
		};
		HIT_PASSIVE hit_obj_p;												//静的弾痕
		//
		class HIT_ACTIVE {
			struct Hit {		      /**/
				bool Flag{ false };   /*弾痕フラグ*/
				int use{ 0 };	      /*使用フレーム*/
				MV1 pic;	      /*弾痕モデル*/
				VECTOR_ref pos;	      /*座標*/
				MATRIX_ref mat;	      /**/
			};								      /**/
			std::array<Hit, 24> hit_obj;					      /*弾痕*/
			int hitbuf = 0;		       /*使用弾痕*/
		public:
			void Set(const MV1& hit_pic) {
				for (auto& h : this->hit_obj) {
					h.Flag = false;
					h.pic = hit_pic.Duplicate();
					h.use = 0;
					h.mat = MGetIdent();
					h.pos = VGet(0.f, 0.f, 0.f);
				}
			}
			void Set(const moves& this_move, const  VECTOR_ref& Put_pos, const  VECTOR_ref& Put_normal, const VECTOR_ref& ammo_nomal/*this->move.vec.Norm()*/, const float&caliber, bool isPene) {
				float asize = 200.f*caliber;
				auto y_vec = MATRIX_ref::Vtrans(Put_normal, this_move.mat.Inverse());
				auto z_vec = MATRIX_ref::Vtrans(Put_normal.cross(ammo_nomal).Norm(), this_move.mat.Inverse());
				auto scale = VECTOR_ref::vget(asize / std::abs(ammo_nomal.dot(Put_normal)), asize, asize);

				this->hit_obj[this->hitbuf].use = (isPene) ? 0 : 1;				//弾痕
				this->hit_obj[this->hitbuf].mat = MATRIX_ref::GetScale(scale)* MATRIX_ref::Axis1_YZ(y_vec, z_vec);
				this->hit_obj[this->hitbuf].pos = MATRIX_ref::Vtrans(Put_pos - this_move.pos, this_move.mat.Inverse()) + y_vec * 0.005f;
				this->hit_obj[this->hitbuf].Flag = true;
				++this->hitbuf %= this->hit_obj.size();
			}
			void Update(const moves& this_move) {
				//弾痕
				for (auto& h : this->hit_obj) {
					if (h.Flag) {
						h.pic.SetMatrix(h.mat* this_move.mat*MATRIX_ref::Mtrans(this_move.pos + MATRIX_ref::Vtrans(h.pos, this_move.mat)));
					}
				}
			}
			void Draw() {
				//弾痕
				for (auto& h : this->hit_obj) {
					if (h.Flag) {
						h.pic.DrawFrame(h.use);
					}
				}
			}
			void Dispose() {
				for (auto& h : this->hit_obj) {
					h.Flag = false;
					h.use = 0;
					h.pic.Dispose();
					h.pos = VGet(0, 0, 0);
					h.mat = MGetIdent();
				}
			}
		};
		//弾データ
		class AmmoData {
		private:
			std::string name;
			std::string path;
			MV1 m_obj;
			MV1 m_obj_full;//未発射
			//
			float caliber{ 0.f };
			float speed = 100.f;//弾速
			float pene = 10.f;//貫通
			int damage = 10;//ダメージ
		public:
			void Set_name(const std::string& value) noexcept { name = value; }

			const MV1& Get_model(void) const noexcept { return m_obj; }

			const auto& Get_model_full(void) const noexcept { return m_obj_full; }
			const float& Get_caliber(void) const noexcept { return caliber; }
			const float& Get_speed(void) const noexcept { return speed; }
			const float& Get_pene(void) const noexcept { return pene; }
			const int& Get_damage(void) const noexcept { return damage; }
			const std::string& Get_name(void) const noexcept { return name; }

			auto& Set_speed(void) noexcept { return speed; }

			void Set_before(std::string path_, std::string named) {
				this->name = named;
				this->path = path_ + named;
				MV1::Load(this->path + "/ammo.mv1", &this->m_obj);
				MV1::Load(this->path + "/ammo2.mv1", &this->m_obj_full);
			}
			void Set(void) noexcept {
				int mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
				this->caliber = getparams::_float(mdata) * 0.001f;//口径
				this->speed = getparams::_float(mdata);	//弾速
				this->pene = getparams::_float(mdata);	//貫通
				this->damage = getparams::_int(mdata);//ダメージ
				FileRead_close(mdata);
			}

			AmmoData(void) noexcept {
				this->caliber = 0.f;
				this->speed = 100.f;//弾速
				this->pene = 10.f;//貫通
				this->damage = 10;//ダメージ
			}
			AmmoData(const AmmoData& tgt_minimam) {
				this->caliber = tgt_minimam.caliber;
				this->speed = tgt_minimam.speed;
				this->pene = tgt_minimam.pene;
				this->damage = tgt_minimam.damage;
			}
			void operator=(const AmmoData& tgt_minimam) {
				this->caliber = tgt_minimam.caliber;
				this->speed = tgt_minimam.speed;
				this->pene = tgt_minimam.pene;
				this->damage = tgt_minimam.damage;
			}
		};

		typedef std::pair<int, float> ArmerMeshInfo;


		class AmmoObj {
		private:
			bool			First_Flag{ false };
			bool			Hit_Flag{ false };
			float			Hit_cnt{ 0.f };
			bool			Flag{ false };
			int				m_RicochetCnt{ 0 };
			bool			DrawFlag{ false };
			int				cnt{ 0 };
			AmmoData*		spec{ nullptr };
			moves			m_move;
			VECTOR_ref		repos;
			float			speed{ 0.f };
			float			pene{ 0.f };
			VECTOR_ref		base_pos;
			const MV1*		m_MapCol{ nullptr };
		public:
			float			Hit_alpha{ 0.f };
			int				Hit_window_x{ 0 };
			int				Hit_window_y{ 0 };
		public:
			void Set_Ptr_(const MV1* pMapPtr) noexcept { m_MapCol = pMapPtr; }
			const auto& Getrepos()const noexcept { return this->repos; }
			const auto& GetMove()const noexcept { return this->m_move; }
			const auto GetEffectSize()const noexcept { return ((this->spec->Get_caliber() >= 0.020f) ? this->spec->Get_caliber() : 0.025f) / 0.1f; }

			const auto GetDamage()const noexcept { return this->spec->Get_damage(); }
			const auto GetCaliberSize()const noexcept { return this->spec->Get_caliber(); }
			const auto GetFlag()const noexcept { return this->Flag; }
		public:
			void Set() noexcept {
				this->Flag = false;
				this->m_RicochetCnt = 0;
				this->First_Flag = false;
				this->DrawFlag = false;
			}
			void Put(AmmoData* spec_t, const moves& move_) noexcept {
				this->Hit_Flag = false;
				this->Flag = true;
				this->m_RicochetCnt = 0;
				this->First_Flag = true;
				this->DrawFlag = true;
				this->cnt = 0;
				this->spec = spec_t;
				this->m_move = move_;
				this->m_move.repos = this->m_move.pos;
				this->repos = this->m_move.pos;
				this->base_pos = this->m_move.pos;
				this->speed = this->spec->Get_speed() * Scale_Rate;
				this->pene = this->spec->Get_pene();
			}
			void Hit() {
				this->Hit_Flag = true;
			}
			void Update() noexcept {
				if (this->Flag) {
					//移動確定
					this->m_move.SetPos(this->m_move.pos + (this->m_move.vec * (this->speed / FPS)));
					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->speed <= 0.f || this->pene <= 0.f || this->m_RicochetCnt > 5) {
						this->m_RicochetCnt = 0;
						this->Flag = false;
					}
					if (!this->Flag) {
						this->Hit_Flag = true;
					}
					//this->speed -= 5.f / FPS;
					//this->pene -= 5.f / FPS;
					//
				}
			}
			void Set_Draw(void) noexcept {
				if (this->Hit_Flag) {
					this->Hit_Flag = false;
					this->Hit_cnt = 0.25f;
					auto p = ConvWorldPosToScreenPos(this->m_move.pos.get());
					if (p.z >= 0.f && p.z <= 1.f) {
						this->Hit_alpha = 1.f;
						this->Hit_window_x = int(p.x);
						this->Hit_window_y = int(p.y);
					}
				}
				if (this->Hit_cnt > 0.f) {
					Easing(&this->Hit_alpha, 2.f, 0.95f, EasingType::OutExpo);
					this->Hit_cnt -= 1.f / FPS;
				}
				else {
					Easing(&this->Hit_alpha, 0.f, 0.8f, EasingType::OutExpo);
					this->Hit_cnt = 0.f;
				}
			}
			void Draw(void) noexcept {
				if (this->DrawFlag) {
					DrawCapsule_3D(this->m_move.pos, this->repos, ((this->spec->Get_caliber() - 0.00762f) * 0.1f + 0.00762f), GetColor(255, 255, 172), GetColor(255, 255, 255));
					if (!this->Flag) {
						this->DrawFlag = false;
					}
				}
			}
			/*おわり*/
			void Dispose(void) noexcept { m_MapCol = nullptr; }
		public:
			const auto ColCheckMap() noexcept {
				MV1_COLL_RESULT_POLY ColResGround; ColResGround.HitFlag = FALSE;
				if (this->Flag) {
					if (this->First_Flag) {
						this->repos = this->m_move.pos + (this->m_move.repos - this->m_move.pos).Norm() * ((this->base_pos - this->m_move.pos).size());
					}
					else {
						this->repos = this->m_move.pos + (this->m_move.repos - this->m_move.pos).Norm() * ((this->m_move.repos - this->m_move.pos).size() * FPS / 60.f);
					}
					ColResGround = m_MapCol->CollCheck_Line(this->repos, this->m_move.pos);
					if (ColResGround.HitFlag == TRUE) {
						this->m_move.pos = ColResGround.HitPosition;
					}
				}
				return ColResGround;
			}
			const auto FlagOff() noexcept {
				if (this->Flag) {
					this->Flag = false;
					return true;
				}
				return false;
			}
			const auto PenetrationCheck(float pArmer, const VECTOR_ref& normal) const noexcept {
				return (this->pene > pArmer * (1.0f / std::abs(this->m_move.vec.Norm().dot(normal))));
			}
			void Ricochet(const VECTOR_ref& position, const VECTOR_ref& normal) noexcept {
				//this->pene *= 0.8f;
				this->m_RicochetCnt++;
				this->m_move.vec = (this->m_move.vec + normal * ((this->m_move.vec.dot(normal)) * -2.0f)).Norm();
				this->m_move.pos = this->m_move.vec * (0.1f) + position;
			}

		};
		//戦車砲データ
		class gun_frame {
			std::vector<std::string> useammo;//現状未使用

			int Ammo_Cap = 0;
			std::string name;
			std::vector<AmmoData> Spec;	/**/
			float load_time{ 0.f };
			std::array<frames, 3> frame;
			int sound_use = 0;
		public:
			const auto& Get_Ammo_Cap()const noexcept { return Ammo_Cap; }
			const auto& Get_name()const noexcept { return name; }
			const auto& Get_Spec()const noexcept { return Spec; }
			const auto& Get_load_time()const noexcept { return load_time; }
			const auto& Get_frame(size_t ID_t)const noexcept { return frame[ID_t]; }
			const auto& Get_sound()const noexcept { return sound_use; }
			//const auto& Get_()const noexcept { return; }

			gun_frame(void) noexcept {
				frame[0].first = -1;
				frame[1].first = -1;
				frame[2].first = -1;
			}

			void Set(const MV1& obj, int i) {
				this->frame[0].Set_World(i, obj);
				this->frame[2].first = -1;
				this->frame[1].first = -1;
				auto p2 = obj.frame_parent(this->frame[0].first);
				if (p2 >= 0) {
					this->frame[0].second -= obj.frame(int(p2)); //親がいる時引いとく
				}
				if (obj.frame_child_num(this->frame[0].first) <= 0) {
					return;
				}
				int child_num = (int)obj.frame_child(this->frame[0].first, 0);
				if (obj.frame_name(child_num).find("仰角", 0) != std::string::npos) {
					this->frame[1].Set_Local(child_num, obj);
				}
				if (this->frame[1].first == -1) {
					return;
				}
				if (obj.frame_child_num(this->frame[1].first) <= 0) {
					return;
				}
				{
					this->frame[2].Set_Local((int)obj.frame_child(this->frame[1].first, 0), obj);
				}
			}

			void Set_Performance(int mdata, const std::string& stt) {
				this->name = getparams::getright(stt);
				this->load_time = getparams::_float(mdata);
				this->Ammo_Cap = getparams::_int(mdata);
				this->sound_use = getparams::_int(mdata);
				this->Spec.resize(this->Spec.size() + 1);
				this->Spec.back().Set_before("data/ammo/", getparams::_str(mdata));
				while (true) {
					auto stp = getparams::Getstr(mdata);
					if (stp.find("useammo" + std::to_string(this->useammo.size())) == std::string::npos) {
						break;
					}
					this->useammo.emplace_back(getparams::getright(stp));
				}
			}

			void Set_Ammos_after(void) noexcept {
				for (auto& a : this->Spec) {
					a.Set();
				}
			}
		};
		//
		//戦車データ
		class VhehicleData {
			std::string name;									/**/
			MV1 obj, col;										/**/
			VECTOR_ref minpos, maxpos;							/**/
			std::vector<gun_frame> gunframe;					/**/
			std::vector<frames> wheelframe;						/**/
			std::vector<frames> wheelframe_nospring;			/*誘導輪回転*/
			int HP = 0;											/**/
			std::vector<ArmerMeshInfo> armer_mesh;				/*装甲ID*/
			std::vector<int> space_mesh;						/*装甲ID*/
			std::vector<int> module_mesh;						/*装甲ID*/
			bool isfloat{ false };								/*浮くかどうか*/
			float down_in_water{ 0.f };							/*沈む判定箇所*/
			float front_speed_limit{ 0.f };						/*前進速度(km/h)*/
			float back_speed_limit{ 0.f };						/*後退速度(km/h)*/
			float body_rad_limit{ 0.f };							/*旋回速度(度/秒)*/
			float turret_rad_limit{ 0.f };						/*砲塔駆動速度(度/秒)*/
			frames fps_view;									//コックピット
			GraphHandle ui_pic;									//シルエット
			int pic_x, pic_y;									//サイズ
			std::array<int, 4> square{ 0 };						//車輛の四辺
			std::array<std::vector<frames>, 2> b2upsideframe;	/*履帯上*/
			std::array<std::vector<frames>, 2> b2downsideframe;	/*履帯*/


			void copy(const VhehicleData& t) {
				this->wheelframe.clear();
				for (auto& p : t.wheelframe) {
					this->wheelframe.resize(this->wheelframe.size() + 1);
					this->wheelframe.back() = p;
				}
				this->wheelframe_nospring.clear();
				for (auto& p : t.wheelframe_nospring) {
					this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
					this->wheelframe_nospring.back() = p;
				}
				this->name = t.name;
				this->minpos = t.minpos;
				this->maxpos = t.maxpos;
				this->gunframe = t.gunframe;
				this->HP = t.HP;
				this->armer_mesh = t.armer_mesh;
				this->space_mesh = t.space_mesh;
				this->module_mesh = t.module_mesh;
				this->isfloat = t.isfloat;
				this->down_in_water = t.down_in_water;
				this->front_speed_limit = t.front_speed_limit;
				this->back_speed_limit = t.back_speed_limit;
				this->body_rad_limit = t.body_rad_limit;
				this->turret_rad_limit = t.turret_rad_limit;
				this->square = t.square;
				this->b2upsideframe = t.b2upsideframe;
				this->b2downsideframe = t.b2downsideframe;
				this->fps_view = t.fps_view;

				this->ui_pic = t.ui_pic.Duplicate();
				this->pic_x = t.pic_x;
				this->pic_y = t.pic_y;
			}
		public:
			int Reload_ID = -1;
			//
			const auto& Get_name()const noexcept { return name; }//未使用

			const auto& Get_obj()const noexcept { return obj; }
			const auto& Get_col()const noexcept { return col; }
			const auto& Get_minpos()const noexcept { return minpos; }
			const auto& Get_maxpos()const noexcept { return maxpos; }
			const auto& Get_gunframe(size_t ID_t)const noexcept { return gunframe[ID_t]; }
			const auto& Get_gunframe()const noexcept { return gunframe; }
			const auto& Get_wheelframe()const noexcept { return wheelframe; }
			const auto& Get_wheelframe_nospring()const noexcept { return wheelframe_nospring; }
			const auto& Get_HP()const noexcept { return HP; }
			const auto& Get_armer_mesh()const noexcept { return armer_mesh; }
			const auto& Get_space_mesh()const noexcept { return space_mesh; }
			const auto& Get_module_mesh()const noexcept { return module_mesh; }
			const auto& Get_isfloat()const noexcept { return isfloat; }
			const auto& Get_down_in_water()const noexcept { return down_in_water; }
			const auto& Get_front_speed_limit()const noexcept { return front_speed_limit; }
			const auto& Get_back_speed_limit()const noexcept { return back_speed_limit; }
			const auto& Get_body_rad_limit()const noexcept { return body_rad_limit; }
			const auto& Get_turret_rad_limit()const noexcept { return turret_rad_limit; }
			const auto& Get_fps_view()const noexcept { return fps_view; }
			const auto& Get_ui_pic()const noexcept { return ui_pic; }
			const auto& Get_pic_x()const noexcept { return pic_x; }
			const auto& Get_pic_y()const noexcept { return pic_y; }
			const auto& Get_square(size_t ID_t)const noexcept { return square[ID_t]; }
			const auto& Get_square()const noexcept { return square; }
			const auto& Get_b2upsideframe(size_t ID_t)const noexcept { return b2upsideframe[ID_t]; }
			const auto& Get_b2upsideframe()const noexcept { return b2upsideframe; }
			const auto& Get_b2downsideframe()const noexcept { return b2downsideframe; }
			//コンストラクタ
			VhehicleData(void) noexcept { }
			VhehicleData(const VhehicleData& t) { this->copy(t); }
			void operator=(const VhehicleData& t) { this->copy(t); }
			//事前読み込み
			static void Set_Pre(std::vector<VhehicleData>* veh_, const char* name) {
				auto data_t = GetFileNamesInDirectory(name);
				for (auto& d : data_t) {
					if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						veh_->resize(veh_->size() + 1);
						veh_->back().name = d.cFileName;
					}
				}
				for (auto& t : *veh_) {
					MV1::Load(std::string(name) + t.name + "/model_LOADCALC.mv1", &t.obj);
					MV1::Load(std::string(name) + t.name + "/col.mv1", &t.col);
					t.ui_pic = GraphHandle::Load(std::string(name) + t.name + "/pic.png");
				}
			}

			const int GetSide(bool isLeft, bool isFront) const noexcept {
				int ans = 0;
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if ((isLeft ? 1.f : -1.f) * f.second.x() >= 0) {
						ans = f.first;
						tmp = f.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (ans != f.first) {
						if ((isLeft ? 1.f : -1.f) * f.second.x() >= 0) {
							if (isFront) {
								if (tmp > f.second.z()) {
									ans = f.first;
									tmp = f.second.z();
								}
							}
							else {
								if (tmp < f.second.z()) {
									ans = f.first;
									tmp = f.second.z();
								}
							}
						}
					}
				}
				return ans;
			}
			//メイン読み込み
			void Set(void) noexcept {
				using namespace std::literals;
				//αテスト
				this->obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				GetGraphSize(this->ui_pic.get(), &this->pic_x, &this->pic_y);
				//固有
				this->down_in_water = 0.f;
				for (int i = 0; i < this->obj.mesh_num(); i++) {
					auto p = this->obj.mesh_maxpos(i).y();
					if (this->down_in_water < p) {
						this->down_in_water = p;
					}
				}
				this->down_in_water /= 2.f;
				for (int i = 0; i < this->obj.frame_num(); i++) {
					std::string p = this->obj.frame_name(i);
					if (p.find("転輪", 0) != std::string::npos) {
						this->wheelframe.resize(this->wheelframe.size() + 1);
						this->wheelframe.back().Set_World(i, this->obj);
					}
					else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
						this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
						this->wheelframe_nospring.back().Set_World(i, this->obj);
					}
					else if (p.find("旋回", 0) != std::string::npos) {
						this->gunframe.resize(this->gunframe.size() + 1);
						this->gunframe.back().Set(this->obj, i);
					}
					else if (p == "min") {
						this->minpos = this->obj.frame(i);
					}
					else if (p == "max") {
						this->maxpos = this->obj.frame(i);
					}
					else if (p.find("２D物理", 0) != std::string::npos || p.find("2D物理", 0) != std::string::npos) { //2D物理
						this->b2upsideframe[0].clear();
						this->b2upsideframe[1].clear();
						for (int z = 0; z < this->obj.frame_child_num(i); z++) {
							if (this->obj.frame(i + 1 + z).x() > 0) {
								this->b2upsideframe[0].resize(this->b2upsideframe[0].size() + 1);
								this->b2upsideframe[0].back().Set_World(i + 1 + z, this->obj);
							}
							else {
								this->b2upsideframe[1].resize(this->b2upsideframe[1].size() + 1);
								this->b2upsideframe[1].back().Set_World(i + 1 + z, this->obj);
							}
						}
						std::sort(this->b2upsideframe[0].begin(), this->b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
						std::sort(this->b2upsideframe[1].begin(), this->b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
					}
					else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
						this->b2downsideframe[0].clear();
						this->b2downsideframe[1].clear();
						for (int z = 0; z < this->obj.frame_child_num(i); z++) {
							if (this->obj.frame(i + 1 + z).x() > 0) {
								this->b2downsideframe[0].resize(this->b2downsideframe[0].size() + 1);
								this->b2downsideframe[0].back().Set_World(i + 1 + z, this->obj);
							}
							else {
								this->b2downsideframe[1].resize(this->b2downsideframe[1].size() + 1);
								this->b2downsideframe[1].back().Set_World(i + 1 + z, this->obj);
							}
						}
					}
				}
				//4隅確定
				this->square[0] = GetSide(true, false);			//2		左後部0
				this->square[1] = GetSide(true, true);			//10	左前部1
				this->square[2] = GetSide(false, false);		//3		右後部2
				this->square[3] = GetSide(false, true);			//11	右前部3
				//装甲
				for (int i = 0; i < this->col.mesh_num(); i++) {
					std::string p = this->col.material_name(i);
					if (p.find("armer", 0) != std::string::npos) {
						this->armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//装甲
					}
					else if (p.find("space", 0) != std::string::npos) {
						this->space_mesh.emplace_back(i);//空間装甲
					}
					else {
						this->module_mesh.emplace_back(i);//モジュール
					}
				}
				//data
				{
					int mdata = FileRead_open(("data/tank/" + this->name + "/data.txt").c_str(), FALSE);
					this->isfloat = getparams::_bool(mdata);
					this->front_speed_limit = getparams::_float(mdata);
					this->back_speed_limit = getparams::_float(mdata);
					this->body_rad_limit = getparams::_float(mdata);
					this->turret_rad_limit = deg2rad(getparams::_float(mdata));
					this->HP = getparams::_int(mdata);
					auto stt = getparams::_str(mdata);
					for (auto& g : this->gunframe) { g.Set_Performance(mdata, stt); }
					Reload_ID = getparams::_int(mdata);//サウンド
					FileRead_close(mdata);
					for (auto& g : this->gunframe) { g.Set_Ammos_after(); }
				}
			}
		};
		//
		class Guns {
		private:
			std::array<AmmoObj, max_bullet> bullet;			/*確保する弾*/
			size_t use_bullet{ 0 };							/*確保する弾*/
			moves Gun_move;
			float loadcnt{ 0 };								/*装てんカウンター*/
			bool reload_se_f{ true };
			float fired{ 0.f };								/*駐退*/
			float firereact{ 0.f };							/*反動*/
			int rounds{ 0 };								/*弾数*/
			gun_frame gun_info;								/**/
			std::vector<AmmoData> Spec;						/**/
			float xrad{ 0.f }, yrad{ 0.f };
			float xrad_shot{ 0.f };														//射撃反動x
			float zrad_shot{ 0.f };														//射撃反動z
		private:
			bool Calc_Gun(bool key, const VECTOR_ref& vehPos) {
				auto SE = SoundPool::Instance();
				bool ans = (key && this->loadcnt == 0 && this->rounds > 0);
				if (ans) {
					this->loadcnt = this->gun_info.Get_load_time();
					this->reload_se_f = true;
					this->rounds = std::max<int>(this->rounds - 1, 0);
					this->fired = 1.f;
					this->firereact = std::clamp(this->firereact + this->Spec[0].Get_caliber() * 10.f, 0.f, 3.f);
				}
				if (this->reload_se_f && this->loadcnt < 1.f) {
					this->reload_se_f = false;
					//サウンド
					SE->Get((int)SoundEnum::Tank_Reload).Play_3D(0, vehPos, 3.f*Scale_Rate, 128);
				}
				this->loadcnt = std::max(this->loadcnt - 1.f / FPS, 0.f);
				this->fired = std::max(this->fired - 1.f / FPS, 0.f);
				this->firereact = std::max(this->firereact - 1.f / FPS, 0.f);
				return ans;
			}
			//弾の生成
			void Create_bullet(AmmoData* spec_t, const VECTOR_ref& pos_t) noexcept {
				moves tmp;
				tmp.SetPos(pos_t);
				tmp.vec = Gun_move.mat.zvec() * -1.f;
				this->bullet[this->use_bullet].Put(spec_t, tmp);
				++this->use_bullet %= this->bullet.size();
			}
		public:
			auto& GetBullet(void) noexcept { return bullet; }

			const auto& Getrounds(void) const noexcept { return rounds; }
			const auto& Getloadtime(void) const noexcept { return loadcnt; }
			const auto& Getgun_info(void) const noexcept { return gun_info; }
			const auto& GetCaliberSize(void) const noexcept { return this->Spec[0].Get_caliber(); }
			//銃反動
			void FireReaction(MATRIX_ref* mat_t) noexcept {
				auto firereact_t = deg2rad(-this->firereact * this->Spec[0].Get_caliber() * 50.f);
				Easing(&this->xrad_shot, firereact_t * cos(this->yrad), 0.85f, EasingType::OutExpo);
				Easing(&this->zrad_shot, firereact_t * sin(this->yrad), 0.85f, EasingType::OutExpo);
				(*mat_t) *= MATRIX_ref::RotAxis(mat_t->xvec(), -this->xrad_shot) * MATRIX_ref::RotAxis(mat_t->zvec(), this->zrad_shot);
			}
		public:
			//弾
			void SetUp_bullet(const MV1* pMapPtr) noexcept { for (auto& a : this->bullet) { a.Set_Ptr_(pMapPtr); } }
			//UI描画用用意
			void Set_Draw_bullet(void) noexcept { for (auto& a : this->bullet) { a.Set_Draw(); } }
			//角度指示
			void SetGunRad(const float view_xrad_t, const float view_yrad_t, float limit) {
				this->yrad += std::clamp(view_yrad_t, -limit, limit); //this->yrad = std::clamp(this->yrad + std::clamp(view_yrad_t, -limit, limit),deg2rad(-30.0)+this->yrad,deg2rad(30.0)+this->yrad);//射界制限
				this->xrad = std::clamp(this->xrad + std::clamp(view_xrad_t, -limit, limit), deg2rad(-10), deg2rad(20));
			}
			//
			void Set_Frame(MV1* obj_body_t)const noexcept {
				frames id;
				id = this->gun_info.Get_frame(0);
				if (id.first >= 0) { obj_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::RotY(this->yrad) * MATRIX_ref::Mtrans(id.second)); }
				id = this->gun_info.Get_frame(1);
				if (id.first >= 0) { obj_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::RotX(this->xrad) * MATRIX_ref::Mtrans(id.second)); }
				id = this->gun_info.Get_frame(2);
				if (id.first >= 0) { obj_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::Mtrans(VECTOR_ref::front() * (this->fired * 0.5f)) * MATRIX_ref::Mtrans(id.second)); }
			}
			//
			Guns(void) noexcept { }
			Guns(const Guns&) { }
			//
			void Set(const gun_frame& resorce) {
				this->gun_info = resorce;
				this->rounds = this->gun_info.Get_Ammo_Cap();
				//使用砲弾
				this->Spec = this->gun_info.Get_Spec();
			}
			//銃演算
			bool Update(bool key, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t, const VECTOR_ref& vehPos) {
				auto SE = SoundPool::Instance();
				bool isshot = false;
				Gun_move.mat = mat_t;
				//射撃
				if (Calc_Gun(key, vehPos)) {
					//排莢、弾
					Create_bullet(&this->Spec[0], pos_t);
					isshot = true;
					//サウンド
					SE->Get((int)SoundEnum::Tank_Shot).Play_3D(this->gun_info.Get_sound(), vehPos, 250.f*Scale_Rate, 128);
				}
				//弾の処理
				for (auto& a : this->bullet) {
					a.Update();
				}
				return isshot;
			}
			void CheckHit() {
				//for (auto& a : this->bullet) { a.Update(); }
			}
			/*弾道描画*/
			void Draw_ammo(void) noexcept {
				for (auto& a : this->bullet) { a.Draw(); }
			}
			/* UI向けにヒット部分を表示*/
			void Draw_Hit_UI(GraphHandle& Hit_Graph) noexcept {
				for (auto& a : this->bullet) {
					if (a.Hit_alpha >= 10.f / 255.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * a.Hit_alpha));
						Hit_Graph.DrawRotaGraph(a.Hit_window_x, a.Hit_window_y, (float)y_r(a.Hit_alpha * 0.5f * 100.0f) / 100.f, 0.f, true);
					}
				}
			}
			/*おわり*/
			void Dispose(void) noexcept {
				for (auto& a : this->bullet) { a.Dispose(); }			//弾
			}
			void Reset(void) noexcept {
				this->fired = 0.f;
				this->loadcnt = 0.f;
				this->reload_se_f = true;
				this->rounds = 0;
				this->Spec.clear();
			}
		};
		//操作共通
		class VehicleClass : public ObjectBaseClass, public Effect_UseControl {
		private:
			//HP関連
			class Damages {
			private:
				//被弾角度
				class damage_rad {
				public:
					float alpfa{ 1.f };
					float rad{ 0.f };
				};
			private:
				unsigned int got_damage_color{ 0 };
				int got_damage_x{ 0 };
				float got_damage_f{ 0.f };
				std::vector<damage_rad> got_damage_rad;
				int got_damage{ 0 };
				int HP{ 100 };
				float HP_r{ 100.f };
				int HP_full{ 100 };
				std::vector<int> HP_parts;
			public:
				//getter
				const auto IsShow(void) const noexcept { return (int(this->HP_r) - this->HP) >= 2; }
				const auto Get_alive(void) const noexcept { return this->HP != 0; }																								//生きているか
				const auto HP_Per(void) const noexcept { return float(this->HP) / this->HP_full; }
				const auto HP_r_Per(void) const noexcept { return this->HP_r / this->HP_full; }
				const auto& Get_HP()const noexcept { return HP; }
				const auto& Get_HP_r()const noexcept { return HP_r; }
				const auto& Get_HP_full()const noexcept { return HP_full; }
				const auto& Get_got_damage()const noexcept { return got_damage; }
				const auto& Get_got_damage_rad(void) const noexcept { return got_damage_rad; }
				const auto& Get_got_damage_f()const noexcept { return got_damage_f; }
				const auto& Get_got_damage_x()const noexcept { return got_damage_x; }
				const auto& Get_got_damage_color()const noexcept { return got_damage_color; }
				const auto& Get_HP_parts()const noexcept { return HP_parts; }
			public:
				void SubHP_Parts(int damage_t, int parts_Set_t) noexcept {
					this->HP_parts[parts_Set_t] = std::max<int>(this->HP_parts[parts_Set_t] - damage_t, 0); //
				}
				void SubHP(int damage_t, float rad_t, bool isshow = true)  noexcept {
					auto old = this->HP;
					this->HP = std::max<int>(this->HP - damage_t, 0); //
					this->got_damage = old - this->HP;
					this->got_damage_color = GetColor(255, 255, 255);

					if (float(this->HP) / float(this->HP_full) <= 0.66) {
						this->got_damage_color = GetColor(255, 255, 0);
					}
					if (float(this->HP) / float(this->HP_full) <= 0.33) {
						this->got_damage_color = GetColor(255, 128, 0);
					}
					if (damage_t != this->got_damage) {
						this->got_damage_color = GetColor(255, 0, 0);
					}
					this->got_damage_x = -255 + GetRand(255 * 2);
					this->got_damage_f = 1.f;

					this->got_damage_rad.resize(this->got_damage_rad.size() + 1);
					this->got_damage_rad.back().alpfa = 1.f;
					this->got_damage_rad.back().rad = rad_t;

					if (!isshow) {
						this->HP_r = float(this->HP);
					}
				}
				void AddHP(int repair_t) noexcept {
					this->HP = std::clamp<int>(this->HP + repair_t, 0, this->HP_full);
				}
			public:
				void Set(int parts_num, int Full_t = -1) noexcept {
					if (Full_t > 0) {
						this->HP_full = Full_t;
					}
					this->HP = this->HP_full;
					this->HP_parts.resize(parts_num);
					for (auto& h : this->HP_parts) { h = this->HP_full; }//モジュール耐久

				}
				void Update(void) noexcept {
					Easing(&this->HP_r, float(this->HP), 0.95f, EasingType::OutExpo);
					Easing(&this->got_damage_f, 0.f, 0.95f, EasingType::OutExpo);

					{
						for (auto& d : this->got_damage_rad) { Easing(&d.alpfa, 0.f, 0.975f, EasingType::OutExpo); }
						while (true) {
							bool none = true;
							for (auto& d : this->got_damage_rad) {
								if (std::clamp(int(255.f * (1.f - powf(1.f - d.alpfa, 5.f))), 0, 255) < 5) {
									auto i = (&d - &this->got_damage_rad.front());
									std::iter_swap(this->got_damage_rad.begin() + i, this->got_damage_rad.end() - 1);
									this->got_damage_rad.pop_back();
									none = false;
									break;
								}
							}
							if (none) {
								break;
							}
						}
					}
				}
				void Dispose(void) noexcept {
					this->HP = 0;
					this->HP_parts.clear();
				}
			};
			class VEHICLE_HitControl {
			private:
				class HitSortInfo {
					size_t m_hitmesh{ SIZE_MAX };
					float m_hitDistance{ -1 };
				public:
					bool operator<(const HitSortInfo& tgt) const noexcept { return this->m_hitDistance < tgt.m_hitDistance; }
					void Set(size_t f_t, float s_t)noexcept {
						this->m_hitmesh = f_t;
						this->m_hitDistance = s_t;
					}
					const auto GetHitMesh() const noexcept { return this->m_hitmesh; }
					bool Hit_Check(std::optional<ArmerMeshInfo>& HitArmerMesh, const std::shared_ptr<VehicleClass>& tgt) noexcept {
						//装甲面に当たらなかったならスルー
						if (this->m_hitDistance == (std::numeric_limits<float>::max)()) {
							return false;
						}
						//装甲面に当たったのでHitArmerMeshに代入して終了
						for (auto& mesh : tgt->use_veh->Get_armer_mesh()) {
							if (this->m_hitmesh == mesh.first) {
								HitArmerMesh = mesh;
								return false;
							}
						}
						return true;
					}
				};
			private:
				std::vector<HitSortInfo> hitssort;												/*フレームに当たった順番*/
				std::vector<MV1_COLL_RESULT_POLY> hitres;									/*確保*/
			public:
				void sort_Hit(void) noexcept { std::sort(this->hitssort.begin(), this->hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; }); }
				std::vector<HitSortInfo>& Get_hitssort(void) noexcept { return this->hitssort; }
				const MV1_COLL_RESULT_POLY& Get_hitres(size_t ID_t) const noexcept { return this->hitres[ID_t]; }
				bool HitCheck(int m, const VECTOR_ref& repos, const MV1_COLL_RESULT_POLY& hitres_t) {
					this->hitres[m] = hitres_t;
					if (this->hitres[m].HitFlag == TRUE) {
						this->hitssort[m].Set(m, (repos - this->hitres[m].HitPosition).size());
						return true;
					}
					else {
						this->hitssort[m].Set(m, (std::numeric_limits<float>::max)());
						return false;
					}
				}
				void Set(size_t siz_t) {
					this->hitres.resize(siz_t);		//モジュールごとの当たり判定結果を確保
					this->hitssort.resize(siz_t);	//モジュールごとの当たり判定順序を確保
				}
				void Reset(void) noexcept {
					this->hitres.clear();
					this->hitssort.clear();
				}
			};
			class cat_frame {
			public:
				frames frame;
				float will_y{ 0.f };
				MV1_COLL_RESULT_POLY colres{};
				EffectS gndsmkeffcs;
				float gndsmksize = 1.f;

				cat_frame(void) noexcept {
					frame.first = -1;
					will_y = 0.f;
					gndsmksize = 1.f;
				}
				cat_frame(const cat_frame& tgt) {
					frame = tgt.frame;
					will_y = 0.f;
					gndsmksize = 1.f;
				}
				void operator=(const cat_frame& tgt) { frame = tgt.frame; }
			};
			struct foot_frame {
				frames frame;
				float will_y{ 0.f };
			};
			//履帯BOX2D
			class FootWorld {
			public:
				std::shared_ptr<b2World> world{ nullptr };		/*足world*/
				b2RevoluteJointDef f_jointDef;					/*ジョイント*/
				std::vector<b2Pats> Foot, Wheel, Yudo;			/**/
			};
		private:
			bool												m_ReadySwitch{ false };
			bool												m_KeyActive{ true };

			bool												nearhit{ false };								//
			float												dist = 100.f;

			std::array<bool, 6>									key{ false };									//キー
			float												xrad_Add{ 0.f };								//マウスエイム用変数確保
			float												yrad_Add{ 0.f };								//マウスエイム用変数確保
			float												xrad_p{ 0.f };									//マウスエイム用変数確保
			float												yrad_p{ 0.f };									//マウスエイム用変数確保

			float												yrad_body{ 0.f };

			float												wheel_Left{ 0.f };
			float												wheel_Right{ 0.f };								//転輪回転
			std::vector<Guns>									Gun_;											/**/
			float												speed{ 0.f };									//移動速度
			float												speed_add{ 0.f };
			float												speed_sub{ 0.f };								/**/
			VECTOR_ref											m_rad;
			VECTOR_ref											radAdd;
			VECTOR_ref											m_rad_Buf;
			float												yradadd_left{ 0.f };
			float												yradadd_right{ 0.f };							/**/
			b2Pats												b2mine;											/*box2d*/
			float												spd_buf{ 0.f };									/*box2d*/
			float												wheel_Leftadd{ 0.f };
			float												wheel_Rightadd{ 0.f };							//転輪回転
			VECTOR_ref											wheel_normal{ VECTOR_ref::up() };				/**/
			std::array<std::vector<cat_frame>, 2>				b2downsideframe;								/*履帯*/
			std::vector<foot_frame>								wheelframe;										/**/
			std::array<FootWorld, 2>							foot;											//履帯BOX2D
			float												spd_rec;										/**/
			float												view_xrad{ 0.f };								/**/
			float												view_yrad{ 0.f };								/**/
			float												engine_time{ 0.f };								/*エンジン音声*/
			float												range{ 6.0f };									/**/
			float												range_r = range;								/**/
			float												range_change = range / 10.f;					/**/
			float												ratio = 1.f;									/**/
			bool												changeview = false;								/**/
			MATRIX_ref											lookvec;										/**/
			HIT_ACTIVE											Hit_active;										/**/

			bool												m_PosBufOverRideFlag{ false };
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			float												m_yRadOverRide;

			VECTOR_ref											add_vec_real;									//移動ベクトルバッファ
			Damages												Damage;
			std::shared_ptr<VehicleClass>*						MINE_v{ nullptr };
			std::vector<std::shared_ptr<VehicleClass>>*			ALL_v{ nullptr };
			VEHICLE_HitControl									hitControl;
			VhehicleData*										use_veh{ nullptr };								/*固有値*/
		public:
			CharaTypeID											m_CharaType{ CharaTypeID::Enemy };

			float												m_Score{ 0.f };							//スコア


			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			const auto&		GetHP(void) const noexcept { return this->Damage.Get_HP(); }
			const auto&		GetHPMax(void) const noexcept { return this->Damage.Get_HP_full(); }

			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			const auto&		GetName(void) const noexcept { return this->use_veh->Get_name(); }



			const auto&		GetLookVec(void) const noexcept { return this->lookvec; }




			//getter
			const auto		Head_direction(void)const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(Gun_[0].Getgun_info().Get_frame(1).first); }
			const auto		Get_body_yrad(void) const noexcept { return this->yrad_body; }
			const auto&		Get_dist(void) const noexcept { return this->dist; }																			//UI用
			const auto		Get_pseed_per(void) const noexcept { return add_vec_real.size() / (this->use_veh->Get_front_speed_limit() / 3.6f); }				//移動速度のパーセンテージ
			const auto		is_ADS(void) const noexcept { return this->range == 0.f; }																		//ADS中
			const auto&		Gunloadtime(size_t id_t) const noexcept { return this->Gun_[id_t].Getloadtime(); }
			const auto&		Gunround(size_t id_t) const noexcept { return this->Gun_[id_t].Getrounds(); }
			const auto&		Guninfo(size_t id_t) const noexcept { return this->Gun_[id_t].Getgun_info(); }
			const auto		Get_Gunsize(void) const noexcept { return this->Gun_.size(); }
			const auto&		Get_ratio(void) const noexcept { return this->ratio; }																			//UI用
			const auto&		Get_changeview(void) const noexcept { return this->changeview; }																//照準変更時
			const auto		Get_EyePos_Base(void) const noexcept { return (is_ADS()) ? (GetObj_const().frame(Guninfo(0).Get_frame(1).first)) : (this->m_move.pos + (this->m_move.mat.yvec() * 3.f * Scale_Rate)); }
			const auto		Set_MidPos(void) noexcept { return (this->m_move.pos + (this->m_move.mat.yvec() * 1.5f * Scale_Rate)); }						/*HPバーを表示する場所*/
			void			ReSet_range(void) noexcept { range = 6.f; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, float yrad) {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_yRadOverRide = yrad;
			}
			//使用するポインタ設定
			void			Set_Ptr(std::vector<std::shared_ptr<VehicleClass>>* ALL_v_t, std::shared_ptr<VehicleClass>* MINE_v_t) noexcept {
				ALL_v = ALL_v_t;
				MINE_v = MINE_v_t;
			}
			//判定起動
			const auto		RefreshCol(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, float pRange) noexcept {
				//すでに起動しているなら無視
				if (this->nearhit) { return true; }
				if (GetMinLenSegmentToPoint(StartPos, EndPos, this->m_move.pos) <= pRange) {
					//判定起動
					this->nearhit = true;
					for (int i = 0; i < this->m_col.mesh_num(); i++) {
						this->m_col.RefreshCollInfo(-1, i);
					}
					return true;
				}
				return false;
			}
			//判定取得
			const auto		col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, const int sel) const noexcept { return this->m_col.CollCheck_Line(StartPos, EndPos, -1, sel); }
			void			col_nearest(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) const noexcept {
				MV1_COLL_RESULT_POLY colres;
				for (int i = 0; i < this->m_col.mesh_num(); ++i) {
					colres = col_line(StartPos, *EndPos, i);
					if (colres.HitFlag == TRUE) {
						*EndPos = colres.HitPosition;
					}
				}
			}
			//被弾チェック
			const auto		CheckAmmoHited(const AmmoObj& pAmmo) {
				auto HitCheck_Tank = [&](int m, const moves& ray) { return hitControl.HitCheck(m, ray.repos, col_line(ray.repos, ray.pos, m)); };
				bool is_Hit = false;
				for (auto& m : this->use_veh->Get_module_mesh()) { is_Hit |= HitCheck_Tank(m, pAmmo.GetMove()); }				//モジュール
				for (auto& m : this->use_veh->Get_space_mesh()) { is_Hit |= HitCheck_Tank(m, pAmmo.GetMove()); }				//空間装甲
				for (auto& m : this->use_veh->Get_armer_mesh()) { is_Hit |= HitCheck_Tank(m.first, pAmmo.GetMove()); }		//装甲
				return is_Hit;
			}
			//被弾処理
			const auto		CalcAmmoHited(AmmoObj& pAmmo, std::shared_ptr<VehicleClass>& pShooter) {
				auto SE = SoundPool::Instance();
				this->hitControl.sort_Hit();				//当たり判定を近い順にソート
				//ダメージ面に届くまで判定
				std::optional<ArmerMeshInfo> HitArmerMesh;
				bool isDamage = false;
				for (auto& tt : this->hitControl.Get_hitssort()) {
					if (!tt.Hit_Check(HitArmerMesh, *MINE_v)) {
						if (HitArmerMesh.has_value()) {
							//弾がダメージ層に届いた
							auto position = this->hitControl.Get_hitres(HitArmerMesh.value().first).HitPosition;
							auto normalvec = this->hitControl.Get_hitres(HitArmerMesh.value().first).Normal;
							//ダメージ面に当たった時に装甲値に勝てるかどうか
							if (pAmmo.PenetrationCheck(HitArmerMesh.value().second, normalvec)) {
								//貫通
								pAmmo.FlagOff();
								SE->Get((int)SoundEnum::Tank_Ricochet).Play_3D(0, position, 50.f*Scale_Rate, 64);
								//ダメージ計算
								auto v1 = MATRIX_ref::RotY(this->yrad_body).zvec();
								auto v2 = (pShooter->m_move.pos - this->m_move.pos).Norm(); v2.y(0);
								this->Damage.SubHP(pAmmo.GetDamage(), atan2f(v1.cross(v2).y(), v1.dot(v2)));
								this->Damage.SubHP_Parts(pAmmo.GetDamage(), HitArmerMesh.value().first);
								if (!this->Damage.Get_alive()) {
									//撃破
									/*
									if (this->MINE_c != nullptr) {
										pShooter->scores.Set_kill(*MINE_v, 70);
										(*MINE_v)->scores.Set_death(pShooter);
									}
									//*/
								}
								isDamage = true;
							}
							else {
								//跳弾
								pAmmo.Ricochet(position, normalvec);
								SE->Get((int)SoundEnum::Tank_Damage).Play_3D(0, position, 50.f*Scale_Rate, 64);
							}
							//エフェクトセット
							Effect_UseControl::Set_Effect(Effect::ef_reco, position, normalvec, pAmmo.GetEffectSize()*Scale_Rate);
							this->Hit_active.Set(this->m_move, position, normalvec, pAmmo.GetMove().vec.Norm(), pAmmo.GetCaliberSize()*Scale_Rate, !pAmmo.GetFlag());
						}
						else {
							//何処にも届かなかった
						}
						break;
					}
					else {
						//空間装甲などの判定
						//空間装甲、モジュールに当たったのでモジュールに30ダメ
						for (auto& mesh : this->use_veh->Get_space_mesh()) {
							if (tt.GetHitMesh() == mesh) {
								Effect_UseControl::Set_Effect(Effect::ef_reco, pAmmo.GetMove().pos, this->hitControl.Get_hitres(tt.GetHitMesh()).Normal, pAmmo.GetEffectSize()*Scale_Rate);
								this->Damage.SubHP_Parts(30, (int)tt.GetHitMesh());
							}
						}
						for (auto& mesh : this->use_veh->Get_module_mesh()) {
							if (tt.GetHitMesh() == mesh) {
								Effect_UseControl::Set_Effect(Effect::ef_reco, pAmmo.GetMove().pos, this->hitControl.Get_hitres(tt.GetHitMesh()).Normal, pAmmo.GetEffectSize()*Scale_Rate);
								this->Damage.SubHP_Parts(30, (int)tt.GetHitMesh());
							}
						}
					}
				}
				return isDamage;
			}
		private:
			void Init_Tank(std::shared_ptr<b2World>& pB2World) {
				//本体
				GetObj().material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				//コリジョン
				for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
				hitControl.Set(this->m_col.mesh_num());
				//
				VECTOR_ref minmaxsub = this->use_veh->Get_maxpos() - this->use_veh->Get_minpos();
				VECTOR_ref minmaxadd = this->use_veh->Get_minpos() + this->use_veh->Get_maxpos();
				b2PolygonShape dynamicBox;
				dynamicBox.SetAsBox(minmaxsub.x() / 2, minmaxsub.z() / 2, b2Vec2(minmaxadd.x() / 2, -minmaxadd.z() / 2), 0.f);	/*ダイナミックボディに別のボックスシェイプを定義します。*/
				this->b2mine.Set(CreateB2Body(pB2World, b2_dynamicBody, this->m_move.pos.x(), this->m_move.pos.z(), atan2f(-this->m_move.mat.zvec().x(), -this->m_move.mat.zvec().z())), &dynamicBox);	/*シェイプをボディに追加*/
				for (auto& f : foot) {
					f.world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
					float LorR = ((&f == &foot.front()) ? 1.f : -1.f);
					//履帯
					f.Foot.clear();
					{
						b2BodyDef bd;
						b2Body* BodyB = f.world->CreateBody(&bd);
						b2EdgeShape shape;
						shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
						BodyB->CreateFixture(&shape, 0.0f);
						b2Body* BodyA = BodyB;
						b2Vec2 anchor;
						for (const auto& w : this->use_veh->Get_b2upsideframe(&f - &foot.front())) {
							anchor = b2Vec2(w.second.z(), w.second.y());
							if (w.second.x() * LorR > 0) {
								f.Foot.resize(f.Foot.size() + 1);
								b2PolygonShape f_dynamicBox; /*ダイナミックボディに別のボックスシェイプを定義します。*/
								f_dynamicBox.SetAsBox(0.2f, 0.05f);
								f.Foot.back().Set(CreateB2Body(f.world, b2_dynamicBody, anchor.x, anchor.y), &f_dynamicBox);
								f.f_jointDef.Initialize(BodyA, f.Foot.back().Get(), anchor);
								f.world->CreateJoint(&f.f_jointDef);
								BodyA = f.Foot.back().Get();
							}
						}
						if (!f.Foot.empty()) {
							f.f_jointDef.Initialize(BodyA, BodyB, anchor);
							f.world->CreateJoint(&f.f_jointDef);
						}
					}
					for (const auto& w : this->use_veh->Get_b2downsideframe()) {
						auto& w2 = this->b2downsideframe[&w - &this->use_veh->Get_b2downsideframe().front()];
						w2.resize(w.size());
						for (auto& t : w2) {
							t.frame = w[&t - &w2.front()];
							t.gndsmkeffcs.set_loop(effectControl.effsorce.back());
							t.gndsmkeffcs.put_loop(VECTOR_ref::vget(0, -1, 0), VECTOR_ref::vget(0, 0, 1), 0.1f*Scale_Rate);
							t.gndsmksize = 0.1f;
						}
					}
					//転輪
					f.Wheel.clear();
					f.Yudo.clear();
					if (!f.Foot.empty()) {
						//転輪(動く)
						for (const auto& w : this->use_veh->Get_wheelframe()) {
							this->wheelframe.resize(this->wheelframe.size() + 1);
							this->wheelframe.back().frame = w;
							VECTOR_ref vects = GetObj().GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LorR > 0) {
								f.Wheel.resize(f.Wheel.size() + 1);
								b2CircleShape shape;
								shape.m_radius = vects.y() - 0.1f;
								f.Wheel.back().Set(CreateB2Body(f.world, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
						//誘導輪(動かない)
						for (const auto& w : this->use_veh->Get_wheelframe_nospring()) {
							VECTOR_ref vects = GetObj().GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LorR > 0) {
								f.Yudo.resize(f.Yudo.size() + 1);
								b2CircleShape shape;
								shape.m_radius = 0.05f;
								f.Yudo.back().Set(CreateB2Body(f.world, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
					}
				}

				//砲
				this->Gun_.resize(this->use_veh->Get_gunframe().size());
				for (auto& g : this->Gun_) { g.Set(this->use_veh->Get_gunframe(&g - &this->Gun_.front())); }
				//
								//ヒットポイント
				Damage.Set((int)this->m_col.mesh_num(), this->use_veh->Get_HP());
				/*戦車スポーン*/
				auto pp = this->m_move.mat.zvec()*-1.f;
				this->yrad_body = atan2f(pp.x(), pp.z());
				this->b2mine.SetTransform(b2Vec2(this->m_move.pos.x(), this->m_move.pos.z()), this->yrad_body);
				//弾init
				for (auto& g : this->Gun_) { g.SetUp_bullet(this->m_MapCol); }

				this->m_move.vec.clear();
			}
			void Update_Tank() {
				bool isfloat = (this->m_move.pos.y() == -this->use_veh->Get_down_in_water());
				this->spd_rec = this->speed;
				//砲塔旋回
				{
					float limit = this->use_veh->Get_turret_rad_limit() / FPS;
					for (auto& g : this->Gun_) {
						//角度指示
						g.SetGunRad(this->view_xrad, this->view_yrad, limit);
						//反映
						g.Set_Frame(&GetObj());
						g.Set_Frame(&this->m_col);
					}
				}
				{
					auto y_vec = this->m_move.mat.yvec();
					for (auto& f : this->wheelframe) {
						MATRIX_ref tmp;
						GetObj().frame_Reset(f.frame.first);
						auto startpos = GetObj().frame(f.frame.first);
						auto ColResGround = m_MapCol->CollCheck_Line(startpos + y_vec * ((-f.frame.second.y()) + 2.f*Scale_Rate), startpos + y_vec * ((-f.frame.second.y()) - 0.3f*Scale_Rate));

						Easing(&f.will_y, (ColResGround.HitFlag == TRUE) ? (ColResGround.HitPosition.y + y_vec.y() * f.frame.second.y() - startpos.y()) : -0.3f, 0.9f, EasingType::OutExpo);
						tmp = MATRIX_ref::Mtrans(VECTOR_ref::up() * f.will_y);

						frames id;
						id = f.frame;
						if (id.first >= 0) { GetObj().SetFrameLocalMatrix(id.first, (MATRIX_ref::RotX((f.frame.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * tmp) * MATRIX_ref::Mtrans(id.second)); }
					}
					for (const auto& f : this->use_veh->Get_wheelframe_nospring()) {
						frames id;
						id = f;
						if (id.first >= 0) { GetObj().SetFrameLocalMatrix(id.first, MATRIX_ref::RotX((f.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * MATRIX_ref::Mtrans(id.second)); }
					}
				}
				Easing(
					&wheel_normal,
					((GetObj().frame(this->use_veh->Get_square(0)) - GetObj().frame(this->use_veh->Get_square(3))).cross(GetObj().frame(this->use_veh->Get_square(1)) - GetObj().frame(this->use_veh->Get_square(2)))).Norm(),
					0.95f, EasingType::OutExpo);
				//履帯
				{
					for (auto& g : this->b2downsideframe) {
						for (auto& t : g) {
							MATRIX_ref tmp;
							GetObj().frame_Reset(t.frame.first);
							auto startpos = GetObj().frame(t.frame.first);
							t.colres = m_MapCol->CollCheck_Line(startpos + this->m_move.mat.yvec() * ((-t.frame.second.y()) + 2.f*Scale_Rate), startpos + this->m_move.mat.yvec() * ((-t.frame.second.y()) - 0.3f*Scale_Rate));
							if (t.colres.HitFlag == TRUE) {
								tmp = MATRIX_ref::Mtrans(VECTOR_ref::up() * (t.colres.HitPosition.y + this->m_move.mat.yvec().y() * t.frame.second.y() - startpos.y()));
							}
							else {
								tmp = MATRIX_ref::Mtrans(VECTOR_ref::up() * -0.4f);
							}
							frames id;
							id = t.frame;
							if (id.first >= 0) { GetObj().SetFrameLocalMatrix(id.first, tmp * MATRIX_ref::Mtrans(id.second)); }
						}
					}
				}
				//移動
				{
					//リセット
					{
						const auto yvec = (this->use_veh->Get_isfloat() && isfloat) ? VECTOR_ref::up() : wheel_normal;
						auto pp = (this->m_move.mat * MATRIX_ref::RotVec2(VECTOR_ref::up(), yvec).Inverse()).zvec() * -1.f;
						this->yrad_body = atan2f(pp.x(), pp.z());
						if (this->m_PosBufOverRideFlag) {
							this->yrad_body = -this->m_yRadOverRide;
						}
						const auto zvec = (MATRIX_ref::RotY(this->yrad_body)*MATRIX_ref::RotVec2(VECTOR_ref::up(), yvec)).zvec() * -1.f;
						this->m_move.mat = MATRIX_ref::Axis1_YZ(yvec, zvec);
					}
					//地面判定
					{
						auto yp = this->m_move.pos.y();
						float hight_t = 0.f;
						int cnt_t = 0;
						bool touch = false;
						for (const auto& g : this->b2downsideframe) {
							for (const auto& t : g) {
								hight_t += GetObj().frame(t.frame.first).y();
								cnt_t++;
								if (t.colres.HitFlag == TRUE) {
									touch = true;
								}
							}
						}
						//*
						int cnt_sq = 0;
						for (const auto& s : this->use_veh->Get_square()) {
							auto p_t = GetObj().frame(s);
							auto ColResGround = m_MapCol->CollCheck_Line(p_t + (VECTOR_ref::up() * 3.f*Scale_Rate), p_t + (VECTOR_ref::up() * -0.5f*Scale_Rate));
							if (ColResGround.HitFlag == TRUE) {
								hight_t += ColResGround.HitPosition.y;
								cnt_t++;
								cnt_sq++;
								touch = true;
							}
						}
						//*/
						if (touch) {
							Easing(&yp, (hight_t / cnt_t), 0.9f, EasingType::OutExpo);
						}
						this->m_move.pos.y(yp);
						//地面or水面にいるかどうか
						if (touch || (this->use_veh->Get_isfloat() && isfloat)) {
							//前進後退
							{
								const auto old = this->speed_add + this->speed_sub;
								if (key[2]) {
									this->speed_add = (this->speed_add < (this->use_veh->Get_front_speed_limit() / 3.6f)) ? (this->speed_add + (0.15f / 3.6f) * (60.f / FPS)) : this->speed_add;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.7f / 3.6f) * (60.f / FPS)) : this->speed_sub;
								}
								if (key[3]) {
									this->speed_sub = (this->speed_sub > (this->use_veh->Get_back_speed_limit() / 3.6f)) ? (this->speed_sub - (0.15f / 3.6f) * (60.f / FPS)) : this->speed_sub;
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.7f / 3.6f) * (60.f / FPS)) : this->speed_add;
								}
								if (!(key[2]) && !(key[3])) {
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
								}
								this->speed = (old + ((this->speed_add + this->speed_sub) - old) * 0.1f) / FPS;
								auto vec = this->m_move.mat.zvec() * -(this->speed*Scale_Rate);
								this->m_move.vec.x(vec.x());
								if (cnt_sq >= 3) {
									this->m_move.vec.y(vec.y());
								}
								else {
									this->m_move.vec.yadd(M_GR / (FPS * FPS));
								}
								this->m_move.vec.z(vec.z());
							}
							//旋回
							{
								this->yradadd_left = (key[4]) ? std::max(this->yradadd_left - deg2rad(3.5f * (60.f / FPS)), deg2rad(-this->use_veh->Get_body_rad_limit())) : std::min(this->yradadd_left + deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd_right = (key[5]) ? std::min(this->yradadd_right + deg2rad(3.5f * (60.f / FPS)), deg2rad(this->use_veh->Get_body_rad_limit())) : std::max(this->yradadd_right - deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->radAdd.y((this->yradadd_left + this->yradadd_right) / FPS);
							}
							//慣性
							{
								const auto radold = this->radAdd;
								this->radAdd.x(deg2rad(-(this->speed / (60.f / FPS)) / (0.1f / 3.6f) * 50.f));
								this->radAdd.z(deg2rad(-this->radAdd.y() / (deg2rad(5.f) / FPS) * 5.f));

								Easing(&m_rad,
									VECTOR_ref::vget(
										std::clamp(this->radAdd.x() - radold.x(), deg2rad(-30.f), deg2rad(30.f)),
										0.f,
										std::clamp(this->radAdd.z() - radold.z(), deg2rad(-15.f), deg2rad(15.f))
									), 0.95f, EasingType::OutExpo);

								this->m_move.mat *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), -this->m_rad.x()) * MATRIX_ref::RotAxis(this->m_move.mat.zvec(), m_rad.z());
							}
							//
						}
						else {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
					}
					if (this->m_PosBufOverRideFlag) {
						this->m_move.pos = this->m_PosBufOverRide;
						this->m_move.vec = this->m_VecBufOverRide;
					}
					this->m_move.pos += this->m_move.vec;
				}
				//射撃反動
				for (auto& g : this->Gun_) {
					g.FireReaction(&this->m_move.mat);
				}
				//転輪
				this->wheel_Leftadd = -(this->speed * 2.f - this->radAdd.y() * 5.f);
				this->wheel_Rightadd = -(this->speed * 2.f + this->radAdd.y() * 5.f);

				this->wheel_Left += this->wheel_Leftadd;  // -this->radAdd.y() * 5.f;
				this->wheel_Right += this->wheel_Rightadd; // +this->radAdd.y() * 5.f;
				//戦車壁判定
				if (this->m_PosBufOverRideFlag) {
					this->b2mine.SetTransform(b2Vec2(this->m_move.pos.x(), this->m_move.pos.z()), this->yrad_body);
				}
				else {
					this->b2mine.Update(this->m_move.vec, this->radAdd.y());
				}
				//浮く
				if (this->use_veh->Get_isfloat()) {
					this->m_move.pos.y(std::max(this->m_move.pos.y(), -this->use_veh->Get_down_in_water()));
				}
				//射撃
				for (auto& cg : this->Gun_) {
					auto mazPos = GetObj().frame(cg.Getgun_info().Get_frame(2).first);
					auto mazMat = GetObj().GetFrameLocalWorldMatrix(cg.Getgun_info().Get_frame(1).first);
					if (cg.Update(key[(&cg == &this->Gun_.front()) ? 0 : 1], mazPos, mazMat,this->m_move.pos)) {
						Effect_UseControl::Set_Effect(Effect::ef_fire, mazPos, mazMat.zvec() * -1.f, cg.GetCaliberSize() / 0.1f * Scale_Rate);				//銃発砲エフェクトのセット
					}
					for (auto& a : cg.GetBullet()) {
						if (a.GetFlag()) {
							MV1_COLL_RESULT_POLY ColResGround = a.ColCheckMap();
							bool is_HitAll = false;
							for (auto& tgt : *ALL_v) {
								if (tgt == *MINE_v) { continue; }
								if (tgt->RefreshCol(a.Getrepos(), a.GetMove().pos, 10.0f*Scale_Rate)) {
									//とりあえず当たったかどうか探す
									if (tgt->CheckAmmoHited(a)) {
										is_HitAll = true;
										a.Hit();
										if (tgt->CalcAmmoHited(a, *MINE_v)) { break; }
									}
								}
							}
							if (ColResGround.HitFlag == TRUE && !is_HitAll) {
								if (a.FlagOff()) {
									Effect_UseControl::Set_FootEffect(a.GetMove().pos, ColResGround.Normal, 0.05f / 0.1f * Scale_Rate);
									hit_obj_p.Set(a.GetCaliberSize() * Scale_Rate, a.GetMove().pos, ColResGround.Normal, a.GetMove().vec);	//弾痕
								}
							}
						}
					}

				}

				this->m_PosBufOverRideFlag = false;
			}
			void Update_Tank2() {
				auto pp = this->m_move.mat.zvec()*-1.f;
				this->yrad_body = atan2f(pp.x(), pp.z());
				//戦車座標反映
				this->m_move.mat *= MATRIX_ref::RotY(-this->b2mine.Rad() - this->yrad_body);
				this->m_move.pos = VECTOR_ref::vget(this->b2mine.Pos().x, this->m_move.pos.y(), this->b2mine.Pos().y);

				float spdrec = this->spd_buf;
				Easing(&this->spd_buf, this->b2mine.Speed() * ((this->spd_buf > 0) ? 1.f : -1.f), 0.99f, EasingType::OutExpo);
				this->speed = this->spd_buf - spdrec;

				//転輪
				b2Vec2 tmpb2 = b2Vec2((M_GR / FPS * 0.5f) * (this->m_move.mat.zvec().dot(VECTOR_ref::up())), (M_GR / FPS * 0.5f) * (this->m_move.mat.yvec().dot(VECTOR_ref::up())));
				int LorR = 0;
				for (auto& f : foot) {
					size_t i = 0;
					VECTOR_ref vects;
					int LR_t = ((LorR == 0) ? 1 : -1);
					if (f.Foot.size() != 0) {
						auto Wheel_t = f.Wheel.begin();
						for (const auto& w : this->use_veh->Get_wheelframe()) {
							vects = GetObj().GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LR_t > 0) {
								Wheel_t->SetTransform(b2Vec2(vects.z(), vects.y()), (LorR == 0) ? this->wheel_Left : this->wheel_Right);
								Wheel_t++;
							}
						}
						i = 0;
						for (const auto& w : this->use_veh->Get_wheelframe_nospring()) {
							vects = GetObj().GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LR_t > 0) {
								f.Yudo[i++].SetTransform(b2Vec2(vects.z(), vects.y()), (LorR == 0) ? this->wheel_Left : this->wheel_Right);
							}
						}
						for (auto& t : f.Foot) {
							t.SetLinearVelocity(tmpb2); //
						}

						f.world->Step(0.1f, 3, 3);

						for (auto& t : f.Foot) {
							t.pos = VECTOR_ref::vget(t.pos.x(), t.Pos().y, t.Pos().x);
						}
					}
					{
						i = 0;
						for (const auto& w : this->use_veh->Get_b2upsideframe(LorR)) {
							if (w.second.x() * LR_t > 0) {
								auto& t = f.Foot[i++];
								t.pos = VECTOR_ref::vget(w.second.x(), t.pos.y(), t.pos.z());
								GetObj().SetFrameLocalMatrix(w.first, MATRIX_ref::Mtrans(t.pos));
							}
						}
					}
					LorR++;
				}

				GetObj().SetMatrix(this->m_move.MatIn());

				for (auto& g : this->b2downsideframe) {
					for (auto& t : g) {
						if (t.colres.HitFlag == TRUE) {
							Easing(&t.gndsmksize, 0.1f + std::abs(this->speed - this->spd_rec) / ((0.01f / 3.6f) / FPS) * 0.5f, 0.975f, EasingType::OutExpo);
						}
						else {
							t.gndsmksize = 0.1f;
						}
						t.gndsmkeffcs.handle.SetPos(GetObj().frame(t.frame.first) + this->m_move.mat.yvec() * (-t.frame.second.y()));
						t.gndsmkeffcs.handle.SetScale(std::clamp(t.gndsmksize, 0.1f, 0.5f)*Scale_Rate);
					}
				}
			}
		public: //コンストラクタ、デストラクタ
			VehicleClass(void) noexcept { this->m_objType = ObjType::Vehicle; }
			~VehicleClass(void) noexcept {}
		public: //継承
			void			ValueSet(const moves& pMove, VhehicleData* pVeh_data, const MV1& hit_pic, std::shared_ptr<b2World>& pB2World) {
				this->m_move = pMove;
				this->use_veh = pVeh_data;
				//弾痕
				Hit_active.Set(hit_pic);
				//戦車set
				Init_Tank(pB2World);

				this->m_PosBufOverRideFlag = false;
				this->m_PosBufOverRide = this->m_move.pos;
				this->m_VecBufOverRide.clear();

				this->xrad_Add = 0;
				this->yrad_Add = 0;
			}
			//
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_IsDraw = true;
			}
			//
			void			ExecuteRadBuf(const InputControl& pInput) {
				//回転
				{
					this->m_rad_Buf.x(std::clamp(this->m_rad_Buf.x() + pInput.GetAddxRad(), -deg2rad(60.f), deg2rad(60.f)));
					this->m_rad_Buf.y(this->m_rad_Buf.y() + pInput.GetAddyRad());
				}
			}
			const auto		GetRadBuf(void) const noexcept { return this->m_rad_Buf; }

			void			SetInput(const InputControl& pInput, bool pReady) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;

				auto xPrev = this->xrad_p;
				auto yPrev = this->yrad_p;
				this->xrad_p = std::clamp(pInput.GetxRad(), deg2rad(-60), deg2rad(60));				//x軸回転(仰俯)
				this->yrad_p = pInput.GetyRad();
				this->xrad_Add = this->xrad_p - xPrev;
				this->yrad_Add = this->yrad_p - yPrev;

				key[0] = pInput.GetAction5() && pReady;			//射撃
				key[1] = false && pReady;						//マシンガン
				key[2] = pInput.GetGoFrontPress() && pReady;	//前進
				key[3] = pInput.GetGoBackPress() && pReady;		//後退
				key[4] = pInput.GetGoRightPress() && pReady;	//右
				key[5] = pInput.GetGoLeftPress() && pReady;		//左
			}
			/*カメラ指定*/
			void			Setcamera(cam_info& camera_main, const float fov_base) noexcept {
				VECTOR_ref eyepos, eyetgt;
				auto OLD = range;
				if (is_ADS()) {
					eyepos = Get_EyePos_Base();
					eyetgt = eyepos + (GetObj().frame(Guninfo(0).Get_frame(2).first) - eyepos) * 1.f;

					ratio = std::clamp(ratio + float(GetMouseWheelRotVol()) * 2.0f, 0.0f, 10.f);
					if (ratio == 0.f) {
						range = 0.f + range_change;
						range_r = range;
					}
					else {
						Easing(&camera_main.fov, fov_base / ratio, 0.9f, EasingType::OutExpo);
					}
				}
				else {
					ratio = 2.0f;
					range = std::clamp(range - float(GetMouseWheelRotVol()) * range_change, 0.f, 9.f);

					Easing(&range_r, range, 0.8f, EasingType::OutExpo);
					eyepos = Get_EyePos_Base() + lookvec.zvec() * range_r*Scale_Rate;
					eyetgt = eyepos + lookvec.zvec() * (-range_r * Scale_Rate);
					{
						bool ans = false;
						while (true) {
							auto colres = m_MapCol->CollCheck_Line(eyepos, eyetgt);
							if (colres.HitFlag == TRUE) {
								ans = true;
								if (eyetgt == colres.HitPosition) { break; }
								eyetgt = colres.HitPosition;
							}
							else {
								break;
							}
						}
						if (ans) {
							eyepos = eyetgt;
						}
					}
					eyetgt = eyepos + lookvec.zvec() * (-std::max(range_r*Scale_Rate, 1.f));
					Easing(&camera_main.fov, fov_base, 0.9f, EasingType::OutExpo);
				}
				changeview = ((range != OLD) && (range == 0.f || OLD == 0.f));
				camera_main.set_cam_pos(eyepos, eyetgt, this->m_move.mat.yvec());

				Easing(&camera_main.near_, 10.f + 2.f*((is_ADS()) ? ratio : 0.f), 0.9f, EasingType::OutExpo);
				Easing(&camera_main.far_, std::max(dist, Scale_Rate * 20.f) + Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);//Scale_Rate * 200.f
			}
			//
			void			Execute(void) noexcept override {
				Damage.Update();
				if (Damage.Get_alive()) {
					{
						if (false) { //砲塔ロック
							view_yrad = 0.f;
							view_xrad = 0.f;
						}
						else {
							//狙い
							lookvec = MATRIX_ref::RotX(this->xrad_Add) * lookvec * MATRIX_ref::RotY(this->yrad_Add);
							//狙い
							{
								VECTOR_ref startpos = Get_EyePos_Base();
								VECTOR_ref endpos = startpos + (lookvec.zvec() * -1.f).Norm() * (100.f*Scale_Rate);
								{
									while (true) {
										auto colres = m_MapCol->CollCheck_Line(startpos, endpos);
										if (colres.HitFlag == TRUE) {
											if (endpos == colres.HitPosition) { break; }
											endpos = colres.HitPosition;
										}
										else {
											break;
										}
									}
								}
								//反映
								auto vec_a = (GetObj().frame(Guninfo(0).Get_frame(1).first) - endpos).Norm();
								auto vec_z = (GetObj().frame(Guninfo(0).Get_frame(2).first) - GetObj().frame(Guninfo(0).Get_frame(1).first));
								float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
								float cost = vec_a.cross(vec_z).y() / (z_hyp);
								view_yrad = (atan2f(cost, sqrtf(std::abs(1.f - cost * cost)))) / ((is_ADS()) ? 2.f : 5.f);
								view_xrad = (atan2f(-vec_z.y(), z_hyp) - atan2f(vec_a.y(), 1.f)) / ((is_ADS()) ? 2.f : 5.f);
							}
						}
					}
				}
				else {
					key[0] = false;	//射撃
					key[1] = false;	//マシンガン
					key[2] = false;	//前進
					key[3] = false;	//後退
					key[4] = false;	//右
					key[5] = false;	//左
					view_yrad = 0.f;
					view_xrad = 0.f;
				}
				this->nearhit = false;
				//戦車演算
				Update_Tank();
			}
			void			Execute_After(void) noexcept {
				//戦車演算
				{
					auto OldPos = this->m_move.pos;
					//戦車座標反映
					Update_Tank2();
					this->add_vec_real = this->m_move.pos - OldPos;
					this->m_col.SetMatrix(GetObj().GetMatrix());
				}
				//エンジン音
				if (this->engine_time == 0.f) {
					auto SE = SoundPool::Instance();
					SE->Get((int)SoundEnum::Tank_engine).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 32);//, DX_PLAYTYPE_LOOP
					this->engine_time = 1.f;
				}
				else {
					this->engine_time -= 1.f / FPS;
					if (this->engine_time <= 0.f) {
						this->engine_time = 0.f;
					}
				}
				//エフェクトの処理
				Effect_UseControl::Update_Effect();
				//弾痕
				Hit_active.Update(this->m_move);
			}
			//
			void			Draw(void) noexcept override {
				if (!is_ADS()) {
					MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
					GetObj().DrawMesh(0);
					MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
					GetObj().DrawMesh(1);
					MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
					for (int i = 2; i < GetObj().mesh_num(); i++) {
						GetObj().DrawMesh(i);
					}
					//m_col.DrawModel();
					Hit_active.Draw();
				}
				Draw_LAM_Effect();
				Draw_ammo();
			}
			void			Draw_LAM_Effect(void) noexcept {
				auto StartPos = GetObj().frame(Guninfo(0).Get_frame(2).first);
				auto EndPos = StartPos + (StartPos - GetObj().frame(Guninfo(0).Get_frame(1).first)).Norm() * 100.f*Scale_Rate;
				{
					while (true) {
						auto colres = m_MapCol->CollCheck_Line(StartPos, EndPos);
						if (colres.HitFlag == TRUE) {
							if (EndPos == colres.HitPosition) { break; }
							EndPos = colres.HitPosition;
						}
						else {
							break;
						}
					}
				}
				for (auto& tgt : *ALL_v) {
					if ((tgt == *MINE_v) || !tgt->Damage.Get_alive()) { continue; }
					if (tgt->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) {
						tgt->col_nearest(StartPos, &EndPos);
					}
				}
				dist = (StartPos - EndPos).size();
				//if (!is_ADS())
				{
					SetUseLighting(FALSE);
					//SetFogEnable(FALSE);
					{
						float laser_siz = 0.01f;
						DrawCapsule_3D(StartPos, EndPos, laser_siz*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
						DrawSphere_3D(EndPos, std::clamp(powf((EndPos - GetCameraPosition()).size() + 0.5f, 2) * 0.2f + 8.f, 0.1f, 5.f)*laser_siz*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
					}
					SetUseLighting(TRUE);
					//SetFogEnable(TRUE);
				}
			}
			void			Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
				for (auto& cg : this->Gun_) { cg.Draw_Hit_UI(hit_Graph); }
			}
			void			Draw_ammo(void) noexcept {
				for (auto& cg : this->Gun_) { cg.Draw_ammo(); }
			}
			//
			void			Dispose(void) noexcept override {
				ObjectBaseClass::Dispose();
				this->b2mine.Dispose();			/*Box2D*/
				for (auto& t : foot) {
					for (auto& f : t.Foot) { f.Dispose(); }
					t.Foot.clear();
					for (auto& f : t.Wheel) { f.Dispose(); }
					t.Wheel.clear();
					for (auto& f : t.Yudo) { f.Dispose(); }
					t.Yudo.clear();
				}
				for (auto& g : this->b2downsideframe) {
					for (auto& t : g) {
						t.gndsmkeffcs.handle.Dispose();
					}
					g.clear();
				}
				for (auto& cg : this->Gun_) { cg.Dispose(); }
				Effect_UseControl::Dispose_Effect();
				Hit_active.Dispose();
				this->speed = 0.f;
				this->speed_add = 0.f;
				this->speed_sub = 0.f;
				this->wheel_Left = 0.f;
				this->wheel_Right = 0.f;
				this->wheel_Leftadd = 0.f;
				this->wheel_Rightadd = 0.f;
				this->m_rad.clear();
				this->radAdd.clear();
				this->yradadd_left = 0.f;
				this->yradadd_right = 0.f;
				for (auto& f : foot) { f.world.reset(); }
				for (auto& g : this->Gun_) { g.Reset(); }
				this->Gun_.clear();
				this->Damage.Dispose();
				hitControl.Reset();
			}
		};

	};
};
