#include <iostream>
#include "ihomeserver.h"
#include "ihomesession.h"
#include "sconfig.h"
#include "slog.h"
#include<signal.h>
using namespace std;

SConfig config;
SLog log;


int main()
{

    IHomeServer server;
     server.InitServer();

     signal(SIGPIPE,SIG_IGN);
     signal(SIGINT,SIG_IGN);
     signal(SIGHUP,SIG_IGN);


    return 0;
}

