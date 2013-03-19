##s
uniform ivec2 FrameCount; 
uniform ivec2 Frame; 
uniform mat4 View; 
uniform mat4 Projection; 
uniform mat4 World; 
uniform vec2 Offset; 
uniform float Z; 
uniform vec2 Flip; 
uniform vec2 Size; 
attribute vec2 position; 
attribute vec2 texcoord; 
varying vec2 vtexcoord; 
void main(void) 
{ 
  mat4 wv = View * World; 
  vec2 frameSize = vec2(1.0/float(FrameCount.x),1.0/float(FrameCount.y));
  vec2 ftexcoord = Flip + (vec2(1.0,1.0)-2.0*Flip)*texcoord; 
  vtexcoord = vec2((ftexcoord.x+float(Frame.x)) * frameSize.x,(ftexcoord.y+float(Frame.y)) * frameSize.y); 
  vec4 pos = vec4(Size * position.xy+Offset,0.0,0.0) + wv * vec4(0.0,0.0,0.0,1.0); 
  gl_Position = Projection * pos + vec4(0.0,0.0,Z,0.0); 
} 
##s
uniform sampler2D Texture; 
uniform vec4 ColorKey;
uniform vec4 Color;
varying vec2 vtexcoord; 
void main(void) 
{ 
  gl_FragColor = texture2D(Texture, vtexcoord); 
  if(distance(gl_FragColor, ColorKey) < 0.01)
  {
    gl_FragColor = 0.0000001*gl_FragColor*ColorKey + Color;
  }
} 
##
Global:View;
Global:Projection;
World:World;
Default:FrameCount=1 1;
Default:Flip=0 0;
Default:Size=1.0 1.0;
