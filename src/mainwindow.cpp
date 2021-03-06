// alterlinux-i3-manager - A i3wm setting manager for Alter Linux
// mainwindow.cpp
//
// Watasuke
// Email  : <watasuke102@gmail.com>
// Twitter: @Watasuke102
//
// (c) 2020 Fascode Network.
// This software is released under the GPL-3.0 License.

#include "mainwindow.h"
#include <QtWidgets>
#include "looks_page.h"
#include "modules_page.h"

_MainWindow::_MainWindow(QWidget* parent) {
  // Layouts
  QVBoxLayout* layout    = new QVBoxLayout();
  QHBoxLayout* buttons   = new QHBoxLayout();
  // Widgets
  QWidget*     main_widget    = new QWidget();
  QTabWidget*  tab_widget     = new QTabWidget();
  QPushButton* button_exit    = new QPushButton(tr("Exit"));
  QPushButton* button_restore = new QPushButton(tr("Restore to default settings"));
  QPushButton* button_apply   = new QPushButton(tr("Apply"));
  // Add pages to TabWidget
  tab_looks_   = new _LooksPage();
  tab_modules_ = new _ModulesPage();
  tab_widget->addTab(tab_looks_,   tr("Looks"));
  tab_widget->addTab(tab_modules_, tr("Modules"));
  // Buttons init
  QObject::connect(button_exit,    &QPushButton::clicked, this, &_MainWindow::exit_);
  QObject::connect(button_restore, &QPushButton::clicked, this, &_MainWindow::restore_);
  QObject::connect(button_apply,   &QPushButton::clicked, this, &_MainWindow::apply_);
  buttons->addWidget(button_exit);
  buttons->addWidget(button_restore);
  buttons->addStretch();
  buttons->addWidget(button_apply);
  // Add widgets to layout
  QLabel* header = new QLabel(tr("Settings manager for Alter Linux i3wm edition "));
  header->setStyleSheet("font-size:20px;");
  layout->addWidget(header);
  layout->addWidget(tab_widget);
  layout->addLayout(buttons);
  // Add layout to MainWidget
  main_widget->setLayout(layout);
  setCentralWidget(main_widget);
  setFixedSize(500, 400);
}

void _MainWindow::CreateManagerConfigDirectory()
{
  // Create directory
  QDir config_file_dir(QDir::homePath() + "/.config");
  if (!config_file_dir.exists("alterlinux-i3-manager"))
    config_file_dir.mkdir("alterlinux-i3-manager");
}

void _MainWindow::closeEvent(QCloseEvent* event)
{
  int ans = QMessageBox::question(this, tr("Warning"), tr("Changes will be discarded.\nAre you sure to exit?"));
  if(ans == QMessageBox::No) event->ignore();
}

void _MainWindow::exit_() {
  close();
}

void _MainWindow::restore_() {
  int ans = QMessageBox::question(this, tr("Warning"), tr("Are you sure to set default settings?"));
  if(ans == QMessageBox::No) return;
  tab_looks_  ->ResetAllState();
  tab_modules_->ResetAllState();
  system("rm -rf ~/.config/alterlinux-i3-manager");
  system("cp -r /usr/share/alterlinux-i3-manager/data/alterlinux-i3-manager ~/.config");
  system("rm -rf ~/.config/polybar");
  system("cp -r /usr/share/alterlinux-i3-manager/data/polybar ~/.config");
  system("rm -rf ~/.config/rofi");
  system("cp -r /usr/share/alterlinux-i3-manager/data/rofi ~/.config");
  system("i3-msg restart");
}

void _MainWindow::apply_() {
  _SettingList list;
  list.shape_        = tab_looks_->SelectedShape_();
  list.theme_color_  = tab_looks_->ThemeColor_();
  list.icon_color_   = tab_looks_->IconColor_();
  list.bar_position_ = tab_looks_->BarPosition_();
  list.translucent_  = tab_looks_->TranslucentBar_();
  bool rounded = tab_looks_->RoundedBothEnds_();
  if (rounded == true) {
    // when bar is on the bottom
    if(list.bar_position_ == "true")
      list.rounded_ = "-top     = 15.0";
    // when bar is on the top
    if(list.bar_position_ == "false")
      list.rounded_ = "-bottom  = 15.0";
  } else
      list.rounded_ = "         = 0";
  ChangeRofiSetting_(list);
  ChangePolybarSetting_(list);
  ChangeModulesSetting_();
  ChangeManagersPolybarSetting_(list);
  ChangeManagersModulesSetting_(list);
}
