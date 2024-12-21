/******************************************************************************
 * @file context.cpp
 * @brief Définition des méthodes de la classe context définies dans le header context.h
 *
 * Ce fichier définit l'action d'un champ de force sur les particules et les intéractions
 * des particules au contact d'un obstacle statique ou d'une autre particule.
 ******************************************************************************/

#include "Context.h"
#include <cmath>
#include <iostream>
#include <ostream>

/**
* @brief Actualise le contexte de la simulation après un certain pas temporel en appelant chacun des méthodes ci-dessous.
* @param dt Le pas temporel de la simulation
*/
void Context::updatePhysicalSystem(float dt){
    applyExternalForce(dt);
    updateExpectedPosition(dt);
    addStaticContactConstraints();
    addDynamicContactConstraints();
    projectConstraints();
    deleteContactConstraints();
    applyFriction();
    updateVelocityAndPosition(dt);
}

/**
* @brief Applique les forces extérieures comme le champ de force au système de particules en mettant à jour leurs vitesses
* @param dt Le pas temporel de la simulation
*/
void Context::applyExternalForce(float dt){
    for (particle &p : particles) {
        p.future_velocity[0]=p.velocity[0]+champ_de_force[0]*dt;
        p.future_velocity[1]=p.velocity[1]+champ_de_force[1]*dt;
    }
};

/**
* @brief Met à jour les positions des particules après un pas de temps
* @param dt Le pas temporel de la simulation
*/
void Context::updateExpectedPosition(float dt){
    for (particle &p : particles) {
        p.future_pos[0]=p.pos[0]+p.future_velocity[0]*dt;
        p.future_pos[1]=p.pos[1]+p.future_velocity[1]*dt;
    }
};

/**
* @brief Ajoute des contraintes statiques si un contact avec un collider et une particule est détecté
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
* @brief Résoud les effets d'une contrainte statique en mettant à jour la nouvelle position future au niveau du point d'impact et la vitesse comme un rebond sur le collider
* @param constraint Une contrainte statique à résoudre
* @param particle La particule sur laquelle s'applique la contrainte, qui doit être la même que dans la contrainte
*/
void enforceStaticGroundConstraint(const StaticConstraint& constraint,particle& particle){

    const std::vector<double>& normal=constraint.normal;
    const std::pair<double,double>& contactPoint=constraint.pt_impact;

    double r=particle.radius;
    double p_sca = particle.future_velocity[0]*normal[0]+particle.future_velocity[1]*normal[1];

    particle.future_pos={contactPoint.first+normal[0]*r,contactPoint.second+normal[1]*r};
    particle.future_velocity={particle.future_velocity[0]-2*p_sca*normal[0],particle.future_velocity[1]-2*p_sca*normal[1]};
}

/**
* @brief Ajoute des contraintes dynamiques si un contact entre deux particules est détecté
*/
void Context::addDynamicContactConstraints() {
    // Parcourir toutes les combinaisons de particules
    for (int i=0;i<particles.size();++i) {
        particle &p1= particles[i];
        for (int j=i+1;j<particles.size();++j) {
            particle &p2 = particles[j];

            // Calculer la distance entre les deux particules
            double deltaX=p2.future_pos[0]-p1.future_pos[0];
            double deltaY=p2.future_pos[1]-p1.future_pos[1];
            double distance=std::sqrt(deltaX*deltaX+deltaY*deltaY);

            // Vérifier si elles se chevauchent (collision)
            if (distance<p1.radius+p2.radius) {
                // Ajouter une contrainte dynamique
                std::pair<double, double> impact_point = {p1.future_pos[0]+deltaX*(p1.radius/distance),p1.future_pos[1]+deltaY*(p1.radius/distance)};
                DynamicConstraint constraint = {impact_point, p1, p2};
                D_Constraints.push_back(constraint);
            }
        }
    }
}

/**
* @brief Résoud les effets d'une contrainte dynamique en mettant à jour la vitesse comme un rebond sur l'autre particule
* @param constraint Une contrainte dynamique à résoudre
* @param particle La particule sur laquelle s'applique la contrainte, qui doit être la même que l'une des deux particules de la contrainte
*/
void enforcedynamicConstraint(const DynamicConstraint& constraint,particle& Particle){

    auto& p1 = constraint.part1;
    auto& p2 = constraint.part2;

    double deltaX = p2.future_pos[0] - p1.future_pos[0];
    double deltaY = p2.future_pos[1] - p1.future_pos[1];
    double distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    std::vector<double> normal = {deltaX / distance, deltaY / distance};

    particle contact_particle;
    if (Particle==p1){contact_particle=p2;}else{contact_particle=p1;}

    // Echange des vitesses en norme et rebond
    double p_sca=(contact_particle.future_velocity[0]-Particle.future_velocity[0])*normal[0]+(contact_particle.future_velocity[1]-Particle.future_velocity[1])*normal[1];
    Particle.future_velocity[0]+=p_sca*normal[0];
    Particle.future_velocity[1]+=p_sca*normal[1];

    // On écarte la particule de l'autre
    double dist_dep=(p1.radius+p2.radius-distance)/2;
    int sign=1;
    if (Particle==p1){sign=-1;}
    Particle.future_pos[0]+=sign*dist_dep*normal[0];
    Particle.future_pos[1]+=sign*dist_dep*normal[0];
}

/**
* @brief Résoud toutes les contraintes (statiques, entre particule, avec les bords)
*/
void Context::projectConstraints(){
    // On procède particule par particule
    for (int i=0;i<particles.size();i++){
        particle &p=particles[i];

        // Interactions avec les colliders statiques
        for (StaticConstraint &sc:S_Constraints){
            if (sc.part==p){enforceStaticGroundConstraint(sc,p);}
        }
        // Interactions entre les particules
        for (DynamicConstraint &dc:D_Constraints){
            if (dc.part1==p || dc.part2==p){enforcedynamicConstraint(dc,p);}
        }
        // Interactions avec les bords (fonctionnent comme des colliders (plus simples et s'adaptent à la taille de la fenêtre))
        if (p.future_pos[1]>=height-10-p.radius){
             p.future_pos[1]=height-10-p.radius;
             p.future_velocity[1]=-p.future_velocity[1];
        }
        if (p.future_pos[1]<=10+p.radius){
            p.future_pos[1]=10+p.radius;
            p.future_velocity[1]=-p.future_velocity[1];
        }
        if (p.future_pos[0]>=width-10-p.radius){
            p.future_pos[0]=width-10-p.radius;
            p.future_velocity[0]=-p.future_velocity[0];
        }
        if (p.future_pos[0]<=10+p.radius){
            p.future_pos[0]=10+p.radius;
            p.future_velocity[0]=-p.future_velocity[0];
        }
    }
}

/**
* @brief Applique une force de frottement pour réduire la vitesse des particules
*/
void Context::applyFriction(){
    for (particle &p:particles){
        p.future_velocity[0]-=alpha*p.future_velocity[0];
        p.future_velocity[1]-=alpha*p.future_velocity[1];
    }
};

/**
*@brief Supprime les contraintes de contact
*/
void Context::deleteContactConstraints(){
    S_Constraints.clear();
    D_Constraints.clear();
};


/**
* @brief Met à jour la position réelle des particules après dt
* A faire: mettre à jour la vitesse comme 1/dt * (p_future-p_init) avant.
* @param dt Le pas temporel de la simulation
*/
void Context::updateVelocityAndPosition(float dt){
    for (particle &p : particles) {
        p.pos=p.future_pos;
        p.velocity=p.future_velocity;
    }
};
