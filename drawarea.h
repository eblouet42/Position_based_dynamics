/******************************************************************************
 * @file drawaerea.h
 * @brief Définition de la classe DrawArea utilisée pour actualiser l'interface utilisateur de la simulation
 *
 *
 * Ce fichier définit des méthodes pour actualiser l'interface à chaque pas temporel, ainsi qu'une méthode pour ajouter une particule dynamiquement
 ******************************************************************************/

#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QtOpenGLWidgets/qopenglwidget.h>
#include <QWidget>
#include <QTimer>
#include "Context.h"

/**
 * @class DrawArea
 * @brief Classe pour représenter l'interface de la simulation
 *
 * Cette classe définit la représentation des colliders et des particles à l'écran, et l'action d'un double clic pour ajouter une particule à l'endroit du clic.
 */
class DrawArea : public QOpenGLWidget {
    Q_OBJECT //je sais pas ce que c'est mais c'est là

private:
    double radius=10; /**< Rayon des particules ajoutées */
    QTimer *timer;    /**< Timer pour le pas de temps */

public:
    Context context=Context(); /**< Contexte de la simulation*/

    /**
    * @brief Constructeur par défaut "explicite".
    * Relie le timeout du timer à la méthode animate sur le DrawArea
    * Définit aussi les colliders présent dès le départ
    */
    explicit DrawArea(QWidget *parent = nullptr);

    /**
     * @brief Actualise la représentation à l'aide du contexte
     * @param e Un QPaintEvent pour dessiner les particules et les colliders, ainsi que les bords de la simulation
     */
    void paintEvent(QPaintEvent *e) override;

    /**
     * @brief Actualise le contexte (la liste de particule) lors d'un clic pour ajouter une particule à l'endroit du clic
     * @param event Un QMouseEvent pour récupérer la position du clic
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    /**
     * @brief Actualise le contexte après un certain pas de temps à l'aide de la méthode updatePhysicalSystem de Context.h
     */
    void animate();

signals: //jsp?
};

#endif // DRAWAREA_H
