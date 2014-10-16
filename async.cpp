#include <thread>
#include <iostream>
#include <fstream>
#include <cstring>
#include <functional>
using namespace std;
using namespace std::placeholders;


typedef function<void(int, int, void*, void*)> ReadFileCallback;

void ReadFile (char *fname, ReadFileCallback pCallback, void *context)
{
    ifstream file (fname, ios::in | ios::ate);
    streampos begin, end;
    char *buf;
    int sz;

    if (!file.is_open ()) {
        pCallback (0, 0, nullptr, context);
        return;
    }

    end = file.tellg ();
    file.seekg (0, ios::beg);
    begin = file.tellg ();
    sz = end - begin;

    buf = new char[sz];
    file.read (buf, sz);
    file.close ();
    pCallback (1, sz, buf, context);
}

void ReadFileAsync (char *fname, ReadFileCallback pCallback, void *context)
{
    std::thread t (ReadFile, fname, pCallback, context);
    t.join ();
}


typedef function<void*(void*)> TaskFunc;

class Task {

public:
    Task (TaskFunc f)
        :vNext(nullptr)
    {
        vFunc = f;
    }

virtual  ~Task () { }

    void Run (void *pArgs)
    {
        void *res = vFunc (pArgs);
        if (vNext) vNext->Run (res);
    }

    void AndThen (Task *t)
    {
        vNext = t;
    }


private:
    TaskFunc   vFunc;
    Task      *vNext;
};

class ConfigMgr {

public:
    ConfigMgr (char *fname)
        : vName(fname) {}

    void Init (Task *pAndThen)
    {
        nextTask = pAndThen;
        ReadFileAsync (vName, AfterReadFile, this);
    }

    char* GetValue (char *key)
    {
        if (strcmp (key, "SVRS") == 0) return svrs;
        if (strcmp (key, "Join") == 0) return join;
        return "";
    }

private:

    static void AfterReadFile (int status, int size, void *buf, void *pCfgMgr)
    {
        ConfigMgr *cfgmgr = (ConfigMgr*) pCfgMgr;
        if (status == 0) {
            // Failed
        } else {
            cfgmgr->ParseFile ((char*) buf);
            cfgmgr->nextTask->Run (pCfgMgr);
        }
    }

    char* SearchValue (char *buf, char *key)
    {
            char* idx, *idx1;
            char *b;

        if ((idx = strstr (buf, key)) == nullptr) return nullptr;
        idx += strlen(key)+1;
        if ((idx1 = strstr (idx, "\n")) == nullptr) return nullptr;

        b = new char[(idx1-idx)+1];
        memcpy (b, idx, (idx1-idx));
        b[(idx1-idx)] = 0;
        return b;
    }

    void ParseFile (char *buf)
    {
        int idx, idx1;
        svrs = SearchValue (buf, "SVRS");
        join = SearchValue (buf, "JOIN");
    }

    char *vName;
    char *svrs;
    char *join;
    Task *nextTask;
};


class Activity_TSVCInit {

private:
    void DoReadStateFile (int status, int size, void *buf, void *context)
    {
        if (status == 0) {
            cout << "Failed to read state file!" << endl;
        } else {
            cout << "Read state file: ";
            for (int i = 0; i < size-1; i++) cout << ((char*)buf)[i];
        }
        cout << endl;
    }

    void* ReadStateFile (void *pRes)
    {
        ReadFileAsync ("tacs.state", std::bind (&Activity_TSVCInit::DoReadStateFile, this, _1,_2,_3,_4), nullptr);
        return nullptr;
    }

public:

    void Run ()
    {
        ConfigMgr cfg("config.ini");

        Task t([&cfg] (void *r) -> void* { std::cout << cfg.GetValue ("SVRS") << " " << cfg.GetValue ("Join") << endl; return nullptr; });
        Task t2(std::bind (&Activity_TSVCInit::ReadStateFile, this, _1));

        t.AndThen (&t2);

        cfg.Init (&t);
    }

};

int main ()
{
    Activity_TSVCInit a;
    a.Run ();
    return 0;
}
