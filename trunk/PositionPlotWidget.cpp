/*
 * PositionPlotWidget.cpp
 *
 *  Created on: Apr 25, 2010
 *      Author: Vincent
 */

#include "PositionPlotWidget.h"
#include "stat.h"

PositionPlotWidget::PositionPlotWidget(QWidget *parent) :
    QWidget(parent) {
    ui.setupUi(this);
    //    ui.orderTable->resizeColumnsToContents();
    mads = ui.madsSpin->value();
    connect(ui.orderTable, SIGNAL(itemSelectionChanged()), SLOT(
            updateSelection()));
    posMad = posMedian = 0;
    connect(ui.madsSpin, SIGNAL(valueChanged(double)), SIGNAL(madsChanged(double)));
}

PositionPlotWidget::~PositionPlotWidget() {
}

void PositionPlotWidget::setRowCount(int rows) {
    rowCount = rows;
}

void PositionPlotWidget::setColumnCount(int columns) {
    colCount = columns;
}

int PositionPlotWidget::getColumnCount() const {
    return colCount;
}

int PositionPlotWidget::getRowCount() const {
    return rowCount;
}

void PositionPlotWidget::setData(const QStringList &labels,
        const QList<double> &data) {
    this->labels.clear();
    this->data.clear();
    QList<double> tempData = data;
    QStringList tempLabels = labels;
    ui.orderTable->setColumnCount(labels.size());
    int minIndex;
    for (int i = 0; i < data.size(); ++i) {
        minIndex = minIndexOf(tempData);
        this->data << tempData.takeAt(minIndex);
        this->labels << tempLabels.takeAt(minIndex);
        ui.orderTable->setItem(0, i, new QTableWidgetItem(this->labels.last()));
    }
    ui.orderTable->resizeColumnsToContents();
    ui.orderTable->setFixedHeight(ui.orderTable->rowHeight(0));
    updatePlot();
}

void PositionPlotWidget::setPlotName(const QString &name) {
    this->name = name;
    ui.nameLabel->setText(name);
}

void PositionPlotWidget::setMads(double madMultiplier) {
    mads = madMultiplier;
    updatePlot();
}

double PositionPlotWidget::getMads() const {
    return mads;
}

//void PositionPlotWidget::update() {
//    ui.densityPlot->update();
//}

void PositionPlotWidget::setRange(int low, int high) {
    ui.densityPlot->setRange(low, high);
    updatePlot();
}

void PositionPlotWidget::on_madsSpin_valueChanged(double value) {
    setMads(value);
}

void PositionPlotWidget::updatePlot() {
    if (data.size() == 0) {
        return;
    }
    ui.densityPlot->clear();
    QList<double> highlighted;
    posMad = mad(data);
    posMedian = median(data);
    highlighted << posMedian << posMedian - posMad * mads << posMedian + posMad
            * mads;
    ui.densityPlot->drawValues(data);
    ui.densityPlot->drawHighlightedValues(highlighted);
    updateSelection();
    ui.densityPlot->update();
}

void PositionPlotWidget::setPlotHeight(int height) {
    ui.densityPlot->setPlotHeight(height);
}

void PositionPlotWidget::setPlotWidth(int width) {
    ui.densityPlot->setPlotWidth(width);
}

void PositionPlotWidget::setAxisTicks(int ticks) {
    ui.densityPlot->setTickNumber(ticks);
}

void PositionPlotWidget::updateSelection() {
    QList<QTableWidgetItem *> items = ui.orderTable->selectedItems();
    QList<double> selected;
    for (int i = 0; i < items.size(); ++i) {
        selected << data[items[i]->column()];
    }
    ui.densityPlot->drawSelectedValues(selected);
}

double PositionPlotWidget::getMad() const {
    return posMad;
}

double PositionPlotWidget::getMedian() const {
    return posMedian;
}

QList<double> PositionPlotWidget::getData() const {
    return data;
}

QStringList PositionPlotWidget::getLabels() const {
    return labels;
}
