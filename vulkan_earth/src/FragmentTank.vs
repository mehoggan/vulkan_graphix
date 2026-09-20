uniform sampler2D color_texture;  
uniform sampler2D normal_texture;

varying vec3 vertex_normal;
varying vec3 half_vector;  
  
void main(){
 
  vec3 normalPixel = gl_NormalMatrix*normalize(texture2D(normal_texture, gl_TexCoord[0].st).rgb * 2.0 - 1.0);

  float ambient = gl_LightSource[0].ambient.r;
  
  vec3 lightDir = vec3(gl_LightSource[0].position.xyz);
  
  float diffuse_value = max(ambient,dot(vertex_normal, normalize(-lightDir)));


  float spec_value = max(dot(normalize(vertex_normal), half_vector), 0.0);
 
     
 
  vec3 color = /*diffuse_value*/texture2D(color_texture, gl_TexCoord[0].st).rgb
		/*+ gl_LightSource[0].specular.xyz * pow(spec_value, 10000)*/;
  

  gl_FragColor = vec4(color, 1.0);  
}  
