/*
 * DensityPlotWidget.cpp
 *
 *  Created on: Apr 25, 2010
 *      Author: Vincent
 */

#include "DensityPlotWidget.h"
#include <QPainter>

DensityPlotWidget::DensityPlotWidget(QWidget *parent) :
	QLabel(parent) {
	rangeLow = rangeHigh = 0;
	margin = 10;
	tickNumber = 3;
	//    setBackgroundRole(QPalette::Light);
	this->setAutoFillBackground(true);
	this->setStyleSheet("background-color: rgb(255, 255, 255)");
}

DensityPlotWidget::~DensityPlotWidget() {
}

void DensityPlotWidget::drawValue(double value) {
	valueList.clear();
	valueList << value;
	update();
}

void DensityPlotWidget::drawHighlightedValue(double value) {
	hlList.clear();
	hlList << value;
	update();
}

void DensityPlotWidget::setRange(int low, int high) {
	rangeLow = low < high ? low : high;
	rangeHigh = low > high ? low : high;
}

void DensityPlotWidget::drawValues(const QList<double> &values) {
	valueList = values;
	update();
}

void DensityPlotWidget::drawHighlightedValues(const QList<double> &values) {
	hlList = values;
	update();
}

void DensityPlotWidget::clear() {
	valueList.clear();
	hlList.clear();
	selList.clear();
	update();
}

void DensityPlotWidget::paintEvent(QPaintEvent *event) {
	QLabel::paintEvent(event);
	h = height();
	w = width();

	// Geometry is too small for drawing
	if (h <= 20 || w <= 20) {
		return;
	}

	QPainter painter(this);
	QFontMetrics fm(painter.font());
	margin = fm.height();

	drawFrame(&painter);
	drawLabels(&painter);
	drawValues(&painter);
	drawHighlightedValues(&painter);
	drawSelectedValues(&painter);
}

void DensityPlotWidget::drawFrame(QPainter *painter) {
	painter->save();
	//    painter->drawRect(margin, 0, w - 2 * margin, h - margin);
	painter->drawLine(margin, h - margin, w - margin, h - margin);
	painter->restore();
}

void DensityPlotWidget::drawValues(QPainter *painter, const QList<double> &list) {
	painter->save();
	if (rangeHigh == rangeLow) {
		return;
	}
	double pixelsOverRange = (w - 2 * margin) / (rangeHigh - rangeLow);
	painter->translate(margin, 0);
	for (int i = 0; i < list.size(); ++i) {
		painter->drawLine((list[i] - rangeLow) * pixelsOverRange, margin,
				(list[i] - rangeLow) * pixelsOverRange, h - 2 * margin);
	}
	painter->restore();
}

void DensityPlotWidget::drawValues(QPainter *painter) {
	painter->save();
	painter->setPen(Qt::gray);
	drawValues(painter, valueList);
	painter->restore();
}

void DensityPlotWidget::drawHighlightedValues(QPainter *painter) {
	painter->save();
	painter->setPen(Qt::red);
	if (rangeHigh == rangeLow) {
		return;
	}
	double pixelsOverRange = (w - 2 * margin) / (rangeHigh - rangeLow);
	for (int i = 0; i < hlList.size(); ++i) {
		painter->drawLine((hlList[i] - rangeLow) * pixelsOverRange, margin / 2,
				(hlList[i] - rangeLow) * pixelsOverRange, h - 1.5 * margin);
	}
	painter->restore();
}

void DensityPlotWidget::drawSelectedValues(QPainter *painter) {
	painter->save();
	painter->setPen(Qt::blue);
	drawValues(painter, selList);
	painter->restore();
}

void DensityPlotWidget::drawSelectedValues(const QList<double> & values) {
	selList = values;
	update();
}

void DensityPlotWidget::drawSelectedValue(double value) {
	selList.clear();
	selList << value;
	update();
}

void DensityPlotWidget::drawLabels(QPainter *painter) {
	double inc = (rangeHigh - rangeLow) / (tickNumber + 1);
	double realInc = (w - 2 * margin) / (tickNumber + 1);
	for (int i = 0; i <= tickNumber; ++i) {
		painter->drawLine(margin + i * realInc, h - margin, margin + i
				* realInc, h - margin / 2);
		painter->drawText(margin + i * realInc, h - 1, QString::number(
				int(rangeLow + i * inc)));
	}
	QFontMetrics fm(painter->font());
	QString lastLabel = QString::number(int(rangeHigh));
	int stringWidth = fm.width(lastLabel);
	painter->drawLine(w - margin, h - margin, w - margin, h - margin / 2);
	painter->drawText(w - margin - stringWidth, h - 1, lastLabel);
}

void DensityPlotWidget::setDimension(int width, int height) {
	setFixedWidth(width);
	setFixedHeight(height);
}

void DensityPlotWidget::setTickNumber(int numOfTicks) {
	tickNumber = numOfTicks;
	update();
}

void DensityPlotWidget::setPlotWidth(int width) {
	setFixedWidth(width);
}

void DensityPlotWidget::setPlotHeight(int height) {
	setFixedHeight(height);
}

void DensityPlotWidget::setColor(const QColor & color) {
	this->color = color;
}
