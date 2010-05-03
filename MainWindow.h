/*
 * MainWindow.h
 *
 *  Created on: Feb 26, 2010
 *      Author: Vincent
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "ui_MainWindow.h"
#include <QImage>
#include <vector>
#include <QList>

class QStandardItemModel;
class PositionPlotWidget;

#ifndef slots
#define slots
#endif

class MainWindow: public QMainWindow {
Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    bool eventFilter(QObject * watched, QEvent * event);
    QStringList getResidues(QPlainTextEdit * edit) const;

public slots:
    void on_actionOpen_triggered();
    void on_actionInvert_triggered();
    void on_sensitivitySpin_valueChanged(double sen);
    void on_imageScaleSpin_valueChanged(double scale);
    //	void on_rotationBaseCombo_currentIndexChanged(int index);
    void on_actionToggleMask_triggered();
    void on_actionApplyMask_triggered();
    void on_actionSaveMask_triggered();
    void on_actionSaveMaskImage_triggered();
    void on_actionLoadMask_triggered();
    void on_actionClearRefinement_triggered();
    void on_actionClearGrid_triggered();
    void on_actionCalculateAverage_triggered();
    void on_actionShowAverage_toggled(bool shown);
    void on_actionCanvasShowImage_toggled(bool shown);
    void on_actionResetView_triggered();
    void on_actionShowCounts_triggered();
    void on_actionClearLastRefinement_triggered();
    void on_actionSelectBackground_toggled(bool select);
    void on_actionSubtractBackground_triggered();
    void on_actionExportPspm_triggered();
    void on_actionExportAverages_triggered();
    void on_actionExportScanSitePssm_triggered();
    void on_excludedResidueEdit_textChanged();
    void on_phosphoEdit_textChanged();
    void on_excludeResidueButton_clicked();
    void on_phosphoButton_clicked();
    void refineTriggered();
    void updateDrawings();
    void updatePlotScroll();
    void updatePlot();
    void updateMatrix();
    void scaleImage(double scale);
    void changeScale(double scale);
    void resizePlot();
    void about();

signals:
void scaleChanged(double factor);

protected:
    void closeEvent(QCloseEvent *event);

private:
    enum RefineType {
        REF_POINTER, REF_POLYGON, REF_ELLIPSE
    };

private:
    void showStatus(const QString &statusMessage);
    void log(const QString &line);
    QString getFormatName(QImage::Format format);
    void loadImage(const QString fileName);
    // Transform to real coordinate
    int trc(int x, double factor) {
        return x / factor;
    }
    // Transform to screen coordinate
    int tsc(int x, double factor) {
        return x * factor;
    }

private:
    Ui::MainWindow ui;
    QImage image;
    double maxColor;
    QImage invertedImage;
    RefineType refineType;
    QPolygon polygon;
    bool isFirstBgPoint;
    double imageScale;
    QList<PositionPlotWidget *> posPlotWidgets;
    static QStringList designRowHeader, designColHeader;
    QStringList rowHeader, colHeader;
    QStandardItemModel *averageModel;
    QString lastBlotPath;
    QString lastMaskPath;
    bool maskOn;
};

#endif /* MAINWINDOW_H_ */
