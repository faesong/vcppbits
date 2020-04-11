#include <iostream>
#include <variant>
#include <map>
#include <vector>

#include "VcppBits/StringUtils/StringUtils.hpp"
#include "VcppBits/KeyFile/KeyFile.hpp"

namespace V2 {

using VcppBits::KeyFile;

template<typename T>
struct NoneConstraint {
    bool isValid (const T) const { return true; };
};


// TODO3: rename, and note that it only works for fundamental type settings
// (the ones that are covered by std::numeric_limits)
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

template<typename T>
struct EnumConstraint {
    using value_type = T;
    explicit EnumConstraint (const std::vector<T> &pValues)
        : _validValues (pValues) {
    }

    template <typename TT>
    bool
    isValid(const TT pValueToTest, typename std::enable_if<!std::is_floating_point<TT>::value>::type* dummy = nullptr) const {
        return std::find(_validValues.begin(), _validValues.end(), pValueToTest)
            != _validValues.end();
    }

    template <typename TT>
    bool
    isValid (const TT pValueToTest, typename std::enable_if<std::is_floating_point<TT>::value>::type* dummy = nullptr) const {
        return std::find_if(_validValues.begin(),
                         _validValues.end(),
                         [&pValueToTest] (const TT pVal) {
                             return std::abs(pVal - pValueToTest)
                                 < std::numeric_limits<T>::epsilon();
                         })
            != _validValues.end();
    }

    std::vector<value_type> _validValues;
};


template<typename T,
         template <typename> class CT,
         std::string _toString(T),
         T _fromString(const std::string&)>
struct SettingValue {
    using value_type = T;
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
    void setByString (const std::string pStr) {
        setValue(_fromString(pStr));
    }

    void resetToDefault () {
        _val = _defaultVal;
    }

    const value_type& getValue () const {
        return _val;
    }

    bool setValue (const value_type pValue) {
        if (_constraint.isValid(pValue)) {
            _val = pValue;
            return true;
        }
        return false;
    }

    const CT<T>& getConstraint () const {
        return _constraint;
    }

private:
    value_type _val;
    const value_type _defaultVal;
    CT<T> _constraint;
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
    }

    SettingImpl (SettingImpl& pOther) = delete;
    SettingImpl (SettingImpl&& pOther) = delete;
    SettingImpl operator= (SettingImpl& pOther) = delete;
    SettingImpl operator= (SettingImpl&& pOther) = delete;

    EnumT getType() {
        return EnumT(_val.index());
    }

    template <typename T>
    typename std::enable_if<detail::is_one_of_variants_types<storage_type, T>, T>::type&
    get () {
        return std::get<T>(_val);
    }

    template<typename T>
    const typename T::value_type& getValue () const {
        return std::get<T>(_val).getValue();
    }

    void setByString (const std::string &pStr) {
        return std::visit([&pStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            // TODO: do we need this first if block?
            if constexpr (std::is_same_v<typename T::value_type, std::string>)
                arg.setValue(pStr);
            else if constexpr (detail::is_one_of_variants_types<storage_type, T>)
                return arg.setByString(pStr);
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

    void resetToDefault () {
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (detail::is_one_of_variants_types<storage_type, T>)
                return arg.resetToDefault();
            else
                static_assert(detail::always_false<T>::value,
                              "non-exhaustive visitor!");
        }, _val);
    }

protected:
    storage_type _val;
};




class SettingsException: public std::exception {
public:
    enum class Type { ALREADY_EXISTS, NOT_FOUND };
    explicit SettingsException (const std::string& message, const Type pType):
        _msg(message){}

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


template<typename Setting>
class SettingsImpl {
public:
    using SettingsMap = std::map<std::string, Setting>;
    using SettingsPtrsMap = std::map<std::string, Setting*>;
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
            Setting& operator*() const {
                return *(_current->second);
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


    SettingsImpl (const std::string &pFilename/* = "" */)
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
        SettingsMap::iterator
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
        SettingsMap::const_iterator
            it = _values.begin(),
            end = _values.end();
        for (;it != end; ++it) {
            file.appendKey((*it).first, (*it).second.getAsString());
        }
        file.writeToFile(_filename);
    }


    const Setting& getSetting (const std::string& pName) const {
        if (_valuesMap.count(pName) < 1) {
            throw SettingsException("Setting " + pName + " not found",
                                    SettingsException::Type::NOT_FOUND);
        }

        return *_valuesMap.at(pName);
    }

    Setting& getSetting (const std::string& pName) {
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
    Setting& appendSetting (const std::string &pName,
                            Args... pArgs) {
        if (_valuesMap.count(pName) > 0) {
            throw SettingsException(
                "Setting " + pName + " already exists",
                SettingsException::Type::ALREADY_EXISTS);
        }

        auto res = _values.emplace(pName, pArgs...);

        Setting& inserted_set = _values.at(pName);
        _valuesMap.insert(SettingsPtrsMap::value_type(pName,
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
    const typename T::value_type& getValue (const std::string& pName) const {
        return getSetting(pName).getValue<T>();
    }
private:

    SettingsMap _values;
    SettingsPtrsMap _valuesMap;
    SettingsCategories _categories;
    std::string _filename;
};


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

} // namespace V2
