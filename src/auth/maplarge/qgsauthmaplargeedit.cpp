/***************************************************************************
    qgsauthmaplargeedit.cpp
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

#include "qgsauthmaplargeedit.h"
#include "ui_qgsauthmaplargeedit.h"


QgsAuthMapLargeEdit::QgsAuthMapLargeEdit( QWidget *parent )
  : QgsAuthMethodEdit( parent )
{
  setupUi( this );
  connect( leUsername, &QLineEdit::textChanged, this, &QgsAuthMapLargeEdit::leUsername_textChanged );
  connect( chkPasswordShow, &QCheckBox::stateChanged, this, &QgsAuthMapLargeEdit::chkPasswordShow_stateChanged );
}

bool QgsAuthMapLargeEdit::validateConfig()
{
  bool curvalid = !leUsername->text().isEmpty();
  if ( mValid != curvalid )
  {
    mValid = curvalid;
    emit validityChanged( curvalid );
  }
  return curvalid;
}

QgsStringMap QgsAuthMapLargeEdit::configMap() const
{
  QgsStringMap config;
  config.insert( QStringLiteral( "username" ), leUsername->text() );
  config.insert( QStringLiteral( "password" ), lePassword->text() );

  return config;
}

void QgsAuthMapLargeEdit::loadConfig( const QgsStringMap &configmap )
{
  clearConfig();

  mConfigMap = configmap;
  leUsername->setText( configmap.value( QStringLiteral( "username" ) ) );
  lePassword->setText( configmap.value( QStringLiteral( "password" ) ) );

  validateConfig();
}

void QgsAuthMapLargeEdit::resetConfig()
{
  loadConfig( mConfigMap );
}

void QgsAuthMapLargeEdit::clearConfig()
{
  leUsername->clear();
  lePassword->clear();
  leRealm->clear();
  chkPasswordShow->setChecked( false );
}

void QgsAuthMapLargeEdit::leUsername_textChanged( const QString &txt )
{
  Q_UNUSED( txt )
  validateConfig();
}

void QgsAuthBasicEdit::chkPasswordShow_stateChanged( int state )
{
  lePassword->setEchoMode( ( state > 0 ) ? QLineEdit::Normal : QLineEdit::Password );
}
