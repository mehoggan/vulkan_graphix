uniform float timer;
varying vec3 vertex_normal;

void main() {  
  gl_TexCoord[0] = gl_MultiTexCoord0;  
  vertex_normal = normalize(gl_Normal);

  vec4 newVertex=vec4(gl_Vertex);
  newVertex.y+=sin(newVertex.x/5000.0+timer)*300.0;
  newVertex.y+=sin(newVertex.z/3000.0+timer)*300.0;
  newVertex.y+=sin(newVertex.x/632.0+timer)*30.0;
  newVertex.y+=sin(newVertex.z/341.0+timer)*30.0;

  gl_Position = gl_ModelViewProjectionMatrix * newVertex;  
}  