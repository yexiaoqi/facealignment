#include <GL\glew.h>
#include <GL\freeglut.h>

#include "ArgParser.h"
#include "GLRender.h"
#include "PyStream.h"
#include "SingleImageReconstructor.h"

#define ROTATE 1
// jisy :use 68 landmarkers
//#define FACESHAPE_PNTNUM 78 
#define FACESHAPE_PNTNUM 68
/////////////////////////////////////////////////// My Block ////////////////////////////////////////////////////////
ArgParser mParser;
GLRender mGLRender;
SingleImageReconstructor mImgRecon;
ModelParametersBs_Video mVideoRes;
ModelParametersBs mFrameRes;
cv::VideoCapture mCap;
//int mEnableStream = 0;
cv::Mat mFrame;
int nFps, nFrame, frameCnt, meshCnt;
// OpenGL
bool isOnline;
bool isResultReview;
bool isDraw2DLandmark;
int imgWidth, imgHeight;
int gWinWidth, gWinHeight;
cv::Mat imageDisp;
cv::Mat imageDispFace;
GLuint textureIds;
GLuint textureIdsFace;
int  mouseX, mouseY;
bool mouseLeft, mouseMiddle, mouseRight;
double dragPosX, dragPosY, dragPosZ;
glm::mat4 modelMat; 
void onMouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void KeyboardCallback(unsigned char key, int x, int y);
void ReshapeCallback(int w, int h);
void CloseCallback();
void RenderCallback();
void Reset();
// Functions
void drawLandmark(cv::Mat& disp, std::vector<cv::Point2d> pts, int width, int height);
void pos(double *px, double *py, double *pz, const int x, const int y, const int *viewport);

// [Deprecated]
std::vector<std::vector<cv::Point2d>> videoPts2D;
void loadVideoPts2D(std::string pts2d_file);
std::vector<cv::Point2d> getPts2D(int nIdx);
///////////////////////////////// Viewer Functions (Don't Modify) /////////////////////////////////////////
//������ͶӰ���ӵ���Ϣ�������ӳ�䵽��������
void pos(double *px, double *py, double *pz, const int x, const int y, const int *viewport)
{
	/*
	Use the ortho projection and viewport information
	to map from mouse co-ordinates back into world
	co-ordinates
	*/

	*px = (double)(x - viewport[0]) / (double)(viewport[2]);
	*py = (double)(y - viewport[1]) / (double)(viewport[3]);

	float _left = -1.0 * (float)gWinWidth / (float)gWinHeight;
	float _right = 1.0 * (float)gWinWidth / (float)gWinHeight;
	float _top = 1.0;
	float _bottom = -1.0;
	float _zNear = 1.0;

	*px = _left + (*px)*(_right - _left);
	*py = _top + (*py)*(_bottom - _top);
	*pz = _zNear;
}

void Reset() 
{
	mouseX = 0;       //mouse control variables
	mouseY = 0;
	mouseLeft = false;
	mouseMiddle = false;
	mouseRight = false;
	dragPosX = 0.0;
	dragPosY = 0.0;
	dragPosZ = 0.0;

}

void onMouseCallback(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (state == GLUT_UP)
		switch (button)
	{
		case GLUT_LEFT_BUTTON:   mouseLeft = false; break;
		case GLUT_MIDDLE_BUTTON: mouseMiddle = false; break;
		case GLUT_RIGHT_BUTTON:  mouseRight = false; break;
	}
	else
		switch (button)
	{
		case GLUT_LEFT_BUTTON:	 mouseLeft = true; break;
		case GLUT_MIDDLE_BUTTON: mouseMiddle = true; break;
		case GLUT_RIGHT_BUTTON:	 mouseRight = true; break;
	}

	int viewport[4];
	viewport[0] = viewport[1] = 0;
	viewport[2] = gWinWidth;
	viewport[3] = gWinHeight;
	pos(&dragPosX, &dragPosY, &dragPosZ, x, y, viewport);
}

//    ������Ҽ����Կ���ģ����ת������м����Կ���ģ������
void MotionCallback(int x, int y)
{

	bool changed = false;

	const int dx = x - mouseX;
	const int dy = y - mouseY;

	int viewport[4];
	viewport[0] = viewport[1] = 0;
	viewport[2] = gWinWidth;
	viewport[3] = gWinHeight;

	if (dx == 0 && dy == 0)
		return;

	if (mouseMiddle || (mouseLeft && mouseRight)) //
	{
		//obj_scale += dy*0.01;
		glm::mat4 tmp = glm::mat4(1.0);
		tmp = glm::translate(tmp, glm::vec3(0.f, 0.f, dy*2.0f));
		modelMat = modelMat * tmp;

		changed = true;
	}
	else
	{
		if (mouseLeft) //
		{
			double ax, ay, az;
			double bx, by, bz;
			double angle;

			ax = dy;
			ay = dx;
			az = 0.0;
			angle = sqrt(ax*ax + ay*ay + az*az) / (double)(viewport[2] + 1)* 3.1415926f; //sqrt(x*x+y*y+z*z)

			/* Use inverse matrix to determine local axis of rotation */

			glm::mat4 invMat = glm::inverse(modelMat);
			bx = invMat[0][0] * ax + invMat[0][1] * ay + invMat[0][2] * az;
			by = invMat[1][0] * ax + invMat[1][1] * ay + invMat[1][2] * az;
			bz = invMat[2][0] * ax + invMat[2][1] * ay + invMat[2][2] * az;

			modelMat = glm::rotate(modelMat, (float)angle, glm::vec3((float)bx, (float)by, (float)bz));

			changed = true;
		}
		else
			if (mouseRight) //
			{
				double px, py, pz;

				pos(&px, &py, &pz, x, y, viewport);

				glm::mat4 tmp = glm::mat4(1.0);
				tmp = glm::translate(tmp, glm::vec3((float)px - dragPosX, (float)py - dragPosY, (float)pz - dragPosZ));
				modelMat = modelMat * tmp;

				dragPosX = px;
				dragPosY = py;
				dragPosZ = pz;

				changed = true;
			}
	}

	mouseX = x;
	mouseY = y;

	if (changed)
	{
		glutPostRedisplay();
	}
}

void CloseCallback() 
{
	std::cout << "GLUT:\t Finished" << std::endl;
	glutLeaveMainLoop();
}

void ReshapeCallback(int w, int h) 
{

	////////////////////////////////////////////
	int winWidth = gWinWidth;
	int winHeight = gWinHeight;
	float aspect = (float)winWidth / (float)winHeight;
	float Focal = (float)mImgRecon.recon_camera_.focal_;
	float zFar = (float)mImgRecon.recon_camera_.far_c_;
	winWidth = w;
	winHeight = h;
	float _top = 1.0;
	float _bottom = -1.0;
	float _left = -(double)w / (double)h;
	float _right = -_left;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0*aspect, 1.0*aspect, -1.0, 1.0, Focal, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//////////////////////////////// My Function ////////////////////////////////
int main(int argc, char *argv[]) 
{

	// Parse Args
	mParser.Parse(argc, argv);//��������  ����Parse(int argc, char* argv[])	����ѡ�������ļ�Ϊdefault.conf	
  //��ʵ����ֻ������indexfile��bsfolder��videoinput��bsnames��bsindex��indexfile��landmarkfileһ����

	std::string m_indexFile = mParser.get_indexFile();//�õ�indexfile  data/jisy-20161215/68markers.txt
	std::string m_videoInput = mParser.get_videoInput(); // �õ�videoinput  data/video/2.mov
	//std::cout << m_videoInput; //added by yqy
	std::string m_bsModelBin = mParser.get_bsModelBin();//�õ�bsModelBin//��ʱΪ��
	//std::cout <<"1111111111"<< m_bsModelBin << std::endl;//added by yqy
	//std::string m_landmarkFile = mParser.get_landmarkFile();//commented by yqy
	//std::string m_bsCoefs = mParser.get_bsCoefficients();//Ϊ��
	//std::cout <<"m_bsCoefs:" <<m_bsCoefs << std::endl;
	//std::string m_bsNames = mParser.get_bsNames();//  �õ�51��bs����������Neutral��EyeBlink_L��EyeBlink_R  data/list_51.txt
	//std::string m_bsIndex = mParser.get_bsIndex();//�õ���m_bsNames��Ӧ��bs������ data/bsIndex.txt
	int m_numLandmark = mParser.get_numLandmark();//m_numLandmark����configFile��inputLine >> m_numLandmark_; Ϊ68
	int m_numBs = mParser.get_numBs();//numBs = 51
	int m_cameraIndex = mParser.get_cameraIndex();
	//mEnableStream = mParser.get_enableStream();
	std::string m_bsFolder = mParser.get_bsFolder();//�õ�bsfolder  data/model_yqy_20170330_tri  ����faceshift�õ���mesh����
	std::cout << m_bsFolder << std::endl;
	//std::string m_landmarkFolder = mParser.get_landmarkFolder();//�õ�landmarkfolder
	// load blendshape
	if (!m_bsModelBin.empty())//    if m_bsModelBin�ǿգ�����Models�е�LoadBsBin
		mImgRecon.bsData_.LoadBsBin(m_bsModelBin, m_numBs);
	else//else ����Models�е�LoadBs�����μ��ؿ϶���else��
		mImgRecon.bsData_.LoadBs(m_bsFolder, m_numBs);//bsFolder =data/model_yqy_20170330_tri ����51��.obj�ļ�
	mImgRecon.recon_mesh_ = mImgRecon.bsData_.blendshapes_[0];//neural����ģ����Ϊrecon_mesh  
	//����һ��LoadBs����blendshapes_.push_back(obj_mesh); blendshapes_����һ��mesh�ṹ
	mImgRecon.recon_mesh_.update_normal();//recon_mesh���·��ߣ������淨������㷨��
	// load index3d
	mImgRecon.numPts_ = m_numLandmark;//numPts_Ϊ68
	mImgRecon.LoadIndex3D(m_indexFile);//����index3d�õ�index3D_ //��data/jisy-20161215/68markers.txt����
	// project landmark
	mImgRecon.bsData_.ProjectLandmark(mImgRecon.index3D_);//ͨ��index3D_��λ����ά��Ϣ����landmarks_model_
	// open the video file

	if (!m_videoInput.empty())//if m_videoInput�ǿգ�����opencv���open������Ƶ��get֡��
	{
		mCap.open(m_videoInput);
		nFrame = mCap.get(CV_CAP_PROP_FRAME_COUNT);
	}
	else//else������opencv���open���������֡��Ϊ99999(�������ǰ¼����Ƶ�����if��ʵʱ�����else��
	{
		mCap.open(m_cameraIndex);
		nFrame = 99999;
	}
	//std::cout << "1111111111" << m_videoInput << std::endl;
	nFps = mCap.get(CV_CAP_PROP_FPS);//  ��׽֡����

	//    ������ת
#ifdef ROTATE
	int height = mCap.get(CV_CAP_PROP_FRAME_WIDTH);
	int width = mCap.get(CV_CAP_PROP_FRAME_HEIGHT);
#else
	int width = mCap.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = mCap.get(CV_CAP_PROP_FRAME_HEIGHT);
#endif // ROTATE

	//�õ���͸�
	imgWidth = width;
	imgHeight = height;
	//���֡���ʣ�֡����Ƶ��С
	std::cout << "Fps of video is " << nFps << std::endl;
	std::cout << "nFrame of video is " << nFrame << std::endl;
	std::cout << "The size of video is " << width << " x " << height << std::endl;
	// init parameters
	gWinHeight = height;
	gWinWidth = width * 2;
	frameCnt = 0;
	meshCnt = 0;
	//  isOnline,isResultReview��isDraw2DLandmarkΪfalse
	isOnline = false;
	isResultReview = false;
	isDraw2DLandmark = false;
	//if (!m_bsCoefs.empty()) //    if m_bsCoefs�ǿգ�isResultReviewΪtrue������LoadBsCoeff
	//{
	//	isResultReview = true;
	//	mVideoRes.LoadBsCoeff(m_bsCoefs, nFrame, false);
	//}

	// Init OpenGL   ��ʼ��OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(gWinWidth, gWinHeight);
	glutCreateWindow("3D Face Fitting");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glewInit();
	glGenTextures(1, &textureIds); // left  ����ָ���������������(1�������������ǵ����þ���ŵ�GLuint����ָ�루�ڶ�����������
	glGenTextures(1, &textureIdsFace); // right

	// Register Callbacks  ����callback
	glutDisplayFunc(RenderCallback);
	glutCloseFunc(CloseCallback);
	glutReshapeFunc(ReshapeCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutMouseFunc(onMouseCallback);
	glutMotionFunc(MotionCallback);
	glutIdleFunc(NULL);

	// Load Frame0 && Prepare Image Display  ���ص�һ֡ͼ��
	mCap >> mFrame;
#ifdef ROTATE
	cv::transpose(mFrame, mFrame);//rotate 90d  ��ת90��
	cv::flip(mFrame, mFrame, 1);//flip by y axis
#endif // ROTATE

	cv::imwrite("fisrstFrame.jpg", mFrame);//imwrite��һ��ͼ��
	mImgRecon.SetInputImage(mFrame);//��������
	mImgRecon.Init();//��ʼ��ģ�͵�λ��
	mImgRecon.PrepareConstraints();//��ģ�͵�λ�ô���constraints2D_������ÿ��landmark��Ȩ��
	mImgRecon.LandmarkDetect(mImgRecon.input_img_);	//����dlib����landmark��Ⲣ����pts2D_
	mFrame.copyTo(imageDisp);
	if (isDraw2DLandmark) drawLandmark(imageDisp, mImgRecon.pts2D_, imgWidth, imgHeight);//if (isDraw2DLandmark) ��ʾlandmark

	//��Ⱦ��ʼ��
	//׼��bg����ͼ
	// Prepare Display Content for OpenGL Render ( Background & Mesh Model & Spheres )
	mGLRender.Init(Eigen::Vector2i(imgWidth, imgHeight)); // render size = image size
	// A. prepare bg ground image
	mGLRender.addModel(BufModel(), 0);//����ģ�ͣ�0��modelIdx
	mGLRender.models[0].CreateBGplaneModel();//��������
	mGLRender.models[0].LoadShaderProgram("shaders/background_vertex_shader.glsl", "shaders/background_fragment_shader.glsl");//������ɫ��
	mGLRender.models[0].BufCreateTexture(imgWidth, imgHeight);
	mGLRender.models[0].BufUpdateTexture(imageDisp);//�������������
	mGLRender.models[0].isDraw_ = true;
	//   ׼��ģ��  B. prepare model
	mGLRender.addModel(BufModel(), 1);//  ����ģ��
	mGLRender.models[1].CreateDispModel(mImgRecon.recon_mesh_, false);//    ����dispmodel
	mGLRender.models[1].LoadShaderProgram("shaders/face_vertex_shader_dir.glsl", "shaders/face_fragment_shader_dir.glsl");//������ɫ��
	mGLRender.models[1].isDraw_ = false;
	mGLRender.models[1].colorMode_ = 0;
	if (isResultReview) //if (isResultReview)������mesh�����ߺ�dispmodel
	{		// if result review mode & update model
		mImgRecon.UpdateMesh(mVideoRes.bsVideo_[frameCnt].params_);
		mImgRecon.recon_mesh_.update_normal();
		mGLRender.models[1].UpdateDispModel(mImgRecon.recon_mesh_);
	}
	// ׼���ο���  C. prepare spheres
	mGLRender.addSphere(SolidSphere(), 0);//  ���Ӳο���
	mGLRender.spheres[0].Init();//  ���زο���
	mGLRender.spheres[0].LoadShaderProgram("shaders/sphere_vertex_shader.glsl", "shaders/sphere_fragment_shader.glsl");//    ������ɫ��
	mImgRecon.UpdateLandmarkPos();	// ����landmarkλ��update in single image reconstructor
	mGLRender.spheres[0].SetData(mImgRecon.landmark3DPos_); // ����3dlandmark����set data
	mGLRender.spheres[0].SetColor(1.f, 0.f, 0.f);
	mGLRender.spheres[0].SetScale(0.01f);//������ɫ�ʹ�С
	mGLRender.spheres[0].exception_idx_ = -1;
	mGLRender.spheres[0].drawSphere_ = false;
	//   ���û�����ģ�͡��ӽǺ�ͶӰ����  6 - Set Buffer Model's Model && View && Projection Matrix
	mGLRender.models[1].model_ = mImgRecon.model_mat_;
	mGLRender.models[1].view_ = mImgRecon.camera_view_;
	mGLRender.models[1].proj_ = mImgRecon.projection_;
	mGLRender.spheres[0].model_ = mImgRecon.model_mat_;
	mGLRender.spheres[0].view_ = mImgRecon.camera_view_;
	mGLRender.spheres[0].proj_ = mImgRecon.projection_;
	// 7 - Add 1 to frameCnt
	frameCnt++;
	glutMainLoop();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'a':
	case 'A':  // process all the data
	{
		isOnline = !isOnline;
		break;
	}
	case 'r':
	case 'R':  // reset 's model matrix
	{
		modelMat = glm::mat4(1.0);
		break;
	}
	case 'P':
	case 'p':  // toggle if display mesh model
	{
		mGLRender.models[1].isDraw_ = !mGLRender.models[1].isDraw_;
		break;
	}
	case 'm':
	case 'M': // change display color mode
	{
		mGLRender.models[1].colorMode_++;
		mGLRender.models[1].colorMode_ = mGLRender.models[1].colorMode_ % 3;
		break;
	}
	case 'o':
	case 'O': // toggle if draw 2D landmark
	{
		isDraw2DLandmark = !isDraw2DLandmark;
		mFrame.copyTo(imageDisp);	// reset imageDisp
		if (isDraw2DLandmark) drawLandmark(imageDisp, mImgRecon.pts2D_, imgWidth, imgHeight);
		mGLRender.models[0].BufUpdateTexture(imageDisp); // update texture
		break;
	}
	case 'n':
	case 'N': // display next frame
	{
		if (frameCnt < nFrame){
			// 1 - capture frame
			mCap >> mFrame;
#ifdef ROTATE
			cv::transpose(mFrame, mFrame);//rotete 90d
			cv::flip(mFrame, mFrame, 1);//flip by y axis
#endif // ROTATE

			//cv::waitKey(10);
			mImgRecon.SetInputImage(mFrame);
			// 2 -  add landmark into display image
			mFrame.copyTo(imageDisp);
			mImgRecon.LandmarkDetect(mImgRecon.input_img_);
			if (isDraw2DLandmark) drawLandmark(imageDisp, mImgRecon.pts2D_, imgWidth, imgHeight);
			mGLRender.models[0].BufUpdateTexture(imageDisp);
		}
		frameCnt++;
		std::cout << "============= frame: " << frameCnt << std::endl;
		break;
	}
	case 'c':
	case 'C':{
		 //Calibration
		//glm::mat4 transformation = modelMat;
		//glm::vec3 scale;
		//glm::quat rotation;
		//glm::vec3 translation;
		//glm::vec3 skew;
		//glm::vec4 perspective;
		//glm::decompose(transformation, scale, rotation, translation, skew, perspective);
		//glm::vec3 euler = glm::eulerAngles(rotation) /** 3.14159f / 180.f*/;
		//mImgRecon.recon_model_.Rx_ = euler.x;
		//mImgRecon.recon_model_.Ry_ = -euler.y;
		//mImgRecon.recon_model_.Rz_ = euler.z;
		//mImgRecon.recon_model_.Tx_ = translation.x;
		//mImgRecon.recon_model_.Ty_ = translation.y;
		//mImgRecon.recon_model_.Tz_ = translation.z;

		//glm::mat4 tmpMat = modelMat;
		//cv::Mat cvTmpMat = (cv::Mat_<double>(3, 3) <<
		//	tmpMat[0][0], tmpMat[0][1], tmpMat[0][2],
		//	tmpMat[1][0], tmpMat[1][1], tmpMat[1][2],
		//	tmpMat[2][0], tmpMat[2][1], tmpMat[2][2]
		//	);
		//cv::Vec3f eulerAngle = rotationMatrixToEulerAngles(cvTmpMat);
		//mImgRecon.recon_model_.Rx_ = eulerAngle[2];
		//mImgRecon.recon_model_.Ry_ = eulerAngle[1];
		//mImgRecon.recon_model_.Rz_ = eulerAngle[0];
		//mImgRecon.recon_model_.Tx_ = tmpMat[0][3];
		//mImgRecon.recon_model_.Ty_ = tmpMat[1][3];
		//mImgRecon.recon_model_.Tz_ = tmpMat[2][3];
		//mImgRecon.UpdateModelMat();
		//modelMat = glm::mat4(1.0);
		break;
	}
	case 's':
	case 'S':{
		// random blendshape model
		meshCnt++;
		meshCnt = meshCnt % NUM_BLENDSHAPE;
		mImgRecon.recon_mesh_ = mImgRecon.bsData_.blendshapes_.at(meshCnt);
		mGLRender.models[1].UpdateDispModel(mImgRecon.recon_mesh_);
		mImgRecon.UpdateLandmarkPos();
		mGLRender.spheres[0].SetData(mImgRecon.landmark3DPos_);
		break;
	}
	case 't':
	case 'T':
	{

		break;
	}
	case 'l':
	case 'L':
	{
		// toggle if show landmark 3D
		mGLRender.spheres[0].drawSphere_ = !mGLRender.spheres[0].drawSphere_;
		break;
	}
	case 'w':
	case 'W':
	{
		// save blendshape coefficient
		mVideoRes.SaveBsCoeff("default.bs", false);
		break;
	}

	case ' ':
	{
		// 1 - call fit
		// fit accurately to first frame
		if (frameCnt == 1)
		{
			//cv::imwrite("clx_001.bmp", mImgRecon.input_img_);
			mImgRecon.recon_model_.ResetBsCoeff();
			mImgRecon.OptimizeFocalLength();//jisy fix
			mImgRecon.FitFirstFrame();
		}
		else{
			mImgRecon.Fit();
		}
		if (!mVideoRes.bsVideo_.empty()) mVideoRes.bsVideo_.pop_back();
		mVideoRes.bsVideo_.push_back(mImgRecon.recon_model_);
		// 2 - post display result
		mImgRecon.UpdateMesh(mImgRecon.recon_model_.params_);
		mImgRecon.recon_mesh_.update_normal();
		//mImgRecon.UpdateModelMat();
		mImgRecon.UpdateProjectionMat();
		mImgRecon.UpdateLandmarkPos();
		for (int kk = 0; kk < mGLRender.models.size(); kk++) {
			mGLRender.models[kk].model_ = mImgRecon.model_mat_ * modelMat;
			mGLRender.models[kk].proj_ = mImgRecon.projection_;
		}
		for (int kk = 0; kk < mGLRender.spheres.size(); kk++) {
			mGLRender.spheres[kk].model_ = mImgRecon.model_mat_ *modelMat;
			mGLRender.spheres[kk].proj_ = mImgRecon.projection_;
		}
		mGLRender.models[1].UpdateDispModel(mImgRecon.recon_mesh_);
		mGLRender.spheres[0].SetData(mImgRecon.landmark3DPos_);
		break;
	}
	default:
	{
		fflush(stdin);
		break;
	}	//flush all other input 				
	}
	fflush(stdin);
	glutPostRedisplay();
}

void RenderCallback() 
{
	//ȷ�����ڿ�ߣ�ͬʱ��mframe������mImgRecon.input_img_��
	int winWidth = gWinWidth;
	int winHeight = gWinHeight;
	// Step-1 Process my events
	if (isOnline && frameCnt < nFrame) //if �������в���֡��δ����֡������׽֡
	{//jisy to do

		mCap >> mFrame;//jisy rotate image 
	//  ��ת	
#ifdef ROTATE
		mCap >> mFrame;//drop a frame
		cv::transpose(mFrame, mFrame);//rotete 90d
		cv::flip(mFrame, mFrame, 1);//flip by y axis
#endif // ROTATE

		//cv::waitKey(10);
		mImgRecon.SetInputImage(mFrame);//��������ͼ�񣨿�=�У���=�У�
		// process dispImage
		mFrame.copyTo(imageDisp);//  ��mframe�Ķ�������imageDisp��
		mImgRecon.LandmarkDetect(mImgRecon.input_img_);//  ���landmark
		if (isDraw2DLandmark) drawLandmark(imageDisp, mImgRecon.pts2D_, imgWidth, imgHeight);//  if (isDraw2DLandmark)������landmark
		mGLRender.models[0].BufUpdateTexture(imageDisp);//��������
		if (!isResultReview) 
		{
			if (frameCnt == 1)//if ��һ֡������bsϵ�����Ż����࣬��һ֡ͼ���Ż�������ʮ�Σ�
			{
				mImgRecon.ResetParams();
				mImgRecon.OptimizeFocalLength();//jisy fix
				//this->recon_camera_.focal_
				mImgRecon.FitFirstFrame();
			}
			else//�Ż���ֻ����һ�Σ�
			{
				mImgRecon.Fit();
			}
			mFrameRes = mImgRecon.recon_model_;
			mVideoRes.bsVideo_.push_back(mFrameRes);
		}
		else 
		{
			//poseParameters_frame_ = poseParameters_[frameCnt_];
			mFrameRes = mVideoRes.bsVideo_[frameCnt];
		}

		mImgRecon.UpdateMesh(mFrameRes.params_);//  ����mesh
		mImgRecon.recon_mesh_.update_normal();//���·���
		//mImgRecon.UpdateModelMat();//mat translate
		mImgRecon.UpdateProjectionMat();//����ͶӰ����
		mImgRecon.UpdateLandmarkPos();//����landmarkλ��
		mGLRender.models[1].UpdateDispModel(mImgRecon.recon_mesh_);//����dispmodel
		mGLRender.spheres[0].SetData(mImgRecon.landmark3DPos_);//���òο�������
		frameCnt++;//֡����

	}

	// Step-2 Call render
	// 1. update model, view, projection matrix
	for (int kk = 0; kk < mGLRender.models.size(); kk++) 
	{
		mGLRender.models[kk].model_ = mImgRecon.model_mat_ * modelMat;//model_mat_ = Tmat * Rmat��תλ��
		mGLRender.models[kk].proj_ = mImgRecon.projection_;
	}
	for (int kk = 0; kk < mGLRender.spheres.size(); kk++) 
	{
		mGLRender.spheres[kk].model_ = mImgRecon.model_mat_ *modelMat;
		mGLRender.spheres[kk].proj_ = mImgRecon.projection_;
	}
	// 2. render
	bool isDrawFace_old = mGLRender.models[1].isDraw_;	// render to right window
	bool isDrawLandmark_old = mGLRender.spheres[0].drawSphere_; 
	mGLRender.models[1].isDraw_ = true;
	mGLRender.spheres[0].drawSphere_ = true;
	mGLRender.RenderFace();
	mGLRender.GetThumb(imageDispFace);//��ȡRGBA���ݴ浽imageDispFace  �Ұ�ߴ���
	//cv::imwrite("imageDiapFace.jpg", imageDispFace);
	mGLRender.models[1].isDraw_ = isDrawFace_old;  // render to left window
	mGLRender.spheres[0].drawSphere_ = isDrawLandmark_old;
	mGLRender.Render();
	mGLRender.GetThumb(imageDisp);//���ߴ���
	/*****************************************************************************************
							Step-3 Draw content on opengl window
	*******************************************************************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//std::cout << winWidth << " x " << winHeight << std::endl;
	glViewport(0, 0, winWidth, winHeight);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1);
	glClearColor(0.296875, 0.296875, 0.3203125, 1.0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	{
		glLoadIdentity();
		glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{
			// Left window
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureIds);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mGLRender.win_size_[0], mGLRender.win_size_[1],
				0, GL_RGBA, GL_UNSIGNED_BYTE, imageDisp.data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glBegin(GL_QUADS); {
				glTexCoord2f(0, 1);  glVertex2f(0, 0);
				glTexCoord2f(1, 1);  glVertex2f(0.5, 0);
				glTexCoord2f(1, 0);  glVertex2f(0.5, 1);
				glTexCoord2f(0, 0);  glVertex2f(0, 1);
			}
			glEnd();

			// Right window
			glBindTexture(GL_TEXTURE_2D, textureIdsFace);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mGLRender.win_size_[0], mGLRender.win_size_[1],
				0, GL_RGBA, GL_UNSIGNED_BYTE, imageDispFace.data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glBegin(GL_QUADS); {
				glTexCoord2f(0, 1);  glVertex2f(0.5, 0);
				glTexCoord2f(1, 1);  glVertex2f(1, 0);
				glTexCoord2f(1, 0);  glVertex2f(1, 1);
				glTexCoord2f(0, 0);  glVertex2f(0.5, 1);
			}
			glEnd();

		}
		glDisable(GL_TEXTURE_2D);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
	//////////////////////////////////////////////////////////////////////////////////
}


void drawLandmark(cv::Mat& disp, std::vector<cv::Point2d> pts, int width, int height) 
{
	for (int kk = 0; kk < pts.size(); kk++) 
	{
		cv::circle(imageDisp, cv::Point((int)pts.at(kk).x, (int)pts.at(kk).y), 4, cv::Scalar(0, 255, 0), -1);//JISY : (height - y) -> y
	}
}

// [Deprecated]
void loadVideoPts2D(std::string pts2d_file)
{
	videoPts2D.resize(nFrame);
	std::ifstream fin(pts2d_file);
	cv::Point2d pt2d;
	if (fin.is_open()) 
	{
		for (int ii = 0; ii < nFrame; ii++)
		{
			for (int kk = 0; kk < FACESHAPE_PNTNUM; kk++) 
			{
				fin >> pt2d.x >> pt2d.y;
				pt2d.y = imgHeight - pt2d.y;
				if (kk < mImgRecon.numPts_)
				{
					videoPts2D[ii].push_back(pt2d);
				}
			}
		}
	}
	fin.close();
	return;
}

std::vector<cv::Point2d> getPts2D(int nIdx)
{
	return videoPts2D[nIdx];
}


