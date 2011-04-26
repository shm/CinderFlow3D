#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIO.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "VectorFieldController.h"
#include <stdio.h>

#include "cinder/qtime/MovieWriter.h"



#define NUM_INITIAL_PARTICLES 200
#define NUM_INITIAL_PREDATORS 2
#define NUM_PARTICLES_TO_SPAWN 5

using namespace ci;
using namespace ci::app;
using namespace std ;

class CinderFlow3DApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void setup();
	void update();
	void draw();
	ColorAf getColor() ;
    void createEmitters ( Vec3f pos, unsigned ttl ) ;
    void updateEmitters() ;
    
	// PARAMS
	params::InterfaceGl	mParams;
    qtime::MovieWriter	mMovieWriter;
    
    VectorFieldController field ;
    bool debugMode ;
	
	// CAMERA
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	Vec3f				mEye, mCenter, mUp;
	float				mCameraDistance;
    
    float ex ;
    float ey ;
    float ez ;

    list<emitter> emitters ;

    bool				mCentralGravity;
	bool				mFlatten;
	bool				mSaveFrames;
	bool				mIsRenderingPrint;
};

void CinderFlow3DApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 40.0f );
}

void CinderFlow3DApp::createEmitters( Vec3f pos, unsigned ttl = 70 ) {
    emitter e ;
    e.position = pos ;
    e.color = this->getColor() ;
    e.emitRate = 100;
    e.ttl = ttl ;
    emitters.push_back(e) ;
}

ColorAf CinderFlow3DApp::getColor() {
    if ( (rand() % 2) == 1) {
        return ColorAf(0.0f,0.2f,0.3f,0.5f);    
    } else {
        return ColorAf(0.3f,0.2f,0.0f,0.5f);            
    }
}



void CinderFlow3DApp::updateEmitters() {
    for( list<emitter>::iterator em = emitters.begin(); em != emitters.end(); ++em ){
        for (unsigned i = 0;i < em->emitRate; ++i) {
            Vec3f r = Rand::randVec3f() * 0.06f;
            particle p;
            p.position = em->position ;
            p.momentum = r;
            p.color = em->color;
            field.particles.push_back(p);            
        }
        --em->ttl;
        if (em->ttl < 1)
            emitters.erase(em);
    }
}




void CinderFlow3DApp::setup()
{	
	Rand::randomize();
	
	mCenter			= Vec3f( getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f );
	mCentralGravity = true;
	mFlatten		= false;
	
    mSaveFrames		= false;

	mIsRenderingPrint = false;
    
    debugMode = false ;
	
    
    ex = ey = ez = 0.0f ;
    
    // SETUP CAMERA
	mCameraDistance = 100.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	
    mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 5000.0f );

	// SETUP PARAMS
	mParams = params::InterfaceGl( "CinderFlow 3D", Vec2i( 200, 310 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams.addSeparator();
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=10.0 max=2000.0 step=1.0 keyIncr=s keyDecr=w" );
	mParams.addSeparator();
    
    
    field.setup() ;
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );

    qtime::MovieWriter::Format format;
    string path = getSaveFilePath();
    if( !path.empty() ) {
        mMovieWriter = qtime::MovieWriter( path, getWindowWidth(), getWindowHeight(), format );
    }
}

void CinderFlow3DApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'd' ) { 
        debugMode = !debugMode;
    }
    if( event.getChar() == 'e' ) { 
        this->createEmitters( Vec3f(++ex, ++ey, ++ez) );
    }

    if( event.getChar() == 'q' ) { 
        emitters.clear() ;
    }

}


void CinderFlow3DApp::update()
{
	mEye	= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
    
    field.update();
    updateEmitters();
}

void CinderFlow3DApp::draw()
{	
    
    if (debugMode) {
        field.drawDebug();
    } else {
        field.draw();
    }

	// DRAW PARAMS WINDOW
	params::InterfaceGl::draw();
    
    if( mMovieWriter )
		mMovieWriter.addFrame( copyWindowSurface() );
    
}

CINDER_APP_BASIC( CinderFlow3DApp, RendererGl )
