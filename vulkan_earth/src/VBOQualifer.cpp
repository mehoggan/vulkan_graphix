#include <cstring>
#include "VBOQualifer.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
//#include "macro_crtdbg.h"


VBOQualifer::VBOQualifer(){
	this->redBits=0; this->greenBits=0; this->blueBits=0; this->alphaBits=0; 
	this->depthBits=0; this->stencilBits=0; 
	this->maxTextureSize=0; this->maxLights=0; this->maxClipPlanes=0;
	this->maxModelViewStacks=0; this->maxAttribStacks=0; this->maxTextureStacks=0;
	this->qualified=true;
}

VBOQualifer::~VBOQualifer(){
	/*for(int i=0 ; i<extensions_supported ; i++) {
		if(extensions[i]) {
			printf("I am deleting at (%d) ---------------------\n",i);
			delete [] extensions[i];
			extensions[i]=NULL;
		}
	}
	if(extensions) {
		delete [] extensions;
	}*/
}

bool VBOQualifer::getQualified() {
	return this->qualified;
}

bool VBOQualifer::establishIfQualified() {
	char* str = NULL;
	
	str = (char*)glGetString(GL_VENDOR);
	if(str) this->vendor = str;
	else {
		this->qualified=false;
	}
	
	str = (char*)glGetString(GL_RENDERER);
	if(str) this->renderer = str;
	else {
		this->qualified=false;
	}
	
	str = (char*)glGetString(GL_VERSION);
	if(str) this->version = str;
	else {
		this->qualified = false;
	}
	
	this->extensions_supported=0;
	str = (char*)glGetString(GL_EXTENSIONS);
	if(str){
		char* tok = str;
		int index=0;
		int size=strlen(tok);
		int iteration=0;
		while(iteration++<size){
			if(*(tok + index)==' '||*(tok + index)=='\n'||*(tok + index)=='\t'){
				this->extensions_supported++;
			}
			index++;
		}
		this->extensions=new char*[extensions_supported];			// <--------- THE LEAK!
	}else {
		this->qualified = false;
		exit(0);
	}
	
	char* tok = str;
	int size=strlen(tok);
	int array_index=0;
	int char_pos=0;
	int iteration=0;
	char extension[256];
	while(iteration<size){
		if(*(tok+iteration)!=' '){
			extension[char_pos++]=*(tok+iteration);
		}else{
			this->extensions[array_index]=new char[sizeof(extension)];
			strcpy(this->extensions[array_index],extension);
			array_index++;
			char_pos=0;
			memset(extension,0,256);
		}
		iteration++;
	}
	
	glGetIntegerv(GL_RED_BITS, &this->redBits);
	glGetIntegerv(GL_GREEN_BITS, &this->greenBits);
	glGetIntegerv(GL_BLUE_BITS, &this->blueBits);
	glGetIntegerv(GL_ALPHA_BITS, &this->alphaBits);
	glGetIntegerv(GL_DEPTH_BITS, &this->depthBits);
	glGetIntegerv(GL_STENCIL_BITS, &this->stencilBits);
	glGetIntegerv(GL_MAX_LIGHTS, &this->maxLights);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->maxTextureSize);
	glGetIntegerv(GL_MAX_CLIP_PLANES, &this->maxClipPlanes);
	glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &this->maxModelViewStacks);
	glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &this->maxProjectionStacks);
	glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &this->maxAttribStacks);
	glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &this->maxTextureStacks);

	return this->qualified;
}

bool VBOQualifer::isExtensionSupported(const char* exten) {
	bool extension_exists=false;
	for(int x=0;x<this->extensions_supported;x++){
		const char* compare = this->extensions[x];
		if(strcmp(compare,exten)==0){
			char out[256];
			extension_exists=true;
		}
	}
	return extension_exists;
}