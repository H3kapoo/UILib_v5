/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;

uniform mat4 uModelMatrix[100];
uniform mat4 uProjMatrix;

out vec2 textureCoordsOut;
flat out int index;

void main()
{
    index = gl_InstanceID;
    textureCoordsOut = vTex;
    gl_Position = uProjMatrix * uModelMatrix[index] * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform vec3 res;
uniform vec4 uColor[100];
uniform float uTime;

in vec2 textureCoordsOut;
flat in int index;

void main()
{
   gl_FragColor = uColor[index];
   // gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
