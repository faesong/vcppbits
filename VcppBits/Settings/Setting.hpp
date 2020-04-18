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


#ifndef VcppBits_SETTING_HPP_INCLUDED__
#define VcppBits_SETTING_HPP_INCLUDED__

#include <string>
#include <vector>
#include <functional>
#include <variant>

namespace VcppBits {

class Setting;

namespace detail {
template <typename T> T get (const Setting& pSetting);
template <typename T> void set (Setting& pSetting, const T& pNewVal);

} // namespace detail

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

    explicit Setting (int* ILLEGAL_CONSTRUCTOR = nullptr,
                      int* STUB_FOR_STD_CONTAINERS_DO_NOT_USE_ME = nullptr);
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
            setInt(_defaultValue.intVal);
            break;
        case FLOATINGPOINT:
            setFloat(_defaultValue.floatVal);
            break;
        default:
            throw;
        }
    }

    bool isDefault () {
        switch (this->valuetype) {
        case BOOLEAN:
            return _defaultValue.boolVal == _value.boolVal;
        case STRING:
            return stringDefaultVal == stringVal;
        case INTEGER:
            return _defaultValue.intVal == _value.intVal;
        case FLOATINGPOINT:
            return _defaultValue.floatVal == _value.floatVal;
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

    bool getValue (bool) const {
        return getBool();
    }

    int getValue (int) const {
        return getInt();
    }

    float getValue (float) const {
        return getFloat();
    }

    std::string getValue (std::string) const {
        return getString();
    }


    // THIS IS BROKEN IN MSVC
    template <typename T> T getValue () const {
        return detail::get<T>(*this);
    }
    template <typename T> void setValue (const T &new_val) {
        detail::set<T>(*this, new_val);
    }

    void setString (const std::string &new_val);
    const std::string &getString () const;
    std::vector<std::string> getPossibleStrings () const;
    std::string getDefaultString () const;
    size_t getStringPos () const;

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
    size_t stringPos;
    typedef std::pair<void *, std::function<void()>> SettingListener;
    std::vector<SettingListener> listeners;
};

class SettingUpdater {
public:
    SettingUpdater (Setting &pSetting, bool *pPtr)
        : _setting (pSetting),
          _ptr (pPtr) {
        subscribeToSettingUpdate();
    }
    SettingUpdater (Setting &pSetting, int *pPtr)
        : _setting (pSetting),
          _ptr (pPtr) {
        subscribeToSettingUpdate();
    }
    SettingUpdater (Setting &pSetting, float *pPtr)
        : _setting (pSetting),
          _ptr (pPtr) {
        subscribeToSettingUpdate();
    }
    SettingUpdater (Setting &pSetting, std::string *pPtr)
        : _setting (pSetting),
          _ptr (pPtr) {
        subscribeToSettingUpdate();
    }

    ~SettingUpdater () {
        _setting.removeUpdateHandler(this);
    }

    // TODO avoid all these
    SettingUpdater (const SettingUpdater& pOther)
      : _setting (pOther._setting),
        _ptr (pOther._ptr) {
        subscribeToSettingUpdate();
    }
    SettingUpdater& operator= (const SettingUpdater&) {
        throw;
    }
    SettingUpdater (SettingUpdater&& pOther)
        : _setting(pOther._setting) {
        throw;
    }
    SettingUpdater& operator= (SettingUpdater&&) {
        throw;
    }

private:
    void subscribeToSettingUpdate () {
        auto listener_func = std::bind(&SettingUpdater::onSettingUpdate, this);
        _setting.addUpdateHandler(this, listener_func);
        onSettingUpdate(); // initialize the value immediately
    }
    void onSettingUpdate () {
        switch (_setting.getValueType()) {
        case Setting::BOOLEAN:
            *std::get<bool*>(_ptr) = _setting.getBool();
            return;
        case Setting::INTEGER:
            *std::get<int*>(_ptr) = _setting.getInt();
            return;
        case Setting::FLOATINGPOINT:
            *std::get<float*>(_ptr) = _setting.getFloat();
            return;
        case Setting::STRING:
            *std::get<std::string*>(_ptr) = _setting.getString();
            return;
        default:
            return;
        }
    }

    Setting &_setting;
    std::variant<bool*, int*, float*, std::string*> _ptr;
};

} // namespace VcppBits

#endif // VcppBits_SETTING_HPP_INCLUDED__

