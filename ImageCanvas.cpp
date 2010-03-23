/*
 * ImageCanvas.cpp
 *
 *  Created on: Feb 27, 2010
 *      Author: Vincent
 */

#include "ImageCanvas.h"
#include <QtGui>

ImageCanvas::ImageCanvas(QWidget *parent) :
    QLabel(parent) {
    angle = .0;
    startX = 0;
    startY = 0;
    endX = 0;
    endY = 0;
    this->col = 0;
    this->row = 0;
    this->radius = 0;
    showBg = false;
    min = max = 0;
    image = 0;
    scale = 1.0;
}

ImageCanvas::~ImageCanvas() {
}

QBitmap ImageCanvas::getMask() const {
    QBitmap mask(image->size());
    mask.clear();
    QBrush brush;
    QPen pen;
    brush.setColor(QColor(Qt::color1));
    brush.setStyle(Qt::SolidPattern);
    pen.setStyle(Qt::NoPen);
    QPainter painter(&mask);
    painter.setPen(pen);
    painter.setBrush(brush);

    painter.save();
    painter.translate(startX, startY);
    painter.rotate(angle);
    for (QVector<QRect>::const_iterator itr = circleGrid.begin(); itr
            != circleGrid.end(); ++itr) {
        painter.drawEllipse(*itr);
    }
    painter.restore();

    painter.save();
    brush.setColor(QColor(Qt::color0));
    painter.setBrush(brush);
    //	painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    for (QVector<QPolygon>::const_iterator itr = polygons.begin(); itr
            != polygons.end(); ++itr) {
        painter.drawPolygon(*itr);
    }
    painter.restore();
    painter.end();
    return mask;
}

void ImageCanvas::clearCircleGrid() {
    circleGrid.clear();
    update();
}

void ImageCanvas::clearPolygons() {
    polygons.clear();
    update();
}

void ImageCanvas::addCircleGrid(int leftTopX, int leftTopY, int rightBottomX,
        int rightBottomY, double radius, int col, int row, double angle) {
    startX = leftTopX;
    startY = leftTopY;
    endX = rightBottomX;
    endY = rightBottomY;
    this->angle = angle;
    this->col = col;
    this->row = row;
    this->radius = radius;
    drawCircleGrid();
}

void ImageCanvas::drawCircleGrid() {
    double xInterval = ((double) endX - (double) startX) / (double) (col - 1);
    double yInterval = ((double) endY - (double) startY) / (double) (row - 1);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            circleGrid.push_back(QRect(j * xInterval - radius, i * yInterval
                    - radius, 2 * radius, 2 * radius));
        }
    }
    this->angle = angle;
}

void ImageCanvas::addPolygon(const QPolygon &polygon) {
    polygons.push_back(polygon);
}

void ImageCanvas::rotate(double angle) {
    this->angle = angle;
}

void ImageCanvas::paintEvent(QPaintEvent *event) {
    QLabel::paintEvent(event);

    if (circleGrid.size() == 0) {
        return;
    }

    QPainter painter(this);
    painter.scale(scale, scale);
    painter.save();
    painter.translate(startX, startY);
    painter.rotate(angle);
    for (QVector<QRect>::iterator itr = circleGrid.begin(); itr
            != circleGrid.end(); ++itr) {
        painter.drawEllipse(*itr);
    }
    painter.restore();

    painter.save();
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(Qt::color1));
    //	painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    for (QVector<QPolygon>::const_iterator itr = polygons.begin(); itr
            != polygons.end(); ++itr) {
        painter.drawPolygon(*itr);
    }
    painter.restore();

    if (showBg) {
        painter.save();
        painter.drawRect(bg);
        painter.restore();
    }
}

void ImageCanvas::scanBackground() {
    if (!showBg) {
        return;
    }
    int leftx = bg.x();
    int lefty = bg.y();
    int rightx = leftx + bg.width();
    int righty = lefty + bg.height();
    min = 255;
    max = 0;
    for (int i = leftx; i < rightx; ++i) {
        for (int j = lefty; j < righty; ++j) {
            int gray = qGray(image->pixel(i, j));
            if (gray > max) {
                max = gray;
            }
            if (gray < min) {
                min = gray;
            }
        }
    }
    if (min < 255) {
        emit backgroundMinChanged(min);
    }
    if (max > 0) {
        emit backgroundMaxChanged(max);
    }
}
