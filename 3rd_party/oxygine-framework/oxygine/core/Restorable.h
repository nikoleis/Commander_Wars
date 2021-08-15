#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/closure/closure.h"
#include <qmutex.h>
#include <qvector.h>

namespace oxygine
{
    class Restorable : public IClosureOwner
    {
    public:
        using RestoreCallback = OwnedClosure<void,Restorable*>;
        using restorable = QVector<Restorable*>;
        explicit Restorable();
        virtual ~Restorable();
        static const restorable& getObjects();
        static void restoreAll();
        static void releaseAll();
        static bool isRestored();
        virtual Restorable* _getRestorableObject() = 0;
        virtual void release() = 0;
        void restore();
        void reg(RestoreCallback cb);
        void unreg();

    private:
        restorable::const_iterator findRestorable(Restorable* r);
        //non copyable
        Restorable(const Restorable&);
        const Restorable& operator=(const Restorable&);

    private:
        RestoreCallback m_cb;
        bool m_registered;

        static QMutex m_mutex;
        static restorable m_restorable;
    };
}
