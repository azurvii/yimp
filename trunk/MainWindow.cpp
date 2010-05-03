/*
 * MainWindow.cpp
 *
 *  Created on: Feb 26, 2010
 *      Author: Vincent
 */

#include "MainWindow.h"
#include <QtGui>
#include "PositionPlotWidget.h"
#include "DoubleDelegate.h"
#include "stat.h"

// Arbitrary strings for file type identification purpose
#define SAVE_FILE_HEADER_V1 ((quint32)0x293F29AC)
#define SAVE_FILE_HEADER_V2 ((quint32)0x49CF2E98)
QStringList MainWindow::designRowHeader;
QStringList MainWindow::designColHeader;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    ui.setupUi(this);
    ui.imageCanvas->installEventFilter(this);
    ui.warningLabel->setVisible(false);

    //	connect(ui.myCanvas, SIGNAL(xRotChanged(double)), ui.xRotLcd, SLOT(display(double)));
    //	connect(ui.myCanvas, SIGNAL(yRotChanged(double)), ui.yRotLcd, SLOT(display(double)));
    //	connect(ui.myCanvas, SIGNAL(zRotChanged(double)), ui.zRotLcd, SLOT(display(double)));
    //	connect(ui.myCanvas, SIGNAL(xPanChanged(double)), ui.xPanLcd, SLOT(display(double)));
    //	connect(ui.myCanvas, SIGNAL(yPanChanged(double)), ui.yPanLcd, SLOT(display(double)));
    //	connect(ui.myCanvas, SIGNAL(zPanChanged(double)), ui.zPanLcd, SLOT(display(double)));
    connect(ui.myCanvas, SIGNAL(sensitivityChanged(double)), ui.sensitivitySpin, SLOT(setValue(double)));
    connect(ui.myCanvas, SIGNAL(imageScaleChanged(double)), ui.imageScaleSpin, SLOT(setValue(double)));
    connect(ui.gridColumnSpin, SIGNAL(valueChanged(int)), SLOT(updateDrawings()));
    connect(ui.gridColumnSpin, SIGNAL(valueChanged(int)), SLOT(updatePlotScroll()));
    connect(ui.gridRowSpin, SIGNAL(valueChanged(int)), SLOT(updateDrawings()));
    connect(ui.gridRowSpin, SIGNAL(valueChanged(int)), SLOT(updatePlotScroll()));
    connect(ui.gridRadiusSpin, SIGNAL(valueChanged(double)), SLOT(updateDrawings()));
    connect(ui.gridStartXSpin, SIGNAL(valueChanged(double)), SLOT(updateDrawings()));
    connect(ui.gridStartYSpin, SIGNAL(valueChanged(double)), SLOT(updateDrawings()));
    connect(ui.gridEndXSpin, SIGNAL(valueChanged(double)), SLOT(updateDrawings()));
    connect(ui.gridEndYSpin, SIGNAL(valueChanged(double)), SLOT(updateDrawings()));
    connect(ui.gridAngleSpin, SIGNAL(valueChanged(double)), SLOT(updateDrawings()));
    connect(ui.gridGroup, SIGNAL(toggled(bool)), SLOT(updateDrawings()));
    connect(ui.refinePointerRadio, SIGNAL(toggled(bool)), SLOT(refineTriggered()));
    connect(ui.refinePolygonRadio, SIGNAL(toggled(bool)), SLOT(refineTriggered()));
    connect(ui.actionLookDownZ, SIGNAL(toggled(bool)), ui.myCanvas, SLOT(lookDownZ(bool)));
    connect(ui.actionOrthoView, SIGNAL(toggled(bool)), ui.myCanvas, SLOT(setOrthoView(bool)));
    connect(ui.bgStartXSpin, SIGNAL(valueChanged(int)), SLOT(updateDrawings()));
    connect(ui.bgStartYSpin, SIGNAL(valueChanged(int)), SLOT(updateDrawings()));
    connect(ui.bgEndXSpin, SIGNAL(valueChanged(int)), SLOT(updateDrawings()));
    connect(ui.bgEndYSpin, SIGNAL(valueChanged(int)), SLOT(updateDrawings()));
    connect(ui.scaleSpin, SIGNAL(valueChanged(double)), SLOT(changeScale(double)));
    connect(ui.imageCanvas, SIGNAL(backgroundMaxChanged(int)), ui.bgMaxSpin, SLOT(setValue(int)));
    connect(ui.imageCanvas, SIGNAL(backgroundMinChanged(int)), ui.bgMinSpin, SLOT(setValue(int)));
    connect(this, SIGNAL(scaleChanged(double)), SLOT(changeScale(double)));
    connect(ui.actionInvertedImage, SIGNAL(toggled(bool)), SLOT(updateDrawings()));
    connect(ui.plotHeightSpin, SIGNAL(valueChanged(int)), SLOT(resizePlot()));
    connect(ui.plotWidthSpin, SIGNAL(valueChanged(int)), SLOT(resizePlot()));
    connect(ui.isScansite2MatrixCheck, SIGNAL(toggled(bool)), SLOT(updateMatrix()));

    QSettings settings;
    settings.beginGroup("GUI");
    if (settings.contains("imageSplitterState")) {
        ui.imageSplitter->restoreState(
                settings.value("imageSplitterState").toByteArray());
    }
    if (settings.contains("canvasSplitterState")) {
        ui.canvasSplitter->restoreState(
                settings.value("canvasSplitterState").toByteArray());
    }
    if (settings.contains("matrixSplitterState")) {
        ui.matrixSplitter->restoreState(
                settings.value("matrixSplitterState").toByteArray());
    }
    if (settings.contains("plotSplitterState")) {
        ui.plotSplitter->restoreState(
                settings.value("plotSplitterState").toByteArray());
    }
    if (settings.contains("windowGeometry")) {
        restoreGeometry(settings.value("windowGeometry").toByteArray());
    }
    if (settings.contains("windowState")) {
        restoreState(settings.value("windowState").toByteArray());
    }
    if (settings.contains("currentTab")) {
        ui.tabWidget->setCurrentIndex(settings.value("currentTab", 0).toInt());
    }
    lastBlotPath = settings.value("lastBlotPath", QDir::homePath()).toString();
    lastMaskPath = settings.value("lastMaskPath", QDir::homePath()).toString();
    settings.endGroup();

    ui.imageScaleSpin->setValue(ui.myCanvas->getImageScale());
    ui.sensitivitySpin->setValue(ui.myCanvas->getSensitivity());
    //	ui.rotationBaseCombo->setCurrentIndex(ui.myCanvas->getRotationBase());

    imageScale = 1.0;
    refineType = REF_POINTER;
    isFirstBgPoint = false;
    averageModel = 0;

    if (designRowHeader.size() == 0) {
        designRowHeader << "-5" << "-4" << "-3" << "-2" << "-1" << "+1" << "+2"
                << "+3" << "+4";
    }
    if (designColHeader.size() == 0) {
        designColHeader << "K" << "I" << "H" << "G" << "F" << "E" << "D" << "C"
                << "A" << "V" << "T" << "S" << "R" << "Q" << "P" << "N" << "M"
                << "L" << "pY" << "pT" << "Y" << "W";
    }
    rowHeader = designRowHeader;
    colHeader = designColHeader;

    DoubleDelegate *delegate = new DoubleDelegate(this);
    ui.matrixTable->setItemDelegate(delegate);
    maskOn = false;
}

MainWindow::~MainWindow() {
}

bool MainWindow::eventFilter(QObject * watched, QEvent * event) {
    if (watched == ui.imageCanvas) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *> (event);
            int x = trc(mouseEvent->x(), imageScale);
            int y = trc(mouseEvent->y(), imageScale);
            if (ui.actionSelectBackground->isChecked()) {
                if (isFirstBgPoint) {
                    ui.bgStartXSpin->setValue(x);
                    ui.bgStartYSpin->setValue(y);
                    showStatus(tr("Background left-top (%1,%2)").arg(x).arg(y));
                    isFirstBgPoint = false;
                } else {
                    ui.bgEndXSpin->setValue(x);
                    ui.bgEndYSpin->setValue(y);
                    showStatus(
                            tr("Background right-bottom (%1,%2)").arg(x).arg(y));
                    ui.actionSelectBackground->toggle();
                }
            } else {
                //                const QPixmap *pmap = ui.imageCanvas->pixmap();
                switch (refineType) {
                case REF_ELLIPSE:
                case REF_POINTER:
                    //                    if (pmap && x < pmap->width() && y < pmap->height()) {
                    //                        QRgb rgb = pmap->toImage().pixel(x, y);
                    //                        showStatus(
                    //                                tr("Color @(%1,%2) is (%3,%4,%5), grayscale %6").arg(
                    //                                        x).arg(y).arg(qRed(rgb)).arg(
                    //                                        qGreen(rgb)).arg(qBlue(rgb)).arg(qGray(
                    //                                        rgb)));
                    //                    } else {
                    showStatus(tr("Position (%1,%2)").arg(x).arg(y));
                    //                    }
                    break;
                case REF_POLYGON:
                    polygon << QPoint(x, y);
                    showStatus(tr("Polygon point added (%1,%2)").arg(x).arg(y));
                    break;
                default:
                    break;
                }
            }
        } else if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *> (event);
            int x = trc(mouseEvent->x(), imageScale);
            int y = trc(mouseEvent->y(), imageScale);
            switch (refineType) {
            case REF_POLYGON:
                ui.imageCanvas->addPolygon(polygon);
                polygon = QPolygon();
                showStatus(tr("Polygon ended (%1,%2)").arg(x).arg(y));
                ui.imageCanvas->update();
                break;
            default:
                break;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"),
            lastBlotPath, tr("Images (*.png *.jpg *.bmp *.tif *.tiff)"));
    if (!fileName.isEmpty()) {
        lastBlotPath = fileName;
        ui.bgStartXSpin->setValue(0);
        ui.bgStartYSpin->setValue(0);
        ui.bgEndXSpin->setValue(0);
        ui.bgEndYSpin->setValue(0);
        ui.imageCanvas->clearBackground();
        loadImage(fileName);
        ui.gridStartXSpin->setRange(0, image.width() - 1);
        ui.gridStartYSpin->setRange(0, image.height() - 1);
        ui.gridEndXSpin->setRange(0, image.width() - 1);
        ui.gridEndYSpin->setRange(0, image.height() - 1);
        ui.bgStartXSpin->setRange(0, image.width() - 1);
        ui.bgStartYSpin->setRange(0, image.height() - 1);
        ui.bgEndXSpin->setRange(0, image.width() - 1);
        ui.bgEndYSpin->setRange(0, image.height() - 1);
        ui.gridGroup->setChecked(false);
    }
}

void MainWindow::on_actionInvert_triggered() {
    if (image.isNull()) {
        return;
    }
    image.invertPixels();
    invertedImage.invertPixels();
    ui.imageCanvas->setImage(&image);
    updateDrawings();
    ui.myCanvas->updateGL();
}

void MainWindow::on_sensitivitySpin_valueChanged(double sen) {
    ui.myCanvas->setSensitivity(sen);
}

void MainWindow::on_imageScaleSpin_valueChanged(double scale) {
    if (image.isNull()) {
        return;
    }
    ui.myCanvas->setImageScale(scale);
}

void MainWindow::on_actionShowAverage_toggled(bool shown) {
    if (image.isNull()) {
        return;
    }
    if (!ui.myCanvas->hasResults()) {
        ui.actionCalculateAverage->trigger();
    }
    ui.myCanvas->drawAverages(shown);
}

void MainWindow::on_actionCanvasShowImage_toggled(bool shown) {
    if (image.isNull()) {
        return;
    }
    ui.myCanvas->drawImage(shown);
}

void MainWindow::on_actionResetView_triggered() {
    ui.myCanvas->resetAll();
}

void MainWindow::on_actionShowCounts_triggered() {
    if (image.isNull()) {
        return;
    }
    if (!ui.myCanvas->hasResults()) {
        ui.actionCalculateAverage->trigger();
    }
    QVector<QVector<int> > counts = ui.myCanvas->getLastCounts(true);
    log(tr("Counts:"));
    for (QVector<QVector<int> >::const_iterator itr = counts.begin(); itr
            != counts.end(); ++itr) {
        QString line;
        for (QVector<int>::const_iterator rowItr = itr->begin(); rowItr
                != itr->end(); ++rowItr) {
            line += QString::number(*rowItr) + ",";
        }
        log(line);
    }
    log(tr("----------------Finished----------------"));
}

void MainWindow::on_actionClearLastRefinement_triggered() {
    if (image.isNull()) {
        return;
    }
    ui.imageCanvas->clearLastPolygon();
    ui.imageCanvas->update();
}

void MainWindow::on_actionSelectBackground_toggled(bool select) {
    if (image.isNull()) {
        return;
    }
    isFirstBgPoint = select;
    ui.imageCanvas->drawBackground(!select);
}

//void MainWindow::on_rotationBaseCombo_currentIndexChanged(int index) {
//	ui.myCanvas->setRotationBase((MyCanvas::RotBase) index);
//}

void MainWindow::log(const QString &line) {
    ui.textBrowser->append(line);
}

void MainWindow::updateDrawings() {
    ui.imageCanvas->clearCircleGrid();
    if (ui.gridGroup->isChecked()) {
        int startX = ui.gridStartXSpin->value();
        int startY = ui.gridStartYSpin->value();
        int endX = ui.gridEndXSpin->value();
        int endY = ui.gridEndYSpin->value();
        double angle = ui.gridAngleSpin->value();
        int col = ui.gridColumnSpin->value();
        int row = ui.gridRowSpin->value();
        double radius = ui.gridRadiusSpin->value();
        if ((col > 1 && (endX - startX) / (col - 1) < radius * 2) || (row > 1
                && (endY - startY) / (row - 1) < radius * 2)) {
            ui.warningLabel->setVisible(true);
        } else {
            ui.warningLabel->setVisible(false);
        }
        ui.imageCanvas->addCircleGrid(startX, startY, endX, endY, radius, col,
                row, angle);
    }
    if (ui.bgGroup->isChecked()) {
        ui.imageCanvas->drawBackground(ui.bgStartXSpin->value(),
                ui.bgStartYSpin->value(), ui.bgEndXSpin->value(),
                ui.bgEndYSpin->value());
    } else {
        ui.imageCanvas->clearBackground();
    }
    ui.imageCanvas->update();
}

void MainWindow::on_actionToggleMask_triggered() {
    if (image.isNull()) {
        return;
    }
    if (maskOn) {
        ui.myCanvas->disableMask();
        ui.myCanvas->update();
    } else {
        QBitmap mask = ui.imageCanvas->getMask();
        ui.myCanvas->setMask(mask);
        //  QPixmap pmap = *ui.imageCanvas->pixmap();
        //  pmap.setMask(mask);
        //  ui.imageCanvas->setPixmap(pmap);
        ui.myCanvas->update();
        //  ui.imageCanvas->update();
    }
    maskOn = !maskOn;
}

void MainWindow::on_actionApplyMask_triggered() {
    if (image.isNull()) {
        return;
    }
    maskOn = true;
    QBitmap mask = ui.imageCanvas->getMask();
    ui.myCanvas->setMask(mask);
    //  QPixmap pmap = *ui.imageCanvas->pixmap();
    //  pmap.setMask(mask);
    //  ui.imageCanvas->setPixmap(pmap);
    ui.myCanvas->update();
    //  ui.imageCanvas->update();
}

void MainWindow::on_actionSaveMask_triggered() {
    if (image.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save project"),
            lastMaskPath, tr("YIMP Projects (*.yim)"));
    if (fileName.isEmpty()) {
        return;
    }
    lastMaskPath = fileName;
    if (!fileName.endsWith(".yim", Qt::CaseInsensitive)) {
        fileName += ".yim";
    }
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QDataStream ds(&file);
    //    ds.setVersion(QDataStream::Qt_4_6);
    ds << SAVE_FILE_HEADER_V1;
    //	ds << ui.imageCanvas->getGridStartPoint();
    //	ds << ui.imageCanvas->getGrid();
    ds << ui.imageCanvas->getPolygons();
    //	ds << ui.imageCanvas->getGridAngle();
    int cols = ui.gridColumnSpin->value();
    int rows = ui.gridRowSpin->value();
    ds << cols;
    ds << rows;
    ds << ui.gridAngleSpin->value();
    ds << ui.gridStartXSpin->value();
    ds << ui.gridStartYSpin->value();
    ds << ui.gridEndXSpin->value();
    ds << ui.gridEndYSpin->value();
    ds << ui.gridRadiusSpin->value();
}

void MainWindow::on_actionSaveMaskImage_triggered() {
    if (image.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save mask image"), lastMaskPath, tr(
                    "Images (*.jpg *.png *.tif *.tiff)"));
    if (fileName.isEmpty()) {
        return;
    }
    lastMaskPath = fileName;
    ui.imageCanvas->getMask().save(fileName);
}

void MainWindow::on_actionLoadMask_triggered() {
    if (image.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open mask"),
            lastMaskPath, tr("YIMP Masks (*.yim)"));
    if (fileName.isEmpty()) {
        return;
    }
    lastMaskPath = fileName;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QDataStream ds(&file);
    //    ds.setVersion(QDataStream::Qt_4_6);
    quint32 header;
    ds >> header;
    if (!(header == SAVE_FILE_HEADER_V1 || header == SAVE_FILE_HEADER_V2)) {
        QMessageBox::critical(this, tr("Error"), tr(
                "This is not a valid mask file."));
        return;
    }
    //	QList<QRect> grid;
    QVector<QPolygon> polygons;
    //	QPoint gridStart;
    double gridAngle, radius;
    double startX, startY, endX, endY;
    int col, row;
    ds >> polygons >> col >> row >> gridAngle >> startX >> startY >> endX
            >> endY >> radius;
    //	ui.imageCanvas->loadCircleGrid(grid);
    //	ui.imageCanvas->loadGridStartPoint(gridStart);
    ui.imageCanvas->loadPolygons(polygons);
    //	ui.imageCanvas->loadGridAngle(gridAngle);
    ui.gridColumnSpin->setValue(col);
    ui.gridRowSpin->setValue(row);
    ui.gridAngleSpin->setValue(gridAngle);
    ui.gridStartXSpin->setValue(startX);
    ui.gridStartYSpin->setValue(startY);
    ui.gridEndXSpin->setValue(endX);
    ui.gridEndYSpin->setValue(endY);
    ui.gridRadiusSpin->setValue(radius);
    //    if (header == SAVE_FILE_HEADER_V2) {
    //
    //    }
    //	ui.imageCanvas->update();
    ui.gridGroup->setChecked(true);
    updateDrawings();
    ui.actionApplyMask->trigger();
}

void MainWindow::on_actionExportPspm_triggered() {
    if (image.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save PSPM"),
            QDir::homePath(), tr("Comma-separated values (*.csv)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream ds(&file);

    log(tr("Exporting PSPM..."));
    QVector<QVector<double> > aves = ui.myCanvas->calculateAverages(
            ui.imageCanvas->getGrid(), ui.gridColumnSpin->value(),
            ui.imageCanvas->getGridStartPoint());

    QVector<QVector<double> > output(aves.front().size());
    for (int i = 0; i < aves.size(); ++i) {
        double rowsum = 0.0;
        for (int j = 0; j < aves.front().size(); ++j) {
            rowsum += aves[i][j];
        }
        for (int j = 0; j < aves.front().size(); ++j) {
            output[j] << aves[i][j] / rowsum;
        }
    }

    for (int i = 0; i < output.size(); ++i) {
        QString line;
        for (int j = 0; j < output[i].size(); ++j) {
            line += QString::number(output[i][j]);
            if (j != output[i].size() - 1) {
                line += ",";
            }
        }
        ds << line << "\n";
    }
    log(tr("----------------Finished----------------"));
}

void MainWindow::on_actionExportAverages_triggered() {
    if (image.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save averages"),
            QDir::homePath(), tr("Comma-separated values (*.csv)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream ds(&file);

    log(tr("Exporting averages..."));
    QVector<QVector<double> > aves = ui.myCanvas->calculateAverages(
            ui.imageCanvas->getGrid(), ui.gridColumnSpin->value(),
            ui.imageCanvas->getGridStartPoint());

    QVector<QVector<double> > output(aves.front().size());
    for (int i = 0; i < aves.size(); ++i) {
        for (int j = 0; j < aves.front().size(); ++j) {
            output[j] << aves[i][j];
        }
    }

    for (int i = 0; i < output.size(); ++i) {
        QString line;
        for (int j = 0; j < output[i].size(); ++j) {
            line += QString::number(output[i][j]);
            if (j != output[i].size() - 1) {
                line += ",";
            }
        }
        ds << line << "\n";
    }
    log(tr("----------------Finished----------------"));
}

void MainWindow::on_actionExportScanSitePssm_triggered() {
    if (image.isNull()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr(
            "Save ScanSite PSSM"), QDir::homePath(),
            tr("ScanSite PSSM (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream ds(&file);

    log(tr("Exporting ScanSite PSSM..."));
    QStandardItemModel *theModel =
            static_cast<QStandardItemModel *> (ui.matrixTable->model());
    QString hheader;
    for (int j = 0; j < theModel->columnCount(); ++j) {
        hheader
                += theModel->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString();
        if (j < theModel->columnCount() - 1) {
            hheader += "\t";
        }
    }
    ds << hheader << "\n";
    for (int i = 0; i < theModel->rowCount(); ++i) {
        QString line;
        for (int j = 0; j < theModel->columnCount(); ++j) {
            line += QString::number(
                    theModel->item(i, j)->data(Qt::DisplayRole).toDouble());
            if (j < theModel->columnCount() - 1) {
                line += "\t";
            }
        }
        ds << line << "\n";
    }
    log(tr("----------------Finished----------------"));
}

void MainWindow::on_actionClearRefinement_triggered() {
    if (image.isNull()) {
        return;
    }
    ui.imageCanvas->clearPolygons();
    ui.actionApplyMask->trigger();
}

void MainWindow::on_actionClearGrid_triggered() {
    if (image.isNull()) {
        return;
    }
    ui.imageCanvas->clearCircleGrid();
    ui.actionApplyMask->trigger();
}

void MainWindow::on_actionCalculateAverage_triggered() {
    if (image.isNull()) {
        return;
    }
    QVector<QVector<double> > aves = ui.myCanvas->calculateAverages(
            ui.imageCanvas->getGrid(), ui.gridColumnSpin->value(),
            ui.imageCanvas->getGridStartPoint());
    log(tr("Averages:"));
    if (averageModel) {
        delete averageModel;
    }
    averageModel = new QStandardItemModel(rowHeader.size(), colHeader.size());
    averageModel->setHorizontalHeaderLabels(colHeader);
    averageModel->setVerticalHeaderLabels(rowHeader);
    QStandardItem *item;
    QVector<QVector<double> > output(aves.front().size());
    for (int i = 0; i < aves.size(); ++i) {
        for (int j = 0; j < aves.front().size(); ++j) {
            output[j] << aves[i][j];
            item = new QStandardItem;
            item->setData(aves[i][j], Qt::DisplayRole);
            averageModel->setItem(i, j, item);
        }
    }
    updatePlotScroll();
    updatePlot();
    updateMatrix();
    for (int i = 0; i < output.size(); ++i) {
        QString line;
        for (int j = 0; j < output[i].size(); ++j) {
            line += QString::number(output[i][j]);
            if (j != output[i].size() - 1) {
                line += ",";
            }
        }
        log(line);
    }
    log(tr("----------------Finished----------------"));
}

void MainWindow::on_actionSubtractBackground_triggered() {
    if (image.isNull()) {
        return;
    }
    int bgValue;
    bool inv = ui.actionInvertedImage->isChecked();
    if (inv) {
        bgValue = maxColor - ui.bgMinSpin->value();
    } else {
        bgValue = ui.bgMinSpin->value();
    }
    int pixelValue = 0;
    QImage image;
    if (inv) {
        image = this->invertedImage;
    } else {
        image = this->image;
    }
    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            pixelValue = qGray(image.pixel(i, j)) - bgValue;
            if (pixelValue < 0) {
                pixelValue = 0;
            }
            image.setPixel(i, j, (uint) pixelValue);
        }
    }
    if (inv) {
        this->invertedImage = image;
        image.invertPixels();
        this->image = image;
    } else {
        this->image = image;
        image.invertPixels();
        this->invertedImage = image;
    }
    ui.imageCanvas->setImage(&this->image);
    if (inv) {
        ui.myCanvas->renderGrayQImage(this->invertedImage);
    } else {
        ui.myCanvas->renderGrayQImage(this->image);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.beginGroup("GUI");
    settings.setValue("imageSplitterState", ui.imageSplitter->saveState());
    settings.setValue("plotSplitterState", ui.plotSplitter->saveState());
    settings.setValue("canvasSplitterState", ui.canvasSplitter->saveState());
    settings.setValue("matrixSplitterState", ui.matrixSplitter->saveState());
    settings.setValue("lastBlotPath", lastBlotPath);
    settings.setValue("lastMaskPath", lastMaskPath);
    settings.setValue("currentTab", ui.tabWidget->currentIndex());
    settings.setValue("windowGeometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
    settings.endGroup();
    settings.sync();
    event->accept();
}

void MainWindow::refineTriggered() {
    if (ui.refinePointerRadio->isChecked()) {
        refineType = REF_POINTER;
    } else if (ui.refinePolygonRadio->isChecked()) {
        refineType = REF_POLYGON;
        polygon = QPolygon();
    }
}

void MainWindow::loadImage(const QString fileName) {
    rowHeader = designRowHeader;
    colHeader = designColHeader;
    image.load(fileName);
    invertedImage = image;
    int grayDepth = image.depth();
    if (!image.isGrayscale()) {
        grayDepth = grayDepth / 4;
    }
    maxColor = (int) pow(2.0, (double) grayDepth) - 1;
    invertedImage.invertPixels();
    ui.imageCanvas->setImage(&image);
    if (ui.actionInvertedImage->isChecked()) {
        ui.myCanvas->renderGrayQImage(invertedImage);
    } else {
        ui.myCanvas->renderGrayQImage(image);
    }

    log(tr("File: %1").arg(fileName));
    log(tr("Size: %1x%2").arg(image.width()).arg(image.height()));
    log(tr("Format: %1 (%2)").arg(getFormatName(image.format())).arg(
            (int) image.format()));
    log(tr("Grayscale: %1").arg(image.isGrayscale() ? tr("Yes") : tr("No")));
    log(tr("Depth: %1").arg(image.depth()));
    log(tr("----------------Finished----------------"));

}

QString MainWindow::getFormatName(QImage::Format format) {
    switch (format) {
    case QImage::Format_Mono:
        return tr("Mono");
        break;
    case QImage::Format_Indexed8:
        return tr("Indexed8");
        break;
    case QImage::Format_RGB16:
        return tr("RGB16");
        break;
    case QImage::Format_RGB32:
        return tr("RGB32");
        break;
    case QImage::Format_ARGB32:
        return tr("ARGB32");
        break;
    case QImage::Format_Invalid:
        return tr("Invalid");
        break;
    default:
        return tr("Unknown");
        break;
    }
}

void MainWindow::showStatus(const QString &statusMessage) {
    ui.statusbar->showMessage(statusMessage);
}

void MainWindow::scaleImage(double scale) {
    if (!ui.imageCanvas->pixmap()) {
        return;
    }
    if (imageScale != scale) {
        double width = image.width() * scale;
        double height = image.height() * scale;
        imageScale = scale;
        QPixmap pmap = QPixmap::fromImage(image).scaled(width, height);
        //        resize(scale * pixmap()->size());
        ui.imageCanvas->setPixmap(pmap);
        emit scaleChanged(scale);
    }
}

void MainWindow::changeScale(double scale) {
    scaleImage(scale);
    ui.scaleSpin->setValue(scale);
    ui.imageCanvas->setScale(scale);
    updateDrawings();
}

void MainWindow::updatePlotScroll() {
    if (ui.plotScroll->widget()) {
        delete ui.plotScroll->takeWidget();
    }
    QWidget *cWidget = new QWidget(this);
    QVBoxLayout *vlayout = new QVBoxLayout;
    posPlotWidgets.clear();
    PositionPlotWidget *ppw;
    for (int i = 0; i < rowHeader.size(); ++i) {
        ppw = new PositionPlotWidget(this);
        vlayout->addWidget(ppw);
        ppw->setRange(0, maxColor);
        ppw->setPlotName(rowHeader[i]);
        //        connect(ui.plotHeightSpin, SIGNAL(valueChanged(int)), ppw, SLOT(setPlotHeight(int)));
        //        connect(ui.plotWidthSpin, SIGNAL(valueChanged(int)), ppw, SLOT(setPlotWidth(int)));
        connect(ui.plotTickSpin, SIGNAL(valueChanged(int)), ppw, SLOT(setAxisTicks(int)));
        connect(ppw, SIGNAL(madsChanged(double)), SLOT(updateMatrix()));
        posPlotWidgets.append(ppw);
    }
    cWidget->setLayout(vlayout);
    ui.plotScroll->setWidget(cWidget);
    resizePlot();
}

void MainWindow::updatePlot() {
    if (averageModel) {
        int rows = averageModel->rowCount();
        int cols = averageModel->columnCount();
        QStringList rowString;
        QList<double> rowData;
        for (int i = 0; i < rows; ++i) {
            rowString.clear();
            rowData.clear();
            for (int j = 0; j < cols; ++j) {
                rowString << colHeader[j];
                rowData.append(
                        averageModel->item(i, j)->data(Qt::DisplayRole).toDouble());
            }
            posPlotWidgets[i]->setData(rowString, rowData);
            posPlotWidgets[i]->setPlotName(rowHeader[i]);
            posPlotWidgets[i]->update();
        }
    }
}

void MainWindow::updateMatrix() {
    QStandardItemModel *displayModel;
    if (ui.isScansite2MatrixCheck->isChecked()) {
        displayModel = new QStandardItemModel(15, averageModel->columnCount());
        QStringList dispRowHeader;
        QStringList dispColHeader = colHeader;
        dispRowHeader << "-7" << "-6" << "-5" << "-4" << "-3" << "-2" << "-1"
                << "0" << "+1" << "+2" << "+3" << "+4" << "+5" << "+6" << "+7";
        displayModel->setVerticalHeaderLabels(dispRowHeader);
        displayModel->insertColumn(displayModel->columnCount()); // Add terminal penalty column
        dispColHeader << "*";
        displayModel->setHorizontalHeaderLabels(dispColHeader);
        QStandardItem *item;
        QStringList phosphoList = getResidues(ui.phosphoEdit);
        QList<int> phosphoIndeces;
        for (int j = 0; j < phosphoList.size(); ++j) {
            phosphoIndeces << colHeader.indexOf(phosphoList[j]);
        }
        for (int i = 0; i < displayModel->rowCount(); ++i) {
            if ((i >= 2 && i <= 6) || (i >= 8 && i <= 11)) {
                // real data go here
                int ssRow = i - 2;
                if (i > 7) {
                    --ssRow;
                }
                // Get the denominator
                double posMedian = posPlotWidgets[ssRow]->getMedian();
                double posMads = posPlotWidgets[ssRow]->getMads();
                double posMad = posPlotWidgets[ssRow]->getMad();
                double upBound = posMedian + posMad * posMads;
                double loBound = posMedian - posMad * posMads;
                QList<double> data;
                for (int j = 0; j < averageModel->columnCount(); ++j) {
                    data
                            << averageModel->item(ssRow, j)->data(
                                    Qt::DisplayRole).toDouble();
                }
                QList<double> nonsel;
                for (int j = 0; j < data.size(); ++j) {
                    if (phosphoIndeces.contains(ssRow)) {
                        nonsel << data[j];
                    } else if (data[j] <= upBound && data[j] >= loBound) {
                        nonsel << data[j];
                    }
                }
                double denom = mean(nonsel);
                for (int j = 0; j < displayModel->columnCount() - 1; ++j) {
                    item = new QStandardItem;
                    if (j < averageModel->columnCount()) {
                        item->setData(averageModel->item(ssRow, j)->data(
                                Qt::DisplayRole).toDouble() / denom,
                                Qt::DisplayRole);
                    } else {
                        item->setData(0, Qt::DisplayRole);
                    }
                    displayModel->setItem(i, j, item);
                }
            } else {
                // Padding zeros or ones
                for (int j = 0; j < displayModel->columnCount() - 1; ++j) {
                    if (i == 7) {
                        if (phosphoIndeces.contains(j)) {
                            item = new QStandardItem;
                            item->setData(21, Qt::DisplayRole);
                            displayModel->setItem(i, j, item);
                        } else {
                            item = new QStandardItem;
                            item->setData(0, Qt::DisplayRole);
                            displayModel->setItem(i, j, item);
                        }
                    } else {
                        item = new QStandardItem;
                        item->setData(1, Qt::DisplayRole);
                        displayModel->setItem(i, j, item);
                    }
                }
            }
            // Terminal penalty column
            item = new QStandardItem;
            item->setData(0.0001, Qt::DisplayRole);
            displayModel->setItem(i, displayModel->columnCount() - 1, item);
        }
    } else {
        displayModel = averageModel;
    }
    ui.matrixTable->setModel(displayModel);
}

void MainWindow::resizePlot() {
    int plotH = ui.plotHeightSpin->value();
    int plotW = ui.plotWidthSpin->value();
    QWidget *cWidget = ui.plotScroll->widget();
    cWidget->setFixedHeight(plotH * rowHeader.size());
    cWidget->setFixedWidth(plotW);
}

void MainWindow::on_excludedResidueEdit_textChanged() {
    ui.excludedResidueLabel->setText(getResidues(ui.excludedResidueEdit).join(
            ", "));
}

void MainWindow::on_phosphoEdit_textChanged() {
    ui.phosphoLabel->setText(getResidues(ui.phosphoEdit).join(", "));
}

QStringList MainWindow::getResidues(QPlainTextEdit * edit) const {
    QStringList toBeProcessed = edit->toPlainText().split(QRegExp("\\W+"),
            QString::SkipEmptyParts);
    QStringList returnList;
    for (int i = 0; i < toBeProcessed.size(); ++i) {
        if (colHeader.contains(toBeProcessed[i])) {
            returnList << toBeProcessed[i];
        }
    }
    return returnList;
}

void MainWindow::on_excludeResidueButton_clicked() {
    QStringList exList = getResidues(ui.excludedResidueEdit);
    QList<int> colIndeces;
    for (int i = 0; i < exList.size(); ++i) {
        colIndeces << colHeader.indexOf(exList[i]);
    }
    qSort(colIndeces);
    int curIndex;
    while (!colIndeces.isEmpty()) {
        curIndex = colIndeces.takeLast();
        colHeader.removeAt(curIndex);
        averageModel->removeColumn(curIndex);
    }
    updatePlotScroll();
    updatePlot();
    updateMatrix();
    on_excludedResidueEdit_textChanged();
}

void MainWindow::on_phosphoButton_clicked() {
    updateMatrix();
}

void MainWindow::about() {
    QMessageBox::about(
            this,
            tr("About YIMP"),
            tr(
                    "<b>YIMP</b> is developed in Yaffe Lab.\n"
                        "It is designed for generating <a href=\"http://scansite.mit.edu\">ScanSite</a> matrices from "
                        "analyzing dot blot images."));
}
