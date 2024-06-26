/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;

uniform mat4 uModelMatrix;
uniform mat4 uProjMatrix;

out vec2 texOut;

void main()
{
    texOut = vTex;
    gl_Position = uProjMatrix * uModelMatrix * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform sampler2D uTexture;
in vec2 texOut;

void main()
{
   gl_FragColor = texture(uTexture, texOut);
   // gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
