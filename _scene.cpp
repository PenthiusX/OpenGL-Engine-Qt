
#include "_scene.h"
#include "_tools.h"
#include <future>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
 * Class: _Scene()
 * This class define the scene manager , manages what needs to be rendered and what propertes need to be
 * set inside via a sceneentity object. essentially sets values in the scene entity object into the Renderer for drawing
 * Sets up Delegation to the class _Framebuffer,_Render and _Physics to work in one scene instance in cohision.
 * Autor: Aditya Mattoo

*/

/*
 * Constructor: _Scene class
*/
_Scene::_Scene(){
    isCamera = false;
    fboObject = new _FrameBuffer();
    fboObject->initialise();

    loopIndex = 0;
}
_Scene::~_Scene(){
    renderObjects.clear();
    delete r;
    delete fboObject;
}
/*
  ▪   ▐ ▄ ▪  ▄▄▄▄▄▪   ▄▄▄· ▄▄▌  ▪  ·▄▄▄▄•▄▄▄ .
  ██ •█▌▐███ •██  ██ ▐█ ▀█ ██•  ██ ▪▀·.█▌▀▄.▀·
  ▐█·▐█▐▐▌▐█· ▐█.▪▐█·▄█▀▀█ ██▪  ▐█·▄█▀▀▀•▐▀▀▪▄
  ▐█▌██▐█▌▐█▌ ▐█▌·▐█▌▐█ ▪▐▌▐█▌▐▌▐█▌█▌▪▄█▀▐█▄▄▌
  ▀▀▀▀▀ █▪▀▀▀ ▀▀▀ ▀▀▀ ▀  ▀ .▀▀▀ ▀▀▀·▀▀▀ • ▀▀▀
*/
/*
* binds the propertes set by the scene objectes into the
* renderer instace for rendering in the scene
* this is being called by the _GlWidget class.
*/
void _Scene::addSceneObject(_SceneEntity s){
    // Only sets the scene object if the camera has been set already and scene object is active
    if (s.getIsActive() == true){
        s.setOrderInIndex(renderObjects.size());
        if (isCamera){
            s.setOrderInIndex(renderObjects.size());
            r = new _Renderer();//creates a new renderare object for each sceneEntity that gets added to the scene
            r->setCamViewMatrix(cam.getEyePosition(), cam.getFocalPoint(), cam.getUpVector());
            r->setProjectionMatrix(resW,resH,cam.getFOV(),cam.getNearClipDistance(),cam.getFarClipDistance());
            r->initSceneEntityInRenderer(s);//sets the model data , matrix , tex and shders in the renderer
            renderObjects.push_back(r);//add the renderer object to array for batch render
            //
            if(s.getIsPhysicsObject()){
                _Physics phys;
                phys.initialiseSceneEntity(s);
                physVector.push_back(phys);
            }
        }
        else if(!isCamera){//use default values for camera if no camera set.
            r = new _Renderer();
            r->setCamViewMatrix(QVector3D(0.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 0.0));//set a default camera value
            r->setProjectionMatrix(resW,resH,cam.getFOV(),cam.getNearClipDistance(),cam.getFarClipDistance());
            r->initSceneEntityInRenderer(s);
            renderObjects.push_back(r);
            //
            if(s.getIsPhysicsObject()){
                _Physics phys;
                phys.initialiseSceneEntity(s);
                physVector.push_back(phys);
            }
        }
    }

    noOfVerticesInScene = 0;
    noOfUniquesObjectsInScene = 0;
    noOfUniquesObjectsInScene = renderObjects.size() - 6;
    for(uint i = 0; i<renderObjects.size(); i++){
        this->noOfVerticesInScene += renderObjects[i]->getSceneEntity().getModelInfo().getVertexArray().size();
    }
    //sets the no of triangle for debug
    noOfTrianglesInScene = noOfVerticesInScene * 0.333333333333;
}
/*
   ▄▄ • ▄▄▄ .▄▄▄▄▄   .▄▄ · ▄▄▄ .▄▄▄▄▄
  ▐█ ▀ ▪▀▄.▀·•██     ▐█ ▀. ▀▄.▀·•██
  ▄█ ▀█▄▐▀▀▪▄ ▐█.▪   ▄▀▀▀█▄▐▀▀▪▄ ▐█.▪
  ▐█▄▪▐█▐█▄▄▌ ▐█▌· • ▐█▄▪▐█▐█▄▄▌ ▐█▌·
  ·▀▀▀▀  ▀▀▀  ▀▀▀  •  ▀▀▀▀  ▀▀▀  ▀▀▀
*/
/*
 * returns the vector array of sceneObjects.
 * this is being called by the _GlWidget class.
*/
std::vector<_Renderer*> _Scene::getSceneObjects(){
    return renderObjects;
}
/*
 * function checks if the cmaera is attached and sets the local
 * camera object with the camera object passed via parameter for use in rendering
 * and setting the view matrix in the renderer
*/
void _Scene::addCamera(_Camera c){
    isCamera = true;
    cam = c;
}
/*
 * sets the camera updated values to every render entity matrix
*/
void _Scene::updateCamera(_Camera c){
    cam = c;
    if(isCamera == true)
        for (uint i = 0; i < renderObjects.size(); i++){
            renderObjects[i]->setCamViewMatrix(c.getEyePosition(),c.getFocalPoint(),c.getUpVector());
            renderObjects[i]->setProjectionMatrix(resW,resH,cam.getFOV(),cam.getNearClipDistance(),cam.getFarClipDistance());
        }
}
/*
 * return the required Entity from index
*!!!Non Optimal function avoid using the find Funtions in a loop!!!!!
*!!!You risk increasing the complexity of the loop from On to O(posinIndex - n)^2!!!
*They need to be run only once to find the object.
*/
/*

*/
_SceneEntity _Scene::findSceneEntity(uint iD){
    _SceneEntity empty;
    for(int f = 0 ; f < renderObjects.size() ; f++){
        if(renderObjects[f]->getSceneEntity().getId() == iD)
            return renderObjects[f]->getSceneEntity();
    }
    return empty;
}

_SceneEntity _Scene::findSceneEntity(QString tag){
    _SceneEntity empty;
    for(int f = 0 ; f < renderObjects.size(); f++){
        if(renderObjects[f]->getSceneEntity().getTag() == tag)
            return renderObjects[f]->getSceneEntity();
    }
    return empty;
}

uint _Scene::getTotalVertexCount(){
    return noOfVerticesInScene;
}

uint _Scene::getTotalTriangleCount(){
    return noOfTrianglesInScene;
}

uint _Scene::getSceneObjectCount(){
    return noOfUniquesObjectsInScene;
}
/*
 * returns the SceneEntity that is hitBy the mousePointer ray.
*/
_SceneEntity _Scene::getSceneEntityHitWithRay(){
    return rayHitSceneEntity;
}

/*
 */
void _Scene::removeSceneObject(uint index){
    renderObjects.erase(renderObjects.begin()+index);
}
void _Scene::removeSceneObject(_SceneEntity s){
    for(int r = 0 ; r < renderObjects.size() ; r++)
        if(renderObjects[r]->getSceneEntity().getId() == s.getId()){
            renderObjects[r] == NULL;
            renderObjects.erase(renderObjects.begin()+r);
        }
}

/*
  • ▌ ▄ ·.       ▄• ▄▌.▄▄ · ▄▄▄ .   ▄▄▄ .▄• ▄▌ ▄▄ .  ▐ ▄  ▄▄▄▄▄
  ·██ ▐███▪▪     █▪ █▌▐█ ▀. ▀▄.▀·   ▀▄.▀·█▪ █▌▀▄.▀· •█▌▐█ •██
  ▐█ ▌▐▌▐█· ▄█▀▄ █▌▐█▌▄▀▀▀█▄▐▀▀▪▄   ▐▀▀▪▄█▌ █▌▐▀▀▪▄ ▐█▐▐▌  ▐█.▪
  ██ ██▌▐█▌▐█▌.▐▌▐█▄█▌▐█▄▪▐█▐█▄▄▌   ▐█▄▄▌▐█▄█ ▐█▄▄▌ ██▐█▌  ▐█▌·
  ▀▀  █▪▀▀▀ ▀█▄▀▪ ▀▀▀  ▀▀▀▀  ▀▀▀     ▀▀▀  ▀▀  ▀▀▀  ▀▀ █▪  ▀▀▀
*/
/*

*/
void _Scene::setMousePositionInScene(QVector2D mousePos,Qt::MouseButton m){
    if(m == Qt::RightButton){
        mousePositionR = mousePos;
    }
    else if(m == Qt::LeftButton){
        mousePositionL = mousePos;
        //Physics+Helpers update on detached threads
        pu = std::thread(&_Scene::updateAllPhysicsObjectsOnce,this);
        ph = std::thread(&_Scene::updateHelpersOnce,this);
        if(pu.joinable()){pu.detach();}
        if(ph.joinable()){ph.detach();}
    }
    else if(m == Qt::MiddleButton){
        //Physics+Helpers update on detached threads
        pu = std::thread(&_Scene::updateAllPhysicsObjectsOnce,this);
        ph = std::thread(&_Scene::updateHelpersOnce,this);
        if(pu.joinable()){pu.detach();}
        if(ph.joinable()){ph.detach();}
    }
}
/*
        ▐ ▄     ▄▄▄  ▄▄▄ ..▄▄ · ▪  ·▄▄▄▄•▄▄▄ .
 ▪     •█▌▐█    ▀▄ █·▀▄.▀·▐█ ▀. ██ ▪▀·.█▌▀▄.▀·
  ▄█▀▄ ▐█▐▐▌    ▐▀▀▄ ▐▀▀▪▄▄▀▀▀█▄▐█·▄█▀▀▀•▐▀▀▪▄
 ▐█▌.▐▌██▐█▌    ▐█•█▌▐█▄▄▌▐█▄▪▐█▐█▌█▌▪▄█▀▐█▄▄▌
  ▀█▄▀ ▀▀ █▪    .▀  ▀ ▀▀▀  ▀▀▀▀ ▀▀▀·▀▀▀ • ▀▀▀
*/
/*
 * gets called on resize and all operations will run when the windows is resized
 * this is being called by the _GlWidget class.

*/
void _Scene::onResize(int w,int h){
    resW = w;
    resH = h;
    for(uint i = 0; i < renderObjects.size(); i++){
        renderObjects[i]->setProjectionMatrix(w,h,cam.getFOV(),cam.getNearClipDistance(),cam.getFarClipDistance());}
    //FBO init and updateTexture on Resize
    fboObject->setupFramebuffer(w,h);//FBO buffer and textures getSetup here.
}
/*
  ▄• ▄▌ ▄▄▄··▄▄▄▄   ▄▄▄· ▄▄▄▄▄▄▄▄ .
  █▪██▌▐█ ▄███▪ ██ ▐█ ▀█ •██  ▀▄.▀·
  █▌▐█▌ ██▀·▐█· ▐█▌▄█▀▀█  ▐█.▪▐▀▀▪▄
  ▐█▄█▌▐█▪·•██. ██ ▐█ ▪▐▌ ▐█▌·▐█▄▄▌
   ▀▀▀ .▀   ▀▀▀▀▀•  ▀  ▀  ▀▀▀  ▀▀▀
*/
/*
 * This function is render function that will call the glDraw fuinction in
 * the render final draw of all sceneEntity objects attached to scene.
 * this is being called by the _GlWidget class.

*/
void _Scene::render(){
    //sets the Frame for the framebufferObject.
    fboObject->setUpdatedFrame();// Rhe frames are being bound underneath in the draw() function below
    //--------------------------------------c
    //Frame to render is below
    for (uint i = 0; i < renderObjects.size(); i++)//Rendering Scene Object/Primitives
    {
        //Frame update
        renderObjects[i]->draw();////Render all objects that are active.

        if(renderObjects[i]->getSceneEntity().getTag()=="light"){
            glm::vec3 sp  =  renderObjects[i]->getSceneEntity().getPostion();//sets the light Position
            QVector4D col =  renderObjects[i]->getSceneEntity().getColor();//light color
                         //pos ,                   color,                           ambient,spec,diffuse
            lx =  _Light(glm::vec3(sp.x,sp.y,sp.z),glm::vec4(col.x(),col.y(),col.z(),1.),0.1,1.0,1.0);//stores as a _Light container
        }
        renderObjects[i]->updateLightUniforms(lx);//update the light uniform if existant.
    }
    //-----------------------------------------
   // Frame above is loaded in buffers and rendered on FBOquad below
    fboObject->setMousePos(mousePositionR); //sets the mouse pointervalues for the shader applied on the FBO quad
    fboObject->renderFrameOnQuad(); // sets the frame on the Quad that has been hardcoded into the function
}
/*
 *
 * this function updated at setintervals based on a clock timer.
 */
void _Scene::fixedUpdate(float intervalTime)
{
  if(_Tools::setIntervals(intervalTime))//everything runs inside this if statement.
  {
//   std::thread tUp(&_Scene::updateAllPhysicsObjectsLoop,this);
//   if(tUp.joinable()){tUp.detach();}
//   updateAllPhysicsObjectsLoop();
  }
}

/*
 ▄▄▄· ▄ .▄ ▄· ▄▌.▄▄ · ▪   ▄▄· .▄▄ ·
▐█ ▄███▪▐█▐█▪██▌▐█ ▀. ██ ▐█ ▌▪▐█ ▀.
 ██▀·██▀▐█▐█▌▐█▪▄▀▀▀█▄▐█·██ ▄ ▄▀▀▀█▄
▐█ ·•██▌▐▀ ▐█▀·.▐█▄▪▐█▐█▌▐███▌▐█▄▪▐█
.▀   ▀▀▀ ·  ▀ •  ▀▀▀▀ ▀▀▀·▀▀▀  ▀▀▀▀
*/
/*
 * update the physcs variables realtime or on MouseClick as currently configured
 * is called in the _scene class's render() function.

*/
void _Scene::updateAllPhysicsObjectsOnce(){
    if(physVector.size() > 0){
        for (uint index = 0; index < physVector.size(); index++){
            physVector[index].setSceneEntity(renderObjects[physVector[index].getSceneEntity().getIndexPosInScene()]->getSceneEntity());

            //Passing some essentials into the updateLoop for physics
            //updates the physics object instance and runs the main physics updateOperations.
            physVector[index].updateMousePhysics(glm::vec2(mousePositionL.x(),mousePositionL.y()),
                                            glm::vec3(cam.getEyePosition().x(),//Camera Position
                                                      cam.getEyePosition().y(),
                                                      cam.getEyePosition().z()),
                                            glm::vec2(resW,resH));

            //updates the status of scneEntity variable that get changed inside the Physis class on Collision Events.
            //style of implmentation can vary, its essentally updates the sceEntityObjet in the rendered if it is
            //getting collided with ray, So SceneEntity is the sharedVariable across classes Physics and Renderer
            uint pi = physVector[index].getSceneEntity().getIndexPosInScene();
            renderObjects[pi]->setSceneEntityInRenderer(physVector[index].getSceneEntity());//Is needed if we need to see changes to the sceneEntity in the main render as well.

            if(renderObjects[pi]->getSceneEntity().getisHitByRay()){
                rayHitSceneEntity = renderObjects[pi]->getSceneEntity();
            }
        }
    }
}

/*
 * update the physcs variables realtime and is relativel optmised to run in a loop.
 * is called in the _scene class's fixedUpdate() function.

 */
void _Scene::updateAllPhysicsObjectsLoop()
{    if(physVector.size() > 0){
//        physVector[loopIndex].setSceneEntity(renderObjects[physVector[loopIndex].getSceneEntity().getIndexPosInScene()]->getSceneEntity());
//        //Passing some essentials into the updateLoop for physics
//        //updates the physics object instance and runs the main physics updateOperations.
//        physVector[loopIndex].updateMousePhysics(glm::vec2(mousePositionL.x(),mousePositionL.y()),
//                                            glm::vec3(cam.getEyePosition().x(),//Camera Position
//                                                      cam.getEyePosition().y(),
//                                                      cam.getEyePosition().z()),
//                                            glm::vec2(resW,resH));


//       TriTriIntersection test,!!FITTING TEST!!----needs  modification of isHit scenario
        bool is = false;
        is = physVector[loopIndex].updateObjObjPhysics(physVector);
         _SceneEntity ss = physVector[loopIndex].getSceneEntity();
        ss.setIsHitByTri(is);
        renderObjects[ss.getIndexPosInScene()]->setSceneEntityInRenderer(ss);
//        if(renderObjects[ss.getIndexPosInScene()]->getSceneEntity().getIsHitByTri()){
//            triCollidedSceneEntity = renderObjects[ss.getIndexPosInScene()]->getSceneEntity();//sets the sceneEntity that has been set by ray.
//        }

        qDebug() << is;
        //--------------------------------------------
        loopIndex++;
        if(loopIndex >= physVector.size())
            loopIndex = 0;
    }
}
/*
   ▄ .▄▄▄▄ .▄▄▌   ▄▄▄·▄▄▄ .▄▄▄  .▄▄ ·
  ██▪▐█▀▄.▀·██•  ▐█ ▄█▀▄.▀·▀▄ █·▐█ ▀.
  ██▀▐█▐▀▀▪▄██▪   ██▀·▐▀▀▪▄▐▀▀▄ ▄▀▀▀█▄
  ██▌▐▀▐█▄▄▌▐█▌▐▌▐█▪·•▐█▄▄▌▐█•█▌▐█▄▪▐█
  ▀▀▀ · ▀▀▀ .▀▀▀ .▀    ▀▀▀ .▀  ▀ ▀▀▀▀
*/
/*
 */
void _Scene::updateHelpersOnce(){
    if(mxIndex > 0,minIndex > 0,pivotIndex > 0){
        glm::vec4 mx,mn,cntrd;
        glm::vec3 rot,pos;
        for (uint i = 0; i < physVector.size(); i++){
            if(physVector[i].getSceneEntity().getIsPhysicsHelper() && physVector[i].getSceneEntity().getisHitByRay()){
                glm::vec3 p = physVector[i].getRayTriIntersectionPoint();
                renderObjects[mPointerIndex]->setPosition(p);
                //sc = physVector[i].getSceneEntity().getScale();

                //Temporary Helpers for Max min extents
                mx = physVector[i].getSceneEntity().getModelInfo().getMaxExtent();
                mn = physVector[i].getSceneEntity().getModelInfo().getMinExtent();
                cntrd = physVector[i].getSceneEntity().getModelInfo().getCentroid();

                pos = physVector[i].getSceneEntity().getPostion();
                rot = physVector[i].getSceneEntity().getRotation();
            }
        }
        renderObjects[cIndex]->setPosition(cntrd);

        renderObjects[mxIndex]->setPosition(glm::vec3(mx.x,mx.y,mx.z));
        renderObjects[mxIndex]->setRotation(glm::vec3(1.5,0.0,0.0));
        //renderObjects[mxIndex]->lookAt(cam.getEyePosition());//buggy lookat

        renderObjects[minIndex]->setPosition(glm::vec3(mn.x,mn.y,mn.z));
        renderObjects[minIndex]->setRotation(glm::vec3(1.5,0.0,0.0));
        //renderObjects[minIndex]->lookAt(cam.getEyePosition());//buggy look at

        renderObjects[pivotIndex]->setPosition(pos);
        renderObjects[pivotIndex]->setRotation(rot);
    }
}

//Not in use
void _Scene::updateHelpersLoop(uint index){
    if(renderObjects[index]->getSceneEntity().getIsPhysicsHelper() &&  renderObjects[index]->getSceneEntity().getisHitByRay()){
        // binding the pivot object to focus object
        renderObjects[pivotIndex]->setPosition(renderObjects[index]->getSceneEntity().getPostion());
        renderObjects[pivotIndex]->setRotation(renderObjects[index]->getSceneEntity().getRotation());
    }
}
/*

*/
void _Scene::setHelperIndexVars(){
    pivotIndex = findSceneEntity("pivot").getIndexPosInScene();
    cIndex = findSceneEntity("cent").getIndexPosInScene();
    mxIndex = findSceneEntity("max").getIndexPosInScene();
    minIndex = findSceneEntity("min").getIndexPosInScene();
    mPointerIndex = findSceneEntity("mousePointerObject").getIndexPosInScene();
}
/*
*/
void _Scene::addAllHelperTypesInScene(){
    //----------Physics Helpers-------
    sph.setId(1);
    sph.setTag("boundingSphere");
    sph.setIsLineMode(true);
    sph.setPhysicsObject(_SceneEntity::Sphere,_SceneEntity::NoHelper);
    sph.setIsTransformationLocal(false);//keep it false(true only if object need to move like physics boides or particles)
    sph.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");
    sph.setColor(QVector4D(0.3,0.5,0.0,0.9));
    sph.setScale(1.0f);
    sph.setModelData(":/models/sphere.obj");
    sph.setIsActive(false);
    //---
    bb.setId(2);
    bb.setTag("boundingBox");
    bb.setIsLineMode(true);
    bb.setPhysicsObject(_SceneEntity::Box,_SceneEntity::NoHelper);
    bb.setIsTransformationLocal(false);
    bb.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");
    bb.setColor(QVector4D(0.5,1.0,1.0,0.9));
    bb.setScale(1.0f);
    bb.setModelData(":/models/cube.obj");//dont need to reparse modelfile
    bb.setIsActive(false);
    //
    addSceneObject(sph);
    addSceneObject(bb);
    //----------Orentation Helpers---------------
    pivot.setId(888);
    pivot.setTag("pivot");
    pivot.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");//texture Compliable shader not complete//need to pass UVs externally//
    pivot.setColor(QVector4D(1.0,1.0,1.0,1.0));
    pivot.setScale(1.0f);
    pivot.setModelData(":/models/pivot.obj");
    //---
    mpnt.setId(999);
    mpnt.setTag("mousePointerObject");
    mpnt.setIsTransformationLocal(false);
    mpnt.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");
    mpnt.setScale(0.02f);
    mpnt.setModelData(sph.getModelInfo());
    //---
    cnet.setId(991);
    cnet.setTag("cent");
    cnet.setIsTransformationLocal(false);
    cnet.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");
    cnet.setScale(0.07f);
    cnet.setModelData(sph.getModelInfo());
    //---
    max.setId(992);
    max.setTag("max");
    max.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");
    max.setColor(QVector4D(0.5,0.5,0.5,1.0));
    max.setScale(0.1f);
    max.setModelData(":/models/helpers/max.obj");
    //---
    min.setId(993);
    min.setTag("min");
    min.setShader(":/shaders/dmvshader.glsl", ":/shaders/dmfshader.glsl");
    min.setScale(0.1f);
    min.setModelData(":/models/helpers/min.obj");
    //
    addSceneObject(mpnt);
    addSceneObject(cnet);
    addSceneObject(min);
    addSceneObject(max);
    addSceneObject(pivot);
    //
    setHelperIndexVars();
}
