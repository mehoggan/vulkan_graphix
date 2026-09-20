varying vec3 vertex_normal;
varying vec3 half_vector;

void main() {  
  half_vector = normalize(gl_LightSource[0].halfVector.xyz);
  gl_TexCoord[0] = gl_MultiTexCoord0;  
  vertex_normal = normalize(gl_Normal);
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}  