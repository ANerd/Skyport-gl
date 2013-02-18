##s
uniform mat4 World;
uniform mat4 ViewProjection;
attribute vec4 Position;
void main(void)
{
    gl_Position = ViewProjection * (World * Position);
}
##s
uniform vec4 Color;
void main(void)
{
    gl_FragColor = Color;
}
##
Global:ViewProjection;
World:World;
