/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2012  Georg Rudoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#ifndef PLUGINS_NETSTOREMANAGER_PLUGINS_YANDEXDISK_YANDEXDISK_H
#define PLUGINS_NETSTOREMANAGER_PLUGINS_YANDEXDISK_YANDEXDISK_H
#include <memory>
#include <QObject>
#include <interfaces/iinfo.h>
#include <interfaces/iplugin2.h>
#include <interfaces/netstoremanager/istorageplugin.h>

namespace LeechCraft
{
namespace NetStoreManager
{
namespace YandexDisk
{
	class Account;
	typedef std::shared_ptr<Account> Account_ptr;

	class Plugin : public QObject
				 , public IInfo
				 , public IPlugin2
				 , public IStoragePlugin
	{
		Q_OBJECT
		Q_INTERFACES (IInfo
				IPlugin2
				LeechCraft::NetStoreManager::IStoragePlugin);

		QList<Account_ptr> Accounts_;
	public:
		void Init (ICoreProxy_ptr);
		void SecondInit ();
		void Release ();
		QByteArray GetUniqueID () const;
		QString GetName () const;
		QString GetInfo () const;
		QIcon GetIcon () const;

		QSet<QByteArray> GetPluginClasses () const;

		QObject* GetObject ();
		QString GetStorageName () const;
		QIcon GetStorageIcon () const;
		void RegisterAccount (const QString&);
		QObjectList GetAccounts () const;
		void RemoveAccount (QObject*);
	private:
		void ReadAccounts ();
		void WriteAccounts () const;
	public slots:
		void initPlugin (QObject*);
	signals:
		void gotEntity (const LeechCraft::Entity&);
		void delegateEntity (const LeechCraft::Entity&, int*, QObject**);
		void accountAdded (QObject*);
		void accountRemoved (QObject*);
	};
}
}
}

#endif
