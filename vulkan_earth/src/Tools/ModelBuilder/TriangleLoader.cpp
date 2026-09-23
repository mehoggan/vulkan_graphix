#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <vector> 
#include <iostream> 
#include <istream> 
#include <ostream> 
#include <iterator> 
#include <sstream> 
#include <algorithm>
#include <fstream>
#include <math.h>

/*	TO RUN PROGRAM		*/
//triangleLoader.exe Test.x3d Test.ogl [smooth]

using namespace std;

void crossProduct(float Ox, float Oy, float Oz, float Ux, float Uy, float Uz, float Vx, float Vy, float Vz, float* N) {
	float O[3] = {Ox, Oy, Oz};
	float U[3] = {Ux-Ox, Uy-Oy, Uz-Oz};
	float V[3] = {Vx-Ox, Vy-Oy, Vz-Oz};

	N[0] = (Vy*Uz - Uy*Vz);
	N[1] = (Ux*Vz - Vx*Uz); 
	N[2] = (Vx*Uy - Ux*Vy);

	float mag=sqrt(N[0]*N[0]+N[1]*N[1]+N[2]*N[2]);
	N[0]/=mag;
	N[1]/=mag;
	N[2]/=mag;
	
}

int main (int argc, char* argv[]) {
	unsigned int c;
	unsigned int red, green, blue;
	bool done = false;
	bool smooth = false;
	if((argc==4)&&(strcmp(argv[3],"smooth")==0)){
		smooth=true;
	}

	if(argc < 3) {
		printf("Error: You Need To Specify the File To Write To\n");
		exit(0);
	}else {
		printf("Opening Files\n");

		FILE* x3d_file = fopen(argv[1], "r");
		ofstream outputFile;
		outputFile.open(argv[2]);		// open output file for writing and write "header"

		if(!x3d_file) {
			printf("You must specify a file to read from\n");
			exit(0);
		}
		
		fseek(x3d_file,0,SEEK_END);
		int read_file_size = ftell(x3d_file);
		rewind(x3d_file);
		
		char* buffer;
		buffer = (char*)malloc(sizeof(char)*read_file_size);
		size_t result = fread(buffer,1,read_file_size,x3d_file);

		string str(buffer);
		stringstream strstr(str);
		istream_iterator<std::string> it(strstr); 
		istream_iterator<std::string> end;   
		vector<string> results(it, end);
		vector<float> Vertices(0);
		vector<float> TexCoord(0);
		vector<float> Normal(0);
		vector<int> Indices(0);
		vector<int> TexIndices(0);
		for(int x = 0; x < results.size(); x++) {
			if(results[x].compare("coordIndex=\"") == 0) {
				cout << "START OF VERTEX INDICES" << endl;
				int i=0;
				x=x+1;
				while(results[x].compare("\"") != 0 && results[x].compare("") != 0) { 
					std::istringstream iss(results[x]);
					iss >> i >> std::dec;
					if(i != -1) {
						Indices.push_back((int)i);
						cout << i << ", ";
					}
					x++;
				}
				cout << endl << "END OF VERTEX INDICES" << endl;
			}
			if(results[x].compare("texCoordIndex=\"") == 0) {
				cout << "START OF TEXT_COORD INDICES" << endl;
				int i=0;
				x=x+1;
				while(results[x].compare("-1\"") != 0 && results[x].compare("") != 0) { 
					std::istringstream iss(results[x]);
					iss >> i >> std::dec;
					if(i != -1) {
						TexIndices.push_back((int)i);
						cout << i << ", ";
					}
					x++;
				}
				cout << endl << "END OF TEXT_COORD INDICES" << endl;
			}
			if(results[x].compare("<Coordinate") == 0) {
				float f=0.0;
				x+=2;
				cout << "START OF VERTICES" << endl;
				while(results[x].compare("\"") != 0 && results[x].compare("") != 0) { 
					std::istringstream iss(results[x]);
					iss >> f >> std::dec;
					Vertices.push_back(f);
					cout << f << ", ";
					x++;
				}
				cout << endl << "END OF VERTICES" << endl;
			}
			if(results[x].compare("<TextureCoordinate") == 0) {
				float f=0.0;
				x+=2;
				cout << "START OF TEX COORD" << endl;
				while(results[x].compare("\"") != 0 && results[x].compare("") != 0) { 
					std::istringstream iss(results[x]);
					iss >> f >> std::dec;
					TexCoord.push_back(f);
					cout << f << ", ";
					x++;
				}
				cout << endl << "END OF TEX COORD" << endl;
			}
		}
		vector<float> actualVertices(0);
		vector<float> actualTexCoord(0);
		vector<float> actualNormals(0);
		for(int x = 0; x < Indices.size(); x++) {
			actualVertices.push_back(Vertices[Indices[x] * 3 + 0]);
			actualVertices.push_back(Vertices[Indices[x] * 3 + 1]);
			actualVertices.push_back(Vertices[Indices[x] * 3 + 2]);
		}
		for(int x = 0; x < TexIndices.size(); x++) {
			actualTexCoord.push_back(TexCoord[TexIndices[x] * 2 + 0]);
			actualTexCoord.push_back(TexCoord[TexIndices[x] * 2 + 1]);
		}
		for(int x = 0; x < actualVertices.size(); x+=9) {
			//cout << actualVertices[x] << ", ";
			float Ox = actualVertices[x + 0];
			float Oy = actualVertices[x + 1];
			float Oz = actualVertices[x + 2];
			float Ux = actualVertices[x + 3];
			float Uy = actualVertices[x + 4];
			float Uz = actualVertices[x + 5];
			float Vx = actualVertices[x + 6];
			float Vy = actualVertices[x + 7];
			float Vz = actualVertices[x + 8];
			float N[3];
			crossProduct(Ox,Oy,Oz,Ux,Uy,Uz,Vx,Vy,Vz,N);
			cout << "Normal = <" << N[0] << ", " << N[1] << ", " << N[2] << "> " << endl;
			if(!smooth){
				actualNormals.push_back(N[0]);
				actualNormals.push_back(N[1]);
				actualNormals.push_back(N[2]);
				actualNormals.push_back(N[0]);
				actualNormals.push_back(N[1]);
				actualNormals.push_back(N[2]);
				actualNormals.push_back(N[0]);
				actualNormals.push_back(N[1]);
				actualNormals.push_back(N[2]);
			} else {
				Normal.push_back(N[0]);
				Normal.push_back(N[1]);
				Normal.push_back(N[2]);
				Normal.push_back(N[0]);
				Normal.push_back(N[1]);
				Normal.push_back(N[2]);
				Normal.push_back(N[0]);
				Normal.push_back(N[1]);
				Normal.push_back(N[2]);
			}
		}

		if(smooth){
			cout << "Smoothing " << endl;
			for(int x = 0; x < actualVertices.size(); x+=3){
				float nX=0.0;
				float nY=0.0;
				float nZ=0.0;
				for(int y=0;y<actualVertices.size();y+=3){
					if(((actualVertices[x]==actualVertices[y])&&(actualVertices[x+1]==actualVertices[y+1]))&&
						(actualVertices[x+2]==actualVertices[y+2])){
						nX+=Normal[y];
						nY+=Normal[y+1];
						nZ+=Normal[y+2];
					}
				}
				float mag=sqrt(nX*nX+nY*nY+nZ*nZ);
				nX/=mag;
				nY/=mag;
				nZ/=mag;
				actualNormals.push_back(nX);
				actualNormals.push_back(nY);
				actualNormals.push_back(nZ);
			}
		}

		cout << "Done Smoothing " << endl;
		cout << "Writing to file " << endl;
		int y = 0;
		for(int x = 0; x < actualNormals.size(); x+=3) {
			//if(y < actualTexCoord.size() && (y + 1) < actualTexCoord.size()) {
				outputFile<<actualTexCoord[y]<<" "<<actualTexCoord[y+1]<<" ";
			//}
			//if(x < actualNormals.size() && (x + 1) < actualNormals.size()) {
				outputFile<<actualNormals[x]<<" "<<actualNormals[x+1]<<" "<<actualNormals[x+2]<<" ";
				outputFile<<actualVertices[x]<<" "<<actualVertices[x+1]<<" "<<actualVertices[x+2]<<" ";
			//}
			y+=2;
		}
		cout << "Done writing to file " << endl;
		cout << endl;
		if(buffer) {
			free(buffer);
		}
		fclose(x3d_file);
		outputFile.close();
	}
	return(0);
}
