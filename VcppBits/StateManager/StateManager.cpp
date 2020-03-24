// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

// Copyright 2015-2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>
//
// This file is part of CppBits.
//
// CppBits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CppBits Top is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppBits.  If not, see <http://www.gnu.org/licenses/>.


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
