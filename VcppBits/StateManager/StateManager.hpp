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


#ifndef VcppBits_STATE_MANAGER_HPP_INCLUDED__
#define VcppBits_STATE_MANAGER_HPP_INCLUDED__


#include <string>
#include <deque>

#include "VcppBits/StateManager/IState.hpp"
#include "VcppBits/StateManager/IStateChangesListener.hpp"

namespace VcppBits {

// typical state lifetame goes like this:
//
// load() --> state is running now
//
// freeze() --> some other state is loaded, and this one is therefore suspended
// for now
//
// resume() --> a state which froze us now unloaded, therefore we are running
// again
//
// unload() --> we have been unloaded, and might never be loaded again

class StateManager {
public:
    //StateManager ();
    //~StateManager ();

    void pushState (IState *pState);
    void popState ();
    //    void popAllStates ();

    IState *currentState () {
        return _states.back();
    }

    // To be called every frame by top-level app manager
    // returning false means stack got empty
    bool update ();

    void setListener (IStateChangesListener *pListener) {
        _listener = pListener;
    }

    IState* getFrozenState () {
        if (_states.size() > 1) {
            return _states[_states.size() - 1];
        }
        return nullptr;
    }


private:

    void notifyListener (const StateEventType pEventType,
                         IState* pState) {
        if (_listener) {
            _listener->processStateManagerEvent(pEventType, pState);
        }
    }
    void _doPushState ();

    std::deque<IState *> _states;
    bool _topIsFrozen = false;

    IState *_stateToPush = nullptr;
    bool _popRequested = false;

    IStateChangesListener *_listener = nullptr;
};

} // namespace VcppBits

#endif // VcppBits_STATE_MANAGER_HPP_INCLUDED__
