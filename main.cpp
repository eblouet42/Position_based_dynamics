/******************************************************************************
 * @file main.cpp
 * @brief Permet de lancer l'application de simulation de particules
 ******************************************************************************/

#include "mainwindow.h"

#include <QApplication>

/**
* @brief Ca fait un truc qui fait que ça lance l'appli
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
