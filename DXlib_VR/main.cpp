#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

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
	bool set_grass_ready(const char* buf,const float x_max=10.f,const float z_max = 10.f, const float x_min = -10.f, const float z_min = -10.f) {
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
		int xs = 0,ys = 0;
		GetSoftImageSize(grass_pos, &xs, &ys);

		for (int i = 0; i < grasss; ++i) {

			float x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
			float z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
			int _r_, _g_, _b_, _a_;
			while (1) {
				GetPixelSoftImage(grass_pos, int((x_t - x_min)/ (x_max - x_min)*float(xs)), int((z_t - z_min) / (z_max - z_min)*float(ys)), &_r_, &_g_, &_b_, &_a_);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE, true);	/*�ėp�N���X*/
	auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);	/*�f�o�b�O�N���X*/
	auto grassparts = std::make_unique<GRASS>();	/*���N���X*/
	//
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	VECTOR_ref pos_HAND1;
	MATRIX_ref mat_HAND1;
	VECTOR_ref pos_HAND2;
	MATRIX_ref mat_HAND2;
	float rad = 0.f;

	VECTOR_ref pos_track0;
	MATRIX_ref mat_track0;
	VECTOR_ref pos_track1;
	MATRIX_ref mat_track1;
	VECTOR_ref pos_track2;
	MATRIX_ref mat_track2;


	VECTOR_ref pos_;
	MATRIX_ref mat_;

	DXDraw::cam_info cams;

	MV1 map_mod;
	MV1::Load("data/map/model.mv1", &map_mod, false);
	map_mod.SetMatrix(MGetIdent());

	MV1 sky_mod;
	MV1::Load("data/map/sky/model.mv1", &sky_mod, false);
	sky_mod.SetMatrix(MGetIdent());

	grassparts->set_grass_ready_before();

	grassparts->set_grass_ready("data/grassput.bmp",5.f,5.f,-5.f,-5.f);
	//
	bool startp = false;
	cams.set_cam_info(VGet(0, 1.f, 0), VGet(0, 0, -1.f), VGet(0, 1.f, 0), deg2rad(90), 0.1f, 5000.f);//�J����
	{
		Drawparts->Set_Light_Shadow(VGet(10.f, 10.f, 10.f), VGet(-10.f, -10.f, -10.f), VGet(-0.5f, -0.5f, -0.5f), [&] {/*map_mod.DrawModel();*/ });	//���C�g�A�e
		//main_loop
		while (ProcessMessage() == 0) {
			const auto waits = GetNowHiPerformanceCount();
			Debugparts->put_way();
			//���W�擾
			Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &pos_HMD, &mat_HMD);
			Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &pos_HAND1, &mat_HAND1);
			Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &pos_HAND2, &mat_HAND2);

			Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &pos_track0, &mat_track0);
			//Drawparts->GetDevicePositionVR(Drawparts->tracker_num[1], &pos_track1, &mat_track1);
			//Drawparts->GetDevicePositionVR(Drawparts->tracker_num[2], &pos_track2, &mat_track2);

			//����
			mat_ = MATRIX_ref::RotY(deg2rad(rad));
			{
				//VR�p
				if (Drawparts->get_hand1_num() != -1) {
					auto& ptr_ = *Drawparts->get_device_hand1();
					if (ptr_.turn && ptr_.now) {
						if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) {
							startp = true;
						}
						if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
							rad += ptr_.touch.x();
						}
					}
				}
				//����
				if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
					startp = true;
				}
			}
			//VR��ԂɓK�p
			Drawparts->Move_Player();
			//�J�����w��
			if (Drawparts->use_vr) {
				cams.campos = pos_ + pos_HMD;
				cams.camvec = (mat_HMD*mat_).zvec();
				cams.camup = (mat_HMD*mat_).yvec();
			}
			else {
				cams.campos = pos_ + VGet(0, 1.6f, 0);
				cams.camvec = mat_.zvec();
				cams.camup = mat_.yvec();
			}
			//�`��
			Drawparts->draw_VR(
				[&] {
					
					SetUseLighting(FALSE);
					sky_mod.DrawModel();
					SetUseLighting(TRUE);

					Drawparts->Draw_by_Shadow(
						[&] {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						map_mod.DrawModel();
						//hand1
						DrawCone3D((pos_ + MATRIX_ref::Vtrans((pos_HAND1- pos_HMD),mat_) + pos_HMD + (mat_HAND1 * mat_).zvec()*0.1f).get(), (pos_ + MATRIX_ref::Vtrans((pos_HAND1 - pos_HMD), mat_) + pos_HMD - (mat_HAND1 * mat_).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						//hand2
						DrawCone3D((pos_ + MATRIX_ref::Vtrans((pos_HAND2 - pos_HMD), mat_) + pos_HMD + (mat_HAND2 * mat_).zvec()*0.1f).get(), (pos_ + MATRIX_ref::Vtrans((pos_HAND2 - pos_HMD), mat_) + pos_HMD - (mat_HAND2 * mat_).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

						DrawCone3D((pos_ + MATRIX_ref::Vtrans((pos_track0 - pos_HMD), mat_) + pos_HMD + (mat_track0 * mat_).zvec()*0.1f).get(), (pos_ + MATRIX_ref::Vtrans((pos_track0 - pos_HMD), mat_) + pos_HMD - (mat_track0 * mat_).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						DrawCone3D((pos_ + MATRIX_ref::Vtrans((pos_track1 - pos_HMD), mat_) + pos_HMD + (mat_track1 * mat_).zvec()*0.1f).get(), (pos_ + MATRIX_ref::Vtrans((pos_track1 - pos_HMD), mat_) + pos_HMD - (mat_track1 * mat_).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						DrawCone3D((pos_ + MATRIX_ref::Vtrans((pos_track2 - pos_HMD), mat_) + pos_HMD + (mat_track2 * mat_).zvec()*0.1f).get(), (pos_ + MATRIX_ref::Vtrans((pos_track2 - pos_HMD), mat_) + pos_HMD - (mat_track2 * mat_).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

						grassparts->draw_grass();
					});
				},
				cams);
			//debug
			GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK), false);
			Debugparts->end_way();
			Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
			//��ʍX�V
			Drawparts->Screen_Flip(waits);
			if ((CheckHitKey(KEY_INPUT_ESCAPE) != 0) | startp) {
				break;
			}
		}
	}
	grassparts->delete_grass();

	return 0;
}