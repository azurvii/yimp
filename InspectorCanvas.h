/*
 * InspectorCanvas.h
 *
 *  Created on: Jun 2, 2010
 *      Author: Vincent
 */

#ifndef INSPECTORCANVAS_H_
#define INSPECTORCANVAS_H_

#include <QGLWidget>
#include <QBitmap>
#include "Matrix.h"
#include "Processor.h"

class InspectorCanvas: public QGLWidget {
Q_OBJECT
public:
	InspectorCanvas(QWidget * parent = 0);
	virtual ~InspectorCanvas();
	double getSensitivity() const;
	double getImageScale() const;
	bool isImageShown() const;

public slots:
	void setEye(double x, double y, double z);
	void setFocus(double x, double y, double z);
	void resetView();
	void setSensitivity(double sensitivity);
	void setUnitScale(double scale);
	void setLookDownZ(bool lookZ);
	void setOrthoView(bool ortho);
	void setMatrix(const RealMatrix * matrix);
	void setMaxColor(int color);
	void setMinColor(int color);
	void setUpperBorder(int color);
	void setLowerBorder(int color);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);

	signals:
	void sensitivityChanged(double sensitivity);
	void unitScaleChanged(double scale);

private:
	QPoint lastPos;
	double eyeX;
	double eyeY;
	double eyeZ;
	double focusX;
	double focusY;
	double focusZ;
	double sensitivity;
	double imageScale;
	bool imageShown;
	bool lookZ;
	bool ortho;
	const RealMatrix * renderingMatrix;
	double guideLimit;
	double axisLimit;
	double tickInterval;
	GLfloat axisXColor[3];
	GLfloat axisYColor[3];
	GLfloat axisZColor[3];
	int colorMax;
	int colorMin;
	int borderUpper;
	int borderLower;

private:
	void renderImage();
};

#endif /* INSPECTORCANVAS_H_ */
