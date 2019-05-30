#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include "_scene.h"
#include "_physics.h"

/*
 * Class: _GLWidget
 * This class is the Controller is a typical MVC where the
 * View and Controller are being handeled by the QT GUI frameowrk and
 * its wrapper functions for creating Opengl and GUI windowing context.
 * Created: 5_02_2019
 * Author: Aditya,Saurabh
*/

class _GLWidget :  public QOpenGLWidget
{
public:
    _GLWidget();
    explicit _GLWidget(QWidget *parent = 0);
    ~_GLWidget();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h)Q_DECL_OVERRIDE;
    void paintGL()Q_DECL_OVERRIDE;
    //
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void wheelEvent(QWheelEvent *e) override;
    virtual void  keyPressEvent(QKeyEvent *event) override;

private:
    void addRandomSceneEntitestoScene();
    void applyStuffToallEntites(bool isit);

    //Variables
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int shaderProgram;
    QOpenGLContext *context;

    std::vector<_Scene*> sceneVector;//Vector of scene for drawing multiple scene Contexts//!!prototype implmentaition not implemented and checked!!!
    _Scene *scene;//Scene include renderer , camera and sceneentity classes, so no need to reinclude header
    _SceneEntity s, s1,s2,mpoint,*onPress;//custom Objects
    _SceneEntity background_quad,pivot;//default sceneobjects
    _AssetLoader assetLoader;
    _Camera cam;

    //Varible for InputCallbackFunctions
    unsigned int idmatch;
    double scroolScale;
    QVector2D rotRads;
	bool isCamFocus;
    QVector2D mousePressPositionL,mousePressPositionR;
    QVector2D mousePositionL,mousePositionR;
};

#endif // _GLWIDGET_H
