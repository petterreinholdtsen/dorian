#include <QtGui>

#if defined(Q_WS_MAEMO_5)
#   include <QtGui/QX11Info>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#   include <QAbstractKineticScroller>
#endif

#include "adopterwindow.h"
#include "trace.h"
#include "bookview.h"
#include "platform.h"
#include "settings.h"
#include "progress.h"
#include "translucentbutton.h"

AdopterWindow::AdopterWindow(QWidget *parent): MainBase(parent), bookView(0),
    grabbingVolumeKeys(false), progress(0), previousButton(0), nextButton(0)
{
    TRACE;

    // Monitor settings
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));

}

void AdopterWindow::takeBookView(BookView *view,
                                 Progress *prog,
                                 TranslucentButton *previous,
                                 TranslucentButton *next)
{
    TRACE;

    Q_ASSERT(view);
    Q_ASSERT(prog);
    Q_ASSERT(previous);
    Q_ASSERT(next);

    leaveBookView();

    bookView = view;
    bookView->setParent(this);
    centralWidget()->layout()->addWidget(bookView);
    bookView->show();

    progress = prog;
    previousButton = previous;
    nextButton = next;
    progress->setParent(this);
    previousButton->setParent(this);
    nextButton->setParent(this);

    // Handle page and/or volume keys
    connect(this, SIGNAL(pageUp()), this, SLOT(onPageUp()),
            Qt::QueuedConnection);
    connect(this, SIGNAL(pageDown()), this, SLOT(onPageDown()),
            Qt::QueuedConnection);
}

void AdopterWindow::leaveBookView()
{
    TRACE;
    if (bookView) {
        bookView->hide();
        centralWidget()->layout()->removeWidget(bookView);
    }
    bookView = 0;
    progress = 0;
    nextButton = 0;
    previousButton = 0;
    disconnect(this, SLOT(onPageUp()));
    disconnect(this, SLOT(onPageDown()));
}

bool AdopterWindow::hasBookView()
{
    return bookView != 0;
}

void AdopterWindow::grabVolumeKeys(bool grab)
{
    TRACE;
    grabbingVolumeKeys = grab;
#ifdef Q_WS_MAEMO_5
    doGrabVolumeKeys(grab);
#endif
}

#ifdef Q_WS_MAEMO_5

void AdopterWindow::doGrabVolumeKeys(bool grab)
{
    TRACE;
    if (!isVisible()) {
        qDebug() << "Not visible - skipping";
        return;
    }
    if (!winId()) {
        qDebug() << "Could not get window ID - skipping";
        return;
    }
    unsigned long val = grab? 1: 0;
    Atom atom =
            XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False);
    if (!atom) {
        qCritical() << "Unable to obtain _HILDON_ZOOM_KEY_ATOM";
        return;
    }
    XChangeProperty(QX11Info::display(),
        winId(),
        atom,
        XA_INTEGER,
        32,
        PropModeReplace,
        reinterpret_cast<unsigned char *>(&val),
        1);
    qDebug() << "Grabbed volume keys";
}

#endif // Q_WS_MAEMO_5

void AdopterWindow::showEvent(QShowEvent *e)
{
    Trace t("AdopterWindow::showEvent");

    MainBase::showEvent(e);
#if defined(Q_WS_MAEMO_5)
    doGrabVolumeKeys(grabbingVolumeKeys);
#endif
    placeDecorations();
}

void AdopterWindow::resizeEvent(QResizeEvent *event)
{
    Trace t("AdopterWindow::resizeEvent");
    MainBase::resizeEvent(event);
    placeDecorations();

#if defined(Q_WS_MAEMO_5)
    // Restore previous reading position
    if (!bookView) {
        return;
    }
    QTimer::singleShot(110, bookView, SLOT(restoreLastBookmark()));
#endif // Q_WS_MAEMO_5

#if defined(Q_OS_SYMBIAN)
    // Adjust reading position after orientation change
    if (!bookView) {
        return;
    }
    if (event->oldSize().width() == -1) {
        return;
    }
    bool oldPortrait = event->oldSize().width() < event->oldSize().height();
    bool portrait = event->size().width() < event->size().height();
    if (oldPortrait == portrait) {
        return;
    }
    QTimer::singleShot(990, bookView, SLOT(adjustPosition()));
#endif // Q_OS_SYMBIAN
}

void AdopterWindow::closeEvent(QCloseEvent *event)
{
    Trace t("AdopterWindow::closeEvent");
    if (bookView) {
        bookView->setLastBookmark();
    }
    MainBase::closeEvent(event);
}

void AdopterWindow::leaveEvent(QEvent *event)
{
    Trace t("AdopterWindow::leaveEvent");
    if (bookView) {
        bookView->setLastBookmark();
    }
    MainBase::leaveEvent(event);
}

void AdopterWindow::keyPressEvent(QKeyEvent *event)
{
    TRACE;
    switch (event->key()) {
    case Qt::Key_PageDown:
#ifdef Q_WS_MAEMO_5
    case Qt::Key_F7:
#endif
        emit pageDown();
        event->accept();
        break;
    case Qt::Key_PageUp:
#ifdef Q_WS_MAEMO_5
    case Qt::Key_F8:
#endif
        emit pageUp();
        event->accept();
        break;
    default:
        ;
    }
    MainBase::keyPressEvent(event);
}

void AdopterWindow::onSettingsChanged(const QString &key)
{
    if (key == "usevolumekeys") {
        bool grab = Settings::instance()->value(key, false).toBool();
        qDebug() << "AdopterWindow::onSettingsChanged: usevolumekeys" << grab;
        grabVolumeKeys(grab);
    }
}

void AdopterWindow::placeDecorations()
{
    Trace t("AdopterWindow::placeDecorations");

    if (!hasBookView()) {
        return;
    }

    int extraHeight = 0;

    QRect geo = bookView->geometry();
    qDebug() << "bookView:" << geo;

#ifdef Q_OS_SYMBIAN
    // Work around Symbian bug: If tool bar is hidden, increase bottom
    // decorator widgets' Y coordinates by the tool bar's height
    if (isToolBarHidden()) {
        extraHeight = toolBarHeight();
    }

    // Work around another Symbian bug: When returning from full screen mode
    // in landscape, the book view widget's height is miscalculated.
    // My apologies for this kludge
    if (geo.height() == 288) {
        qDebug() << "Adjusting bottom Y";
        extraHeight -= 288 - 223;
    }
#endif // Q_OS_SYMBIAN

    progress->setGeometry(geo.x(),
        geo.y() + geo.height() - progress->thickness() + extraHeight,
        geo.width(), progress->thickness());
    previousButton->setGeometry(geo.x(),
        geo.y() + geo.height() - TranslucentButton::pixels + extraHeight,
        TranslucentButton::pixels, TranslucentButton::pixels);
    nextButton->setGeometry(geo.x() + geo.width() - TranslucentButton::pixels,
        geo.y(), TranslucentButton::pixels, TranslucentButton::pixels);
    progress->flash();
    previousButton->flash();
    nextButton->flash();
    qDebug() << "progress:" << progress->geometry();
}

void AdopterWindow::onPageUp()
{
    if (bookView && grabbingVolumeKeys) {
        setEnabled(false);
        bookView->goPreviousPage();
        setEnabled(true);
    }
}

void AdopterWindow::onPageDown()
{
    if (bookView && grabbingVolumeKeys) {
        setEnabled(false);
        bookView->goNextPage();
        setEnabled(true);
    }
}
