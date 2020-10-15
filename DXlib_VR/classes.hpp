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
public:
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
	frames RIGHTarm0_f;
	//������W�n
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
				RIGHTarm0_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
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
				LEFTarm0_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
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
