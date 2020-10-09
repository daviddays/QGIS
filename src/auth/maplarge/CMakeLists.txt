SET(AUTH_MAPLARGE_SRCS
        qgsauthmaplargemethod.cpp
        qgsauthmaplargeedit.cpp
        )

SET(AUTH_MAPLARGE_HDRS
        qgsauthmaplargeedit.h
        qgsauthmaplargemethod.h
        )

SET(AUTH_MAPLARGE_UIS qgsauthmaplargeedit.ui)

INCLUDE_DIRECTORIES (
        ../../core
        ../../core/auth
        ../../core/geometry
        ../../core/metadata
        ${CMAKE_BINARY_DIR}/src/core
        ${CMAKE_BINARY_DIR}/src/gui
)
INCLUDE_DIRECTORIES (SYSTEM
        ${QCA_INCLUDE_DIR}
        ${QTKEYCHAIN_INCLUDE_DIR}
        )
INCLUDE_DIRECTORIES (
        ../../gui
        ../../gui/auth
        ${CMAKE_CURRENT_BINARY_DIR}
)

QT5_WRAP_UI (AUTH_MAPLARGE_UIS_H ${AUTH_MAPLARGE_UIS})

ADD_LIBRARY (maplargeauthmethod MODULE ${AUTH_MAPLARGE_SRCS} ${AUTH_MAPLARGE_HDRS} ${AUTH_MAPLARGE_UIS_H})

TARGET_LINK_LIBRARIES (maplargeauthmethod
        qgis_core
        qgis_gui
        )
TARGET_COMPILE_DEFINITIONS(maplargeauthmethod PRIVATE "-DQT_NO_FOREACH")

INSTALL(TARGETS maplargeauthmethod
        RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
        LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})