#ifndef TRANSLUCENTBUTTON_H
#define TRANSLUCENTBUTTON_H

#include <QtGui>
#include <QString>

class TranslucentButton: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit TranslucentButton(const QString &name, QWidget *parent);
    void flash();
    qreal opacity() const {return mOpacity;}
    void setOpacity(qreal opacity);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QString name;
    qreal mOpacity;
};

#endif // TRANSLUCENTBUTTON_H
