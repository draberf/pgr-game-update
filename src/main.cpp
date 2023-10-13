// dependencies
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// standard C++ libraries
#include <iostream>
#include <vector>
#include <filesystem>

// project-specific libraries
#include "geometry.h"
#include "glmodel.h"
#include "scenegraph.h"
#include "glutils.h"
#include "mediator.h"

// shaders
#include "shaders.h"


// timer class
#include "Timer.hpp"

// require S keypress to start?
#define REQUIRE_S false

// levels:

struct TerrainBlock {
    float w, h, d;
    float x, y;
    bool isHazard;
};

static std::vector<TerrainBlock> levelOne = {

    // starting area
    {1.f, 5.9f, 2.f, -2.f, -0.9, false},        //0
    {9.9f, 1.f, 2.f, -2.f, -2.f, false},        //1

    // bridge
    {4.f, 4.f, 2.f, 8.f, -2.f, false},          //2
    {3.8f, 1.f, 1.f, 12.1, 1.f, false},         //3
    {4.f, 4.f, 2.f, 16.f, -2.f, false},         //4

    // leaps
    {4.f, 6.f, 4.f, 24.f, -2.f, false},         //5
    {0.8f, 1.f, 0.5f, 32.75f, 3.f, false},      //6
    {0.1f, 4.9f, 0.1f, 32.95f, -1.8f, false},    //7 <- STEM
    {4.f, 6.f, 4.f, 38.f, -2.f, false},         //8

    // finish
    {4.f, 1.f, 4.f, 46.f, 5.f, false},          //9
    {0.5f, 0.5f, 0.5f, 47.75f, 7.25f, false},   //10 <- FINISHBLOCK

    // lava
    {60.f, 10.f, 60.f, -8.f, -14.f, true}       //11
};
# define FINISH_I 10

static ControlComponent* cc = nullptr;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    return;
    if (action == GLFW_PRESS) {
        cc->KeyPress(key);
        return;
    }
    if (action == GLFW_REPEAT) {
        cc->KeyPress(key);
    }
}

// logical structure of a GLFW app:
// https://www.glfw.org/documentation.html
int main(void)
{

#ifdef _DEBUG
    {
        // GLM
        glm::mat4 debug_mat = glm::mat4(1.f);

        // geometry.cpp
        VertexNormal* ptr = nullptr;
        Block* debug_block = new Block(5.f, 5.f, 5.f);
        debug_block->PrintSize();
        delete debug_block;

        // assimp

        Assimp::Importer imp;

    }
#endif


    GLFWwindow* window;
    // initialize GLFW
    if (!glfwInit())
        return -1;
    
    // create window (and context)
    // window properties defined in glutils.h
    window = glfwCreateWindow(WIDTH, HEIGHT, "pgr-game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // select context
    glfwMakeContextCurrent(window);
    // synchronize refresh rates
    glfwSwapInterval(1);


    // attempt to initialize GLEW
    auto err = glewInit();

    if (err != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        return -1;
    }



#ifdef _DEBUG
    std::cout << glGetString(GL_VERSION) << std::endl;
#endif



    // create vertex buffer objects for base program
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE , 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // create program with shaders
    GLuint program = createProgram({
        createShader(GL_VERTEX_SHADER, basicVS),
        createShader(GL_FRAGMENT_SHADER, basicFS),
        });

    // create teeth program
    GLuint teethprg = createProgram({
        createShader(GL_VERTEX_SHADER, teethVS),
        createShader(GL_FRAGMENT_SHADER, textureFS),
        });

    GLuint colorTex;
    glCreateTextures(GL_TEXTURE_2D, 1, &colorTex);
    glTextureStorage2D(colorTex, 1, GL_RGBA8, WIDTH, HEIGHT);
    glTextureParameteri(colorTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(colorTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint depthTex;
    glCreateTextures(GL_TEXTURE_2D, 1, &depthTex);
    glTextureStorage2D(depthTex, 1, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);

    GLuint fbo;
    glCreateFramebuffers(1, &fbo);
    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, colorTex, 0);
    glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, depthTex, 0);
    glNamedFramebufferTexture(fbo, GL_STENCIL_ATTACHMENT, depthTex, 0);


    GLuint uniformProg = glGetUniformLocation(teethprg, "u_prog");

    // create SCENE GRAPH
    SceneComposite* graph = new SceneComposite();

    // add skybox
    GLuint skybox_program = createProgram(
        {
            createShader(GL_VERTEX_SHADER, skyboxVS),
            createShader(GL_FRAGMENT_SHADER, skyboxFS)
        }
    );

    SceneLeaf* sb = new SceneLeaf(new Skybox("textures/skybox_mini.png", 0.f, skybox_program));

    graph->Add(sb);

    GLuint viewUniform  = glGetUniformLocation(program, "u_view");
    GLuint projUniform  = glGetUniformLocation(program, "u_proj");
    GLuint celsUniform  = glGetUniformLocation(program, "u_cels");

    // add character to scene

    // CHARACTER HEIGHT: 2.2f

    Character * character = new Character(
        ".\\models\\hunter\\Hunter-Character-Model.fbx",
        0.f,
        1.f,
        0.3f,
        program
    );

    SceneLeaf* characterLeaf = new SceneLeaf(character);
    graph->Add(characterLeaf);

    // create vector of terrains
    std::vector<Terrain*> t_vec;

    Terrain* finishBlock = nullptr;

    // load terrain from variable:
    for (unsigned int i = -0; i < levelOne.size(); i++) {
        Terrain* t = new Terrain(
            levelOne[i].w,
            levelOne[i].h,
            levelOne[i].d,
            levelOne[i].x,
            levelOne[i].y,
            0.f,
            program
        );
        if (levelOne[i].isHazard) {
            t->UpdateColor(0.9f, 0.1f, 0.1f);
            t->SetHazard(true);
        }
        if (i == FINISH_I) {
            t->UpdateColor(0.8f, 0.8f, 0.3f);
            finishBlock = t;
        }
        t_vec.push_back(t);
        graph->Add(new SceneLeaf(t));
    }


    // prepare unifroms
    glProgramUniformMatrix4fv(program, projUniform, 1, GL_FALSE, glm::value_ptr(ComputeProjMatrix()));
    glProgramUniform1i(program, celsUniform, 0);

    // setup mediators

    cc = new ControlComponent();
    CharacterComponent* characterComponent = new CharacterComponent(character);
    ShaderComponent* shaderComponent = new ShaderComponent();

    ControlMediator* control = new ControlMediator(cc, characterComponent, shaderComponent);

    glfwSetKeyCallback(window, KeyCallback);


    // finally, initialize Timer
    // returns amount of SECONDS passed
    Timer<double> t;
    t.reset();
    float dt = 0.f;

    bool start = (!REQUIRE_S);


    unsigned int chomp_frames = 0;
    float prog = 0.f;

    // EVENT LOOP
    while (!glfwWindowShouldClose(window))
    {
        dt = t.elapsedFromLast();

        // 1) EVENT TRAVERSAL
        // -- I/O handling
        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            start = true;

        if (!start) continue;

        // can probably be a loop
        int stateR  = glfwGetKey(window, GLFW_KEY_RIGHT);
        int stateL  = glfwGetKey(window, GLFW_KEY_LEFT);
        int stateSp = glfwGetKey(window, GLFW_KEY_SPACE);
        if (stateR == GLFW_PRESS) {
            cc->KeyPress(GLFW_KEY_RIGHT);
        }
        if (stateL == GLFW_PRESS) {
            cc->KeyPress(GLFW_KEY_LEFT);
        } 
        if (stateSp == GLFW_PRESS) {
            cc->KeyPress(GLFW_KEY_SPACE);
        }



        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }


        // 2) UPDATE TRAVERSAL
        // -- updating the graph, simulation, animation

        if (!character->GetDeathStatus()) {

            // check for collision with finish block:
            if (character->CheckFinish(finishBlock)) {
                std::cout << "Congratulations! You have reached the finish!" << std::endl;
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            character->CalculateCollisions(t_vec);
            graph->Update(dt);
            // check for character death
            if (character->GetDeathStatus()) {
                std::cout << "You died by touching a hazard! Game over!" << std::endl;
            }
        }



        glm::vec3 camTarget = glm::vec3(0.f, 0.f, 0.f);
        camTarget.x = character->GetCameraTargetXY().x;
        camTarget.y = character->GetCameraTargetXY().y;

        glm::vec3 camPos = glm::vec3(0.f, 0.f, 3.f);
        camPos.x = character->GetCharacTargetXY().x;
        camPos.y = character->GetCharacTargetXY().y;


        auto viewM = glm::lookAt(camPos, camTarget, glm::vec3(0.f, 1.f, 0.f));

        glProgramUniformMatrix4fv(program, viewUniform, 1, GL_FALSE, glm::value_ptr(viewM));



        // 3) DRAW PREPARATION
        // -- preparing the scene for rendering,
        //    translating graph to a more effective structure
        // -- (probably not necessary here)



        // 4) DRAW TRAVERSAL
        // -- rendering the scene

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glClearColor(0.7, 0.7, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0x00);

        graph->Draw(DRAW_TYPE::SKYBOX);

        //  -- object render

        glBindVertexArray(vbo);
        glUseProgram(program);

        glProgramUniform1i(program, celsUniform, 0);

        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        
        graph->Draw(DRAW_TYPE::GEOMETRY);



        glBindVertexArray(0);

        //  -- outline render


        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        
        glProgramUniform1i(program, celsUniform, 1);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        graph->Draw(DRAW_TYPE::GEOMETRY);
        glDisable(GL_CULL_FACE);

        // reset stencil buffer
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // render filled framebuffer

        glBindTextureUnit(0, colorTex);
        glClearColor(0.f, 0.f, 0.f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (character->GetDeathStatus()) {
            chomp_frames += 1;
            prog = pow((float)chomp_frames/30.f, 2.4f);
        }

        glUseProgram(teethprg);
        glUniform1f(uniformProg, prog);

        glDrawArrays(GL_TRIANGLES, 0, 48);


        // swap buffers to draw
        glfwSwapBuffers(window);

        if (chomp_frames > 35) {
            glfwSetWindowShouldClose(window, true);
        }

    }

    glfwTerminate();
    return 0;
}