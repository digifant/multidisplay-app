#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include <QDebug>
#include <QSettings>
#include <QPushButton>

#include <builddatetime.h>

#ifdef Q_OS_ANDROID
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QAndroidJniObject>
#else
//HACK https://doc-snapshots.qt.io/qt6-dev/extras-changes-qt6.html
#include <QtCore/private/qandroidextras_p.h>
#endif
#endif

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
//#if defined (DIGIFANTAPP) or defined ( DIGIFANTVANAPP )
    ui->textBrowser->hide();
#if defined (Q_OS_ANDROID)
    ui->buttonBox->hide();
#endif
    QString dfoAbout;
    dfoAbout="<h1>" + tr("multidisplay UI") + "</h1>";
    dfoAbout+="<p>...by digifant-onlineabstimmung.de</p>";
    dfoAbout+="<br>";
    dfoAbout+="<p>M.Sc. Dominik Gummel<br>St-Agatha Straße 8<br>66271 Kleinblittersdorf<br>Germany<br><br>UstId DE 272480438<br><br>info@digifant-onlineabstimmung.de<br>Phone: +49 6805 / 9301963";
#define QUOTE(string) _QUOTE(string)
#define _QUOTE(string) #string
    //dfoAbout+="<br><p>" + QString(QUOTE(ANDROID_VERSION_NAME)) + " / " + QString(QUOTE(ANDROID_VERSION_CODE)) + " built on " + BUILDDATETIME + "</p>";
    dfoAbout+="<br><p>" + QString(QUOTE(ANDROID_VERSION_NAME)) + " / " + QString(QUOTE(ANDROID_VERSION_CODE)) + " X-mas Edition 2022 </p>";
    ui->label->setText(dfoAbout);

#if defined (Q_OS_ANDROID)
    QPushButton *emailButton = new QPushButton ("email me", this);
    ui->verticalLayout->addWidget (emailButton);
    connect(emailButton, SIGNAL(clicked()), this, SLOT(emailIntend()));

    QPushButton *httpButton = new QPushButton ("more information (web link)", this);
    ui->verticalLayout->addWidget (httpButton);
    connect(httpButton, SIGNAL(clicked()), this, SLOT(httpIntend()));

    //delete on close! AndroidMainWindows eventhandler will initiate the re-creation
    //fixes broken dialog on 2. show
    setAttribute( Qt::WA_DeleteOnClose, true );
 #endif
//#endif
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::emailIntend() {
#if defined (Q_OS_ANDROID)
    qDebug() << "emailIntend";
#if defined (DIGIFANTAPP) or defined ( DIGIFANTVANAPP )
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    //Qt 5
    QAndroidJniObject to = QAndroidJniObject::fromString("info@digifant-onlineabstimmung.de");
    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper", "sendEmail","(Ljava/lang/String;)V",
                                               to.object<jstring>() );
#else
    //Qt 6
    QJniObject to = QJniObject::fromString("info@digifant-onlineabstimmung.de");
    QJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper", "sendEmail","(Ljava/lang/String;)V",
                                               to.object<jstring>() );
#endif

    //QAndroidJniObject s1 = QAndroidJniObject::fromString("http://blog.fefe.de");
    //QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper",
    //                                       "openUrl",
    //                                       "(Ljava/lang/String;)V",
    //                                        s1.object<jstring>() );
#else
    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/mui/MuiIntentHelper","sendEmail");
#endif
#endif
}

void AboutDialog::httpIntend() {
#if defined (Q_OS_ANDROID)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAndroidJniObject s1 = QAndroidJniObject::fromString("http://mdforum.designer2k2.at/viewtopic.php?f=15&t=334");
    QSettings settings;
    if ( settings.value("md/md", QVariant ( true ).toBool() )  == false ) {
        s1 = QAndroidJniObject::fromString("http://digifant-einzelabstimmung.de/bofh-ng/de/digifant-1/live-daten-auslesen");
    }
#else
    QJniObject s1 = QJniObject::fromString("http://mdforum.designer2k2.at/viewtopic.php?f=15&t=334");
    QSettings settings;
    if ( settings.value("md/md", QVariant ( true ).toBool() )  == false ) {
        s1 = QJniObject::fromString("http://digifant-einzelabstimmung.de/bofh-ng/de/digifant-1/live-daten-auslesen");
    }
#endif
#if defined ( DIGIFANTVANAPP )
    s1 = QAndroidJniObject::fromString("http://digifant-einzelabstimmung.de/bofh-ng/de/digifant-1/live-daten-auslesen");
#endif

#if defined ( DIGIFANTAPP ) or defined ( DIGIFANTVANAPP )
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper",
                                           "openUrl",
                                           "(Ljava/lang/String;)V",
                                            s1.object<jstring>() );
#else
    QJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper",
                                           "openUrl",
                                           "(Ljava/lang/String;)V",
                                            s1.object<jstring>() );
#endif
#else
    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/mui/MuiIntentHelper",
                                           "openUrl",
                                           "(Ljava/lang/String;)V",
                                            s1.object<jstring>() );
#endif
#endif
}
