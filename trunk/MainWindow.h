/*
 * MainWindow.h
 *
 *  Created on: Jun 2, 2010
 *      Author: Vincent
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "ui_MainWindow.h"
#include "Processor.h"
#include "Patch.h"

class PositionPlotWidget;

class MainWindow: public QMainWindow {
Q_OBJECT
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();

public slots:
	void showStatus(const QString & message);
	void updatePatchType();
	void on_imageScaleSpin_valueChanged(double scale);
	void on_rowColButton_clicked();
	void on_actionLoadImage_triggered();
	void on_actionLoadProject_triggered();
	void on_actionSaveProject_triggered();
	void on_actionExportScansitePssm_triggered();
	void on_actionExportAverageIntensities_triggered();
	void on_canvasSensitivitySlider_valueChanged(int sensitivityValue);
	void on_canvasScaleSlider_valueChanged(int scaleValue);
	void on_actionInvertImage_triggered();
	void on_actionApplyMask_triggered();
	void on_actionToggleMask_triggered();
	void on_excludeResidueButton_clicked();
	void on_excludedResidueEdit_textChanged();
	void on_phosphoEdit_textChanged();
	void on_actionResetView_triggered();
	void updateTypeLabels();
	void updateProcessorStartPoint();
	void updateProcessorEndPoint();
	void updatePlotScroll();
	void updateInspector();
	void updateProcessor();
	void updatePlot();
	void resizePlot();
	void updateMatrix();
	void updateProcessorMadRanges();
	void displayMatrix(const RealMatrix & matrix);
	void log(const QString & message);
	void setScansiteMatrixValue(int row, int column);

protected:
	void closeEvent(QCloseEvent * event);
	void initProject();
	bool eventFilter(QObject * watched, QEvent * event);
	// Transform to real coordinate
	inline int trc(int x, double factor);
	// Transform to screen coordinate
	inline int tsc(int x, double factor);

private:
	Ui::MainWindow ui;
	Processor processor;
	int transientPeriod;
	Patch::PatchType patchType;
	QPolygon patchPolygon;
	QString lastImagePath;
	QString lastProjectPath;
	QString lastExportPath;
	bool imageMasked;
	QList<PositionPlotWidget *> posPlotWidgets;

private:
	static QString getFormatName(QImage::Format format);
	QStringList getResidues(QPlainTextEdit * edit) const;
	void loadSettings();
	void saveSettings();
	void makeConnections();
	void debugMatrix(const RealMatrix * matrix);
	void debug(const QString &message);
};

#endif /* MAINWINDOW_H_ */
