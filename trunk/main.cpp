/*
 * main.cpp
 *
 *  Created on: Feb 26, 2010
 *      Author: Vincent
 */

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Azurvii");
    QApplication::setOrganizationDomain("azurvii.com");
    QApplication::setApplicationName("YIMP");
    MainWindow w;
    w.show();
    return a.exec();
}
