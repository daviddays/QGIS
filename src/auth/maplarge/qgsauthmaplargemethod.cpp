/***************************************************************************
    qgsauthmaplargemethod.cpp
    ---------------------
    begin                : October 8, 2020
    copyright            : (C) 2020 by MapLarge, USA
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

#include "qgsauthmaplargemethod.h"
#include "qgsauthmaplargeedit.h"

#include "qgsauthmanager.h"
#include "qgslogger.h"
#include "qgsapplication.h"

#include <QNetworkProxy>
#include <QMutexLocker>
#include <QUuid>

static const QString AUTH_METHOD_KEY = QStringLiteral( "MapLarge" );
static const QString AUTH_METHOD_DESCRIPTION = QStringLiteral( "Authentication for MapLarge Deployments" );

QMap<QString, QgsAuthMethodConfig> QgsAuthBasicMethod::sAuthConfigCache = QMap<QString, QgsAuthMethodConfig>();


QgsAuthMapLargeMethod::QgsAuthMapLargeMethod()
{
  setVersion( 2 );
  setExpansions( QgsAuthMethod::NetworkRequest | QgsAuthMethod::DataSourceUri );
  setDataProviders( QStringList()
                    << QStringLiteral( "wfs" )
                    << QStringLiteral( "wmts" )
                    << QStringLiteral( "wms" ) );

}

QString QgsAuthMapLargeMethod::key() const
{
  return AUTH_METHOD_KEY;
}

QString QgsAuthMapLargeMethod::description() const
{
  return AUTH_METHOD_DESCRIPTION;
}

QString QgsAuthMapLargeMethod::displayDescription() const
{
  return tr( "Login authentcation against MapLarge deployments" );
}

bool QgsAuthMapLargeMethod::updateNetworkRequest( QNetworkRequest &request, const QString &authcfg,
    const QString &dataprovider )
{
  Q_UNUSED( dataprovider )
  QgsAuthMethodConfig mconfig = getMethodConfig( authcfg );
  if ( !mconfig.isValid() )
  {
    QgsDebugMsg( QStringLiteral( "Update request config FAILED for authcfg: %1: config invalid" ).arg( authcfg ) );
    return false;
  }

  QString username = mconfig.config( QStringLiteral( "username" ) );
  QString password = mconfig.config( QStringLiteral( "password" ) );

  if ( !username.isEmpty() )
  {
    request.setRawHeader( "Authorization", "Basic " + QStringLiteral( "%1:%2" ).arg( username, password ).toLatin1().toBase64() );
  }
  return true;
}

bool QgsAuthMapLargeMethod::updateDataSourceUriItems( QStringList &connectionItems, const QString &authcfg,
    const QString &dataprovider )
{
  Q_UNUSED( dataprovider )
  QMutexLocker locker( &mMutex );
  QgsAuthMethodConfig mconfig = getMethodConfig( authcfg );
  if ( !mconfig.isValid() )
  {
    QgsDebugMsg( QStringLiteral( "Update URI items FAILED for authcfg: %1: basic config invalid" ).arg( authcfg ) );
    return false;
  }

  QString username = mconfig.config( QStringLiteral( "username" ) );
  QString password = mconfig.config( QStringLiteral( "password" ) );

  if ( username.isEmpty() )
  {
    QgsDebugMsg( QStringLiteral( "Update URI items FAILED for authcfg: %1: username empty" ).arg( authcfg ) );
    return false;
  }


  QString userparam = "mluser:'" + escapeUserPass( username );
  connectionItems.append( userparam );

  QString passparam = "mlpass:" + escapeUserPass( password );
  QString authInfo = "ainfo="+userparam+";"+passparam;
  connectionItems.append( authInfo );

  return true;
}

bool QgsAuthMapLargeMethod::updateNetworkProxy( QNetworkProxy &proxy, const QString &authcfg, const QString &dataprovider )
{
  Q_UNUSED( dataprovider )
  QMutexLocker locker( &mMutex );

  QgsAuthMethodConfig mconfig = getMethodConfig( authcfg );
  if ( !mconfig.isValid() )
  {
    QgsDebugMsg( QStringLiteral( "Update proxy config FAILED for authcfg: %1: config invalid" ).arg( authcfg ) );
    return false;
  }

  QString username = mconfig.config( QStringLiteral( "username" ) );
  QString password = mconfig.config( QStringLiteral( "password" ) );

  if ( !username.isEmpty() )
  {
    proxy.setUser( username );
    proxy.setPassword( password );
  }
  return true;
}

void QgsAuthMapLargeMethod::updateMethodConfig( QgsAuthMethodConfig &mconfig )
{
  QMutexLocker locker( &mMutex );
  if ( mconfig.hasConfig( QStringLiteral( "oldconfigstyle" ) ) )
  {
    QgsDebugMsg( QStringLiteral( "Updating old style auth method config" ) );

    QStringList conflist = mconfig.config( QStringLiteral( "oldconfigstyle" ) ).split( QStringLiteral( "|||" ) );
    mconfig.setConfig( QStringLiteral( "realm" ), conflist.at( 0 ) );
    mconfig.setConfig( QStringLiteral( "username" ), conflist.at( 1 ) );
    mconfig.setConfig( QStringLiteral( "password" ), conflist.at( 2 ) );
    mconfig.removeConfig( QStringLiteral( "oldconfigstyle" ) );
  }

  // TODO: add updates as method version() increases due to config storage changes
}

void QgsAuthMapLargeMethod::clearCachedConfig( const QString &authcfg )
{
  removeMethodConfig( authcfg );
}

QgsAuthMethodConfig QgsAuthMapLargeMethod::getMethodConfig( const QString &authcfg, bool fullconfig )
{
  QMutexLocker locker( &mMutex );
  QgsAuthMethodConfig mconfig;

  // check if it is cached
  if ( sAuthConfigCache.contains( authcfg ) )
  {
    mconfig = sAuthConfigCache.value( authcfg );
    QgsDebugMsg( QStringLiteral( "Retrieved config for authcfg: %1" ).arg( authcfg ) );
    return mconfig;
  }

  // else build basic bundle
  if ( !QgsApplication::authManager()->loadAuthenticationConfig( authcfg, mconfig, fullconfig ) )
  {
    QgsDebugMsg( QStringLiteral( "Retrieve config FAILED for authcfg: %1" ).arg( authcfg ) );
    return QgsAuthMethodConfig();
  }

  // cache bundle
  putMethodConfig( authcfg, mconfig );

  return mconfig;
}

void QgsAuthMapLargeMethod::putMethodConfig( const QString &authcfg, const QgsAuthMethodConfig &mconfig )
{
  QMutexLocker locker( &mMutex );
  QgsDebugMsg( QStringLiteral( "Putting basic config for authcfg: %1" ).arg( authcfg ) );
  sAuthConfigCache.insert( authcfg, mconfig );
}

void QgsAuthMapLargeMethod::removeMethodConfig( const QString &authcfg )
{
  QMutexLocker locker( &mMutex );
  if ( sAuthConfigCache.contains( authcfg ) )
  {
    sAuthConfigCache.remove( authcfg );
    QgsDebugMsg( QStringLiteral( "Removed basic config for authcfg: %1" ).arg( authcfg ) );
  }
}

QString QgsAuthMapLargeMethod::escapeUserPass( const QString &val, QChar delim ) const
{
  QString escaped = val;

  escaped.replace( '\\', QLatin1String( "\\\\" ) );
  escaped.replace( delim, QStringLiteral( "\\%1" ).arg( delim ) );

  return escaped;
}

//////////////////////////////////////////////
// Plugin externals
//////////////////////////////////////////////

/**
 * Required class factory to return a pointer to a newly created object
 */
QGISEXTERN QgsAuthMapLargeMethod *classFactory()
{
  return new QgsAuthMapLargeMethod();
}

/**
 * Required key function (used to map the plugin to a data store type)
 */
QGISEXTERN QString authMethodKey()
{
  return AUTH_METHOD_KEY;
}

/**
 * Required description function
 */
QGISEXTERN QString description()
{
  return AUTH_METHOD_DESCRIPTION;
}

/**
 * Required isAuthMethod function. Used to determine if this shared library
 * is an authentication method plugin
 */
QGISEXTERN bool isAuthMethod()
{
  return true;
}

/**
 * Optional class factory to return a pointer to a newly created edit widget
 */
QGISEXTERN QgsAuthMapLargeEdit *editWidget( QWidget *parent )
{
  return new QgsAuthMapLargeEdit( parent );
}

/**
 * Required cleanup function
 */
QGISEXTERN void cleanupAuthMethod() // pass QgsAuthMethod *method, then delete method  ?
{
}
