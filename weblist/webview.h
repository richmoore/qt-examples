#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#include <qgraphicslistview.h>

class WebViewItem : public QtGraphicsListViewItem
{
    Q_OBJECT
    
public:
    WebViewItem( int index, QtGraphicsListView *view );

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
    void updateItem();

private:
    QWebPage *page;
};

class WebView : public QtGraphicsListView
{
public:
    WebView(Qt::Orientation orientation=Qt::Vertical, QGraphicsWidget *parent=0, Qt::WindowFlags wFlags=0);
    ~WebView();

    void setEnablePreviews( bool enable );

    QSizeF itemSize(const QStyleOptionViewItemV4 *option, int index) const;

protected:
    QtGraphicsListViewItem *newViewItem(int index);

private:
    bool mEnablePreviews;
};

#endif // WEBVIEW_H

