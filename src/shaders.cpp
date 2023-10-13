/**
*  @file Shaders used in the applications, stored as strings.
* 
* 
* 
*/ 

#pragma once

// VERTEX SHADERS

const char* basicVS = R".(
    #version 460

    layout(location=0)in vec3 position;
    layout(location=1)in vec3 normal;

    uniform mat4 u_model = mat4(1.f);
    uniform mat4 u_view  = mat4(1.f);
    uniform mat4 u_proj  = mat4(1.f);
    uniform int  u_cels  = 0;
    uniform uint u_glow  = 0;

    uniform vec3 a_color = vec3(0.6f, 0.f, 0.f);

    out vec3 vColor;
    flat out uint glow;

    void main(){

        vec4 pos = u_model*vec4(position,1);
        vec4 tmp_norm = u_model*vec4(normal,0);
        vec3 norm = normalize(vec3(tmp_norm.xyz));
        
        if (u_cels == 1) {
            vec3 tmp = vec3(pos.xyz);
            tmp = tmp + norm*0.04f;
            pos = vec4(tmp,1);
        }

        pos = u_proj*u_view*pos;        

        if (u_cels == 1) {
            vColor = vec3(0,0,0);
        } else {
            vColor = a_color;
        }

        // set glow
        glow = u_glow;

        gl_Position = pos;
    }
).";

const char* skyboxVS = R".(

    #version 460

    uniform float offset = 0;
    uniform float xZoom = 2;
    uniform float yZoom = 1;

    out vec2 texCoords;


    void main() {
    
        int quad[] = int[](
            -1, -1,
             1, -1,
             1,  1,

             1,  1,
            -1,  1,
            -1, -1
        );

        if(gl_VertexID>=6){
            gl_Position = vec4(0.f,0.f,0.f,1.f);
            return;
        }

        vec2 pos;
        pos[0] = float(quad[gl_VertexID*2]);
        pos[1] = float(quad[gl_VertexID*2 + 1]);

        gl_Position = vec4(pos,0,1);

        float centerX = offset;
        float centerY = 0.2f;

        vec2 texOffset = (pos + 1) / 2;

        // twice as wide x for wider view
        texCoords.x = centerX + texOffset.x * 0.5f/xZoom;
        texCoords.y = centerY + texOffset.y * 0.5f/yZoom;

    }


).";

const char* quadVS = R".(

    #version 460

    out vec2 texCoords;

    void main()
    {
        float positions[] = float[](
            -1.f, -1.f,
             1.f, -1.f,
             1.f,  1.f,
             1.f,  1.f,
            -1.f,  1.f,
            -1.f, -1.f
        );

        if(gl_VertexID >= 6) {
            gl_Position = vec4(0,0,0,1);
            texCoords = vec2(0,0);
            return;
        }

        texCoords = vec2(positions[2*gl_VertexID],positions[2*gl_VertexID+1]);
        gl_Position = vec4(texCoords,0,1);
    }

).";

const char* teethVS = R".(
    #version 460

    uniform float u_prog;

    out vec2 texCoords;

    void main() {

        if (gl_VertexID >= 48) {
            gl_Position = vec4(0,0,0,1);
            texCoords = vec2(0,0);
            return;
        }
    
        float prog = clamp(u_prog, 0.f, 1.f);

        uint rect = uint(floor(gl_VertexID / 12));
        float start_x = -1.0f + float(rect)*0.5f;
        float start_tex_x = 0.f + float(rect)*0.25f;
        
        uint index = uint(mod(gl_VertexID, 12));
        vec2 pos = vec2(0.f,0.f);

        float x_pos[] = float[](
            0, 0.25, 0.25,
            0.25, 0, 0,
            0.25, 0.5, 0.25,
            0.25, 0.5, 0.5
        );

        float y_pos[] = float[](
            -1, -1+2.f*prog, 1,
            1, 1-2.f*prog, -1,
            -1+2.f*prog, -1, 1,
            1, -1, 1-2.f*prog
        );

        float tex_x[] = float[](
            0.f, 0.125f, 0.125f,
            0.125f, 0.f, 0.f,
            0.125f, 0.25f, 0.125f,
            0.125f, 0.25f, 0.25f
        );

        float tex_y[] = float[](
            0, 0, 1,
            1, 1, 0,
            0, 0, 1,
            1, 0, 1
        );

        pos = vec2(start_x+x_pos[index], y_pos[index]);
            
        texCoords = vec2(start_tex_x+tex_x[index], tex_y[index]);
        gl_Position = vec4(pos,0,1);
    }
).";


// FRAGMENT SHADERS


const char* basicFS = R".(
    #version 460
    in vec3 vColor;
    flat in uint glow;
    layout(location=0)out vec4 fColor;
    layout(location=1)out vec4 bright;
    void main(){
        fColor = vec4(vColor,1);
        bright = vec4(0,0,0,1);
        if (glow == 1) { 
            bright = vec4(vColor,1);
        }
    }
).";

// source: https://learnopengl.com/Advanced-Lighting/Bloom
const char* gaussFS = R".(

    #version 460

    out vec4 fColor;

    in vec2 texCoords;

    uniform sampler2D image;

    uniform uint horizontal;
    uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    void main()
    {
        vec2 tex_offset = 1.0 / textureSize(image, 0);
        vec3 result = texture(image, texCoords).rgb * weight[0];
        if (horizontal == 1) {
            for (int i = 1; i < 5; i++) {
                result += texture(image, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
                result += texture(image, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            }
        } else {
            for (int i = 1; i < 5; i++) {
                result += texture(image, texCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
                result += texture(image, texCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            }
        }
        fColor = vec4(result, 1.0);
    }

).";

const char* textureFS = R".(
    #version 460

    out vec4 fColor;

    in vec2 texCoords;

    uniform sampler2D scene;

    void main()
    {
        fColor = vec4(texture(scene, texCoords).rgb, 1.0);
    }
).";

// source: https://learnopengl.com/Advanced-Lighting/Bloom
const char* blendFS = R".(

    #version 460

    out vec4 fColor;

    in vec2 texCoords;

    uniform sampler2D scene;
    uniform sampler2D blur;
    uniform float exposure = 1.0;

    void main()
    {
        const float gamma = 2.2;

        vec3 color = texture(scene, texCoords).rgb;
        vec3 bloom = texture(scene, texCoords).rgb;

        color += bloom;

        vec3 result = vec3(1.0) - exp(-color * exposure);

        result = pow(result, vec3(1.0/gamma));
        fColor = vec4(result, 1.0);
    }

).";

const char* skyboxFS = R".(
    #version 460

    in vec2 texCoords;

    uniform sampler2D sb_tex;

    layout(location=0)out vec4 fColor;
    void main(){
        fColor = texture(sb_tex, texCoords);
    }


).";