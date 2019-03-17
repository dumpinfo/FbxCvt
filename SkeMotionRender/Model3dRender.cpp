
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new DEBUG_CLIENTBLOCK
#endif

 
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include <stdlib.h>
#include <crtdbg.h>
//#include "flistread.h"
#include "read_cfg.h"
#include "Scene.h"

 

//int ui_loop(int argc, char **argv, const char *name);

void init( GraphDrawLib *grawLib)
{
   static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0 };
   static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0 };
   static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0 };
   static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0 };

   grawLib->glLightfv( GL_LIGHT0, GL_POSITION, pos );
   grawLib->glEnable( GL_CULL_FACE );
   grawLib->glEnable( GL_LIGHTING );
   grawLib->glEnable( GL_LIGHT0 );
   grawLib->glEnable( GL_DEPTH_TEST );


   grawLib->glEnable( GL_NORMALIZE );
}

/* new window size or exposure */
void reshape( GraphDrawLib *grawLib, int width, int height )
{
   GLfloat  h = (GLfloat) height / (GLfloat) width;

   grawLib->glViewport(0, 0, (GLint)width, (GLint)height);
   grawLib->glMatrixMode(GL_PROJECTION);
   grawLib->glLoadIdentity();
   grawLib->glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
   grawLib->glMatrixMode(GL_MODELVIEW);
   grawLib->glLoadIdentity();
   grawLib->glTranslatef( 0.0, 0.0, -40.0 );
   //KH_add grawLib->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

 

class VWordModel
{
  public:
  VWordModel(GraphDrawLib*gLib);
  ~VWordModel();
  Camera *Camera1 ;
  Transformation *SysTrans  ;
  vector< Light *> Lights; 
  // = new StlShape[ element_num ];
  vector< TextureSurface * > TextQuad;
  vector< Geometry *        >     shapeObjects;  // 
  vector<Cube*>             kglCubes;
  vector<ConeShape*>   kglCones;
  vector< Transformation *>    shapeTransform;//    = new Transformation[ element_num ];
  Polygon  *AxisXX  ;                              
  Polygon  *AxisYY  ;                              
  Polygon  *AxisZZ  ;                              
  Node *Root ;
  GLViewer *MyViewer ;
  TextureSurface* groundface  ;
  Transformation* texsurface_trs;
  int LightIdx;
};

VWordModel::VWordModel(GraphDrawLib*gLib)
{
  Camera1  = new Camera(gLib, PERSPECTIVE);
  SysTrans = new Transformation(gLib);
  LightIdx = 0;
  //Light1   = new Light(gLib);
  //StlShape       *StlElements = new StlShape[ element_num ];
  //Transformation *StlTrans    = new Transformation[ element_num ];
  AxisXX = new Polygon(gLib);  AxisXX->nodename ="AxisXX = new Polygon";                             
  AxisYY = new Polygon(gLib);  AxisYY->nodename ="AxisYY = new Polygon";                             
  AxisZZ = new Polygon(gLib);  AxisZZ->nodename ="AxisZZ = new Polygon";                             
  Root   = new Node;     Root  ->nodename ="Root";
  Root->grawLib = gLib;
  MyViewer = new GLViewer;// MyViewer ->nodename = "Viewer";
 // groundface = new TextureSurface;
 // texsurface_trs = new Transformation(gLib); //need to be free
  //= Light1->grawLib
  Camera1->grawLib = SysTrans->grawLib  = AxisXX->grawLib =
  AxisYY->grawLib = AxisZZ->grawLib = Root->grawLib = MyViewer->grawLib = gLib;
	//groundface->grawLib =  texsurface_trs->grawLib = gLib;
}

VWordModel::~VWordModel()
{
	/*delete Camera1  ;
	delete SysTrans  ;
	 
	delete AxisXX;
	delete AxisYY;
	delete AxisZZ;
	delete Root;
	delete MyViewer;
	delete groundface;
	delete texsurface_trs ;*/
	delete  Root;
	Root = NULL;
	_CrtDumpMemoryLeaks();
	return;
}

//GraphDrawLib graphLib;
//GraphDrawLib graphLibDbg;
//VWordModel   worldModel(&graphLib);

/*
void traverseFbsNodes(FbxNode*node, int node_level)
{
int i;
for (i = 0; i < node_level; i++)
printf("   ");
printf("+");
printf("%s\n", node->nodeName.c_str());
for (i = 0; i < node->subNodes.size(); i++)
traverseFbsNodes(node->subNodes[i], node_level + 1);
}
*/
struct SceneCfg
{ 	double  camx, camy, camz, campov;
	double  rot_angle, rot_x, rot_y, rot_z;
	double  trans_x, trans_y, trans_z;
	double  light_x, light_y, light_z;
};

//void ReadFileFbxObjList(char*filename, vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks);
/*struct FbxNode
{
	string nodeName;
	FbxNode *nodeParent;
	vector<FbxNode *>subNodes;
};

struct FbxNodeLink
{
	string linkName;
	FbxNode * fromeNode;
	FbxNode * toNode;
	double linkLength;
	double nodePos[3];
	double nodeRot[3];
};

void ReadFileFbxObjList(char*filename, vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks)
*/

void ProcessNodesLinkage(vector<Cube*>& kglCubes, vector<ConeShape*>& kglCones,
	                                    vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks)
{
	int i;
	for (i = 0; i < fbxLinks.size(); i++)
	{
		int fromNodeIdx = fbxLinks[i].fromeNode->nodeIdx;
		int toNodeIdx     = fbxLinks[i].toNode->nodeIdx;
		kglCubes[fromNodeIdx]->AddChild(kglCones[i]);
		kglCones[i]->AddChild(kglCubes[toNodeIdx]);
		kglCubes[toNodeIdx]->nodePosX = fbxLinks[i].nodePos[0];
		kglCubes[toNodeIdx]->nodePosY = fbxLinks[i].nodePos[1];
		kglCubes[toNodeIdx]->nodePosZ = fbxLinks[i].nodePos[2];
		kglCubes[toNodeIdx]->angleX = fbxLinks[i].nodeRot[0];
		kglCubes[toNodeIdx]->angleY = fbxLinks[i].nodeRot[1];
		kglCubes[toNodeIdx]->angleZ = fbxLinks[i].nodeRot[2];

		kglCones[i]->SetConeHeight(fbxLinks[i].linkLength);
		kglCones[i]->SetDirection(fbxLinks[i].nodePos[0], fbxLinks[i].nodePos[1], fbxLinks[i].nodePos[2]);
		kglCubes[fromNodeIdx]->SetSize(fbxLinks[i].linkLength / 20, fbxLinks[i].linkLength / 20, fbxLinks[i].linkLength / 20);
	//	kglCubes[toNodeIdx]->SetSize(fbxLinks[i].linkLength / 8, fbxLinks[i].linkLength / 8, fbxLinks[i].linkLength / 8);
	}
}

/*nodePosX, nodePosY, nodePosZ;
	double  angleX, angleY, angleZ*/

void SetModelScene(GraphDrawLib *grawLib, VWordModel &world_model , SceneCfg&scene, vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks)
{
	int i;
	world_model.kglCubes.resize(fbxNodes.size());
	world_model.kglCones.resize(fbxLinks.size());
	/*vector<Cube*>             kglCubes;
  vector<ConeShape*>   kglCones;
	*/
	for (i = 0; i < fbxLinks.size(); i++)
	{
		world_model.kglCones[i] = new ConeShape(grawLib);	
		world_model.kglCones[i]->SetMaterial(GetColorMat(0.0, 1.0, 1.0, grawLib));
	}

	for (i = 0; i < fbxNodes.size(); i++)
	{
		world_model.kglCubes[i] = new Cube(grawLib);
		world_model.kglCubes[i]->SetMaterial(GetColorMat(180.0, 1.0, 1.0, grawLib));
		world_model.kglCubes[i]->SetSize(0.02, 0.02, 0.02);
	}

	world_model.kglCubes[0]->angleX = world_model.kglCubes[0]->angleY = world_model.kglCubes[0]->angleZ = 0;
	world_model.kglCubes[0]->nodePosX = world_model.kglCubes[0]->nodePosY = world_model.kglCubes[0]->nodePosZ = 0;

	ProcessNodesLinkage(world_model.kglCubes, world_model.kglCones,
		 fbxNodes, fbxLinks);
	
	world_model.SysTrans->AddChild(world_model.kglCubes[0]);

	float vx[][3] = { { 0, -0.003, 0.0 }, { 100, -0.003, 0.0 }, { 100, 0.003, 0.0 }, { 0, 0.003, 0.0 } };
	float vy[][3] = { { -0.003, 0, 0.0 }, { 0.003, 0, 0.0 }, { 0.003, 100, 0.0 }, { -0.003, 100, 0.0 } };
	float vz[][3] = { { -0.003, 0, 0.0 }, { -0.003, 0, 100.0 }, { 0.003, 0, 100.0 }, { 0.003, 0, 0.0 } };
	//#########################SET CAMERA POSE#########################################
	world_model.Camera1->SetValue(POSITION, scene.camx, scene.camy, scene.camz);
	world_model.Camera1->SetValue(AIMAT, 0, 0, 0);
	world_model.Camera1->SetValue(UPDIRECTION, 0, 1, 0);
	world_model.Camera1->SetValue(ASPECT, 1);
	world_model.Camera1->SetValue(NEAR, 0.1);
	world_model.Camera1->SetValue(FAR, 200);
	world_model.Camera1->SetValue(YANGLE, scene.campov);
	world_model.Camera1->nodename = "Camera1";
	//#######################SET SYS ROT&TRANS##########################
	//##################################################################  
	world_model.SysTrans->SetValue(ROTATION, scene.rot_angle, scene.rot_x, scene.rot_y, scene.rot_z, 0);
	world_model.SysTrans->SetValue(TRANSLATION, scene.trans_x, scene.trans_y, scene.trans_z, 2);
	//#######################Draw SYS AXIS##############################
	//##################################################################  

	world_model.AxisXX->SetVerticesv(vx, 4); world_model.AxisXX->SetMaterial(GetColorMat(360 * 0.0 / 8.0, 1.0, 1.0, grawLib));
	world_model.AxisYY->SetVerticesv(vy, 4); world_model.AxisYY->SetMaterial(GetColorMat(120.0, 1.0, 1.0, grawLib));
	world_model.AxisZZ->SetVerticesv(vz, 4); world_model.AxisZZ->SetMaterial(GetColorMat(240.0, 1.0, 1.0, grawLib));

	//===================================================================================
	//====================================================================================
	int stlIdx = 0;
	//vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks
	 
//	if (trim(strvec[0]) == "LIGHT")
	{

		Light * Light1 = new Light(grawLib);
		Light1->SetValue(POSITION, scene.light_x, scene.light_y, scene.light_z, 1);
		Light1->SetValue(SPOT_DIRECTION, -scene.light_x, -scene.light_y, -scene.light_z);
		Light1->SetValue(CUTOFFANGLE, 180.0);
		Light1->TurnOn();   char lightname[40];
		sprintf(lightname, "Light_%i", world_model.LightIdx); Light1->nodename = lightname;
		world_model.Lights.push_back(Light1);
		world_model.LightIdx++;
	}


	//   for( i = 0; i <  world_model.StlElements.size(); i++ )
	//    world_model.SysTrans->AddChild(world_model.StlElements[i]);

	//for( i = 0; i <  (element_num-1); i++ )
	//  world_model.StlElements[i]->AddChild(world_model.StlElements[i+1]);

	world_model.SysTrans->AddChild(world_model.AxisXX);
	world_model.SysTrans->AddChild(world_model.AxisYY);
	world_model.SysTrans->AddChild(world_model.AxisZZ);
	// world_model.SysTrans->AddChild( world_model.groundface);



	world_model.Root->AddChild(world_model.SysTrans);


	for (i = 0; i< world_model.Lights.size(); i++)
	{
		//world_model.SysTrans->AddChild(world_model.Lights[i]);

		world_model.Root->AddChild(world_model.Lights[i]);
	}


	//Root->AddChild(Light1);
	//Light1->AddChild(SysTrans);
	world_model.Root->AddChild(world_model.Camera1);  world_model.Root->nodename = "root node";

	//Viewer:
}

/*void reshape(int width, int height)
{
if (height==0)
{
height=1;
}

graphLib.glViewport(0,0,width,height);

graphLib.glMatrixMode(GL_PROJECTION);
graphLib.glLoadIdentity();
graphLib.gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f ,150.0f);

graphLib.glMatrixMode(GL_MODELVIEW);
graphLib.glLoadIdentity();
}*/

class CharSkeleton
{
public:
vector<FbxNode> fbxNodes;
vector<FbxNodeLink> fbxLinks;
int sequence_id;
float time;
};

void ReadFbxSkeletionMotionFile(char*filename, vector<CharSkeleton>&motionFrames)
{

	vector <string>        nodeNameVec;
	map<string, int>       strMap;
	map<string, FbxNode*>  strNodeMap;

	std::vector<std::string> file_lines;
	readlistfile(filename, file_lines);
	//readlistfile(filename, lines);

	int i, j, fcount, nFrameNum;
	nFrameNum = file_lines.size() / 28;
	motionFrames.resize(nFrameNum);

	for (fcount = 0; fcount < nFrameNum;fcount++)
	{
		vector<string> lines;
		nodeNameVec.clear(); 
		strMap.clear(); 
		strNodeMap.clear();
		lines.clear();

		for (i = 0; i < 28; i++)
		{
			lines.push_back(file_lines[28*fcount+i]);
		}

		CharSkeleton *frame;
		frame = &motionFrames[fcount];
		//frame->fbxLinks.clear();
		//frame->fbxNodes.clear();

	//	for (i = 0; i < lines.size(); i++)
		for (i = 0; i < 25; i++)
		{
			
			std::vector<std::string> strvec; strvec.clear();
			splitString(lines[i], strvec, ",");
			string node_name = trim(strvec[0].c_str());
			if (strMap[node_name] == 0)
			{
				nodeNameVec.push_back(node_name);
				strMap[node_name] = 1;
			}
			node_name = trim(strvec[1].c_str());
			if (strMap[node_name] == 0)
			{
				nodeNameVec.push_back(node_name);
				strMap[node_name] = 1;
			}
		}

		frame->fbxNodes.resize(nodeNameVec.size());
		for (i = 0; i < nodeNameVec.size(); i++)
		{
			frame->fbxNodes[i].nodeName = nodeNameVec[i];
			strNodeMap[frame->fbxNodes[i].nodeName] = &frame->fbxNodes[i];
			frame->fbxNodes[i].nodeParent = NULL;
			frame->fbxNodes[i].nodeIdx = i;
		}

		frame->fbxLinks.resize(25);// lines.size());


	//	for (i = 0; i < lines.size(); i++)
		for (i = 0; i < 25; i++)
		{   // 0          1                     2      3     4    5      6      7
			//RootNode , ZombieResident_Mesh , 0.00, 0.00 ,0.00, -90.00, -0.00 ,0.00
			std::vector<std::string> strvec;
			splitString(lines[i], strvec, ",");
			//FbxNodeObjVec[i].nodeName = strvec[0];
			frame->fbxLinks[i].fromeNode = strNodeMap[trim(strvec[0])];
			frame->fbxLinks[i].toNode = strNodeMap[trim(strvec[1])];
			FbxNode* n1 = frame->fbxLinks[i].fromeNode;
			FbxNode* n2 = frame->fbxLinks[i].toNode;
			//printf("%s\n", lines[i].c_str());
			for (j = 0; j < 3; j++) frame->fbxLinks[i].nodePos[j] = atof(strvec[2 + j].c_str()) / 150.0;
			for (j = 0; j < 3; j++) frame->fbxLinks[i].nodeRot[j] = atof(strvec[5 + j].c_str());
			//nodeDict[FbxNodeObjVec[i].nodeName] = &FbxNodeObjVec[i];
			frame->fbxLinks[i].fromeNode->subNodes.push_back(frame->fbxLinks[i].toNode);
			frame->fbxLinks[i].toNode->nodeParent = frame->fbxLinks[i].fromeNode;
			//frame->fbxLinks[i].linkLength = 20;
			
			frame->fbxLinks[i].linkLength = sqrt(frame->fbxLinks[i].nodePos[0] * frame->fbxLinks[i].nodePos[0] +
				frame->fbxLinks[i].nodePos[1] * frame->fbxLinks[i].nodePos[1] +
				frame->fbxLinks[i].nodePos[2] * frame->fbxLinks[i].nodePos[2]);/**/
			//fbxLinks[i].SetDirection
			printf("link length %6.2lf\n", frame->fbxLinks[i].linkLength);
			frame->fbxLinks[i].linkIdx = i;
		}

		//motionFrames.push_back(frame);
	}

}
///fbxobj.txt
//void RenderSkeletion(char* motion_file, char* save_img_file)
void RenderSkeletion(vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks, char* save_img_file)
{
	GraphDrawLib  graphLib;
	VWordModel   worldModel(&graphLib);

	//vector<FbxNode> fbxNodes;
	//vector<FbxNodeLink> fbxLinks;
	//ReadFileFbxObjList(motion_file, fbxNodes, fbxLinks);
	traverseFbsNodes(&fbxNodes[0], 0);

	SceneCfg scene;

	scene.camx = 1.2; scene.camy = 0.9;  scene.camz = 3.6;  scene.campov = 45;
	scene.rot_angle = 00.0; scene.rot_x = 1.0;  scene.rot_y = 0.0; scene.rot_z = 0;
	scene.trans_x = scene.trans_y = scene.trans_z = 0;
	scene.light_x = 2.0; scene.light_y = 2.0; scene.light_z = 1.8;

	Light::LightNum = 0;
	SetModelScene(&graphLib, worldModel, scene, fbxNodes, fbxLinks);
	//return 1;
	graphLib.InitDrawLib(1600, 1600);
	reshape(&graphLib, 1600, 1600);
	init(&graphLib);
	//  ReadModelFile( argc,  argv );
	//worldModel.MyViewer->Init(argc, argv);
	worldModel.MyViewer->SetValue(BACKCOLOR, GREY);
	worldModel.MyViewer->SetValue(BUFFER, DOUBLE);
	worldModel.MyViewer->CreateWin("Test TkGL", 1600, 1600);

	worldModel.MyViewer->Show(worldModel.Root);
	worldModel.MyViewer->Display0();
	// tkSwapBuffers();
	graphLib.gl_ctx.glXSaveRenderImg(save_img_file);
}



int main(int argc, char *argv[])
{
	vector<CharSkeleton> motionFrames;
	ReadFbxSkeletionMotionFile("skeleton_motion.txt",  motionFrames);
	
	int i;
	for (i = 0; i < 200; i++)
	{
		char filename[200];
		sprintf(filename, "sub\\animation%03i.bmp", i);
		RenderSkeletion(motionFrames[i].fbxNodes, motionFrames[i].fbxLinks, filename);
	}
	
	/*RenderSkeletion("fbxobj1.txt", "img_render.bmp");
	try
	{
		RenderSkeletion("fbxobj.txt", "img_render1.bmp");
	}
	catch (...)
	{
	}
	*/
  return 1;
}

