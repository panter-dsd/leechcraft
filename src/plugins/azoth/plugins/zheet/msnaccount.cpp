/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2011  Georg Rudoy
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

#include "msnaccount.h"
#include <msn/notificationserver.h>
#include <util/util.h>
#include <interfaces/iproxyobject.h>
#include "msnprotocol.h"
#include "callbacks.h"
#include "core.h"
#include "msnaccountconfigwidget.h"

namespace LeechCraft
{
namespace Azoth
{
namespace Zheet
{
	namespace ZheetUtil
	{
		std::string ToStd (const QString& str)
		{
			return str.toUtf8 ().constData ();
		}

		QString FromStd (const std::string& str)
		{
			return QString::fromUtf8 (str.c_str ());
		}

		MSN::BuddyStatus ToMSNState (State st)
		{
			switch (st)
			{
			case SOnline:
			case SChat:
				return MSN::STATUS_AVAILABLE;
			case SAway:
				return MSN::STATUS_AWAY;
			case SXA:
				return MSN::STATUS_IDLE;
			case SDND:
				return MSN::STATUS_BUSY;
			case SInvisible:
			default:
				return MSN::STATUS_INVISIBLE;
			}
		}
	}

	MSNAccount::MSNAccount (const QString& name, MSNProtocol *parent)
	: QObject (parent)
	, Proto_ (parent)
	, Name_ (name)
	, Server_ ("messenger.hotmail.com")
	, Port_ (1863)
	, CB_ (new Callbacks (this))
	, Conn_ (0)
	, Connecting_ (false)
	{
		connect (CB_,
				SIGNAL (finishedConnecting ()),
				this,
				SLOT (handleConnected ()));
	}

	void MSNAccount::Init ()
	{
		const QString& pass = Core::Instance ().GetPluginProxy ()->GetAccountPassword (this);
		Conn_ = new MSN::NotificationServerConnection (Passport_,
				pass.toUtf8 ().constData (), *CB_);
		CB_->SetNotificationServerConnection (Conn_);
	}

	QByteArray MSNAccount::Serialize () const
	{
		quint16 version = 1;

		QByteArray result;
		{
			QDataStream ostr (&result, QIODevice::WriteOnly);
			ostr << version
				<< Name_
				<< ZheetUtil::FromStd (Passport_)
				<< Server_
				<< Port_;
		}

		return result;
	}

	MSNAccount* MSNAccount::Deserialize (const QByteArray& data, MSNProtocol *parent)
	{
		quint16 version = 0;

		QDataStream in (data);
		in >> version;

		if (version != 1)
		{
			qWarning () << Q_FUNC_INFO
					<< "unknown version"
					<< version;
			return 0;
		}

		QString name;
		in >> name;

		QString passport;

		MSNAccount *result = new MSNAccount (name, parent);
		in >> passport
			>> result->Server_
			>> result->Port_;
		result->Passport_ = ZheetUtil::ToStd (passport);
		result->Init ();

		return result;
	}

	void MSNAccount::FillConfig (MSNAccountConfigWidget *w)
	{
		Passport_ = ZheetUtil::ToStd (w->GetID ());

		const QString& pass = w->GetPassword ();
		if (!pass.isEmpty ())
			Core::Instance ().GetPluginProxy ()->SetPassword (pass, this);
	}

	QObject* MSNAccount::GetObject ()
	{
		return this;
	}

	QObject* MSNAccount::GetParentProtocol () const
	{
		return Proto_;
	}

	IAccount::AccountFeatures MSNAccount::GetAccountFeatures () const
	{
		return FRenamable | FSupportsXA | FHasConfigurationDialog;
	}

	QList<QObject*> MSNAccount::GetCLEntries ()
	{
		return QList<QObject*> ();
	}

	QString MSNAccount::GetAccountName () const
	{
		return Name_;
	}

	QString MSNAccount::GetOurNick () const
	{
		return ZheetUtil::FromStd (Passport_);
	}

	void MSNAccount::RenameAccount (const QString& name)
	{
		Name_ = name;
	}

	QByteArray MSNAccount::GetAccountID () const
	{
		return "Azoth.msn.libmsn." + ZheetUtil::FromStd (Passport_).toUtf8 ();
	}

	QList<QAction*> MSNAccount::GetActions () const
	{
		return QList<QAction*> ();
	}

	void MSNAccount::QueryInfo (const QString&)
	{
	}

	void MSNAccount::OpenConfigurationDialog ()
	{
	}

	EntryStatus MSNAccount::GetState () const
	{
		return EntryStatus ();
	}

	void MSNAccount::ChangeState (const EntryStatus& status)
	{
		if (!Conn_)
		{
			qWarning () << Q_FUNC_INFO
					<< "null Conn_";
			return;
		}

		if (status.State_ == SOffline)
			Conn_->disconnect ();
		else if (!Conn_->isConnected ())
		{
			if (!Connecting_)
			{
				Conn_->connect (ZheetUtil::ToStd (Server_), Port_);
				Connecting_ = true;
			}
			PendingStatus_ = status;
		}
		else
		{
			uint cid = 0;
			cid += MSN::MSNC1;
			cid += MSN::MSNC2;
			cid += MSN::MSNC3;
			cid += MSN::MSNC4;
			cid += MSN::MSNC5;
			cid += MSN::MSNC6;
			cid += MSN::MSNC7;
			cid += MSN::SupportMultiPacketMessaging;

			Conn_->setState (ZheetUtil::ToMSNState (status.State_), cid);

			MSN::personalInfo info;
			info.PSM = ZheetUtil::ToStd (status.StatusString_);
			Conn_->setPersonalStatus (info);
		}
	}

	void MSNAccount::Synchronize ()
	{
	}

	void MSNAccount::Authorize (QObject*)
	{
	}

	void MSNAccount::DenyAuth (QObject*)
	{
	}

	void MSNAccount::RequestAuth (const QString&, const QString&, const QString&, const QStringList&)
	{
	}

	void MSNAccount::RemoveEntry (QObject*)
	{
	}

	QObject* MSNAccount::GetTransferManager () const
	{
		return 0;
	}

	void MSNAccount::handleConnected ()
	{
		Connecting_ = false;
		ChangeState (PendingStatus_);
	}
}
}
}