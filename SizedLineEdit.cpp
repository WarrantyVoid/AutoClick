#include "SizedLineEdit.hpp"


SizedLineEdit::SizedLineEdit(QWidget *parent)
: QLineEdit(parent)
{
}


QSize SizedLineEdit::sizeHint() const
{
    return minimumSize();
}
