CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (leechcraft_lmp)
INCLUDE (InitLCPlugin OPTIONAL)

SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

IF (NOT WIN32)
	FIND_PACKAGE (Phonon)
ENDIF (NOT WIN32)

IF (WIN32)
	MESSAGE (STATUS "Using Qt's Phonon on Windows")
	SET (QT_USE_PHONON TRUE)
ELSE (WIN32)
	IF (NOT PHONON_FOUND)
		MESSAGE (WARNING "KDE Phonon not found, trying Qt's one instead...")
		SET (QT_USE_PHONON TRUE)
	ELSE (NOT PHONON_FOUND)
		MESSAGE (STATUS "Found KDE Phonon! Woohoo!")
		SET (QT_PHONON_INCLUDES ${PHONON_INCLUDES})
		SET (QT_PHONON_LIBS ${PHONON_LIBS})
		ADD_DEFINITIONS (-DHAVE_KDE_PHONON)
	ENDIF (NOT PHONON_FOUND)
ENDIF (WIN32)

FIND_PACKAGE (Taglib REQUIRED)
ADD_DEFINITIONS (${TAGLIB_CFLAGS})

SET (QT_USE_QTDECLARATIVE TRUE)
SET (QT_USE_QTNETWORK TRUE)
SET (QT_USE_QTSQL TRUE)
SET (QT_USE_QTXML TRUE)
INCLUDE (${QT_USE_FILE})
IF (NOT PHONON_FOUND)
	MESSAGE (FATAL_ERROR "No Phonon is found, LMP cannot be built")
ENDIF (NOT PHONON_FOUND)

INCLUDE_DIRECTORIES (
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${Boost_INCLUDE_DIR}
	${QT_PHONON_INCLUDES}
	${LEECHCRAFT_INCLUDE_DIR}
	)
SET (SRCS
	lmp.cpp
	xmlsettingsmanager.cpp
	playertab.cpp
	player.cpp
	core.cpp
	mediainfo.cpp
	localfileresolver.cpp
	playlistdelegate.cpp
	localcollection.cpp
	localcollectionstorage.cpp
	util.cpp
	collectiontypes.cpp
	collectiondelegate.cpp
	nowplayingwidget.cpp
	staticplaylistmanager.cpp
	playlistmanager.cpp
	artistsinfodisplay.cpp
	albumartmanager.cpp
	lmpsystemtrayicon.cpp
	fsbrowserwidget.cpp
	fsmodel.cpp
	rootpathsettingsmanager.cpp
	localcollectionwatcher.cpp
	recommendationswidget.cpp
	radiowidget.cpp
	releaseswidget.cpp
	playlistundocommand.cpp
	biowidget.cpp
	biopropproxy.cpp
	audiopropswidget.cpp
	playlistwidget.cpp
	aalabeleventfilter.cpp
	collectionsortermodel.cpp
	collectionstatsdialog.cpp
	eventswidget.cpp
	plmanagerwidget.cpp
	nowplayingpixmaphandler.cpp
	sync/cloudwidget.cpp
	sync/clouduploadmanager.cpp
	sync/clouduploader.cpp
	sync/copymanager.cpp
	sync/devicesbrowserwidget.cpp
	sync/syncmanagerbase.cpp
	sync/syncmanager.cpp
	sync/transcodejob.cpp
	sync/transcodemanager.cpp
	sync/transcodingparamswidget.cpp
	sync/uploadmodel.cpp
	playlistparsers/commonpl.cpp
	playlistparsers/playlistfactory.cpp
	playlistparsers/m3u.cpp
	playlistparsers/pls.cpp
	playlistparsers/xspf.cpp
	)
SET (FORMS
	playertab.ui
	nowplayingwidget.ui
	fsbrowserwidget.ui
	recommendationswidget.ui
	radiowidget.ui
	releaseswidget.ui
	biowidget.ui
	audiopropswidget.ui
	playlistwidget.ui
	collectionstatsdialog.ui
	eventswidget.ui
	plmanagerwidget.ui
	sync/cloudwidget.ui
	sync/devicesbrowserwidget.ui
	sync/transcodingparamswidget.ui
	)
SET (RESOURCES resources.qrc)
CreateTrVars ("lmp" "en;es;ru_RU" TRANSLATIONS COMPILED_TRANSLATIONS)
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCCS ${RESOURCES})

OPTION (ENABLE_LMP_MPRIS "Enable MPRIS support for LMP" TRUE)

ADD_CUSTOM_COMMAND (OUTPUT
	${COMPILED_TRANSLATIONS}
	COMMAND "${QT_LRELEASE_EXECUTABLE}" ${TRANSLATIONS}
	DEPENDS ${TRANSLATIONS}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
ADD_LIBRARY (leechcraft_lmp SHARED
	${COMPILED_TRANSLATIONS}
	${SRCS}
	${UIS_H}
	${RCCS}
	)
IF (ENABLE_LMP_MPRIS)
	ADD_SUBDIRECTORY (mpris)
	ADD_DEFINITIONS (-DENABLE_MPRIS)
ENDIF (ENABLE_LMP_MPRIS)
TARGET_LINK_LIBRARIES (leechcraft_lmp
	${QT_LIBRARIES}
	${LEECHCRAFT_LIBRARIES}
	${QT_PHONON_LIBS}
	${TAGLIB_LIBRARIES}

	${MPRIS_SUBLIB}
	)
INSTALL (TARGETS leechcraft_lmp DESTINATION ${LC_PLUGINS_DEST})
INSTALL (FILES ${COMPILED_TRANSLATIONS} DESTINATION ${LC_TRANSLATIONS_DEST})
INSTALL (FILES lmpsettings.xml DESTINATION ${LC_SETTINGS_DEST})
INSTALL (DIRECTORY interfaces DESTINATION include/leechcraft)

OPTION (ENABLE_LMP_DUMBSYNC "Enable DumbSync, plugin for syncing with Flash-like media players" ON)
OPTION (ENABLE_LMP_MP3TUNES "Enable plugin for syncing and using the mp3tunes.com service" ON)

IF (ENABLE_LMP_DUMBSYNC)
	ADD_SUBDIRECTORY (plugins/dumbsync)
ENDIF (ENABLE_LMP_DUMBSYNC)
IF (ENABLE_LMP_MP3TUNES)
	ADD_SUBDIRECTORY (plugins/mp3tunes)
ENDIF (ENABLE_LMP_MP3TUNES)
