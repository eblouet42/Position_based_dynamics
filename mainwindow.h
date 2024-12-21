/******************************************************************************
 * @file mainwindow.h
 * @brief Définition de la classe MainWindow.h qui hérite de QMainWindow
 *
 *
 * Ce fichier définit l'interface utilisateur de l'application et le cadre de la simulation.
 ******************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "drawarea.h"

// je sais pas ce que c'est mais trkl
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Classe pour représenter la fenêtre de l'application
 *
 * Cette classe définit l'architecture de la fenêtre de l'application avec une partie UI et une partie simulation
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
    * @brief Constructeur de MainWindow par défaut.
    */
    MainWindow(QWidget *parent = nullptr);

    /**
    * @brief Destructeur de MainWindow par défaut.
    */
    ~MainWindow();

private:
    Ui::MainWindow *ui; /**< Interface utilisateur*/
    DrawArea *draw_area; /**< Zone de la simulation*/
};
#endif // MAINWINDOW_H
