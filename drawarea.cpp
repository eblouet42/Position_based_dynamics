/******************************************************************************
 * @file drawaerea.cpp
 * @brief Implémentation des méthodes de la classe DrawArea utilisées pour actualiser l'interface utilisateur de la simulation
 *
 *
 * Ce fichier implémente les méthodes définies dans le fichier drawarea.h
 ******************************************************************************/

#include "drawarea.h"
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>

/**
* @brief Constructeur par défaut "explicite".
* Relie le timeout du timer à la méthode animate sur le DrawArea
* Définit aussi les colliders présent dès le départ
*/
DrawArea::DrawArea(QWidget *parent)
    : QOpenGLWidget{parent}, timer(new QTimer(this)) {
    // On lie le timeout du timer à la méthode animate sur le DrawArea
    connect(timer, &QTimer::timeout, this, &DrawArea::animate);
    timer->start(20);

    // On peut ici initialiser des colliders dans l'environnement de la simulation

    /*std::shared_ptr<collider> bordbas=std::make_shared<plancollider>(std::make_pair(this->width()/2, this->height()-100), this->width()/2, 0);
    std::shared_ptr<collider> bordhaut=std::make_shared<plancollider>(std::make_pair(this->width()/2, 10), this->width()/2,M_PI);
    std::shared_ptr<collider> bordgauche=std::make_shared<plancollider>(std::make_pair(10, this->height()/2), this->height()/2, -M_PI/2);
    std::shared_ptr<collider> borddroit=std::make_shared<plancollider>(std::make_pair(this->width()-10, this->height()/2), this->height()/2, M_PI/2);
    context.addCollider(bordbas);
    context.addCollider(bordhaut);
    context.addCollider(bordgauche);
    context.addCollider(borddroit);*/

    std::shared_ptr<collider> planCollider1_1=std::make_shared<plancollider>(std::make_pair(700.0, 80.0), 200.0, 0);
    std::shared_ptr<collider> planCollider1_2=std::make_shared<plancollider>(std::make_pair(700.0, 100.0), 200.0, -M_PI);
    std::shared_ptr<collider> planCollider1_3=std::make_shared<plancollider>(std::make_pair(500.0, 90.0), 10.0, M_PI/2);
    std::shared_ptr<collider> planCollider1_4=std::make_shared<plancollider>(std::make_pair(900.0, 90.0), 10.0, -M_PI/2);
    context.addCollider(planCollider1_1);
    context.addCollider(planCollider1_2);
    context.addCollider(planCollider1_3);
    context.addCollider(planCollider1_4);


    std::shared_ptr<collider> planCollider2_1=std::make_shared<plancollider>(std::make_pair(700.0, 140.0), 200.0, 0);
    std::shared_ptr<collider> planCollider2_2=std::make_shared<plancollider>(std::make_pair(700.0, 150.0), 200.0, M_PI);
    std::shared_ptr<collider> planCollider2_3=std::make_shared<plancollider>(std::make_pair(500.0, 145.0), 5.0, M_PI/2);
    std::shared_ptr<collider> planCollider2_4=std::make_shared<plancollider>(std::make_pair(900.0, 145.0), 5.0, -M_PI/2);
    context.addCollider(planCollider2_1);
    context.addCollider(planCollider2_2);
    context.addCollider(planCollider2_3);
    context.addCollider(planCollider2_4);

    std::shared_ptr<collider> planCollider3_1=std::make_shared<plancollider>(std::make_pair(100.0, 150.0), 100.0, -M_PI/8);
    std::shared_ptr<collider> planCollider3_2=std::make_shared<plancollider>(std::make_pair(100.0, 188.27), 92.39, -M_PI);
    context.addCollider(planCollider3_1);
    context.addCollider(planCollider3_2);

    std::shared_ptr<collider> sphereCollider1=std::make_shared<spherecollider>(std::make_pair(200.0, 50.0), 30.0);
    std::shared_ptr<collider> sphereCollider2=std::make_shared<spherecollider>(std::make_pair(350.0, 150.0), 20.0);
    std::shared_ptr<collider> sphereCollider3=std::make_shared<spherecollider>(std::make_pair(600, 200.0), 10.0);
    context.addCollider(sphereCollider1);
    context.addCollider(sphereCollider2);
    context.addCollider(sphereCollider3);

}

/**
* @brief Actualise la représentation à l'aide du contexte
* @param e Un QPaintEvent pour dessiner les particules et les colliders, ainsi que les bords de la simulation
*/
void DrawArea::paintEvent(QPaintEvent *e) {
    QPainter p(this);

    // Dessin de l'écran (blancs) et de ses bords (verts)
    p.setPen(Qt::white);
    p.setBrush(QBrush(Qt::white));
    QRectF screen(0, 0, this->width(), this->height());
    p.drawRect(screen);
    p.setPen(Qt::black);
    p.setBrush(QBrush(Qt::green));
    QRectF bordbas(0, this->height()-10, this->width(), 10);
    p.drawRect(bordbas);
    QRectF bordhaut(0, 0, this->width(), 10);
    p.drawRect(bordhaut);
    QRectF bordgauche(0, 0, 10, this->height());
    p.drawRect(bordgauche);
    QRectF borddroit(this->width()-10, 0, 10, this->height());
    p.drawRect(borddroit);

    // Dessin des particules
    p.setPen(Qt::yellow);
    p.setBrush(QBrush(Qt::red));
    for (const particle &particle : context.particles) {
        QRectF target(particle.pos[0] - particle.radius, particle.pos[1] - particle.radius, 2 * particle.radius, 2 * particle.radius);
        p.drawEllipse(target);
    }

    // Dessin des colliders
    p.setPen(Qt::blue);
    p.setBrush(QBrush(Qt::black));

    for (const auto& colliderPtr : context.colliders) {
        // std::dynamic_pointer_cast pour vérifier si le colliderPtr peut être cast en plancollider ou non
        if (auto plan = std::dynamic_pointer_cast<plancollider>(colliderPtr)) {
            // On trace la ligne entre les deux extrémités du plan
            QPointF center(plan->origin.first, plan->origin.second);
            QPointF normal(plan->normal[0], plan->normal[1]);
            double distance = plan->length;
            QPointF point1(center.x()+normal.y()*distance,center.y()-normal.x()*distance);
            QPointF point2(center.x()-normal.y()*distance,center.y()+normal.x()*distance);
            p.drawLine(point1, point2);
        }
        // std::dynamic_pointer_cast pour vérifier si le colliderPtr peut être cast en spherecollider ou non
        else if (auto sphere = std::dynamic_pointer_cast<spherecollider>(colliderPtr)) {
            // On trace un cercle représentant la sphère
            QRectF sp(sphere->origin.first-sphere->radius,sphere->origin.second-sphere->radius,2*sphere->radius,2*sphere->radius);
            p.drawEllipse(sp);
        }
    }
}

/**
* @brief Actualise le contexte (la liste de particule) lors d'un clic pour ajouter une particule à l'endroit du clic
* @param event Un QMouseEvent pour récupérer la position du clic
*/
void DrawArea::mouseDoubleClickEvent(QMouseEvent *event) {
    particle newParticle;
    // On récupère la position de la souris
    newParticle.pos={(double)event->position().x(),(double)event->position().y()};
    newParticle.future_pos={0,0};
    newParticle.velocity={30,-40};
    newParticle.future_velocity={0,0};
    newParticle.radius=radius;
    newParticle.mass=2;
    context.particles.push_back(newParticle);

    // On définit quelques caractéristiquees de la simulation ici, à chaque clic pour s'adapter à des variations de la fenêtre de simulation par l'utilisateur
    context.width=this->width();
    context.height=this->height();
    context.champ_de_force={0,gravity};

    // Méthode magique qui fait que toutes les méthodes se relancent
    update();
}

/**
* @brief Actualise le contexte après un certain pas de temps à l'aide de la méthode updatePhysicalSystem de Context.h
*/
void DrawArea::animate(){
    context.updatePhysicalSystem(((double) timer->interval())/100);
    // Méthode magique qui fait que toutes les méthodes se relancent
    update();
}
