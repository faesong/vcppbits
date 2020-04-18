// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


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


#include  "VcppBits/StateManager/StateManager.hpp"


#include <stdexcept>


namespace VcppBits {

void StateManager::pushState (IState *pState) {
    if (_stateToPush) {
        throw std::runtime_error("StateManager::pushState(): can only push one "
                                 "state before next StateManager::update()");
    }
    _stateToPush = pState;
}

void StateManager::popState () {
    if (_popRequested) {
        throw std::runtime_error("StateManager::popState(): can only pop one "
                                 "state before next StateManager::update()");
    }
    _popRequested = true;
}

void StateManager::_doPushState () {
    _states.push_back(_stateToPush);
    notifyListener(StateEventType::LOAD, _stateToPush);

    _stateToPush->load();
    _stateToPush = nullptr;
    _topIsFrozen = false;
}

bool StateManager::update () {
    //if (_statesToPush.empty()) && !_popRequested && !_popAllRequested
    if (_stateToPush) {
        if (_popRequested) { throw 200; }
        if (_states.size()) {
            if (_topIsFrozen) {
                _doPushState();
            } else {
                notifyListener(StateEventType::FREEZE, _states.back());
                _states.back()->freeze(_stateToPush);
                _topIsFrozen = true;
            }
        } else {
            _doPushState();
        }
    } else if (_popRequested) {
        notifyListener(StateEventType::UNLOAD, _states.back());
        _states.back()->unload();
        _states.pop_back();
        _popRequested = false;
        if (_states.size()) {
            _topIsFrozen = true;
        } else {
            return false;
        }
    }

    if (_topIsFrozen && !_stateToPush) {
        notifyListener(StateEventType::RESUME, _states.back());
        _states.back()->resume();
        _topIsFrozen = false;
    }

    return true;
}

} // namespace VcppBits
