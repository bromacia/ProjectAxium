#ifndef __WEBSERVER_H
#define __WEBSERVER_H

class WebServer : public ACE_Based::Runnable
{
    public:
        void run();
};
#endif
