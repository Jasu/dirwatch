#include "platform_watcher.h"

#include "../../directory_watcher.h"
#include "../../exception/exception.h"

namespace dirwatch
{
  namespace detail
  {
    platform_watcher::platform_watcher(directory_watcher * watcher)
      : _directory_watcher(watcher)
    {
      boost::filesystem::path p = watcher->get_path();
      _directory_basic = CreateFileW(
        p.wstring().c_str(),
        GENERIC_READ,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
        );
      if (_directory_basic == INVALID_HANDLE_VALUE)
      {
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not open directory for basic."));
      }
      _directory_attributes = CreateFileW(
        p.wstring().c_str(),
        GENERIC_READ,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
        );
      if (_directory_attributes == INVALID_HANDLE_VALUE)
      {
		CloseHandle(_directory_basic);
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not open directory for attributes."));
      }
      _ol_event_basic = CreateEvent(NULL, FALSE, FALSE, "dirwatch_ol_event_basic");
      if (!_ol_event_basic)
      {
        CloseHandle(_directory_basic);
        CloseHandle(_directory_attributes);
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not create basic event."));
      }
      _ol_event_attributes = CreateEvent(NULL, FALSE, FALSE, "dirwatch_ol_event_attributes");
      if (!_ol_event_attributes)
      {
        CloseHandle(_directory_basic);
        CloseHandle(_directory_attributes);
        CloseHandle(_ol_event_basic);
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not create attributes event."));
      }
    }

    platform_watcher::~platform_watcher()
    {
      CloseHandle(_directory_basic);
      CloseHandle(_directory_attributes);
      CloseHandle(_ol_event_basic);
      CloseHandle(_ol_event_attributes);
    }

    void platform_watcher::watch()
    {
      FILE_NOTIFY_INFORMATION *fni_basic = (FILE_NOTIFY_INFORMATION*)_aligned_malloc(16*(sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH), sizeof(DWORD)), 
								*fni_attributes = (FILE_NOTIFY_INFORMATION*)_aligned_malloc(16*(sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH), sizeof(DWORD));
      ZeroMemory(fni_basic, sizeof(fni_basic)+MAX_PATH);
      ZeroMemory(fni_attributes, sizeof(fni_attributes)+MAX_PATH);

	  OVERLAPPED ol_basic, ol_attributes;
      ZeroMemory(&ol_basic, sizeof(ol_basic));
      ZeroMemory(&ol_attributes, sizeof(ol_basic));      
	  ol_basic.hEvent = _ol_event_basic;
      ol_attributes.hEvent = _ol_event_attributes;

      try
      {
        //FILE_NOTIFY_INFORMATION does not distinguish between attributes changes and content changes.
        //Have two ReadDirectoryChanges simultaneously, one for real changes and another for attributes.
        read_directory_changes_basic(fni_basic, &ol_basic);
        read_directory_changes_attributes(fni_attributes, &ol_attributes);

        HANDLE handles[2] = {_ol_event_basic, _ol_event_attributes};
        for(;;)
        {
          DWORD wait_result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
          switch (wait_result)
          {
            case WAIT_ABANDONED_0:
            case WAIT_ABANDONED_0 + 1:
              BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Wait abandoned."));
            case WAIT_TIMEOUT:
              continue;
            case WAIT_FAILED:
              BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Wait failed."));
            case WAIT_OBJECT_0:
            case WAIT_OBJECT_0 + 1:
              //Don't trust the result - multiple objects may be signalled during a single call.
              if (HasOverlappedIoCompleted(&ol_basic))
              {
                DWORD bytes;
                if (GetOverlappedResult(_directory_basic, &ol_basic, &bytes, FALSE))
                {
                  handle_directory_changes_basic(fni_basic);
                  read_directory_changes_basic(fni_basic, &ol_basic);
                }
                else
                {
                  BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("GetOverlappedResult failed for ol_basic."));
                }
              }
              if (HasOverlappedIoCompleted(&ol_attributes))
              {
                DWORD bytes;
                if (GetOverlappedResult(_directory_attributes, &ol_attributes, &bytes, FALSE))
                {
                  handle_directory_changes_attributes(fni_attributes);
                  read_directory_changes_attributes(fni_attributes, &ol_attributes);
                }
                else
                {
                  BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("GetOverlappedResult failed for ol_attributes."));
                }
              }
              break;
            default:
              BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Unknown return value from WaitForMultipleObjects."));
          }
        }
      }
      catch(...)
      {		 
        CancelIoEx(_directory_basic, &ol_basic);
        CancelIoEx(_directory_attributes, &ol_attributes);
		delete fni_basic;
		delete fni_attributes;
        throw;
      }
	_aligned_free(fni_basic);
	_aligned_free(fni_attributes);
    }

    void platform_watcher::handle_directory_changes_basic(FILE_NOTIFY_INFORMATION * fni)
    {
      for (;;)
      {
        boost::filesystem::path p(std::wstring(fni->FileName, fni->FileNameLength / sizeof(wchar_t)));
        switch (fni->Action)
        {
          case FILE_ACTION_ADDED:
          case FILE_ACTION_RENAMED_NEW_NAME:
            _directory_watcher->on_node_created(p);
            break;
          case FILE_ACTION_REMOVED:
          case FILE_ACTION_RENAMED_OLD_NAME:
            _directory_watcher->on_node_deleted(p);
            break;
          case FILE_ACTION_MODIFIED:
            _directory_watcher->on_file_contents_changed(p);
            break;
        }
		if (!fni->NextEntryOffset)
			return;
        fni = (FILE_NOTIFY_INFORMATION*)((char*)fni + fni->NextEntryOffset);
	  }
    }

    void platform_watcher::handle_directory_changes_attributes(FILE_NOTIFY_INFORMATION * fni)
    {
      for(;;) 
      {
        boost::filesystem::path p(std::wstring(fni->FileName, fni->FileNameLength / sizeof(wchar_t)));
        switch (fni->Action)
        {
          case FILE_ACTION_MODIFIED:
            _directory_watcher->on_node_attributes_changed(p);
            break;
          case FILE_ACTION_ADDED:
          case FILE_ACTION_RENAMED_NEW_NAME:
          case FILE_ACTION_REMOVED:
          case FILE_ACTION_RENAMED_OLD_NAME:
            BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Unexpected action for attribute changes."));
        }
		if (!fni->NextEntryOffset)
			return;
        fni = (FILE_NOTIFY_INFORMATION*)((char*)fni + fni->NextEntryOffset);
      };
    }

    void platform_watcher::read_directory_changes_basic(FILE_NOTIFY_INFORMATION * fni, OVERLAPPED * ol)
    {
      if(!ReadDirectoryChangesW(
          _directory_basic,
          fni,
          16*(sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH),
          TRUE,
          //CHANGE_FILE_NAME includes deletion
          FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
          NULL,
          ol,
          NULL
          ))
      {
		DWORD error = GetLastError();
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not ReadDirectoryChangesW for basic file changes."));
      }
    }

    void platform_watcher::read_directory_changes_attributes(FILE_NOTIFY_INFORMATION * fni, OVERLAPPED * ol)
    {
      if (!ReadDirectoryChangesW(
          _directory_attributes,
          fni,
          16*(sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH),
          TRUE,
          FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SECURITY,
          NULL,
          ol,
          NULL
          ))
      {
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not ReadDirectoryChangesW for attribute file changes."));
      }
    }
  }
}

