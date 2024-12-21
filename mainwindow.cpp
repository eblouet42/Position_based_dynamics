#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "drawarea.h"

/**
* \brief Constructeur de MainWindow par défaut.
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connecte le bouton "Rattatas" avec le text pour afficher "UwU?"
    QObject::connect(ui->start_button, &QPushButton::pressed, ui->label, [this](){(*ui->label).setText("UwU?");});

    // Créer un objet DrawArea pour la simulation et l'ajouter dans l'UI
    this->draw_area = new DrawArea();
    ui->verticalLayout->addWidget(draw_area);
}

/**
* \brief Destructeur de MainWindow par défaut.
*/
MainWindow::~MainWindow()
{
    delete ui;
}
