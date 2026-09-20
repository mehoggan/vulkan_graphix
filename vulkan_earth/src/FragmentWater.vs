uniform sampler2D color_texture;  
uniform sampler2D normal_texture;

varying vec3 vertex_normal;  
  
void main(){
  vec3 normalPixel = gl_NormalMatrix * normalize(texture2D(normal_texture, gl_TexCoord[0].st).rgb * 2.0 - 1.0);
  
  vec3 lightDir = normalize(vec3(0,0,1));
  
  float diffuse_value = max(dot(vertex_normal, lightDir), 0.0);
  
  float diffuse_bump = max(dot(normalPixel, lightDir), 0.0);
 
     
 
  vec3 color = /*diffuse_bump * diffuse_value*/texture2D(color_texture, gl_TexCoord[0].st).rgb;  
  gl_FragColor = vec4(color, 0.7);  
}  
