/***************************************************************************
    qgsauthmaplargemethod.h
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

#ifndef QGSAUTHMAPLARGEMETHOD_H
#define QGSAUTHMAPLARGEMETHOD_H

#include <QObject>
#include <QMutex>

#include "qgsauthconfig.h
#include "qgsauthmethod.h


class QgsAuthMapLargeMethod : public QgsAuthMethod
{
    Q_OBJECT

  public:
    explicit QgsAuthMapLargeMethod();

    // QgsAuthMethod interface
    QString key() const override;

    QString description() const override;

    QString displayDescription() const override;

    bool updateNetworkRequest( QNetworkRequest &request, const QString &authcfg,
                               const QString &dataprovider = QString() ) override;

    bool updateDataSourceUriItems( QStringList &connectionItems, const QString &authcfg,
                                   const QString &dataprovider = QString() ) override;


    bool updateNetworkProxy( QNetworkProxy &proxy, const QString &authcfg,
                             const QString &dataprovider = QString() ) override;

    void clearCachedConfig( const QString &authcfg ) override;

    void updateMethodConfig( QgsAuthMethodConfig &mconfig ) override;

  private:
    QgsAuthMethodConfig getMethodConfig( const QString &authcfg, bool fullconfig = true );

    void putMethodConfig( const QString &authcfg, const QgsAuthMethodConfig &mconfig );

    void removeMethodConfig( const QString &authcfg );

    QString escapeUserPass( const QString &val, QChar delim = '\'' ) const;

    static QMap<QString, QgsAuthMethodConfig> sAuthConfigCache;

};

#endif // QGSAUTHMAPLARGEMETHOD_H
