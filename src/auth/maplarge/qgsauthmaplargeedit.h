/***************************************************************************
    qgsauthmaplargeedit.h
    ---------------------
    begin                : October 8, 2020
    copyright            : (C) 2020 by MapLarge USA
    author               : David Days
    email                : david dot days at maplarge dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSAUTHMAPLARGEEDIT_H
#define QGSAUTHMAPLARGEEDIT_H

#include <QWidget>
#include "qgsauthmethodedit.h"
#include "ui_qgsauthmaplargeedit.h"

#include "qgsauthconfig.h"


class QgsAuthMapLargeEdit : public QgsAuthMethodEdit, private Ui::QgsAuthMapLargeEdit
{
    Q_OBJECT

  public:
    explicit QgsAuthMapLargeEdit( QWidget *parent = nullptr );

    bool validateConfig() override;

    QgsStringMap configMap() const override;

  public slots:
    void loadConfig( const QgsStringMap &configmap ) override;

    void resetConfig() override;

    void clearConfig() override;

  private slots:
    void leUsername_textChanged( const QString &txt );

    void chkPasswordShow_stateChanged( int state );

  private:
    QgsStringMap mConfigMap;
    bool mValid = false;
};

#endif // QGSAUTHMAPLARGEEDIT_H
