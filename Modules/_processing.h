#ifndef _PROCESSING_H
#define _PROCESSING_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QWidget>
#include <Compute/_gpu_compute.h>
//22_04_2019
class _Processing : public QObject , protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit _Processing(QObject *parent = nullptr);

signals:
    void outputImage(char *img, unsigned int iwidth, unsigned int iheight);
    void inputImageRecived(char *img, unsigned int iwidth, unsigned int iheight);

public slots:
    void inputImage(char *img, unsigned int iwidth, unsigned int iheight);
    void passThroughFrame(char *img, unsigned int iwidth, unsigned int iheight);
    void init();
    void setActiveProcess(const char *slot);

protected:

    QOpenGLContext *context = nullptr;
    QOffscreenSurface *surface = nullptr;
    _GPU_Compute *gpu_compute = nullptr;
    char * colorFrame = nullptr;

    unsigned int framebuffer;
    unsigned int renderbuffer;
    unsigned int framebuffer_texture;
    unsigned int framebuffer_Width=1360,framebuffer_Height=1024;
    const char* active_slot = nullptr;
};

#endif // _PROCESSING_H