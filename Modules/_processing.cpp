#include "_processing.h"
#include <QDebug>
#include <QThread>
#include <QMetaMethod>
#include <array>
#include <Compute/_cpu_compute.h>

/* Processing class
 * used for image prcessing operations
 * this contains image input and output slots
 *
 *      inputImage : slot for 8-bit grayscale image input
 *
 *      image processing done by class methods which are activated by using setActiveProcess function
 *
 *      outputImage : signal for 32-bit rgba image out
 * Author : Saurabh
 * created:22_04_2019
*/

/* _Processing constructor
 * creates offscreen render surface and context
 * instance should be created in parent gui thread
*/
_Processing::_Processing(QObject *parent) : QObject(parent) , QOpenGLExtraFunctions()
{
    QSurfaceFormat format;

    //create the context
    if (!context) {
        context = new QOpenGLContext(this);
        context->setFormat(format);
    }

    if(!context->isValid())
    {
        qDebug() << "GL context not Valid";
    }

    surface = new QOffscreenSurface();
    surface->setFormat(context->format());

    //if glWidget's context need to be shared with scanner context, make glwidget's context current first and uncomment following line
    //context->setShareContext(QOpenGLContext::currentContext());

    surface->create();

    qDebug() << "offscreen render init" ;
}

/* Function : setActiveProcess(const char* slot)
 * set the active process to be done on image
 * eg:
 *    //for setting in same thread
 *    setActiveProcess(SLOT(passThroughFrame(char* ,unsigned int,unsigned int)));
 *
 *    //for setting no process in same thread
 *    setActiveProcess(nullptr);
 *
 *    //for setting in differant thread
 *    QMetaObject::invokeMethod(processing, "setActiveProcess", Qt::QueuedConnection,Q_ARG(const char*,SLOT(passThroughFrame(char* ,unsigned int,unsigned int)) ));
 *
 *    //for setting no process in differant thread
 *    QMetaObject::invokeMethod(processing, "setActiveProcess", Qt::QueuedConnection,Q_ARG(const char*,nullptr));
 *
 *   //this has to be called once before above two calls
 *   qRegisterMetaType<const char*>("const char*");
*/
void _Processing::setActiveProcess(const char* slot)
{
    if(active_slot)
        disconnect(this,SIGNAL(inputImageRecived(char*,unsigned int,unsigned int)),this,active_slot);
    if(slot)
        connect(this,SIGNAL(inputImageRecived(char*,unsigned int,unsigned int)),this,slot);
    active_slot = slot;
}


/* slot : inputImage(char *img, unsigned int iwidth, unsigned int iheight)
 * connet this to image source in machine->camera
 * takes 8-bit grayscale image currently
*/
void _Processing::inputImage(char *img, unsigned int iwidth, unsigned int iheight)
{
    //call the internal slot which will be connected to active process
    emit inputImageRecived(img,iwidth,iheight);
}


/* process : passThroughFrame(char *img, unsigned int iwidth, unsigned int iheight)
 * convert 8-bit grayscale image to 32-bit rgba image
 * passes image as it is for displaying
*/
void _Processing::passThroughFrame(char *img, unsigned int iwidth, unsigned int iheight)
{
    if(!colorFrame) colorFrame = new char[MAX_FRAME_WIDTH*MAX_FRAME_HEIGHT*4];
    for (int index = 0; index < (iwidth*iheight); index++) {
        colorFrame[index*4] = img[index];
        colorFrame[index*4+1] = img[index];
        colorFrame[index*4+2] = img[index];
        colorFrame[index*4+3] = 255;
    }

    //send image out after processing is done
    emit outputImage(colorFrame,iwidth,iheight);
}

void _Processing::histogram(char *img, unsigned int iwidth, unsigned int iheight)
{
    std::array<unsigned int, 256> hist{};

    for (int index = 0; index < (iwidth*iheight); index++) {
        unsigned char v = img[index];
        //if((v<256)&&(v>=0))
        hist[v]+=1;
    }

    float scaler=55000.0;
    for (unsigned int w = 0; w < 256; w++) {
        for (unsigned int h = 0; h < 256; h++) {
            unsigned int index = 256*h+w;
            if(hist[w]>(float(256-h)*scaler/float(256)))
            {
                colorFrame2[index] = 0;
            }
            else {
                colorFrame2[index] = 255;
            }
        }
    }


    //qDebug() << "k means" <<  _Cpu_Compute::compute_k_means(hist);
    //send image out after processing is done
    emit outputImage2(colorFrame2,256,256);

    passThroughFrame(img,iwidth,iheight);
    //emit outputImage(colorFrame,iwidth,iheight);
}

/* Function : init()
 * this function has to called when thread starts
 * creates context in thread and initializes opengl functions
 *
*/
void _Processing::init()
{
    if (!context->create())
        qFatal("Cannot create the requested OpenGL context!");
    else {
        //make context active
        bool success = context->makeCurrent(surface);
        qDebug() << "making context current in thread processing" << QThread::currentThread()<< "success:" << success;

        //qDebug() << "making context current in thread" << QThread::currentThread()<< "success:" << success;

        initializeOpenGLFunctions();

        //this object will handle all compute operations
        gpu_compute = new _GPU_Compute();//should be created when context is active

        //checking if gl context is working

        qDebug() << QString::fromUtf8((char*)glGetString(GL_VERSION));

        //GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS

                int workGroupSizes[3] = { 0 };
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSizes[0]);
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSizes[1]);
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSizes[2]);
                int workGroupCounts[3] = { 0 };
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);
                int max_compute_workgroup_invocations;
                //glGetIntegeri(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,&max_compute_workgroup_invocations);

        qDebug()<< "workGroupSizes" << workGroupSizes[0] << " " << workGroupSizes[1]<< " " << workGroupSizes[2];
        qDebug()<< "workGroupCounts" << workGroupCounts[0] << " " << workGroupCounts[1]<< " " << workGroupCounts[2];
        qDebug()<< "max_compute_workgroup_invocations" << max_compute_workgroup_invocations;

    }
    colorFrame2 = new char[256*256];
}

/* process : markLineLaser
 * marks red line on detected line laser path
 *
*/
void _Processing::markLineLaser(char *img, unsigned int iwidth, unsigned int iheight)
{
    static bool init = true;

    //initialise empty textures for processing
    static _Texture texture(nullptr,iwidth,iheight);
    static _Texture texture_out(nullptr,iwidth,iheight);
    static _Texture texture_outt(nullptr,iwidth,iheight);

    if(init)
    {
    //load texture
    texture.load(GL_RED,GL_UNSIGNED_BYTE);
    texture_out.load(GL_RGBA,GL_UNSIGNED_BYTE);
    texture_outt.load(GL_R32I,GL_RED_INTEGER, GL_INT);

    //texture.unbind();
    init = false;
    }
    //Do the Processing

    //send the image to gpu texture
    texture.setImage(img,iwidth,iheight);

    //compute operation(edge detecton currently)
    //gpu_compute->compute_row_wise_mean(texture,texture_out);
    //gpu_compute->compute_threshold(texture,texture_outt);
//        gpu_compute->compute_sobel_edge(texture_outt,texture_out);
//        gpu_compute->compute_copy_8_to_32(texture,texture_outt);
//        gpu_compute->compute_copy_32_to_8(texture_outt,texture_out);
    //gpu_compute->compute_canny_edge(texture_outt,texture_out);
    gpu_compute->compute_row_wise_arg_max(texture,texture_outt);
    gpu_compute->compute_copy_red_to_rgba(texture,texture_out);
    gpu_compute->compute_mark_column_index(texture_outt,texture_out);
    //gpu_compute->compute_copy_32_to_8(texture_outt,texture_out);

    //get image from gpu texture
    //send signal to update display texture
    emit outputImage(gpu_compute->get_texture_image_framebuffer(texture_out),iwidth,iheight);

}

/* function: makeCurrent
 * set the context active
*/
bool _Processing::makeCurrent()
{
    return context->makeCurrent(surface);
}

/* function: doneCurrent
 * set the context inactive
*/
void _Processing::doneCurrent()
{
    context->doneCurrent();
}