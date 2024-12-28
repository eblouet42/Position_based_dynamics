/******************************************************************************
 * @file mainwindow.cpp
 * @brief Implémente les méthodes définies dans le fichier mainwindow.h
 ******************************************************************************/

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "drawarea.h"

/**
* @brief Constructeur de MainWindow par défaut.
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Créer un objet DrawArea pour la simulation et l'ajouter dans l'UI
    this->draw_area = new DrawArea();
    ui->verticalLayout->addWidget(draw_area);

    QObject::connect(ui->resetButton, &QPushButton::pressed, &draw_area->context, &Context::resetSimulation);
    QObject::connect(ui->frictionButton, &QPushButton::pressed, &draw_area->context, &Context::frictionTrigger);
    QObject::connect(ui->gravityButton, &QPushButton::pressed, &draw_area->context, &Context::gravityChange);

}

/**
* @brief Destructeur de MainWindow par défaut.
*/
MainWindow::~MainWindow()
{
    delete ui;
}
