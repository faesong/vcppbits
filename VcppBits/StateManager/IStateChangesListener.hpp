// Copyright 2015-2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>
//
// This file is part of CppBits.
//
// CppBits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CppBits is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppBits.  If not, see <http://www.gnu.org/licenses/>.


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
