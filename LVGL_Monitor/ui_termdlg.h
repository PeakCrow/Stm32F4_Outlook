/********************************************************************************
** Form generated from reading UI file 'termdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TERMDLG_H
#define UI_TERMDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TermDlg
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;

    void setupUi(QDialog *TermDlg)
    {
        if (TermDlg->objectName().isEmpty())
            TermDlg->setObjectName(QString::fromUtf8("TermDlg"));
        TermDlg->resize(753, 254);
#if QT_CONFIG(whatsthis)
        TermDlg->setWhatsThis(QString::fromUtf8(""));
#endif // QT_CONFIG(whatsthis)
        verticalLayout = new QVBoxLayout(TermDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));

        verticalLayout->addLayout(verticalLayout_2);


        retranslateUi(TermDlg);

        QMetaObject::connectSlotsByName(TermDlg);
    } // setupUi

    void retranslateUi(QDialog *TermDlg)
    {
        TermDlg->setWindowTitle(QCoreApplication::translate("TermDlg", "Terminal", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TermDlg: public Ui_TermDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TERMDLG_H
