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
    QVector<QRect> getGrid() const;
    QPoint getGridStartPoint() const;
    QVector<QPolygon> getPolygons() const;
    double getGridAngle() const;

public slots:
    void addCircleGrid(int leftTopX, int leftTopY, int rightBottomX,
            int rightBottomY, double radius, int col, int row, double angle);
    void drawBackground(int leftTopX, int leftTopY, int rightBottomX,
            int rightBottomY);
    void drawBackground(bool shown);
    void loadCircleGrid(const QVector<QRect> &circleGrid);
    void drawCircleGrid();
    void addPolygon(const QPolygon &polygon);
    void loadPolygons(const QVector<QPolygon> &polygons);
    void loadGridStartPoint(const QPoint &startPoint);
    void loadGridAngle(double angle);
    void rotate(double angle);
    void clearCircleGrid();
    void clearPolygons();
    void clearLastPolygon();
    void clearBackground();
    void setImage(QImage *image);
    void setScale(double scale);

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
    double maxColor;
    double scale;
};

#endif /* IMAGECANVAS_H_ */