#include <QtQuick>
#include <QDBusConnection>
#include <sailfishapp.h>

#include <QtDebug>

int main(int argc, char **argv) {

    QGuiApplication* app = SailfishApp::application(argc, argv);
    bool prestart = app->arguments().contains(QStringLiteral("--prestart"));

    QQuickView* view = SailfishApp::createView();
    view->setSource(SailfishApp::pathToMainQml());

    app->setOrganizationName(QStringLiteral("org.freedesktop.portal.desktop.amber"));
    app->setApplicationName(QStringLiteral("ui"));
    //app->setApplicationVersion(QStringLiteral(APP_VERSION));

    if (!QDBusConnection::sessionBus().registerObject("/org/freedesktop/impl/portal/desktop/amber/ui", view))
        qWarning() << "Could not register D-Bus object.";

    if (!QDBusConnection::sessionBus().registerService("org.freedesktop.impl.portal.desktop.amber.ui"))
        qWarning() << "Could not register D-Bus service.";

    if(!prestart)
    {
        view->show();
    } else {
        qDebug() << "Prestart, not showing UI yet.";
    }
    return app->exec();
}
