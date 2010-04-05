/*
 * MyCanvas.cpp
 *
 *  Created on: Feb 26, 2010
 *      Author: Vincent
 */

#include "MyCanvas.h"
#include <QtGui>

#define DEFAULT_EYE_X -10.0
#define DEFAULT_EYE_Y -10.0
#define DEFAULT_EYE_Z 40.0
#define DEFAULT_CENTER_X 100.0
#define DEFAULT_CENTER_Y 100.0
#define DEFAULT_CENTER_Z .0
#define DEFAULT_SENSITIVIY .2
#define DEFAULT_IMAGE_SCALE .1

MyCanvas::MyCanvas(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer | QGL::Rgba
            | QGL::DepthBuffer), parent) {
    eyeX = DEFAULT_EYE_X;
    eyeY = DEFAULT_EYE_Y;
    eyeZ = DEFAULT_EYE_Z;
    centerX = DEFAULT_CENTER_X;
    centerY = DEFAULT_CENTER_Y;
    centerZ = DEFAULT_CENTER_Z;
    sensitivity = DEFAULT_SENSITIVIY;
    imageScale = DEFAULT_IMAGE_SCALE;
    //	rotBase = DEFAULT_ROT_BASE;
    image = 0;
    maxColor = 0;
    isMasked = false;
    showAverage = false;
    showImage = true;
    lookZ = false;
    ortho = false;
}

MyCanvas::~MyCanvas() {
}

void MyCanvas::renderGrayQImage(const QImage &image) {
    this->image = &image;
    maxColor = (int) pow(2.0, (double) image.depth()) - 1;
    mask = QBitmap(image.width(), image.height());
    mask.clear();
    lastAverages.clear();
    isMasked = false;
    updateGL();
}

void MyCanvas::setSensitivity(double sen) {
    if (sensitivity != sen) {
        sensitivity = sen;
        emit sensitivityChanged(sen);
        updateGL();
    }
}
void MyCanvas::setImageScale(double scale) {
    if (imageScale != scale) {
        imageScale = scale;
        emit imageScaleChanged(scale);
        updateGL();
    }
}

QVector<double> MyCanvas::calculateAverages(const QVector<QRect> &rects,
        const QPoint &offset) {
    QVector<double> results;
    lastAverages.clear();
    lastCounts.clear();
    lastRects.clear();
    for (QVector<QRect>::const_iterator itr = rects.begin(); itr != rects.end(); ++itr) {
        results << calculateAverage(*itr, offset);
    }
    return results;
}

QVector<QVector<double> > MyCanvas::calculateAverages(
        const QVector<QRect> &rects, int col, const QPoint &offset) {
    QVector<QVector<double> > results;
    lastAverages.clear();
    lastCounts.clear();
    lastRects.clear();
    lastCol = col;
    QVector<double> row;
    int rowCount = 0;
    for (QVector<QRect>::const_iterator itr = rects.begin(); itr != rects.end(); ++itr) {
        ++rowCount;
        row.push_back(calculateAverage(*itr, offset));
        if (rowCount == col) {
            results.push_back(row);
            row.clear();
            rowCount = 0;
        }
    }
    return results;
}

double MyCanvas::calculateAverage(QRect rect, const QPoint &offset) {
    QImage maskImage = mask.toImage();
    QRgb color1Rgb = QColor(Qt::color1).rgb();
    rect.translate(offset);
    int startX = rect.x();
    int startY = rect.y();
    int endX = startX + rect.width();
    int endY = startY + rect.height();
    double average = 0;
    double sum = 0;
    int count = 0;
    for (int i = startX; i < endX; ++i) {
        for (int j = startY; j < endY; ++j) {
            if (isMasked) {
                if (maskImage.pixel(i, j) != color1Rgb) {
                    continue;
                }
            }
            sum += qGray(image->pixel(i, j));
            ++count;
        }
    }
    if (count > 0) {
        average = sum / count;
        lastAverages << average;
        lastCounts << count;
        lastRects << rect;
    }
    return average;
}

void MyCanvas::drawAverages(bool shown) {
    showAverage = shown;
    updateGL();
}

void MyCanvas::drawImage(bool shown) {
    showImage = shown;
    updateGL();
}

void MyCanvas::drawAverages() {
    //	qDebug() << tr("SHOWING AVERAGES STARTING (%1,%2,%3)").arg(
    //			lastCalculation.front()[0] * imageScale).arg(
    //			lastCalculation.front()[1] * imageScale).arg(
    //			lastCalculation.front()[2] * imageScale);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin( GL_QUADS);
    int h = image->height();
    QVector<double>::const_iterator itr = lastAverages.begin();
    QVector<QRect>::const_iterator itrR = lastRects.begin();
    //    GLboolean lightEnabled;
    //    GLboolean light0Enabled;
    //    glGetBooleanv(GL_LIGHTING, &lightEnabled);
    //    glGetBooleanv(GL_LIGHT0, &light0Enabled);
    GLfloat n[6][3] = { { 0.0, 0.0, 1.0 }, { 0.0, 0.0, -1.0 },
            { 1.0, 0.0, 0.0 }, { 0.0, -1.0, 0.0 }, { -1.0, 0.0, 0.0 }, { 0.0,
                    1.0, 0.0 } };
    for (; itr != lastAverages.end() && itrR != lastRects.end(); ++itr, ++itrR) {
        GLfloat xy[4][2] = { { itrR->x() * imageScale, (h + 1 - itrR->y())
                * imageScale }, { (itrR->x() + itrR->width()) * imageScale, (h
                + 1 - itrR->y()) * imageScale }, { (itrR->x() + itrR->width())
                * imageScale, (h + 1 - (itrR->y() + itrR->height()))
                * imageScale }, { itrR->x() * imageScale, (h + 1 - (itrR->y()
                + itrR->height())) * imageScale } };
        GLfloat points[8][3] = { { xy[0][0], xy[0][1], (*itr) * imageScale }, {
                xy[1][0], xy[1][1], (*itr) * imageScale }, { xy[2][0],
                xy[2][1], (*itr) * imageScale }, { xy[3][0], xy[3][1], (*itr)
                * imageScale }, { xy[0][0], xy[0][1], 0 }, { xy[1][0],
                xy[1][1], 0 }, { xy[2][0], xy[2][1], 0 }, { xy[3][0], xy[3][1],
                0 } };
        GLfloat col[4] = { 0, 0, 0, 1 };
        if ((*itr) == 0 || (*itr) == maxColor) {
            col[0] = .7f;
        } else {
            col[0] = (GLfloat)(*itr) / maxColor;
            col[1] = .7f;
            col[2] = (maxColor - (GLfloat)(*itr)) / maxColor;
        }
        glMaterialfv(GL_FRONT, GL_AMBIENT, col);
        //        glMaterialfv(GL_FRONT, GL_SPECULAR, col);
        // Top
        glNormal3fv(n[0]);
        glVertex3fv(points[0]);
        glVertex3fv(points[1]);
        glVertex3fv(points[2]);
        glVertex3fv(points[3]);
        // Bottom
        glNormal3fv(n[1]);
        glVertex3fv(points[4]);
        glVertex3fv(points[7]);
        glVertex3fv(points[6]);
        glVertex3fv(points[5]);
        // Side 1
        glNormal3fv(n[2]);
        glVertex3fv(points[2]);
        glVertex3fv(points[1]);
        glVertex3fv(points[5]);
        glVertex3fv(points[6]);
        // Side 2
        glNormal3fv(n[3]);
        glVertex3fv(points[3]);
        glVertex3fv(points[2]);
        glVertex3fv(points[6]);
        glVertex3fv(points[7]);
        // Side 3
        glNormal3fv(n[4]);
        glVertex3fv(points[0]);
        glVertex3fv(points[3]);
        glVertex3fv(points[7]);
        glVertex3fv(points[4]);
        // Side 4
        glNormal3fv(n[5]);
        glVertex3fv(points[1]);
        glVertex3fv(points[0]);
        glVertex3fv(points[4]);
        glVertex3fv(points[5]);
    }
    glEnd();
    //    if (!lightEnabled) {
    //        glDisable(GL_LIGHTING);
    //    }
    //    if (!light0Enabled) {
    //        glDisable(GL_LIGHT0);
    //    }
}

void MyCanvas::lookDownZ(bool lookZ) {
    this->lookZ = lookZ;
    updateGL();
}

void MyCanvas::setOrthoView(bool ortho) {
    this->ortho = ortho;
    resizeGL(width(), height());
    updateGL();
}

void MyCanvas::initializeGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    //	glEnable( GL_LIGHTING);
    //	glEnable( GL_LIGHT0);
#ifdef Q_WS_MAC
    glEnable( GL_MULTISAMPLE);
#endif
    GLfloat lightPos[4] = { eyeX, eyeY, eyeZ, 0 };
    GLfloat lightDiffuse[4] = { 1, 1, 1, 1 };
    GLfloat black[4] = { 0, 0, 0, 1 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glMaterialfv(GL_FRONT, GL_AMBIENT, black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glEnable( GL_LIGHT0);
    glEnable( GL_LIGHTING);
    //    glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
    //    glEnable ( GL_COLOR_MATERIAL);
    //    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //    glColorMaterial(GL_FRONT, GL_DIFFUSE);
}

void MyCanvas::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //	glTranslatef(xPan, yPan, zPan);
    //
    //	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    //	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    //	glRotatef(zRot, 0.0f, 0.0f, 1.0f);
    if (lookZ) {
        gluLookAt(eyeX, eyeY, eyeZ, eyeX, eyeY, eyeZ - 10.0, .0, 1.0, .0);
    } else {
        gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, .0, .0, 1.0);
    }

    glDisable( GL_LIGHTING);
    // Draw the Axis
    glBegin( GL_LINES);
    // X
    glColor3f(.3f, .0f, .0f);
    glVertex3f(-10000.0f, .0f, .0f);
    //	glColor3f(.5f, .5f, .0f);
    glVertex3f(10000.0f, .0f, .0f);
    glColor3f(.2f, .2f, .2f);
    for (int i = 20; i < 500; i += 20) {
        glVertex3f(-500.0f, i, .0f);
        glVertex3f(500.0f, i, .0f);
        glVertex3f(-500.0f, -i, .0f);
        glVertex3f(500.0f, -i, .0f);
    }
    // Y
    glColor3f(.0f, .3f, .0f);
    glVertex3f(.0f, -10000.0f, .0f);
    //	glColor3f(.7f, .7f, .0f);
    glVertex3f(.0f, 10000.0f, .0f);
    glColor3f(.2f, .2f, .2f);
    for (int i = 20; i < 500; i += 20) {
        glVertex3f(i, -500.0f, .0f);
        glVertex3f(i, 500.0f, .0f);
        glVertex3f(-i, -500.0f, .0f);
        glVertex3f(-i, 500.0f, .0f);
    }
    // Z
    glColor3f(.0f, .0f, .3f);
    glVertex3f(.0f, .0f, -10000.0f);
    //	glColor3f(.9f, .9f, .0f);
    glVertex3f(.0f, .0f, 10000.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    if (showImage && image) {
        renderImage();
    }

    if (showAverage) {
        drawAverages();
    }

    /*
     glBegin( GL_TRIANGLES);
     glColor3f(.7f, .2f, .2f);
     glVertex2f(4.0f, .0f);
     glColor3f(.2f, .7f, .2f);
     glVertex2f(-4.0f, 4.0f);
     glColor3f(.2f, .2f, .7f);
     glVertex2f(-4.0f, -4.0f);
     glEnd();
     */
    swapBuffers();
}

void MyCanvas::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode( GL_PROJECTION); // Select The Projection Matrix
    glLoadIdentity(); // Reset The Projection Matrix

    if (ortho) {
        glOrtho(-width * imageScale, width * imageScale, -height * imageScale,
                height * imageScale, -10000.0, 10000.0);
    } else {
        // Calculate The Aspect Ratio Of The Window
        gluPerspective(45.0, (GLdouble) width / (GLdouble) height, 0.1, 10000.0);
    }

    glMatrixMode( GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity(); // Reset The Modelview Matrix
}

void MyCanvas::setEye(double x, double y, double z) {
    eyeX = x;
    eyeY = y;
    eyeZ = z;
    GLfloat lightPos[4] = { eyeX, eyeY, eyeZ, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void MyCanvas::setCenter(double x, double y, double z) {
    centerX = x;
    centerY = y;
    centerZ = z;
}

//void MyCanvas::setRotationBase(RotBase base) {
//	rotBase = base;
//}

void MyCanvas::resetAll() {
    setEye(DEFAULT_EYE_X, DEFAULT_EYE_Y, DEFAULT_EYE_Z);
    setCenter(DEFAULT_CENTER_X, DEFAULT_CENTER_Y, DEFAULT_CENTER_Z);
    setSensitivity(DEFAULT_SENSITIVIY);
    setImageScale(DEFAULT_IMAGE_SCALE);
    updateGL();
}

void MyCanvas::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        double localSense = sensitivity * .01;
        setCenter((centerX - eyeX) * cos(-dx * localSense) - (centerY - eyeY)
                * sin(-dx * localSense) + eyeX,
                (centerX - eyeX) * sin(-dx * localSense) + (centerY - eyeY)
                        * cos(-dx * localSense) + eyeY, centerZ - dy
                        * sensitivity);
    } else if (event->buttons() & Qt::RightButton) {
        //		if (event->modifiers() & Qt::ShiftModifier) {
        //			setEye(eyeX, eyeY, eyeZ + dy * sensitivity / 2.0);
        //			setCenter(centerX, centerY, centerZ + dy * sensitivity / 2.0);
        //		} else {
        double localSense = sensitivity * .5;
        double moveX;
        double moveY;
        if (lookZ) {
            moveX = -dx * localSense;
            moveY = dy * localSense;
        } else {
            double X = centerX - eyeX;
            double Y = centerY - eyeY;
            double longEdge = sqrt(X * X + Y * Y);
            moveX = (dy * X - dx * Y) / longEdge * localSense;
            moveY = (dy * Y + dx * X) / longEdge * localSense;
        }
        setEye(eyeX + moveX, eyeY + moveY, eyeZ);
        setCenter(centerX + moveX, centerY + moveY, centerZ);
        //		}
    }
    lastPos = event->pos();
    updateGL();
}

void MyCanvas::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
}

void MyCanvas::wheelEvent(QWheelEvent * event) {
    double dz = event->delta();
    setEye(eyeX, eyeY, eyeZ - dz * sensitivity / 5.0);
    updateGL();
}

void MyCanvas::renderImage() {
    int w = image->width();
    int h = image->height();
    QImage maskImage = mask.toImage();
    QRgb color1Rgb = QColor(Qt::color1).rgb();
    glDisable( GL_LIGHTING);
    glPointSize(1.0f);
    glBegin( GL_POINTS);
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            if (isMasked) {
                if (maskImage.pixel(i, j) != color1Rgb) {
                    continue;
                }
            }
            int gray = qGray(image->pixel(i, j));
            if (gray == 0 || gray == maxColor) {
                glColor3f(.7f, .0f, .0f);
            } else {
                glColor3f((GLfloat) gray / maxColor, .7f, (maxColor
                        - (GLfloat) gray) / maxColor);
            }
            glVertex3f(i * imageScale, (h + 1 - j) * imageScale, gray
                    * imageScale);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}
