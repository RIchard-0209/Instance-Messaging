QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PictureBubble.cpp \
    TextBubble.cpp \
    appsources.cpp \
    bubbleframe.cpp \
    chatdialog.cpp \
    chatitembase.cpp \
    chatpage.cpp \
    chatuserlist.cpp \
    chatuserwid.cpp \
    chatview.cpp \
    clickedbtn.cpp \
    clickedlabel.cpp \
    customizeedit.cpp \
    global.cpp \
    httpmgr.cpp \
    listitembase.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    messagetextedit.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    statewidget.cpp \
    tcpmgr.cpp \
    timerbtn.cpp \
 #   userdata.cpp \
    usermgr.cpp

HEADERS += \
    PictureBubble.h \
    TextBubble.h \
    appsources.h \
    bubbleframe.h \
    chatdialog.h \
    chatitembase.h \
    chatpage.h \
    chatuserlist.h \
    chatuserwid.h \
    chatview.h \
    clickedbtn.h \
    clickedlabel.h \
    customizeedit.h \
    global.h \
    httpmgr.h \
    listitembase.h \
    logindialog.h \
    mainwindow.h \
    messagetextedit.h \
    registerdialog.h \
    resetdialog.h \
    singleton.h \
    statewidget.h \
    tcpmgr.h \
    timerbtn.h \
#    userdata.h \
    usermgr.h

FORMS += \
    chatdialog.ui \
    chatpage.ui \
    chatuserwid.ui \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    resetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RC_ICONS = icon.ico
# 目的文件夹
DESTDIR = $${TARGET}/bin

RESOURCES += \
    src.qrc

DISTFILES += \
    config.ini



# win32:CONFIG(debug, debug | release)
# {
#     #指定要拷贝的文件目录为工程目录下release目录下的所有dll、lib文件，例如工程目录在D:\QT\Test
#     #PWD就为D:/QT/Test，DllFile = D:/QT/Test/release/*.dll
#     TargetConfig = $${PWD}/config.ini
#     #将输入目录中的"/"替换为"\"
#     TargetConfig = $$replace(TargetConfig, /, \\)
#     #将输出目录中的"/"替换为"\"
#     OutputDir =  $${OUT_PWD}/$${DESTDIR}
#     OutputDir = $$replace(OutputDir, /, \\)
#     #执行copy命令
#     message("TargetConfig: $$TargetConfig")
#     message("OutputDir: $$OutputDir")

#     copydata.commands = copy $$TargetConfig $$OutputDir
#     QMAKE_EXTRA_TARGETS += copydata
# }

win32:CONFIG(debug, debug | release)
{
    #指定要拷贝的文件目录为工程目录下release目录下的所有dll、lib文件，例如工程目录在D:\QT\Test
    #PWD就为D:/QT/Test，DllFile = D:/QT/Test/release/*.dll
    TargetConfig = $${PWD}/config.ini
    #将输入目录中的"/"替换为"\"
    TargetConfig = $$replace(TargetConfig, /, \\)
    #将输出目录中的"/"替换为"\"
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    //执行copy命令
    QMAKE_POST_LINK += cp -f \"$$TargetConfig\" \"$$OutputDir\"
}
