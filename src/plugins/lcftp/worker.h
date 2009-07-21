#ifndef PLUGINS_LCFTP_WORKER_H
#define PLUGINS_LCFTP_WORKER_H
#include <boost/shared_ptr.hpp>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QUrl>
#include <QDateTime>
#include <curl/curl.h>
#include <plugininterface/guarded.h>

namespace LeechCraft
{
	namespace Plugins
	{
		namespace LCFTP
		{
			typedef boost::shared_ptr<CURL> CURL_ptr;

			struct Wrapper;

			struct TaskData
			{
				int ID_;
				QUrl URL_;
				QString Filename_;
			};

			struct FetchedEntry
			{
				QUrl URL_;
				QString OriginalLocalPath_;
				quint64 Size_;
				QDateTime DateTime_;
				QString User_;
				QString Group_;
			};

			class Worker : public QThread
			{
				Q_OBJECT

				friend struct Wrapper;
				int ID_;
				boost::shared_ptr<QFile> File_;
				boost::shared_ptr<QBuffer> ListBuffer_;
				Util::Guarded<QUrl> URL_;
				Util::Guarded<bool> Exit_;
				Util::Guarded<bool> IsWorking_;
				Util::Guarded<quint64> DLNow_,
					DLTotal_,
					ULNow_,
					ULTotal_,
					InitialSize_;
				Util::Guarded<QDateTime> StartDT_;
			public:
				struct TaskState
				{
					int WorkID_;
					bool IsWorking_;
					QUrl URL_;
					QPair<quint64, quint64> DL_;
					QPair<quint64, quint64> UL_;
					quint64 DLSpeed_;
					quint64 ULSpeed_;
				};

				Worker (int, QObject* = 0);
				virtual ~Worker ();

				bool IsWorking () const;
				void SetExit ();
				TaskState GetState () const;
				QPair<quint64, quint64> GetDL () const;
				QPair<quint64, quint64> GetUL () const;
				QUrl GetURL () const;
			protected:
				void run ();
			private:
				void HandleTask (const TaskData&, CURL_ptr);
				void ParseBuffer ();
				void Reset ();
				size_t WriteData (void*, size_t, size_t);
				size_t ListDir (void*, size_t, size_t);
				int Progress (double, double, double, double);
			signals:
				void error (const QString&);
				void finished (const TaskData&);
			};

			typedef boost::shared_ptr<Worker> Worker_ptr;
		};
	};
};

#endif

