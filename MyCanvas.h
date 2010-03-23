/*
 * MyCanvas.h
 *
 *  Created on: Feb 26, 2010
 *      Author: Vincent
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
    double getSensitivity() const {
        return sensitivity;
    }

    double getImageScale() const {
        return imageScale;
    }

    bool getShowAverage() const {
        return showAverage;
    }

    bool getShowImage() const {
        return showImage;
    }

    const QVector<double> &getLastAverages() const {
        return lastAverages;
    }

    QVector<QVector<double> > getLastAverages(bool) const {
        QVector<QVector<double> > results;
        QVector<double> row;
        for (QVector<double>::const_iterator itr = lastAverages.begin(); itr
                != lastAverages.end(); ++itr) {
            for (int i = 0; i < lastCol; ++i) {
                row.push_back(*itr);
            }
            results.push_back(row);
            row.clear();
        }
        return results;
    }

    QVector<QVector<int> > getLastCounts(bool) const {
        QVector<QVector<int> > results;
        QVector<int> row;
        int rowCount = 0;
        for (QVector<int>::const_iterator itr = lastCounts.begin(); itr
                != lastCounts.end(); ++itr) {
            row.push_back(*itr);
            ++rowCount;
            if (rowCount == lastCol) {
                results.push_back(row);
                row.clear();
                rowCount = 0;
            }
        }
        return results;
    }

    const QVector<int> &getLastCounts() const {
        return lastCounts;
    }

    const QVector<QRect> &getLastRects() const {
        return lastRects;
    }

    QVector<QVector<QRect> > getLastRects(bool) const {
        QVector<QVector<QRect> > results;
        QVector<QRect> row;
        for (QVector<QRect>::const_iterator itr = lastRects.begin(); itr
                != lastRects.end(); ++itr) {
            for (int i = 0; i < lastCol; ++i) {
                row.push_back(*itr);
            }
            results.push_back(row);
            row.clear();
        }
        return results;
    }

    int getLastCol() const {
        return lastCol;
    }

    bool hasResults() const {
        return lastAverages.size() > 0;
    }

    //	RotBase getRotationBase() const {
    //		return rotBase;
    //	}

public slots:
    void setEye(double x, double y, double z);
    void setCenter(double x, double y, double z);
    void resetAll();
    void renderGrayQImage(const QImage &image);
    void setSensitivity(double sen);
    void setImageScale(double scale);
    //	void setRotationBase(RotBase base);
    void setMask(const QBitmap &mask) {
        isMasked = true;
        this->mask = mask;
    }
    QVector<double> calculateAverages(const QVector<QRect> &rects,
            const QPoint &offset = QPoint(0, 0));
    QVector<QVector<double> > calculateAverages(const QVector<QRect> &rects, int col,
            const QPoint &offset = QPoint(0, 0));
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
};

#endif /* MYCANVAS_H_ */
