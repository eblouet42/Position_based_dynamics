/******************************************************************************
 * @file context.h
 * @brief Définition de la classe context utilisée pour gérer le déplacement des
 * particules à chaque pas temporel.
 *
 * Ce fichier définit l'action d'un champ de force sur les particules et les intéractions
 * des particules au contact d'un obstacle statique ou d'une autre particule.
 ******************************************************************************/

#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <vector>
#include "collider.h"


/**
 * @class Context
 * @brief Classe pour représenter un ensemble de particules dans un environnement soumis à un champ de force.
 *
 * Cette classe implémente le contexte de la simulation défini par un vecteur de particules, des colliders,
 * un champ de force, un coefficient de frottement, des contraintes statiques, une hauteur et une largeur de l'environnement.
 * Elle implémente toutes les méthodes pour actualiser la situation à chaque pas temporel
 */
class Context {
public:
    std::vector<particle> particles; /**< Vecteur de particules */
    std::vector<std::shared_ptr<collider>> colliders; /**< Vecteur de colliders. L'ampoul magique a forcé l'utilisation de shared_ptr: à expliquer... */
    std::vector<double> champ_de_force; /**< Vecteur représentant un champ de force */
    double alpha; /**< Coefficient de frottement linéaire*/
    std::vector<StaticConstraint> S_Constraints; /**< Vecteur contenant les contraintes statiques ajoutées lors de la méthode addStaticContactConstraints pour les utiliser dans la méthode enforceStaticGroundConstraint du fichier context.cpp */
    std::vector<DynamicConstraint> D_Constraints; /**< Vecteur contenant les contraintes dynamiques ajoutées lors de la méthode addDynamicContactConstraints pour les utiliser dans la méthode enforceDynamicGroundConstraint du fichier context.cpp */
    int width; /**< Largeur de l'environnement*/
    int height; /**< Longueur de l'environnment*/

    /**
     * @brief Constructeur par défaut.
     */
    Context(){particles={},colliders={},champ_de_force={0,0},alpha=0,S_Constraints={},D_Constraints={},width=0,height=0;}

    /**
     * @brief Méthode pour ajouter un collider au vecteur de colliders.
     * @param newCollider Nouveau collider à ajouter à la simulation.
     */
    void addCollider(std::shared_ptr<collider> newCollider) {colliders.push_back(newCollider);}

    /**
     * @brief Actualise le contexte de la simulation après un certain pas temporel en appelant chacun des méthodes ci-dessous.
     * @param dt Le pas temporel de la simulation
     */
    void updatePhysicalSystem(float dt);

    /**
     * @brief Applique les forces extérieures comme le champ de force au système de particules en mettant à jour leurs vitesses
     * @param dt Le pas temporel de la simulation
     */
    void applyExternalForce(float dt);

    /**
     * @brief Met à jour les positions des particules après un pas de temps
     * @param dt Le pas temporel de la simulation
     */
    void updateExpectedPosition(float dt);

    /**
     * @brief Ajoute des contraintes statiques si un contact avec un collider et une particule est détecté
     */
    void addStaticContactConstraints();


    /**
     * @brief Ajoute des contraintes dynamiques si un contact entre deux particules est détecté
     */
    void addDynamicContactConstraints();

    /**
    * @brief Résoud toutes les contraintes (statiques, entre particule, avec les bords)
    */
    void projectConstraints();


    /**
     * @brief Applique une force de frottement pour réduire la vitesse des particules
     */
    void applyFriction();

    /**
     *@brief Supprime les contraintes de contact
     */
    void deleteContactConstraints();

    /**
     * @brief Met à jour la position réelle et la vitesse réelle des particules après dt
     * A faire: mettre à jour la vitesse comme 1/dt * (p_future-p_init) avant.
     * @param dt Le pas temporel de la simulation
     */
    void updateVelocityAndPosition(float dt);
};

#endif // CONTEXT_H
