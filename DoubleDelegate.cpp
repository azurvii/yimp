/*
 * DoubleDelegate.cpp
 *
 *  Created on: Apr 28, 2010
 *      Author: Vincent
 */

#include "DoubleDelegate.h"
#include <limits.h>
#include <QLineEdit>

DoubleDelegate::~DoubleDelegate() {
    // TODO Auto-generated destructor stub
}

DoubleDelegate::DoubleDelegate(QObject *parent) :
    QItemDelegate(parent) {
}

QWidget *DoubleDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &, const QModelIndex &) const {
    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void DoubleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    QLineEdit *spinBox = static_cast<QLineEdit*> (editor);
    spinBox->setText(QString::number(value, 'g', 15));
}

void DoubleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const {
    QLineEdit *spinBox = static_cast<QLineEdit*> (editor);
    double value = spinBox->text().toDouble();
    model->setData(index, value, Qt::EditRole);
}

void DoubleDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &) const {
    editor->setGeometry(option.rect);
}
