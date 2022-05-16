#include "MainWindow.h"

#include "process/Cpu.h"
#include "process/Process.h"
#include "process/ProcessWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
    const QString baseName = "simos_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }

  //  MainWindow w;
  //  w.show();

  auto                    &p = ProcessScheduler::GetInstance();
  std::vector<Instruction> insts;

  insts.push_back(Instruction{"A", [](Process &) { return false; }});
  insts.push_back(Instruction{"B", [](Process &) { return false; }});
  insts.push_back(Instruction{"C", [](Process &) { return false; }});
  insts.push_back(Instruction{"D", [](Process &) { return false; }});
  insts.push_back(Instruction{"E", [](Process &) { return false; }});
  insts.push_back(Instruction{"F", [](Process &) { return false; }});
  insts.push_back(Instruction{"G", [](Process &) { return false; }});
  insts.push_back(Instruction{"A", [](Process &) { return false; }});
  insts.push_back(Instruction{"B", [](Process &) { return false; }});
  insts.push_back(Instruction{"A", [](Process &) { return false; }});

  p.StartProcess("First", insts);

  insts.push_back(Instruction{"1", [](Process &) { return false; }});
  insts.push_back(Instruction{"2", [](Process &) { return false; }});
  insts.push_back(Instruction{"3", [](Process &) { return false; }});
  insts.push_back(Instruction{"4", [](Process &) { return false; }});
  insts.push_back(Instruction{"5", [](Process &) { return false; }});
  insts.push_back(Instruction{"6", [](Process &) { return false; }});
  insts.push_back(Instruction{"7", [](Process &) { return false; }});
  insts.push_back(Instruction{"8", [](Process &) { return false; }});
  insts.push_back(Instruction{"9", [](Process &) { return false; }});
  insts.push_back(Instruction{"10", [](Process &) { return false; }});
  insts.push_back(Instruction{"11", [](Process &) { return false; }});
  insts.push_back(Instruction{"12", [](Process &) { return false; }});
  insts.push_back(Instruction{"13", [](Process &) { return false; }});
  insts.push_back(Instruction{"14", [](Process &) { return false; }});
  insts.push_back(Instruction{"15", [](Process &) { return false; }});
  insts.push_back(Instruction{"16", [](Process &) { return false; }});
  insts.push_back(Instruction{"17", [](Process &) { return false; }});
  insts.push_back(Instruction{"18", [](Process &) { return false; }});
  insts.push_back(Instruction{"19", [](Process &) { return false; }});
  insts.push_back(Instruction{"20", [](Process &) { return false; }});
  insts.push_back(Instruction{"21", [](Process &) { return false; }});
  insts.push_back(Instruction{"22", [](Process &) { return false; }});
  insts.push_back(Instruction{"23", [](Process &) { return false; }});
  insts.push_back(Instruction{"24", [](Process &) { return false; }});
  insts.push_back(Instruction{"25", [](Process &) { return false; }});
  insts.push_back(Instruction{"26", [](Process &) { return false; }});
  insts.push_back(Instruction{"27", [](Process &) { return false; }});
  p.StartProcess("Second", insts);

  ProcessWindow proc_window;
  proc_window.show();
  return a.exec();
}
