// -*- c++ -*-

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QtGui/QWidget>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QVariant>

class QTimeLine;
class QLabel;

class Animator : public QWidget
{
    Q_OBJECT
public:
    Animator( QWidget *parent=0 );
    ~Animator();

    QTimeLine *timeline();

    void addAnimation( QWidget *item, const QString &property, const QVariant &start, const QVariant &end );

public slots:
    void start();

protected:
    int interpolate( int start, int end, qreal amt );
    double interpolate( double start, double end, qreal amt );
    QPoint interpolate( const QPoint &start, const QPoint &end, qreal amt );
    QRect interpolate( const QRect &start, const QRect &end, qreal amt );

private slots:
    void onValueChanged( qreal value );
    void loopAndReverse();

private:
    QTimeLine *qtimeline;

    struct AnimationItem
    {
	AnimationItem( const QString &property, const QVariant &start, const QVariant &end ) {
	    this->property = property;
	    this->start = start;
	    this->end = end;
	}

	AnimationItem( const AnimationItem &a ) {
	    this->property = a.property;
	    this->start = a.start;
	    this->end = a.end;
	}
    
	AnimationItem &operator=(const AnimationItem &other) {
	    property = other.property;
	    start = other.start;
	    end = other.end;
	    return *this;
	}

	QString property;
	QVariant start;
	QVariant end;
    };

    typedef QList<AnimationItem> Animation;

    void animateOne( QWidget *item, const AnimationItem &anim, qreal value );

    QHash<QWidget*, Animation> anims;

    QLabel *hello;
    QLabel *world;
};

#endif // ANIMATOR_H
