#include <sailfishapp.h>

int main(int argc, char **argv) {


    QGuiApplication* app = SailfishApp::application(argc, argv);
    bool prestart = app->arguments().contains(QStringLiteral("--prestart"));

    QQuickView* view = SailfishApp::createView();
    view->setSource(SailfishApp::pathToMainQml());

    /*
    if (!QDBusConnection::sessionBus().registerObject("/org/freedesktop/impl/portal/desktop/amber/ui", view))
        qWarning() << "Could not register D-Bus object.";

    if (!QDBusConnection::sessionBus().registerService("org.freedesktop.impl.portal.desktop.amber.ui"))
        qWarning() << "Could not register D-Bus service.";

    */
    if(!prestart)
    {
        view->show();
    }
    return app->exec();
}
