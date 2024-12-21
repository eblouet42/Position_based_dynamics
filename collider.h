/******************************************************************************
 * \file collider.h
 * \brief Définition des structures et classes utilisées pour les collisions.
 *
 * Ce fichier définit les structures de particules, les contraintes statiques,
 * et les classes abstraites et concrètes pour gérer les collisions
 * (plans et sphères).
 ******************************************************************************/

#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include <optional>
#include <cmath>
#include <utility>

/**
 * \struct particle
 * \brief Représente une particule sur un plan avec ses propriétés physiques.
 *
 * Cette structure contient la position actuelle,
 * la position future prédite (qui change à chaque force appliquée à chaque itération),
 * la vitesse,le rayon et enfin la masse de la particule.
 */
struct particle {
    std::vector<double> pos; /**< Position actuelle de la particule. */
    std::vector<double> future_pos; /**< Position future calculée de la particule. */
    std::vector<double> velocity; /**< Vitesse actuelle de la particule. */
    double radius; /**< Rayon de la particule. */
    double mass; /**< Masse de la particule. */

    /**
     * \brief Opérateur de comparaison pour vérifier l'égalité entre deux particules.
     * \param other L'autre particule à comparer.
     * \return true si les particules sont égales, false sinon.
     */
    bool operator==(const particle& other) const {
        return pos==other.pos && future_pos==other.future_pos && velocity==other.velocity && radius==other.radius && mass==other.mass;}
};

/**
 * \struct StaticConstraint
 * \brief Représente une contrainte statique résultant d'une collision.
 *
 * Cette structure contient les informations sur le point d'impact,
 * la normale de collision et la particule impliquée.
 * Cette structure est définie dans cette classe et non context.h
 * car les deux fichiers ne peuvent s'inclure l'un et l'autre
 */
struct StaticConstraint {
    std::pair<double, double> pt_impact; /**< Point d'impact de la collision. */
    std::vector<double> normal; /**< Normale de la collision. */
    particle part; /**< Particule impliquée dans la collision. */
};

/**
 * \class collider
 * \brief Classe abstraite pour représenter un objet pouvant détecter des collisions.
 *
 * Cette classe définit une méthode virtuelle pure pour vérifier le contact
 * entre une particule et l'objet de collision.
 */
class collider {

public:
    /**
     * \brief Destructeur virtuel par défaut.
     */
    virtual ~collider() = default;

    /**
     * \brief Vérifie si une particule entre en contact avec cet objet.
     * \param particle La particule à tester.
     * \return Une contrainte statique si un contact est détecté, std::nullopt sinon.
     */
    virtual auto checkContact(const particle& particle)-> std::optional<StaticConstraint> = 0;
};

/**
 * \class plancollider
 * \brief Classe pour représenter un plan détectant des collisions.
 *
 * Cette classe hérite de la classe collider et implémente la vérification de contact
 * avec un plan défini par son point milieu, la longueur entre le milieu et chacun des bords, et sa normale.
 * Ses caractéristiques sont publiques pour être accédées dans la
 * méthode enforceStaticGroundConstraint de context.cpp
 */
class plancollider : public collider {

public:
    std::pair<double, double> origin; /**< Milieu du plan. */
    double length; /**< Distance entre le milieu du plan et un bord du plan. */
    std::vector<double> normal; /**< Normale du plan. */

    /**
     * \brief Constructeur du plan de collision.
     * \param point Point d'origine du plan.
     * \param length Longueur entre le milieu du plan et un des bords.
     * \param angle Angle d'orientation du plan (!en radians!).
     */
    plancollider(const std::pair<double, double>& point, double length, double angle)
        : origin(point), length(length), normal({-std::sin(angle),-std::cos(angle)}) {}

    /**
     * \brief Vérifie si une particule entre en contact avec le plan.
     * \param particle La particule à tester.
     * \return Une contrainte statique si un contact est détecté, std::nullopt sinon.
     */
    auto checkContact(const particle& particle)-> std::optional<StaticConstraint> override {
        //géométrie vectorielle: calcul du projeté orthogonal sur le plan en calculant d'abord la distance de la particule au plan
        double d_plan=normal[0]*(particle.future_pos[0]-origin.first)+normal[1]*(particle.future_pos[1]-origin.second);
        std::pair<double,double> impact_point={particle.future_pos[0]-d_plan*normal[0],particle.future_pos[1]-d_plan*normal[1]};

        double distance_au_centre=std::sqrt( (impact_point.first-origin.first)*(impact_point.first-origin.first)+(impact_point.second-origin.second)*(impact_point.second-origin.second));

        // On vérifie si la particule est à une distance plus petite que son rayon du plan,
        // et si la particule ne passe pas à côté de la surface plane
        if (distance_au_centre<=length && std::abs(d_plan)<particle.radius) {return StaticConstraint{impact_point,normal,particle};}
        else {return std::nullopt;}
    }
};

/**
 * \class spherecollider
 * \brief Classe pour représenter une sphère détectant des collisions.
 *
 * Cette classe hérite de la classe collider et implémente la vérification de contact
 * avec une sphère (un cercle, en 2D) définie par son centre et son rayon.
 * Ses caractéristiques sont publiques pour être accédées dans la
 * méthode enforceStaticGroundConstraint de context.cpp
 */
class spherecollider : public collider {

public:

    std::pair<double, double> origin; /**< Centre de la sphère. */
    double radius; /**< Rayon de la sphère. */

    /**
     * \brief Constructeur de la sphère de collision.
     * \param origin Centre de la sphère.
     * \param radius Rayon de la sphère.
     */
    spherecollider(const std::pair<double, double>& origin, double radius)
        : origin(origin), radius(radius) {}

    /**
     * \brief Vérifie si une particule entre en contact avec la sphère.
     * \param particle La particule à tester.
     * \return Une contrainte statique si un contact est détecté, std::nullopt sinon.
     */
    auto checkContact(const particle& particle)-> std::optional<StaticConstraint> override {
        double deltaX=particle.future_pos[0]-origin.first;
        double deltaY=particle.future_pos[1]-origin.second;
        double distance =std::sqrt(deltaX*deltaX+deltaY*deltaY);
        //Contact si la distance est plus petite que la somme des deux rayons (particule et sphère de collision)
        if (distance<=radius+particle.radius){
            std::vector<double> normal={deltaX/distance,deltaY/distance};
            std::pair<double,double> impact_point={origin.first+radius*normal[0],origin.second+radius*normal[1]};
            return StaticConstraint{impact_point,normal,particle};
        }else{return std::nullopt;}
    }
};

#endif // COLLIDER_H
