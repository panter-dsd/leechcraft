IF (NOT QT_USE_FILE)
	CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
	IF (COMMAND cmake_policy)
		cmake_policy (SET CMP0003 NEW)
	ENDIF (COMMAND cmake_policy)

	PROJECT (leechcraft_poshuku)

	IF (NOT CMAKE_MODULE_PATH)
		SET (CMAKE_MODULE_PATH "/usr/local/share/leechcraft/cmake;/usr/share/leechcraft/cmake")
	ENDIF (NOT CMAKE_MODULE_PATH)

	FIND_PACKAGE (Boost REQUIRED)
	FIND_PACKAGE (Qt4 REQUIRED)
	FIND_PACKAGE (LeechCraft REQUIRED)
ENDIF (NOT QT_USE_FILE)

OPTION (ENABLE_IDN "Enable support for Internationalized Domain Names" OFF)

IF (ENABLE_IDN)
	ADD_DEFINITIONS (-DENABLE_IDN)
ENDIF (ENABLE_IDN)

SET (QT_USE_QTSQL TRUE)
SET (QT_USE_QTXML TRUE)
SET (QT_USE_QTWEBKIT TRUE)
SET (QT_USE_QTNETWORK TRUE)
INCLUDE (${QT_USE_FILE})
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${LEECHCRAFT_INCLUDE_DIR}
	)
SET (SRCS
	poshuku.cpp
	xmlsettingsmanager.cpp
	core.cpp
	browserwidget.cpp
	customwebview.cpp
	progresslineedit.cpp
	favoritesmodel.cpp
	addtofavoritesdialog.cpp
	filtermodel.cpp
	favoritesdelegate.cpp
	favoritestreeview.cpp
	customwebpage.cpp
	historymodel.cpp
	storagebackend.cpp
	sqlstoragebackend.cpp
	sqlstoragebackend_mysql.cpp
	urlcompletionmodel.cpp
	finddialog.cpp
	screenshotsavedialog.cpp
	cookieseditdialog.cpp
	cookieseditmodel.cpp
	sourceviewer.cpp
	htmlhighlighter.cpp
	xbelparser.cpp
	xbelgenerator.cpp
	pluginmanager.cpp
	historyfiltermodel.cpp
	proxyobject.cpp
	jsproxy.cpp
	externalproxy.cpp
	notification.cpp
	passwordremember.cpp
	pageformsdata.cpp
	linkhistory.cpp
	cookiesfilter.cpp
	searchtext.cpp
	reloadintervalselector.cpp
	editbookmarkdialog.cpp
	favoriteschecker.cpp
	webpluginfactory.cpp
	browserwidgetsettings.cpp
	hideabletabwidget.cpp
	bookmarkswidget.cpp
	historywidget.cpp
	urlframe.cpp
	importentityhandler.cpp
	)
SET (HEADERS
	poshuku.h
	xmlsettingsmanager.h
	core.h
	browserwidget.h
	customwebview.h
	progresslineedit.h
	favoritesmodel.h
	addtofavoritesdialog.h
	filtermodel.h
	favoritesdelegate.h
	favoritestreeview.h
	customwebpage.h
	historymodel.h
	storagebackend.h
	sqlstoragebackend.h
	sqlstoragebackend_mysql.h
	urlcompletionmodel.h
	finddialog.h
	screenshotsavedialog.h
	cookieseditdialog.h
	cookieseditmodel.h
	sourceviewer.h
	htmlhighlighter.h
	xbelparser.h
	xbelgenerator.h
	pluginmanager.h
	historyfiltermodel.h
	proxyobject.h
	jsproxy.h
	externalproxy.h
	notification.h
	passwordremember.h
	pageformsdata.h
	linkhistory.h
	cookiesfilter.h
	searchtext.h
	reloadintervalselector.h
	editbookmarkdialog.h
	favoriteschecker.h
	webpluginfactory.h
	browserwidgetsettings.h
	hideabletabwidget.h
	bookmarkswidget.h
	historywidget.h
	urlframe.h
	importentityhandler.h
	interfaces/iwebplugin.h
	interfaces/iflashoverrider.h
	)
SET (FORMS
	browserwidget.ui
	addtofavoritesdialog.ui
	finddialog.ui
	screenshotsavedialog.ui
	cookieseditdialog.ui
	sourceviewer.ui
	passwordremember.ui
	searchtext.ui
	reloadintervalselector.ui
	editbookmarkdialog.ui
	hideabletabwidget.ui
	bookmarkswidget.ui
	historywidget.ui
	urlframe.ui
	)
SET (RESOURCES poshukuresources.qrc)
CreateTrVars ("poshuku" "ar;be;de;en;eo;es;fr;it;pl;ru_RU;tt;uk_UA" TRANSLATIONS COMPILED_TRANSLATIONS)

IF (NOT LC_NO_MOC)
	QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
ENDIF (NOT LC_NO_MOC)
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCCS ${RESOURCES})

ADD_CUSTOM_COMMAND (OUTPUT
	${COMPILED_TRANSLATIONS}
	COMMAND "${QT_LRELEASE_EXECUTABLE}" ${TRANSLATIONS}
	DEPENDS ${TRANSLATIONS}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
ADD_LIBRARY (leechcraft_poshuku SHARED
	${COMPILED_TRANSLATIONS}
	${SRCS}
	${MOC_SRCS}
	${UIS_H}
	${RCCS}
	)

IF (ENABLE_IDN)
	SET (IDN_LIB "idn")
ENDIF (ENABLE_IDN)

TARGET_LINK_LIBRARIES (leechcraft_poshuku
	${QT_LIBRARIES}
	${LEECHCRAFT_LIBRARIES}
	${IDN_LIB}
	)
INSTALL (TARGETS leechcraft_poshuku DESTINATION ${LC_PLUGINS_DEST})
INSTALL (FILES ${COMPILED_TRANSLATIONS} DESTINATION ${LC_TRANSLATIONS_DEST})
INSTALL (FILES poshukusettings.xml DESTINATION ${LC_SETTINGS_DEST})
INSTALL (DIRECTORY installed/poshuku/ DESTINATION ${LC_INSTALLEDMANIFEST_DEST}/poshuku)

SET (POSHUKU_INCLUDE_DIR ${CURRENT_SOURCE_DIR})

OPTION (ENABLE_POSHUKU_CLEANWEB "Build content filter plugin for Poshuku browser" ON)
OPTION (ENABLE_POSHUKU_FATAPE "Build FatApe, support for GreaseMonkey userscripts" ON)
OPTION (ENABLE_POSHUKU_FILESCHEME "Build support of local file browsing for Poshuku browser" ON)
OPTION (ENABLE_POSHUKU_FUA "Build user agent customization plugin for Poshuku browser" ON)
OPTION (ENABLE_POSHUKU_KEYWORDS "Build support of url keywords for Poshuku browser" ON)
OPTION (ENABLE_POSHUKU_ONLINEBOOKMARKS "Build support of online bookmark synchronization for Poshuku browser" ON)
OPTION (ENABLE_POSHUKU_POGOOGLUE "Build support for searching selected text in Google" ON)

IF (ENABLE_POSHUKU_CLEANWEB)
	ADD_SUBDIRECTORY (plugins/cleanweb)
ENDIF (ENABLE_POSHUKU_CLEANWEB)
IF (ENABLE_POSHUKU_KEYWORDS)
	ADD_SUBDIRECTORY (plugins/keywords)
ENDIF (ENABLE_POSHUKU_KEYWORDS)
IF (ENABLE_POSHUKU_FATAPE)
	ADD_SUBDIRECTORY (plugins/fatape)
ENDIF (ENABLE_POSHUKU_FATAPE)
IF (ENABLE_POSHUKU_FUA)
	ADD_SUBDIRECTORY (plugins/fua)
ENDIF (ENABLE_POSHUKU_FUA)
IF (ENABLE_POSHUKU_FILESCHEME)
	ADD_SUBDIRECTORY (plugins/filescheme)
ENDIF (ENABLE_POSHUKU_FILESCHEME)
IF (ENABLE_POSHUKU_ONLINEBOOKMARKS)
	ADD_SUBDIRECTORY (plugins/onlinebookmarks)
ENDIF (ENABLE_POSHUKU_ONLINEBOOKMARKS)
IF (ENABLE_POSHUKU_POGOOGLUE)
	ADD_SUBDIRECTORY (plugins/pogooglue)
ENDIF (ENABLE_POSHUKU_POGOOGLUE)
