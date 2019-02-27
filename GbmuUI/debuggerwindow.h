#ifndef DEBUGGERWINDOW_H
#define DEBUGGERWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include <QListWidget>

#define MAIN_REGISTERS_BEGIN 0
#define MAIN_REGISTERS_END 5
#define VIDEO_REGISTERS_BEGIN 6
#define VIDEO_REGISTERS_END 21 
#define OTHER_REGISTERS_BEGIN 22
#define OTHER_REGISTERS_END 38

namespace Ui {
class DebuggerWindow;
}

class DebuggerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebuggerWindow(QWidget *parent = nullptr);
    ~DebuggerWindow();
	void refresh_info();
	void refresh_registers();
	void refresh_instr();
	void addBreakpoint();
	bool duplicateInListWidgetItem(const QString &value, const QListWidget *list);

private slots:
    void on_stepButton_clicked();
    void on_runOneFrameButton_clicked();
    void on_runDurationButton_clicked();
	void on_runDurationSpinBox_valueChanged(int arg1);
	void on_addBreakpointButton_clicked();
	void on_breakpointsEdit_editingFinished();
	void on_deleteBreakpointButton_clicked();

private:
    Ui::DebuggerWindow *ui;
};

#endif // DEBUGGERWINDOW_H
