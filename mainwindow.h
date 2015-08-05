#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

#include "scene.h"
#include "animationcontroller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QActionGroup *actionGroup;
    Scene scene;
    AnimationController *animationController;
    QString fileName;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeSceneMode(QAction *action);
    void setChangesMode(int value);
    void addFrame();
    void deleteFrame();
    void setCurrentFrame(int frameIndex);
    void saveFile(bool saveAnyway = false);
    void saveFileAs();
    void openFile();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
