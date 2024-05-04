#version 330 core
in vec2 texCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
    finalColor = texture(texture0, texCoord) * colDiffuse;
}