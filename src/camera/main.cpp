#include <QApplication>
#include "camera.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(camera);

  QApplication app(argc, argv);
  Camera mainWin;
  mainWin.show();
  return app.exec();
}
