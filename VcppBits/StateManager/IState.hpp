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


#ifndef VcppBits_I_STATE_HPP_INCLUDED__
#define VcppBits_I_STATE_HPP_INCLUDED__


#include <string>


namespace VcppBits {

class IState {
public:
    virtual ~IState () {}
    virtual void load () = 0;
    virtual void resume () {}
    virtual void freeze (IState* /*pPushedState*/) {} 
    virtual void unload () = 0;
    virtual std::string getName () const = 0;
};

} // namespace VcppBits

#endif // VcppBits_I_STATE_HPP_INCLUDED__
