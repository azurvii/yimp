/*
 * DensityPlotWidget.h
 *
 *  Created on: Apr 25, 2010
 *      Author: Vincent
 *
 * Provide the canvas for density plot
 *
 */

#ifndef DENSITYPLOTWIDGET_H_
#define DENSITYPLOTWIDGET_H_

#include <QLabel>
#include <QImage>
#include <QColor>

class DensityPlotWidget: public QLabel {
Q_OBJECT
public:
	DensityPlotWidget(QWidget *parent = 0);
	virtual ~DensityPlotWidget();

public slots:
	void setRange(int low, int high);
	void clear();
	void drawValue(double value);
	void drawValues(const QList<double> &values);
	void drawHighlightedValue(double value);
	void drawHighlightedValues(const QList<double> &values);
	void drawSelectedValue(double value);
	void drawSelectedValues(const QList<double> &values);
	void setDimension(int width, int height);
	void setColor(const QColor & color);
	void setTickNumber(int numOfTicks);
	void setPlotWidth(int width);
	void setPlotHeight(int height);

protected:
	void paintEvent(QPaintEvent *event);
	void drawFrame(QPainter *painter);
	void drawValues(QPainter *painter, const QList<double> &list);
	void drawValues(QPainter *painter);
	void drawHighlightedValues(QPainter *painter);
	void drawSelectedValues(QPainter *painter);
	void drawLabels(QPainter *painter);

private:
	QList<double> valueList;
	QList<double> hlList;
	QList<double> selList;
	int rangeLow, rangeHigh;
	QImage image;
	double h, w;
	double margin;
	int tickNumber;
	QColor color;
};

#endif /* DENSITYPLOTWIDGET_H_ */
