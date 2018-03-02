#ifndef SIZEDLINEEDIT_HPP
#define SIZEDLINEEDIT_HPP

#include <QLineEdit>

class SizedLineEdit : public QLineEdit
{
public:
    explicit SizedLineEdit(QWidget *parent = 0);
    QSize sizeHint() const;
};

#endif // SIZEDLINEEDIT_HPP
