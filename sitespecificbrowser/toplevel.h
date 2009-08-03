#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <qwebview.h>
#include <qwebpage.h>

class Page : public QWebPage
{
    Q_OBJECT

public:
    Page();

protected:
    bool acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type );
};

class TopLevel : public QWebView
{
    Q_OBJECT

public:
    TopLevel();

protected slots:
    void iconLoaded();

private:
    Page *m_page;
};

#endif // TOPLEVEL_H
