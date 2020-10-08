#pragma once

class GRASS {
private:
	int grasss = 50;				/*grass�̐�*/
	std::vector<VERTEX3D> grassver; /*grass*/
	std::vector<DWORD> grassind;    /*grass*/
	int VerBuf, IndexBuf;			/*grass*/
	MV1 grass;						/*grass���f��*/
	GraphHandle graph;				/*�摜�n���h��*/
	int IndexNum, VerNum;			/*grass*/
	int vnum, pnum;					/*grass*/
	MV1_REF_POLYGONLIST RefMesh;	/*grass*/
public:
	GRASS(void) {
	}
	void set_grass_ready_before(void) {
		SetUseASyncLoadFlag(FALSE);
		graph = GraphHandle::Load("data/map/grass/grass.png");		 /*grass*/
		MV1::Load("data/map/grass/grass.mv1", &grass, false);		/*grass*/
		SetUseASyncLoadFlag(FALSE);
		return;
	}
	void set_grass_cancel(void) {
		SetASyncLoadFinishDeleteFlag(graph.get());										 /*grass*/
		SetASyncLoadFinishDeleteFlag(grass.get());		/*grass*/
	}
	bool set_grass_ready(const char* buf, const float x_max = 10.f, const float z_max = 10.f, const float x_min = -10.f, const float z_min = -10.f) {
		/*grass*/
		MV1SetupReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̍쐬*/
		RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
		IndexNum = RefMesh.PolygonNum * 3 * grasss; /*�C���f�b�N�X�̐����擾*/
		VerNum = RefMesh.VertexNum * grasss;	/*���_�̐����擾*/
		grassver.resize(VerNum);   /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
		grassind.resize(IndexNum); /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/

		vnum = 0;
		pnum = 0;

		int grass_pos = LoadSoftImage(buf);
		int xs = 0, ys = 0;
		GetSoftImageSize(grass_pos, &xs, &ys);

		for (int i = 0; i < grasss; ++i) {

			float x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
			float z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
			int _r_, _g_, _b_, _a_;
			while (1) {
				GetPixelSoftImage(grass_pos, int((x_t - x_min) / (x_max - x_min)*float(xs)), int((z_t - z_min) / (z_max - z_min)*float(ys)), &_r_, &_g_, &_b_, &_a_);
				if (_r_ <= 128) {
					break;
				}
				else {
					x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
					z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
				}
			}
			VECTOR_ref tmpvect = VGet(x_t, 0.0f, z_t);
			//
			MV1SetMatrix(grass.get(), MMult(MMult(MGetRotY(deg2rad(GetRand(90))), MGetRotVec2(VGet(0, 1.f, 0), VGet(0, 1.f, 0))), MGetTranslate(tmpvect.get())));
			//���
			MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*�Q�Ɨp���b�V���̍X�V*/
			RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
			for (int j = 0; j < RefMesh.VertexNum; ++j) {
				auto& g = grassver[j + vnum];
				g.pos = RefMesh.Vertexs[j].Position;
				g.norm = RefMesh.Vertexs[j].Normal;
				g.dif = RefMesh.Vertexs[j].DiffuseColor;
				g.spc = RefMesh.Vertexs[j].SpecularColor;
				g.u = RefMesh.Vertexs[j].TexCoord[0].u;
				g.v = RefMesh.Vertexs[j].TexCoord[0].v;
				g.su = RefMesh.Vertexs[j].TexCoord[1].u;
				g.sv = RefMesh.Vertexs[j].TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(RefMesh.Polygons[j].VIndex); ++k)
					grassind[j * 3 + k + pnum] = WORD(RefMesh.Polygons[j].VIndex[k] + vnum);
			}
			vnum += RefMesh.VertexNum;
			pnum += RefMesh.PolygonNum * 3;
		}
		VerBuf = CreateVertexBuffer(VerNum, DX_VERTEX_TYPE_NORMAL_3D);
		IndexBuf = CreateIndexBuffer(IndexNum, DX_INDEX_TYPE_32BIT);
		SetVertexBufferData(0, grassver.data(), VerNum, VerBuf);
		SetIndexBufferData(0, grassind.data(), IndexNum, IndexBuf);
		return true;
	}
	void draw_grass(void) {
		DrawPolygonIndexed3D_UseVertexBuffer(VerBuf, IndexBuf, graph.get(), TRUE);
	}
	void delete_grass(void) {
		graph.Dispose();
		grass.Dispose();
		grassver.clear();
		grassind.clear();
	}

};
class Chara {
	class sendstat {

	public:
		MV1::ani anime[9];
		MATRIX_ref bodys_f;

		MATRIX_ref head_f;
		MATRIX_ref RIGHTarm1_f;
		MATRIX_ref RIGHTarm2_f;
		MATRIX_ref RIGHThand_f;
		MATRIX_ref LEFTarm1_f;
		MATRIX_ref LEFTarm2_f;
		MATRIX_ref LEFThand_f;
		MATRIX_ref bodyg_f;
		MATRIX_ref bodyb_f;
		MATRIX_ref body_f;
		bool start_c = true;

		void get_data(Chara& data) {
			for (int i = 0; i < 9; i++) {
				this->anime[i].per = data.body.get_anime(i).per;
				this->anime[i].time = data.body.get_anime(i).time;
			}
			this->bodys_f = data.body.GetMatrix();
			this->head_f = data.body.GetFrameLocalMatrix(data.head_f.first);
			this->RIGHTarm1_f = data.body.GetFrameLocalMatrix(data.RIGHTarm1_f.first);
			this->RIGHTarm2_f = data.body.GetFrameLocalMatrix(data.RIGHTarm2_f.first);
			this->RIGHThand_f = data.body.GetFrameLocalMatrix(data.RIGHThand_f.first);
			this->LEFTarm1_f = data.body.GetFrameLocalMatrix(data.LEFTarm1_f.first);
			this->LEFTarm2_f = data.body.GetFrameLocalMatrix(data.LEFTarm2_f.first);
			this->LEFThand_f = data.body.GetFrameLocalMatrix(data.LEFThand_f.first);
			this->bodyg_f = data.body.GetFrameLocalMatrix(data.bodyg_f.first);
			this->bodyb_f = data.body.GetFrameLocalMatrix(data.bodyb_f.first);
			this->body_f = data.body.GetFrameLocalMatrix(data.body_f.first);
		}
		void put_data(Chara& data) {
			for (int i = 0; i < 9; i++) {
				data.body.get_anime(i).per = this->anime[i].per;
				data.body.get_anime(i).time = this->anime[i].time;
			}
			data.body.SetMatrix(this->bodys_f*MATRIX_ref::Mtrans(VGet(0, 0, 1.f)));
			data.body.SetFrameLocalMatrix(data.head_f.first, this->head_f);
			data.body.SetFrameLocalMatrix(data.RIGHTarm1_f.first, this->RIGHTarm1_f);
			data.body.SetFrameLocalMatrix(data.RIGHTarm2_f.first, this->RIGHTarm2_f);
			data.body.SetFrameLocalMatrix(data.RIGHThand_f.first, this->RIGHThand_f);
			data.body.SetFrameLocalMatrix(data.LEFTarm1_f.first, this->LEFTarm1_f);
			data.body.SetFrameLocalMatrix(data.LEFTarm2_f.first, this->LEFTarm2_f);
			data.body.SetFrameLocalMatrix(data.LEFThand_f.first, this->LEFThand_f);
			data.body.SetFrameLocalMatrix(data.bodyg_f.first, this->bodyg_f);
			data.body.SetFrameLocalMatrix(data.bodyb_f.first, this->bodyb_f);
			data.body.SetFrameLocalMatrix(data.body_f.first, this->body_f);
		}
	};
public:
	//���p
	class Vehcs {
	public:
		//����
		std::string name;				  /**/
		MV1 obj, col;					  /**/
		VECTOR_ref minpos, maxpos;			  /**/
		std::vector<gun_frame> gunframe;			  /**/
		std::vector<foot_frame> wheelframe;			  /**/
		std::vector<foot_frame> wheelframe_nospring;		  /*�U���։�]*/
		uint16_t HP = 0;					  /**/
		std::vector<std::pair<size_t, float>> armer_mesh; /*���bID*/
		std::vector<size_t> space_mesh;			  /*���bID*/
		std::vector<size_t> module_mesh;		  /*���bID*/
		int camo_tex = 0;				  /**/
		std::vector<int> camog;				  /**/
		bool isfloat = false;			  /*�������ǂ���*/
		float down_in_water = 0.f;			  /*���ޔ���ӏ�*/
		float max_speed_limit = 0.f;			  /*�ō����x(km/h)*/
		float mid_speed_limit = 0.f;			  /*���s���x(km/h)*/
		float min_speed_limit = 0.f;			  /*�������x(km/h)*/
		float flont_speed_limit = 0.f;			  /*�O�i���x(km/h)*/
		float back_speed_limit = 0.f;			  /*��ޑ��x(km/h)*/
		float body_rad_limit = 0.f;			  /*���񑬓x(�x/�b)*/
		float turret_rad_limit = 0.f;			  /*�C���쓮���x(�x/�b)*/
		frames fps_view;//�R�b�N�s�b�g
		GraphHandle ui_pic;//�V���G�b�g
		int pic_x, pic_y;//�T�C�Y
		//���
		std::array<int, 4> square{ 0 };//���p�̎l��
		std::array<std::vector<frames>, 2> b2upsideframe; /*���я�*/
		std::array<std::vector<frames>, 2> b2downsideframe; /*���я�*/
		std::vector<frames> burner;//�A�t�^�[�o�[�i�[
		frames hook;//���̓t�b�N

		std::vector<frames> wire;
		std::vector<frames> catapult;
		//
		void into(const Vehcs& t) {
			this->wheelframe.clear();
			for (auto& p : t.wheelframe) {
				this->wheelframe.resize(this->wheelframe.size() + 1);
				this->wheelframe.back().frame = p.frame;
			}
			this->wheelframe_nospring.clear();
			for (auto& p : t.wheelframe_nospring) {
				this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
				this->wheelframe_nospring.back().frame = p.frame;
			}
			this->name = t.name;
			this->minpos = t.minpos;
			this->maxpos = t.maxpos;
			this->gunframe = t.gunframe;
			this->HP = t.HP;
			this->armer_mesh = t.armer_mesh;
			this->space_mesh = t.space_mesh;
			this->module_mesh = t.module_mesh;
			this->camo_tex = t.camo_tex;
			this->camog = t.camog;
			this->isfloat = t.isfloat;
			this->down_in_water = t.down_in_water;
			this->max_speed_limit = t.max_speed_limit;
			this->mid_speed_limit = t.mid_speed_limit;
			this->min_speed_limit = t.min_speed_limit;
			this->flont_speed_limit = t.flont_speed_limit;
			this->back_speed_limit = t.back_speed_limit;
			this->body_rad_limit = t.body_rad_limit;
			this->turret_rad_limit = t.turret_rad_limit;
			this->square = t.square;
			this->b2upsideframe = t.b2upsideframe;
			this->b2downsideframe = t.b2downsideframe;
			this->burner = t.burner;
			this->hook = t.hook;
			this->fps_view = t.fps_view;

			this->wire = t.wire;
			this->catapult = t.catapult;

			this->ui_pic = t.ui_pic.Duplicate();
			this->pic_x = t.pic_x;
			this->pic_y = t.pic_y;
		}
		//���O�ǂݍ���
		static void set_vehicles_pre(const char* name, std::vector<Mainclass::Vehcs>* veh_, const bool& Async) {
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((std::string(name) + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						veh_->resize(veh_->size() + 1);
						veh_->back().name = win32fdt.cFileName;
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
			for (auto& t : *veh_) {
				MV1::Load(std::string(name) + t.name + "/model.mv1", &t.obj, Async);
				MV1::Load(std::string(name) + t.name + "/col.mv1", &t.col, Async);
				t.ui_pic = GraphHandle::Load(std::string(name) + t.name + "/pic.png");
			}
		}
		//���C���ǂݍ���
		template <size_t N>
		static void set_vehicles(std::array<std::vector<Mainclass::Vehcs>, N>* vehcs) {
			using namespace std::literals;
			//����
			for (auto& veh : (*vehcs)) {
				for (auto& t : veh) {
					//���e�X�g
					t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
					//
					GetGraphSize(t.ui_pic.get(), &t.pic_x, &t.pic_y);
				}
			}
			//�ŗL
			for (auto& t : (*vehcs)[0]) {
				t.down_in_water = 0.f;
				for (int i = 0; i < t.obj.mesh_num(); i++) {
					auto p = t.obj.mesh_maxpos(i).y();
					if (t.down_in_water < p) {
						t.down_in_water = p;
					}
				}
				t.down_in_water /= 2.f;
				for (int i = 0; i < t.obj.frame_num(); i++) {
					std::string p = t.obj.frame_name(i);
					if (p.find("�]��", 0) != std::string::npos) {
						t.wheelframe.resize(t.wheelframe.size() + 1);
						t.wheelframe.back().frame = { i,t.obj.frame(i) };
					}
					else if ((p.find("��", 0) != std::string::npos) && (p.find("�]��", 0) == std::string::npos)) {
						t.wheelframe_nospring.resize(t.wheelframe_nospring.size() + 1);
						t.wheelframe_nospring.back().frame = { i,t.obj.frame(i) };
					}
					else if (p.find("����", 0) != std::string::npos) {
						t.gunframe.resize(t.gunframe.size() + 1);
						auto& b = t.gunframe.back();
						b.frame1 = { i,t.obj.frame(i) };
						auto p2 = t.obj.frame_parent(b.frame1.first);
						if (p2 >= 0) {
							b.frame1.second -= t.obj.frame(int(p2)); //�e�����鎞�����Ƃ�
						}
						if (t.obj.frame_child_num(b.frame1.first) > 0) {
							if (t.obj.frame_name(b.frame1.first + 1).find("�p", 0) != std::string::npos) {
								b.frame2 = { b.frame1.first + 1,t.obj.frame(b.frame1.first + 1) - t.obj.frame(b.frame1.first) };
								if (t.obj.frame_child_num(b.frame1.first) > 0) {
									b.frame3 = { b.frame2.first + 1,t.obj.frame(b.frame2.first + 1) - t.obj.frame(b.frame2.first) };
								}
								else {
									b.frame3.first = -1;
								}
							}
						}
						else {
							b.frame2.first = -1;
						}
					}
					else if (p.find("min", 0) != std::string::npos) {
						t.minpos = t.obj.frame(i);
					}
					else if (p.find("max", 0) != std::string::npos) {
						t.maxpos = t.obj.frame(i);
					}
					else if (p.find("�QD����", 0) != std::string::npos || p.find("2D����", 0) != std::string::npos) { //2D����
						t.b2upsideframe[0].clear();
						t.b2upsideframe[1].clear();
						for (int z = 0; z < t.obj.frame_child_num(i); z++) {
							if (t.obj.frame(i + 1 + z).x() > 0) {
								t.b2upsideframe[0].emplace_back(i + 1 + z, t.obj.frame(i + 1 + z));
							}
							else {
								t.b2upsideframe[1].emplace_back(i + 1 + z, t.obj.frame(i + 1 + z));
							}
						}
						std::sort(t.b2upsideframe[0].begin(), t.b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //�\�[�g
						std::sort(t.b2upsideframe[1].begin(), t.b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //�\�[�g
					}
					else if (p.find("���ѐݒu��", 0) != std::string::npos) { //2D����
						t.b2downsideframe[0].clear();
						t.b2downsideframe[1].clear();
						for (int z = 0; z < t.obj.frame_child_num(i); z++) {
							if (t.obj.frame(i + 1 + z).x() > 0) {
								t.b2downsideframe[0].emplace_back(i + 1 + z, t.obj.frame(i + 1 + z));
							}
							else {
								t.b2downsideframe[1].emplace_back(i + 1 + z, t.obj.frame(i + 1 + z));
							}
						}
					}
					else if (p.find("���_", 0) != std::string::npos) {
						t.fps_view.first = i;
						t.fps_view.second = t.obj.frame(t.fps_view.first);
					}

				}
				//2	���㕔0
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (f.frame.second.x() >= 0) {
							t.square[0] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[0] != f.frame.first) {
							if (f.frame.second.x() >= 0) {
								if (tmp < f.frame.second.z()) {
									t.square[0] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//10	���O��1
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (f.frame.second.x() >= 0) {
							t.square[1] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[1] != f.frame.first) {
							if (f.frame.second.x() >= 0) {
								if (tmp > f.frame.second.z()) {
									t.square[1] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//3	�E�㕔2
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (!(f.frame.second.x() >= 0)) {
							t.square[2] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[2] != f.frame.first) {
							if (!(f.frame.second.x() >= 0)) {
								if (tmp < f.frame.second.z()) {
									t.square[2] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//11	�E�O��3
				{
					float tmp = 0.f;
					for (auto& f : t.wheelframe) {
						if (!(f.frame.second.x() >= 0)) {
							t.square[3] = f.frame.first;
							tmp = f.frame.second.z();
							break;
						}
					}
					for (auto& f : t.wheelframe) {
						if (t.square[3] != f.frame.first) {
							if (!(f.frame.second.x() >= 0)) {
								if (tmp > f.frame.second.z()) {
									t.square[3] = f.frame.first;
									tmp = f.frame.second.z();
								}
							}
						}
					}
				}
				//���b
				for (int i = 0; i < t.col.mesh_num(); i++) {
					std::string p = t.col.material_name(i);
					if (p.find("armer", 0) != std::string::npos) {
						t.armer_mesh.emplace_back(i, std::stof(getright(p.c_str())));//���b
					}
					else if (p.find("space", 0) != std::string::npos) {
						t.space_mesh.emplace_back(i);//��ԑ��b
					}
					else {
						t.module_mesh.emplace_back(i);//���W���[��
					}
				}
				//����
				{
					t.camo_tex = -1;
					for (int i = 0; i < MV1GetTextureNum(t.obj.get()); i++) {
						std::string p = MV1GetTextureName(t.obj.get(), i);
						if (p.find("b.", 0) != std::string::npos || p.find("B.", 0) != std::string::npos) {
							t.camo_tex = i;
							break;
						}
					}
					SetUseTransColor(FALSE);
					WIN32_FIND_DATA win32fdt;
					HANDLE hFind;
					hFind = FindFirstFile(("data/tank/"s + t.name + "/B*.jpg").c_str(), &win32fdt);
					if (hFind != INVALID_HANDLE_VALUE) {
						do {
							if (win32fdt.cFileName[0] != '.') {
								t.camog.resize(t.camog.size() + 1);
								t.camog.back() = MV1LoadTexture(("data/tank/"s + t.name + "/" + win32fdt.cFileName).c_str());
							}
						} while (FindNextFile(hFind, &win32fdt));
					} //else{ return false; }
					FindClose(hFind);
					SetUseTransColor(TRUE);
				}
				//data
				{
					int mdata = FileRead_open(("data/tank/" + t.name + "/data.txt").c_str(), FALSE);
					char mstr[64]; /*tank*/
					t.isfloat = getparam_bool(mdata);
					t.flont_speed_limit = getparam_f(mdata);
					t.back_speed_limit = getparam_f(mdata);
					t.body_rad_limit = getparam_f(mdata);
					t.turret_rad_limit = getparam_f(mdata);
					t.HP = uint16_t(getparam_u(mdata));
					FileRead_gets(mstr, 64, mdata);
					for (auto& g : t.gunframe) {
						g.name = getright(mstr);
						g.load_time = getparam_f(mdata);
						g.rounds = uint16_t(getparam_u(mdata));
						while (true) {
							FileRead_gets(mstr, 64, mdata);
							if (std::string(mstr).find(("useammo" + std::to_string(g.useammo.size()))) == std::string::npos) {
								break;
							}
							g.useammo.emplace_back(getright(mstr));
						}
					}
					FileRead_close(mdata);
				}
			}
			for (auto& t : (*vehcs)[1]) {
				//
				t.down_in_water = 0.f;
				for (int i = 0; i < t.obj.mesh_num(); i++) {
					auto p = t.obj.mesh_minpos(i).y();
					if (t.down_in_water > p) {
						t.down_in_water = p;
					}
				}
				//t.down_in_water /= 2.f;
				//�ő�ŏ����擾
				for (int i = 0; i < t.obj.mesh_num(); i++) {
					if (t.maxpos.x() < t.obj.mesh_maxpos(i).x()) {
						t.maxpos.x(t.obj.mesh_maxpos(i).x());
					}
					if (t.maxpos.z() < t.obj.mesh_maxpos(i).z()) {
						t.maxpos.z(t.obj.mesh_maxpos(i).z());
					}
					if (t.minpos.x() > t.obj.mesh_minpos(i).x()) {
						t.minpos.x(t.obj.mesh_minpos(i).x());
					}
					if (t.minpos.z() > t.obj.mesh_minpos(i).z()) {
						t.minpos.z(t.obj.mesh_minpos(i).z());
					}
				}
				//�t���[��
				for (int i = 0; i < t.obj.frame_num(); i++) {
					std::string p = t.obj.frame_name(i);
					if (p.find("�r", 0) != std::string::npos) {
						if (p.find("�n�b�`", 0) == std::string::npos) {
							t.wheelframe.resize(t.wheelframe.size() + 1);
							t.wheelframe.back().frame.first = i;
							t.wheelframe.back().frame.second = t.obj.frame(t.wheelframe.back().frame.first);

							t.wheelframe_nospring.resize(t.wheelframe_nospring.size() + 1);
							t.wheelframe_nospring.back().frame.first = t.wheelframe.back().frame.first + 1;
							t.wheelframe_nospring.back().frame.second = t.obj.frame(t.wheelframe_nospring.back().frame.first) - t.wheelframe.back().frame.second;
						}
					}
					else if (p.find("����", 0) != std::string::npos) {
						t.gunframe.resize(t.gunframe.size() + 1);
						t.gunframe.back().frame1.first = i;
						t.gunframe.back().frame1.second = t.obj.frame(t.gunframe.back().frame1.first);
						auto p2 = t.obj.frame_parent(t.gunframe.back().frame1.first);
						if (p2 >= 0) {
							t.gunframe.back().frame1.second -= t.obj.frame(int(p2)); //�e�����鎞�����Ƃ�
						}
						if (t.obj.frame_child_num(t.gunframe.back().frame1.first) >= 0) {
							if (t.obj.frame_name(t.gunframe.back().frame1.first + 1).find("�p", 0) != std::string::npos) {
								t.gunframe.back().frame2.first = t.gunframe.back().frame1.first + 1;
								t.gunframe.back().frame2.second = t.obj.frame(t.gunframe.back().frame2.first) - t.obj.frame(t.gunframe.back().frame1.first);
								if (t.obj.frame_child_num(t.gunframe.back().frame1.first) >= 0) {
									t.gunframe.back().frame3.first = t.gunframe.back().frame2.first + 1;
									t.gunframe.back().frame3.second = t.obj.frame(t.gunframe.back().frame3.first) - t.obj.frame(t.gunframe.back().frame2.first);
								}
								else {
									t.gunframe.back().frame3.first = -1;
								}
							}
						}
						else {
							t.gunframe.back().frame2.first = -1;
						}
					}
					else if (p.find("�o�[�i�[", 0) != std::string::npos) {
						t.burner.resize(t.burner.size() + 1);
						t.burner.back().first = i;
						t.burner.back().second = t.obj.frame(t.burner.back().first);
					}
					else if (p.find("�t�b�N", 0) != std::string::npos) {
						t.hook.first = i;
						t.hook.second = t.obj.frame(t.hook.first);
					}
					else if (p.find("���_", 0) != std::string::npos) {
						t.fps_view.first = i;
						t.fps_view.second = t.obj.frame(t.fps_view.first);
					}
				}
				//���b�V��
				for (int i = 0; i < t.col.mesh_num(); i++) {
					std::string p = t.col.material_name(i);
					if (p.find("armer", 0) != std::string::npos) { //���b
						t.armer_mesh.resize(t.armer_mesh.size() + 1);
						t.armer_mesh.back().first = i;
						t.armer_mesh.back().second = std::stof(getright(p.c_str())); //���b�l
					}
					else if (p.find("space", 0) != std::string::npos) {		    //��ԑ��b
						t.space_mesh.resize(t.space_mesh.size() + 1);
						t.space_mesh.back() = i;
					}
					else { //���W���[��
						t.module_mesh.resize(t.module_mesh.size() + 1);
						t.module_mesh.back() = i;
					}
				}
				//����
				{
					t.camo_tex = -1;
					for (int i = 0; i < MV1GetTextureNum(t.obj.get()); i++) {
						std::string p = MV1GetTextureName(t.obj.get(), i);
						if (p.find("b.", 0) != std::string::npos || p.find("B.", 0) != std::string::npos) {
							t.camo_tex = i;
							break;
						}
					}

					//t.camo.resize(t.camo.size() + 1);
					{
						SetUseTransColor(FALSE);
						WIN32_FIND_DATA win32fdt;
						HANDLE hFind;
						hFind = FindFirstFile(("data/plane/"s + t.name + "/B*.jpg").c_str(), &win32fdt);
						if (hFind != INVALID_HANDLE_VALUE) {
							do {
								if (win32fdt.cFileName[0] != '.') {
									t.camog.resize(t.camog.size() + 1);
									t.camog.back() = LoadGraph(("data/plane/"s + t.name + "/" + win32fdt.cFileName).c_str());
								}
							} while (FindNextFile(hFind, &win32fdt));
						} //else{ return false; }
						FindClose(hFind);
						SetUseTransColor(TRUE);
					}
				}
				//�f�[�^�擾
				{
					int mdata = FileRead_open(("data/plane/" + t.name + "/data.txt").c_str(), FALSE);
					char mstr[64]; /*tank*/
					t.isfloat = getparam_bool(mdata);
					t.max_speed_limit = getparam_f(mdata) / 3.6f;
					t.mid_speed_limit = getparam_f(mdata) / 3.6f;
					t.min_speed_limit = getparam_f(mdata) / 3.6f;
					t.body_rad_limit = getparam_f(mdata);
					t.HP = uint16_t(getparam_u(mdata));
					FileRead_gets(mstr, 64, mdata);
					for (auto& g : t.gunframe) {
						g.name = getright(mstr);
						g.load_time = getparam_f(mdata);
						g.rounds = uint16_t(getparam_u(mdata));
						while (true) {
							FileRead_gets(mstr, 64, mdata);
							if (std::string(mstr).find(("useammo" + std::to_string(g.useammo.size()))) == std::string::npos) {
								break;
							}
							g.useammo.resize(g.useammo.size() + 1);
							g.useammo.back() = getright(mstr);
						}
					}
					FileRead_close(mdata);
				}
			}
			for (auto& t : (*vehcs)[2]) {

				for (int i = 0; i < t.obj.frame_num(); i++) {
					std::string p = t.obj.frame_name(i);
					if (p.find("ܲ԰", 0) != std::string::npos) {
						t.wire.resize(t.wire.size() + 1);
						t.wire.back().first = i;
						t.wire.back().second = t.obj.frame(t.wire.back().first);
					}
					else if (p.find("������", 0) != std::string::npos) {
						t.catapult.resize(t.catapult.size() + 1);
						t.catapult.back().first = i;
						t.catapult.back().second = t.obj.frame(t.catapult.back().first + 2) - t.obj.frame(t.catapult.back().first);
					}
				}
				for (int i = 0; i < t.col.mesh_num(); i++) {
					std::string p = t.col.material_name(i);
					if (p.find("armer", 0) != std::string::npos) { //���b
						t.armer_mesh.resize(t.armer_mesh.size() + 1);
						t.armer_mesh.back().first = i;
						t.armer_mesh.back().second = std::stof(getright(p.c_str())); //���b�l
					}
					else if (p.find("space", 0) != std::string::npos) {		    //��ԑ��b
						t.space_mesh.resize(t.space_mesh.size() + 1);
						t.space_mesh.back() = i;
					}
					else { //���W���[��
						t.module_mesh.resize(t.module_mesh.size() + 1);
						t.module_mesh.back() = i;
					}
				}


				VECTOR_ref size;
				for (int i = 0; i < t.col.mesh_num(); i++) {
					VECTOR_ref sizetmp = t.col.mesh_maxpos(i) - t.col.mesh_minpos(i);
					if (size.x() < sizetmp.x()) {
						size.x(sizetmp.x());
					}
					if (size.y() < sizetmp.y()) {
						size.y(sizetmp.y());
					}
					if (size.z() < sizetmp.z()) {
						size.z(sizetmp.z());
					}
				}

				/*
				for (int i = 0; i < t.col.mesh_num(); i++) {
					t.col.SetupCollInfo(int(size.x() / 5.f), int(size.y() / 5.f), int(size.z() / 5.f), 0, i);
				}
				*/
				//t.col.SetupCollInfo(int(size.x() / 5.f), int(size.y() / 5.f), int(size.z() / 5.f), 0, 0);
				//
				//t.obj.SetFrameLocalMatrix(t.catapult[0].first + 2, MATRIX_ref::RotX(deg2rad(-75)) * MATRIX_ref::Mtrans(t.catapult[0].second));

								//�f�[�^�擾
				{
					int mdata = FileRead_open(("data/carrier/" + t.name + "/data.txt").c_str(), FALSE);
					char mstr[64]; /*tank*/
					t.isfloat = getparam_bool(mdata);
					t.max_speed_limit = getparam_f(mdata) / 3.6f;
					t.mid_speed_limit = getparam_f(mdata) / 3.6f;
					t.min_speed_limit = getparam_f(mdata) / 3.6f;
					t.body_rad_limit = getparam_f(mdata);
					t.HP = uint16_t(getparam_u(mdata));
					FileRead_gets(mstr, 64, mdata);
					for (auto& g : t.gunframe) {
						g.name = getright(mstr);
						g.load_time = getparam_f(mdata);
						g.rounds = uint16_t(getparam_u(mdata));
						while (true) {
							FileRead_gets(mstr, 64, mdata);
							if (std::string(mstr).find(("useammo" + std::to_string(g.useammo.size()))) == std::string::npos) {
								break;
							}
							g.useammo.resize(g.useammo.size() + 1);
							g.useammo.back() = getright(mstr);
						}
					}
					FileRead_close(mdata);
				}
			}
		}
	};
	class vehicles {
	public:
		Vehcs use_veh;							      /*�ŗL�l*/
		MV1 obj;							      /**/
		size_t use_id = 0;						      //�g�p����ԗ�(�@��)
		uint16_t HP = 0;						      /*�̗�*/
		VECTOR_ref pos;							      //�ԑ̍��W
		MATRIX_ref mat;							      //�ԑ̉�]�s��
		VECTOR_ref add;							      //�ԑ̉����x
		float speed = 0.f, speed_add = 0.f, speed_sub = 0.f;		      /**/
		float xrad = 0.f, xradadd = 0.f, xradadd_left = 0.f, xradadd_right = 0.f; /**/
		float yrad = 0.f, yradadd = 0.f, yradadd_left = 0.f, yradadd_right = 0.f;	    /**/
		float zrad = 0.f, zradadd = 0.f, zradadd_left = 0.f, zradadd_right = 0.f; /**/
		float wheel_Left = 0.f, wheel_Right = 0.f;			      //�]�։�]
		float wheel_Leftadd = 0.f, wheel_Rightadd = 0.f;		      //�]�։�]
	public:
		void reset() {
			//*
			this->obj.Dispose();
			this->HP = 0;
			this->speed_add = 0.f;
			this->speed_sub = 0.f;
			this->speed = 0.f;
			this->add = VGet(0.f, 0.f, 0.f);
			this->wheel_Left = 0.f;
			this->wheel_Right = 0.f;
			this->wheel_Leftadd = 0.f;
			this->wheel_Rightadd = 0.f;
			this->xrad = 0.f;
			this->xradadd = 0.f;
			this->xradadd_left = 0.f;
			this->xradadd_right = 0.f;
			this->yrad = 0.f;
			this->yradadd = 0.f;
			this->yradadd_left = 0.f;
			this->yradadd_right = 0.f;
			this->zrad = 0.f;
			this->zradadd = 0.f;
			this->zradadd_left = 0.f;
			this->zradadd_right = 0.f;
			//*/
		}
	};
	vehicles vehicle;
	std::array<bool, 15> key{ false };    //�L�[
	std::array<p_animes, 6> p_animes_rudder;      //���_�[�A�j���[�V����


	sendstat senddata;
	/*���f���A����*/
	MV1 body;
	//�v���C���[���W�n
	VECTOR_ref pos;
	MATRIX_ref mat;
	//�������W�n
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	float add_ypos = 0.f;//���������x
	float body_xrad = 0.f;//���̊p�x
	float body_yrad = 0.f;//���̊p�x
	float body_zrad = 0.f;//���̊p�x
	frames head_f;
	float head_hight = 0.f;
	frames LEFTeye_f;
	frames RIGHTeye_f;
	//
	frames bodyg_f;
	frames bodyc_f;
	frames bodyb_f;
	frames body_f;
	//�E����W�n
	VECTOR_ref pos_RIGHTHAND;
	MATRIX_ref mat_RIGHTHAND;
	VECTOR_ref vecadd_RIGHTHAND, vecadd_RIGHTHAND_p;//
	frames RIGHThand2_f;
	frames RIGHThand_f;
	frames RIGHTarm2_f;
	frames RIGHTarm1_f;
	//������W�n
	VECTOR_ref pos_LEFTHAND;
	MATRIX_ref mat_LEFTHAND;
	frames LEFThand2_f;
	frames LEFThand_f;
	frames LEFTarm2_f;
	frames LEFTarm1_f;
	//
	bool start_c = true;

	void Ready_chara(MV1& hand_) {
		//��
		hand_.DuplicateonAnime(&this->body);
		for (int i = 0; i < int(this->body.frame_num()); i++) {
			std::string p = this->body.frame_name(i);
			if (p == std::string("�O���[�u")) {
				bodyg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p == std::string("�����g")) {
				bodyc_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("����") != std::string::npos) {

			}
			else if (p.find("���Ђ�") != std::string::npos) {

			}
			else if (p.find("������") != std::string::npos) {

			}
			else if (p.find("�������") != std::string::npos) {

			}
			else if (p.find("�E��") != std::string::npos) {

			}
			else if (p.find("�E�Ђ�") != std::string::npos) {

			}
			else if (p.find("�E����") != std::string::npos) {

			}
			else if (p.find("�E�����") != std::string::npos) {

			}
			else if (p.find("�����g��") != std::string::npos) {

			}
			else if (p.find("�㔼�g") != std::string::npos && p.find("�㔼�g2") == std::string::npos) {
				bodyb_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("�㔼�g2") != std::string::npos) {
				body_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("��") != std::string::npos && p.find("��") == std::string::npos) {

			}
			else if (p.find("��") != std::string::npos && p.find("��") == std::string::npos) {
				head_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				head_hight = this->body.frame(head_f.first).y();
			}
			else if (p.find("�E�ڐ�") != std::string::npos) {
				RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("���ڐ�") != std::string::npos) {
				LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("����") != std::string::npos) {

			}
			else if (p.find("�E��") != std::string::npos) {

			}
			else if (p.find("�E�r") != std::string::npos && p.find("��") == std::string::npos) {
				RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("�E�r��") != std::string::npos) {
			}
			else if (p.find("�E�Ђ�") != std::string::npos) {
				RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("�E�蝀") != std::string::npos) {
			}
			else if (p == std::string("�E���")) {
				RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("�E����") != std::string::npos) {
				RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("����") != std::string::npos) {

			}
			else if (p.find("���r") != std::string::npos && p.find("��") == std::string::npos) {
				LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("���r��") != std::string::npos) {

			}
			else if (p.find("���Ђ�") != std::string::npos) {
				LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("���蝀") != std::string::npos) {

			}
			else if (p == std::string("�����")) {
				LEFThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("������") != std::string::npos) {
				LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
		}
	}
	void Set_chara_Position(const VECTOR_ref& pos_, const MATRIX_ref& mat_, const MATRIX_ref& mat_H) {
		this->pos = pos_;
		this->mat = mat_;
		this->mat_HMD = mat_H;
	}
	void Draw_chara() {
		this->body.DrawModel();
	}
	void Delete_chara() {
	}
	void Init_chara() {
		this->body.Dispose();
		Delete_chara();
	}
};
