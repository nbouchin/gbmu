#include "debuggerwindow.h"
#include "ui_debuggerwindow.h"

#include "mainwindow.h"
#include "src/Debugger.hpp"

#include <QCheckBox>
#include <QDebug>
#include <QInputDialog>

#include <unistd.h>
DebuggerWindow::DebuggerWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::DebuggerWindow) {
  ui->setupUi(this);

  // Shortcut settings
  ui->resetButton->setShortcut(QKeySequence(Qt::Key_R));
  ui->stepButton->setShortcut(QKeySequence(Qt::Key_S));
  ui->runOneFrameButton->setShortcut(QKeySequence(Qt::Key_F));
  ui->runDurationButton->setShortcut(QKeySequence(Qt::Key_D));

  // Main Registers
  ui->registersWidget->setShowGrid(false);
  ui->registersWidget->setColumnCount(1);
  QStringList titles;
  titles << "Value";
  ui->registersWidget->setHorizontalHeaderLabels(titles);

  ui->registersWidget->setRowCount(6);
  QStringList verticalLabels;
  verticalLabels << "PC"
                 << "AF"
                 << "BC"
                 << "DE"
                 << "HL"
                 << "SP";
  ui->registersWidget->setVerticalHeaderLabels(verticalLabels);
  ui->registersWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->registersWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  // Video Registers
  ui->videoRegistersWidget->setShowGrid(false);
  ui->videoRegistersWidget->setColumnCount(2);
  titles.clear();
  titles << "Addr"
         << "Value";
  ui->videoRegistersWidget->setHorizontalHeaderLabels(titles);

  ui->videoRegistersWidget->setRowCount(16);
  verticalLabels.clear();
  verticalLabels << "LCDC"
                 << "STAT"
                 << "SCY"
                 << "SCX"
                 << "LY"
                 << "LYC"
                 << "DMA"
                 << "BGP"
                 << "OBP0"
                 << "OBP1"
                 << "WY"
                 << "WX"
                 << "BCPS"
                 << "BCPD"
                 << "OCPS"
                 << "OCPD";
  ui->videoRegistersWidget->setVerticalHeaderLabels(verticalLabels);
  ui->videoRegistersWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->videoRegistersWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  // Other Registers
  ui->otherRegistersWidget->setShowGrid(false);
  ui->otherRegistersWidget->setColumnCount(2);
  titles.clear();
  titles << "Addr"
         << "Value";
  ui->otherRegistersWidget->setHorizontalHeaderLabels(titles);

  ui->otherRegistersWidget->setRowCount(17);
  verticalLabels.clear();
  verticalLabels << "P1"
                 << "SB"
                 << "SC"
                 << "DIV"
                 << "TIMA"
                 << "TMA"
                 << "TAC"
                 << "KEY1"
                 << "VBK"
                 << "HDMA1"
                 << "HDMA2"
                 << "HDMA3"
                 << "HDMA4"
                 << "HDMA5"
                 << "SVBK"
                 << "IF"
                 << "IE";
  ui->otherRegistersWidget->setVerticalHeaderLabels(verticalLabels);
  ui->otherRegistersWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->otherRegistersWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  init_registers_view();

  // Instructions pool size
  ui->disassemblerWidget->setShowGrid(false);
  ui->disassemblerWidget->setColumnCount(2);
  titles.clear();
  titles << "Instruction"
         << "Data";
  ui->disassemblerWidget->setHorizontalHeaderLabels(titles);

  ui->disassemblerWidget->setRowCount(6);
  ui->disassemblerWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->disassemblerWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  refresh_instr(true);

  // Memory Map
  ui->memoryWidget->setShowGrid(false);
  ui->memoryWidget->setColumnCount(16);
  ui->memoryWidget->setRowCount(10);
  titles.clear();
  titles << "x0"
         << "x1"
         << "x2"
         << "x3"
         << "x4"
         << "x5"
         << "x6"
         << "x7"
         << "x8"
         << "x9"
         << "xA"
         << "xB"
         << "xC"
         << "xD"
         << "xE"
         << "xF";
  ui->memoryWidget->setHorizontalHeaderLabels(titles);
  ui->memoryWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->memoryWidget->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  _addr_memory_map = 0x00;
  refresh_memory_map(true);

  // Breakpoints
  ui->breakpointsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->breakpointsEdit->setMaxLength(4);

  // Watchpoints
  ui->watchpointsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->memoryLineEdit->setMaxLength(4);
  ui->watchpointsAddrEdit->setMaxLength(4);
  ui->watchpointsValueEdit->setMaxLength(4);
  connect(ui->registersWidget->verticalHeader(), &QHeaderView::sectionClicked,
          [this](int logicalIndex) {
            QString label =
                ui->registersWidget->verticalHeaderItem(logicalIndex)->text();
            add_watchpoint(label, QString::number(logicalIndex));
          });
  connect(
      ui->videoRegistersWidget->verticalHeader(), &QHeaderView::sectionClicked,
      [this](int logicalIndex) {
        QString label =
            ui->videoRegistersWidget->verticalHeaderItem(logicalIndex)->text();
        QString addrText =
            ui->videoRegistersWidget->item(logicalIndex, 0)->text();
        add_watchpoint(label, addrText);
      });
  connect(
      ui->otherRegistersWidget->verticalHeader(), &QHeaderView::sectionClicked,
      [this](int logicalIndex) {
        QString label =
            ui->otherRegistersWidget->verticalHeaderItem(logicalIndex)->text();
        QString addrText =
            ui->otherRegistersWidget->item(logicalIndex, 0)->text();
        add_watchpoint(label, addrText);
      });

  // Run Duration Spin Box Min-Max
  ui->runDurationSpinBox->setMinimum(1);
  ui->runDurationSpinBox->setMaximum(10);
}

DebuggerWindow::~DebuggerWindow() { delete ui; }

inline QString DebuggerWindow::qstring_hex_pad(uint32_t value, int padding) {
  return QString("%1").arg(value, padding, 16, QChar('0')).toUpper();
}

void DebuggerWindow::init_registers_view() {
  std::vector<uint16_t> registers =
      g_gameboy->get_debugger().construct_register_pool();
  for (int i = 0; i < 6; i++) {
    ui->registersWidget->setItem(
        i, 0, new QTableWidgetItem(qstring_hex_pad(registers[i], 4)));
  }

  QStringList addrList;
  addrList << "FF40"
           << "FF41"
           << "FF42"
           << "FF43"
           << "FF44"
           << "FF45"
           << "FF46"
           << "FF47"
           << "FF48"
           << "FF49"
           << "FF4A"
           << "FF4B"
           << "FF68"
           << "FF69"
           << "FF6A"
           << "FF6B";
  for (int i = 0; i < 16; i++) {
    ui->videoRegistersWidget->setItem(i, 0,
                                      new QTableWidgetItem(addrList.at(i)));
    ui->videoRegistersWidget->setItem(
        i, 1, new QTableWidgetItem(qstring_hex_pad(registers[i + 6], 4)));
  }

  addrList.clear();
  addrList << "FF00"
           << "FF01"
           << "FF02"
           << "FF04"
           << "FF05"
           << "FF06"
           << "FF07"
           << "FF4D"
           << "FF4F"
           << "FF51"
           << "FF52"
           << "FF53"
           << "FF54"
           << "FF55"
           << "FF70"
           << "FF0F"
           << "FFFF";
  for (int i = 0; i < 17; i++) {
    ui->otherRegistersWidget->setItem(i, 0,
                                      new QTableWidgetItem(addrList.at(i)));
    ui->otherRegistersWidget->setItem(
        i, 1, new QTableWidgetItem(qstring_hex_pad(registers[i + 6 + 16], 4)));
  }
}

void DebuggerWindow::refresh_memory_map(bool init) {
  QStringList titles;
  std::vector<Byte> memory_dump =
      g_gameboy->get_debugger().get_memory_dump(_addr_memory_map);
  if (memory_dump.size() == 160) {
    int k = 0;
    for (int i = 0; i < 10; i++) {
      titles << qstring_hex_pad(_addr_memory_map + (16 * i), 4);
      for (int j = 0; j < 16; j++) {
        if (init)
          ui->memoryWidget->setItem(
              i, j, new QTableWidgetItem(qstring_hex_pad(memory_dump[k], 2)));
        else
          ui->memoryWidget->item(i, j)->setText(
              qstring_hex_pad(memory_dump[k], 2));
        k++;
      }
    }
    ui->memoryWidget->setVerticalHeaderLabels(titles);
  }
  if (init) init = false;
}

void DebuggerWindow::refresh_instr(bool init) {
  QStringList titles;
  g_gameboy->get_debugger().set_instruction_pool_size(6);
  std::vector<Debugger::_debug_info> instr_pool =
      g_gameboy->get_debugger().get_instruction_pool();
  QString value;
  int pool_size = instr_pool.size();
  for (int i = 0; i < pool_size && i < 6; i++) {
    titles << qstring_hex_pad(instr_pool[i].pc, 4);
    if (init)
      ui->disassemblerWidget->setItem(
          i, 0, new QTableWidgetItem(instr_pool[i].instr));
    else
      ui->disassemblerWidget->item(i, 0)->setText(instr_pool[i].instr);
    value =
        qstring_hex_pad(instr_pool[i].value[0], 2) + " " +
        (instr_pool[i].size >= 2 ? qstring_hex_pad(instr_pool[i].value[1], 2)
                                 : "") +
        " " +
        (instr_pool[i].size == 3 ? qstring_hex_pad(instr_pool[i].value[2], 2)
                                 : "");
    if (init)
      ui->disassemblerWidget->setItem(i, 1, new QTableWidgetItem(value));
    else
      ui->disassemblerWidget->item(i, 1)->setText(value);
  }
  ui->disassemblerWidget->setVerticalHeaderLabels(titles);
}

void DebuggerWindow::reset_color(QTableWidget *widget, int column) {
  int nbItem = widget->rowCount();
  for (int i = 0; i < nbItem; i++) {
    QTableWidgetItem *item = widget->item(i, column);
    item->setForeground(QBrush(QApplication::palette().text()));
  }
}

void DebuggerWindow::refresh_registers() {
  std::vector<std::pair<int, uint16_t>> registers =
      g_gameboy->get_debugger().get_register_diffs();
  reset_color(ui->registersWidget, 0);
  reset_color(ui->videoRegistersWidget, 1);
  reset_color(ui->otherRegistersWidget, 1);
  for (auto value : registers) {
    if (value.first >= MAIN_REGISTERS_BEGIN &&
        value.first <= MAIN_REGISTERS_END) {
      ui->registersWidget->item(value.first, 0)
          ->setText(qstring_hex_pad(value.second, 4));
      ui->registersWidget->item(value.first, 0)
          ->setForeground(QBrush(QColor(Qt::blue)));
    } else if (value.first >= VIDEO_REGISTERS_BEGIN &&
               value.first <= VIDEO_REGISTERS_END) {
      ui->videoRegistersWidget->item(value.first - VIDEO_REGISTERS_BEGIN, 1)
          ->setText(qstring_hex_pad(value.second, 4));
      ui->videoRegistersWidget->item(value.first - VIDEO_REGISTERS_BEGIN, 1)
          ->setForeground(QBrush(QColor(Qt::blue)));
    } else if (value.first >= OTHER_REGISTERS_BEGIN &&
               value.first <= OTHER_REGISTERS_END) {
      ui->otherRegistersWidget->item(value.first - OTHER_REGISTERS_BEGIN, 1)
          ->setText(qstring_hex_pad(value.second, 4));
      ui->otherRegistersWidget->item(value.first - OTHER_REGISTERS_BEGIN, 1)
          ->setForeground(QBrush(QColor(Qt::blue)));
    }
  }
}

bool DebuggerWindow::duplicateInListWidgetItem(const QString &value,
                                               const QListWidget *list) {
  int nbItem = list->count();
  for (int i = 0; i < nbItem; i++) {
    QListWidgetItem *item = list->item(i);
    if (item->text() == value) return (true);
  }
  return (false);
}

void DebuggerWindow::add_watchpoint(const QString &label,
                                    const QString &addrText) {
  bool ok;
  QString value = QInputDialog::getText(
      this, tr("Add Watchpoint"), tr("Value:"), QLineEdit::Normal, "", &ok);
  QRegExp hexMatcher("^[0-9A-F]{1,4}$", Qt::CaseInsensitive);
  if (hexMatcher.exactMatch(value) || value.isEmpty()) {
    int32_t valueHex = value.isEmpty() ? -1 : value.toInt(&ok, 16);
    if (ok) {
      uint16_t addr = addrText.toUInt(&ok, 16);
      if (ok) {
        if (!duplicateInListWidgetItem(
                label + ":" + qstring_hex_pad(addr, 4) + ":" +
                    (value.isEmpty() ? "" : qstring_hex_pad(valueHex, 4)),
                ui->watchpointsWidget)) {
          g_gameboy->get_debugger().add_watchpoint(addr, valueHex);
          ui->watchpointsWidget->addItem(new QListWidgetItem(
              label + ":" + qstring_hex_pad(addr, 4) + ":" +
              (value.isEmpty() ? "" : qstring_hex_pad(valueHex, 4))));
        }
      }
    }
  }
}

void DebuggerWindow::addBreakpoint() {
  QRegExp hexMatcher("^[0-9A-F]{1,4}$", Qt::CaseInsensitive);
  if (hexMatcher.exactMatch(ui->breakpointsEdit->text())) {
    bool ok;
    uint16_t addr = ui->breakpointsEdit->text().toUInt(&ok, 16);
    if (ok) {
      if (!duplicateInListWidgetItem(qstring_hex_pad(addr, 4),
                                     ui->breakpointsWidget)) {
        g_gameboy->get_debugger().add_breakpoint(addr);
        ui->breakpointsWidget->addItem(
            new QListWidgetItem(qstring_hex_pad(addr, 4)));
      }
    }
  }
  ui->breakpointsEdit->clear();
}

void DebuggerWindow::refresh_info() {
  refresh_registers();
  refresh_instr();
  refresh_memory_map();
}

void DebuggerWindow::on_stepButton_clicked() {
  g_gameboy->notify_debugger(Debugger::RUN_ONE_STEP);
  while (!g_gameboy->get_debugger().get_lock()) {
  }
  refresh_info();
}

void DebuggerWindow::on_runOneFrameButton_clicked() {
  g_gameboy->notify_debugger(Debugger::RUN_ONE_FRAME);
  while (!g_gameboy->get_debugger().get_lock()) {
  }
  refresh_info();
}

void DebuggerWindow::on_runDurationButton_clicked() {
  g_gameboy->notify_debugger(Debugger::RUN_DURATION,
                             ui->runDurationSpinBox->value());
  while (!g_gameboy->get_debugger().get_lock()) {
  }
  refresh_info();
}

void DebuggerWindow::on_addBreakpointButton_clicked() { addBreakpoint(); }

void DebuggerWindow::on_breakpointsEdit_returnPressed() { addBreakpoint(); }

void DebuggerWindow::on_deleteBreakpointButton_clicked() {
  QList<QListWidgetItem *> items = ui->breakpointsWidget->selectedItems();
  foreach (QListWidgetItem *item, items) {
    bool ok;
    uint16_t addr =
        ui->breakpointsWidget->takeItem(ui->breakpointsWidget->row(item))
            ->text()
            .toUInt(&ok, 16);
    if (ok) {
      g_gameboy->get_debugger().remove_breakpoint(addr);
      delete ui->breakpointsWidget->takeItem(ui->breakpointsWidget->row(item));
    }
  }
}

void DebuggerWindow::on_DebuggerWindow_rejected() {
  g_gameboy->get_debugger().set_lock(false);
  g_gameboy->get_debugger().toggle();
}

void DebuggerWindow::on_deleteWatchpointButton_clicked() {
  QList<QListWidgetItem *> items = ui->watchpointsWidget->selectedItems();
  foreach (QListWidgetItem *item, items) {
    bool okAddr;
    bool okValue;
    QStringList watchpointText =
        ui->watchpointsWidget->takeItem(ui->watchpointsWidget->row(item))
            ->text()
            .split(":");
    uint16_t addr = watchpointText.at(1).toUInt(&okAddr, 16);
    uint16_t value = watchpointText.at(2).toUInt(&okValue, 16);
    if (okAddr && okValue) {
      g_gameboy->get_debugger().remove_watchpoint(addr, value);
      delete ui->watchpointsWidget->takeItem(ui->watchpointsWidget->row(item));
    }
  }
}

void DebuggerWindow::on_memoryLineEdit_editingFinished() {
  QRegExp hexMatcher("^[0-9A-F]{1,4}$", Qt::CaseInsensitive);
  if (hexMatcher.exactMatch(ui->memoryLineEdit->text())) {
    bool ok;
    uint16_t addr = ui->memoryLineEdit->text().toUInt(&ok, 16);
    if (ok) {
      addr &= 0xFFF0;
      if (addr > 0xFF60) addr = 0xFF60;
      _addr_memory_map = addr;
      refresh_memory_map();
    }
  }
  ui->memoryLineEdit->clear();
}

void DebuggerWindow::on_resetButton_clicked() {
  g_gameboy->reset();
  refresh_info();
}

void DebuggerWindow::on_runCpuSecondButton_clicked() {
  g_gameboy->notify_debugger(Debugger::RUN_CPU_SEC);
  while (!g_gameboy->get_debugger().get_lock()) {
  }
  refresh_info();
}

void DebuggerWindow::add_watchpoint_manual() {
  bool ok = true;
  if (ui->watchpointsAddrEdit->text().isEmpty()) return;
  QRegExp hexMatcher("^[0-9A-F]{1,4}$", Qt::CaseInsensitive);
  if (hexMatcher.exactMatch(ui->watchpointsValueEdit->text()) ||
      ui->watchpointsValueEdit->text().isEmpty()) {
    int32_t valueHex = ui->watchpointsValueEdit->text().isEmpty()
                           ? -1
                           : ui->watchpointsValueEdit->text().toInt(&ok, 16);
    if (ok) {
      uint16_t addr = ui->watchpointsAddrEdit->text().toUInt(&ok, 16);
      if (ok && addr > 5) {
        if (!duplicateInListWidgetItem(
                "NONE:" + qstring_hex_pad(addr, 4) + ":" +
                    (ui->watchpointsValueEdit->text().isEmpty()
                         ? ""
                         : qstring_hex_pad(valueHex, 4)),
                ui->watchpointsWidget)) {
          g_gameboy->get_debugger().add_watchpoint(addr, valueHex);
          ui->watchpointsWidget->addItem(
              new QListWidgetItem("NONE:" + qstring_hex_pad(addr, 4) + ":" +
                                  (ui->watchpointsValueEdit->text().isEmpty()
                                       ? ""
                                       : qstring_hex_pad(valueHex, 4))));
        }
      }
    }
  }
  ui->watchpointsAddrEdit->clear();
  ui->watchpointsValueEdit->clear();
}

void DebuggerWindow::on_watchpointsAddrEdit_returnPressed() {
  add_watchpoint_manual();
}

void DebuggerWindow::on_watchpointsValueEdit_returnPressed() {
  add_watchpoint_manual();
}

void DebuggerWindow::on_addWatchpointButton_clicked() {
  add_watchpoint_manual();
}
