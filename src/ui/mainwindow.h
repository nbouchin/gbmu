#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

#include "debuggerwindow.h"
#include "gbmuscreen.h"
#include "mainwindow.h"
#include "src/Gameboy.hpp"
#include "ui_mainwindow.h"
#include "worker.h"

class Worker;

namespace Ui {
class MainWindow;
}

extern Gameboy *g_gameboy;
extern QMutex mutexGb;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_actionOpen_triggered();

  void on_actionPlay_triggered();

  void on_actionStop_triggered();

  void on_actionMute_triggered();

  void on_actionDebug_triggered();

  void on_actionSave_triggered();

  void on_actionSpeed_triggered();

  void on_actionSnapshot_triggered();

  void on_actionLoad_Snapshot_triggered();

  void on_actionDefault_toggled(bool arg1);

  void on_actionDMG_toggled(bool arg1);

  void on_actionCGB_toggled(bool arg1);

 private:
  Ui::MainWindow *ui;
  QString _rom_path;
  QThread *_gameboy_thread = nullptr;
  Worker *_gameboy_worker = nullptr;
  QTimer *_timer_screen = nullptr;
  GbmuScreen *_gameboy_screen = nullptr;
  QIcon _pause_icon;
  QIcon _play_icon;
  QIcon _sound_icon;
  QIcon _mute_icon;
  bool _is_muted = false;

  // Key Event function
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
  void pause_gameboy(bool stop = false);
  void resizeEvent(QResizeEvent *event);
  GbType get_gb_type();
};

#endif  // MAINWINDOW_H
