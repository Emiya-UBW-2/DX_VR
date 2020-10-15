#pragma once

class GRASS {
private:
	int grasss = 50;				/*grassの数*/
	std::vector<VERTEX3D> grassver; /*grass*/
	std::vector<DWORD> grassind;    /*grass*/
	int VerBuf, IndexBuf;			/*grass*/
	MV1 grass;						/*grassモデル*/
	GraphHandle graph;				/*画像ハンドル*/
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
		MV1SetupReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの作成*/
		RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの取得*/
		IndexNum = RefMesh.PolygonNum * 3 * grasss; /*インデックスの数を取得*/
		VerNum = RefMesh.VertexNum * grasss;	/*頂点の数を取得*/
		grassver.resize(VerNum);   /*頂点データとインデックスデータを格納するメモリ領域の確保*/
		grassind.resize(IndexNum); /*頂点データとインデックスデータを格納するメモリ領域の確保*/

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
			//上省
			MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*参照用メッシュの更新*/
			RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの取得*/
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
public:
	/*モデル、音声*/
	MV1 body;
	//プレイヤー座標系
	VECTOR_ref pos;
	MATRIX_ref mat;
	//頭部座標系
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	float add_ypos = 0.f;//垂直加速度
	float body_xrad = 0.f;//胴体角度
	float body_yrad = 0.f;//胴体角度
	float body_zrad = 0.f;//胴体角度
	frames head_f;
	float head_hight = 0.f;
	frames LEFTeye_f;
	frames RIGHTeye_f;
	//
	frames bodyg_f;
	frames bodyc_f;
	frames bodyb_f;
	frames body_f;
	//右手座標系
	VECTOR_ref pos_RIGHTHAND;
	MATRIX_ref mat_RIGHTHAND;
	VECTOR_ref vecadd_RIGHTHAND, vecadd_RIGHTHAND_p;//
	frames RIGHThand2_f;
	frames RIGHThand_f;
	frames RIGHTarm2_f;
	frames RIGHTarm1_f;
	frames RIGHTarm0_f;
	//左手座標系
	VECTOR_ref pos_LEFTHAND;
	MATRIX_ref mat_LEFTHAND;
	frames LEFThand2_f;
	frames LEFThand_f;
	frames LEFTarm2_f;
	frames LEFTarm1_f;
	frames LEFTarm0_f;
	//
	VECTOR_ref pos_WAIST;
	MATRIX_ref mat_WAIST;
	//
	bool start_c = true;

	void Ready_chara(MV1& hand_) {
		//手
		hand_.DuplicateonAnime(&this->body);
		for (int i = 0; i < int(this->body.frame_num()); i++) {
			std::string p = this->body.frame_name(i);
			if (p == std::string("グルーブ")) {
				bodyg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p == std::string("下半身")) {
				bodyc_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左足") != std::string::npos) {

			}
			else if (p.find("左ひざ") != std::string::npos) {

			}
			else if (p.find("左足首") != std::string::npos) {

			}
			else if (p.find("左足首先") != std::string::npos) {

			}
			else if (p.find("右足") != std::string::npos) {

			}
			else if (p.find("右ひざ") != std::string::npos) {

			}
			else if (p.find("右足首") != std::string::npos) {

			}
			else if (p.find("右足首先") != std::string::npos) {

			}
			else if (p.find("下半身先") != std::string::npos) {

			}
			else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
				bodyb_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("上半身2") != std::string::npos) {
				body_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("首") != std::string::npos && p.find("手") == std::string::npos) {

			}
			else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
				head_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				head_hight = this->body.frame(head_f.first).y();
			}
			else if (p.find("右目先") != std::string::npos) {
				RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左目先") != std::string::npos) {
				LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("頭先") != std::string::npos) {

			}
			else if (p.find("右肩") != std::string::npos) {
				RIGHTarm0_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
				RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右腕捩") != std::string::npos) {
			}
			else if (p.find("右ひじ") != std::string::npos) {
				RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右手捩") != std::string::npos) {
			}
			else if (p == std::string("右手首")) {
				RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右手首先") != std::string::npos) {
				RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左肩") != std::string::npos) {
				LEFTarm0_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
				LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左腕捩") != std::string::npos) {

			}
			else if (p.find("左ひじ") != std::string::npos) {
				LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左手捩") != std::string::npos) {

			}
			else if (p == std::string("左手首")) {
				LEFThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左手首先") != std::string::npos) {
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
