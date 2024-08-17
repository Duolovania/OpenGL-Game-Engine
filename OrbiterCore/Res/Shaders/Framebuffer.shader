#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoords;

out vec2 v_TexCoord;

void main()
{
   gl_Position = vec4(position.x, position.y, 0.0, 1.0);
   v_TexCoord = texCoords;
};

#shader fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoord;

uniform sampler2D screenTexture;

void main()
{
    vec3 frag = texture(screenTexture, v_TexCoord).rgb;
    float grayscale = dot(frag, vec3(0.299, 0.587, 0.114));
    color = vec4(vec3(grayscale), 1.0);
    // color = vec4(0, 1, 0, 1);
};