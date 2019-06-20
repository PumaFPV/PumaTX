#include "simulateduiwidget.h"
#include "ui_simulateduiwidgetJumperT12.h"

// NOTE: RadioUiAction(NUMBER,...): NUMBER relates to enum EnumKeys in the specific board.h

SimulatedUIWidgetJumperT12::SimulatedUIWidgetJumperT12(SimulatorInterface *simulator, QWidget * parent):
  SimulatedUIWidget(simulator, parent),
  ui(new Ui::SimulatedUIWidgetJumperT12)
{
  RadioUiAction * act;
  QPolygon polygon;

  ui->setupUi(this);

  act = new RadioUiAction(3, QList<int>() << Qt::Key_Up << Qt::Key_PageUp, SIMU_STR_HLP_KEYS_GO_UP, SIMU_STR_HLP_ACT_UP);
  addRadioWidget(ui->rightbuttons->addArea(QRect(69, 11, 60, 25), "JumperT12/right_up.png", act));

  act = new RadioUiAction(2, QList<int>() << Qt::Key_Down << Qt::Key_PageDown, SIMU_STR_HLP_KEYS_GO_DN, SIMU_STR_HLP_ACT_DN);
  addRadioWidget(ui->rightbuttons->addArea(QRect(61, 115, 60, 25), "JumperT12/right_down.png", act));

  act = new RadioUiAction(4, QList<int>() << Qt::Key_Right << Qt::Key_Plus, SIMU_STR_HLP_KEY_RGT % "|" % SIMU_STR_HLP_KEY_PLS, SIMU_STR_HLP_ACT_PLS);
  addRadioWidget(ui->rightbuttons->addArea(QRect(109, 53, 30, 40), "JumperT12/right_right.png", act));

  act = new RadioUiAction(5, QList<int>() << Qt::Key_Left << Qt::Key_Minus, SIMU_STR_HLP_KEY_LFT % "|" % SIMU_STR_HLP_KEY_MIN, SIMU_STR_HLP_ACT_MIN);
  addRadioWidget(ui->rightbuttons->addArea(QRect(49, 62, 30, 40), "JumperT12/right_left.png", act));

  m_mouseMidClickAction = new RadioUiAction(1, QList<int>() << Qt::Key_Enter << Qt::Key_Return, SIMU_STR_HLP_KEYS_ACTIVATE, SIMU_STR_HLP_ACT_ENT);
  addRadioWidget(ui->leftbuttons->addArea(QRect(21, 30, 60, 25), "JumperT12/left_ent.png", m_mouseMidClickAction));

  act = new RadioUiAction(0, QList<int>() << Qt::Key_Delete << Qt::Key_Escape << Qt::Key_Backspace, SIMU_STR_HLP_KEYS_EXIT, SIMU_STR_HLP_ACT_EXIT);
  addRadioWidget(ui->leftbuttons->addArea(QRect(21, 80, 60, 60), "JumperT12/left_exit.png", act));

  addRadioWidget(ui->leftbuttons->addArea(QRect(9, 154, 30, 30), "JumperT12/left_scrnshot.png", m_screenshotAction));

  m_backlightColors << QColor(215, 243, 255);  // X7 Blue
  m_backlightColors << QColor(166,247,159);
  m_backlightColors << QColor(247,159,166);
  m_backlightColors << QColor(255,195,151);
  m_backlightColors << QColor(247,242,159);

  setLcd(ui->lcd);

  QString css = "#radioUiWidget {"
                "background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1,"
                "stop:0 rgba(255, 255, 255, 255),"
                "stop:0.757062 rgba(241, 238, 238, 255),"
                "stop:1 rgba(247, 245, 245, 255));"
                "}";

  QTimer * tim = new QTimer(this);
  tim->setSingleShot(true);
  connect(tim, &QTimer::timeout, [this, css]() {
    emit customStyleRequest(css);
  });
  tim->start(100);
}

SimulatedUIWidgetJumperT12::~SimulatedUIWidgetJumperT12()
{
  delete ui;
}
