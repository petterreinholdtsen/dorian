#include <QtGui>

#include "splash.h"
#include "platform.h"
#include "settings.h"
#include "trace.h"

Splash::Splash(): QSplashScreen()
{
    TRACE;

    Platform *platform = Platform::instance();
#if defined(Q_OS_SYMBIAN)
    QRect geo = QApplication::desktop()->availableGeometry();
    if (geo.width() < geo.height()) {
        setPixmap(QPixmap(platform->icon("splash", ".jpg")).scaled(360, 518,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        setPixmap(QPixmap(platform->icon("splash-landscape", ".jpg")).
            scaled(520, 330,
                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
#elif defined(Q_WS_MAEMO_5)
    // Force landscape: QSplash seems to be broken in portrait
    platform->setOrientation(this, "landscape");
    setPixmap(QPixmap(platform->icon("splash-landscape", ".jpg")));
#else
    Q_UNUSED(platform);
#endif
}
