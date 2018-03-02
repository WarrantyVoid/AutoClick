#include "LimitedKeySequence.hpp"
#include <QLineEdit>


LimitedKeySequence::LimitedKeySequence(QWidget *parent)
    : QKeySequenceEdit(parent)
{
    // Workaround: Make QKeySequenceEdit scale correctly in vertical direction
    QLineEdit *keyEdit = dynamic_cast<QLineEdit*>(children().front());
    keyEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}


void LimitedKeySequence::keyPressEvent(QKeyEvent *event)
{
    QKeySequenceEdit::keyPressEvent(event);

    QKeySequence seq(QKeySequence::fromString(keySequence().toString().split(", ").first()));
    setKeySequence(seq);
}

