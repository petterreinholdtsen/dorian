#ifndef DYALOG_H
#define DYALOG_H

#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QMenuBar>

class QWidget;
class QPushButton;
class QScrollArea;
class QVBoxLayout;
class QMenuBar;
class QAction;

/** Maemo- and Mac-friendly dialog box. */
class Dyalog:
        public QDialog
{
    Q_OBJECT

public:
    explicit Dyalog(QWidget *parent, bool showButtons = true);

    /** Add widget to the scrollable content pane. */
    void addWidget(QWidget *widget);

    /** Add stretch to the scrollable content pane. */
    void addStretch(int stretch = 0);

    /** Add button to the dialog button box. */
    void addButton(const QString &label, QObject *receiver, const char *slot,
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::AcceptRole);

public slots:
#ifdef Q_OS_SYMBIAN
    void show();
    int exec();
#endif

private:
    QScrollArea *scroller;
    QWidget *content;
    QVBoxLayout *contentLayout;
    QDialogButtonBox *buttonBox;
    bool showButtons;
#ifdef Q_OS_SYMBIAN
    QAction *leftSoftKey;
    QMenuBar *menuBar;
#endif
};

#endif // DYALOG_H
