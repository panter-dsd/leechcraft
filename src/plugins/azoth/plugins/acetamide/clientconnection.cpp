/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2010  Oleg Linkin
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

#include "clientconnection.h"
#include <QTextCodec>
#include <plugininterface/util.h>
#include <interfaces/iprotocol.h>
#include <interfaces/iproxyobject.h>
#include "channelclentry.h"
#include "channelhandler.h"
#include "core.h"
#include "ircprotocol.h"
#include "ircserverclentry.h"
#include "ircserverhandler.h"

namespace LeechCraft
{
namespace Azoth
{
namespace Acetamide
{
	ClientConnection::ClientConnection (IrcAccount *account)
	: Account_ (account)
	, ProxyObject_ (0)
	{
		QObject *proxyObj = qobject_cast<IrcProtocol*> (account->
				GetParentProtocol ())->GetProxyObject ();
		ProxyObject_ = qobject_cast<IProxyObject*> (proxyObj);

		connect (this,
				SIGNAL (gotCLItems (const QList<QObject*>&)),
				Account_,
				SIGNAL (gotCLItems (const QList<QObject*>&)));
	}

	QObject* ClientConnection::GetCLEntry (const QString& id,
			const QString& nickname) const
	{
		if (ServerHandlers_.contains (id) && nickname.isEmpty ())
			return ServerHandlers_ [id]->GetCLEntry ();
		else if (!nickname.isEmpty ())
			return ServerHandlers_ [id]->GetParticipantEntry (nickname)
					.get ();
		else
			Q_FOREACH (IrcServerHandler *ish, ServerHandlers_.values ())
				if (ish->IsChannelExists (id))
					return ish->GetChannelHandler (id)->GetCLEntry ();

	}

	QList<QObject*> ClientConnection::GetCLEntries () const
	{
		QList<QObject*> result;
		Q_FOREACH (IrcServerHandler *ish, ServerHandlers_)
		{
			result << ish->GetCLEntry ();
			Q_FOREACH (ChannelHandler *ch, ish->GetChannelHandlers ())
			{

			}
		}
		return result;
	}

	void ClientConnection::Sinchronize ()
	{
	}

	IrcAccount* ClientConnection::GetAccount () const
	{
		return Account_;
	}

	bool ClientConnection::IsServerExists (const QString& key)
	{
		return ServerHandlers_.contains (key);
	}

	IrcServerCLEntry*
			ClientConnection::JoinServer (const ServerOptions& server)
	{
		QString serverId = server.ServerName_ + ":" +
				QString::number (server.ServerPort_);

		IrcServerHandler *ish = new IrcServerHandler (server, Account_);
		ServerHandlers_ [serverId] = ish;
		if (ish->ConnectToServer ())
			if (Account_->GetState ().State_ == SOffline)
				Account_->
						ChangeState (EntryStatus (SOnline, QString ()));
		return ish->GetCLEntry ();
	}

	ChannelCLEntry*
			ClientConnection::JoinChannel (const ServerOptions& server,
					const ChannelOptions& channel)
	{
		QString serverId = server.ServerName_ + ":" +
				QString::number (server.ServerPort_);
		QString channelId = channel.ChannelName_ + "@" +
				channel.ServerName_;

		if (ServerHandlers_ [serverId]->IsChannelExists (channelId))
		{
			Entity e = Util::MakeNotification ("Azoth",
				tr ("This server is already joined."),
				PCritical_);
			Core::Instance ().SendEntity (e);
			return 0;
		}

		if (!ServerHandlers_ [serverId]->JoinChannel (channel))
		{
			Entity e = Util::MakeNotification ("Azoth",
					tr ("Unable to join the channel."),
					PCritical_);
			Core::Instance ().SendEntity (e);
			return 0;
		}
		ChannelCLEntry *cle = ServerHandlers_ [serverId]->
				GetChannelHandler (channelId)->GetCLEntry ();

		if (!cle)
			return 0;

		emit gotCLItems (QList<QObject*> () << cle);

	}

	IrcServerHandler*
			ClientConnection::GetIrcServerHandler (const QString& id)
	{
		return ServerHandlers_ [id];
	}


};
};
};