#include "timerfactory.h"
#include <vector>

namespace TS4
{
    namespace // private
    {
        std::vector<ITimerModule*> modules;
    }

    namespace TimerFactory
    {
        void attachModule(ITimerModule* module)
        {
            modules.push_back(module);
        }

        ITimer* makeTimer()
        {
            for (ITimerModule* m : modules)
            {
                ITimer* timer = m->getChannel();
                if (timer != nullptr) return timer;
            }
            return nullptr;
        }

        void returnTimer(ITimer* timer)
        {
            modules[0]->releaseChannel(timer);  /// FIX
        }
    }
}