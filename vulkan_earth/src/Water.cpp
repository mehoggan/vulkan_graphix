#include <cstdlib>
#include <ctime>
#include <iostream>
#include "math.h"
#include "Vertex.h"
#include "Normal.h"
#include "TexCoord.h"
#include "Shader.h"
#include "Water.h"
#include <GL/glx.h>
//#include "macro_crtdbg.h"

using namespace std;

Water::Water() {
}

Water::Water(int scale, int size) {
	srand(time(NULL));
	vboQualify = NULL;
	this->verifyVBOs();
	this->timer=0.0;
	this->scale=scale;
	this->size=size;
	this->totalVertices=this->size*this->size;
	this->triStripBufferSize=(this->size-1)*(this->size-1)*6;
	this->initData();
	PFNGLGENBUFFERSARBPROC pglGenBuffersARB=NULL;                     // VBO Name Generation Procedure
	PFNGLBINDBUFFERARBPROC pglBindBufferARB=NULL;                     // VBO Bind Procedure
	PFNGLBUFFERDATAARBPROC pglBufferDataARB=NULL;                     // VBO Data Loading Procedure
	PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB=NULL;               // VBO Sub Data Loading Procedure
	PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB=NULL;               // VBO Deletion Procedure
	PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB=NULL; // return various parameters of VBO
	PFNGLMAPBUFFERARBPROC pglMapBufferARB=NULL;                       // map VBO procedure
	PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB=NULL;                   // unmap VBO procedure
	this->prepTerrain();
	this->shader=new Shader();
	this->shader->init("VertexWater.vs","FragmentWater.vs");
	this->color_texture = LoadTexture("Water.raw", 1024, 1024);
	this->normal_texture = LoadTexture("bumpMap.raw",256, 256);
	this->prepareData();
	
}

Water::~Water() {
	if(surfaceheight != NULL){
		for(int i=0;i<size;i++){ 
			delete this->surfaceheight[i];
		}	
		delete this->surfaceheight;
	}
	delete vboQualify;
	delete shader;
	delete [] tex_coord;
	delete [] materialSpecular;
	delete [] materialShininess;
	delete [] materialDiffuse;
	delete [] normals;
	delete [] vertices;
	this->pglDeleteBuffersARB(1,&vertexVBOId);
	this->pglDeleteBuffersARB(1,&normalVBOId);
	this->pglDeleteBuffersARB(1,&textureVBOId);
	glDeleteTextures(1,&color_texture);
	glDeleteTextures(1,&normal_texture);
}

GLint Water::getScale() {return this->scale;}
GLint Water::getActualSize(){ return (this->size)*(this->scale); }

void Water::initData(){
	this->vertices = new Vertex[this->triStripBufferSize];
	this->normals = new Normal[this->triStripBufferSize];
	this->tex_coord = new TexCoord[this->triStripBufferSize];
	this->materialSpecular=new GLfloat[4];
	this->materialSpecular[0]=0.0;
	this->materialSpecular[1]=0.0;
	this->materialSpecular[2]=0.0;
	this->materialSpecular[3]=0.0;
	this->materialShininess=new GLfloat[1];
	this->materialShininess[0]=10000.0;
	this->materialDiffuse=new GLfloat[4];
	this->materialDiffuse[0]=0.0;
	this->materialDiffuse[1]=1.0;
	this->materialDiffuse[2]=0.0;
	this->materialDiffuse[3]=1.0;
}

GLuint Water::LoadTexture(const char * filename, int width, int height) {
	GLuint texture;
	unsigned char * data;
	FILE * file;
	file = fopen(filename, "rb");
	if (file==NULL) {
	}
	this->data=new unsigned char[width*height*3];
	fread(this->data,width*height*3,1,file);
	fclose(file);
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); 
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,this->data);
	free(this->data);
	return texture;
}

void Water::draw() {
	int SIZE = this->size;
	int SCALE = this->scale;
	int BUFFERSIZE = this->triStripBufferSize;
	
	this->shader->bind();
	glEnable(GL_LIGHTING);								// NOT PART OF SHADER CODE
	
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); 
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
    glActiveTexture(GL_TEXTURE0);  
	glEnable(GL_TEXTURE_2D);  
	int texture_location = glGetUniformLocation(shader->id(), "color_texture");  
	glUniform1i(texture_location, 0);  
	glBindTexture(GL_TEXTURE_2D, this->color_texture); 
	
    glActiveTexture(GL_TEXTURE1);  
	glEnable(GL_TEXTURE_2D);  
	int normal_location = glGetUniformLocation(shader->id(), "normal_texture");  
	glUniform1i(normal_location, 1);  
	glBindTexture(GL_TEXTURE_2D, this->normal_texture);

	this->timerLoc = glGetUniformLocation(shader->id(),"timer");
	glUniform1f(this->timerLoc,timer);
	timer+=0.002*3.14159265;
	if(timer>=2*3.14159265) timer=0.0;

	glEnableClientState(GL_NORMAL_ARRAY);					// Enable Normal Arrays
	glEnableClientState(GL_VERTEX_ARRAY);					// Enable Vertex Arrays
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);			// Enable Texture Arrays
	this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB, this->vertexVBOId);
	glVertexPointer(3, GL_FLOAT, 0, (void*)NULL);
	glNormalPointer(GL_FLOAT, 0, (void*)(BUFFERSIZE*sizeof(Vertex)));
	glTexCoordPointer(2, GL_FLOAT, 0, (void*)(BUFFERSIZE*(sizeof(Vertex)+sizeof(Normal))));
	glDrawArrays(GL_TRIANGLES, 0, BUFFERSIZE);
	glDisableClientState(GL_VERTEX_ARRAY);					// Disable Vertex Arrays
	glDisableClientState(GL_NORMAL_ARRAY);					// Disable Vertex Arrays
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);			// Disable Texture Arrays
		
	glActiveTexture(GL_TEXTURE1);  
	glBindTexture(GL_TEXTURE_2D, 0);  
	glDisable(GL_TEXTURE_2D);  
  
	glActiveTexture(GL_TEXTURE0);  
	glBindTexture(GL_TEXTURE_2D, 0);  
	glDisable(GL_TEXTURE_2D);  
	
	glDisable(GL_LIGHTING);									// NOT PART OF SHADER CODE
	this->shader->unbind();
}

void Water::calcAverageofSixNormals(	Vertex* v_0,
					GLfloat x1,GLfloat y1,GLfloat z1,
					GLfloat x2,GLfloat y2,GLfloat z2,
					GLfloat x3,GLfloat y3,GLfloat z3,
					GLfloat x4,GLfloat y4,GLfloat z4,
					GLfloat x5,GLfloat y5,GLfloat z5,
					GLfloat x6,GLfloat y6,GLfloat z6,
					Normal* n){											
	GLfloat u_1X=x1-v_0->coordX; GLfloat u_1Y=y1-v_0->coordY; GLfloat u_1Z=z1-v_0->coordZ;
	GLfloat u_2X=x2-v_0->coordX; GLfloat u_2Y=y2-v_0->coordY; GLfloat u_2Z=z2-v_0->coordZ;
	GLfloat u_3X=x3-v_0->coordX; GLfloat u_3Y=y3-v_0->coordY; GLfloat u_3Z=z3-v_0->coordZ;
	GLfloat u_4X=x4-v_0->coordX; GLfloat u_4Y=y4-v_0->coordY; GLfloat u_4Z=z4-v_0->coordZ;
	GLfloat u_5X=x5-v_0->coordX; GLfloat u_5Y=y5-v_0->coordY; GLfloat u_5Z=z5-v_0->coordZ;
	GLfloat u_6X=x6-v_0->coordX; GLfloat u_6Y=y6-v_0->coordY; GLfloat u_6Z=z6-v_0->coordZ;
	n->compoX+=u_6Y*u_1Z-u_1Y*u_6Z;	n->compoY+=u_6Z*u_1X-u_6X*u_1Z;	n->compoZ+=u_6X*u_1Y-u_1X*u_6Y;
	n->compoX+=u_1Y*u_2Z-u_2Y*u_1Z;	n->compoY+=u_1Z*u_2X-u_1X*u_2Z;	n->compoZ+=u_1X*u_2Y-u_2X*u_1Y;
	n->compoX+=u_2Y*u_3Z-u_3Y*u_2Z;	n->compoY+=u_2Z*u_3X-u_2X*u_3Z;	n->compoZ+=u_2X*u_3Y-u_3X*u_2Y;
	n->compoX+=u_3Y*u_4Z-u_4Y*u_3Z;	n->compoY+=u_3Z*u_4X-u_3X*u_4Z;	n->compoZ+=u_3X*u_4Y-u_4X*u_3Y;
	n->compoX+=u_4Y*u_5Z-u_5Y*u_4Z;	n->compoY+=u_4Z*u_5X-u_4X*u_5Z;	n->compoZ+=u_4X*u_5Y-u_5X*u_4Y;
	n->compoX+=u_5Y*u_6Z-u_6Y*u_5Z;	n->compoY+=u_5Z*u_6X-u_5X*u_6Z;	n->compoZ+=u_5X*u_6Y-u_6X*u_5Y;
	n->compoX/=6;
	n->compoY/=6;
	n->compoZ/=6;
	GLfloat magnitude=sqrt((n->compoX*n->compoX)+(n->compoY*n->compoY)+(n->compoZ*n->compoZ));
	n->compoX/=magnitude;
	n->compoY/=magnitude;
	n->compoZ/=magnitude;
}

void Water::verifyVBOs(){
	if(vboQualify)	delete vboQualify;
	vboQualify = new VBOQualifer();
	vboQualify->establishIfQualified();
	if(vboQualify->getQualified()){
		if(vboQualify->isExtensionSupported("GL_ARB_vertex_buffer_object")){
			this->pglGenBuffersARB=(PFNGLGENBUFFERSARBPROC)glXGetProcAddress((const GLubyte*)"glGenBuffersARB");
			this->pglBindBufferARB=(PFNGLBINDBUFFERARBPROC)glXGetProcAddress((const GLubyte*)"glBindBufferARB");
			this->pglBufferDataARB=(PFNGLBUFFERDATAARBPROC)glXGetProcAddress((const GLubyte*)"glBufferDataARB");
			this->pglBufferSubDataARB=(PFNGLBUFFERSUBDATAARBPROC)glXGetProcAddress((const GLubyte*)"glBufferSubDataARB");
			this->pglDeleteBuffersARB=(PFNGLDELETEBUFFERSARBPROC)glXGetProcAddress((const GLubyte*)"glDeleteBuffersARB");
			this->pglGetBufferParameterivARB=(PFNGLGETBUFFERPARAMETERIVARBPROC)glXGetProcAddress((const GLubyte*)"glGetBufferParameterivARB");
			this->pglMapBufferARB=(PFNGLMAPBUFFERARBPROC)glXGetProcAddress((const GLubyte*)"glMapBufferARB");
			this->pglUnmapBufferARB=(PFNGLUNMAPBUFFERARBPROC)glXGetProcAddress((const GLubyte*)"glUnmapBufferARB");
			if(	this->pglGenBuffersARB&&this->pglBindBufferARB&&this->pglBufferDataARB&&this->pglBufferSubDataARB&&
				this->pglDeleteBuffersARB&&this->pglGetBufferParameterivARB&&this->pglMapBufferARB&&this->pglUnmapBufferARB){
			}else{
				cout << "Pointers to Buffer Functions Failed to be Obtained" << endl;
				exit(0);
			}
		}else {
			cout << "GL_ARB_vertex_buffer_object IS NOT Supported" << endl;
			exit(0);
		}
	}else {
		cout << "ERROR: VBO Creation Failed" << endl;
		exit(0);
	}
}

void Water::prepTerrain(){
	surfaceheight=new int*[this->size];
	for(int i=0;i<this->size;i++){ 
		surfaceheight[i]=new int[this->size];
	}
	for(int y=0; y < size; y++){
		for(int x = 0; x < size; x++){
			surfaceheight[x][y] = -5*100; 
		}
	}
}


void Water::prepareData() {
	int BUFFERSIZE = this->triStripBufferSize;
	int SIZE = this->size;
	int SCALE = this->scale;
	
	//	
	// 				v_k
	//	v_i			v_z
	//		x-----x
	//		|    /|
	//		|  /  |
	//		|/    | 
	//		x-----x
	//	v_j			v_y
	//	v_x
	int index = 0;
	int indexNormals=0;
	int indexTexture=0;
	for(int i=0;i<SIZE-1;i++){
		for(int j=0;j<SIZE-1;j++){
			/************************************************************/
			/*	V_I -- N_I		                            */
			/************************************************************/
			Vertex v_i(j*SCALE,surfaceheight[i][j]/*SCALE*/,i*SCALE);
			this->vertices[index++]=v_i;
			TexCoord t_i(i/((float)SIZE-1),(j)/((float)SIZE-1));
			this->tex_coord[indexTexture++]=t_i;
			Normal n_i(0,0,0);
			if(i==0&&j==0){
				this->calcAverageofSixNormals(	&v_i,v_i.coordX,v_i.coordY,v_i.coordZ,v_i.coordX,v_i.coordY,v_i.coordZ,			
								v_i.coordX,v_i.coordY,v_i.coordZ,v_i.coordX,v_i.coordY,v_i.coordZ,
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),&n_i);
			}else if(i==0){
				this->calcAverageofSixNormals(	&v_i,v_i.coordX,v_i.coordY,v_i.coordZ,v_i.coordX,v_i.coordY,v_i.coordZ,
								(GLfloat)(j-1),surfaceheight[i][j-1],(GLfloat)(i),
								(GLfloat)(j-1),surfaceheight[i+1][j-1],(GLfloat)(i+1),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),&n_i);
			}else if(j==0){
				this->calcAverageofSixNormals(	&v_i,(GLfloat)(j+1),surfaceheight[i-1][j+1],(GLfloat)(i-1),
								(GLfloat)(j),surfaceheight[i-1][j],(GLfloat)(i-1),
								v_i.coordX,v_i.coordY,v_i.coordZ,v_i.coordX,v_i.coordY,v_i.coordZ,
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),&n_i);
			}
			else{
				this->calcAverageofSixNormals(	&v_i,(GLfloat)(j+1),surfaceheight[i-1][j+1],(GLfloat)(i-1),
								(GLfloat)(j),surfaceheight[i-1][j],(GLfloat)(i-1),
								(GLfloat)(j-1),surfaceheight[i][j-1],(GLfloat)(i),
								(GLfloat)(j-1),surfaceheight[i+1][j-1],(GLfloat)(i+1),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),&n_i);
			}
			this->normals[indexNormals++]=n_i;
			/************************************************************/
			/*	V_J -- N_J		                 	    */
			/************************************************************/
			Vertex v_j(j*SCALE,surfaceheight[i+1][j]/*SCALE*/,(i+1)*SCALE);
			this->vertices[index++]=v_j;
			TexCoord t_j((i+1)/((float)SIZE-1),(j)/((float)SIZE-1));
			this->tex_coord[indexTexture++]=t_j;
			Normal n_j(0,0,0);
			if(i==SIZE-2&&j==0){
				this->calcAverageofSixNormals(	&v_j,(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								v_j.coordX,v_j.coordY,v_j.coordZ,v_j.coordX,v_j.coordY,v_j.coordZ,
								v_j.coordX,v_j.coordY,v_j.coordZ,(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_j);
			}else if(j==0){
				this->calcAverageofSixNormals(	&v_j,(GLfloat)j+1,surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								v_j.coordX,v_j.coordY,v_j.coordZ,v_j.coordX,v_j.coordY,v_j.coordZ,
								(GLfloat)(j),surfaceheight[i+2][j],(GLfloat)(i+2),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_j);
			}else if(i==SIZE-2){
				this->calcAverageofSixNormals(	&v_j,(GLfloat)j+1,surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j-1),surfaceheight[i+1][j-1],(GLfloat)(i+1),
								v_j.coordX,v_j.coordY,v_j.coordZ,v_j.coordX,v_j.coordY,v_j.coordZ,
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_j);
			}else{
				this->calcAverageofSixNormals(	&v_j,(GLfloat)j+1,surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j-1),surfaceheight[i+1][j-1],(GLfloat)(i+1),
								(GLfloat)(j-1),surfaceheight[i+2][j-1],(GLfloat)(i+2),
								(GLfloat)(j),surfaceheight[i+2][j],(GLfloat)(i+2),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_j);
			}
			this->normals[indexNormals++]=n_j;
			/************************************************************/
			/*	V_K -- N_K					    */
			/************************************************************/
			Vertex v_k((j+1)*SCALE,surfaceheight[i][j+1]/*SCALE*/,(i)*SCALE);
			this->vertices[index++]=v_k;
			TexCoord t_k(i/((float)SIZE-1),(j+1)/((float)SIZE-1));
			this->tex_coord[indexTexture++]=t_k;
			Normal n_k(0,0,0);
			if(i==0&&j==SIZE-2){
				this->calcAverageofSixNormals(	&v_k,v_k.coordX,v_k.coordY,v_k.coordZ,v_k.coordX,v_k.coordY,v_k.coordZ,
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								v_k.coordX,v_k.coordY,v_k.coordZ,&n_k);
			}else if(i==0){
				this->calcAverageofSixNormals(	&v_k,v_i.coordX,v_i.coordY,v_i.coordZ,v_i.coordX,v_i.coordY,v_i.coordZ,
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								(GLfloat)(j+2),surfaceheight[i][j+2],(GLfloat)(i),&n_k);
			}else if(j==SIZE-2){
				this->calcAverageofSixNormals(	&v_k,v_i.coordX,v_i.coordY,v_i.coordZ,
								(GLfloat)(j+1),surfaceheight[i-1][j+1],(GLfloat)(i-1),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								v_i.coordX,v_i.coordY,v_i.coordZ,&n_k);
			}
			else{
				this->calcAverageofSixNormals(	&v_k,(GLfloat)(j+2),surfaceheight[i-1][j+2],(GLfloat)(i-1),
								(GLfloat)(j+1),surfaceheight[i-1][j+1],(GLfloat)(i-1),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								(GLfloat)(j+2),surfaceheight[i][j+2],(GLfloat)(i),&n_k);
			}
			this->normals[indexNormals++]=n_k;
			/************************************************************/
			/*	V_X -- N_X	(SAME AS V_J/N_J)	            */
			/************************************************************/
			Vertex v_x(j*SCALE,surfaceheight[i+1][j]/*SCALE*/,(i+1)*SCALE);
			this->vertices[index++]=v_x;
			TexCoord t_x((i+1)/((float)SIZE-1),(j)/((float)SIZE-1));
			this->tex_coord[indexTexture++]=t_x;
			Normal n_x(0,0,0);
			if(i==SIZE-2&&j==0){
				this->calcAverageofSixNormals(	&v_x,(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								v_x.coordX,v_x.coordY,v_x.coordZ,v_x.coordX,v_x.coordY,v_x.coordZ,
								v_x.coordX,v_x.coordY,v_x.coordZ,(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_x);
			}else if(j==0){
				this->calcAverageofSixNormals(	&v_x,(GLfloat)j+1,surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),v_x.coordX,v_x.coordY,v_x.coordZ,
								v_x.coordX,v_x.coordY,v_x.coordZ,(GLfloat)(j),surfaceheight[i+2][j],(GLfloat)(i+2),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_x);
			}else if(i==SIZE-2){
				this->calcAverageofSixNormals(	&v_x,(GLfloat)j+1,surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j-1),surfaceheight[i+1][j-1],(GLfloat)(i+1),
								v_x.coordX,v_x.coordY,v_x.coordZ,v_x.coordX,v_x.coordY,v_x.coordZ,
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_x);
			}else{
				this->calcAverageofSixNormals(	&v_x,(GLfloat)j+1,surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j-1),surfaceheight[i+1][j-1],(GLfloat)(i+1),
								(GLfloat)(j-1),surfaceheight[i+2][j-1],(GLfloat)(i+2),
								(GLfloat)(j),surfaceheight[i+2][j],(GLfloat)(i+2),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),&n_x);
			}
			this->normals[indexNormals++]=n_x;
			
			/************************************************************/
			/*	V_Y -- N_Y					    */
			/************************************************************/
			Vertex v_y((j+1)*SCALE,surfaceheight[i+1][j+1]/*SCALE*/,(i+1)*SCALE);
			this->vertices[index++]=v_y;
			TexCoord t_y((i+1)/((float)SIZE-1),(j+1)/((float)SIZE-1));
			this->tex_coord[indexTexture++]=t_y;
			Normal n_y(0,0,0);
			if(i==SIZE-2&&j==SIZE-2){
				this->calcAverageofSixNormals(	&v_y,v_y.coordX,v_y.coordY,v_y.coordZ,(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),v_y.coordX,v_y.coordY,v_y.coordZ,
								v_y.coordX,v_y.coordY,v_y.coordZ,v_y.coordX,v_y.coordY,v_y.coordZ,&n_y);
			}else if(i==SIZE-2){												
				this->calcAverageofSixNormals(	&v_y,(GLfloat)(j+2),surfaceheight[i][j+2],(GLfloat)(i),
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								v_y.coordX,v_y.coordY,v_y.coordZ,v_y.coordX,v_y.coordY,v_y.coordZ,
								(GLfloat)(j+2),surfaceheight[i+1][j+2],(GLfloat)(i+1),&n_y);
			}else if(j==SIZE-2){
				this->calcAverageofSixNormals(	&v_y,v_y.coordX,v_y.coordY,v_y.coordZ,
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j),surfaceheight[i+2][j],(GLfloat)(i+2),
								(GLfloat)(j+1),surfaceheight[i+2][j+1],(GLfloat)(i+2),
								v_y.coordX,v_y.coordY,v_y.coordZ,&n_y);
			}
			else{
				this->calcAverageofSixNormals(	&v_y,(GLfloat)(j+2),surfaceheight[i][j+2],(GLfloat)(i),
								(GLfloat)(j+1),surfaceheight[i][j+1],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j),surfaceheight[i+2][j],(GLfloat)(i+2),
								(GLfloat)(j+1),surfaceheight[i+2][j+1],(GLfloat)(i+2),
								(GLfloat)(j+2),surfaceheight[i+1][j+2],(GLfloat)(i+1),&n_y);
			}
			this->normals[indexNormals++]=n_y;
			/************************************************************/
			/*	V_Z -- N_Z					    */
			/************************************************************/
			Vertex v_z((j+1)*SCALE,surfaceheight[i][j+1]/*SCALE*/,(i)*SCALE);
			this->vertices[index++]=v_z;
			TexCoord t_z(i/((float)SIZE-1),(j+1)/((float)SIZE-1));
			this->tex_coord[indexTexture++]=t_z;
			Normal n_z(0,0,0);
			if(i==0&&j==SIZE-2){
				this->calcAverageofSixNormals(	&v_z,v_z.coordX,v_z.coordY,v_z.coordZ,v_z.coordX,v_z.coordY,v_z.coordZ,
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								v_z.coordX,v_z.coordY,v_z.coordZ,&n_z);
			}else if(i==0){
				this->calcAverageofSixNormals(	&v_z,v_i.coordX,v_i.coordY,v_i.coordZ,
								v_i.coordX,v_i.coordY,v_i.coordZ,
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								(GLfloat)(j+2),surfaceheight[i][j+2],(GLfloat)(i),&n_z);
			}else if(j==SIZE-2){
				this->calcAverageofSixNormals(	&v_z,v_i.coordX,v_i.coordY,v_i.coordZ,(GLfloat)(j+1),surfaceheight[i-1][j+1],(GLfloat)(i-1),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),v_i.coordX,v_i.coordY,v_i.coordZ,&n_z);
			}
			else{
				this->calcAverageofSixNormals(	&v_z,(GLfloat)(j+2),surfaceheight[i-1][j+2],(GLfloat)(i-1),
								(GLfloat)(j+1),surfaceheight[i-1][j+1],(GLfloat)(i-1),
								(GLfloat)(j),surfaceheight[i][j],(GLfloat)(i),
								(GLfloat)(j),surfaceheight[i+1][j],(GLfloat)(i+1),
								(GLfloat)(j+1),surfaceheight[i+1][j+1],(GLfloat)(i+1),
								(GLfloat)(j+2),surfaceheight[i][j+2],(GLfloat)(i),&n_z);
			}
			this->normals[indexNormals++]=n_z;
		}
	}
	this->pglGenBuffersARB(1,&vertexVBOId);								//Create VBO for Vertices
	this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB,vertexVBOId);			
	this->pglBufferDataARB(	GL_ARRAY_BUFFER_ARB, BUFFERSIZE*(sizeof(Vertex)+sizeof(Normal)+sizeof(TexCoord)), 0, GL_DYNAMIC_DRAW_ARB );
	this->pglBufferSubDataARB( GL_ARRAY_BUFFER_ARB, 0, BUFFERSIZE*sizeof(Vertex), this->vertices);
	this->pglBufferSubDataARB( GL_ARRAY_BUFFER_ARB, BUFFERSIZE*sizeof(Vertex), BUFFERSIZE*sizeof(Normal), this->normals);
	this->pglBufferSubDataARB( GL_ARRAY_BUFFER_ARB, BUFFERSIZE*(sizeof(Vertex)+sizeof(Normal)), BUFFERSIZE*sizeof(TexCoord), this->tex_coord);
}
