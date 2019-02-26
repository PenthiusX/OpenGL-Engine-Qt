#ifndef _RENDERER_H
#define _RENDERER_H
#include <vector>
#include "_shader.h"
#include "_sceneentity.h"
#include <qopenglextrafunctions.h>
#include <qmatrix4x4.h>
#include <qelapsedtimer.h>//for timer
/*
 * The Renderer class
 * To create an abstraction for randering data
 * Extends the _glwidget class, will be included in the _glwidget class
 * Created: 8_02_2019
 * Author: Aditya,Saurabh
*/
class _Renderer : protected QOpenGLExtraFunctions
{
public:
    _Renderer();
    ~_Renderer();

    void setShader();//default shatder to load
    void setShader(QString vertexShader, QString fragmentShader);//takes a string literal and passes
    void setBuffers(std::vector<float>vertexArray,std::vector<unsigned int> indexArray);//take vertex and index data and binds it to object buffer
    void setTexture(char* texBitmap);//takes am image and binds it to object
	void setMatrices(int w,int h);
	void setModelMatrix(QVector3D position, int scale, QQuaternion rotation);
	void setCamViewMatrix(QVector3D eyePos, QVector3D focalPoint, QVector3D upVector);
	void setProjectionMatrix(int resW, int resH, float fov, float zFar, float zNear);
	void addToScene(_SceneEntity sceneObject);//adds a sceneEntityType object to scene
    void draw();//Draws everything bound in the scene

private:
unsigned int VBO;//vertex buffer object
unsigned int VAO;//attribute buffer object 
unsigned int EBO;//index buffer object

//Shader class object sets the shaders and passes
//the program to the current context
_Shader* shdr;

//Matrices for Translation and view
//will be multiplied with the position to set translation
//rotaion ,scaling witrespect to view.
QMatrix4x4 model4x4;
QMatrix4x4 projection4x4;
QMatrix4x4 view4x4;
QMatrix4x4 mvp;

//Stores the uniform location allocated in the shader
int colorUniform, mvpUniform;
//Holds the vertex and index data
std::vector<float> vertices;
std::vector<unsigned int> indices;
QElapsedTimer timer;
};

#endif // _RENDERER_H