#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <assert.h>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

int RCmain(int, char *[]);

const float rt_eps = 1e-4;

SceneParser *parser;
Camera *camera;
Group *group;

void dfs(const Ray &r, float lim, int depth, float weight, bool inside, Vector3f &color){
    color = Vector3f::ZERO;
    if(weight < 1e-3)
        return;
    
    Hit h;
    if(!group->intersect(r, h, lim)){
        color = parser->getBackgroundColor();
        return;
    }

    assert(Vector3f::dot(h.getNormal(), r.getDirection()) < 0);

    Vector3f hp = r.pointAtParameter(h.getT());
    bool shadow = true;
    for(int i = 0; i < parser->getNumLights(); i++){
        Light *light = parser->getLight(i);
        Vector3f dir, col;
        light->getIllumination(hp, dir, col);
        if(Vector3f::dot(dir, h.getNormal()) <= 0)
            continue;
        Hit temphit;
        if(!group->intersect(Ray(hp, dir), temphit, rt_eps) || light->closer(temphit.getT())){
            shadow = false;
            color += h.getMaterial()->Shade(r, h, dir, col);
        }
    }
    if(shadow){
        color = parser->getBackgroundColor();
    }
    if(depth > 1){
        Vector3f R, tcol;
        float ratio;
        if(h.getMaterial()->getReflect(r, h, R, ratio)){
            dfs(Ray(hp, R), rt_eps, depth - 1, weight * ratio, inside, tcol);
            color += tcol * ratio;
        }
        if(h.getMaterial()->getRefract(r, h, inside, R, ratio)){
            dfs(Ray(hp, R), rt_eps, depth - 1, weight * ratio, !inside, tcol);
            color += tcol * ratio;
        }
    }
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    // return RCmain(argc, argv);

    if (argc != 3) {
        cout << "Usage: FIN <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    parser = new SceneParser(inputFile.c_str());
    camera = parser->getCamera();
    group = parser->getGroup();

    int width = camera->getWidth(), height = camera->getHeight();
    Image img(width, height);

    for(int x = 0; x < width; x++){
        cout << x / (float) width << endl;
        for(int y = 0; y < height; y++){
            Ray r = camera->generateRay(Vector2f(x, y));
            Vector3f color;
            dfs(r, 0, 5, 1, false, color);
            img.SetPixel(x, y, color);
        }
    }
    img.SaveImage(outputFile.c_str());
    return 0;
}

int RCmain(int argc, char *argv[]){
    if (argc != 3) {
        cout << "Usage: FIN <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    SceneParser parser(inputFile.c_str());
    Camera *camera = parser.getCamera();
    Group *group = parser.getGroup();
    int width = camera->getWidth(), height = camera->getHeight();
    Image img(width, height);
    
    for(int x = 0; x < width; x++){
        if(x != width / 2)
            continue;
        cout << x / (float) width << endl;
        for(int y = 0; y < height; y++){
            Ray r = camera->generateRay(Vector2f(x, y));
            Hit h;
            if(group->intersect(r, h, 0)){
                Vector3f sumcolor = Vector3f::ZERO;
                for(int i = 0; i < parser.getNumLights(); i++){
                    Light *light = parser.getLight(i);
                    Vector3f dir, col;
                    light->getIllumination(r.pointAtParameter(h.getT()), dir, col);
                    sumcolor += h.getMaterial()->Shade(r, h, dir, col);
                }
                img.SetPixel(x, y, sumcolor);
            }else
                img.SetPixel(x, y, parser.getBackgroundColor());
        }
    }
    img.SaveImage(outputFile.c_str());
    return 0;
}