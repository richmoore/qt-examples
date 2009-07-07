#include <QtCore/QTimeLine>
#include <QtGui/QLabel>
#include <QtCore/QVariant>

#include <animator.h>

#include <qdebug.h>

Animator::Animator( QWidget *parent )
    : QWidget( parent )
{
    qtimeline = new QTimeLine( 5000, this );
    connect( qtimeline, SIGNAL(valueChanged(qreal)), this, SLOT(onValueChanged(qreal)) );
    connect( qtimeline, SIGNAL(finished()), this, SLOT(loopAndReverse()), Qt::QueuedConnection );
}

Animator::~Animator()
{
}

QTimeLine *Animator::timeline()
{
    return qtimeline;
}

void Animator::addAnimation( QWidget *item, const QString &property, const QVariant &start, const QVariant &end )
{
    if ( anims.contains(item) ) {
	// Add the animation to the existing ones
    }
    else {
	qDebug() << "Adding animation";

	Animation a;
	a.append( AnimationItem( property, start, end ) );
	anims[item] = a;
    }
}

void Animator::start()
{
    qtimeline->start();
}

int Animator::interpolate( int start, int end, qreal amt )
{
    return (end - start) * amt + start;
}

double Animator::interpolate( double start, double end, qreal amt )
{
    return (end - start) * amt + start;
}

QPoint Animator::interpolate( const QPoint &start, const QPoint &end, qreal amt )
{
    int x = (end.x() - start.x()) * amt + start.x();
    int y = (end.y() - start.y()) * amt + start.y();

    return QPoint( x, y );
}

QRect Animator::interpolate( const QRect &start, const QRect &end, qreal amt )
{
    int x = (end.x() - start.x()) * amt + start.x();
    int y = (end.y() - start.y()) * amt + start.y();
    int w = (end.width() - start.width()) * amt + start.width();
    int h = (end.height() - start.height()) * amt + start.height();

    return QRect( x, y, w, h );
}

void Animator::animateOne( QWidget *item, const AnimationItem &anim, qreal value )
{
    QVariant::Type type = item->property( anim.property.toLatin1() ).type();

    if ( QVariant::Int == type )
	item->setProperty( anim.property.toLatin1(), interpolate( anim.start.toInt(), anim.end.toInt(), value ) );
    else if ( QVariant::Double == type )
	item->setProperty( anim.property.toLatin1(), interpolate( anim.start.toDouble(), anim.end.toDouble(), value ) );
    else if ( QVariant::Point == type )
	item->setProperty( anim.property.toLatin1(), interpolate( anim.start.toPoint(), anim.end.toPoint(), value ) );
    else if ( QVariant::Rect == type )
	item->setProperty( anim.property.toLatin1(), interpolate( anim.start.toRect(), anim.end.toRect(), value ) );
    else {
	qDebug() << "Attempt to animate property " << anim.property << " of unsupport type " << QVariant::typeToName(type);
    }
}

void Animator::onValueChanged( qreal value )
{
    QHashIterator<QWidget*, Animation> item(anims);
    while( item.hasNext() ) {
	item.next();

	QListIterator<AnimationItem> anim( item.value() );
	while( anim.hasNext() ) {
	    animateOne( item.key(), anim.next(), value );
	}
    }
}

void Animator::loopAndReverse()
{
    qtimeline->toggleDirection();
    qtimeline->start();
}
