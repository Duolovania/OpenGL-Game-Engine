#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float texIndex;

out vec2 v_TexCoord;
out float v_TexIndex;
out vec4 v_Color;

uniform mat4 u_MVP;

void main()
{
   gl_Position = u_MVP * position; // u_MVP * position = ortho. position * u_MVP = perspective.
   v_TexCoord = texCoord;
   v_TexIndex = texIndex;
   v_Color = a_Color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexIndex;
in vec4 v_Color;

uniform vec4 u_Color;
uniform sampler2D u_Textures[100]; // how many textures.

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor = texture(u_Textures[index], v_TexCoord);
    color = texColor * v_Color * u_Color; // output
};