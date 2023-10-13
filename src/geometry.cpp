#include "geometry.h"

/**
*  @brief Structure for a single triangle vertex normals and a combined face normal.
*
*/
struct FaceTriangle {
    VertexNormal    vn[3];
    float           faceNormal[3];
};



/**
 * @brief Common superclass for geometry objects.
 *
 */

VertexNormal* Geometry::GetGeometry() {
    std::cerr << "ERROR: Function GetGeometry() unspecified for this class." << std::endl;
    return nullptr;
}

/**
*  @brief Return size of Geometry array in bytes.
*
*
*/
unsigned int Geometry::GeometrySize() {
    std::cerr << "ERROR: Function GeometrySize() unspecified for this class." << std::endl;
    return 0;
}


/**
 * @brief Geometry class for six-faced blocks.
 */
Block::Block() {
    this->w = 5.0f;
    this->h = 5.0f;
    this->d = 5.0f;
    mesh = (VertexNormal*)malloc(36 * sizeof(VertexNormal));
    UpdateGeometry();
}

Block::Block(float w, float h, float d) {
    this->w = w;
    this->h = h;
    this->d = d;
    mesh = (VertexNormal*)malloc(36 * sizeof(VertexNormal));
    UpdateGeometry();
}

Block::~Block() {
    free(mesh);
}

void Block::UpdateGeometry() {
    // origin position: Front bottom right
    // width and height into positive
    // depth into negative

    /*

          H +--------+ G
           /        /|
        E /        / |
         +--------+ F|
         |  (D)   |  + C
         |        | /
         |        |/
         +--------+
        A          B

    */

    VertexNormal v[36] = {
        // front face
        {{ 0.f    ,  0.f    , 0.f    }, {-1.f, -1.f,  1.f}},
        {{ this->w,  0.f    , 0.f    }, { 1.f, -1.f,  1.f}},
        {{ this->w,  this->h, 0.f    }, { 1.f,  1.f,  1.f}},
        {{ this->w,  this->h, 0.f    }, { 1.f,  1.f,  1.f}},
        {{ 0.f    ,  this->h, 0.f    }, {-1.f,  1.f,  1.f}},
        {{ 0.f    ,  0.f    , 0.f    }, {-1.f, -1.f,  1.f}},
        // right face
        {{ this->w,  0.f    , 0.f    }, { 1.f, -1.f,  1.f}},
        {{ this->w,  0.f    ,-this->d}, { 1.f, -1.f, -1.f}},
        {{ this->w,  this->h,-this->d}, { 1.f,  1.f, -1.f}},
        {{ this->w,  this->h,-this->d}, { 1.f,  1.f, -1.f}},
        {{ this->w,  this->h, 0.f    }, { 1.f,  1.f,  1.f}},
        {{ this->w,  0.f    , 0.f    }, { 1.f, -1.f,  1.f}},
        // back face
        {{ this->w,  0.f    ,-this->d}, { 1.f, -1.f, -1.f}},
        {{ 0.f    ,  0.f    ,-this->d}, {-1.f, -1.f, -1.f}},
        {{ 0.f    ,  this->h,-this->d}, {-1.f,  1.f, -1.f}},
        {{ 0.f    ,  this->h,-this->d}, {-1.f,  1.f, -1.f}},
        {{ this->w,  this->h,-this->d}, { 1.f,  1.f, -1.f}},
        {{ this->w,  0.f    ,-this->d}, { 1.f, -1.f, -1.f}},
        // left face
        {{ 0.f    ,  0.f    ,-this->d}, {-1.f, -1.f, -1.f}},
        {{ 0.f    ,  0.f    , 0.f    }, {-1.f, -1.f,  1.f}},
        {{ 0.f    ,  this->h, 0.f    }, {-1.f,  1.f,  1.f}},
        {{ 0.f    ,  this->h, 0.f    }, {-1.f,  1.f,  1.f}},
        {{ 0.f    ,  this->h,-this->d}, {-1.f,  1.f, -1.f}},
        {{ 0.f    ,  0.f    ,-this->d}, {-1.f, -1.f, -1.f}},
        // top face
        {{ 0.f    ,  this->h, 0.f    }, {-1.f,  1.f,  1.f}},
        {{ this->w,  this->h, 0.f    }, { 1.f,  1.f,  1.f}},
        {{ this->w,  this->h,-this->d}, { 1.f,  1.f, -1.f}},
        {{ this->w,  this->h,-this->d}, { 1.f,  1.f, -1.f}},
        {{ 0.f    ,  this->h,-this->d}, {-1.f,  1.f, -1.f}},
        {{ 0.f    ,  this->h, 0.f    }, {-1.f,  1.f,  1.f}},
        // bot face
        {{ 0.f    ,  0.f    ,-this->d}, {-1.f, -1.f, -1.f}},
        {{ this->w,  0.f    ,-this->d}, { 1.f, -1.f, -1.f}},
        {{ this->w,  0.f    , 0.f    }, { 1.f, -1.f,  1.f}},
        {{ this->w,  0.f    , 0.f    }, { 1.f, -1.f,  1.f}},
        {{ 0.f    ,  0.f    , 0.f    }, {-1.f, -1.f,  1.f}},
        {{ 0.f    ,  0.f    ,-this->d}, {-1.f, -1.f, -1.f}}
    };

    memcpy(mesh, v, 36 * sizeof(VertexNormal));
}

VertexNormal* Block::GetGeometry() {
    return this->mesh;
}

unsigned int Block::GeometrySize() {
    return 36 * 6 * sizeof(float);
}

void Block::PrintSize() {
    std::cout << "w: " << this->w << std::endl;
    std::cout << "h: " << this->h << std::endl;
    std::cout << "d: " << this->d << std::endl;
}