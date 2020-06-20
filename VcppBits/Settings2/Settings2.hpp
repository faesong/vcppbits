// The MIT License (MIT)

// Copyright 2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>

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


#pragma once


#include <iostream>
#include <variant>
#include <map>
#include <vector>
#include <functional>

#include "VcppBits/StringUtils/StringUtils.hpp"
#include "VcppBits/KeyFile/KeyFile.hpp"

namespace V2 {

using VcppBits::KeyFile;

template<typename T>
struct NoneConstraint {
    bool isValid (const T) const { return true; };
};


// TODO3: rename to FundamentalArithmeticConstraint, and note that it only works
// for fundamental type settings (the ones that are covered by
// std::numeric_limits)
template<typename T>
struct ArithmeticConstraint {
    using value_type = T;
    ArithmeticConstraint () {}
    ArithmeticConstraint (const T pMin,
                          const T pMax)
        : _min (pMin),
          _max (pMax) {}
    bool isValid (const T pValueToTest) const {
        return (pValueToTest >= _min) && (pValueToTest <= _max);
    }

    value_type _min = std::numeric_limits<value_type>::lowest();
    value_type _max = std::numeric_limits<value_type>::max();
};

// works for anything that has <= and >= operators defined,
// but does not have default-constructor
template<typename T>
struct GenericArithmeticConstraint {
    using value_type = T;
    GenericArithmeticConstraint (const T pMin,
                                 const T pMax)
        : _min (pMin),
          _max (pMax) {}
    bool isValid (const T pValueToTest) const {
        return (pValueToTest >= _min) && (pValueToTest <= _max);
    }

    T _min;
    T _max;
};


template<typename T>
struct EnumConstraint {
    using value_type = T;
    explicit EnumConstraint (const std::vector<T> &pValues)
        : _validValues (pValues) {
    }

    template <typename TT>
    bool
    isValid (const TT pValueToTest) const {
        return _getPos(pValueToTest) != _validValues.end();
    }

    template <typename TT>
    int getPosInt (TT pValueToFind) const {
        return std::distance(_validValues.cbegin(), _getPos(pValueToFind));
    }

    template <typename TT>
    typename std::vector<value_type>::const_iterator
    _getPos (const TT pValueToTest,
            typename std::enable_if<!std::is_floating_point<TT>::value>::type* dummy = nullptr) const {
        (void) dummy;
        return std::find(_validValues.cbegin(), _validValues.cend(), pValueToTest);;
    }

    template <typename TT>
    typename std::vector<value_type>::const_iterator
    _getPos (const TT pValueToTest,
             typename std::enable_if<std::is_floating_point<TT>::value>::type* dummy = nullptr) const {
        (void) dummy;
        return std::find_if(_validValues.cbegin(),
                            _validValues.cend(),
                            [&pValueToTest] (const TT pVal) {
                                return std::abs(pVal - pValueToTest)
                                    < std::numeric_limits<T>::epsilon();
                            });
    }

    std::vector<value_type> _validValues;
};


template<typename T,
         template <typename> class CT,
         std::string _toString(T),
         T _fromString(const std::string&)>
struct SettingValue {
    using value_type = T;
    using constraint_type = CT<T>;
    SettingValue (const T pValue, const CT<T> pConstraint = CT<T>{})
        : _val (pValue),
          _defaultVal (pValue),
          _constraint (pConstraint) {
        if (!pConstraint.isValid(pValue)) {
            // TODO: custom exception?
            throw std::runtime_error("value is out of constraint limits");
        }
    }
    operator value_type () const { return _val; }
    std::string getAsString () const { return _toString(_val); }
    bool setByString (const std::string pStr) {
        return setValue(_fromString(pStr));
    }

    const value_type& getValue () const {
        return _val;
    }

    bool setValue (const value_type pValue) {
        if (_constraint.isValid(pValue)) {
            _val = pValue;
            // TODO4: cleanup
            // if (_ptr) {
            //     *_ptr = pValue;
            // }
            return true;
        }
        return false;
    }

    const CT<T>& getConstraint () const {
        return _constraint;
    }

    // TODO cleanup
    // void setPtrToUpdate (value_type* pPtr) {
    //     _ptr = pPtr;
    //     if (*_ptr != _val) {
    //         *_ptr = _val;
    //     }
    // }


    // bool updateFromPtr () {
    //     return setValue(*_ptr);
    // }

    // value_type* getUpdatePtr () {
    //     return _ptr;
    // }

    value_type getDefaultValue () const {
        return _defaultVal;
    }

    // TODO10: this are not supposed to  be used?
    void addUpdateHandler (void *pListenerId,
                           std::function<void(const value_type&)> pFunction) {
        _listeners.push_back(std::make_pair(pListenerId, pFunction));
    }

    void removeUpdateHandler (const void *pListenerId) {
        // TODO5: is this the way to do it??
        typename std::vector<SettingListener>::iterator
            it = _listeners.begin(),
            end = _listeners.end();
        for (; it != end; ++it) {
            if (it->first == pListenerId) {
                _listeners.erase(it);
                return;
            }
        }
    }

    void onUpdate() {
        typename std::vector<SettingListener>::iterator
            it = _listeners.begin(),
            end = _listeners.end();
        int i = 0;
        for (; it != end; ++it) {
            it->second(_val);
            ++i;
        }
    }

    value_type *getIncomingPtr () {
        _incoming = _val;
        return &_incoming;
    }

    value_type *getRawIncomingPtr () {
        return &_incoming;
    }

private:
    value_type _val;
    value_type _incoming;
    // TODO: cleanup
    // value_type* _ptr = nullptr;
    const value_type _defaultVal;
    CT<T> _constraint;

    using SettingListener = std::pair<void *, std::function<void(const value_type&)>>;
    std::vector<SettingListener> _listeners;
};

namespace detail {

template <typename, typename>
constexpr bool is_one_of_variants_types = false;

template <typename... Ts, typename T>
constexpr bool is_one_of_variants_types<std::variant<Ts...>, T>
    = (std::is_same_v<T, Ts> || ...);

template<class T> struct always_false : std::false_type {};


} // namespace detail


template <typename EnumT, typename... Ts>
class SettingImpl {
public:
    using storage_type = std::variant<Ts...>;
    using Type = EnumT;

    template <typename T,
              typename DUMMY = typename std::enable_if<detail::is_one_of_variants_types<storage_type, T>>::type>
    explicit SettingImpl(T setting_value, DUMMY* dummy = nullptr)
        : _val (setting_value) {
        (void) dummy;
    }

    SettingImpl (const SettingImpl& pOther) = delete;
    SettingImpl (SettingImpl&& pOther) = delete;
    SettingImpl operator= (const SettingImpl& pOther) = delete;
    SettingImpl operator= (SettingImpl&& pOther) = delete;

    EnumT getType () {
        return EnumT(_val.index());
    }

    // will not expose altogether
    /*
    template <typename T>
    typename std::enable_if<detail::is_one_of_variants_types<storage_type, T>, T>::type&
    get () {
        return std::get<T>(_val);
    }
    */

    template<typename T>
    const typename T::value_type& get () const {
        return std::get<T>(_val).getValue();
    }

    template<typename T>
    void set (const typename T::value_type& pValue) {
        if(std::get<T>(_val).getValue() != pValue
           && std::get<T>(_val).setValue(pValue)) {
            std::get<T>(_val).onUpdate();
        }
    }

    template<typename T>
    void triggerListeners () {
        std::get<T>(_val).onUpdate();
    }

    void setByString (const std::string &pStr) {
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            // TODO: do we need this first if block?
            if constexpr (std::is_same_v<typename T::value_type, std::string>) {
                if(pStr != arg.getValue() && arg.setValue(pStr)) {
                    arg.onUpdate();
                }
            }
            else if constexpr (detail::is_one_of_variants_types<storage_type, T>) {
                if (arg.getAsString() != pStr && arg.setByString(pStr)) {
                    arg.onUpdate();
                }
            }
            else
                static_assert(detail::always_false<T>::value, "non-exhaustive visitor!");
        }, _val);
    }

    std::string getAsString () const {
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<typename T::value_type, std::string>)
                return arg.getValue();
            else if constexpr (detail::is_one_of_variants_types<storage_type, T>)
                return arg.getAsString();
            else
                static_assert(detail::always_false<T>::value, "non-exhaustive visitor!");
            }, _val);
    }

    bool isDefault () const {
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (detail::is_one_of_variants_types<storage_type, T>)
                return arg.getValue() == arg.getDefaultValue();
            else
                static_assert(detail::always_false<T>::value, "non-exhaustive visitor!");
        }, _val);
    }


    void resetToDefault () {
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (detail::is_one_of_variants_types<storage_type, T>) {
                if (arg.getDefaultValue() != arg.getValue()) {
                    arg.setValue(arg.getDefaultValue());
                    arg.onUpdate();
                }
            }
            else
                static_assert(detail::always_false<T>::value,
                              "non-exhaustive visitor!");
        }, _val);
    }

    template<typename T>
    typename T::value_type* getIncomingPtr () {
        return std::get<T>(_val).getIncomingPtr();
    }

    // TODO4: make non-template?
    template<typename T>
    void setFromIncomingPtr () {
        auto p = std::get<T>(_val).getRawIncomingPtr();
        if (*p != get<T>()) {
            set<T>(*p);
        }
    }

    // TODO: add unittest for it
    void updateFromPtr () {
        return std::visit([&](auto&& arg) {
                              using T = std::decay_t<decltype(arg)>;
            if constexpr (detail::is_one_of_variants_types<storage_type, T>) {
                if (arg.getValue() != *arg.getUpdatePtr()
                    && arg.updateFromPtr()) {
                    arg.onUpdate();
                }
            }
            else
                static_assert(detail::always_false<T>::value,
                              "non-exhaustive visitor!");
        }, _val);
    }

    template<typename T>
    const typename T::constraint_type& getConstraint () const {
        return std::get<T>(_val).getConstraint();
    }

    template<typename T>
    void addUpdateHandler (void *pListenerId,
                           std::function<void(const typename T::value_type&)> pFunction) {
        std::get<T>(_val).addUpdateHandler(pListenerId, pFunction);
    }

    // TODO2: come up with better name and add remove() counterpart
    void addSimpleUpdateHandler (void *pListenerId,
                                 std::function<void()> pFunction) {
        return std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (detail::is_one_of_variants_types<storage_type, T>) {
                    arg.addUpdateHandler(
                        pListenerId,
                        [pFunction] (const typename T::value_type&) {
                            pFunction();
                        });

            }
            else
                static_assert(detail::always_false<T>::value,
                              "non-exhaustive visitor!");
                          }, _val);
    }


    template<typename T>
    void removeUpdateHandler (void *pListenerId) {
        std::get<T>(_val).removeUpdateHandler(pListenerId);
    }

    // TODO no type check here and next??
    template<typename T>
    int getEnumPos () const {
        return getConstraint<T>().getPosInt(get<T>());
    }

    template<typename T>
    const std::vector<typename T::value_type>& getEnumElements() const {
        return getConstraint<T>()._validValues;
    }


protected:
    storage_type _val;
};




class SettingsException: public std::exception {
public:
    enum class Type { ALREADY_EXISTS, NOT_FOUND };
    explicit SettingsException (const std::string& message, const Type pType):
        _msg(message),
        _type(pType) {
    }

    virtual ~SettingsException() throw (){}

    Type getType () {
        return _type;
    }

    virtual const char* what() const throw (){
       return _msg.c_str();
    }

protected:
    std::string _msg;
    Type _type;
};

template<typename SettingT>
class SettingsImpl {
public:
    // TODO do we need this?
    using setting_type = SettingT;

    using SettingsMap = std::map<std::string, SettingT>;
    using SettingsPtrsMap = std::map<std::string, SettingT*>;
    using SettingsCategories = std::map<std::string, SettingsPtrsMap>;

    class SettingsSectionView {
    public:
        class iterator {
        public:
            explicit iterator (const typename SettingsPtrsMap::iterator &pIt)
                : _current (pIt) {
            }
            iterator operator++() {
                ++_current;
                return *this;
            }
            bool operator!=(const iterator& pOther) const {
                return _current != pOther._current;
            }
            const typename SettingsPtrsMap::value_type& operator*() const {
                return *(_current);
            }
        private:
            typename SettingsPtrsMap::iterator _current;
        };

        iterator begin () { return iterator(_settingsPtrsMap.begin()); }
        iterator end () { return iterator(_settingsPtrsMap.end()); }

        SettingsSectionView (const std::string& pName,
                             SettingsPtrsMap& pSettingsPtrsMap)
            : _name (pName),
              _settingsPtrsMap (pSettingsPtrsMap) {
        }

        const std::string& getName() const {
            return _name;
        }

    private:

        const std::string& _name;
        SettingsPtrsMap& _settingsPtrsMap;
    };

    class iterator {
    public:
        iterator (const typename SettingsCategories::iterator &pIt)
            : _current (pIt) {
        }
        iterator operator++() {
            ++_current;
            return *this;
        }
        bool operator!=(const iterator& pOther) const {
            return _current != pOther._current;
        }
        SettingsSectionView operator*() const {
            return SettingsSectionView(_current->first, _current->second);
        }
    private:
        typename SettingsCategories::iterator _current;
    };

    iterator begin () { return iterator(_categories.begin()); }
    iterator end () { return iterator(_categories.end()); }

    SettingsSectionView getSection (const std::string &pSectionName) {
        return SettingsSectionView(pSectionName, _categories[pSectionName]);
    }


    SettingsImpl (const std::string &pFilename = "")
        : _filename (pFilename) {
    }

    void load () {
        using VcppBits::KeyFileSectionsIterator;
        using VcppBits::KeyFileSettingsIterator;
        if (!_filename.size()) {
            return;
        }
        try {
            // TODO10: migrate to range-based for
            KeyFile f (_filename);
            KeyFileSectionsIterator sec_iter = f.getSectionsIterator();
            while (sec_iter.isElement()) {
                KeyFileSettingsIterator set_iter = sec_iter.getSettingsIterator();
                const std::string sec_name = sec_iter.getName();
                const std::string prefix =
                    sec_name.empty() ? "" : sec_name + ".";
                while (set_iter.isElement()) {
                    if (_values.count(prefix + set_iter.getName())) {
                        try {
                            _values.at(prefix + set_iter.getName())
                                .setByString(set_iter.getValue());
                        }
                        catch (const SettingsException& oor) {
                            (void) oor;
                        }
                    }
                    set_iter.peekNext();
                }
                sec_iter.peekNext();
            }
        }
        catch (const KeyFile::file_not_found&) {
        }
    }

    void resetAll() {
        typename SettingsMap::iterator
            it = _values.begin(),
            end = _values.end();
        for (;it != end; ++it) {
            (*it).second.resetToDefault();
        }
    }

    void reloadFromFile () {
        load();
    }

    void loadFromFile (const std::string &pName) {
        _filename = pName;
        load();
    }

    ~SettingsImpl () {
        writeFile();
    }

    void writeFile () {
        if (!_filename.size()) {
            return;
        }
        KeyFile file;
        typename SettingsMap::const_iterator
            it = _values.begin(),
            end = _values.end();
        for (;it != end; ++it) {
            file.appendKey((*it).first, (*it).second.getAsString());
        }
        file.writeToFile(_filename);
    }


    const SettingT& getSetting (const std::string& pName) const {
        if (_valuesMap.count(pName) < 1) {
            throw SettingsException("Setting " + pName + " not found",
                                    SettingsException::Type::NOT_FOUND);
        }

        return *_valuesMap.at(pName);
    }

    SettingT& getSetting (const std::string& pName) {
        if (_valuesMap.count(pName) < 1) {
            throw SettingsException("Setting " + pName + " not found",
                                    SettingsException::Type::NOT_FOUND);
        }

        return *_valuesMap.at(pName);
    }

    bool hasSetting (const std::string& pName) const {
        return _valuesMap.count(pName);
    }

    template <typename... Args>
    SettingT& appendSetting (const std::string &pName,
                             Args... pArgs) {
        if (_valuesMap.count(pName) > 0) {
            throw SettingsException(
                "Setting " + pName + " already exists",
                SettingsException::Type::ALREADY_EXISTS);
        }

        auto res = _values.emplace(pName, pArgs...);
        // TODO use it?
        (void) res;

        SettingT& inserted_set = _values.at(pName);
        _valuesMap.insert(typename SettingsPtrsMap::value_type(pName,
                                                               &inserted_set));

        size_t n = pName.find('.', 0);
        std::string categ_name =
            n == std::string::npos ? "" : pName.substr(0, n);

        _categories[categ_name][pName] = &inserted_set;

        return inserted_set;

    }

    void setFilename (const std::string& pName) {
        _filename = pName;
    }

    template<typename T>
    const typename T::value_type& get (const std::string& pName) const {
        return getSetting(pName).template get<T>();
    }

    template<typename T>
    void set (const std::string& pName,
              const typename T::value_type &pNewVal) {
        return getSetting(pName).template set<T>(pNewVal);
    }

    template<typename T>
    void triggerListeners (const std::string& pName) {
        return getSetting(pName).template triggerListeners<T>();
    }

private:

    SettingsMap _values;
    SettingsPtrsMap _valuesMap;
    SettingsCategories _categories;
    std::string _filename;
};

// template <typename SettingT, typename T>
// typename T::value_type &_get (SettingsImpl<SettingT> &pSettings,
//                               const std::string& pName) {
//     return pSettings.getSetting(pName)::template.get<T>();
// }


inline std::string bool_to_string (const bool pBoolValue) {
    return VcppBits::StringUtils::toString(pBoolValue);
}

inline std::string string_to_string (const std::string pString) {
    // one of the most beautiful pieces of code in my life
    return pString;
}

template <typename T>
inline T from_string (const std::string &pStr) {
    std::stringstream ss(pStr);
    T ret_val;
    ss >> ret_val;
    return ret_val;
}


using BoolValue = SettingValue<bool,
                               NoneConstraint,
                               bool_to_string,
                               from_string<bool>>;
using IntValue = SettingValue<int,
                              ArithmeticConstraint,
                              std::to_string,
                              from_string<int>>;
using FloatValue = SettingValue<float,
                                ArithmeticConstraint,
                                std::to_string,
                                from_string<float>>;
using EnumIntValue = SettingValue<int,
                                  EnumConstraint,
                                  std::to_string,
                                  from_string<int>>;
using EnumFloatValue = SettingValue<float,
                                    EnumConstraint,
                                    std::to_string,
                                    from_string<float>>;
using StringValue = SettingValue<std::string,
                                 NoneConstraint,
                                 string_to_string,
                                 from_string<std::string>>;
using EnumStringValue = SettingValue<std::string,
                                     EnumConstraint,
                                     string_to_string,
                                     from_string<std::string>>;

namespace SettingsDefault {
enum class SettingTypeEnum : std::size_t { BOOL,
                                           INT,
                                           FLOAT,
                                           ENUM_INT,
                                           ENUM_FLOAT,
                                           STRING,
                                           ENUM_STRING };

using Setting = SettingImpl<SettingTypeEnum,
                            BoolValue,
                            IntValue,
                            FloatValue,
                            EnumIntValue,
                            EnumFloatValue,
                            StringValue,
                            EnumStringValue>;

using Settings = SettingsImpl<Setting>;
} // namespace SettingsDefault

namespace SettingsUtils {

template <typename T, typename SettingsT>
typename SettingsT::setting_type&
createArithmetic (SettingsT& pSettings,
                  const std::string& pSettingName,
                  const typename T::value_type pMin,
                  const typename T::value_type pMax,
                  const typename T::value_type pDefault,
                  typename T::value_type* pValueToKeepUpdated = nullptr) {
    auto &set = pSettings.appendSetting(
        pSettingName, T(pDefault, ArithmeticConstraint(pMin, pMax)));
    if (pValueToKeepUpdated) {
        set.template addUpdateHandler<T>(
            &pSettings,
            [pValueToKeepUpdated] (const typename T::value_type& pNewVal) {
                *pValueToKeepUpdated = pNewVal;
            });
        *pValueToKeepUpdated = pDefault;
    }

    return set;
}

template <typename T, typename SettingsT>
typename SettingsT::setting_type&
createEnum (SettingsT& pSettings,
            const std::string& pSettingName,
            const std::vector<typename T::value_type> pValues,
            const typename T::value_type pDefault,
            typename T::value_type* pValueToKeepUpdated = nullptr,
            int* pIntValueToKeepUpdated = nullptr) {
    auto &set = pSettings.appendSetting(
        pSettingName, T(pDefault, EnumConstraint(pValues)));
    set.template addUpdateHandler<T>(
        &pSettings,
        [pValueToKeepUpdated, &set, pIntValueToKeepUpdated] (const typename T::value_type& pNewVal) {
            if (pValueToKeepUpdated) {
                *pValueToKeepUpdated = pNewVal;
            }
            if (pIntValueToKeepUpdated) {
                *pIntValueToKeepUpdated = set.template getEnumPos<T>();
            }
        });
    if (pValueToKeepUpdated) {
        *pValueToKeepUpdated = pDefault;
    }
    if (pIntValueToKeepUpdated) {
        *pIntValueToKeepUpdated = set.template getEnumPos<T>();
    }
    return set;
}

template <typename T, typename SettingsT>
typename SettingsT::setting_type&
create (SettingsT& pSettings,
        const std::string& pSettingName,
        typename T::value_type pDefault,
        typename T::value_type* pValueToKeepUpdated = nullptr) {
    auto &set = pSettings.appendSetting(pSettingName, T(pDefault));

    if (pValueToKeepUpdated) {
        set.template addUpdateHandler<T>(
            &pSettings,
            [pValueToKeepUpdated] (const typename T::value_type& pNewVal) {
                *pValueToKeepUpdated = pNewVal;
            });
        *pValueToKeepUpdated = pDefault;
    }

    return set;
}

} // namespace SettingsUtils

} // namespace V2
