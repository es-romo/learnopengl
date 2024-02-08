#version 330 core
in vec3 fColor;
in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;


out vec4 FragColor;

void main()
{
   FragColor = texture(tex1, TexCoord) * vec4(fColor.xyz, 0.5);
}