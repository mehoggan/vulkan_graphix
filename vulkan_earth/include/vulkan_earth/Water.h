#ifndef WATER_H_
#define WATER_H_

#include <stdio.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "vulkan_earth/Normal.h"
#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/render/Mesh.h"
#include "vulkan_earth/render/Texture.h"

namespace vulkan_earth::render {
class RenderContext;
}

class Water {
public:
    Water();
    Water(std::int32_t new_scale, std::int32_t new_size);
    ~Water();
    void draw(vulkan_earth::render::RenderContext& context);
    void initData();
    void prepareData(std::int32_t steps,
                     std::int32_t increase,
                     float radius,
                     std::int32_t random_jump);
    void calcAverageofSixNormals(Vertex* v_0,
                                 float x1,
                                 float y1,
                                 float z1,
                                 float x2,
                                 float y2,
                                 float z2,
                                 float x3,
                                 float y3,
                                 float z3,
                                 float x4,
                                 float y4,
                                 float z4,
                                 float x5,
                                 float y5,
                                 float z5,
                                 float x6,
                                 float y6,
                                 float z6,
                                 Normal* n);
    void prepTerrain();
    void prepareData();
    void terrainGen(std::int32_t steps,
                    std::int32_t increase,
                    float radius,
                    std::int32_t random_jump);
    std::int32_t getActualSize();
    std::int32_t getScale();
    void stdMessageBox(const std::string& output);
    void errorMessageBox(const std::string& output);

private:
    std::int32_t scale;
    std::int32_t size;
    std::int32_t total_vertices;
    std::int32_t tri_strip_buffer_size;
    std::int32_t** surfaceheight;
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> tex_coord;
    std::shared_ptr<vulkan_earth::render::Texture> color_texture;
    std::unique_ptr<vulkan_earth::render::StaticMesh> mesh;
    float timer;
};

#endif /*	WATER_H_	*/
