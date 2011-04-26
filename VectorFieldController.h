//
//  VectorFieldController.h
//  Flocking
//
//  Created by Michael Schaerfer on 4/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/multi_array.hpp>
#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <list>
#include <vector>


#include <boost/multi_array.hpp>


struct vectorCell {
    ci::Vec3f movement ;
    ci::Vec3f change ;
    vectorCell() : movement(0.0f, 0.0f, 0.0f), change(0.0f, 0.0f, 0.0f) {} ;
} ;

struct particle {
    ci::Vec3f position ;
    ci::Vec3f momentum ;
    ci::ColorAf color ;
    particle() : 
    position( 0.0f, 0.0f, 0.0f ), 
    momentum( 0.0f, 0.0f, 0.0f ) {} ;
} ;

struct emitter {
    ci::Vec3f position ;
    ci::ColorAf color ;
    int ttl ;
    unsigned emitRate ;
} ;

typedef boost::multi_array<vectorCell, 3> VectorField ;


class VectorFieldController {
public:
    VectorFieldController() ;
    void setup() ;
    void draw() ;
    void drawDebug() ;
    
    void update() ;
    
    VectorField field ;
    ci::Vec2f screenRatio ;
    
    std::list<particle> particles ;
    
} ;