#include <GL/glew.h>

#include "scenegraph.h"
#include "glutils.h"
#include "shaders.h"

#include <vector>

void LevelOne(SceneComposite* graph) {

	GLuint skybox_program = createProgram(
		{
			createShader(GL_VERTEX_SHADER, skyboxVS),
			createShader(GL_FRAGMENT_SHADER, skyboxFS)
		}
	);

	graph->Add(new SceneLeaf(new Skybox("textures/skybox_mini.png", 0.f, skybox_program)));

}

struct TerrainBlock {
	float w, h, d;
	float x, y;
	bool isHazard;
};


