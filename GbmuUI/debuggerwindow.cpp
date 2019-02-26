#include "debuggerwindow.h"
#include "ui_debuggerwindow.h"

#include "mainwindow.h"
#include "src/Debugger.hpp"

DebuggerWindow::DebuggerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggerWindow)
{
    ui->setupUi(this);

	//All Registers in one vector
	std::vector<uint16_t> registers = g_gameboy.get_debugger().construct_register_pool();

	//Main Registers
	ui->registersWidget->setColumnCount(1);
	QStringList titles;
	titles << "Value";
	ui->registersWidget->setHorizontalHeaderLabels(titles);

	ui->registersWidget->setRowCount(6);
	QStringList verticalLabels;
	verticalLabels << "PC" << "AF" << "BC" << "DE" << "HL" << "SP";
	ui->registersWidget->setVerticalHeaderLabels(verticalLabels);
	ui->registersWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->registersWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	for (int i = 0; i < 6; i++){
		ui->registersWidget->setItem(i, 0, new QTableWidgetItem(QString::number(registers[i], 16)));
	}

	//Video Registers
	ui->videoRegistersWidget->setColumnCount(2);
	titles.clear();
	titles << "Addr" << "Value";
	ui->videoRegistersWidget->setHorizontalHeaderLabels(titles);

	ui->videoRegistersWidget->setRowCount(16);
	verticalLabels.clear();
	verticalLabels << "LCDC" << "STAT" << "SCY" << "SCX" << "LY" << "LYC" << "DMA" << "BGP" << "OBP0" << "OBP1" << "WY" << "WX" << "BCPS" << "BCPD" << "OCPS" << "OCPD";
	ui->videoRegistersWidget->setVerticalHeaderLabels(verticalLabels);
	ui->videoRegistersWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->videoRegistersWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QStringList addrList;
	addrList << "FF40" << "FF41" << "FF42" << "FF43" << "FF44" << "FF45" << "FF46" << "FF47" << "FF48" << "FF49" << "FF4A" << "FF4B" << "FF68" << "FF69" << "FF6A" << "FF6B";
	for (int i = 0; i < 16; i++){
		ui->videoRegistersWidget->setItem(i, 0, new QTableWidgetItem(addrList.at(i)));
		ui->videoRegistersWidget->setItem(i, 1, new QTableWidgetItem(QString::number(registers[i + 6], 16)));
	}

	//Other Registers
	ui->otherRegistersWidget->setColumnCount(2);
	titles.clear();
	titles << "Addr" << "Value";
	ui->otherRegistersWidget->setHorizontalHeaderLabels(titles);

	ui->otherRegistersWidget->setRowCount(17);
	verticalLabels.clear();
	verticalLabels << "P1" << "SB" << "SC" << "DIV" << "TIMA" << "TMA" << "TAC" << "KEY1" << "VBK" << "HDMA1" << "HDMA2" << "HDMA3" << "HDMA4" << "HDMA5" << "SVBK" << "IF" << "IE";
	ui->otherRegistersWidget->setVerticalHeaderLabels(verticalLabels);
	ui->otherRegistersWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->otherRegistersWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	addrList.clear();
	addrList << "FF00" << "FF01" << "FF02" << "FF04" << "FF05" << "FF06" << "FF07" << "FF4D" << "FF4F" << "FF51" << "FF52" << "FF53" << "FF54" << "FF55" << "FF70" << "FF0F" << "FFFF";
	for (int i = 0; i < 17; i++){
		ui->otherRegistersWidget->setItem(i, 0, new QTableWidgetItem(addrList.at(i)));
		ui->otherRegistersWidget->setItem(i, 1, new QTableWidgetItem(QString::number(registers[i + 6 + 16], 16)));
	}
	
	//Instructions pool size
	ui->disassemblerWidget->setColumnCount(3);
	titles.clear();
	titles << "Address" << "Instruction" << "Data";
	ui->disassemblerWidget->setHorizontalHeaderLabels(titles);

	ui->disassemblerWidget->setRowCount(6);
	ui->disassemblerWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->disassemblerWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	refresh_instr();
}

DebuggerWindow::~DebuggerWindow()
{
    delete ui;
}

void DebuggerWindow::refresh_instr()
{
	g_gameboy.get_debugger().set_instruction_pool_size(6);
	std::vector<Debugger::_debug_info> instr_pool = g_gameboy.get_debugger().get_instruction_pool();
	QString value;
	for (int i = 0; i < 6; i++){
		ui->disassemblerWidget->setItem(i, 0, new QTableWidgetItem(QString::number(instr_pool[i].pc, 16)));
		ui->disassemblerWidget->setItem(i, 1, new QTableWidgetItem(instr_pool[i].instr));
		value = QString::number(instr_pool[i].value[0], 16) + " " + QString::number(instr_pool[i].value[1], 16) + " " + QString::number(instr_pool[i].value[2], 16);
		ui->disassemblerWidget->setItem(i, 2, new QTableWidgetItem(value));
	}
}

void DebuggerWindow::refresh_registers()
{
	std::vector<std::pair<int, uint16_t>> registers = g_gameboy.get_debugger().get_register_diffs();
//	for (auto it = registers.begin(); it != registers.end();it++)
	for (auto value: registers) {
		if (value.first >= MAIN_REGISTERS_BEGIN && value.first <= MAIN_REGISTERS_END)
			ui->registersWidget->setItem(value.first, 0, new QTableWidgetItem(QString::number(value.second, 16)));
		else if (value.first >= VIDEO_REGISTERS_BEGIN && value.first <= VIDEO_REGISTERS_END)
			ui->videoRegistersWidget->setItem(value.first - VIDEO_REGISTERS_BEGIN, 1, new QTableWidgetItem(QString::number(value.second, 16)));
		else if (value.first >= OTHER_REGISTERS_BEGIN && value.first <= OTHER_REGISTERS_END)
			ui->otherRegistersWidget->setItem(value.first - OTHER_REGISTERS_BEGIN, 1, new QTableWidgetItem(QString::number(value.second, 16)));
	}
}

void DebuggerWindow::refresh_info()
{
	refresh_registers();
	refresh_instr();
}

#include <iostream>
void DebuggerWindow::on_stepButton_clicked()
{
	g_gameboy.notify_debugger(Debugger::RUN_ONE_STEP);
	while (g_gameboy.get_debugger().get_run_step()){}
	refresh_info();
}

void DebuggerWindow::on_runOneFrameButton_clicked()
{
	g_gameboy.notify_debugger(Debugger::RUN_ONE_FRAME);
	while (g_gameboy.get_debugger().get_run_frame()){}
	refresh_info();
}

void DebuggerWindow::on_runOneSecondButton_clicked()
{
	g_gameboy.notify_debugger(Debugger::RUN_ONE_SEC);
	while (g_gameboy.get_debugger().get_run_sec()){}
	refresh_info();
}