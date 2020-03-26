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


#ifndef VcppBits_SETTING_HPP_INCLUDED__
#define VcppBits_SETTING_HPP_INCLUDED__

#include <string>
#include <vector>
#include <functional>

namespace VcppBits {

class Setting;

class Setting {
public:
    enum TYPE { S_BOOL,
                S_INTEGER, S_INTEGER_BOUNDED,
                S_FLOATINGPOINT, S_FLOATINGPOINT_BOUNDED,
                S_STRING, S_STRING_ONE_OF };
    enum VALUETYPE { BOOLEAN,
                     INTEGER,
                     FLOATINGPOINT,
                     STRING };

    explicit Setting (const std::string &name = std::string("nonexistent"));
    Setting (const std::string &name,
             const bool default_val);
    Setting (const std::string &name,
             const int default_val);
    Setting (const std::string &name,
             const int down_bound,
             const int up_bound,
             const int default_val);
    Setting (const std::string &name,
             const float default_val);
  
    Setting (const std::string &name,
             const float down_bound,
             const float up_bound,
             const float default_val);
    Setting (const std::string &name,
             const std::string &default_val = std::string());
    Setting (const std::string &name,
             const std::vector<std::string> &possible_vals,
             const std::string &default_val);

    Setting& setDescription (const std::string &descr);
    std::string getDescription () const;

    Setting& setLongName (const std::string &long_name);
    std::string getLongName () const;

    TYPE getType () const;
    VALUETYPE getValueType () const;

    std::string getName () const;

    void setBool (const bool new_val);
    bool getBool () const;
    bool getBoolDefault () const;

    void setInt (const int new_val);
    int getInt () const;
    int getIntDefault () const;
    int getIntDown () const;
    int getIntUp () const;

    void setFloat (const float new_val);
    float getFloat () const;
    float getFloatDefault () const;
    float getFloatDown () const;
    float getFloatUp () const;

    void resetToDefault () {
        switch (this->valuetype) {
        case BOOLEAN:
            setBool(_defaultValue.boolVal);
            break;
        case STRING:
            setString(stringDefaultVal);
            break;
        case INTEGER:
            setFloat(_defaultValue.intVal);
            break;
        case FLOATINGPOINT:
            setFloat(_defaultValue.floatVal);
            break;
        default:
            throw;
        }
    }

    int* getIntPtr (); // Note: if you want the listeners to be notified, you
                       // must call triggerChangeEvent() manually on change
                       // Note: upper/lower limits are (obviously) not enforce
                       // that way

                       // call setInt(getInt()) to to enforce limits and trigger
                       // events at the same time
    float* getFloatPtr ();
    bool* getBoolPtr ();

    // whacky hacks to make templated get() function work
    float getValue (float) const {
        return getFloat();
    }

    int getValue (int) const {
        return getInt();
    }

    std::string getValue (std::string) const {
        return getString();
    }

    bool getValue (bool) const {
        return getBool();
    }

    void setString (const std::string &new_val);
    std::string getString () const;
    std::vector<std::string> getPossibleStrings () const;
    std::string getDefaultString () const;
    std::iterator_traits<std::vector<std::string>::iterator>::difference_type
    getStringPos () const;

    void setByString (const std::string &new_val);
    std::string getAsString () const;

    void addUpdateHandler (void *listener_id, std::function<void()> function);
    void removeUpdateHandler (const void *listener_id);

    void triggerChangeEvent () { onChangeEvent(); }


private:
    void onChangeEvent ();
     TYPE type;
     VALUETYPE valuetype;
     std::string name;
    std::string longName;
    std::string description;

    union UnionType {
        bool boolVal;
        int intVal;
        float floatVal;
        UnionType (bool pVal) : boolVal(pVal) {}
        UnionType (int pVal) : intVal(pVal)   {}
        UnionType (float pVal) : floatVal(pVal) {}
		UnionType () : boolVal (false) {}
    };

    union UnionNumericType {
        int intVal;
        float floatVal;
        UnionNumericType (int pVal) : intVal(pVal) {}
        UnionNumericType (float pVal) : floatVal(pVal) {}
        UnionNumericType () : intVal(0) {}
    };
    
    UnionType _value;
    UnionType _defaultValue;
    UnionNumericType _upperValue;
    UnionNumericType _lowerValue;

    std::string stringVal;
    std::string stringDefaultVal;
    std::vector<std::string> stringPossibleVals;
    std::iterator_traits<std::vector<std::string>::iterator>::difference_type stringPos;
    typedef std::pair<void *, std::function<void()>> SettingListener;
    std::vector<SettingListener> listeners;
};

} // namespace VcppBits

#endif // VcppBits_SETTING_HPP_INCLUDED__

