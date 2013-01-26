##s
uniform mat4 ViewProjection; 
uniform mat4 World; 
attribute vec4 Position;
void main(void) 
{ 
  gl_Position = ViewProjection * (World * Position); 
} 
##s
uniform sampler2D Texture; 
void main(void) 
{ 
  gl_FragColor = vec4(1.0,0.0,0.0,1.0);
} 
##
Global:ViewProjection;
World:World;
