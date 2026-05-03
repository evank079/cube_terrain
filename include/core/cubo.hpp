#pragma once

#include <functional>
#include <vector>
#include <glm/glm.hpp>

//Generate a cube with a given position, size and number of subdivisions on surface


class cubo{
public:

    //For storing vertex and position together, normals should be simple for cube
    //but still needs to be included for the heights
    struct Vertex {

        glm::vec3 position;
        glm::vec3 normal;
        float     displacement;   // signed distance pushed along normal
        // glm::vec2 uv;

    };

    cubo(int x, int y, int z, int divs, int leng);
    ~cubo();

    void addFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int n, glm::vec3 origin, glm::vec3 right, glm::vec3 up, glm::vec3 normal, const std::function<float(int row, int col)>& heightAt);

    void buildCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int n, float minHeight, float maxHeight);

private:

    int x;
    int y;
    int z;
    int divs;
    int leng;

};//end cubo
