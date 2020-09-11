// This file is part of VSTGUI. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "../../cfileselector.h"
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <memory>
#include <cerrno>
extern "C" { extern char **environ; }

//------------------------------------------------------------------------
namespace VSTGUI {
namespace X11 {

static constexpr auto zenitypath = "/usr/bin/zenity";

//------------------------------------------------------------------------
struct FileSelector : CNewFileSelector
{
	FileSelector (CFrame* parent, Style style) : CNewFileSelector (parent), style (style)
	{
	}

	~FileSelector ()
	{
        closeProcess ();
	}

	bool runInternal (CBaseObject* delegate) override
	{
		this->delegate = delegate;
		return runZenity ();
	}

	void cancelInternal () override { closeProcess (); }

	bool runModalInternal () override
	{
		if (runInternal (nullptr))
		{
			std::string path;
			path.reserve(1024);

			ssize_t count;
			char buffer[1024];
			while ((count = read (readerFd, buffer, sizeof (buffer))) > 0 ||
				   (count == -1 && errno == EINTR))
			{
				if (count > 0)
					path.append (buffer, count);
			}

			if (count != -1)
			{
				if (! path.empty () && path[0] == '/')
				{
					if (path.back () == '\n')
						path.pop_back ();
					result.emplace_back (path);
				}
			}
		}
		return !result.empty ();
	}

private:
	bool runZenity ()
	{
		std::vector<std::string> args;

		args.push_back (zenitypath);
		args.push_back ("--file-selection");

		if (style == Style::kSelectDirectory)
			args.push_back ("--directory");
		else if (style == Style::kSelectSaveFile)
		{
			args.push_back ("--save");
			args.push_back ("--confirm-overwrite");
		}
		if (!title.empty ())
			args.push_back ("--title=" + title.getString ());
		if (!initialPath.empty ())
			args.push_back ("--filename=" + initialPath.getString ());

		std::vector<char*> argv (args.size () + 1);
		for (size_t i = 0, n = args.size (); i < n; ++i)
			argv[i] = const_cast<char*>(args[i].c_str ());

		if (startProcess (argv.data()))
		{
			return true;
		}
		return false;
	}

	bool startProcess (char* argv[])
	{
		closeProcess();

		SpawnAttrPtr attr { new posix_spawnattr_t };
		if (posix_spawnattr_init (attr.get ()) != 0)
		{
			delete attr.release ();
			return false;
		}

		SpawnActionsPtr actions { new posix_spawn_file_actions_t };
		if (posix_spawn_file_actions_init (actions.get ()) != 0)
		{
			delete actions.release ();
			return false;
		}

		struct PipePair
		{
			int fd[2] = { -1, -1 };
			~PipePair ()
			{
				if (fd[0] != -1) close (fd[0]);
				if (fd[1] != -1) close (fd[1]);
			}
		};

		PipePair rw;
		if (pipe (rw.fd) != 0)
			return false;

#if 0
		char** envp = environ;
#else
		std::vector<char*> cleanEnviron;
		cleanEnviron.reserve (256);
		for (char** envp = environ; *envp; ++envp)
		{
			// ensure the process will link with system libraries,
			// and not these from the Ardour bundle.
			if (strncmp (*envp, "LD_LIBRARY_PATH=", 16) == 0)
				continue;
			cleanEnviron.push_back (*envp);
		}
		cleanEnviron.push_back(nullptr);
		char** envp = cleanEnviron.data ();
#endif

		if (posix_spawnattr_setflags(attr.get (), POSIX_SPAWN_USEVFORK) != 0 ||
			posix_spawn_file_actions_adddup2 (actions.get (), rw.fd[1], STDOUT_FILENO) != 0)
		{
			return false;
		}

		if (posix_spawn(&spawnPid, zenitypath, actions.get (), attr.get (), argv, envp) != 0)
			return false;

		close (rw.fd[1]);
		rw.fd[1] = -1;
		readerFd = rw.fd[0];
		rw.fd[0] = -1;

		return true;
	}

	void closeProcess ()
	{
		if (spawnPid != -1)
		{
			if (waitpid (spawnPid, nullptr, WNOHANG) == 0)
			{
				kill (spawnPid, SIGTERM);
				waitpid (spawnPid, nullptr, 0);
			}
			spawnPid = -1;
		}

		if (readerFd != -1)
		{
			close (readerFd);
			readerFd = -1;
		}
	}

	pid_t spawnPid = -1;
	int readerFd = -1;

	Style style;
	SharedPointer<CBaseObject> delegate;

	struct PosixSpawnDeleter {
	public:
		void operator()(posix_spawnattr_t* attr)
		{
			posix_spawnattr_destroy(attr);
			delete attr;
		}
		void operator()(posix_spawn_file_actions_t* actions)
		{
			posix_spawn_file_actions_destroy(actions);
			delete actions;
		}
	};

	typedef std::unique_ptr<posix_spawnattr_t, PosixSpawnDeleter> SpawnAttrPtr;
	typedef std::unique_ptr<posix_spawn_file_actions_t, PosixSpawnDeleter> SpawnActionsPtr;
};

//------------------------------------------------------------------------
} // X11

//------------------------------------------------------------------------
CNewFileSelector* CNewFileSelector::create (CFrame* parent, Style style)
{
	return new X11::FileSelector (parent, style);
}

//------------------------------------------------------------------------
} // VSTGUI
