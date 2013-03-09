##s
uniform mat4 View; 
uniform float Size;
uniform float Z;
attribute vec4 Position;
attribute vec2 Texcoord;
varying vec2 vTexcoord;
void main(void) 
{ 
  vTexcoord = Texcoord;
  gl_Position = vec4((View * (Position*Size)).xy,Z,1.0);
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
Global:View;
Default:Size=5;
Default:Z=0.999;
