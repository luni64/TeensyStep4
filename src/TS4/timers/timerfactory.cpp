#include "timerfactory.h"
#include "../ErrorHandling/error_handler.h"
#include <vector>

namespace TS4
{
    namespace // private
    {
        std::vector<ITimerModule*> modules;

        unsigned freeTimers = 0;
    }

    namespace TimerFactory
    {
        void attachModule(ITimerModule* module)
        {
            modules.push_back(module);
            freeTimers += module->getFreeChannels();
        }

        ITimer* makeTimer()
        {
            if (modules.empty()) postError(errorCode::noModule);

            for (ITimerModule* m : modules)
            {
                ITimer* timer = m->getChannel();
                if (timer != nullptr)
                {
                    freeTimers--;
                    return timer;
                }
            }
            return nullptr;
        }

        void returnTimer(ITimer* timer)
        {
            modules[0]->releaseChannel(timer); /// FIX
            freeTimers++;
        }

        bool hasFreeTimer()
        {
            return freeTimers > 0;
        }
    }
}