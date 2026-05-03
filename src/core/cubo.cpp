#include "core/cubo.hpp"
#include <vector>


//constructor/destructor
cubo::cubo(int x, int y, int z, int divs, int leng): x(x), y(y), z(z), divs(divs), leng(leng){


    std::vector<Vertex> v;


}//end cubo

cubo::~cubo() {}


void cubo::addFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int n, glm::vec3 origin, glm::vec3 right, glm::vec3 up, glm::vec3 normal, const std::function<float(int row, int col)>& heightAt){

    int baseIndex = vertices.size();

    for (int row = 0; row <= n; row++) {
        for (int col = 0; col <= n; col++) {

            float h = heightAt(row, col);

            Vertex v;
            v.position = origin + (col / float(n)) * right + (row / float(n)) * up + h * normal;
            v.normal = normal;
            v.displacement = h;
            vertices.push_back(v);

        }//end for col

    }//end for row

    //bounds for the hole
    constexpr float holeLo = 0.5f - 0.35f * 0.5f;
    constexpr float holeHi = 0.5f + 0.35f * 0.5f;

    //indicies
    for (int row = 0; row < n; row++) {

        for (int col = 0; col < n; col++) {

            float cx = (col + 0.5f) / float(n);
            float cy = (row + 0.5f) / float(n);
            if (cx > holeLo && cx < holeHi && cy > holeLo && cy < holeHi) continue;

            int BL = baseIndex + row * (n+1) + col;
            int BR = BL + 1;
            int TL = BL + (n+1);
            int TR = TL + 1;

            indices.push_back(BL); indices.push_back(BR); indices.push_back(TL);
            indices.push_back(TL); indices.push_back(BR); indices.push_back(TR);

        }//end for col

    }//end for row

}//end addFace

void cubo::buildCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int n, float minHeight, float maxHeight){

    (void)minHeight; (void)maxHeight;
    auto heightAt = [](int, int) { return 0.0f; };

    addFace(vertices, indices, n, {0,0,1}, {1,0,0}, {0,1,0}, {0,0,1},  heightAt); // front
    addFace(vertices, indices, n, {1,0,0}, {-1,0,0},{0,1,0}, {0,0,-1}, heightAt); // back
    addFace(vertices, indices, n, {0,0,0}, {1,0,0}, {0,0,1}, {0,-1,0}, heightAt); // bottom
    addFace(vertices, indices, n, {0,1,1}, {1,0,0}, {0,0,-1},{0,1,0},  heightAt); // top
    addFace(vertices, indices, n, {0,0,0}, {0,0,1}, {0,1,0}, {-1,0,0}, heightAt); // left
    addFace(vertices, indices, n, {1,0,0}, {0,0,1}, {0,1,0}, {1,0,0},  heightAt); // right

}//end buildCube
