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

uniform vec2 uResolution;
uniform vec4 uInnerColor;
uniform vec4 uBorderColor;
uniform vec4 uBorderSize;
uniform sampler2D uTexture;

in vec2 texOut;

void main()
{
    float ar = uResolution.x / uResolution.y;
    vec2 uv = texOut.xy;
    uv.x *= ar;

    float oneOverX = 1.0f / uResolution.x*ar;
    float oneOverY = 1.0f / uResolution.y;
    float borderTop = uBorderSize.x * oneOverY;
    float borderBot = uBorderSize.y * oneOverY;
    float borderLeft = uBorderSize.z * oneOverX;
    float borderRight = uBorderSize.w * oneOverX;

    vec2 dbl = step(vec2(borderLeft, borderTop), uv.xy);
    float v1 = min(dbl.x, dbl.y);

    vec2 dtr = step(vec2(borderRight, borderBot), vec2(1.0*ar-uv.x, 1.0-uv.y));
    float v2 = min(dtr.x, dtr.y);

    vec4 borderItself = vec4(min(v1, v2));
    
    //TODO: Directly texturing doesnt cut it. We need to realign texture coordinates a bit when boders are present
    vec4 color = mix(uBorderColor, texture(uTexture, texOut) , borderItself);

    gl_FragColor = color;
}
