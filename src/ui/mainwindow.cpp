#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gbmuscreen.h"
#include "debuggerwindow.h"
#include "worker.h"

#include <QFileDialog>
#include <QUrl>
#include <QMessageBox>
#include <QTimer>
#include <QKeyEvent>
#include <QThread>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QBoxLayout>
#include <QPoint>

Gameboy *g_gameboy = nullptr;
QMutex mutexGb;

#include <iostream>
#include <unistd.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//Load icon
	_pause_icon = QIcon(":/resources/pause.png");
	_play_icon = QIcon(":/resources/play.png");
	_sound_icon = QIcon(":/resources/sound.png");
	_mute_icon = QIcon(":/resources/mute.png");

	//Shortcut settings
	ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	ui->actionPlay->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	ui->actionStop->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
	ui->actionMute->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	ui->actionDebug->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	ui->actionSpeed->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
	_rom_path = QFileDialog::getOpenFileName(this, tr("Open ROM"), "/Users/oyagci/Workspace/42/gbmu/tools/blargg-test-roms", tr("*.gb *.gbc"));
	if (_rom_path.isEmpty())
		return ;
	if (g_gameboy && _gameboy_thread && _gameboy_worker && _timer_screen && _gameboy_screen)
	{
		g_gameboy->set_is_abort(true);
		_gameboy_thread->quit();
		_gameboy_thread->wait();
		_gameboy_worker->deleteLater();
		_gameboy_thread->deleteLater();
		_timer_screen->deleteLater();
		delete _gameboy_screen;
		delete g_gameboy;
		g_gameboy = nullptr;
	}
	g_gameboy = new Gameboy(_rom_path.toUtf8().constData());

	//Setting up gameboy's screen
    _gameboy_screen = new GbmuScreen(this);
    _timer_screen = new QTimer(this);
    connect(_timer_screen, &QTimer::timeout, _gameboy_screen, &GbmuScreen::updateGbScreen);
    _timer_screen->start(17);

	//Setting Controller
	_gameboy_thread = new QThread;
	_gameboy_worker = new Worker();
	_gameboy_worker->moveToThread(_gameboy_thread);
	connect(_gameboy_thread, SIGNAL (started()), _gameboy_worker, SLOT (process()));
	connect(_gameboy_worker, SIGNAL (finished()), _gameboy_thread, SLOT (quit()));
	_gameboy_thread->start();
	setCentralWidget(_gameboy_screen);

	//enable toolsbar action
	ui->actionSave->setEnabled(true);
	ui->actionPlay->setEnabled(true);
	ui->actionStop->setEnabled(true);
	ui->actionMute->setEnabled(true);
	ui->actionDebug->setEnabled(true);
	ui->actionSpeed->setEnabled(true);
}

void MainWindow::on_actionSave_triggered()
{
	if (!g_gameboy)
		return;
    QString save_path = _rom_path + ".save";
	g_gameboy->save(save_path.toUtf8().constData());
    QMessageBox::information(this, "Save", "Game Saved");
}

void MainWindow::pause_gameboy(bool stop)
{
	if (g_gameboy == nullptr)
		return;
	if (!g_gameboy->get_pause() || stop)
	{
		ui->actionPlay->setIcon(_play_icon);
		g_gameboy->set_pause(true);
	}
	else
	{
		ui->actionPlay->setIcon(_pause_icon);
		g_gameboy->set_pause(false);
	}
}

void MainWindow::on_actionPlay_triggered()
{
	pause_gameboy();
}

void MainWindow::on_actionStop_triggered()
{
	if (g_gameboy)
	{
		g_gameboy->reset();
		pause_gameboy(true);
	}
}

void MainWindow::on_actionMute_triggered()
{
	_is_muted = !_is_muted;
	if (_is_muted)
		ui->actionMute->setIcon(_mute_icon);
	else
		ui->actionMute->setIcon(_sound_icon);
    g_gameboy->mute_gameboy();
}

void MainWindow::on_actionDebug_triggered()
{
	if (g_gameboy)
	{
		g_gameboy->get_debugger().toggle();
		DebuggerWindow *debuggerwindow = new DebuggerWindow(this);
		debuggerwindow->setModal(false);
		debuggerwindow->show();
	}
}

void MainWindow::on_actionSpeed_triggered()
{
	pause_gameboy(true);
	bool ok;
	int speed = QInputDialog::getInt(this, tr("Set speed"), tr("Value"), 1, 1, 10, 1, &ok);
	if (ok && g_gameboy)
	{
		g_gameboy->components().core->up_cpu_freq(speed);
	}
	pause_gameboy(false);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (!g_gameboy)
		return;
	switch (event->key())
	{
		case Qt::Key_W:
			g_gameboy->key_pressed_wraper(2);
			break;
		case Qt::Key_S:
			g_gameboy->key_pressed_wraper(3);
			break;
		case Qt::Key_A:
			g_gameboy->key_pressed_wraper(1);
			break;
		case Qt::Key_D:
			g_gameboy->key_pressed_wraper(0);
			break;
		case Qt::Key_J:
			g_gameboy->key_pressed_wraper(4);
			break;
		case Qt::Key_K:
			g_gameboy->key_pressed_wraper(5);
			break;
		case Qt::Key_N:
			g_gameboy->key_pressed_wraper(7);
			break;
		case Qt::Key_M:
			g_gameboy->key_pressed_wraper(6);
			break;
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (!g_gameboy)
		return ;
	switch (event->key())
	{
		case Qt::Key_W:
			g_gameboy->key_released_wraper(2);
			break;
		case Qt::Key_S:
			g_gameboy->key_released_wraper(3);
			break;
		case Qt::Key_A:
			g_gameboy->key_released_wraper(1);
			break;
		case Qt::Key_D:
			g_gameboy->key_released_wraper(0);
			break;
		case Qt::Key_J:
			g_gameboy->key_released_wraper(4);
			break;
		case Qt::Key_K:
			g_gameboy->key_released_wraper(5);
			break;
		case Qt::Key_N:
			g_gameboy->key_released_wraper(7);
			break;
		case Qt::Key_M:
			g_gameboy->key_released_wraper(6);
			break;
	}
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	if (_gameboy_screen && event->size().width() > GB_WIDTH && event->size().height() > GB_HEIGTH)
	{
		_gameboy_screen->do_resize();
	}
}
