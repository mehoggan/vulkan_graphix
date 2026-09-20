#include <cstring>
#include "TerrainMaker.h"
#include <GL/glx.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "math.h"
#include "Vertex.h"
#include "Normal.h"
#include "TexCoord.h"
#include "Shader.h"
#include "macro_crtdbg.h"


using namespace std;

TerrainMaker::TerrainMaker(){
}

TerrainMaker::TerrainMaker(int iScale,int iSize){

	srand(time(NULL));
	this->scale=iScale;
	this->size=iSize;
	this->totalVertices=this->size*this->size;
	this->triStripBufferSize=(this->size-1)*(this->size-1)*6;
	this->prepTerrain();
	this->initData();
	PFNGLGENBUFFERSARBPROC pglGenBuffersARB=NULL;                     // VBO Name Generation Procedure
	PFNGLBINDBUFFERARBPROC pglBindBufferARB=NULL;                     // VBO Bind Procedure
	PFNGLBUFFERDATAARBPROC pglBufferDataARB=NULL;                     // VBO Data Loading Procedure
	PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB=NULL;               // VBO Sub Data Loading Procedure
	PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB=NULL;               // VBO Deletion Procedure
	PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB=NULL; // return various parameters of VBO
	PFNGLMAPBUFFERARBPROC pglMapBufferARB=NULL;                       // map VBO procedure
	PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB=NULL;                   // unmap VBO procedure
	
	this->shader=new Shader();
	this->shader->init("VertexShader.vs","FragmentShader.vs");
	this->color_texture = LoadTexture("Rocky.raw", 2048, 2048);
	this->normal_texture = LoadTexture("bumpMap.raw",256, 256); 
	
	this->rotation_angle = 0.0;
	vboQualify = NULL;
	this->verifyVBOs();
	this->wireframeActive=false;
}

TerrainMaker::~TerrainMaker(){
	if(this->th){
		for(int i=0;i<size;i++){ 
			delete this->th[i];
		}
		delete this->th;

	}
	delete vboQualify;
	delete shader;
	delete [] materialSpecular;
	delete [] materialShininess;
	delete [] materialDiffuse;
	delete [] normals;
	delete [] vertices;
	delete [] tex_coord;
	this->pglDeleteBuffersARB(1,&vertexVBOId);
	this->pglDeleteBuffersARB(1,&normalVBOId);
	this->pglDeleteBuffersARB(1,&textureVBOId);
	glDeleteTextures(1,&color_texture);
	glDeleteTextures(1,&normal_texture);
}

GLint TerrainMaker::getScale() {return this->scale;}
GLint TerrainMaker::getActualSize(){ return (this->size)*(this->scale); }

GLuint TerrainMaker::selectTexture(char* tex){
	glDeleteTextures(1,&this->color_texture);

	if(strcmp(tex,"Rock")==0)
		return LoadTexture("Rocky.raw", 2048, 2048);
	else if(strcmp(tex,"Snow")==0)
		return LoadTexture("Snowy.raw", 2048, 2048);
	else if(strcmp(tex,"Ice")==0)		
		return LoadTexture("Icy.raw", 2048, 2048);
	else if(strcmp(tex,"Mars")==0)
		return LoadTexture("RedPlanet.raw", 2048, 2048);
	else if(strcmp(tex,"Desert")==0)
		return LoadTexture("Desert.raw", 2048, 2048);
	else if(strcmp(tex,"Lava")==0)
		return LoadTexture("LavaRock.raw", 2048, 2048);
	else return 0;

}

GLuint TerrainMaker::LoadTexture(const char * filename, int width, int height) {
    GLuint texture;
    unsigned char * data;
    FILE * file;
    file = fopen(filename, "rb");
    if (file==NULL) return 0;
    data=new unsigned char[width*height*3];
    fread(data,width*height*3,1,file);
    fclose(file);
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); 
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    delete[] data;
    return texture;
}

void TerrainMaker::draw() {
	int SIZE = this->size;
	int SCALE = this->scale;
	int BUFFERSIZE = this->triStripBufferSize;
	glEnable(GL_COLOR_MATERIAL);

	if(wireframeActive){
		glColor4f(0,0,0,.75);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnableClientState(GL_VERTEX_ARRAY);
		this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB, this->vertexVBOId);
		glVertexPointer(3, GL_FLOAT, 0, (void*)NULL);
		glDrawArrays(GL_TRIANGLES, 0, BUFFERSIZE);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor4f(1,1,1,.75);
		for(int i=0;i<255;i+=4){
			for(int j=0;j<255;j+=4){
				glBegin(GL_LINES);
				/*glVertex3f(i*scale,th[j][i],j*scale);
				glVertex3f(	i*scale+500*this->normals[(i*255+j)*6].compoX,
							th[j][i]+500*this->normals[(i*255+j)*6].compoY,
							j*scale+500*this->normals[(i*255+j)*6].compoZ);*/
				glVertex3f(i*scale,th[j][i],j*scale);
				glVertex3f(	i*scale+500*this->getNormalAt(i*scale,j*scale).compoX,
							th[j][i]+500*this->getNormalAt(i*scale,j*scale).compoY,
							j*scale+500*this->getNormalAt(i*scale,j*scale).compoZ);
				glEnd();
			}
		}
	} else {
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

		
		glEnableClientState(GL_NORMAL_ARRAY);					
		glEnableClientState(GL_VERTEX_ARRAY);					
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);			
		this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB, this->vertexVBOId);
		glVertexPointer(3, GL_FLOAT, 0, (void*)NULL);
		glNormalPointer(GL_FLOAT, 0, (void*)(BUFFERSIZE*sizeof(Vertex)));
		glTexCoordPointer(2, GL_FLOAT, 0, (void*)(BUFFERSIZE*(sizeof(Vertex)+sizeof(Normal))));
		glDrawArrays(GL_TRIANGLES, 0, BUFFERSIZE);
		glDisableClientState(GL_VERTEX_ARRAY);					
		glDisableClientState(GL_NORMAL_ARRAY);					
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);			
			
		glActiveTexture(GL_TEXTURE1);  
		glBindTexture(GL_TEXTURE_2D, 0);  
		glDisable(GL_TEXTURE_2D);  
	  
		glActiveTexture(GL_TEXTURE0);  
		glBindTexture(GL_TEXTURE_2D, 0);  
		glDisable(GL_TEXTURE_2D);  
		
		glDisable(GL_LIGHTING);									// NOT PART OF SHADER CODE
		this->shader->unbind();
	}
	glDisable(GL_COLOR_MATERIAL);
}

void TerrainMaker::initData(){
	this->vertices = new Vertex[this->triStripBufferSize];
	this->normals = new Normal[this->triStripBufferSize];
	this->tex_coord = new TexCoord[this->triStripBufferSize];
	this->materialSpecular=new GLfloat[4];
	this->materialSpecular[0]=1.0;
	this->materialSpecular[1]=1.0;
	this->materialSpecular[2]=1.0;
	this->materialSpecular[3]=1.0;
	this->materialShininess=new GLfloat[1];
	this->materialShininess[0]=10000.0;
	this->materialDiffuse=new GLfloat[4];
	this->materialDiffuse[0]=0.0;
	this->materialDiffuse[1]=1.0;
	this->materialDiffuse[2]=0.0;
	this->materialDiffuse[3]=1.0;
}

void TerrainMaker::smoothShadeNormal(int x,int z,Normal* n){
	calcNormal(x,z-1,0,n);
	Normal n0(n->compoX,n->compoY,n->compoZ);
	calcNormal(x,z,0,n);
	Normal n1(n->compoX,n->compoY,n->compoZ);
	calcNormal(x,z,1,n);
	Normal n2(n->compoX,n->compoY,n->compoZ);
	calcNormal(x+1,z+1,1,n);
	Normal n3(n->compoX,n->compoY,n->compoZ);
	calcNormal(x+1,z,1,n);
	Normal n4(n->compoX,n->compoY,n->compoZ);
	calcNormal(x+1,z,0,n);
	Normal n5(n->compoX,n->compoY,n->compoZ);

	n->compoX=(n0.compoX+n1.compoX+n2.compoX+n3.compoX+n4.compoX+n5.compoX)/6;
	n->compoY=(n0.compoY+n1.compoY+n2.compoY+n3.compoY+n4.compoY+n5.compoY)/6;
	n->compoZ=(n0.compoZ+n1.compoZ+n2.compoZ+n3.compoZ+n4.compoZ+n5.compoZ)/6;
}

void TerrainMaker::calcNormal(int x,int z,int flag, Normal* n){
	float v1[3],v2[3];
	bool can_calculate = true;
	if(flag==1){
		if(((x-1) >= 0) && ((z-1) >= 0) && (x < this->size) && (z < this->size)) {
			v1[0]=((x-1)*scale)-(x*scale);
			v1[1]=(th[x-1][z-1])-th[x][z];
			v1[2]=((z-1)*scale)-(z*scale);

			v2[0]=((x-1)*scale)-(x*scale);
			v2[1]=(th[x-1][z])-th[x][z];
			v2[2]=0;
		} else {
			can_calculate = false;
		}
	} else {
		if((((x - 1) >= 0)&&(x<this->size)) && ((z + 1) < this->size ) && (z > 0)) {
			v1[0]=((x-1)*scale)-(x*scale);
			v1[1]=(th[x-1][z])-th[x][z];
			v1[2]=0;

			v2[0]=0;
			v2[1]=(th[x][z+1])-th[x][z];
			v2[2]=(((z+1)*scale)-(z*scale));
		} else {
			can_calculate = false;
		}
	}

	if(can_calculate) {
		n->compoX=v1[1]*v2[2]-v1[2]*v2[1];
		n->compoY=v1[2]*v2[0]-v1[0]*v2[2];
		n->compoZ=v1[0]*v2[1]-v1[1]*v2[0];
		float mag=sqrt((n->compoX*n->compoX)+(n->compoY*n->compoY)+(n->compoZ*n->compoZ));
		n->compoX/=mag;
		n->compoY/=mag;
		n->compoZ/=mag;
	} else {
		n->compoX = 0;
		n->compoY = 1;
		n->compoZ = 0;
	}
}

void TerrainMaker::prepareData(int steps, int increase, float radius, int randomJump, int smoothness){
	int chunkSize=this->size/2;
	this->steps=steps;
	this->increase=increase;
	this->radius=radius;
	this->randomJump=randomJump;
	int BUFFERSIZE = this->triStripBufferSize;
	int SIZE = this->size;
	int SCALE = this->scale;
	this->terrainGen(steps,increase,radius,randomJump);
	for(int i=-1;i<smoothness;i++) this->terrainSmoothe(10);	
	
	//	
	// 				v_k
	//	v_i			v_z
	//		x-----x
	//		|    /|
	//		|  /  |
	//		|/	  | 
	//		x-----x
	//	v_j			v_y
	//	v_x
	int index = 0;
	int indexNormals=0;
	int indexTexture=0;
	for(int i=0;i<SIZE-1;i++){
		for(int j=0;j<SIZE-1;j++){
			/************************************************************/
			/*	V_I -- N_I												*/
			/************************************************************/
			Vertex v_i(j*SCALE,this->th[i][j]/*SCALE*/,i*SCALE);
			this->vertices[index++]=v_i;
			TexCoord t_i(	((float)(i%(chunkSize-1)))/(float)(chunkSize-1),
							((float)(j%(chunkSize-1)))/(float)(chunkSize-1));
			this->tex_coord[indexTexture++]=t_i;
			Normal n_i(0,0,0);
			if(i==0&&j==0){
			}else if(i==0){
			}else if(j==0){
			}else{
				smoothShadeNormal(j,i,&n_i);
			}
			this->normals[indexNormals++]=n_i;
			
			/************************************************************/
			/*	V_J -- N_J												*/
			/************************************************************/
			Vertex v_j(j*SCALE,this->th[i+1][j]/*SCALE*/,(i+1)*SCALE);
			this->vertices[index++]=v_j;
			TexCoord t_j(	((float)(i%(chunkSize-1))+1)/(float)(chunkSize-1),
							((float)(j%(chunkSize-1)))/(float)(chunkSize-1));
			this->tex_coord[indexTexture++]=t_j;
			Normal n_j(0,0,0);
			if(i==SIZE-2&&j==0){
			}else if(j==0){
			}else if(i==SIZE-2){
			}else{
				smoothShadeNormal(j,i+1,&n_j);
			}
			this->normals[indexNormals++]=n_j;
			
			/************************************************************/
			/*	V_K -- N_K												*/
			/************************************************************/
			Vertex v_k((j+1)*SCALE,this->th[i][j+1]/*SCALE*/,(i)*SCALE);
			this->vertices[index++]=v_k;
			TexCoord t_k(	((float)(i%(chunkSize-1)))/(float)(chunkSize-1),
							((float)(j%(chunkSize-1))+1)/(float)(chunkSize-1));
			this->tex_coord[indexTexture++]=t_k;
			Normal n_k(0,0,0);
			if(i==0&&j==SIZE-2){
			}else if(i==0){
			}else if(j==SIZE-2){
			}else{
				smoothShadeNormal(j+1,i,&n_k);
			}
			this->normals[indexNormals++]=n_k;
			
			/************************************************************/
			/*	V_X -- N_X	(SAME AS V_J/N_J)							*/
			/************************************************************/
			Vertex v_x(j*SCALE,this->th[i+1][j]/*SCALE*/,(i+1)*SCALE);
			this->vertices[index++]=v_x;
			TexCoord t_x(	((float)(i%(chunkSize-1))+1)/(float)(chunkSize-1),
							((float)(j%(chunkSize-1)))/(float)(chunkSize-1));
			this->tex_coord[indexTexture++]=t_x;
			Normal n_x(0,0,0);
			if(i==SIZE-2&&j==0){
			}else if(j==0){
			}else if(i==SIZE-2){
			}else{
				smoothShadeNormal(j,i+1,&n_x);
			}
			this->normals[indexNormals++]=n_x;
			
			/************************************************************/
			/*	V_Y -- N_Y												*/
			/************************************************************/
			Vertex v_y((j+1)*SCALE,this->th[i+1][j+1]/*SCALE*/,(i+1)*SCALE);
			this->vertices[index++]=v_y;
			TexCoord t_y(	((float)(i%(chunkSize-1))+1)/(float)(chunkSize-1),
							((float)(j%(chunkSize-1))+1)/(float)(chunkSize-1));
			this->tex_coord[indexTexture++]=t_y;
			Normal n_y(0,0,0);
			if(i==SIZE-2&&j==SIZE-2){
			}else if(i==SIZE-2){												
			}else if(j==SIZE-2){
			}else{
				smoothShadeNormal(j+1,i+1,&n_y);
			}
			this->normals[indexNormals++]=n_y;
			
			/************************************************************/
			/*	V_Z -- N_Z												*/
			/************************************************************/
			Vertex v_z((j+1)*SCALE,this->th[i][j+1]/*SCALE*/,(i)*SCALE);
			this->vertices[index++]=v_z;
			TexCoord t_z(	((float)(i%(chunkSize-1)))/(float)(chunkSize-1),
							((float)(j%(chunkSize-1))+1)/(float)(chunkSize-1));
			this->tex_coord[indexTexture++]=t_z;
			Normal n_z(0,0,0);
			if(i==0&&j==SIZE-2){
			}else if(i==0){			
			}else if(j==SIZE-2){
			}else{		
				smoothShadeNormal(j+1,i,&n_z);
			}
			this->normals[indexNormals++]=n_z;
		}
	}
		
	this->pglGenBuffersARB(1,&vertexVBOId);													//Create VBO for Vertices
	this->pglBindBufferARB(GL_ARRAY_BUFFER_ARB,vertexVBOId);			
	this->pglBufferDataARB(	GL_ARRAY_BUFFER_ARB, 
							BUFFERSIZE*(sizeof(Vertex)+sizeof(Normal)+sizeof(TexCoord)),
							0, 
							GL_DYNAMIC_DRAW_ARB );						

	this->pglBufferSubDataARB(	GL_ARRAY_BUFFER_ARB, 
								0,
								BUFFERSIZE*sizeof(Vertex), 
								this->vertices);

	this->pglBufferSubDataARB(	GL_ARRAY_BUFFER_ARB, 
								BUFFERSIZE*sizeof(Vertex), 
								BUFFERSIZE*sizeof(Normal), 
								this->normals);
								
	this->pglBufferSubDataARB(	GL_ARRAY_BUFFER_ARB, 
								BUFFERSIZE*(sizeof(Vertex)+sizeof(Normal)), 
								BUFFERSIZE*sizeof(TexCoord), 
								this->tex_coord);
}

void TerrainMaker::verifyVBOs(){
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
				this->stdMessageBox("Pointers to Buffer Functions Failed to be Obtained");
				exit(0);
			}
		}else {
			this->stdMessageBox("GL_ARB_vertex_buffer_object IS NOT Supported");
			exit(0);
		}
	}else {
		this->stdMessageBox("VBOs Creation Failed");
		exit(0);
	}
}

void TerrainMaker::prepTerrain(){
	this->th=new int*[this->size];					
	for(int i=0;i<this->size;i++){ 
		this->th[i]=new int[this->size];			
	}
	for(int y=0; y < size; y++){
		for(int x = 0; x < size; x++){
			this->th[x][y] = 0; 
		}
	}
}

void TerrainMaker::terrainGen(int steps, int increase, float radius, int randomJump){
	float currentX=this->size/2;
	float currentY=this->size/2;
	float distance=0;

	for(int i=0;i<this->size;i++){
		for(int j=0;j<this->size;j++){
			th[i][j]=0;
		}
	}

	for(int currentStep=1;currentStep<steps;currentStep++){

		int random=(rand()%100);

		if (random>randomJump){
			switch((int)(rand()%4)){
				case 0 : currentX--; break;
				case 1 : currentX++; break;
				case 2 : currentY--; break;
				case 3 : currentY++; break;
			}
			if(((currentX>=this->size)||(currentX<0))||((currentY>=this->size)||(currentY<0))){
				currentX=(int)(rand()%this->size);
				currentY=(int)(rand()%this->size);
			}
		} else {
			currentX=(int)(rand()%this->size);
            currentY=(int)(rand()%this->size);
		}

		for(int x=currentX-radius;x<currentX+radius;x++)
			for(int y=currentY-radius;y<currentY+radius;y++){
				distance=(float)sqrt(pow((double)(currentX-x),2)+pow((double)currentY-y,2));
				if((distance<radius)&&((x>=0 && x<this->size)&&(y>=0&&y<this->size))) 
					th[x][y]+=increase;
			}
	}
}

void TerrainMaker::terrainSqDi(int left, int right, int top, int bottom, int seed, int subtract){
	if((left-right) == 0 || (bottom-top) == 1 ){}//DO NOTHING BUT RETURN
	else{
		while(seed <= 1){
			seed = rand();
			if (seed<0){
				seed*=-1;
			} 
		} 

		this->th[top][left] = rand()%seed;
		this->th[bottom][left] = rand()%seed;
		this->th[top][right] = rand()%seed;
		this->th[bottom][right] = rand()%seed;
		this->th[(top+bottom)/2][(left+right)/2] = 	(((this->th[top][left])+(this->th[bottom][left])+
													(this->th[top][right])+(this->th[bottom][right]))/4)+rand()%seed;

		this->th[(top+bottom)/2][left] = rand()%seed;
		this->th[bottom][(left+right)/2] = rand()%seed;
		this->th[top][(left+right)/2] = rand()%seed;
		this->th[(top+bottom)/2][right] = rand()%seed;
		this->th[(top+bottom)/2][(left+right)/2] = 	(((this->th[(top+bottom)/2][left])+(this->th[bottom][(left+right)/2])+
													(this->th[top][(left+right)/2] = rand()%seed)+(this->th[(top+bottom)/2][right]))/4)+rand()%seed;				

		seed -= subtract;
		this->terrainSqDi(left,(left+right)/2,top,(top+bottom)/2, seed, subtract);
		this->terrainSqDi(left,(left+right)/2,(top+bottom)/2,bottom, seed, subtract);
		this->terrainSqDi((left+right)/2,right,top,(top+bottom)/2, seed, subtract);
		this->terrainSqDi((left+right)/2,right,(top+bottom)/2,bottom, seed, subtract);																						
	}
}

void TerrainMaker::terrainSmoothe(int box_width){
	for(int y = 0; y < this->size; y++){
		for(int x = 0; x < this->size; x++){
			int sum = 0;
			for(int i = y - (box_width/2); i < y + (box_width/2); i++){
				for(int j = x - (box_width/2); j < x + (box_width/2); j++){
					if((i >= 0 && i < this->size) && (j >= 0 && j < this->size)) {
						sum += this->th[i][j];
					}
				}
			}
			this->th[y][x] = sum/(box_width*box_width);
		}
	}

	//*
	for(int y = 0; y < this->size; y++){
		for(int x = 0 ; x < this->size; x++){
			if(x == 0 || y == 0){
				this->th[y][x] = 0;
			}
			if(x == 0 || y == this->size-1){
				this->th[y][x] = 0;
			}
			if(x == this->size-1 || y == 0){
				this->th[y][x] = 0;
			}
			if(x == this->size-1 || y == this->size-1){
				this->th[y][x] = 0;
			}
		}
	}
	//*/
}

void TerrainMaker::terrainSlope(int vertices){
	for(int i=0;i<this->size;i++){
		for(int j=0;j<this->size;j++){
			if(i<vertices){
				th[i][j]-=(vertices-i)*this->scale;
			}
			else if(i>this->size-vertices){
				th[i][j]-=(vertices-(this->size-i))*this->scale;
			}
			if(j<vertices){
				th[i][j]-=(vertices-j)*this->scale;
			
			}
			else if (j>this->size-vertices){
				th[i][j]-=(vertices-(this->size-j))*this->scale;
			}			
		}
	}
}

void TerrainMaker::toggleWireframe(){
	if(wireframeActive) wireframeActive=false;
	else wireframeActive=true;
}

Normal TerrainMaker::getTriangleNormal(float x, float z) {
	/*	NORMAL ORIENTATION VECTOR CODE FOR TANK ORIENTATION	*/
	Vertex* v0 = new Vertex(0,0,0);
	Vertex* v1 = new Vertex(0,0,0);
	Vertex* v2 = new Vertex(0,0,0);
	Vertex* vert_collection[3] = {v0, v1, v2};
	collectVerticesForTriangleNormal(x, z, vert_collection);

	GLfloat u[3] = {(v1->coordX - v0->coordX), (v1->coordY/100 - v0->coordY/100), (v1->coordZ - v0->coordZ)};
	GLfloat v[3] = {(v2->coordX - v0->coordX), (v2->coordY/100 - v0->coordY/100), (v2->coordZ - v0->coordZ)};

	Normal n	(	
					(u[1]*v[2] - v[1]*u[2]), 
					(u[2]*v[0] - u[0]*v[2]), 
					(u[0]*v[1] - v[0]*u[1])
				);

	GLfloat mag = (GLfloat)sqrt(pow((double)(n.compoX),2.0) + pow((double)(n.compoY),2.0) + pow((double)(n.compoZ),2.0)); 
	n.compoX /= mag;
	n.compoY /= mag;
	n.compoZ /= mag;
	if(n.compoY < 0) {
		n.compoX *= -1;
		n.compoY *= -1;
		n.compoZ *= -1;
	}
	delete vert_collection[0];
	delete vert_collection[1];
	delete vert_collection[2];
	return Normal(n.compoX,n.compoY,n.compoZ);
}

Normal TerrainMaker::getNormalAt(GLfloat x, GLfloat z){
	int nX=(int)(x/this->scale);
	int nZ=(int)(z/this->scale);
	float dX=x/this->scale-nX;
	float dZ=z/this->scale-nZ;
	
	if(dX>0){
		if(abs(dX)>0.5){
			nX++;
		}
	} else {
		if(abs(dX)<0.5){
			nX++;
		}
	}
	if(dZ>0){
		if(abs(dZ)>0.5){
			nZ++;
		}
	} else {
		if(abs(dZ)<0.5){
			nZ++;
		}
	}
	float N[3];
	float V[3] = {0, th[nZ][nX+1]-th[nZ][nX], this->scale};
	float U[3] = {this->scale, th[nZ+1][nX]-th[nZ][nX], 0};

	N[0] = (V[1]*U[2] - U[1]*V[2]);
	N[1] = (U[0]*V[2] - V[0]*U[2]); 
	N[2] = (V[0]*U[1] - U[0]*V[1]);

	float mag=sqrt(N[0]*N[0]+N[1]*N[1]+N[2]*N[2]);
	N[0]/=mag;
	N[1]/=mag;
	N[2]/=mag;
	return Normal(N[0],N[1],N[2]);
	/*int i;
	for(i=0;i<(this->size-1)*(this->size-1)*6;i++){
		if((this->vertices[i].coordX==nX*this->scale)&&(this->vertices[i].coordZ==nZ*this->scale)) break;
	}
	return &this->normals[i];*/
}

GLfloat TerrainMaker::getHeightAt(GLfloat x, GLfloat z){
	if((x >= 0 && x < (this->size-1)*this->scale) && (z >= 0 && z < (this->size-1)*this->scale)) {
		int vX=(int)(x/this->scale);
		int vZ=(int)(z/this->scale);
		float dX=x/this->scale-vX;
		float dZ=z/this->scale-vZ;
	
		if(dX<0){
			if(abs(dX)<0.5){
				vX++;
			}
		} else {
			if(abs(dX)>0.5){
				vX++;
			}
		}
		if(dZ<0){
			if(abs(dZ)<0.5){
				vZ++;
			}
		} else {
			if(abs(dZ)>0.5){
				vZ++;
			}
		}
		return this->th[vX][vZ];
	}
	return 0.0;
}

void TerrainMaker::collectVerticesForTriangleNormal(int x, int z, Vertex* three_vertices_array[3]) {
	if((x >= 0 && x < this->size) && (z >= 0 && z < this->size)) {
		three_vertices_array[0]->coordX = x;
		three_vertices_array[0]->coordY = this->th[z][x];
		three_vertices_array[0]->coordZ = z;

		three_vertices_array[1]->coordX = x + 1;
		three_vertices_array[1]->coordY = this->th[z][x + 1];
		three_vertices_array[1]->coordZ = z;

		three_vertices_array[2]->coordX = x;
		three_vertices_array[2]->coordY = this->th[z + 1][x];
		three_vertices_array[2]->coordZ = z + 1;
	}else {
		cout << "Tank out of bounds" << endl;
	}
}

void TerrainMaker::makeCrater(GLfloat fx, GLfloat fz,GLfloat blastSize){
	int x=(int)(fx/this->scale);
	int z=(int)(fz/this->scale);
	Vertex* bufferPtr=(Vertex*)this->pglMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_READ_WRITE);
	int craterSize=(int)(blastSize*1.5);

	if(((x>=0)&&(x<size))&&((z>=0)&&(z<size))){
		GLfloat impactY=th[z][x];
		for(int i=x-craterSize;i<x+craterSize;i++){
			for(int j=z-craterSize;j<z+craterSize;j++){
				GLfloat distance=sqrt((float)((x-i)*(x-i)+(z-j)*(z-j)));
				if((((i >= 0) && (j >= 0)) && ((i < this->size) && (j < this->size)))&&(distance<=blastSize)) {
					GLfloat xDis,yDis,zDis,radius;
					xDis=abs(i-x);
					zDis=abs(j-z);
					radius=sqrt(xDis*xDis+zDis*zDis);

					GLfloat damageDepth=-((sqrt(radius*radius+xDis*xDis+zDis*zDis)-blastSize*2)*this->scale/2);
					GLfloat adjustHeight=th[j][i];
					if(adjustHeight>(impactY+(damageDepth))){
						adjustHeight-=damageDepth;
					} else if(adjustHeight>(impactY-damageDepth)){
						adjustHeight=impactY-damageDepth;
					}
					//lower vertex height in local th[][]
					//lower vertex height for the 6 vertices that share the same location in VBO
					th[j][i]=adjustHeight;				//local														
					if(i == x && j == z && j > 1 && i > 1) {
						adjustHeight=(th[j-1][i]+th[j][i-1]+th[j][i])/3;
					}
					th[j][i]=adjustHeight;				//local
					bufferPtr[(j*(this->size-1)+i)*6].coordY=adjustHeight;	//VBO
					if((j-1)>0){
 						bufferPtr[((j-1)*(this->size-1)+i)*6+1].coordY=adjustHeight;//VBO
						bufferPtr[((j-1)*(this->size-1)+i)*6+3].coordY=adjustHeight;//VBO
						if((i-1)>0){
							bufferPtr[((j-1)*(this->size-1)+(i-1))*6+4].coordY=adjustHeight;//VBO
						}
					}
					if((i-1)>0){
						bufferPtr[((j)*(this->size-1)+(i-1))*6+2].coordY=adjustHeight;//VBO
						bufferPtr[((j)*(this->size-1)+(i-1))*6+5].coordY=adjustHeight;//VBO
					}
				}
			}
		}

		//adjust normals
	
		int normalOffset=this->triStripBufferSize;
		for(int i=x-craterSize;i<x+craterSize;i++){
			for(int j=z-craterSize;j<z+craterSize;j++){
				Normal adjustNormal;
				GLfloat distance=sqrt((float)((x-i)*(x-i)+(z-j)*(z-j)));
				if((i >= 0 && j >= 0 && i < this->size && j < this->size)&&(distance<=blastSize)) {
					this->smoothShadeNormal(j,i,&adjustNormal);
					bufferPtr[normalOffset+(j*(this->size-1)+i)*6].coordX=adjustNormal.compoX;	//VBO
					bufferPtr[normalOffset+(j*(this->size-1)+i)*6].coordY=adjustNormal.compoY;	//VBO
					bufferPtr[normalOffset+(j*(this->size-1)+i)*6].coordZ=adjustNormal.compoZ;	//VBO
					if((j-1)>0){
						bufferPtr[normalOffset+((j-1)*(this->size-1)+i)*6+1].coordX=adjustNormal.compoX;//VBO
						bufferPtr[normalOffset+((j-1)*(this->size-1)+i)*6+1].coordY=adjustNormal.compoY;//VBO
						bufferPtr[normalOffset+((j-1)*(this->size-1)+i)*6+1].coordZ=adjustNormal.compoZ;//VBO
						bufferPtr[normalOffset+((j-1)*(this->size-1)+i)*6+3].coordX=adjustNormal.compoX;//VBO
						bufferPtr[normalOffset+((j-1)*(this->size-1)+i)*6+3].coordY=adjustNormal.compoY;//VBO
						bufferPtr[normalOffset+((j-1)*(this->size-1)+i)*6+3].coordZ=adjustNormal.compoZ;//VBO
						if((i-1)>0){
							bufferPtr[normalOffset+((j-1)*(this->size-1)+(i-1))*6+4].coordX=adjustNormal.compoX;//VBO
							bufferPtr[normalOffset+((j-1)*(this->size-1)+(i-1))*6+4].coordY=adjustNormal.compoY;//VBO
							bufferPtr[normalOffset+((j-1)*(this->size-1)+(i-1))*6+4].coordZ=adjustNormal.compoZ;//VBO
						}
					}
					if((i-1)>0){
						bufferPtr[normalOffset+((j)*(this->size-1)+(i-1))*6+2].coordX=adjustNormal.compoX;//VBO
						bufferPtr[normalOffset+((j)*(this->size-1)+(i-1))*6+2].coordY=adjustNormal.compoY;//VBO
						bufferPtr[normalOffset+((j)*(this->size-1)+(i-1))*6+2].coordZ=adjustNormal.compoZ;//VBO
						bufferPtr[normalOffset+((j)*(this->size-1)+(i-1))*6+5].coordX=adjustNormal.compoX;//VBO
						bufferPtr[normalOffset+((j)*(this->size-1)+(i-1))*6+5].coordY=adjustNormal.compoY;//VBO
						bufferPtr[normalOffset+((j)*(this->size-1)+(i-1))*6+5].coordZ=adjustNormal.compoZ;//VBO
					}
				}
			}
		}
	}
	this->pglUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

/************************************************************************/
/*	Debugging MessageBox functions										*/
/************************************************************************/
void TerrainMaker::stdMessageBox(char* output){

}

void TerrainMaker::errorMessageBox(char* output){

}

