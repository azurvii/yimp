/*
 * MyCanvas.h
 *
 *  Created on: Feb 26, 2010
 *      Author: Vincent
 *
 * Provide a space view portal
 *
 */

#ifndef MYCANVAS_H_
#define MYCANVAS_H_

#include <QGLWidget>
#include <QBitmap>

class MyCanvas: public QGLWidget {
Q_OBJECT
public:
    //	enum RotBase {
    //		ROT_X = 0, ROT_Y, ROT_Z, ROT_XY, ROT_XZ, ROT_YZ
    //	};

public:
    MyCanvas(QWidget *parent = 0);
    virtual ~MyCanvas();
    double getSensitivity() const;
    double getImageScale() const;
    bool getShowAverage() const;
    bool getShowImage() const;
    const QVector<double> &getLastAverages() const;
    QVector<QVector<double> > getLastAverages(bool) const;
    QVector<QVector<int> > getLastCounts(bool) const;
    const QVector<int> &getLastCounts() const;
    const QVector<QRect> &getLastRects() const;
    QVector<QVector<QRect> > getLastRects(bool) const;
    int getLastCol() const;
    bool hasResults() const;

public slots:
    void setEye(double x, double y, double z);
    void setCenter(double x, double y, double z);
    void resetAll();
    void renderGrayQImage(const QImage &image);
    void setSensitivity(double sen);
    void setImageScale(double scale);
    //	void setRotationBase(RotBase base);
    void setMask(const QBitmap &mask);
    void disableMask();
    QVector<double> calculateAverages(const QVector<QRect> &rects,
            const QPoint &offset = QPoint(0, 0));
    QVector<QVector<double> > calculateAverages(const QVector<QRect> &rects,
            int col, const QPoint &offset = QPoint(0, 0));
    double calculateAverage(QRect rect, const QPoint &offset = QPoint(0, 0));
    void drawAverages(bool shown);
    void drawImage(bool shown);
    void drawAverages();
    void lookDownZ(bool lookZ);
    void setOrthoView(bool ortho);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);

    signals:
    void sensitivityChanged(double sen);
    void imageScaleChanged(double scale);

private:
    void renderImage();

private:
    QPoint lastPos;
    //	GLfloat xRot;
    //	GLfloat yRot;
    //	GLfloat zRot;
    //	GLfloat xPan;
    //	GLfloat yPan;
    //	GLfloat zPan;
    double eyeX;
    double eyeY;
    double eyeZ;
    double centerX;
    double centerY;
    double centerZ;
    const QImage *image;
    double sensitivity;
    double imageScale;
    //	RotBase rotBase;
    QBitmap mask;
    bool isMasked;
    bool showAverage;
    bool showImage;
    QVector<double> lastAverages;
    QVector<int> lastCounts;
    QVector<QRect> lastRects;
    int lastCol;
    bool lookZ;
    bool ortho;
    double maxColor;
};

#endif /* MYCANVAS_H_ */
