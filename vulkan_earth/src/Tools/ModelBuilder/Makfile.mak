SOURCE = main.cpp VBOShaderLibrary.cpp Normal.cpp Vertex.cpp TexCoord.cpp
OBJ = main.obj VBOShaderLibrary.obj Normal.obj Vertex.obj TexCoord.obj
LIBS = glew32.lib

VBOShaderLibrary.exe: $(SOURCE)
	cl.exe /EHs $(SOURCE) $(LIBS)
	link.exe $(OBJ) $(LIBS)
	del *.obj
