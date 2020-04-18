// The MIT License (MIT)

// Copyright 2015-2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef VcppBits_I_STATE_CHANGES_LISTENER_HPP_INCLUDED__
#define VcppBits_I_STATE_CHANGES_LISTENER_HPP_INCLUDED__

#include "VcppBits/StateManager/IState.hpp"

namespace VcppBits {

enum class StateEventType { FREEZE, LOAD, UNLOAD, RESUME };

namespace StateEventTypeUtils {
inline std::string toString(const StateEventType pEvent) {
    switch (pEvent) {
    case StateEventType::FREEZE:
        return "freezing";
    case StateEventType::LOAD:
        return "loading";
    case StateEventType::UNLOAD:
        return "unloading";
    case StateEventType::RESUME:
        return "resuming";
    };
    return "<unknown state>";
}
}

class IStateChangesListener {
public:
    virtual void processStateManagerEvent (const StateEventType,
                                           IState* pState) = 0;
    virtual ~IStateChangesListener() {
    }
};

} // namespace VcppBits

#endif // VcppBits_I_STATE_CHANGES_LISTENER_HPP_INCLUDED__
