/******************************************************************************
 * \file context.cpp
 * \brief Définition des méthodes de la classe context définies dans le header context.h
 *
 * Ce fichier définit l'action d'un champ de force sur les particules et les intéractions
 * des particules au contact d'un obstacle statique ou d'une autre particule.
 ******************************************************************************/

#include "Context.h"
#include <cmath>

/**
* \brief Actualise le contexte de la simulation après un certain pas temporel en appelant chacun des méthodes ci-dessous.
* \param dt Le pas temporel de la simulation
*/
void Context::updatePhysicalSystem(float dt){
    applyExternalForce(dt);
    updateExpectedPosition(dt);
    addStaticContactConstraints();
    projectConstraints();
    updateVelocityAndPosition(dt);
}

/**
* \brief Applique les forces extérieures comme le champ de force au système de particules en mettant à jour leurs vitesses
* \param dt Le pas temporel de la simulation
*/
void Context::applyExternalForce(float dt){
    for (particle &p : particles) {
        p.velocity[0]+=champ_de_force[0]*dt;
        p.velocity[1]+=champ_de_force[1]*dt;
    }
};

/**
* \brief Met à jour les positions des particules après un pas de temps
* \param dt Le pas temporel de la simulation
*/
void Context::updateExpectedPosition(float dt){
    for (particle &p : particles) {
        p.future_pos[0]=p.pos[0]+p.velocity[0]*dt;
        p.future_pos[1]=p.pos[1]+p.velocity[1]*dt;
    }
};

/**
* \brief Ajoute des contraintes statiques si un contact avec un collider et une particule est détecté
*/
void Context::addStaticContactConstraints(){
    for (const auto &sc : colliders){
        for (particle &p:particles){
            auto constraint=sc->checkContact(p);
            // Le type std::optional peut être implicitement cast en booléen: 0 si std::nullopt, 1 sinon
            if (constraint){S_Constraints.push_back(*constraint);}
        }
    }
}

/**
* \brief Résoud les effets d'une contrainte statique en mettant à jour la nouvelle position future au niveau du point d'impact et la vitesse comme un rebond sur le collider
*/
void enforceStaticGroundConstraint(const StaticConstraint& constraint,particle& particle){

    const std::vector<double>& normal=constraint.normal;
    const std::pair<double,double>& contactPoint=constraint.pt_impact;

    double r=particle.radius;
    double p_sca = particle.velocity[0]*normal[0]+particle.velocity[1]*normal[1];

    particle.future_pos={contactPoint.first+normal[0]*r,contactPoint.second+normal[1]*r};
    particle.velocity={particle.velocity[0]-2*p_sca*normal[0],particle.velocity[1]-2*p_sca*normal[1]};
}

/**
* \brief Résoud toutes les contraintes (statiques, entre particule, avec les bords)
*/
void Context::projectConstraints(){

    // On procède particule par particule
    for (int i=0;i<particles.size();i++){
        particle &p=particles[i];

        // Premièrement, les interactions avec les colliders (s'il y a)
        // Cet ordre permet de s'assurer que les contraintes statiques soient appliquées avant qu'une modification d'une particule
        // depuis la méthode addStaticContactConstraints empêche la détection d'une contrainte à appliquer.
        for (StaticConstraint &sc:S_Constraints){
            if (sc.part==p){enforceStaticGroundConstraint(sc,p);}
        }

        // Ensuite, les interactions entre particules
            // On s'assure ici qu'on ne compte pas 2 fois une même interaction
            for (int j=i+1;j<particles.size();j++){
            particle &p2=particles[j];
            double deltaX=p.future_pos[0]-p2.future_pos[0];
            double deltaY=p.future_pos[1]-p2.future_pos[1];
            double distance=std::sqrt(deltaX*deltaX+deltaY*deltaY);
            if (distance<p.radius+p2.radius){
                // Echange simple des vitesses...
                std::swap(p.velocity,p2.velocity);
                // ...et petit décalage pour éviter la superposition
                int horizontal=(deltaX>0)*2-1;
                int vertical=(deltaY>0)*2-1;
                p.future_pos[0]+=horizontal;
                p.future_pos[1]+=vertical;
                p2.future_pos[0]-=horizontal;
                p2.future_pos[1]-=vertical;
            }
        }

        // Enfin, les interactions avec les bords (fonctionnent comme des colliders (plus simples))
        if (p.future_pos[1]>=height-10-p.radius){
             p.future_pos[1]=height-10-p.radius;
             p.velocity[1]=-p.velocity[1];
        }
        if (p.future_pos[1]<=10+p.radius){
            p.future_pos[1]=10+p.radius;
            p.velocity[1]=-p.velocity[1];
        }
        if (p.future_pos[0]>=width-10-p.radius){
            p.future_pos[0]=width-10-p.radius;
            p.velocity[0]=-p.velocity[0];
        }
        if (p.future_pos[0]<=10+p.radius){
            p.future_pos[0]=10+p.radius;
            p.velocity[0]=-p.velocity[0];
        }
    }
}

/**
* \brief Met à jour la position réelle des particules après dt
* A faire: mettre à jour la vitesse comme 1/dt * (p_future-p_init) avant.
* \param dt Le pas temporel de la simulation
*/
void Context::updateVelocityAndPosition(float dt){
    for (particle &p : particles) {
        p.pos=p.future_pos;
    }
};

/* Non implémentées:
void Context::dampVelocities(float dt){};
void Context::addDynamicContactConstraints(float dt){};
void Context::applyFriction(float dt){};
void Context::deleteContactConstraints(){};*/
