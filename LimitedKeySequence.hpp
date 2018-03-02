#ifndef LIMITEDKEYSEQUENCE_H
#define LIMITEDKEYSEQUENCE_H


#include <QKeySequenceEdit>


class LimitedKeySequence : public QKeySequenceEdit
{
public:
    explicit LimitedKeySequence(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // LIMITEDKEYSEQUENCE_H
