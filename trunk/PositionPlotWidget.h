/*
 * PositionPlotWidget.h
 *
 *  Created on: Apr 25, 2010
 *      Author: Vincent
 *
 * Define a composite for plot a single position of the blot;
 *  the real drawing will be done in DensityPlotWidget
 *
 */

#ifndef POSITIONPLOTWIDGET_H_
#define POSITIONPLOTWIDGET_H_

#include "ui_PositionPlotWidget.h"

class PositionPlotWidget: public QWidget {
Q_OBJECT
public:
    PositionPlotWidget(QWidget *parent = 0);
    virtual ~PositionPlotWidget();
    double getMads() const;
    double getMad() const;
    double getMedian() const;
    int getRowCount() const;
    int getColumnCount() const;
    QList<double> getData() const;
    QStringList getLabels() const;

public slots:
    void setRowCount(int rows);
    void setColumnCount(int columns);
    void setData(const QStringList &labels, const QList<double> &data);
    void setPlotName(const QString &name);
    void setMads(double madMultiplier);
    void setRange(int low, int high);
    void on_madsSpin_valueChanged(double value);
    void updatePlot();
    void setPlotHeight(int height);
    void setPlotWidth(int width);
    void setAxisTicks(int ticks);
    void updateSelection();

signals:
void madsChanged(double mads);

private:
    int rowCount;
    int colCount;
    QStringList labels;
    QList<double> data;
    QString name;
    Ui::PositionPlotWidget ui;
    double mads, posMad, posMedian;
};

#endif /* POSITIONPLOTWIDGET_H_ */
