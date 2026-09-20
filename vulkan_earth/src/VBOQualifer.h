#ifndef VBO_QUALIFER_H
#define VBO_QUALIFER_H

#include <string>
#include <vector>


class VBOQualifer {
	public:
		VBOQualifer();
		~VBOQualifer();
		bool getQualified();
		bool establishIfQualified();                             
		bool isExtensionSupported(const char* exten);
	private:
		char* vendor;
		char* renderer;
		char* version;
		int extensions_supported;
		char** extensions;
		int redBits;
		int greenBits;
		int blueBits;
		int alphaBits;
		int depthBits;
		int stencilBits;
		int maxTextureSize;
		int maxLights;
		int maxAttribStacks;
		int maxModelViewStacks;
		int maxProjectionStacks;
		int maxClipPlanes;
		int maxTextureStacks;
		bool qualified;
};

#endif // VBO_QUALIFER_H