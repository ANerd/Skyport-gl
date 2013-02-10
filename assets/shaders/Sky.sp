##s
uniform mat4 ViewProjection; 
uniform mat4 World; 
attribute vec4 Position;
attribute vec2 Texcoord;
varying vec2 vTexcoord;
void main(void) 
{ 
  vTexcoord = Texcoord;
  gl_Position = ViewProjection * (World * Position); 
} 
##s
uniform sampler2D Texture; 
varying vec2 vTexcoord;
void main(void) 
{ 
  gl_FragColor = texture2D(Texture, vTexcoord); 
  //gl_FragColor = gl_FragColor * 0.0000001 + vec4(1.0,0.0,0.0,1.0);
} 
##
Global:ViewProjection;
World:World;
