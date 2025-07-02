#include <cstddef>    // size_t

#if defined(_WIN32) || defined(_WIN64)
#    include <Windows.h>
#    include <psapi.h>
#    include <tlhelp32.h>
#elif defined(__APPLE__)
#    include <mach/mach.h>
#    include <mach/task_info.h>
#    include <libproc.h>
#    include <unistd.h>
#else
#    include <cstdio>     // sscanf
#    include <cstring>    // strncmp
#    include <algorithm>  // all_of
#    include <fstream>    // ifstream
#    include <string>     // getline, string
#    include <dirent.h>   // DIR, opendir, readdir
#    include <unistd.h>   // getpid
#endif

using std::size_t;

size_t GetResidentMemory(void) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    PROCESS_MEMORY_COUNTERS info;
    if ( 0 != ::GetProcessMemoryInfo( ::GetCurrentProcess(), &info, sizeof info ) ) return info.WorkingSetSize;
#elif defined(__APPLE__)
    task_basic_info info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    if ( KERN_SUCCESS == ::task_info( ::mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count ) ) return info.resident_size;
#else // Linux
    std::ifstream file("/proc/self/status");
    if ( file.is_open() )
    {
        std::string line;
        while ( std::getline(file, line) )
        {
            if ( 0u == line.rfind("VmRSS:", 0u) )
            {
                size_t kb = 0u;
                std::sscanf(line.c_str(), "VmRSS: %zu kB", &kb);
                return kb * 1024u;
            }
        }
    }
#endif
    return 0u;
}

size_t GetThreadCount(void) noexcept
{
    size_t count = 0u;
#if defined(_WIN32) || defined(_WIN64)
    DWORD pid = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if ( snapshot != INVALID_HANDLE_VALUE )
    {
        THREADENTRY32 entry = {};
        entry.dwSize = sizeof(entry);
        if ( Thread32First(snapshot, &entry) )
        {
            do
            {
                if ( entry.th32OwnerProcessID != pid ) continue;
                ++count;
            } while ( Thread32Next(snapshot, &entry) );
        }
        CloseHandle(snapshot);
    }
#elif defined(__APPLE__)
    task_t task = mach_task_self();
    thread_act_array_t thread_list;
    mach_msg_type_number_t thread_count = 0;
    if ( KERN_SUCCESS == task_threads(task, &thread_list, &thread_count) )
    {
        for ( mach_msg_type_number_t i = 0; i < thread_count; ++i ) mach_port_deallocate( mach_task_self(), thread_list[i] );
        vm_deallocate(mach_task_self(), (vm_address_t)thread_list, thread_count * sizeof(thread_t));
        count = thread_count;
    }
#else // Linux
    std::ifstream file("/proc/self/status");
    if ( file.is_open() )
    {
        std::string line;
        while ( std::getline(file, line) )
        {
            if ( 0u == line.rfind("Threads:", 0u) )
            {
                size_t n = 0u;
                if ( 1 != std::sscanf(line.c_str(), "Threads:%zu", &n) ) continue;
                return n;
            }
        }
    }
#endif
    return count;
}

size_t GetChildProcessCount(void) noexcept
{
    size_t count = 0u;

#if defined(_WIN32) || defined(_WIN64)
    DWORD parent_pid = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if ( snapshot != INVALID_HANDLE_VALUE )
    {
        PROCESSENTRY32 entry = {};
        entry.dwSize = sizeof(entry);
        if ( Process32First(snapshot, &entry) )
        {
            do
            {
                if ( entry.th32ParentProcessID != parent_pid ) continue;
                ++count;
            } while ( Process32Next(snapshot, &entry) );
        }
        CloseHandle(snapshot);
    }
#elif defined(__APPLE__)
    pid_t self_pid = getpid();
    static thread_local int pids[2048];
    int n = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    for ( int i = 0; i < n / sizeof(pid_t); ++i )
    {
        if ( pids[i] <= 0 || pids[i] == self_pid ) continue;
        struct proc_bsdinfo info = {};
        if ( sizeof(info) == proc_pidinfo(pids[i], PROC_PIDTBSDINFO, 0, &info, sizeof(info)) )
        {
            if ( info.pbi_ppid != self_pid ) continue;
            ++count;
        }
    }
#else // Linux
    pid_t self = getpid();
    DIR *dir = opendir("/proc");
    if ( dir )
    {
        struct dirent *entry;
        while ( (entry = readdir(dir)) )
        {
            if ( entry->d_type != DT_DIR ) continue;

            if ( !std::all_of(entry->d_name, entry->d_name + std::strlen(entry->d_name), ::isdigit) ) continue;
            pid_t pid = std::atoi(entry->d_name);
            if ( pid <= 0 || pid == self ) continue;

            std::string path = std::string("/proc/") + entry->d_name + "/status";
            std::ifstream file(path);
            if ( file.is_open() )
            {
                std::string line;
                while ( std::getline(file, line) )
                {
                    if ( line.rfind("PPid:", 0) == 0 )
                    {
                        pid_t ppid = 0;
                        std::sscanf(line.c_str(), "PPid:\t%d", &ppid);
                        if ( ppid != self ) continue;
                        ++count;
                        break;
                    }
                }
            }
        }
        closedir(dir);
    }
#endif

    return count;
}
