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
