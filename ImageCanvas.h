/*
 * ImageCanvas.h
 *
 *  Created on: Feb 27, 2010
 *      Author: Vincent
 */

#ifndef IMAGECANVAS_H_
#define IMAGECANVAS_H_

#include <QLabel>
#include <QBitmap>

#ifndef slots
#define slots
#endif

class ImageCanvas: public QLabel {
Q_OBJECT
public:
    ImageCanvas(QWidget *parent = 0);
    virtual ~ImageCanvas();
    QBitmap getMask() const;
    QVector<QRect> getGrid() const {
        return circleGrid;
    }
    QPoint getGridStartPoint() const {
        return QPoint(startX, startY);
    }
    QVector<QPolygon> getPolygons() const {
        return polygons;
    }
    double getGridAngle() const {
        return angle;
    }

public slots:
    void addCircleGrid(int leftTopX, int leftTopY, int rightBottomX,
            int rightBottomY, double radius, int col, int row, double angle);
    void drawBackground(int leftTopX, int leftTopY, int rightBottomX,
            int rightBottomY) {
        bg.setX(leftTopX);
        bg.setY(leftTopY);
        bg.setWidth(rightBottomX - leftTopX);
        bg.setHeight(rightBottomY - leftTopY);
        scanBackground();
    }
    void drawBackground(bool shown) {
        showBg = shown;
    }
    void loadCircleGrid(const QVector<QRect> &circleGrid) {
        this->circleGrid = circleGrid;
    }
    void drawCircleGrid();
    void addPolygon(const QPolygon &polygon);
    void loadPolygons(const QVector<QPolygon> &polygons) {
        this->polygons = polygons;
    }
    void loadGridStartPoint(const QPoint &startPoint) {
        startX = startPoint.x();
        startY = startPoint.y();
    }
    void loadGridAngle(double angle) {
        this->angle = angle;
    }
    void rotate(double angle);
    void clearCircleGrid();
    void clearPolygons();
    void clearLastPolygon() {
        if (polygons.size() > 0) {
            polygons.pop_back();
        }
    }
    void clearBackground() {
        bg = QRect();
        showBg = false;
    }
    void setImage(QImage *image) {
        this->image = image;
        if (image) {
            this->setPixmap(QPixmap::fromImage(*image).scaled(image->width()
                    * scale, image->height() * scale));
        }
    }
    void setScale(double scale) {
        this->scale = scale;
    }

    signals:
    void backgroundMinChanged(int min);
    void backgroundMaxChanged(int max);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void scanBackground();

private:
    QVector<QRect> circleGrid;
    QVector<QPolygon> polygons;
    //	QList<QRect> ellipses;
    QRect bg;
    double angle;
    double radius;
    int startX;
    int startY;
    int endX;
    int endY;
    int col;
    int row;
    bool showBg;
    int min;
    int max;
    QImage *image;
    double scale;
};

#endif /* IMAGECANVAS_H_ */
