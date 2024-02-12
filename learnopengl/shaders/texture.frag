#version 330 core
in vec3 fColor;
in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform float mixValue;

out vec4 FragColor;

void main()
{
   FragColor = mix(texture(tex1, TexCoord), texture(tex2, TexCoord), mixValue);
}