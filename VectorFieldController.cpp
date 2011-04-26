//
//  VectorFieldController.cpp
//  Flocking
//
//  Created by Michael Schaerfer on 4/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "cinder/gl/gl.h"
#include "VectorFieldController.h"
#include <stdio.h>
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Rect.h"
#include <stdlib.h>
#include <math.h>
#include "cinder/Color.h"

using namespace ci;
using namespace ci::gl;
using namespace std;
using namespace boost::numeric::ublas;
using namespace boost ;


#define FIELD_SIZE 20
#define SCALE 1000

struct sector { 
    int x, y, z;
    double a;
} neighbour[8];


VectorFieldController::VectorFieldController() :
    field( boost::extents[FIELD_SIZE][FIELD_SIZE][FIELD_SIZE] )
{

    
    double a=0;
    for (unsigned i=0; i<8; ++i) {    
        neighbour[i].a = a;
        neighbour[i].x = round(sin(a));
        neighbour[i].y = round(cos(a));
        neighbour[i].z = round(cos(a));
        a += M_PI_4;
    }    
    
};


void VectorFieldController::setup() {
    
    unsigned fieldSize = FIELD_SIZE ;
    for( unsigned i = 0 ; i < fieldSize; ++ i ) {
        for( unsigned j = 0; j < fieldSize; ++ j ) {
            for( unsigned k = 0 ; k < fieldSize; ++ k ) {
                field[i][j][k].movement = Rand::randVec3f() ;
                field[i][j][k].change *= 0 ;
            }
        }
    }
    
};


const double M_PI2 = M_PI*2;

double angleDiff(double angle1, double angle2) {
    double diff = angle2 - angle1;
    while (diff < -M_PI) diff += M_PI2;
    while (diff > M_PI) diff -= M_PI2;
    return diff;
};

void VectorFieldController::update() {
    
    //FIELD:
    unsigned s1 = FIELD_SIZE ;
    unsigned s2 = FIELD_SIZE ;
    unsigned s3 = FIELD_SIZE ;
    

    unsigned w1 = FIELD_SIZE * SCALE ;
    unsigned w2 = FIELD_SIZE * SCALE ;
    unsigned w3 = FIELD_SIZE * SCALE ;
    
    
    for (unsigned i = 0; i < s1; ++ i) {
        for (unsigned j = 0; j < s2; ++ j) {
            for( unsigned k = 0; k < s3; ++ k ) {
                Vec3f &movem = field[i][j][k].movement ;
                
                double cellA = atan2(movem.y,movem.x); 
                
                for (unsigned n=0; n<8; ++n) {                                    
                    double diff = fabs( angleDiff(neighbour[n].a, cellA) );
                    int ix = i+neighbour[n].x;
                    int jy = j+neighbour[n].y;
                    int kz = k+neighbour[n].z;
                    
                    if (diff < 2.0f && ix < s1 && jy < s2 && kz < s3 && jy >= 0 && ix >= 0 && kz >= 0) {
                        Vec3f r = Rand::randVec3f() * 0.2f;            
                        Vec3f influence =  movem * (2-diff)/20 + r;
                        field[ix][jy][kz].change += influence;
                    }
                    
                }        
            }
        }        
    }
    
    for (unsigned i = 0; i < s1; ++ i) {
        for (unsigned j = 0; j < s2; ++ j) {
            for( unsigned k = 0; k < s3; ++ k ) {

                field[i][j][k].movement += field[i][j][k].change ;
                field[i][j][k].movement.limit(1.2f) ;
                field[i][j][k].change *= 0 ;
                
            }
        }        
    }
    
    
    
    
    //PARTICLES:
    for( list<particle>::iterator p = particles.begin(); p != particles.end(); ++p ){
        int x = p->position.x;
        int y = p->position.y;
        int z = p->position.z ;

        if (x < w1 && y < w2 && z < w3 ) { //&& z >= 0 && x >= 0 && x >= 0) {
            p->momentum += (field[x%FIELD_SIZE][y%FIELD_SIZE][z%FIELD_SIZE].movement)*0.5;
            p->position += p->momentum;
            p->momentum *= 0.99;
            
            
        } else {
            cout << " erased .. " ;
            particles.erase(p);
        }        
	}
    
}

void VectorFieldController::draw() {
    
    Vec2i wSize = cinder::app::getWindowSize();
    screenRatio = wSize/Vec2f(FIELD_SIZE,FIELD_SIZE);    
    
    /*//
     enableAlphaBlending();
     glColor4f(0.0f, 0.0f, 0.0f, 1.0f);    
     drawSolidRect( Rectf(0,0,wSize.x,wSize.y) );
     //*/
    
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    
    glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );    
    
    for( list<particle>::iterator p = particles.begin(); p != particles.end(); ++p ){
        float size = 0.5f/p->momentum.lengthSquared() ; // * 0.5 ;
        p->color.a = 0.5f/size;
        
        if (p->color.a > 0.4f) p->color.a = 0.4f;
        if (p->color.a < 0.0005f) p->color.a = 0.0005f;
        
        if (size > 15.0f) size = 15.0f;
        if( size < 0.005f) size = 0.005f ;
        
        
        glColor4f( p->color );
        
        gl::drawSphere( p->position, size, 12 ) ;
	}
    
};

void VectorFieldController::drawDebug() { 
    
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    glColor3f( 1.0f, 0.7f, 0.1f );
    for (unsigned i = 0; i < FIELD_SIZE; ++ i) {
        for (unsigned j = 0; j < FIELD_SIZE; ++ j) {
            for( unsigned k = 0 ; k < FIELD_SIZE ; ++k ) {
                Vec3f cellStart = Vec3f( i+0.5f, j+0.5f, k+0.5f ) ;
                Vec3f cellEnd = cellStart + field[i][j][k].movement ;
                gl::drawVector( cellStart, cellEnd ) ;
            }
        }        
    }    
    
}

